/* 2004.06.05
****************************************
**  Copyright  (C)  W.ch  1999-2004   **
**  Web:  http://www.winchiphead.com  **
****************************************
**  USB Host File Interface for CH374 **
**  TC2.0@PC, KC7.0@MCS51             **
****************************************
*/
/* CH374 主机文件系统接口 */
/* 支持: FAT12/FAT16/FAT32 */

/* MCS-51单片机C语言的U盘文件读写示例程序, 适用于89C52或者更大程序空间的单片机 */
/* 本程序用于演示CH374U内置的3端口根集线器Root-HUB,
   任一端口先插入U盘作为源盘,任一端口后插入U盘作为目的盘,本程序将源盘的任意文件或者*.TXT文件复制到目的盘中
*/
/* CH374的INT#引脚采用查询方式处理, 数据复制方式为"单DPTR复制", 所以速度较慢, 适用于所有MCS51单片机 */


/* C51   CH374HFT.C */
/* LX51  CH374HFT.OBJ , CH374HF6.LIB, C51DPTR1.LIB */
/* OHX51 CH374HFT */

#include <reg52.h>
#include <stdio.h>
#include <string.h>

#define	MAX_BYTE_IO				35		/* 以字节为单位单次读写文件时的最大长度,默认值是29,值大则占用内存多,值小则超过该长度必须分多次读写 */

/* 以下定义的详细说明请看CH374HF6.H文件 */
#define LIB_CFG_INT_EN			0		/* CH374的INT#引脚连接方式,0为"查询方式",1为"中断方式" */

#define CH374_IDX_PORT_ADDR		0xBDF1	/* CH374索引端口的I/O地址 */
#define CH374_DAT_PORT_ADDR		0xBCF0	/* CH374数据端口的I/O地址 */
//#define CH374_IDX_PORT_ADDR		0xEFFF	/* CH374索引端口的I/O地址 */
//#define CH374_DAT_PORT_ADDR		0xE000	/* CH374数据端口的I/O地址 */
/* 只使用单片机内置的1KB外部RAM: 0000H-01FFH 为磁盘读写缓冲区, 以字节为单位读写文件不需要文件数据读写缓冲区FILE_DATA_BUF */
#define	DISK_BASE_BUF_ADDR		0x0000	/* 外部RAM的磁盘数据缓冲区的起始地址,从该单元开始的缓冲区长度为SECTOR_SIZE */

#define CH374_INT_WIRE			INT0	/* P3.2, INT0, CH374的中断线INT#引脚,连接CH374的INT#引脚,用于查询中断状态 */
/* 如果未连接CH374的中断引脚,那么应该去掉上述定义,自动使用寄存器查询方式 */

#define DISK_BASE_BUF_LEN		512		/* 默认的磁盘数据缓冲区大小为512字节,建议选择为2048甚至4096以支持某些大扇区的U盘,为0则禁止在.H文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在.H文件中定义缓冲区,而由应用程序在调用CH375Init之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

#define NO_DEFAULT_CH374_F_ENUM		1		/* 未调用CH374FileEnumer程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_F_QUERY	1		/* 未调用CH374FileQuery程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_RESET		1		/* 未调用CH374Reset程序故禁止以节约代码 */

#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */

#include "..\CH374HF6.H"				/* 如果不需要支持FAT32,那么请选用CH374HF6.H */

UINT8X	FileName[0x20];			// 正在复制的文件名
UINT8X	LibSaveBuf[3][0x80];	// 用于保存子程序库变量的缓冲区
UINT8X	my_buffer[0x7000];		// 复制时的中转缓冲区
UINT16	FileIndex, CountMatch;

/* 在P1.4连接一个LED用于监控演示程序的进度,低电平LED亮,当U盘插入后亮 */
sbit P1_4  = P1^4;
#define LED_OUT_INIT( )		{ P1_4 = 1; }	/* P1.4 高电平 */
#define LED_OUT_ACT( )		{ P1_4 = 0; }	/* P1.4 低电平驱动LED显示 */
#define LED_OUT_INACT( )	{ P1_4 = 1; }	/* P1.4 低电平驱动LED显示 */
sbit P1_5  = P1^5;
/* 在P1.5连接一个LED用于监控演示程序的进度,低电平LED亮,当对U盘操作时亮 */
#define LED_RUN_ACT( )		{ P1_5 = 0; }	/* P1.5 低电平驱动LED显示 */
#define LED_RUN_INACT( )	{ P1_5 = 1; }	/* P1.5 低电平驱动LED显示 */
sbit P1_6  = P1^6;
/* 在P1.6连接一个LED用于监控演示程序的进度,低电平LED亮,当对U盘写操作时亮 */
#define LED_WR_ACT( )		{ P1_6 = 0; }	/* P1.6 低电平驱动LED显示 */
#define LED_WR_INACT( )		{ P1_6 = 1; }	/* P1.6 低电平驱动LED显示 */

void	HostEnableRootHub( void );  // 启用内置的Root-HUB

void	RestoreDisk( UINT8 index );  // 恢复指定U盘的变量

void	SaveDisk( UINT8 index );  // 备份指定U盘的变量

void	HostEnableRootHub( void )  // 启用内置的Root-HUB
{
	CH374_WRITE_REGISTER( REG_USB_SETUP, BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF );  // USB主机方式,允许SOF
	CH374_WRITE_REGISTER( REG_HUB_SETUP, 0x00 );  // 清BIT_HUB_DISABLE,允许内置的ROOT-HUB
	CH374_WRITE_REGISTER( REG_HUB_CTRL, 0x00 );  // 清除ROOT-HUB信息
}

void	RestoreDisk( UINT8 index )  // 恢复指定U盘的变量
{
	mCmdParam.SaveVariable.mSaveVariable = 0x80;  // 恢复多个U盘的变量
	mCmdParam.SaveVariable.mBuffer = LibSaveBuf[ index ];  // 缓冲区
	CH374SaveVariable( );	/* 恢复子程序库的变量,用于子程序库在多个CH374芯片之间或者多个U盘之间进行切换 */
	CH374DirtyBuffer( );	/* 清除磁盘缓冲区 */
}

void	SaveDisk( UINT8 index )  // 备份指定U盘的变量
{
	mCmdParam.SaveVariable.mSaveVariable = 1;  // 备份变量
	mCmdParam.SaveVariable.mBuffer = LibSaveBuf[ index ];  // 缓冲区
	CH374SaveVariable( );	/* 备份子程序库的变量,用于子程序库在多个CH374芯片之间或者多个U盘之间进行切换 */
}

void xFileNameEnumer( void )			/* 文件名枚举回调子程序,参考CH374HF6.H文件中的例子 */
{  /* 每搜索到一个文件FileOpen都会调用本回调程序，xFileNameEnumer返回后，FileOpen递减CH374vFileSize并继续枚举直到搜索不到文件或者目录 */
	UINT8			i, j, c;
	P_FAT_DIR_INFO	pFileDir;
	pFileDir = (P_FAT_DIR_INFO)( pDISK_BASE_BUF + CH374vFdtOffset );  /* 当前FDT的起始地址 */
	if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) return;  /* 是子目录名 */
	if ( pFileDir -> DIR_Name[0] == '.' ) return;  /* 是本级或者上级目录名,必须丢弃不处理 */
#ifndef	COPY_ANY_FILE  /* 此处决定复制那些文件,可以演示只复制*.TXT文件 */
	if ( pFileDir -> DIR_Name[8] != 'T' || pFileDir -> DIR_Name[9] != 'X' || pFileDir -> DIR_Name[10] != 'T' ) return;  /* 过滤文件的扩展名,不是".TXT"文件则不处理 */
#endif
	if ( CountMatch ++ < FileIndex ) return;  /* 指定序号未到 */
	for ( i = 0, j = 1; i < 11; i ++ ) {  /* 复制文件名,长度为11个字符 */
		c = pFileDir -> DIR_Name[ i ];
		if ( i == 0 && c == 0x05 ) c = 0xE5;  /* 转换特殊字符 */
		if ( c != 0x20 ) {  /* 是有效字符 */
			if ( i == 8 ) FileName[j++] = '.';  /* 处理扩展名,添加分隔符 */
			FileName[j++] = c;  /* 复制文件名的一个字符 */
		}
	}
	FileName[j] = 0;  /* 当前文件名完整路径的结束符 */
	FileName[0] = '/';  /* 根目录下的文件 */
	CH374vFileSize = 1;  /* 强行提前结束枚举,本次FileOpen将不再回调xFileNameEnumer而是提前返回 */
}

/* 检查操作状态,如果错误则显示错误代码并停机 */
void	mStopIfError( UINT8 iError )
{
	if ( iError == ERR_SUCCESS ) return;  /* 操作成功 */
	printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
/* 遇到错误后,应该分析错误码以及CH374DiskStatus状态,例如调用CH374DiskConnect查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
   建议出错后的处理步骤:
   1、调用一次CH374DiskReady,成功则继续操作,例如Open,Read/Write等,在CH374DiskReady中会自动调用CH374DiskConnect，不必另外调用
   2、如果CH374DiskReady不成功,那么强行将CH374DiskStatus置为DISK_CONNECT状态,然后从头开始操作(等待U盘连接CH374DiskConnect，CH374DiskReady等) */
	while ( 1 ) {
		LED_OUT_ACT( );  /* LED闪烁 */
		CH374DelaymS( 100 );
		LED_OUT_INACT( );
		CH374DelaymS( 100 );
	}
}

/* 为printf和getkey输入输出初始化串口 */
void	mInitSTDIO( )
{
	SCON = 0x50;
	PCON = 0x80;
//	TL2 = RCAP2L = 0 - 10; /* 18.432MHz晶振, 57600bps */
	TL2 = RCAP2L = 0 - 13; /* 24MHz晶振, 57600bps */
	TH2 = RCAP2H = 0xFF;
	T2CON = 0x34;  /* 定时器2用于串口的波特率发生器 */
	TI = 1;
}

main( ) {
	UINT8	n, s;
	UINT8	SourceDisk, TargetDisk;
	LED_OUT_INIT( );
	LED_OUT_ACT( );  /* 开机后LED亮一下以示工作 */
	CH374DelaymS( 100 );  /* 延时100毫秒 */
	LED_OUT_INACT( );
	mInitSTDIO( );  /* 为了让计算机通过串口监控演示过程 */

	printf( "Start\n" );

	s = CH374LibInit( );  /* 初始化CH374程序库和CH374芯片,操作成功返回0 */
	mStopIfError( s );
	HostEnableRootHub( );  // 启用内置的Root-HUB

	for ( n = 0; n < 3; n ++ ) {  // 清除针对3个端口的U盘文件库状态,每个U盘各一个状态
		SaveDisk( n );  // 备份子程序库的变量,用于子程序库在多个CH374芯片之间或者多个U盘之间进行切换
	}
	SourceDisk = 0xFF;
	TargetDisk = 0xFF;

/* 其它电路初始化 */
	while ( 1 ) {  // 主程序循环
		CH374DelaymS( 10 );  /* 没必要频繁查询,可做其它事,每隔数百mS甚至几秒再来查询一次新设备 */
		for ( n = 0; n < 3; n ++ ) {  // 逐个检查三个ROOT-HUB端口
			RestoreDisk( n );  // 恢复指定U盘的变量
			CH374vEmbHubIndex = n + 1;  // CH374LIB使用1/2/3分别代表0/1/2三个HUB端口
			if ( CH374DiskConnect( ) == ERR_SUCCESS ) {  /* 查询方式: 检查USB设备是否连接并更新磁盘状态,返回成功说明连接 */
				if ( CH374EmbHubAttach( ) == BIT_HUB0_ATTACH	// 检查当前的内置HUB端口,有USB设备但尚未启用
					|| CH374DiskStatus <= DISK_CONNECT ) {   // 之前无设备或者刚插入设备尚未初始化
					LED_OUT_ACT( );
					CH374DelaymS( 200 );  // 等待USB设备上电稳定
/* 检查U盘是否准备好,有些U盘不需要这一步,但是某些U盘必须要执行这一步才能工作 */
					CH374DiskStatus = DISK_CONNECT;  // 清除磁盘状态以便重新初始化(否则可以是接着另一端口的U盘的状态)
					for ( s = 0; s < 5; s ++ ) {  /* 有的U盘总是返回未准备好,不过可以被忽略 */
						CH374DelaymS( 100 );
						printf( "DiskReady ?\n" );
						if ( CH374DiskReady( ) == ERR_SUCCESS ) break;  /* 查询磁盘是否准备好 */
					}
					if ( CH374DiskStatus >= DISK_MOUNTED ) {  // 操作成功,磁盘已经初始化并安装到系统中
						printf( "USB-Disk Ready\n" );
						if ( SourceDisk == 0xFF || SourceDisk == n ) {
							SourceDisk = n;  // 之前没有U盘或者U盘重新插拔
							TargetDisk = 0xFF;  // 先有源盘,再有目的盘
							FileIndex = 0;  // 搜索指定文件的序号
						}
						else if ( TargetDisk == 0xFF || TargetDisk == n ) TargetDisk = n;
					}
					else CH374DiskStatus = 0xFF;  // 不支持的USB设备
					LED_OUT_INACT( );
				}
				else {};  // U盘一直存在,且已经初始化,或者不是U盘设备
			}
			else {  // U盘断开
				if ( n == SourceDisk ) SourceDisk = 0xFF;
				if ( n == TargetDisk ) TargetDisk = 0xFF;
			}
			SaveDisk( n );  // 备份指定U盘的变量
		}
		CH374DelaymS( 10 );  /* 没必要频繁查询,可做其它事,每隔数百mS甚至几秒再来查询一次新设备 */
		if ( SourceDisk != 0xFF && TargetDisk != 0xFF && SourceDisk != TargetDisk ) {  // 有两个U盘
			LED_RUN_ACT( );
			while ( 1 ) {
				RestoreDisk( SourceDisk );  // 恢复源盘的变量
				if ( CH374DiskStatus < DISK_OPEN_FILE ) {  // 文件尚未打开
					printf( "Open source\n" );
					strcpy( mCmdParam.Open.mPathName, "\\*\xFF\x00" );  /* 文件名,0xFF用于指示说明搜索序号在CH374vFileSize变量中,0x00仅用于结束strcpy */
					CH374vFileSize = 0xFFFFFFFF;  /* 快速连续枚举,每找到一个文件调用一次xFileNameEnumer回调子程序,如果值小于0x80000000则每次只枚举一个文件太慢 */
					CountMatch = 0;  // 计数搜索到的匹配文件数,以与指定序号比较
					s = CH374FileOpen( );  /* 枚举,由回调程序xFileNameEnumer产生记录保存到结构中,如果文件名中含有通配符*,则为搜索文件而不打开 */
					if ( s == ERR_SUCCESS ) s = ERR_FOUND_NAME;  // 转换状态码,避免冲突
					if ( ( s == ERR_FOUND_NAME || s == ERR_MISS_FILE ) && CountMatch > FileIndex ) {  // 找到指定序号的文件
						strcpy( mCmdParam.Open.mPathName, FileName );  // 文件名
						s = CH374FileOpen( );  /* 找到文件后正式打开文件 */
					}
					if ( s != ERR_SUCCESS ) {  // 操作结束或者操作失败则清盘
						if ( s == ERR_MISS_FILE ) printf( "Copy completion, %d files\n", FileIndex );  // 复制完成
						else printf( "FileOpen error %02x\n", (UINT16)s );
						SaveDisk( SourceDisk );  // 备份源盘的变量
						SourceDisk = 0xFF;
						break;
					}
					printf( "Open source [%s] ok\n", FileName );
				}
				mCmdParam.ReadX.mSectorCount = sizeof(my_buffer)/DISK_BASE_BUF_LEN;  // 扇区数
				mCmdParam.ReadX.mDataBuffer = &my_buffer[0];  /* 指向文件数据缓冲区的起始地址 */
				CH374vFileSize += CH374vSectorSize - 1;  /* 默认情况下,以扇区方式读取数据时,无法读出文件尾部不足1个扇区的部分,所以必须临时加大文件长度以读取尾部零头 */
				s = CH374FileReadX( );  /* 从文件读取数据 */
				CH374vFileSize -= CH374vSectorSize - 1;  /* 恢复原文件长度 */
				if ( s != ERR_SUCCESS ) {  // 操作失败则清盘
					printf( "FileReadX error %02x\n", (UINT16)s );
					SaveDisk( SourceDisk );  // 备份源盘的变量
					SourceDisk = 0xFF;
					break;
				}
				n = mCmdParam.ReadX.mSectorCount;  // 实际读取扇区数
				if ( n == 0 ) {  // 源文件结束
					memset( & mCmdParam.Modify, 0xFF, sizeof( mCmdParam.Modify ) );
					s = CH374FileModify( );  // 查询当前文件的信息
					memcpy( my_buffer, & mCmdParam.Modify, sizeof( mCmdParam.Modify ) );  // 保存文件信息
					if ( s == ERR_SUCCESS ) s = CH374FileClose( );  // 关闭文件
					if ( s != ERR_SUCCESS ) {  // 操作失败则清盘
						printf( "FileModify or FileClose error %02x\n", (UINT16)s );
						SaveDisk( SourceDisk );  // 备份源盘的变量
						SourceDisk = 0xFF;
						break;
					}
				}
				SaveDisk( SourceDisk );  // 备份源盘的变量
				LED_WR_ACT( );
				RestoreDisk( TargetDisk );  // 恢复目的盘的变量
				if ( CH374DiskStatus < DISK_OPEN_FILE ) {  // 文件尚未打开
					printf( "Create target\n" );
					strcpy( mCmdParam.Open.mPathName, FileName );  // 文件名
					s = CH374FileCreate( );  /* 新建文件 */
					if ( s != ERR_SUCCESS ) {  // 操作失败则清盘
						printf( "FileCreate error %02x\n", (UINT16)s );
						SaveDisk( TargetDisk );  // 备份目的盘的变量
						TargetDisk = 0xFF;
						break;
					}
					printf( "Create target ok\n" );
				}
				if ( n ) {  // 复制数据
					mCmdParam.WriteX.mSectorCount = n;  // 扇区数
					mCmdParam.WriteX.mDataBuffer = &my_buffer[0];  /* 指向文件数据缓冲区的起始地址 */
					s = CH374FileWriteX( );  /* 向文件写入数据 */
					if ( s != ERR_SUCCESS || mCmdParam.WriteX.mSectorCount != n ) {  // 操作失败则清盘
						if ( s == ERR_SUCCESS ) printf( "Write sector count error\n" );
						else printf( "FileWriteX error %02x\n", (UINT16)s );
						SaveDisk( TargetDisk );  // 备份目的盘的变量
						TargetDisk = 0xFF;
						break;
					}
					printf( "." );  // 进度指示
				}
				else {  // 文件结束
					memcpy( & mCmdParam.Modify, my_buffer, sizeof( mCmdParam.Modify ) );  // 文件信息
					s = CH374FileModify( );  // 设置当前文件的信息
					mCmdParam.Close.mUpdateLen = 0;  /* 禁止更新长度 */
					if ( s == ERR_SUCCESS ) s = CH374FileClose( );  // 关闭文件
					if ( s != ERR_SUCCESS ) {  // 操作失败则清盘
						printf( "FileModify or FileClose error %02x\n", (UINT16)s );
						SaveDisk( TargetDisk );  // 备份目的盘的变量
						TargetDisk = 0xFF;
						break;
					}
					printf( "Close target ok\n" );
					FileIndex ++;  // 搜索并复制下一个文件
				}
				SaveDisk( TargetDisk );  // 备份目的盘的变量
				LED_WR_INACT( );
			}
			LED_RUN_INACT( );
		}
	}
}
