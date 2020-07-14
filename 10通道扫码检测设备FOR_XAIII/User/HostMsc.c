#include "includes.h"

static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */ 
static  FATFS fs;
//文件复制
//将psrc文件,copy到pdst.
//psrc,pdst:源文件和目标文件
//fwmode:文件写入模式
//0:不覆盖原有的文件
//1:覆盖原有的文件
u8 FileCopy(u8 *psrc,u8 *pdst)
{
	u8 res;
	u16 br=0;
	u16 bw=0;
	DWORD NextIndex = 0;
		
	f_unlink((const TCHAR*)pdst);
	//while(1)
	{
		res = f_open(&f_fdst,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING); //打开只读文件
		if(res != FR_OK)
		{
			return res;
		}
		f_lseek(&f_fdst,NextIndex);
		res=f_read(&f_fdst,f_Buffer,512,(UINT*)&br); //源头读出512字节
		if(res||br==0)
		{
			//break;
		}
		f_close(&f_fdst);
		
		res=f_open(&f_fdst,(const TCHAR*)pdst,FA_WRITE|FA_OPEN_ALWAYS); 
		if(res != FR_OK)
		{
			return res;
		}
		f_lseek(&f_fdst,NextIndex);
		res=f_write(&f_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //写入目的文件
		if(res||bw<br)
		{
			//break;
		}
		NextIndex = f_size(&f_fdst);
		f_close(&f_fdst);		
	}

	return res;
}

ErrorStatus CopyFireTestFileToMsc(u16 Type)
{
	u16 i;
	char Buffer1[50],FileName[17], Buffer2[50],DstPath[30];
//	u16 br=0;
//	u16 bw=0;

 	/* 挂载文件系统 */
	if (f_mount(&fs,"1:", 1) != FR_OK)
	{
		return ERROR;
	}
	
	sprintf(DstPath, "1:测试数据%04d%02d%02d%02d%02d%02d", 
		RTC_Type.year,RTC_Type.month,RTC_Type.day,
		RTC_Type.hour, RTC_Type.minute, RTC_Type.second);


	f_mkdir((const char*)DstPath);			
	if(Type == 0)
	{//拷贝所有测试组数据
		for(i=0;i<FireRecorder.Number;i++)
		{
			memset(FileName,0, sizeof(FileName));
			memcpy(FileName, &FireRecorder.FileName[i], sizeof(FireRecorder.FileName[0]));
			
			sprintf(Buffer1,"%s/%s.txt", FIRE_TEST_DATA_PATH, FileName);
			sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
			
//		f_open(&f_fdst,(const TCHAR*)Buffer1,FA_READ|FA_OPEN_EXISTING); //打开只读文件		
//		f_read(&f_fdst,f_Buffer,512,(UINT*)&br); //源头读出512字节
//		f_close(&f_fdst);
//		
//		f_open(&f_fdst,(const TCHAR*)Buffer2,FA_WRITE|FA_OPEN_ALWAYS); 
//		f_lseek(&f_fdst,0);
//		f_write(&f_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //写入目的文件

//		f_close(&f_fdst);
		
			if (FR_OK != FileCopy((u8*)Buffer1, (u8*)Buffer2))
			{
				return ERROR;
			}
		}	
	}
	else
	{//只拷贝1组数据
		memset(FileName,0, sizeof(Buffer2));
		memcpy(FileName, &FireRecorder.FileName[FireRecorder.Select], sizeof(FireRecorder.FileName[0]));
		
		sprintf(Buffer1,"%s/%s.txt", FIRE_TEST_DATA_PATH, FileName);
		sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
		if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
		{
			return ERROR;
		}	
	}
	
	/* 卸载文件系统 */
	//f_mount(&fs,"1:", 1);
	return SUCCESS;
}

