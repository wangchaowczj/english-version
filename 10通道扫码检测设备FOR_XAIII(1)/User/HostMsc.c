#include "includes.h"

static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */ 
static  FATFS fs;
//�ļ�����
//��psrc�ļ�,copy��pdst.
//psrc,pdst:Դ�ļ���Ŀ���ļ�
//fwmode:�ļ�д��ģʽ
//0:������ԭ�е��ļ�
//1:����ԭ�е��ļ�
u8 FileCopy(u8 *psrc,u8 *pdst)
{
	u8 res;
	u16 br=0;
	u16 bw=0;
	DWORD NextIndex = 0;
		
	f_unlink((const TCHAR*)pdst);
	//while(1)
	{
		res = f_open(&f_fdst,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING); //��ֻ���ļ�
		if(res != FR_OK)
		{
			return res;
		}
		f_lseek(&f_fdst,NextIndex);
		res=f_read(&f_fdst,f_Buffer,512,(UINT*)&br); //Դͷ����512�ֽ�
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
		res=f_write(&f_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //д��Ŀ���ļ�
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

 	/* �����ļ�ϵͳ */
	if (f_mount(&fs,"1:", 1) != FR_OK)
	{
		return ERROR;
	}
	
	sprintf(DstPath, "1:��������%04d%02d%02d%02d%02d%02d", 
		RTC_Type.year,RTC_Type.month,RTC_Type.day,
		RTC_Type.hour, RTC_Type.minute, RTC_Type.second);


	f_mkdir((const char*)DstPath);			
	if(Type == 0)
	{//�������в���������
		for(i=0;i<FireRecorder.Number;i++)
		{
			memset(FileName,0, sizeof(FileName));
			memcpy(FileName, &FireRecorder.FileName[i], sizeof(FireRecorder.FileName[0]));
			
			sprintf(Buffer1,"%s/%s.txt", FIRE_TEST_DATA_PATH, FileName);
			sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
			
//		f_open(&f_fdst,(const TCHAR*)Buffer1,FA_READ|FA_OPEN_EXISTING); //��ֻ���ļ�		
//		f_read(&f_fdst,f_Buffer,512,(UINT*)&br); //Դͷ����512�ֽ�
//		f_close(&f_fdst);
//		
//		f_open(&f_fdst,(const TCHAR*)Buffer2,FA_WRITE|FA_OPEN_ALWAYS); 
//		f_lseek(&f_fdst,0);
//		f_write(&f_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //д��Ŀ���ļ�

//		f_close(&f_fdst);
		
			if (FR_OK != FileCopy((u8*)Buffer1, (u8*)Buffer2))
			{
				return ERROR;
			}
		}	
	}
	else
	{//ֻ����1������
		memset(FileName,0, sizeof(Buffer2));
		memcpy(FileName, &FireRecorder.FileName[FireRecorder.Select], sizeof(FireRecorder.FileName[0]));
		
		sprintf(Buffer1,"%s/%s.txt", FIRE_TEST_DATA_PATH, FileName);
		sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
		if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
		{
			return ERROR;
		}	
	}
	
	/* ж���ļ�ϵͳ */
	//f_mount(&fs,"1:", 1);
	return SUCCESS;
}

