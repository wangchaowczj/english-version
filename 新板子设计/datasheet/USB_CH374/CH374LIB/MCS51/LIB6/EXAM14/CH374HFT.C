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

/* MCS-51单片机C语言的U盘文件读写示例程序, 适用于89C52或者更大程序空间的单片机,也适用于ATMEL/PHILIPS/SST等具有1KB内部RAM的单片机 */
/* 本程序用于演示CH374U内置的3端口根集线器Root-HUB,
   任一端口插入U盘则参考EXAM7以字节模式读写文件,目录文件名为MY_ADC.TXT,如果RAM多于1K字节,也可改成扇区读写,
   如果插入设备不是U盘,则初始化枚举该USB设备,支持全速USB设备或者低速USB设备,例如USB键盘等,3个HUB端口中只能插入一个U盘
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

//#define DISK_BASE_BUF_LEN		2048	/* 默认的磁盘数据缓冲区大小为512字节,建议选择为2048甚至4096以支持某些大扇区的U盘,为0则禁止在.H文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在.H文件中定义缓冲区,而由应用程序在调用CH375Init之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

#define NO_DEFAULT_CH374_F_ENUM		1		/* 未调用CH374FileEnumer程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_F_QUERY	1		/* 未调用CH374FileQuery程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_RESET		1		/* 未调用CH374Reset程序故禁止以节约代码 */

#include "..\CH374HF6.H"				/* 如果不需要支持FAT32,那么请选用CH374HF6.H */

/* 常用USB结构和相关常量 */

typedef struct _USB_SETUP_REQ {
	UINT8	bType;
	UINT8	bReq;
	UINT8	wValueL;
	UINT8	wValueH;
	UINT8	wIndexL;
	UINT8	wIndexH;
	UINT8	wLengthL;
	UINT8	wLengthH;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

typedef struct _USB_DEVICE_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdUSBL;
    UINT8 bcdUSBH;
    UINT8 bDeviceClass;
    UINT8 bDeviceSubClass;
    UINT8 bDeviceProtocol;
    UINT8 bMaxPacketSize0;
    UINT8 idVendorL;
    UINT8 idVendorH;
    UINT8 idProductL;
    UINT8 idProductH;
    UINT8 bcdDeviceL;
    UINT8 bcdDeviceH;
    UINT8 iManufacturer;
    UINT8 iProduct;
    UINT8 iSerialNumber;
    UINT8 bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

typedef struct _USB_CONFIG_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 wTotalLengthL;
    UINT8 wTotalLengthH;
    UINT8 bNumInterfaces;
    UINT8 bConfigurationValue;
    UINT8 iConfiguration;
    UINT8 bmAttributes;
    UINT8 MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

typedef struct _USB_INTERF_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bInterfaceNumber;
    UINT8 bAlternateSetting;
    UINT8 bNumEndpoints;
    UINT8 bInterfaceClass;
    UINT8 bInterfaceSubClass;
    UINT8 bInterfaceProtocol;
    UINT8 iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bEndpointAddress;
    UINT8 bmAttributes;
    UINT8 wMaxPacketSize;
    UINT8 wMaxPacketSize1;
    UINT8 bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

typedef struct _USB_CONFIG_DESCRIPTOR_LONG {
	USB_CFG_DESCR	cfg_descr;
	USB_ITF_DESCR	itf_descr;
	USB_ENDP_DESCR	endp_descr[4];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

#define		TRUE	1
#define		FALSE	0

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

/* 以下代码用于枚举非U盘设备 */

// 获取设备描述符
const	UINT8	code	SetupGetDevDescr[] = { 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00 };
// 获取配置描述符
const	UINT8	code	SetupGetCfgDescr[] = { 0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x04, 0x00 };
// 设置USB地址
const	UINT8	code	SetupSetUsbAddr[] = { 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 设置USB配置
const	UINT8	code	SetupSetUsbConfig[] = { 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

UINT8	UsbDevEndpSize = DEFAULT_ENDP0_SIZE;	/* USB设备的端点0的最大包尺寸 */

//USB设备相关信息表，CH374U最多支持3个设备
#define		ROOT_DEV_DISCONNECT		0
#define		ROOT_DEV_CONNECTED		1
#define		ROOT_DEV_FAILED			2
#define		ROOT_DEV_SUCCESS		3
struct _RootHubDev{
	UINT8	DeviceStatus;			// 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
	UINT8	DeviceAddress;			// 设备被分配的USB地址
	UINT8	DeviceSpeed;			// 0为低速,非0为全速
	UINT8	DeviceType;				// 设备类型
//	union {
//		struct MOUSE {
//			UINT8	MouseInterruptEndp;		// 鼠标中断端点号
//			UINT8	MouseIntEndpTog;		// 鼠标中断端点的同步标志
//			UINT8	MouseIntEndpSize;		// 鼠标中断端点的长度
//		}
//		struct PRINT {
//		}
//	}
//.....    struct  _Endp_Attr   Endp_Attr[4];	//端点的属性,最多支持4个端点
} xdata RootHubDev[3];

UINT8	xdata	CtrlBuf[8];
UINT8	xdata	TempBuf[64];

void	HostEnableRootHub( void );  // 启用内置的Root-HUB

UINT8	HostCtrlTransfer374( PUINT8 ReqBuf, PUINT8 DatBuf, PUINT8 RetLen );  // 执行控制传输,ReqBuf指向8字节请求码,DatBuf为收发缓冲区
// 如果需要接收和发送数据，那么DatBuf需指向有效缓冲区用于存放后续数据，实际成功收发的总长度保存在ReqLen指向的字节变量中

void	SetHostUsbAddr( UINT8 addr );  // 设置USB主机当前操作的USB设备地址

UINT8	GetDeviceDescr( PUINT8 buf );  // 获取设备描述符

UINT8	GetConfigDescr( PUINT8 buf );  // 获取配置描述符

UINT8	SetUsbAddress( UINT8 addr );  // 设置USB设备地址

UINT8	SetUsbConfig( UINT8 cfg );  // 设置USB设备配置

void	DisableRootHubPort( UINT8 index );  // 关闭指定的ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态

void	ResetRootHubPort( UINT8 index );  // 检测到设备后,复位相应端口的总线,为枚举设备准备,设置为默认为全速

UINT8	EnableRootHubPort( UINT8 index );  // 使能ROOT-HUB端口,相应的BIT_HUB?_EN置1开启端口,返回FALSE设置失败(可能是设备断开了)

void	SetUsbSpeed( UINT8 FullSpeed );  // 设置当前USB速度

void	SelectHubPort( UINT8 index );  // 选择操作指定的ROOT-HUB端口

UINT8	InitDevUsbDisk( void );  // 基于CH374LIB初始化U盘

UINT8	InitDevice( UINT8 index );  // 初始化/枚举指定ROOT-HUB端口的USB设备

UINT8	SearchHubPort( UINT8 type );  // 搜索指定类型的设备所在的端口号,输出端口号为0xFF则未搜索到

UINT8	WriteToDiskFile( void );  // 向U盘文件追加写入数据,该子程序来自EXAM7主程序


void	HostEnableRootHub( void )  // 启用内置的Root-HUB
{
//	CH374_WRITE_REGISTER( REG_USB_SETUP, M_SET_USB_BUS_FREE( CH374_READ_REGISTER( REG_USB_SETUP ) ) );  // USB总线空闲
	CH374_WRITE_REGISTER( REG_USB_SETUP, BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF );  // USB主机方式,允许SOF
	CH374_WRITE_REGISTER( REG_HUB_SETUP, 0x00 );  // 清BIT_HUB_DISABLE,允许内置的ROOT-HUB
	CH374_WRITE_REGISTER( REG_HUB_CTRL, 0x00 );  // 清除ROOT-HUB信息
}

UINT8	HostCtrlTransfer374( PUINT8 ReqBuf, PUINT8 DatBuf, PUINT8 RetLen )  // 执行控制传输,ReqBuf指向8字节请求码,DatBuf为收发缓冲区
// 如果需要接收和发送数据，那么DatBuf需指向有效缓冲区用于存放后续数据，实际成功收发的总长度保存在ReqLen指向的字节变量中
{
	UINT8	s;
	for ( s = 0; s != 8; s ++ ) CH374_WRITE_REGISTER( RAM_HOST_TRAN + s, *( ReqBuf + s ) );  // 预先写入请求码
	CH374DirtyBuffer( );  // 因调用CH374CtrlTransfer而使用了pDISK_BASE_BUF,如果内存多也可不用pDISK_BASE_BUF,但需参考HOST.C另写控制传输子程序
	s = CH374CtrlTransfer( );  // 控制传输
	if ( s == ERR_SUCCESS ) {
		*RetLen = *(ReqBuf+6);  // 请求长度
		memcpy( DatBuf, pDISK_BASE_BUF, *RetLen );  // 返回数据
		return( USB_INT_SUCCESS );
	}
	return( s );
}

void	SetHostUsbAddr( UINT8 addr )  // 设置USB主机当前操作的USB设备地址
{
	CH374_WRITE_REGISTER( REG_USB_ADDR, addr );
}

UINT8	GetDeviceDescr( PUINT8 buf )  // 获取设备描述符
{
	UINT8	s, len;
	UsbDevEndpSize = DEFAULT_ENDP0_SIZE;
	s = HostCtrlTransfer374( SetupGetDevDescr, buf, &len );  // 执行控制传输
	if ( s == USB_INT_SUCCESS ) {
		UsbDevEndpSize = ( (PUSB_DEV_DESCR)buf ) -> bMaxPacketSize0;  // 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndpSize再继续
		if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL ) s = USB_INT_BUF_OVER;  // 描述符长度错误
	}
	return( s );
}

UINT8	GetConfigDescr( PUINT8 buf )  // 获取配置描述符
{
	UINT8	s, len;
	s = HostCtrlTransfer374( SetupGetCfgDescr, buf, &len );  // 执行控制传输
	if ( s == USB_INT_SUCCESS ) {
		if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL ) s = USB_INT_BUF_OVER;  // 返回长度错误
		else {
			len = ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL;
			memcpy ( CtrlBuf, SetupGetCfgDescr, sizeof( SetupGetCfgDescr ) );
			( (PUSB_SETUP_REQ)CtrlBuf ) -> wLengthL = len;  // 完整配置描述符的总长度
			s = HostCtrlTransfer374( CtrlBuf, buf, &len );  // 执行控制传输
			if ( s == USB_INT_SUCCESS ) {
				if ( len < ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL ) s = USB_INT_BUF_OVER;  // 描述符长度错误
			}
		}
	}
	return( s );
}

UINT8	SetUsbAddress( UINT8 addr )  // 设置USB设备地址
{
	UINT8	s;
	memcpy ( CtrlBuf, SetupSetUsbAddr, sizeof( SetupSetUsbAddr ) );
	( (PUSB_SETUP_REQ)CtrlBuf ) -> wValueL = addr;  // USB设备地址
	s = HostCtrlTransfer374( CtrlBuf, NULL, NULL );  // 执行控制传输
	if ( s == USB_INT_SUCCESS ) {
		SetHostUsbAddr( addr );  // 设置USB主机当前操作的USB设备地址
	}
	CH374DelaymS( 10 );  // 等待USB设备完成操作
	return( s );
}

UINT8	SetUsbConfig( UINT8 cfg )  // 设置USB设备配置
{
	memcpy ( CtrlBuf, SetupSetUsbConfig, sizeof( SetupSetUsbConfig ) );
	( (PUSB_SETUP_REQ)CtrlBuf ) -> wValueL = cfg;  // USB设备配置
	return( HostCtrlTransfer374( CtrlBuf, NULL, NULL ) );  // 执行控制传输
}

void	DisableRootHubPort( UINT8 index )  // 关闭指定的ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
{
	RootHubDev[ index ].DeviceStatus = ROOT_DEV_DISCONNECT;
	RootHubDev[ index ].DeviceAddress = 0x00;
	if ( index == 1 ) CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL)&0xF0 );  // 清除有关HUB1的控制数据,实际上不需要清除
	else if ( index == 2 ) CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL)&0x0F );  // 清除有关HUB2的控制数据,实际上不需要清除
	else CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER(REG_HUB_SETUP)&0xF0 );  // 清除有关HUB0的控制数据,实际上不需要清除
//	printf( "HUB %01x close\n",(UINT16)index );
}

void	ResetRootHubPort( UINT8 index )  // 检测到设备后,复位相应端口的总线,为枚举设备准备,设置为默认为全速
{
	UsbDevEndpSize = DEFAULT_ENDP0_SIZE;  /* USB设备的端点0的最大包尺寸 */
	SetHostUsbAddr( 0x00 );
	CH374_WRITE_REGISTER( REG_USB_H_CTRL, 0x00 );
	if ( index == 1 ) {
		CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) & ~ BIT_HUB1_POLAR | BIT_HUB1_RESET );  // 默认为全速,开始复位
		CH374DelaymS( 15 );																 // 复位时间10mS到20mS
		CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) & ~ BIT_HUB1_RESET );  // 结束复位
	}
	else if ( index == 2 ) {
		CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) & ~ BIT_HUB2_POLAR | BIT_HUB2_RESET );  // 默认为全速,开始复位
		CH374DelaymS( 15 );																 // 复位时间10mS到20mS
		CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) & ~ BIT_HUB2_RESET );  // 结束复位
	}
	else {
		CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER(REG_HUB_SETUP) & ~ BIT_HUB0_POLAR | BIT_HUB0_RESET );  // 默认为全速,开始复位
		CH374DelaymS( 15 );																 // 复位时间10mS到20mS
		CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER(REG_HUB_SETUP) & ~ BIT_HUB0_RESET );  // 结束复位
	}
	CH374_WRITE_REGISTER( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_USB_SUSPEND );  // 清中断标志
}

UINT8	EnableRootHubPort( UINT8 index )  // 使能ROOT-HUB端口,相应的BIT_HUB?_EN置1开启端口,返回FALSE设置失败(可能是设备断开了)
{
	if ( RootHubDev[ index ].DeviceStatus < ROOT_DEV_CONNECTED ) RootHubDev[ index ].DeviceStatus = ROOT_DEV_CONNECTED;
	if ( index == 1 ) {
		if ( CH374_READ_REGISTER(REG_HUB_CTRL)&BIT_HUB1_ATTACH ) {  // 有设备
			if ( !(CH374_READ_REGISTER(REG_HUB_CTRL)&BIT_HUB1_EN) ) {  // 尚未使能
				if ( !(CH374_READ_REGISTER(REG_HUB_SETUP) & BIT_HUB1_DX_IN) ) CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) ^ BIT_HUB1_POLAR );  // 如果速度不匹配则设置极性
				RootHubDev[1].DeviceSpeed= !( CH374_READ_REGISTER(REG_HUB_CTRL) & BIT_HUB1_POLAR );
			}
			CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL)|BIT_HUB1_EN );	//使能HUB端口
			return( TRUE );
		}
	}
	else if ( index == 2 ) {
		if ( CH374_READ_REGISTER(REG_HUB_CTRL)&BIT_HUB2_ATTACH ) {  // 有设备
			if ( !(CH374_READ_REGISTER(REG_HUB_CTRL)&BIT_HUB2_EN) ) {  // 尚未使能
				if ( !(CH374_READ_REGISTER(REG_HUB_SETUP) & BIT_HUB2_DX_IN) ) CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL) ^ BIT_HUB2_POLAR );  // 如果速度不匹配则设置极性
				RootHubDev[2].DeviceSpeed= !( CH374_READ_REGISTER(REG_HUB_CTRL) & BIT_HUB2_POLAR );
			}
			CH374_WRITE_REGISTER( REG_HUB_CTRL, CH374_READ_REGISTER(REG_HUB_CTRL)|BIT_HUB2_EN );	//使能HUB端口
			return( TRUE );
		}
	}
	else {
		if ( CH374_READ_REGISTER(REG_HUB_SETUP)&BIT_HUB0_ATTACH ) {  // 有设备
			if ( !(CH374_READ_REGISTER(REG_HUB_SETUP)&BIT_HUB0_EN) ) {  // 尚未使能
				if ( !(CH374_READ_REGISTER(REG_INTER_FLAG) & BIT_HUB0_DX_IN) ) CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER(REG_HUB_SETUP) ^ BIT_HUB0_POLAR );  // 如果速度不匹配则设置极性
				RootHubDev[0].DeviceSpeed= !( CH374_READ_REGISTER(REG_HUB_SETUP) & BIT_HUB0_POLAR );
			}
			CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER(REG_HUB_SETUP)|BIT_HUB0_EN );  //使能HUB端口
			return( TRUE );
		}
	}
	return( FALSE );
}

void	SetUsbSpeed( UINT8 FullSpeed )  // 设置当前USB速度
{
	if ( FullSpeed ) {  // 全速
		CH374_WRITE_REGISTER( REG_USB_SETUP, CH374_READ_REGISTER( REG_USB_SETUP ) & BIT_SETP_RAM_MODE | BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF );  // 全速
		CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER( REG_HUB_SETUP ) & ~ BIT_HUB_PRE_PID );  // 禁止PRE PID
	}
	else CH374_WRITE_REGISTER( REG_USB_SETUP, CH374_READ_REGISTER( REG_USB_SETUP ) & BIT_SETP_RAM_MODE | BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF | BIT_SETP_LOW_SPEED );  // 低速
}

void	SelectHubPort( UINT8 index )  // 选择操作指定的ROOT-HUB端口
{
	SetHostUsbAddr( RootHubDev[index].DeviceAddress );  // 设置USB主机当前操作的USB设备地址
//	if ( RootHubDev[index].DeviceSpeed == FALSE && 该设备是连接在外部HUB某端口 )  // 通过外部HUB与低速USB设备通讯需要前置ID
//		CH374_WRITE_REGISTER( REG_HUB_SETUP, CH374_READ_REGISTER( REG_HUB_SETUP ) | BIT_HUB_PRE_PID );  // 启用PRE PID
	SetUsbSpeed( RootHubDev[index].DeviceSpeed );  // 设置当前USB速度
	CH374vEmbHubIndex = index + 1;  // U盘所在的端口号,通知U盘子程序库
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

UINT8	InitDevUsbDisk( void )  // 基于CH374LIB初始化U盘
{
/* 检查U盘是否准备好,有些U盘不需要这一步,但是某些U盘必须要执行这一步才能工作 */
	UINT8	i;
	for ( i = 0; i < 5; i ++ ) {  /* 有的U盘总是返回未准备好,不过可以被忽略 */
		printf( "DiskReady ?\n" );
		if ( CH374DiskReady( ) == ERR_SUCCESS ) break;  /* 查询磁盘是否准备好 */
		if ( CH374DiskStatus >= DISK_MOUNTED && i > 1 ) break;  /* 有的U盘总是返回未准备好,不过可以被忽略 */
		CH374DelaymS( 100 );
	}
	if ( CH374DiskStatus >= DISK_MOUNTED ) return( ERR_SUCCESS );  // 操作成功,磁盘已经初始化并安装到系统中
	return( ERR_DISK_DISCON );  // 不支持或者不是U盘
}

UINT8	InitDevice( UINT8 index )  // 初始化/枚举指定ROOT-HUB端口的USB设备
// 输入: 内置HUB端口号0/1/2
// 输出: 0-操作失败, 0x31-成功枚举到USB键盘, 0x32-成功枚举到鼠标, 0x70-成功枚举到打印机, 0x80-成功枚举到U盘, 0xFF-成功枚举未知设备, 其它值暂未定义
#define	DEV_ERROR		0x00
#define	DEV_KEYBOARD	0x31
#define	DEV_MOUSE		0x32
#define	DEV_PRINT		0x70
#define	DEV_DISK		0x80
#define	DEV_HUB			0x90
#define	DEV_UNKNOWN		0xFF
{
	UINT8	i, s, cfg, dv_cls, if_cls;
	printf( "Start reset HUB%01d port\n", (UINT16)index );
	ResetRootHubPort( index );  // 检测到设备后,复位相应端口的USB总线
	for ( i = 0, s = 0; i < 100; i ++ ) {  // 等待USB设备复位后重新连接
		if ( EnableRootHubPort( index ) ) {  // 使能ROOT-HUB端口
			i = 0;
			s ++;  // 计时等待USB设备连接后稳定
			if ( s > 100 ) break;  // 已经稳定连接
		}
		CH374DelaymS( 1 );
	}
	if ( i ) {  // 复位后设备没有连接
		DisableRootHubPort( index );
		printf( "Disable HUB%01d port because of disconnect\n", (UINT16)index );
		return( DEV_ERROR );
	}
	SetUsbSpeed( RootHubDev[index].DeviceSpeed );  // 设置当前USB速度
	printf( "GetDeviceDescr @HUB%1d: ", (UINT16)index );
	s = GetDeviceDescr( TempBuf );  // 获取设备描述符
	if ( s == USB_INT_SUCCESS ) {
		for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ ) printf( "0x%02X ", (UINT16)( TempBuf[i] ) );
		printf( "\n" ); // 显示出描述符
		dv_cls = ( (PUSB_DEV_DESCR)TempBuf ) -> bDeviceClass;  // 设备类代码
		s = SetUsbAddress( index + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );  // 设置USB设备地址,加上index可以保证三个HUB端口分配不同的地址
		if ( s == USB_INT_SUCCESS ) {
			RootHubDev[index].DeviceAddress = index + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;  // 保存USB地址
			printf("GetConfigDescr:\n" );
			s = GetConfigDescr( TempBuf );  // 获取配置描述符
			if ( s == USB_INT_SUCCESS ) {
				cfg = ((PUSB_CFG_DESCR)TempBuf ) -> bConfigurationValue;
				for ( i = 0; i < ( (PUSB_CFG_DESCR)TempBuf ) -> wTotalLengthL; i ++ ) printf( "0x%02X ", (UINT16)( TempBuf[i] ) );
				printf("\n");
/* 分析配置描述符，获取端点数据/各端点地址/各端点大小等，更新变量endp_addr和endp_size等 */
				if_cls = ( (PUSB_CFG_DESCR_LONG)TempBuf ) -> itf_descr.bInterfaceClass;  // 接口类代码
				if ( dv_cls == 0x00 && if_cls == 0x08 ) {  // 是USB存储类设备,基本上确认是U盘
					if ( CH374DiskStatus >= DISK_MOUNTED ) printf( "Too many USB-Disk, discard previous\n" );
					CH374DiskStatus = DISK_DISCONNECT;  // 清除磁盘状态以便重新初始化(否则可以是接着另一端口的U盘的状态)
					s = InitDevUsbDisk( );  // 基于CH374LIB初始化U盘
					if ( s == ERR_SUCCESS ) {  // 操作成功,磁盘已经初始化并安装到系统中
						RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
//						SetUsbSpeed( TRUE );  // 默认为全速
						printf( "USB-Disk Ready\n" );
						return( DEV_DISK );  /* U盘初始化成功 */
					}
				}
				else if ( dv_cls == 0x00 && if_cls == 0x07 && ( (PUSB_CFG_DESCR_LONG)TempBuf ) -> itf_descr.bInterfaceSubClass == 0x01 ) {  // 是打印机类设备
					s = SetUsbConfig( cfg );  // 设置USB设备配置
					if ( s == USB_INT_SUCCESS ) {
//						需保存端点信息以便主程序进行USB传输
						RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
						SetUsbSpeed( TRUE );  // 默认为全速
						printf( "USB-Print Ready\n" );
						return( DEV_PRINT );  /* 打印机初始化成功 */
					}
				}
				else if ( dv_cls == 0x00 && if_cls == 0x03 && ( (PUSB_CFG_DESCR_LONG)TempBuf ) -> itf_descr.bInterfaceSubClass <= 0x01 ) {  // 是HID类设备,键盘/鼠标等
					s = SetUsbConfig( cfg );  // 设置USB设备配置
					if ( s == USB_INT_SUCCESS ) {
//						需保存端点信息以便主程序进行USB传输
						RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
						SetUsbSpeed( TRUE );  // 默认为全速
						s = ( (PUSB_CFG_DESCR_LONG)TempBuf ) -> itf_descr.bInterfaceProtocol;
						if ( s == 1 ) {
//							进一步初始化,例如设备键盘指示灯LED等
							printf( "USB-Keyboard Ready\n" );
							return( DEV_KEYBOARD );  /* 键盘初始化成功 */
						}
						else if ( s == 2 ) {
//							为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
							printf( "USB-Mouse Ready\n" );
							return( DEV_MOUSE );  /* 鼠标初始化成功 */
						}
					}
				}
				else if ( dv_cls == 0x09 ) {  // 是HUB类设备,集线器等
// 实际上,CH374LIB支持内置HUB的U盘,此处的HUB也可能是单个内置HUB的U盘,所以,试试调用CH374DiskReady试一下,不是U盘则当成HUB用
					if ( CH374DiskStatus <= DISK_CONNECT ) {  // 目前没有U盘连接,不如试一下
						s = InitDevUsbDisk( );  // 基于CH374LIB初始化U盘
						if ( s == ERR_SUCCESS ) {  // 操作成功,磁盘已经初始化并安装到系统中
							RootHubDev[index].DeviceAddress =CH374_READ_REGISTER( REG_USB_ADDR );  // 保存HUB端口的U盘的地址(而不是HUB地址)
							RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
							SetUsbSpeed( TRUE );  // 默认为全速
							printf( "USB-Disk (embed with HUB) Ready\n" );
							return( DEV_DISK );  /* U盘初始化成功 */
						}
						CH374DiskStatus = DISK_DISCONNECT;  // 没有U盘连接
						s = SetUsbAddress( RootHubDev[index].DeviceAddress );  // 重新设置USB设备地址
						if ( s == USB_INT_SUCCESS ) s = GetConfigDescr( TempBuf );  // 重新获取配置描述符
						if ( s != USB_INT_SUCCESS ) printf( "Hub re-address or re-config error %02x\n", (UINT16)s );
					}
					s = SetUsbConfig( cfg );  // 设置USB设备配置
					if ( s == USB_INT_SUCCESS ) {  // 参考EVT\EMB_HUB\ROOTHUB.C程序
//						需保存端点信息以便主程序进行USB传输
//						给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
						RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
						SetUsbSpeed( TRUE );  // 默认为全速
						printf( "USB-Hub Ready\n" );  // 参考EVT\EMB_HUB\ROOTHUB.C处理外部二级HUB枚举
						return( DEV_HUB );  /* HUB初始化成功 */
					}
				}
				else {   // 可以进一步分析
					s = SetUsbConfig( cfg );  // 设置USB设备配置
					if ( s == USB_INT_SUCCESS ) {
//						需保存端点信息以便主程序进行USB传输
						RootHubDev[index].DeviceStatus =ROOT_DEV_SUCCESS;
						SetUsbSpeed( TRUE );  // 默认为全速
						return( DEV_UNKNOWN );  /* 未知设备初始化成功 */
					}
				}
			}
		}
	}
	printf( "InitDevice Error = %02X\n", (UINT16)s );
	RootHubDev[index].DeviceStatus =ROOT_DEV_FAILED;
	SetUsbSpeed( TRUE );  // 默认为全速
	return( DEV_ERROR );
}

UINT8	SearchHubPort( UINT8 type )  // 搜索指定类型的设备所在的端口号,输出端口号为0xFF则未搜索到
{ // 当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
	UINT8	i;
	for ( i = 0; i < 3; i ++ ) {  // 现时搜索可以避免设备中途拔出而某些信息未及时更新的问题
		if ( RootHubDev[i].DeviceType == type && RootHubDev[i].DeviceStatus >= ROOT_DEV_SUCCESS ) return( i );  // 类型匹配且枚举成功
	}
	return( 0xFF );
}

UINT8	WriteToDiskFile( void )  // 向U盘文件追加写入数据,该子程序来自EXAM7主程序
{
	UINT8	i, month, hour;
	UINT16	date, adc, len;
	LED_RUN_ACT( );  /* 开始操作U盘 */
// 调用前已经判断过U盘就绪
//	i = CH374DiskConnect( );  /* 检查USB设备是否连接并更新磁盘状态,返回成功说明连接 */
//	if ( i != ERR_SUCCESS ) return( i );  /* U盘断开了 */
//	i = CH374DiskReady( );  /* 查询磁盘是否准备好 */
//	if ( i == ERR_DISK_DISCON ) return( i );
//	if ( i != ERR_SUCCESS ) return( i );  /* U盘未就绪,有的U盘永远返回失败而实际上已经就绪,所以只执行而不判断结果 */

/* 如果MY_ADC.TXT文件已经存在则添加数据到尾部,如果不存在则新建文件 */
	printf( "Open\n" );
	strcpy( mCmdParam.Open.mPathName, "/MY_ADC.TXT" );  /* 文件名,该文件在根目录下 */
	i = CH374FileOpen( );  /* 打开文件 */
	if ( i == ERR_SUCCESS ) {  /* 文件存在并且已经被打开,移动文件指针到尾部以便添加数据 */
		printf( "File size = %ld\n", CH374vFileSize );  /* 在成功打开文件后,全局变量CH374vFileSize中是文件当前长度 */
		printf( "Locate tail\n" );
		mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  /* 移到文件的尾部 */
		i = CH374ByteLocate( );
		if ( i != ERR_SUCCESS ) return( i );
	}
	else if ( i == ERR_MISS_FILE ) {  /* 没有找到文件,必须新建文件 */
		LED_WR_ACT( );  /* 写操作 */
		printf( "Create\n" );
/*		strcpy( mCmdParam.Create.mPathName, "/MY_ADC.TXT" );*/  /* 文件名,该文件在根目录下,刚才已经提供给CH374FileOpen */
		i = CH374FileCreate( );  /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
		if ( i != ERR_SUCCESS ) return( i );
	}
	else if ( i != ERR_SUCCESS ) return( i );  /* 打开文件时出错 */
	LED_WR_ACT( );  /* 写操作 */
	printf( "Write begin\n" );
	i = sprintf( mCmdParam.ByteWrite.mByteBuffer, "此前文件长度= %ld 字节\xd\xa", CH374vFileSize );
	mCmdParam.ByteWrite.mByteCount = i;  /* 指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO */
	i = CH374ByteWrite( );  /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS ) return( i );
/* 默认情况下,如果字节数mCmdParam.ByteWrite.mByteCount不为0那么CH374ByteWrite只负责写入数据而不修改文件长度,
   如果长时间不写入数据则应该更新文件长度,防止突然断电后前面写入的数据与文件长度不相符,
   如果需要写完数据后立即修改/更新文件长度,那么可以置字节数mCmdParam.ByteWrite.mByteCount为0后调用CH374ByteWrite强行更新文件长度,
   如果确定不会突然断电或者后面很快有数据不断写入则不必更新文件长度,可以提高速度并减少U盘损耗(U盘内部的内存寿命有限,不宜频繁改写) */
	printf( "Write ADC data\n" );
	for ( hour = 8; hour != 20; hour ++  ) {  /* 用循环方式添加12行数据 */
		TR0=1;  /* 用定时器0的计数值代替ADC数据 */
		month = 5;  /* 假定是5月 */
		date = TL1 & 0x1F;  /* 因为测试板上没有实时时钟芯片,所以用定时器1的计数代替进行演示 */
/*		adc = get_adc_data( ); */
		adc = ( (UINT16)TH0 << 8 ) | TL0;  /* 因为测试板上没有ADC,所以用定时器0的计数代替ADC数据演示 */
		len = sprintf( mCmdParam.ByteWrite.mByteBuffer, "%02d.%02d.%02d ADC=%u\xd\xa", (UINT16)month, date, (UINT16)hour, adc );  /* 将二制制数据格式为一行字符串 */
		mCmdParam.ByteWrite.mByteCount = (unsigned char)len;  /* 指定本次写入的字节数,不能超过MAX_BYTE_IO,否则另用缓冲区分多次写入 */
		i = CH374ByteWrite( );  /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
		if ( i != ERR_SUCCESS ) return( i );
	}
/*	mCmdParam.ByteWrite.mByteCount = 0;  如果指定写入0字节,则用于刷新文件的长度
	CH374ByteWrite( );  以字节为单位向文件写入数据,因为是0字节写入,所以只用于更新文件的长度,当阶段性写入数据后,可以用这种办法更新文件长度 */
	printf( "Write end\n" );
	strcpy( mCmdParam.ByteWrite.mByteBuffer, "今天的ADC数据到此结束\xd\xa" );
	i = strlen( mCmdParam.ByteWrite.mByteBuffer );
	mCmdParam.ByteWrite.mByteCount = i;  /* 指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO */
	i = CH374ByteWrite( );  /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS ) return( i );
/*	printf( "Modify\n" );  如果实际产品中有实时时钟,可以根据需要将文件的日期和时间修改为实际值
	mCmdParam.Modify.mFileAttr = 0xff;   输入参数: 新的文件属性,为0FFH则不修改
	mCmdParam.Modify.mFileTime = MAKE_FILE_TIME( 16, 32, 09 );   输入参数: 新的文件时间: 16:32:09
	mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2004, 5, 18 );  输入参数: 新的文件日期: 2004.05.18
	mCmdParam.Modify.mFileSize = 0xffffffff;   输入参数: 新的文件长度,以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改
	i = CH374FileModify( );   修改当前文件的信息,修改日期
	if ( i != ERR_SUCCESS ) return( i );
*/
	printf( "Close\n" );
	mCmdParam.Close.mUpdateLen = 1;  /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
	i = CH374FileClose( );  /* 关闭文件 */
	if ( i != ERR_SUCCESS ) return( i );
	LED_WR_INACT( );
	LED_RUN_INACT( );
}

main( ) {
	UINT8	n, s;
	UINT8X	DiskPortNum;
	UINT16X	DiskTimer, MouseTimer;
	LED_OUT_INIT( );
	LED_OUT_ACT( );  /* 开机后LED亮一下以示工作 */
	CH374DelaymS( 100 );  /* 延时100毫秒 */
	LED_OUT_INACT( );
	mInitSTDIO( );  /* 为了让计算机通过串口监控演示过程 */
	printf( "Start\n" );

	for ( n = 0; n < 3; n ++ ) RootHubDev[n].DeviceStatus = ROOT_DEV_DISCONNECT;  // 清空
	DiskTimer = 0;
	MouseTimer = 0;

	s = CH374LibInit( );  /* 初始化CH374程序库和CH374芯片,操作成功返回0 */
	mStopIfError( s );
	HostEnableRootHub( );  // 启用内置的Root-HUB

/* 其它电路初始化 */
	while ( 1 ) {  // 主程序循环
		CH374DelaymS( 10 );  /* 没必要频繁查询,可做其它事,每隔数百mS甚至几秒再来查询一次新设备 */
		for ( n = 0; n < 3; n ++ ) {  // 逐个检查三个ROOT-HUB端口
			CH374vEmbHubIndex = n + 1;  // CH374LIB使用1/2/3分别代表0/1/2三个HUB端口
			s = CH374EmbHubAttach( );  // 检查当前的内置HUB端口是否有USB设备
			if ( s == 0 ) {  // 设备断开
				RootHubDev[n].DeviceStatus = ROOT_DEV_DISCONNECT;
				if ( RootHubDev[n].DeviceType == DEV_DISK ) CH374DiskStatus = DISK_DISCONNECT;  // 没有U盘连接
			}
			else if ( s == BIT_HUB0_ATTACH	// 有USB设备但尚未启用
				|| s == ( BIT_HUB0_ATTACH & BIT_HUB0_EN ) && RootHubDev[n].DeviceStatus <= ROOT_DEV_CONNECTED ) {   // 之前无设备或者刚插入设备尚未初始化
				LED_OUT_ACT( );
				RootHubDev[n].DeviceStatus = ROOT_DEV_CONNECTED;
				CH374DelaymS( 200 );  // 等待USB设备上电稳定
				s = InitDevice( n );  // 初始化/枚举指定HUB端口的USB设备
				RootHubDev[n].DeviceType = s;  // 保存设备类型
				if ( s == DEV_DISK ) {  // U盘枚举成功
					DiskPortNum = n;  // 保存U盘所在的端口号
					printf( "Erase\n" );
					SelectHubPort( n );  // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
					strcpy( mCmdParam.Erase.mPathName, "/MY_ADC.TXT" );  /* 文件名,该文件在根目录下 */
					s = CH374FileErase( );  /* 删除文件 */
					if ( s != ERR_SUCCESS && s != ERR_MISS_FILE ) {  /* 删除文件失败并且文件存在 */
						printf( "Erase file error %02X\n", (UINT16)s );
					}
					SetUsbSpeed( TRUE );  // 默认为全速,U盘不会是低速,此操作多此一举
				}
				else if ( s == DEV_HUB ) {  // HUB枚举成功,参考EVT\EMB_HUB\ROOTHUB.C程序
//					做点什么?  给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
				}
				LED_OUT_INACT( );
			}
			else {};  // 设备一直存在,且已经初始化
		}
		CH374DelaymS( 10 );  /* 没必要频繁查询,可做其它事,每隔数百mS甚至几秒再来查询一次新设备 */
		DiskTimer ++;
		if ( DiskTimer >= 1000 ) {  // 假定出现某个客观条件要求写U盘,此处用计数到某个值约20mS*1000即20秒来代表达到某一条件
			while ( 1 ) {
				n = SearchHubPort( DEV_DISK );  // 搜索U盘所在的端口号,事先没有记忆U盘所在端口则可以搜索
				if ( n == DiskPortNum ) break;  // 确实是最后一次U盘枚举的HUB端口号(文件系统库保存了最后一次U盘的状态)
				if ( n == 0xFF ) break;  // 没找到,设备断开了
				RootHubDev[n].DeviceStatus = ROOT_DEV_FAILED;  // 不是最后一次U盘枚举的HUB端口号,则该端口的U盘需要重新分析文件系统
			}
			if ( n == DiskPortNum ) {  // 找到最后一次U盘枚举的HUB端口号
				printf( "WriteFile to USB-Disk\n" );
				SelectHubPort( n );  // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
				s = CH374DiskReady( );  /* 查询磁盘是否准备好 */
				if ( s != ERR_DISK_DISCON ) {  /* 因为有的U盘永远返回失败而实际上已经就绪,所以只判断U盘是否断开 */
					s = WriteToDiskFile( );  // 向U盘文件追加写入数据,该子程序来自EXAM7主程序
					if ( s != ERR_SUCCESS ) {
						printf( "WriteFile error code = %02X\n", (UINT16)s );
					}
				}
				SetUsbSpeed( TRUE );  // 默认为全速,U盘不会是低速,此操作多此一举
				DiskTimer = 0;  // 写U盘后清条件
			}
		}
// 处理键盘/鼠标/打印机等事务,以下演示MOUSE
		MouseTimer ++;
		if ( MouseTimer >= 500 ) {  // 假定需要每隔20mS*10即200mS查询一下鼠标的状态,例子是5秒
			n = SearchHubPort( DEV_MOUSE );  // 搜索MOUSE所在的端口号,输出端口号为0xFF则未搜索到
			if ( n != 0xFF ) {
				printf( "Query Mouse\n" );
				SelectHubPort( n );  // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
				s = GetDeviceDescr( TempBuf );  // 获取设备描述符
				if ( s == USB_INT_SUCCESS ) {
					for ( s = 0; s < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; s ++ ) printf( "0x%02X ", (UINT16)( TempBuf[s] ) );
					printf( "\n" ); // 显示出描述符
				}
//				CH374vUsbPidIn = TRUE;  /* IN */
//				CH374_WRITE_REGISTER( REG_USB_H_PID, M_MK_HOST_PID_ENDP( DEF_USB_PID_IN, RootHubDev[n].MOUSE.MouseInterruptEndp ) );  /* 指定令牌PID和目的端点号 */
//				CH374_WRITE_REGISTER( REG_USB_H_CTRL, RootHubDev[n].MOUSE.MouseIntEndpTog | BIT_HOST_START );  /* 设置同步标志并启动传输 */
//				CH374vRetryCount &= 0x7F;  /* 临时禁止NAK自动重试,因为鼠标没有数据时返回NAK,我们只是查询而不傻等,有数据就取出,没有也不重试傻等 */
//				s = CH374HostTransact( );  /* 传输事务 */
//				CH374vRetryCount |= 0x80;  /* 恢复NAK自动重试 */
//				if ( s == ERR_SUCCESS ) {  /* 成功 */
//					CH374_READ_BLOCK( RAM_HOST_RECV, CH374_READ_REGISTER( REG_USB_LENGTH ), TempBuf );  /* 取出数据 */
//				}
				SetUsbSpeed( TRUE );  // 默认为全速
				MouseTimer = 0;  // 查询鼠标后清条件
			}
		}
// 如果支持多级外部HUB级联,那么此处定期查询HUB各端口状态,以检测外部HUB各端口上的设备插拔事件
	}
}
