#include "includes.h"

LOG_RECORDER LogRecorder;
BLAST_RECORDER BlastRecorder;
BLAST_INFO BlastInfo;
BLAST_SINGLE_MONITOR BlastSingleMonitor;

static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */   
static  OS_EVENT   *FileSystemMutex;

void FileSystemInitOS(void)
{
	INT8U  err;
	
	FileSystemMutex = OSMutexCreate(RES_FILE_MUTEX_PRIO, &err);
}
void FileSystemLock(void)
{
	INT8U  err;
	
	OSMutexPend(FileSystemMutex,0,&err);
}
void FileSystemUnlock(void)
{	
	OSMutexPost(FileSystemMutex);
}

/**
  *************************************************************************************
  * @brief  �ж�һ���ļ��Ƿ����
  * @param  PathName ·�� 
  * @retval ���
  *         @arg @b ERROR   �ļ�������
  *         @arg @b SUCCESS �ļ�����
  * @author ZH
  * @date   2014��12��06��
  * @note   
  *************************************************************************************  
  */
ErrorStatus FileIsExist(char *FilePathName)
{
	FILINFO fno;
	
	if(FR_OK == f_stat(FilePathName, &fno))
	{
		return SUCCESS;
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ��һ���ļ���С
  * @param  PathName ·�� 
  * @retval ���
  *         @arg @b ERROR   �ļ�������
  *         @arg @b SUCCESS �ļ�����
  * @author ZH
  * @date   2018��05��10��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetFileSize(char *FilePathName, u32* size)
{	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	if(FR_OK == f_open(&f_fdst, FilePathName, FA_OPEN_EXISTING | FA_READ))
	{
		*size = f_size(&f_fdst);
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ��������һ���ļ�����
  * @param  PathName ·�� 
  * @param  FileName �ļ��� 
  * @param  FileData �ļ�����
  * @param  Bytes ���ݳ���  
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ�
  * @author ZH
  * @date   2014��12��06��
  * @note   
  *************************************************************************************  
  */
ErrorStatus CreateNewFileData(u8* PathName, u8* FileName, u8* FileData, u16 Bytes)
{
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	DIR dir;
	u8 CRC16[2]={0,0};	

	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_opendir(&dir, (const char*)PathName);
	if (res != FR_OK)
	{//û���ļ���Ӧ��·��
		res = f_mkdir((const char*)PathName);
		if(res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}

	sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_CREATE_ALWAYS | FA_WRITE|FA_READ);
	if(res == FR_OK)
	{
		res = f_write(&f_fdst, FileData, Bytes,&bw);
		if(res == FR_OK)
		{
			GetCrc16Value(0xFFFF, &CRC16[0], FileData, Bytes);//У��ֵ
			res = f_write(&f_fdst, &CRC16[0], 2, &bw);
			if(res == FR_OK)
			{
				f_close(&f_fdst);
				FileSystemUnlock();//�ͷ���Դ������ɶԳ���
				return SUCCESS;				
			}
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}

ErrorStatus GetSpecifiedFileExistState(u8* PathName, u8* FileName)
{
	FRESULT res;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	if(PathName == NULL)
	{
		strcpy((char*)f_Buffer, (char*)FileName);
	}
	else
	{
		sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);	
	}
	
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	if(res == FR_OK)
	{
		return SUCCESS;
	}
	return ERROR;
}
ErrorStatus ReadSpecifiedFileData(u8* PathName, u8* FileName, u8* FileData, u16 Bytes)
{
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	u8 FileCRC16[2]={0,0};
	u8 NewCRC16[2]={0,0};
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
		
	sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_READ);
	if((res == FR_NO_FILE) ||(res == FR_NO_PATH))
	{//û�������ļ�
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	else if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_fdst.fsize != (Bytes+2))//�������ļ�����2���ֽڵ�CRCУ��ֵ
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}

	f_lseek(&f_fdst,0);

	res = f_read(&f_fdst,FileData, Bytes, &bw);	
	if(res == FR_OK)
	{
		res = f_read(&f_fdst,&FileCRC16[0], 10, &bw);//��У��ֵ
		if((res == FR_OK) && (bw == 2))
		{			
			GetCrc16Value(0xFFFF, &NewCRC16[0], FileData, Bytes);//У��ֵ
			if ((NewCRC16[0] == FileCRC16[0]) && (NewCRC16[1] == FileCRC16[1]))
			{//��ֵ֤���
				f_close(&f_fdst);
				FileSystemUnlock();//�ͷ���Դ������ɶԳ���
				return SUCCESS;				
			}		
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ���������õĲ���ֵ
  * @param  �� 
  * @retval �� 
  * @author ZH
  * @date   2014��10��16��
  * @note   ����ֵMCU�ڲ�FLASH�����һҳ��2K��С
  *************************************************************************************  
  */
void ReadFactoryConfigParam(void)
{
	u8 temp[20];
	
	//�豸ID
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DEVICE_ID_FILE_NAME, device_info.mac, sizeof(device_info.mac)))
	{
		memset(device_info.mac, 0, sizeof(device_info.mac));
	}
	
	//�豸Ӳ���汾
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, HW_VERSION_FILE_NAME, device_info.hw_version, sizeof(device_info.hw_version)))
	{
		memset(device_info.hw_version, 0, sizeof(device_info.hw_version));
	}
	
	//�豸����
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DEVICE_TYPE_FILE_NAME, &device_info.DeviceType, sizeof(device_info.DeviceType)))
	{
		memset(&device_info.DeviceType, 0, sizeof(device_info.DeviceType));
	}	
	if((device_info.DeviceType != DEVICE_TYPE_SAFE_MONITOR) && 
        (device_info.DeviceType != DEVICE_TYPE_CHINESE_GENERAL) && 
        (device_info.DeviceType != DEVICE_TYPE_SAFE_MONITOR_OFFLINE))
    {
        device_info.DeviceType = DEVICE_TYPE_SAFE_MONITOR;
    }
    
	//���ܷ���������IP���˿�
	if(ERROR == ReadSpecifiedFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&device_info.Server, sizeof(device_info.Server)))
	{
		device_info.Server.Ip = IpAddrStringToU32("60.205.57.100");
		device_info.Server.Port = 20601;
		memset(&device_info.Server.Date[0], '0',sizeof(device_info.Server.Date));
	}
	
	//�豸�������ò���
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, CTRL_BYTES_FILE_NAME, (u8*)&device_info.config, sizeof(device_info.config)))
	{
		memset((u8*)&device_info.config, 0, sizeof(device_info.config));
	}

	//�����׹ܵ�����Χ
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DET_CURRENT_FILE_NAME, (u8*)&temp[0], 4))
	{	
		device_info.single_current_min = 10;
		device_info.single_current_max = 35;	
	}
	else
	{
		device_info.single_current_min = (u16)(((u16)temp[0]<<8) + temp[1]);
		device_info.single_current_max = (u16)(((u16)temp[2]<<8) + temp[3]);		
	}	
	//�׹�Ƶ�ʷ�Χ
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DET_FREQ_FILE_NAME, (u8*)&temp[0], 8))
	{
		device_info.det_freq_min = 55000;
		device_info.det_freq_max = 110000;
	}
	else
	{
		device_info.det_freq_min = (u32)(((u32)temp[0]<<24) + ((u32)temp[1]<<16) + ((u32)temp[2]<<8) + temp[3]);		
		device_info.det_freq_max = (u32)(((u32)temp[4]<<24) + ((u32)temp[5]<<16) + ((u32)temp[6]<<8) + temp[7]);	
	}	


	//�豸��ص�ѹУ׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.bat.k = 0;
		adc_adjust.bat.b = 0;
	}
	else
	{
		adc_adjust.bat.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.bat.b = (u16)(((u16)temp[2]<<8) + temp[3]);		
	}
	
	//���ߵ�ѹУ׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.linab.k = 0;
		adc_adjust.linab.b = 0;
	}
	else
	{
		adc_adjust.linab.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.linab.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
	}

	//��ѹ�µ���С����У׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.low1current.k = 0;
		adc_adjust.low1current.b = 0;
	}
	else
	{
		adc_adjust.low1current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.low1current.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
	}

	//��ѹ�µ���������У׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.low2current.k = 0;
		adc_adjust.low2current.b = 0;
	}
	else
	{
		adc_adjust.low2current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.low2current.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
	}
	
	//��ѹ�µ���С����У׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.high1current.k = 0;
		adc_adjust.high1current.b = 0;
	}
	else
	{
		adc_adjust.high1current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.high1current.b = (u16)(((u16)temp[2]<<8) + temp[3]);
	}	

	//��ѹ�µ���������У׼����
	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
	{
		adc_adjust.high2current.k = 0;
		adc_adjust.high2current.b = 0;
	}
	else
	{
		adc_adjust.high2current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
		adc_adjust.high2current.b = (u16)(((u16)temp[2]<<8) + temp[3]);
	}
	//���ڳ������
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DELAY_SAMPLE_FILE_NAME, &Op_Data.DelaySampleCfg, sizeof(Op_Data.DelaySampleCfg)))
	{
		memset(&Op_Data.DelaySampleCfg, 0, sizeof(Op_Data.DelaySampleCfg));
	}
	
	//ɨ��ģ������
	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, SCAN_FILE_NAME, &device_info.HasScan, sizeof(device_info.HasScan)))
	{
		device_info.HasScan = 1;//Ĭ�ϴ�ɨ��ͷ
	}  
}



ErrorStatus DeleteSpecifiedFile(u8* PathName, u8* FileName)
{
	FRESULT res;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);
	res = f_unlink((const char*)f_Buffer);
	if((res == FR_OK) || (res == FR_NO_FILE) || (res == FR_NO_PATH))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}
    
/**
  *************************************************************************************
  * @brief  �����ļ�У��ֵȷ��
  * @param  PathName ·�� 
  * @param  FileName �ļ��� 
  * @param  DeleteFlag ɾ����־(0��ʾ��У�鲻ͨ��ʱ��ɾ����1��ʾ����У�鲻ͨ��ʱɾ��)
  * @retval У����
  *         @arg @b ERROR   У�鲻ͨ��
  *         @arg @b SUCCESS У��ͨ��
  * @author ZH
  * @date   2014��12��06��
  * @note   
  *************************************************************************************  
  */
ErrorStatus ConfigFileVerify(u8* PathName, u8* FileName, u8 DeleteFlag)
{ 
    FRESULT res;         /* FatFs function common result code */
	u16 len;
	u8 CRC16[2]={0xFF,0xFF};
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_READ);
	if((res == FR_NO_FILE) ||(res == FR_NO_PATH))
	{//û�������ļ�ʱĬ��У׼ͨ��
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;
	}
	else if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}

	f_lseek(&f_fdst,0);
	
	while(NULL != f_gets((char*)f_Buffer, 250, &f_fdst))
	{
		if(f_Buffer[0] == '[')
		{
			len = strlen((const char*)f_Buffer);
			GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
			continue;
		}
		else if((strlen((const char*)f_Buffer) == 6) && (f_Buffer[0] == ':') && (f_Buffer[5] == '\n'))
		{
			sprintf((char*)&f_Buffer[100],":%02X%02X\n", CRC16[0], CRC16[1]);
			if(strcmp((const char*)&f_Buffer[0], (const char*)&f_Buffer[100])==0)//У��ֵ�Ƚ�
			{
				f_close(&f_fdst);
				FileSystemUnlock();//�ͷ���Դ������ɶԳ���
				return SUCCESS;	//У׼ͨ��		
			}
		}
		break;
	}
	if(DeleteFlag != 0)
	{
		sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);
		f_unlink((const char*)f_Buffer);
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}

void DeleteAllFileSpecifiedPath(char* Path, char* ExtraFileName)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
    res = f_opendir(&dir, Path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
			{                    /* It is a directory */
//				sprintf(buffer, "%s/%s/",path, fn);
//				res = f_unlink((const TCHAR *)&buffer[0]);
//				if (res != FR_OK)
//				{
//					return ERROR;
//				}
            } 
			else 
			{  /* It is a file. */
				if ((ExtraFileName != NULL) && (strcmp(fn, ExtraFileName) == 0))
				{
					continue;
				}
				sprintf((char*)f_Buffer, "%s/%s",Path, fn);
				f_unlink((const char*)f_Buffer);		
            }
        }
    }
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}

ErrorStatus GetConfigFileString(u8* KeyName, u8* ReturnedString, u16 nSize, u8* PathName, u8* FileName)
{	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	ReturnedString[0] = 0;
	
	sprintf((char*)f_Buffer,"%s/%s",PathName, FileName);

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) == FR_OK)
	{
		f_lseek(&f_fdst,0);
		while(NULL != f_gets((char*)f_Buffer, 250, &f_fdst))
		{
			if(f_Buffer[0] == '[')
			{
				if((f_Buffer[strlen((const char*)KeyName)+1] == ']') && (f_Buffer[strlen((const char*)KeyName)+2] == '=')
					&& (f_Buffer[strlen((const char*)f_Buffer)-1] == '\n'))
				{
					if(memcmp(&f_Buffer[1], KeyName, strlen((const char*)KeyName)) == 0)
					{//�ļ����Ѿ��иü�����
						if(strlen((const char*)&f_Buffer[strlen((const char*)KeyName)+3]) > nSize)
						{
							break;
						}
						f_Buffer[strlen((const char*)f_Buffer)-1] = 0x00;
						strcpy((char*)ReturnedString, (const char*)&f_Buffer[strlen((const char*)KeyName)+3]);
						FileSystemUnlock();//�ͷ���Դ������ɶԳ���
						return SUCCESS;
					}				
				}
			}
			else
			{
				break;
			}
		}
		f_close(&f_fdst);			
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}

u32 GetConfigFileU32(u8* KeyName,u32 nDefault, u8* PathName, u8* FileName)
{
	u8 temp[11];
	u32 Value = 0;
	
	memset(temp, 0, sizeof(temp));
	if(ERROR != GetConfigFileString(KeyName, temp, 11,PathName, FileName))
	{
		sscanf((char*)temp, "%ld", &Value);
		return Value;	
	}
	return nDefault;
}

ErrorStatus SaveUserConfigParam(void)
{
	FRESULT res;
	u16 len;
	u8 i;
	u8 CRC16[2]={0xFF,0xFF};
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s",USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_CREATE_ALWAYS | FA_WRITE|FA_READ);
	if(res == FR_NO_PATH)
	{//·��������
		f_mkdir((const char*)USER_CONFIG_PARAM_FILE_PATH);
	}
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_CREATE_ALWAYS | FA_WRITE|FA_READ);
	if(res == FR_OK)
	{	
		i=0;
		while(1)
		{
			i++;
			if(1 == i)
			{
				sprintf((char*)f_Buffer,"[�׹�����]=%d\n", Det.Type);
			}
			else if(2 == i)
			{
				sprintf((char*)f_Buffer,"[Һ������]=%d\n", device_info.LcdBrightStrength);
			}
			else if(3 == i)
			{
				sprintf((char*)f_Buffer,"[����ʱ��]=%d\n", device_info.LcdBrightTime);
			}
			else if(4 == i)
			{
				sprintf((char*)f_Buffer,"[����ʱ]=%d\n", device_info.BlastDelay);
			}
			else if(5 == i)
			{
				sprintf((char*)f_Buffer,"[WIFI���翪��]=%d\n", (u16)GET_WIFI_POWER_STATE());
			}
			else if(6 == i)
			{
				sprintf((char*)f_Buffer,"[�豸��ַ]=%d\n", device_info.name);
			}
			else if(7 == i)
			{
				sprintf((char*)f_Buffer,"[�豸�÷�]=%d\n", device_info.DeviceUsages);
			}
			else if(8 == i)
			{
				sprintf((char*)f_Buffer,"[��������]=%d\n", SlaveInfo.Number);
			}
			else if(9 == i)
			{
				sprintf((char*)f_Buffer,"[�б�����]=%d\n", device_info.NetInfoSort);
			}
			else if(10 == i)
			{
				sprintf((char*)f_Buffer,"[Ĭ��ע�����]=%d\n", device_info.RegNetParamDefault);
			}
			else if(11 == i)
			{
				sprintf((char*)f_Buffer,"[������ɫ]=%lu\n", device_info.BackgroundColor);
			}
			else if(12 == i)
			{
				sprintf((char*)f_Buffer,"[������ɫ]=%lu\n", device_info.TextColor);
			}			
			else
			{
				break;
			}
			f_puts((const char*)f_Buffer,&f_fdst);
			
			len = strlen((const char*)f_Buffer);
			GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);			
		}		
		
		sprintf((char*)&f_Buffer[0], ":%02X%02X\n", CRC16[0], CRC16[1]);
		f_puts((const char*)f_Buffer,&f_fdst);
		
		f_close(&f_fdst);
	
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;	
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}

void ReadUserConfigParam(void)
{
	if(ConfigFileVerify(USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME, 1) == SUCCESS)
	{//�ļ�У��ͨ��
		//�׹�����
		Det.Type = GetConfigFileU32("�׹�����", DET_TYPE_XA06, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		//Һ������
		device_info.LcdBrightStrength = GetConfigFileU32("Һ������", 0x05, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		//����ʱ��
		device_info.LcdBrightTime = GetConfigFileU32("����ʱ��", 0x01, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		//����ʱ
		device_info.BlastDelay = GetConfigFileU32("����ʱ", 0x01, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);

		//�豸��ַ
		device_info.name = GetConfigFileU32("�豸��ַ", 'A', USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		//�豸�÷�
		device_info.DeviceUsages = GetConfigFileU32("�豸�÷�", DEVICE_USAGES_ALONE, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);

		SlaveInfo.Number = (u8)GetConfigFileU32("��������", SLAVE_NUMBER_MAX, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		
        device_info.NetInfoSort = (u8)GetConfigFileU32("�б�����", 0, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
		
        device_info.RegNetParamDefault = (u8)GetConfigFileU32("Ĭ��ע�����", 0, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
        
        device_info.BackgroundColor = GetConfigFileU32("������ɫ", COLOR_DLG_BK, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
        device_info.TextColor = GetConfigFileU32("������ɫ", GUI_BLUE, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME);
        
        //WIFI���翪��
		if(WifiConfigIsOpen())
		{
			OPEN_WIFI_POWER();
		}
		else
		{
			CLOSE_WIFI_POWER();
		}
	}
	else
	{//�ļ�У�鲻ͨ��������Ĭ��ֵ
		Det.Type = DET_TYPE_XA06;
		device_info.LcdBrightStrength = 0x05;
		device_info.LcdBrightTime = 0x01;
		device_info.BlastDelay = 0x01;//0��ʾ����ʱ��1��ʾ5�룬2��ʾ(2-1)����
		device_info.name = 'A';
		device_info.DeviceUsages = DEVICE_USAGES_ALONE;
		SlaveInfo.Number = SLAVE_NUMBER_MAX;
        device_info.RegNetParamDefault = 0;
        
        device_info.BackgroundColor = COLOR_DLG_BK;
        device_info.BackgroundColor = GUI_BLUE;
	}
    if(device_info.TextColor == device_info.BackgroundColor)
    {
        device_info.TextColor = GUI_BLUE;
        device_info.BackgroundColor = COLOR_DLG_BK;    
    }    
	if((Det.Type != DET_TYPE_XA06) && (Det.Type != DET_TYPE_XA21))
	{
		Det.Type = DET_TYPE_XA06;
	}
	
    if(device_info.RegNetParamDefault >= NET_PARAM_MAX)
    {
        device_info.RegNetParamDefault = 0;
    }
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		SetRS485Master();		
	}
	else
	{
		SetRS485Slave();
	}
	if(DEVICE_USAGES_SLAVE != device_info.DeviceUsages)
	{//���Ǵӻ�ʱ��Ĭ��Ϊ��A��
		device_info.name = 'A';
	}
//	if(DEVICE_USAGES_ALONE != device_info.DeviceUsages)
//	{
//		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
//		{
//			if ((device_info.BlastDelay != 0) && (device_info.BlastDelay != 1))
//			{//����ģʽֻ����ʱ0�����5��
//				device_info.BlastDelay = 0x01;//0��ʾ����ʱ��1��ʾ5�룬2��ʾ(2-1)����
//			}
//		}
//	}
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		SetRS485Master();
	}
	else
	{
		SetRS485Slave();
	}	
}
u8 WifiConfigIsOpen(void)
{
	if(0 == GetConfigFileU32("WIFI���翪��", (u8)1, USER_CONFIG_PARAM_FILE_PATH, USER_CONFIG_PARAM_FILE_NAME))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
ErrorStatus SaveWifiInfo(u8* ssid, u8* passwords)
{
	FRESULT res;
	u8 ssidLen = 0;
	DWORD ofs = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s",WIFI_CONFIG_PARAM_FILE_PATH, WIFI_CONFIG_PARAM_FILE_NAME);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE|FA_READ);
	if(res == FR_NO_PATH)
	{//·��������
		f_mkdir((const char*)WIFI_CONFIG_PARAM_FILE_PATH);
		res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE|FA_READ);
	}	
	
	if(res == FR_OK)
	{
		ssidLen = strlen((const char*)ssid);
		f_lseek(&f_fdst,0);
		while(NULL != f_gets((char*)f_Buffer, 250, &f_fdst))
		{				
			if((strlen((const char*)f_Buffer) == 201) && 
				(f_Buffer[ssidLen] == '\t') && 
				(f_Buffer[strlen((const char*)f_Buffer)-1] == '\n') && 
				(memcmp(&f_Buffer[0], ssid, strlen((const char*)ssid)) == 0))
			{//�ļ����Ѿ��иü�����
				f_lseek(&f_fdst, ofs);
				break;
			}
			ofs = f_tell(&f_fdst);
		}

		memset((char*)f_Buffer, 0, 256);
		sprintf((char*)f_Buffer,"%s\t", ssid);			
		
		
		Base64Encode(passwords, &f_Buffer[strlen((const char*)f_Buffer)], strlen((const char*)passwords));
		while(strlen((const char*)f_Buffer) < 200)
		{
			f_Buffer[strlen((const char*)f_Buffer)] = '*';
		}
		
		f_Buffer[strlen((const char*)f_Buffer)] = '\n';
		f_puts((const char*)f_Buffer,&f_fdst);
		
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;	
}
ErrorStatus ReadWifiInfo(u8* ssid, u8* passwords)
{
	u16 i, Len;
	u8 ssidLen = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���

	sprintf((char*)f_Buffer,"%s/%s",WIFI_CONFIG_PARAM_FILE_PATH, WIFI_CONFIG_PARAM_FILE_NAME);

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) == FR_OK)
	{
		ssidLen = strlen((const char*)ssid);
		f_lseek(&f_fdst,0);
		while(NULL != f_gets((char*)f_Buffer, 250, &f_fdst))
		{			
			if((strlen((const char*)f_Buffer) == 201) && 
				(f_Buffer[ssidLen] == '\t') && 
				(f_Buffer[strlen((const char*)f_Buffer)-1] == '\n') && 
				(memcmp(&f_Buffer[0], ssid, strlen((const char*)ssid)) == 0))
			{//�ļ����Ѿ��иü�����
				Len = strlen((const char*)f_Buffer);
				for(i=ssidLen;i<Len;i++)
				{
					if((f_Buffer[i] == '*') || (f_Buffer[i] == '\n'))
					{
						f_Buffer[i] = 0;
						Len = strlen((const char*)&f_Buffer[ssidLen+1]);
						if(Len != 0)
						{
							if(Base64Decode(&f_Buffer[ssidLen+1], passwords, Len))//Base64����
							{
								f_close(&f_fdst);
								FileSystemUnlock();//�ͷ���Դ������ɶԳ���
								return ERROR;
							}
						}

						f_close(&f_fdst);
						FileSystemUnlock();//�ͷ���Դ������ɶԳ���
						return SUCCESS;
					}
				}				
			}
		}
		f_close(&f_fdst);			
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;	
}

ErrorStatus DeleteAllDetInfo(void)
{
	u16 i;	
    
    WriteSystemLog("ɾ�������׹�", strlen("ɾ�������׹�"), CURRENT_STATUS);
    
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		Det.Info[i].Serl.U32Value = 0x00;
		Det.Info[i].Pwd.U32Value = 0x00;
		Det.Info[i].Freq.U32Value = 0x00;
		Det.Info[i].Delay = 0x00;
	}	
	return SaveAllDetInfo(0);
}

ErrorStatus DeleteAllErrorDetInfo(void)
{
	u16 i;	
	
	if(0 == Det.ErrorCount)
	{
		return SUCCESS;
	}
    WriteSystemLog("ɾ�����д����׹�", strlen("ɾ�����д����׹�"), CURRENT_STATUS);
    
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(Det.Info[i].Err != 0x00)
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;
                Det.Info[i].Freq.U32Value = 0x00;
                Det.Info[i].Err = 0x00;
			}		
		}
	}	
	return SaveAllDetInfo(0);
}

ErrorStatus DeleteDetInfo(HOLE* StartHole, HOLE* EndHole)
{
	u16 i;
	u32 StartValue, EndValue, CurrentValue;
	char Buffer[30];
    
    sprintf(Buffer, "ɾ���׹�:%d-%d��%d-%d", StartHole->Name, StartHole->Inside, EndHole->Name, EndHole->Inside);
    WriteSystemLog((char*)Buffer, strlen((char*)Buffer), CURRENT_STATUS);
	
	StartValue = ((u32)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((u32)EndHole->Name<<16) + EndHole->Inside;
	
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			CurrentValue = ((u32)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
			if ((CurrentValue >= StartValue) && (CurrentValue <= EndValue))
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;
                Det.Info[i].Freq.U32Value = 0x00;
                Det.Info[i].Err = 0x00;                
			}		
		}
	}
	return SaveAllDetInfo(0);
}
ErrorStatus DeleteDetInfoNotChangeRank(HOLE* StartHole, HOLE* EndHole)
{
	u16 i;
	u32 StartValue, EndValue, CurrentValue;
	
	StartValue = ((u32)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((u32)EndHole->Name<<16) + EndHole->Inside;
	
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			CurrentValue = ((u32)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
			if ((CurrentValue >= StartValue) && (CurrentValue <= EndValue))
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;
				Det.Info[i].Freq.U32Value = 0x00;
                Det.Info[i].Err = 0x00;                
			}		
		}
	}
	return SaveAllDetInfoNotChangeRank(0);
}
void DeleteDetInfoNotSave(HOLE* StartHole, HOLE* EndHole)
{
	u16 i;
	u32 StartValue, EndValue, CurrentValue;
	
	StartValue = ((u32)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((u32)EndHole->Name<<16) + EndHole->Inside;
	
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			CurrentValue = ((u32)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
			if ((CurrentValue >= StartValue) && (CurrentValue <= EndValue))
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;
                Det.Info[i].Freq.U32Value = 0x00;
                Det.Info[i].Err = 0x00;                
			}		
		}
	}
    GetDetNum();
	AllRegDetRankBaseOnHoleInfo();
}
ErrorStatus SaveAllDetInfo(u8  Type)
{	
	AllRegDetRankBaseOnHoleInfo();
	
	return SaveAllDetInfoNotChangeRank(Type);
}
ErrorStatus SaveAllDetInfoNotChangeRank(u8  Type)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
	u16 i;
	u16 Len;
	u8 CRC16[2]={0xFF,0xFF};
	const char* FileName;

	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	if (Type == 0)
	{//��ǰʹ�õ��׹�����
		FileName = DET_INFO_FILE_NAME;
	}
	else
	{//�������ݵ��׹�����
		FileName = DET_INFO_BACKUP_FILE_NAME;
	}
	//�Ѿ��ļ�ɾ��
	res = f_unlink(FileName);
	if((res == FR_OK) || (res == FR_NO_FILE))
	{
		;
	}	
	else if(res == FR_NO_PATH)
	{
		res = f_mkdir(DET_INFO_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;//�ļ�ϵͳ�쳣
		}
	}	

	if(f_open(&f_fdst, FileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst, 0);
	
	//�׹���Ϣ
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		Len = 0;
		f_Buffer[Len++] = Det.Info[i].Serl.U8Buffer[0];
		f_Buffer[Len++] = Det.Info[i].Serl.U8Buffer[1];
		f_Buffer[Len++] = Det.Info[i].Serl.U8Buffer[2];
        f_Buffer[Len++] = Det.Info[i].Serl.U8Buffer[3];
		f_Buffer[Len++] = Det.Info[i].Pwd.U8Buffer[0];
		f_Buffer[Len++] = Det.Info[i].Pwd.U8Buffer[1];
		f_Buffer[Len++] = Det.Info[i].Pwd.U8Buffer[2];
        f_Buffer[Len++] = Det.Info[i].Pwd.U8Buffer[3];
		f_Buffer[Len++] = Det.Info[i].Freq.U8Buffer[0];
		f_Buffer[Len++] = Det.Info[i].Freq.U8Buffer[1];
		f_Buffer[Len++] = Det.Info[i].Freq.U8Buffer[2];
        f_Buffer[Len++] = Det.Info[i].Freq.U8Buffer[3];
		f_Buffer[Len++] = Det.Info[i].Delay>>8;
		f_Buffer[Len++] = Det.Info[i].Delay&0xFF;			
		f_Buffer[Len++] = Det.Info[i].Hole.Name>>8;
		f_Buffer[Len++] = Det.Info[i].Hole.Name&0xFF;	
		f_Buffer[Len++] = Det.Info[i].Hole.Inside;		
				
		res = f_write(&f_fdst, &f_Buffer[0],Len, &bw);
		if ((res != FR_OK) || (bw != Len))
		{
			f_close(&f_fdst);
			GetDetNum();
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
		
		GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],Len);			
	}
	
	//ÿ���׵�����׹���
	Len = sizeof(Det.HoleNumber);
	res = f_write(&f_fdst, &Det.HoleNumber[0],Len, &bw);
	if ((res != FR_OK) || (bw != Len))
	{
		f_close(&f_fdst);
		GetDetNum();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &Det.HoleNumber[0],Len);	
	
	Len = 2;
	res = f_write(&f_fdst, &CRC16[0],Len, &bw);
	if ((res != FR_OK) || (bw != Len))
	{
		f_close(&f_fdst);
		GetDetNum();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���		
		return ERROR;
	}	
	GetDetNum();
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}

ErrorStatus ReadAllDetInfo(u8  Type)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
	u16 i;
	u16 Len; 
	u8 CRC16[2]={0xFF,0xFF};	
	const char* FileName;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	memset((u8*)&Det.Info[0], 0, sizeof(Det.Info));
	memset((u8*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));

	if (Type == 0)
	{//��ǰʹ�õ��׹�����
		FileName = DET_INFO_FILE_NAME;
	}
	else
	{//�������ݵ��׹�����
		FileName = DET_INFO_BACKUP_FILE_NAME;
	}
	
	if(f_open(&f_fdst, FileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		GetDetNum();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	
	//����֤���ݳ����Ƿ����Ҫ��
	if(f_size(&f_fdst) != (DET_MAX_SIZE*17+2+DET_MAX_SIZE))
	{
		f_close(&f_fdst);	
		GetDetNum();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	f_lseek(&f_fdst, 0);
	
	//���׹���Ϣ
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		Len = 17;
		res = f_read(&f_fdst,f_Buffer, Len, &bw);
		if ((res != FR_OK) && (bw != Len))
		{			
			f_close(&f_fdst);	
			memset((u8*)&Det.Info[0],0,sizeof(Det.Info));
			memset((u8*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
			GetDetNum();
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
		
		Det.Info[i].Serl.U8Buffer[0] = f_Buffer[0];
		Det.Info[i].Serl.U8Buffer[1] = f_Buffer[1]; 
		Det.Info[i].Serl.U8Buffer[2] = f_Buffer[2];
		Det.Info[i].Serl.U8Buffer[3] = f_Buffer[3];
		Det.Info[i].Pwd.U8Buffer[0] = f_Buffer[4];
		Det.Info[i].Pwd.U8Buffer[1] = f_Buffer[5]; 
		Det.Info[i].Pwd.U8Buffer[2] = f_Buffer[6];
		Det.Info[i].Pwd.U8Buffer[3] = f_Buffer[7];
		Det.Info[i].Freq.U8Buffer[0] = f_Buffer[8];
		Det.Info[i].Freq.U8Buffer[1] = f_Buffer[9]; 
		Det.Info[i].Freq.U8Buffer[2] = f_Buffer[10];
		Det.Info[i].Freq.U8Buffer[3] = f_Buffer[11];
		Det.Info[i].Delay = (((u32)f_Buffer[12])<<8)+ (((u32)f_Buffer[13])&0xFF);		
		Det.Info[i].Hole.Name = (((u32)f_Buffer[14])<<8)+ (((u32)f_Buffer[15])&0xFF);	
		Det.Info[i].Hole.Inside = f_Buffer[16];

		//����У�������Ƿ���ȷ
		GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],Len);				
	}
	
	//��ÿ���׵������
	Len = sizeof(Det.HoleNumber);
	res = f_read(&f_fdst,(u8*)&Det.HoleNumber[0], Len, &bw);
	if ((res != FR_OK) && (bw != Len))
	{			
		f_close(&f_fdst);	
		memset((u8*)&Det.Info[0],0,sizeof(Det.Info));
		memset((u8*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
		GetDetNum();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	//����У�������Ƿ���ȷ
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &Det.HoleNumber[0],Len);
		
	//�Ƚ�У��ֵ�Ƿ�һ��
	Len = 2;
	res = f_read(&f_fdst,f_Buffer, Len, &bw);
	if ((res != FR_OK) && (bw != Len))
	{	
		memset((u8*)&Det.Info[0],0,sizeof(Det.Info));
		memset((u8*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
	}
	else
	{
		if (!((f_Buffer[0] == CRC16[0]) && (f_Buffer[1] == CRC16[1])))
		{
			memset((u8*)&Det.Info[0],0,sizeof(Det.Info));
			memset((u8*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
		}	
	}
	f_close(&f_fdst);

	GetDetNum();
	AllRegDetRankBaseOnHoleInfo();
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	return SUCCESS;
}

ErrorStatus SaveNetParam(void)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
	u8 CRC16[2]={0xFF,0xFF};
    u16 i;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	//�Ѿ��ļ�ɾ��
	res = f_unlink((const char*)NET_PARAM_FILE_COMPLETE_PATH);
	if((res == FR_OK) || (res == FR_NO_FILE))
	{
		;
	}	
	else if(res == FR_NO_PATH)
	{
		res = f_mkdir(USER_CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{
			ReadNetParam();
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;//�ļ�ϵͳ�쳣
		}
	}
	
	if(f_open(&f_fdst, (const char*)NET_PARAM_FILE_COMPLETE_PATH, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		ReadNetParam();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst, 0);
	for(i=0; i<NET_PARAM_MAX; i++)
    {
        memcpy((u8*)&f_Buffer[0], (u8*)&NetParam[i], sizeof(NET_PARAM));
                
        res = f_write(&f_fdst, &f_Buffer[0],sizeof(NET_PARAM), &bw);
        if ((res != FR_OK) || (bw != sizeof(NET_PARAM)))
        {
            f_close(&f_fdst);
            ReadNetParam();
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return ERROR;
        } 
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],sizeof(NET_PARAM));
    }
		
	f_Buffer[0] = CRC16[0];
	f_Buffer[1] = CRC16[1];		

	res = f_write(&f_fdst, &CRC16[0],2, &bw);
	if ((res != FR_OK) || (bw != 2))
	{
		f_close(&f_fdst);
		ReadNetParam();
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;	
}

void ReadNetParam(void)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
	u8 CRC16[2]={0xFF,0xFF};	
	u16 i;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
		
	if(f_open(&f_fdst, (const char*)NET_PARAM_FILE_COMPLETE_PATH, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		for(i = 0;i <NET_PARAM_MAX; i++)
		{
			NetParam[i].Count = 1+i;
			NetParam[i].InsideSpacesDly = 0;
			NetParam[i].SpacesDly = 0;		
		}
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ;
	}
	
	//����֤���ݳ����Ƿ����Ҫ��
	if(f_size(&f_fdst) != (sizeof(NET_PARAM)*NET_PARAM_MAX+2))
	{
		f_close(&f_fdst);	
		for(i = 0;i <NET_PARAM_MAX; i++)
		{
			NetParam[i].Count = 1+i;
			NetParam[i].InsideSpacesDly = 0;
			NetParam[i].SpacesDly = 0;	
		}
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ;		
	}
	f_lseek(&f_fdst, 0);
	for(i=0; i<NET_PARAM_MAX; i++)
    {
        res = f_read(&f_fdst,f_Buffer, sizeof(NET_PARAM), &bw);
        if ((res != FR_OK) && (bw != sizeof(NET_PARAM)))
        {			
            f_close(&f_fdst);	
            for(i = 0;i <NET_PARAM_MAX; i++)
            {
                NetParam[i].Count = 1+i;
                NetParam[i].InsideSpacesDly = 0;
                NetParam[i].SpacesDly = 0;		
            }
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return ;
        }
        memcpy((u8*)&NetParam[i], f_Buffer, sizeof(NET_PARAM));	
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],sizeof(NET_PARAM));        
    }		
	//�Ƚ�У��ֵ�Ƿ�һ��
	res = f_read(&f_fdst,f_Buffer, 2, &bw);
	if ((res != FR_OK) && (bw != 2))
	{	
		for(i = 0;i <NET_PARAM_MAX; i++)
		{
			NetParam[i].Count = 1+i;
			NetParam[i].InsideSpacesDly = 0;
			NetParam[i].SpacesDly = 0;		
		}
	}
	else
	{
		if (!((f_Buffer[0] == CRC16[0]) && (f_Buffer[1] == CRC16[1])))
		{//У��ֵ�����
			for(i = 0;i <NET_PARAM_MAX; i++)
			{
				NetParam[i].Count = 1+i;
				NetParam[i].InsideSpacesDly = 0;
				NetParam[i].SpacesDly = 0;		
			}
		}	
	}

	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}
ErrorStatus SaveDetFeature(u8 Feature)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	//�Ѿ��ļ�ɾ��
	res = f_unlink((const char*)DET_FEATURE_FILE_COMPLETE_PATH);
	if((res == FR_OK) || (res == FR_NO_FILE))
	{
		;
	}	
	else if(res == FR_NO_PATH)
	{
		res = f_mkdir(USER_CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;//�ļ�ϵͳ�쳣
		}
	}
	
	if(f_open(&f_fdst, (const char*)DET_FEATURE_FILE_COMPLETE_PATH, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst, 0);
	res = f_write(&f_fdst, &Feature,sizeof(Feature), &bw);
    if ((res != FR_OK) || (bw != sizeof(Feature)))
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return ERROR;
    } 
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;	
}

u8 GetDetFeatureDefult(void)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */	
	u8 Feature = 0;
	
    //������Ĭ�ϵ�������
    if(memcmp(device_info.mac, "F04", 3) == 0)
    {//�ӱ�����
        Feature = 'D';
    }
    if(memcmp(device_info.mac, "F10", 3) == 0)
    {//����
        Feature = '8';
    }
    if(memcmp(device_info.mac, "F34", 3) == 0)
    {//��̩
        Feature = '2';
    }
    else if(memcmp(device_info.mac, "F36", 3) == 0)
    {//ɽ��ʥ����
        Feature = '3';
    }
    else if(memcmp(device_info.mac, "F44", 3) == 0)
    {//����
        Feature = '3';
    }
    else if(memcmp(device_info.mac, "F49", 3) == 0)
    {//��������
        Feature = 'S';
    }
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
		
	if(f_open(&f_fdst, (const char*)DET_FEATURE_FILE_COMPLETE_PATH, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return Feature;
	}
	
	//����֤���ݳ����Ƿ����Ҫ��
	if(f_size(&f_fdst) != 1)
	{
		f_close(&f_fdst);	
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return Feature;		
	}
	f_lseek(&f_fdst, 0);
	res = f_read(&f_fdst,f_Buffer, 1, &bw);
    if ((res != FR_OK) && (bw != 1))
    {			
        f_close(&f_fdst);	
        
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return Feature;
    }		
    if(((f_Buffer[0] >= '0') && (f_Buffer[0] <= '9')) || 
        ((f_Buffer[0] >= 'A') && (f_Buffer[0] <= 'Z')) || 
        ((f_Buffer[0] >= 'a') && (f_Buffer[0] <= 'z')))
    {
        Feature = f_Buffer[0];
    }
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return Feature;
}
ErrorStatus SaveMasterTimeShareTest(u8 TimeShare)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	//�Ѿ��ļ�ɾ��
	res = f_unlink((const char*)MASTER_TIME_SHARE_FILE_COMPLETE_PATH);
	if((res == FR_OK) || (res == FR_NO_FILE))
	{
		;
	}	
	else if(res == FR_NO_PATH)
	{
		res = f_mkdir(USER_CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;//�ļ�ϵͳ�쳣
		}
	}
	
	if(f_open(&f_fdst, (const char*)MASTER_TIME_SHARE_FILE_COMPLETE_PATH, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst, 0);
	res = f_write(&f_fdst, &TimeShare,sizeof(TimeShare), &bw);
    if ((res != FR_OK) || (bw != sizeof(TimeShare)))
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return ERROR;
    } 
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;	
}

u8 GetMasterTimeShareTest(void)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */	
	u8 TimeShare = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
		
	if(f_open(&f_fdst, (const char*)MASTER_TIME_SHARE_FILE_COMPLETE_PATH, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return TimeShare;
	}
	
	//����֤���ݳ����Ƿ����Ҫ��
	if(f_size(&f_fdst) != 1)
	{
		f_close(&f_fdst);	
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return TimeShare;		
	}
	f_lseek(&f_fdst, 0);
	res = f_read(&f_fdst,f_Buffer, 1, &bw);
    if ((res != FR_OK) && (bw != 1))
    {			
        f_close(&f_fdst);	
        
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return TimeShare;
    }		
    TimeShare = f_Buffer[0];
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return TimeShare;
}
/**
  *************************************************************************************
  * @brief  ����ӻ����׹�����
  * @param  SlaveIndex �ӻ�����
  * @retval ErrorStatus  ���
  *    @arg @b TRUE    �ɹ�
  *    @arg @b FALSE   ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SaveSlaveDetInfo(u8 SlaveIndex)
{
	FRESULT res;
	UINT  bw;         /* File read/write count */
	DIR dir;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	sprintf((char*)f_Buffer, "%s/%c.txt",SLAVE_DET_INFO_FILE_PATH, (SlaveIndex<26)?'A'+SlaveIndex:'a'+(SlaveIndex-26));
	
	res = f_opendir(&dir, SLAVE_DET_INFO_FILE_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(SLAVE_DET_INFO_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}	
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst, 0);
			
	res = f_write(&f_fdst, (char*)&Det.Info[0],sizeof(Det.Info[0])*DET_MAX_SIZE, &bw);
	if ((res != FR_OK) || (bw != sizeof(Det.Info[0])*DET_MAX_SIZE))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
		
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;		
}
/**
  *************************************************************************************
  * @brief  ���ӻ����׹�����
  * @param  SlaveIndex �ӻ�����
  * @retval ErrorStatus  ���
  *    @arg @b TRUE    �ɹ�
  *    @arg @b FALSE   ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus ReadSlaveDetInfo(u8 SlaveIndex)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */			
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	sprintf((char*)f_Buffer, "%s/%c.txt",SLAVE_DET_INFO_FILE_PATH, (SlaveIndex<26)?'A'+SlaveIndex:'a'+(SlaveIndex-26));
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_size(&f_fdst) != sizeof(Det.Info[0])*DET_MAX_SIZE)
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	f_lseek(&f_fdst, 0);
	res = f_read(&f_fdst, (char*)&Det.Info[0], sizeof(Det.Info[0])*DET_MAX_SIZE,&bw);//
	if ((res != FR_OK) && (bw != sizeof(Det.Info[0])*DET_MAX_SIZE))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���		
		return ERROR;
	}	
	f_close(&f_fdst);
	
	GetDetNum();
	AllRegDetRankBaseOnHoleInfo();
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	Op_Data.CurrentSlaveIndex = SlaveIndex;
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  �����
  * @param  HoleName Ҫ����Ŀ׺�
  * @param  AddDelay ��������
  * @retval 
  *    @arg @b 0 �ɹ�
  *    @arg @b 1 �׺ų�����Χ
  *    @arg @b 1 ���ڳ�����Χ  
  *    @arg @b 2 ʧ��(��Ϊ��)
  * @author ZH
  * @date   2014��12��03��
  * @note   //Ҫ����ʼ�׺ͽ�ֹ�ױ������Ѿ�ע�����׹�
  *************************************************************************************  
  */
u8 RegInserHole(u16 HoleName, u16 AddDelay)
{
    s16 i;
    u16 Min = 0xFFFF;
        
    for(i=0; i < DET_MAX_SIZE; i++)
    {
        if (GetDetRegStatus(i) != ERROR)
        {
            if (Det.Info[i].Hole.Name >= HoleName)
            {
                if(Min == 0xFFFF)
                {
                    Min = i;
                }
                Det.Info[i].Hole.Name += 1;
                if(Det.Info[i].Hole.Name > DET_MAX_SIZE)
                {
                    ReadAllDetInfo(0);
                    return 1;
                }
                
                Det.Info[i].Delay += AddDelay;
                if (Det.Info[i].Delay > Det.DlyMax)
                {
                    ReadAllDetInfo(0);
                    return 2;
                }
            }
        }
    }
    //����ע����ҲҪ��Ӧ�޸�
    for(i=DET_MAX_SIZE-1; i >= HoleName; i--)
    {
        Det.HoleNumber[i] = Det.HoleNumber[i-1];
    }
    
    if(Det.RegCount == 0)
    {
        if(HoleName == 1)
        {
            Det.HoleNumber[HoleName-1] = Det.HoleNumber[HoleName];
        }        
        else
        {
            Det.HoleNumber[HoleName-1] = Det.HoleNumber[HoleName-2];
        }     
    }
    else
    {
        u16 index;
        
        if(Min==0)
        {//�ڲ���Ŀ׺�ǰ��û����ע���
            index = 0;
        }
        else if(Min >= DET_MAX_SIZE)
        {//�ڲ���Ŀ׺ź��û����ע���
            index = Det.RegCount-1;
        }
        else
        {//�ڲ���Ŀ�ǰ����ע��ף��������������ǰһ����ע��ױ���һ��
            index = Min-1;
        }
        Det.HoleNumber[HoleName-1] = Det.HoleNumber[Det.Info[index].Hole.Name-1];
    }
    SaveAllDetInfo(0);
    return 0;
}

/**
  *************************************************************************************
  * @brief  ���ƶ�
  * @param  StartHole ��ʼ�׺�
  * @param  EndHole ��ֹ�׺�
  * @param  pos �ƶ�����(��Ϊ����ƣ���Ϊ��ǰ��)  
  * @retval 
  *    @arg @b 0 �ɹ�
  *    @arg @b 1 �׺ų�����Χ
  *    @arg @b 1 ���ڳ�����Χ  
  *    @arg @b 2 ʧ��(��Ϊ��)
  * @author ZH
  * @date   2014��12��03��
  * @note   //Ҫ����ʼ�׺ͽ�ֹ�ױ������Ѿ�ע�����׹�
  *************************************************************************************  
  */
u8 HoleMove(u16 StartHole, u16 EndHole, s16 Pos, s16 Delay)
{
	s16 i;
	s32 Value;
    u16 OldHoleName;
    
	AllRegDetRankBaseOnHoleInfo();
	if(Pos == 0)
    {//���ƿף�ֻ��Ϊ�˼Ӽ�����
		for(i=DET_MAX_SIZE-1; i >= 0; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				if ((Det.Info[i].Hole.Name >= StartHole) && (Det.Info[i].Hole.Name <= EndHole))
				{										
					Value = Det.Info[i].Delay + Delay;
					if((Value < 0) || (Value > Det.DlyMax))
					{
						return 3;
					}
					else
					{
						Det.Info[i].Delay = Value;
					}
				}
			}
		}        
    }
	else if(Pos > 0)
	{//�׺���������
		if((EndHole+Pos) > DET_MAX_SIZE)
		{
			return 1;
		}
        
		OldHoleName = 0xFFFF;
		for(i=DET_MAX_SIZE-1; i >= 0; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				if ((Det.Info[i].Hole.Name >= StartHole) && (Det.Info[i].Hole.Name <= EndHole))
				{
					Value = Det.Info[i].Hole.Name + Pos;
					if((Value <= 0) || (Value > DET_MAX_SIZE))
					{
						return 2;
					}
                    if(OldHoleName != Value)
                    {
                        OldHoleName = Value;
                        if(GetCurrentHoleNameDetCount(Value) != 0)
                        {
                            return 2;
                        }                        
                    }
					
					Det.Info[i].Hole.Name += Pos;
					
					Value = Det.Info[i].Delay + Delay;
					if((Value < 0) || (Value > Det.DlyMax))
					{
						return 3;
					}
					else
					{
						Det.Info[i].Delay = Value;
					}
				}
			}
		}
		//����ע����ҲҪ��Ӧ�޸�
		for(i=EndHole; i >= StartHole; i--)
		{
			Det.HoleNumber[i-1+Pos] = Det.HoleNumber[i-1];
		}
	}
	else
	{//�׺������ݼ�
		if(StartHole <= abs(Pos))
		{
			return 1;
		}
		
        OldHoleName = 0xFFFF;
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				if ((Det.Info[i].Hole.Name >= StartHole) && (Det.Info[i].Hole.Name <= EndHole))
				{
					Value = Det.Info[i].Hole.Name + Pos;
					if((Value <= 0) || (Value > DET_MAX_SIZE))
					{
						return 2;
					}
                    if(OldHoleName != Value)
                    {
                        OldHoleName = Value;
                        if(GetCurrentHoleNameDetCount(Value) != 0)
                        {
                            return 2;
                        }                        
                    }
					
					Det.Info[i].Hole.Name += Pos;
					Value = Det.Info[i].Delay + Delay;
					if((Value < 0) || (Value > Det.DlyMax))
					{
						return 3;
					}
					else
					{
						Det.Info[i].Delay = Value;
					}					
				}				
			}
		}
		//����ע����ҲҪ��Ӧ�޸�
		for(i=StartHole; i <= EndHole; i++)
		{
			Det.HoleNumber[i-1+Pos] = Det.HoleNumber[i-1];
		}		
	}
	SaveAllDetInfo(0);
	
	return 0;
}

/**
  *************************************************************************************
  * @brief  ��ȡGPS��Ϣ
  * @param  data Ҫ�������������ָ��
  * @retval ErrorStatus  ��ȡ���
  *    @arg @b TRUE    ��ȡ�ɹ�
  *    @arg @b FALSE   ��ȡʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus ReadOldGPSInfo(GPS_INFO* ptr)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */		
	u8 CRC16[2]={0,0};	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_open(&f_fdst, (const char*)DEV_GPS_INFO_FILENAME, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_size(&f_fdst) != (sizeof(GPS_INFO) +2))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	f_lseek(&f_fdst, 0);
	res = f_read(&f_fdst,f_Buffer, sizeof(GPS_INFO),&bw);//����һ�ε�GPS��Ϣ
	if ((res != FR_OK) && (bw != sizeof(GPS_INFO)))
	{
		f_close(&f_fdst);

		FileSystemUnlock();//�ͷ���Դ������ɶԳ���		
		return ERROR;
	}	
	
	//����CRCУ��ĳ�ֵ�����ļ�����
	GetCrc16Value(0xFFFF, &CRC16[0], DEV_GPS_INFO_FILENAME,strlen(DEV_GPS_INFO_FILENAME));		
	//���������Ƿ���ȷ
	f_Buffer[sizeof(GPS_INFO)] = CRC16[0];
	f_Buffer[sizeof(GPS_INFO)+1] = CRC16[1];	
	GetCrc16Value(0xFFFF, &CRC16[0], &f_Buffer[0], sizeof(GPS_INFO)+2);
	//�Ƚ�У��ֵ�Ƿ�һ��
	res = f_read(&f_fdst,&f_Buffer[500], 2, &bw);
	if ((res != FR_OK) && (bw != 2))
	{		
		f_close(&f_fdst);
		
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	else
	{
		if (!((f_Buffer[500] == CRC16[0]) && (f_Buffer[501] == CRC16[1])))
		{//У��ֵ�����
			f_close(&f_fdst);
			
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}	
	}
	OSSchedLock();
	memcpy((u8*)ptr, f_Buffer, sizeof(GPS_INFO));//У��ͨ��
	OSSchedUnlock();
	f_close(&f_fdst);	
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  ����GPS��Ϣ
  * @param  ptr Ҫ�������������ָ��
  * @retval ErrorStatus  ������
  *    @arg @b TRUE    ����ɹ�
  *    @arg @b FALSE   ����ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SaveGPSInfo(GPS_INFO* ptr)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
	DIR dir;	
	u8 CRC16[2]={0,0};	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_opendir(&dir, OTHER_FILE_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(OTHER_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
	res = f_open(&f_fdst, (const char*)DEV_GPS_INFO_FILENAME, FA_CREATE_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	//����CRCУ��ĳ�ֵ�����ļ����
	GetCrc16Value(0xFFFF, &CRC16[0], DEV_GPS_INFO_FILENAME,strlen(DEV_GPS_INFO_FILENAME));	
	
	memcpy(f_Buffer, (u8*)ptr, sizeof(GPS_INFO));
	res = f_write(&f_fdst, f_Buffer, sizeof(GPS_INFO), &bw);
	if ((res != FR_OK) || (bw != sizeof(GPS_INFO)))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	//������֤�����Ƿ���ȷ
	f_Buffer[sizeof(GPS_INFO)] = CRC16[0];
	f_Buffer[sizeof(GPS_INFO)+1] = CRC16[1];
	GetCrc16Value(0xFFFF, &CRC16[0], &f_Buffer[0], sizeof(GPS_INFO)+2);
	res = f_write(&f_fdst, &CRC16[0],2, &bw);
	if ((res != FR_OK) || (bw != 2))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	f_close(&f_fdst);	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
u8 GetBlastRecorderDeviceUsages(u16 index)
{
	u8 Buffer[50];
	u8 FileName[17];
	u8* FilePath;
	
	FilePath = (u8*)GetBlasterPath();
		
	memcpy(FileName, &BlastRecorder.FileName[index][0],12);
	strcpy((char*)&FileName[12],".txt");	
	if(ERROR == GetConfigFileString("ʹ�÷�ʽ", (u8*)&Buffer[0],50, FilePath, FileName))
	{
		Buffer[0] = '0';
		Buffer[1] = 0x00;
	}
	return( Buffer[0] - '0');
}

u8 GetBlastRecorderDeviceName(u16 index)
{
	u8 Buffer[50];
	u8 FileName[17];
	u8* FilePath;
	
	FilePath = (u8*)GetBlasterPath();
		
	memcpy(FileName, &BlastRecorder.FileName[index][0],12);
	strcpy((char*)&FileName[12],".txt");	
	if(ERROR == GetConfigFileString("ͨ�ŵ�ַ", (u8*)&Buffer[0],50, FilePath, FileName))
	{
		Buffer[0] = 'A';
		Buffer[1] = 0x00;
	}
	return Buffer[0];
}
	
/**
  *************************************************************************************
  * @brief  ��ʼ���𱬼�¼����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��12��06��
  * @note   �𱬼�¼���ݱ����ڽṹ��BlastRecorder��
  *************************************************************************************  
  */
void BlastRecordFileInit(void)
{
	char buffer[256];
	u8 flag_bit = 0;
	u16 j;
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif
	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	//�ӻ������Ŀ¼��ͬ	
	memset((u8*)&BlastRecorder, 0, sizeof(BLAST_RECORDER));
	
    res = f_opendir(&dir, GetBlasterPath());                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
			{                    /* It is a directory */
//				sprintf(buffer, "%s/%s/",path, fn);
//				res = f_unlink((const TCHAR *)&buffer[0]);
//				if (res != FR_OK)
//				{
//					return ERROR;
//				}
            } 
			else 
			{  /* It is a file. */
				if(strlen(fn) == strlen("141206164630.txt"))
				{//�ļ�����ʽ���
					if(0 == memcmp(fn+strlen("141206164630"), ".txt",strlen(".txt")))
					{
						for(i=0; i < strlen("141206164630");i++)
						{
							if((*(fn+i)<'0' )||(*(fn+i)>'9'))
							{
								break;
							}
						}
						if(strlen("141206164630") == i)
						{
							if(BlastRecorder.Number < BLASTER_RECORD_MAX_NUM)
							{//����Ҫ����ļ���δ����Ҫ��
//								FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//								if(ConfigFileVerify(FilePath, (u8*)fn, 0) == SUCCESS)
//								{//�ļ�У��ͨ����û�б��޸�
									memcpy(&BlastRecorder.FileName[BlastRecorder.Number][0], fn, strlen("141206164630"));
									BlastRecorder.Number++;	
//									FileSystemLock();//�����ļ���Դ������ɶԳ���
									continue;								
//								}
//								else
//								{
//									FileSystemLock();//�����ļ���Դ������ɶԳ���
//								}
							}
						}
					}
				}
				sprintf(buffer, "%s/%s",GetBlasterPath(), fn);
				res = f_unlink(buffer);
				if (res != FR_OK)
				{
					f_chmod (buffer, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
					res = f_unlink(buffer);
					if (res != FR_OK)
					{
						BlastRecorder.Number = 0;											
					}					
				}				
            }
        }
    }
	
	//�ļ����Ӵ�С����
	for(i=0;i<BlastRecorder.Number-1;i++)
	{
		flag_bit = 1;
		for(j=0;j<(BlastRecorder.Number-i-1);j++)
		{
			if(memcmp((const char *)&BlastRecorder.FileName[j+1][0], (const char *)&BlastRecorder.FileName[j][0], strlen("141206164630")) > 0)
			{
				flag_bit = 0 ;
				memcpy((char *)buffer, (const char *)&BlastRecorder.FileName[j+1][0],12);
				memcpy(&BlastRecorder.FileName[j+1][0], (const char *)&BlastRecorder.FileName[j][0], strlen("141206164630"));
				memcpy(&BlastRecorder.FileName[j][0], (char *)buffer, strlen("141206164630"));
			}			
		}
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}
/**
  *************************************************************************************
  * @brief  ��ʼ����־��¼����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��5��09��
  * @note   ��־��¼���ݱ����ڽṹ��LogRecorder��
  *************************************************************************************  
  */
void LogRecordFileInit(void)
{
	char buffer[256];
	u8 flag_bit = 0;
	u16 j;
	u8* FilePath;
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif
	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	FilePath = SYSTEM_OPERATE_INFO_PATH;
	
	memset((u8*)&LogRecorder, 0, sizeof(LOG_RECORDER));
	
    res = f_opendir(&dir, (char*)FilePath);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
			{                    /* It is a directory */
//				sprintf(buffer, "%s/%s/",path, fn);
//				res = f_unlink((const TCHAR *)&buffer[0]);
//				if (res != FR_OK)
//				{
//					return ERROR;
//				}
            } 
			else 
			{  /* It is a file. */
				if(strlen(fn) == strlen("141206.txt"))
				{//�ļ�����ʽ���
					if(0 == memcmp(fn+strlen("141206"), ".txt",strlen(".txt")))
					{
						for(i=0; i < strlen("141206");i++)
						{
							if((*(fn+i)<'0' )||(*(fn+i)>'9'))
							{
								break;
							}
						}
						if(strlen("141206") == i)
						{
							if(LogRecorder.Number < LOG_RECORD_MAX_NUM)
							{//����Ҫ����ļ���δ����Ҫ��
//								FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//								if(ConfigFileVerify(FilePath, (u8*)fn, 0) == SUCCESS)
//								{//�ļ�У��ͨ����û�б��޸�
									memcpy(&LogRecorder.FileName[LogRecorder.Number][0], fn, strlen("141206"));
									LogRecorder.Number++;	
//									FileSystemLock();//�����ļ���Դ������ɶԳ���
									continue;								
//								}
//								else
//								{
//									FileSystemLock();//�����ļ���Դ������ɶԳ���
//								}
							}
						}
					}
				}
				if ((memcmp(fn, "��ǰ.txt",strlen("��ǰ.txt")) == 0) || (memcmp(fn, "����.txt",strlen("����.txt")) == 0))
				{//�ϰ汾����־�ļ��Ȳ�Ҫɾ
					continue;
				}
				sprintf(buffer, "%s/%s",FilePath, fn);
				res = f_unlink(buffer);
				if (res != FR_OK)
				{
					f_chmod (buffer, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
					res = f_unlink(buffer);
					if (res != FR_OK)
					{
						LogRecorder.Number = 0;											
					}					
				}				
            }
        }
    }
	
	//�ļ����Ӵ�С����
	for(i=0;i<LogRecorder.Number-1;i++)
	{
		flag_bit = 1;
		for(j=0;j<(LogRecorder.Number-i-1);j++)
		{
			if(memcmp((const char *)&LogRecorder.FileName[j+1][0], (const char *)&LogRecorder.FileName[j][0], strlen("141206")) > 0)
			{
				flag_bit = 0 ;
				memcpy((char *)buffer, (const char *)&LogRecorder.FileName[j+1][0],6);
				memcpy(&LogRecorder.FileName[j+1][0], (const char *)&LogRecorder.FileName[j][0], strlen("141206"));
				memcpy(&LogRecorder.FileName[j][0], (char *)buffer, strlen("141206"));
			}			
		}
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}


ErrorStatus ReadSpecifiedHexFileSize(u8* PathName, u8* Version, u32* FileSize)
{  
    FRESULT res;         /* FatFs function common result code */

	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s.hex",PathName, Version);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	*FileSize = f_size(&f_fdst);
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}

ErrorStatus WriteHexFileData(u8* PathName, u8* Version, u8* FileData, u16 Bytes)
{  
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	DIR dir;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_opendir(&dir, (const char*)PathName);
	if (res != FR_OK)
	{//û���ļ���Ӧ��·��
		res = f_mkdir((const char*)PathName);
		if(res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}

	sprintf((char*)f_Buffer,"%s/%s.hex",PathName, Version);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	if(res == FR_OK)
	{
		f_lseek(&f_fdst,f_size(&f_fdst));
		res = f_write(&f_fdst, FileData, Bytes,&bw);
		if ((res == FR_OK) && (Bytes == bw))
		{
			f_close(&f_fdst);
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return SUCCESS;				
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;	
}
ErrorStatus GetSpecifiedHexFileCRCValue(u8* PathName, u8* Version, u16* CRCValue)
{ 
    FRESULT res; 
	u8 CRC16[2]={0xFF,0xFF};//CRCУ���ʼֵ
	UINT len = 0; 
	u8 Count = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer,"%s/%s.hex",PathName, Version);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}

	f_lseek(&f_fdst,0);
	while((FR_OK == f_read(&f_fdst, f_Buffer, sizeof(f_Buffer), &len)) && (len > 0))//ÿ��У��512�ֽ�
	{
		GetCrc16Value(((u16)CRC16[0]<<8) + CRC16[1], &CRC16[0], &f_Buffer[0],len);	
		Count++;
		if(Count >= 10)
		{
			Count = 0;
			OSTimeDly(1);		//ʱ��ϳ����������������еĻ��ᣬ��Ҫע�ⲻ��ɾ��������
		}

	}
	f_close(&f_fdst);
	
	*CRCValue = ((u16)CRC16[0]<<8) + CRC16[1];
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
ErrorStatus BlastSingleSendedResult(void)
{   
    u8 count=0;
    u16 i;
    s16 old;
    
    count = 0;
    old = BlastSingleMonitor.LinABAdc[0];
    for(i=1; i<BlastSingleMonitor.Count;i++)
    {
        if ((old >= 0) && (BlastSingleMonitor.LinABAdc[i] < 0))
        {
            old = BlastSingleMonitor.LinABAdc[i];
            count++;
        }
        else if ((old <= 0) && (BlastSingleMonitor.LinABAdc[i] > 0))
        {
            old = BlastSingleMonitor.LinABAdc[i];
            count++;
        }           
        if(count >= 3)
        {
            return SUCCESS;
        }        
    }
    return ERROR;
}
ErrorStatus WriteAccreditInfoFileForDL(u8 fileIndex, u32 StartIndex, u16 Bytes, u8* FileData)
{
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	DIR dir;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_opendir(&dir, (const char*)OTHER_FILE_PATH);
	if (res != FR_OK)
	{//û���ļ���Ӧ��·��
		res = f_mkdir((const char*)OTHER_FILE_PATH);
		if(res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
    sprintf((char*)f_Buffer,"%s/temp.bin",OTHER_FILE_PATH);
    if(Bytes == 0)
    {
        if(fileIndex == 0)
        {
            f_rename((const char*)f_Buffer, DL_ACCREDIT_INFO_FILE1_NAME);
        }
        else
        {
            f_rename((const char*)f_Buffer, DL_ACCREDIT_INFO_FILE2_NAME);
        }
        
        
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return SUCCESS;
    }
    else
    {
        if(StartIndex == 0)
        {
            f_unlink((const char*)f_Buffer);
            if(fileIndex == 0)
            {
                f_unlink(DL_ACCREDIT_INFO_FILE1_NAME);
                f_unlink(DL_ACCREDIT_INFO_FILE2_NAME);
            }
        }
        res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        if(res == FR_OK)
        {
            if(f_size(&f_fdst) < StartIndex)
            {
                FileSystemUnlock();//�ͷ���Դ������ɶԳ���
                return ERROR;
            }
            f_lseek(&f_fdst,f_size(&f_fdst));
            res = f_write(&f_fdst, FileData, Bytes,&bw);
            if ((res == FR_OK) && (Bytes == bw))
            {
                f_close(&f_fdst);
                FileSystemUnlock();//�ͷ���Դ������ɶԳ���
                return SUCCESS;				
            }
        }
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        return ERROR;        
    }	
}
/**
  *************************************************************************************
  * @brief  ����Ȩ��Ϣ�ļ�
  * @param[in]  UIDStr��UID�ַ���
  * @param[out]  Outside���ܿ���
  * @param[out]  YXQ����Ч��
  * @retval �ַ�����
  * @author ZH
  * @date   2014��9��15��
  * @note   
  *************************************************************************************  
  */
u8 GetAccreditDetInfoForDL(u8* UIDStr, u8* YXQ)
{
    FRESULT res; 
	UINT len = 0; 
    u16 OneBytes, Count;
    int low, high, mid;
    int Value;
    u8 UidLen;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
    UidLen = strlen((char*)UIDStr);
	res = f_open(&f_fdst, DL_ACCREDIT_INFO_FILE1_NAME, FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 1;//û����Ȩ�ļ�
	}

	f_lseek(&f_fdst,0);
    if(FR_OK != f_read(&f_fdst, f_Buffer, 4, &len))
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 1;
    }
    if(f_Buffer[0] != 0)//���ݸ�ʽ
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 2;//��֧�ֵ����ݸ�ʽ
    }
    OneBytes = f_Buffer[1];//ÿ���׹ܶ�Ӧ�����ݳ���:UID+6�ֽ���Ч��
    Count = (f_Buffer[2]<<8)+f_Buffer[3];//�׹�����
    if(OneBytes != UidLen+6)//ÿ���׹�UID��Ӧ����16�ֽ�, ɽ������17
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 2;//��֧�ֵ����ݸ�ʽ
    }
    f_lseek(&f_fdst, 0);
    if(f_size(&f_fdst) != (OneBytes*Count+4))
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;//�ļ�����
    }

    //���ַ�����,UID���Ѿ���ǰ����С�����˳���ź�
    low = 0;
    high = Count-1;

	while(low <= high)
    {
        mid = (low+high)/2;
        
        f_lseek(&f_fdst, 4+mid*OneBytes);
		if(FR_OK != f_read(&f_fdst, f_Buffer, OneBytes, &len))
        {
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return 1;
        }
        
        Value = memcmp(UIDStr, &f_Buffer[0], UidLen);
        if(Value == 0)
        {
            f_close(&f_fdst);
            
            if(YXQ != NULL)
            {
                memcpy(YXQ, &f_Buffer[UidLen], 6);
            }
            
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return 0; //�ҵ�����Ȩ��Ϣ 
        }
        else if(Value < 0)
        {
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
	}
	f_close(&f_fdst);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return 0xFF;//û����Ȩ
}
u8 HasAccreditInfoFileForDL(void)
{
    FRESULT res; 
	UINT len = 0; 
    u16 OneBytes, Count;
    u32 Value, allBytes;
    u16 CRCValue = 0xFFFF;
    u8 FileBuffer[33];
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_open(&f_fdst, DL_ACCREDIT_INFO_FILE1_NAME, FA_READ);
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;//û����Ȩ�ļ�
	}

	f_lseek(&f_fdst,0);
    if(FR_OK != f_read(&f_fdst, f_Buffer, 4, &len))
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;
    }
    if(f_Buffer[0] != 0)//���ݸ�ʽ
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;//��֧�ֵ����ݸ�ʽ
    }
    OneBytes = f_Buffer[1];//ÿ���׹ܶ�Ӧ�����ݳ���:UID+6�ֽ���Ч��
    Count = (f_Buffer[2]<<8)+f_Buffer[3];//�׹�����
    f_lseek(&f_fdst, 0);
    allBytes = OneBytes*Count+4;
    if(f_size(&f_fdst) != allBytes)
    {
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;//�ļ�����
    }
    Value = 0;
    OneBytes = sizeof(f_Buffer);
    while(Value < allBytes)
    {
        if ((Value+OneBytes) > allBytes)
        {
            OneBytes = allBytes-Value;
        }
        if(FR_OK != f_read(&f_fdst, f_Buffer, OneBytes, &len))
        {
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return 0;//��֧�ֵ����ݸ�ʽ
        }
        Value += len;
        
        CRCValue = GetCrc16Value(CRCValue, NULL, f_Buffer, len);
    }

	f_close(&f_fdst);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���

    if(CRCValue != GetConfigFileU32("�ļ�1У��", 0xFFFFFF, OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME))
    {
        return 0;
    }   
    memset(FileBuffer, 0, sizeof(FileBuffer));
    if(ERROR == GetConfigFileString("�������", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME))
    {
        return 0;
    }
    if(strcmp((char*)FileBuffer, (char*)device_info.mac) != 0)
    {
        return 0;
    }    
    return 1;
}
void DeleteAllAccreditErrorFile(void)
{
    u8 i;
    
    FileSystemLock();//�����ļ���Դ������ɶԳ���
    sprintf((char*)f_Buffer,"%s/temp.bin",OTHER_FILE_PATH);
    f_unlink((char*)f_Buffer);
    
    for(i=0; i<26; i++)
    {
        sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+i);
        f_unlink((char*)f_Buffer);    
    }
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}
u8 GetAccreditErrorFileCount(void)
{
    u8 i, Count;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
    Count=0;
    for(i=0; i<26;i++)
    {
        sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+i);
        if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ) == FR_OK)
        {
            Count++;
            f_close(&f_fdst);
        } 
    }
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return Count;	   
}
u8 GetAccreditErrorFileIndex(u8 ErrorIndex)
{
    u8 i, Count;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
    
    Count=0;
    for(i=0; i<26;i++)
    {
        sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+i);
        if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ) == FR_OK)
        {
            f_close(&f_fdst);
            if(ErrorIndex ==Count)
            {
                FileSystemUnlock();//�ͷ���Դ������ɶԳ���
                return i;
            }
            Count++;
        } 
    }
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return 0xFF;	   
}
u16 GetAccreditErrorCount(u8 FileIndex)
{
    u16 Count;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
    
    sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+FileIndex); 
    
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;//û����Ȩ�ļ�
	}
    Count = f_size(&f_fdst);
    f_close(&f_fdst);
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    
    if ((Count%3) != 0)
    {
        return 0;
    }
    return Count/3;	   
}

u16 GetAllAccreditErrorCount(void)
{
    u8 i;
    u16 Count = 0;
    
    if(device_info.DeviceUsages == DEVICE_USAGES_ALONE)
    {
        Count += GetAccreditErrorCount(0);
    }
    else if(device_info.DeviceUsages == DEVICE_USAGES_MASTER)
    {
        for(i=0; i<SlaveInfo.Number; i++)
        {
            Count += GetAccreditErrorCount(i);
        }    
    }

    return Count;
}
ErrorStatus WriteAccreditErrorFile(u8 FileIndex, u8* FileData, u8 Bytes)
{
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	DIR dir;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_opendir(&dir, (const char*)OTHER_FILE_PATH);
	if (res != FR_OK)
	{//û���ļ���Ӧ��·��
		res = f_mkdir((const char*)OTHER_FILE_PATH);
		if(res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
    sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+FileIndex);   

    res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if(res == FR_OK)
    {
        f_lseek(&f_fdst,f_size(&f_fdst));
        res = f_write(&f_fdst, FileData, Bytes,&bw);
        if ((res == FR_OK) && (Bytes == bw))
        {
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return SUCCESS;				
        }
    }
    f_close(&f_fdst);
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return ERROR;    
}
ErrorStatus ReadAccreditErrorFile(u8 FileIndex, u16 StartIndex, u8 Count, u8* FileData)
{
    FRESULT res;         /* FatFs function common result code */
	UINT bw;
	u16 Bytes;
    
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
    sprintf((char*)f_Buffer,"%s/temp%c.bin",OTHER_FILE_PATH, 0x41+FileIndex);   

    res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
    if(res == FR_OK)
    {
        f_lseek(&f_fdst, StartIndex*3);
        Bytes = Count*3;
        res = f_read(&f_fdst, FileData, Bytes, &bw);
        if ((res == FR_OK) && (Bytes == bw))
        {
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            return SUCCESS;				
        }
    }
    f_close(&f_fdst);
    FileSystemUnlock();//�ͷ���Դ������ɶԳ���
    return ERROR;    
}
/**
  *************************************************************************************
  * @brief  ����Ȩ�ļ��ж�����γ��
  * @param  �� 
  * @retval �Ƿ�����ɹ�
  * @date   2019��6��29��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetLocationFromAccreditFile(void)
{
    u8 FileBuffer[20];
    
    //����Ȩ�ļ��ж�����γ��
    memset(FileBuffer, 0, sizeof(FileBuffer));
    if(ERROR == GetConfigFileString("����ʱ��", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME))
    {
        return ERROR;
    }
    
    sscanf((char*)&FileBuffer[0], "%04d", (int*)&Op_Data.Gps.Year);
    sscanf((char*)&FileBuffer[4], "%02d", (int*)&Op_Data.Gps.Month);
    sscanf((char*)&FileBuffer[6], "%02d", (int*)&Op_Data.Gps.Day);
    sscanf((char*)&FileBuffer[8], "%02d", (int*)&Op_Data.Gps.Hour);
    sscanf((char*)&FileBuffer[10], "%02d", (int*)&Op_Data.Gps.Minute);
    sscanf((char*)&FileBuffer[12], "%02d", (int*)&Op_Data.Gps.Second);
    
    memset(Op_Data.Gps.Longitude, 0, sizeof(Op_Data.Gps.Longitude));
    if(ERROR == GetConfigFileString("���ص㾭��", Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME))
    {
        return ERROR;
    }

    memset(Op_Data.Gps.Latitude, 0, sizeof(Op_Data.Gps.Latitude));
    if(ERROR == GetConfigFileString("���ص�γ��", Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME))
    {
        return ERROR;
    }
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  �ж��ǲ�������Ȩ����
  * @param  �� 
  * @retval 0 ����׼������ 1 ��׼������
  * @date   2019��7��16��
  * @note   
  *************************************************************************************  
  */
u8 AccreditAreaVerifyForDL(void)
{
    u16 i, j, AllowCount, ForbidCount;
    u8 FileBuffer[30];
    u8 Key[20];
    double longitude, latitude;
    double NewLongitude, NewLatitude;
    int Radius;
    
    if((strlen((char*)Op_Data.Gps.Longitude) == 0) || (strlen((char*)Op_Data.Gps.Latitude) == 0))
    {
        return 0;
    }
    
    longitude = GPSFormatConvert(Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude));
    latitude = GPSFormatConvert(Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude));
    
    memset(FileBuffer, 0, sizeof(FileBuffer));
    AllowCount = GetConfigFileU32("׼��������", 0, OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
    ForbidCount = GetConfigFileU32("����������", 0, OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
    
    if(AllowCount == 0)
    {//û������׼������
        return 1;
    }
    for(i=0; i<AllowCount; i++)
    {
        sprintf((char*)Key, "׼����%d����", i+1);
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString(Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)FileBuffer, "%lf", (double*)&NewLongitude); 

        sprintf((char*)Key, "׼����%dγ��", i+1);
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString(Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)FileBuffer, "%lf", (double*)&NewLatitude); 

        sprintf((char*)Key, "׼����%d�뾶", i+1);
        Radius = GetConfigFileU32(Key, 0, OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        if(GetDistance(longitude, latitude, NewLongitude, NewLatitude) < Radius)
        {//��׼������
            if(ForbidCount == 0)
            {
                return 1;//û�н�����
            }
            for(j=0; j<ForbidCount; j++)
            {
                sprintf((char*)Key, "������%d����", j+1);
                memset(FileBuffer, 0, sizeof(FileBuffer));
                GetConfigFileString(Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
                sscanf((char*)FileBuffer, "%lf", (double*)&NewLongitude); 

                sprintf((char*)Key, "������%dγ��", j+1);
                memset(FileBuffer, 0, sizeof(FileBuffer));
                GetConfigFileString(Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
                sscanf((char*)FileBuffer, "%lf", (double*)&NewLatitude); 

                sprintf((char*)Key, "������%d�뾶", j+1);
                Radius = GetConfigFileU32(Key, 0, OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
                if(GetDistance(longitude, latitude, NewLongitude, NewLatitude) < Radius)
                {//��������
                    break;
                }
            }
            if(j == ForbidCount)
            {
                return 1;//���ڽ�������
            }
        }
    }
    return 0;
}

void SaveBlasteFireTime(void)
{
	BlastInfo.Time.year =  RTC_Type.year;
	BlastInfo.Time.month = RTC_Type.month;
	BlastInfo.Time.day =  RTC_Type.day;
	BlastInfo.Time.hour =  RTC_Type.hour;
	BlastInfo.Time.minute =  RTC_Type.minute;
	BlastInfo.Time.second =  RTC_Type.second;
}

/**
  *************************************************************************************
  * @brief  �����𱬼�¼�ϴ���־״̬
  * @param  State 
  *   @arg @b 0��ʾ����Ϊδ�ϴ�  
  *   @arg @b ����ֵ��ʾ����Ϊ���ϴ�   
  * @retval ErrorStatus  ������
  *    @arg @b TRUE    д��ɹ�
  *    @arg @b FALSE   д��ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus UpdateBlasterRecordUploadState(u8 State, u16 Index)
{
	u8 FileName[17];
	u8 CRC16[2]={0xFF,0xFF}, Flag= 0;	
	u16 len;
	DWORD FileIndex = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	if(Index == 0xFFFF)
	{//��ǰ�𱬼�¼
		sprintf((char*)f_Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", 
			GetBlasterPath(), BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);
	}
	else
	{
		if(Index >= BlastRecorder.Number)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
		memcpy(FileName, &BlastRecorder.FileName[Index][0],12);
		strcpy((char*)&FileName[12],".txt");	
		sprintf((char*)f_Buffer,"%s/%s",GetBlasterPath(), FileName);
	}				

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) == FR_OK)
	{	
		f_lseek(&f_fdst,0);
		while(NULL != f_gets((char*)f_Buffer, 250, &f_fdst))
		{
			if((Flag == 0) && (memcmp(&f_Buffer[0], "[��Ϣ�ϴ���־]=", strlen((const char*)"[��Ϣ�ϴ���־]=")) == 0))
			{
				if(State == 0)
				{
					sprintf((char*)&f_Buffer[0],"[��Ϣ�ϴ���־]=δ�ϴ�%04d%02d%02d%02d%02d%02d\n",
						RTC_Type.year, RTC_Type.month, RTC_Type.day, RTC_Type.hour,
						RTC_Type.minute, RTC_Type.second);					
				}
				else
				{
					sprintf((char*)&f_Buffer[0],"[��Ϣ�ϴ���־]=���ϴ�%04d%02d%02d%02d%02d%02d\n",
						RTC_Type.year, RTC_Type.month, RTC_Type.day, RTC_Type.hour,
						RTC_Type.minute, RTC_Type.second);				
				}
				f_lseek(&f_fdst, FileIndex);
				f_puts((const char*)f_Buffer, &f_fdst);
				
				Flag++;
				len = strlen((const char*)f_Buffer);
				GetCrc16Value((CRC16[0]<<8) + CRC16[1], &CRC16[0], &f_Buffer[0],len);
			}
			else if ((Flag == 1)&& ((strlen((const char*)f_Buffer) == 6) && (f_Buffer[0] == ':') && (f_Buffer[5] == '\n')))
			{//��У��ֵ����д���ļ���
				sprintf((char*)&f_Buffer[0],":%02X%02X\n", CRC16[0], CRC16[1]);
				
				f_lseek(&f_fdst, FileIndex);
				f_puts((const char*)f_Buffer, &f_fdst);
				
				Flag++;
				break;
			}
			else
			{
				len = strlen((const char*)f_Buffer);
				GetCrc16Value((CRC16[0]<<8) + CRC16[1], &CRC16[0], &f_Buffer[0],len);
			}	
			FileIndex = f_tell(&f_fdst);
		}
		f_close(&f_fdst);	 		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	if(Flag == 2)
	{		
		return SUCCESS;
	}
	return ERROR;	
}
ErrorStatus OpenBlasterRecordFile(void)
{
    FileSystemLock();//�����ļ���Դ������ɶԳ���
    
	sprintf((char*)f_Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", GetBlasterPath(), BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) != FR_OK)
	{
		return ERROR;
	}
	
	f_lseek(&f_fdst,f_size(&f_fdst));
    return SUCCESS;
}
ErrorStatus AddEndFlagToBlasterRecord(u8* CurrentCRC16)
{
	u16 len;
	u8 CRC16[2]={0xFF,0xFF};

	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	CRC16[0] = *CurrentCRC16;
	CRC16[1] = *(CurrentCRC16+1);
	
	//�ӻ������Ŀ¼��ͬ
	sprintf((char*)f_Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", GetBlasterPath(), BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	f_lseek(&f_fdst,f_size(&f_fdst));
	
	/***������Ϣ�ϴ���־***/
	sprintf((char*)&f_Buffer[0],"[��Ϣ�ϴ���־]=δ�ϴ�%04d%02d%02d%02d%02d%02d\n",
		RTC_Type.year, RTC_Type.month, RTC_Type.day, RTC_Type.hour,
		RTC_Type.minute, RTC_Type.second);	//14���ո�ռλ���ڱ����ϴ�ʱ��
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	//��У��ֵд���ļ���
	sprintf((char*)&f_Buffer[0], ":%02X%02X\n", CRC16[0], CRC16[1]);
	f_puts((const char*)f_Buffer,&f_fdst);	
	f_close(&f_fdst);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}

ErrorStatus AddCurrentDetInfoToBlasterRecord(u8* CRC16, u8 Name)
{
	u16 i,len;
	u8 SlaveIndex;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	

	SlaveIndex = GetIndexFromAddrName(Name);
	
	sprintf((char*)f_Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", GetBlasterPath(), BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);

	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_WRITE|FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	f_lseek(&f_fdst,f_size(&f_fdst));
	
	/********************************��������ÿ����������Ӧ1��**********************************/
	memset(f_Buffer,0,sizeof(f_Buffer));
	sprintf((char*)&f_Buffer[0], "[%c������ID]=", Name);
	len = strlen((const char*)f_Buffer);
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		memcpy(&f_Buffer[len],SlaveInfo.Info[SlaveIndex].ID, sizeof(SlaveInfo.Info[0].ID));
	}
	else
	{
		memcpy(&f_Buffer[len],device_info.mac, sizeof(device_info.mac));	
	}
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***��������������汾***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	sprintf((char*)&f_Buffer[0], "[%c����汾]=", Name);
	len = strlen((const char*)f_Buffer);
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		memcpy(&f_Buffer[len],SlaveInfo.Info[SlaveIndex].Version, sizeof(SlaveInfo.Info[0].Version));
	}
	else
	{
		memcpy(&f_Buffer[len], VERSION_STRING, strlen(VERSION_STRING));	
	}
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);	
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***����ע���׹���***/
	sprintf((char*)&f_Buffer[0], "[%cע���׹���]=%u\n", Name, Det.RegCount);	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***��������׹���***/
	sprintf((char*)&f_Buffer[0], "[%c�����׹���]=%u\n", Name, Det.ErrorCount);		
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***�����׹�����***/
	sprintf((char*)&f_Buffer[0], "[%c�׹�����]=0x%02X(%ums)\n", Name, Det.Type, Det.DlyMax);
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

	/***�׹ܲ���ǰ���ߵ�ѹ***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ���ǰ���ߵ�ѹ]=%umV\n", Name,SlaveInfo.Info[SlaveIndex].V_BeforeTest);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ���ǰ���ߵ�ѹ]=%umV\n", Name,BlastInfo.V_BeforeTest);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***�׹ܲ���ǰ���ߵ���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ���ǰ���ߵ���]=%uuA\n", Name,SlaveInfo.Info[SlaveIndex].I_BeforeTest);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ���ǰ���ߵ���]=%uuA\n", Name,BlastInfo.I_BeforeTest);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***�׹ܲ��Ժ����ߵ���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c����©����]=%uuA\n", Name,SlaveInfo.Info[SlaveIndex].I_Leak);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c����©����]=%uuA\n", Name,BlastInfo.I_Leak);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

	/***��·���ǰ���ߵ�ѹ***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ��Ժ����ߵ�ѹ]=%umV\n", Name,SlaveInfo.Info[SlaveIndex].V_BeforeCharge);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ��Ժ����ߵ�ѹ]=%umV\n", Name,BlastInfo.V_BeforeCharge);	
	}	

	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***��·���ǰ���ߵ���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ��Ժ����ߵ���]=%uuA\n", Name,SlaveInfo.Info[SlaveIndex].I_BeforeCharge);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c�׹ܲ��Ժ����ߵ���]=%uuA\n", Name,BlastInfo.I_BeforeCharge);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***��·�������ߵ�ѹ***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ�ѹ]=%umV\n", Name,SlaveInfo.Info[SlaveIndex].V_AfterCharge);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ�ѹ]=%umV\n", Name,BlastInfo.V_AfterCharge);	
	}	

	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***��·�������ߵ���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ���]=%uuA\n", Name,SlaveInfo.Info[SlaveIndex].I_AfterCharge);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ���]=%uuA\n", Name,BlastInfo.I_AfterCharge);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);		
	
	/***��ǰ���ߵ�ѹ***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c��ǰ���ߵ�ѹ]=%umV\n", Name,SlaveInfo.Info[SlaveIndex].V_BeforeFire);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c��ǰ���ߵ�ѹ]=%umV\n", Name,BlastInfo.V_BeforeFire);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***��ǰ���ߵ���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[%c��ǰ���ߵ���]=%uuA\n", Name,SlaveInfo.Info[SlaveIndex].I_BeforeFire);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[%c��ǰ���ߵ���]=%uuA\n", Name,BlastInfo.I_BeforeFire);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);		

	/***��·�������ߵ�����Сֵ***/
	sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ�����Сֵ]=%uuA\n", Name,BlastInfo.I_AfterChargeMin);
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	
	
	/***��·�������ߵ������ֵ***/
	sprintf((char*)&f_Buffer[0], "[%c��·�������ߵ������ֵ]=%uuA\n", Name,BlastInfo.I_AfterChargeMax);	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);		
	
	/***�����׹���Ϣ***/	
	sprintf((char*)f_Buffer,"[%c�׹���Ϣ����]=�׺�\t�׹�����\tƵ��\t����\t״̬��\t\n", Name);
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);	

	AllRegDetRankBaseOnHoleInfo();
	for(i=0;i<Det.RegCount;i++)
	{
		memset(f_Buffer, 0, sizeof(f_Buffer));
		len = sprintf((char*)f_Buffer,"[%c�׹���Ϣ%d]=\t%d-%d\t%02X%02X%02X%02X%02X%02X%02X%02X\t%ld\t%d\t%02X\t\n", Name,i+1, 
			Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside,
			Det.Info[i].Serl.U8Buffer[0],Det.Info[i].Serl.U8Buffer[1], 
            Det.Info[i].Serl.U8Buffer[2],Det.Info[i].Serl.U8Buffer[3],
			Det.Info[i].Pwd.U8Buffer[0],Det.Info[i].Pwd.U8Buffer[1], 
            Det.Info[i].Pwd.U8Buffer[2],Det.Info[i].Pwd.U8Buffer[3],
			Det.Info[i].Freq.U32Value,Det.Info[i].Delay, Det.Info[i].Err);
			
		f_puts((const char*)f_Buffer, &f_fdst);
		//��������У׼ֵ
		len = strlen((const char*)f_Buffer);
		GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);			
	}
	
	f_close(&f_fdst);

	FileSystemUnlock();//�ͷ���Դ������ɶԳ���	
	return SUCCESS;
}
ErrorStatus AddDetInfoToBlasterRecord(u8* CRC16)
{
	u8 i;
	
	//���������ģʽ���հ�˳������ӻ���������Ϣ�ٱ���
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		for(i=0; i<SlaveInfo.Number; i++)
		{
			if(ReadSlaveDetInfo(i) == ERROR)
			{
				return ERROR;
			}
			Det.Type = SlaveInfo.Info[i].Type;
			Det.DlyMax = GetDetMaxDelay(Det.Type);
						
			if(AddCurrentDetInfoToBlasterRecord(CRC16, GetAddrNameFromIndex(i)) == ERROR)
			{
				return ERROR;
			}		
		}	
	}
	else
	{
		if(AddCurrentDetInfoToBlasterRecord(CRC16, device_info.name) == ERROR)
		{
			return ERROR;
		}	
	}
	
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  �����𱬼�¼
  * @param  ��
  * @retval ErrorStatus  ������
  *    @arg @b TRUE    д��ɹ�
  *    @arg @b FALSE   д��ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus WriteBlasterRecord(void)
{
    FRESULT res;         /* FatFs function common result code */
	DIR dir;	
	u16 len;
	u8 CRC16[2]={0xFF,0xFF};	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
		
	res = f_opendir(&dir, GetBlasterPath());
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(GetBlasterPath());
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
	res = f_chmod (GetBlasterPath(), AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
	if (res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(BlastRecorder.Number >= BLASTER_RECORD_MAX_NUM)
	{//�𱬼�¼����
		memset((char*)f_Buffer, 0, sizeof(f_Buffer));
		sprintf((char*)f_Buffer, "%s/", GetBlasterPath());
		memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], (char*)&BlastRecorder.FileName[BLASTER_RECORD_MAX_NUM-1], sizeof(BlastRecorder.FileName[0]));
		strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
		f_unlink((const char*)f_Buffer);
		BlastRecorder.Number--;
	}
	
	sprintf((char*)f_Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", GetBlasterPath(), 
		BlastInfo.Time.year-2000, BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);		

	res = f_open(&f_fdst, (const char*)f_Buffer, FA_CREATE_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	/***������ʱ������汾***/
	sprintf((char*)&f_Buffer[0], "[����汾]=%s\n", VERSION_STRING);
	f_puts((const char*)f_Buffer, &f_fdst);
	
	//����У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

	/***���浱ǰ�豸ʹ�÷�ʽ***/
	sprintf((char*)&f_Buffer[0], "[ʹ�÷�ʽ]=%d\n", device_info.DeviceUsages);	
	f_puts((const char*)f_Buffer, &f_fdst);
 
	//����У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
    
	/***���浱ǰ�豸���� 2019��6��29��***/
	sprintf((char*)&f_Buffer[0], "[�豸����]=%d\n", device_info.DeviceType);	
	f_puts((const char*)f_Buffer, &f_fdst);	
    
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
    if((DEVICE_USAGES_SLAVE != device_info.DeviceUsages) && (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType))
    {// 2019��7��1��
        u8 FileBuffer[61];//���30������
        u8 i;
        u16 U16Value;
        
        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("��������", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[��������]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 
        
        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("��������", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[��������]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("��Ŀ���", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[��Ŀ���]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("��ͬ���", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[��ͬ���]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("���֤��", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[���֤��]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("��λ����", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
               
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[��λ����]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 
        
        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("׼��������", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)FileBuffer, "%d", (int*)&U16Value);       
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[׼��������]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 

        for(i=0; i<U16Value;i++)
        {
            char Key[20];
            
            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "׼����%d����", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "׼����%d����", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 

            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "׼����%dγ��", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "׼����%d�뾶", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);            
        }

        /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("����������", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)FileBuffer, "%d", (int*)&U16Value);       
        OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
        
        /***���浱ǰ�豸����***/
        sprintf((char*)&f_Buffer[0], "[����������]=%s\n", (char*)FileBuffer);	
        f_puts((const char*)f_Buffer, &f_fdst);	
        
        //��������У׼ֵ
        len = strlen((const char*)f_Buffer);
        GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 

        for(i=0; i<U16Value;i++)
        {
            char Key[20];
            
            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "������%d����", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len); 

            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "������%dγ��", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

            /***�رյ�ǰ�ļ�, ����һ���ļ���������***/
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            memset(FileBuffer, 0, sizeof(FileBuffer));
            sprintf((char*)Key, "������%d�뾶", i+1);
            GetConfigFileString((u8*)Key, FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);     
            OpenBlasterRecordFile();//���´��𱬼�¼�ļ�
            
            /***���浱ǰ�豸����***/
            sprintf((char*)&f_Buffer[0], "[%s]=%s\n", Key, (char*)FileBuffer);	
            f_puts((const char*)f_Buffer, &f_fdst);	
            
            //��������У׼ֵ
            len = strlen((const char*)f_Buffer);
            GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);            
        }         
    }
	/***���浱ǰ�豸���ַ***/
	sprintf((char*)&f_Buffer[0], "[ͨ�ŵ�ַ]=%c\n", device_info.name);	
	f_puts((const char*)f_Buffer, &f_fdst);
	
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***��������ID***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[����ID]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len],device_info.mac, sizeof(device_info.mac));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***��������ID***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[����Ӳ���汾]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len],device_info.hw_version, sizeof(device_info.hw_version));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***������ԿID***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��ԿID]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len],BlastInfo.DkeyInfo.Mac, sizeof(BlastInfo.DkeyInfo.Mac));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

	/***������ԿID***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��Կ����汾]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len],BlastInfo.DkeyInfo.Version, sizeof(BlastInfo.DkeyInfo.Version));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);

	/***������ԿID***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��ԿӲ���汾]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len],BlastInfo.DkeyInfo.HwVersion, sizeof(BlastInfo.DkeyInfo.HwVersion));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***���湤������***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��Կ��Ȩ��]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len], BlastInfo.DkeyInfo.License, sizeof(BlastInfo.DkeyInfo.License));	
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***�����û���***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[�û���]=");
	len = strlen((const char*)f_Buffer);
	memcpy(&f_Buffer[len], BlastInfo.DkeyInfo.Operator, sizeof(BlastInfo.DkeyInfo.Operator));
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***�ϴ���������***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[�ϴ���������]=");
	len = strlen((const char*)f_Buffer);
	if((BlastInfo.DkeyInfo.UploadType >= '0') && (BlastInfo.DkeyInfo.UploadType <= '9'))
	{
		f_Buffer[len++] = BlastInfo.DkeyInfo.UploadType;	
	}
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***������ʱ��***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��ʱ��]=");
	len = strlen((const char*)f_Buffer);
	sprintf((char*)&f_Buffer[len], "%04d%02d%02d%02d%02d%02d", BlastInfo.Time.year, 
		BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);
	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;		
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***���澭γ������***/
	memset(f_Buffer,0,sizeof(f_Buffer));
	strcpy((char*)&f_Buffer[0], "[��γ������]=");
	len = strlen((const char*)f_Buffer);
	if((strlen((char*)Op_Data.Gps.Longitude) >1) && (strlen((char*)Op_Data.Gps.Latitude) > 1))
	{
		memcpy(&f_Buffer[len], Op_Data.Gps.Longitude, strlen((char*)Op_Data.Gps.Longitude));//���ȣ����ݸ�ʽ��һ��
		len += strlen((char*)Op_Data.Gps.Longitude);

		f_Buffer[len++] = ',';
		memcpy(&f_Buffer[len], Op_Data.Gps.Latitude, strlen((char*)Op_Data.Gps.Latitude));//γ�ȣ����ݸ�ʽ��һ��
		len += strlen((char*)Op_Data.Gps.Latitude);	
	}
//	else if(Op_Data.OldGps.GPS_state != GPS_FAILURE)
//	{
//		memcpy(&f_Buffer[len], Op_Data.OldGps.GPS_Longitude, 10);//���ȣ����ݸ�ʽ��һ��
//		len += 10;
//		f_Buffer[len++] = Op_Data.OldGps.GPS_Longitude[11];
//		f_Buffer[len++] = ',';
//		memcpy(&f_Buffer[len], Op_Data.OldGps.GPS_Latitude, 9);//γ�ȣ����ݸ�ʽ��һ��
//		len += 9;	
//		f_Buffer[len++] = Op_Data.OldGps.GPS_Latitude[10];		
//	}

	len = strlen((const char*)f_Buffer);
	f_Buffer[len++] = '\n';
	f_Buffer[len++] = 0x00;	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***����ע���׹���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[ע���׹�����]=%d\n", SlaveInfo.RegCount);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[ע���׹�����]=%d\n", Det.RegCount);
	}
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	/***��������׹���***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[�����׹�����]=%d\n", SlaveInfo.ErrorCount);
	}
	else
	{
		sprintf((char*)&f_Buffer[0], "[�����׹�����]=%d\n", Det.ErrorCount);
	}	
	
	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	
	/***��������������***/
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		sprintf((char*)&f_Buffer[0], "[����������]=%d\n", SlaveInfo.Number);
	}
	else
	{
		strcpy((char*)&f_Buffer[0], "[����������]=1\n");	
	}

	f_puts((const char*)f_Buffer, &f_fdst);
	//��������У׼ֵ
	len = strlen((const char*)f_Buffer);
	GetCrc16Value((CRC16[0]<<8)+CRC16[1], &CRC16[0], &f_Buffer[0],len);
	
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	/********************************��������ÿ����������Ӧ1��**********************************/
	if(AddDetInfoToBlasterRecord(&CRC16[0]) == ERROR)
	{
		return ERROR;	
	}
	
	//����У��ֵ
	if(AddEndFlagToBlasterRecord(&CRC16[0]) == ERROR)
	{
		return ERROR;
	}	
	
	BlastRecordFileInit();

	return SUCCESS;	
}
/**
  *************************************************************************************
  * @brief  ��һ����־�ļ���С
  * @param  PathName ·�� 
  * @retval ���
  * @author ZH
  * @date   2018��05��10��
  * @note   
  *************************************************************************************  
  */
u32 GetLogFileSize(u8 index)
{
	u8 Buffer[30];
	u32 fileSize = 0;

	if(index >= LogRecorder.Number)
	{
		return 0;
	}
	memset((char*)Buffer, 0, sizeof(Buffer));
	sprintf((char*)Buffer, "%s/", SYSTEM_OPERATE_INFO_PATH);
	memcpy((char*)&Buffer[strlen((char*)Buffer)], (char*)&LogRecorder.FileName[index][0], sizeof(LogRecorder.FileName[0]));
	strcpy((char*)&Buffer[strlen((char*)Buffer)], ".txt");
	
	if(GetFileSize((char*)Buffer, &fileSize) == ERROR)
	{
		return 0;
	}
	return fileSize;
}

/**
  *************************************************************************************
  * @brief  ����ϵͳ������־
  * @param  data Ҫ�������������ָ��
  * @param  len   Ҫ��������ݳ���
  * @param  op_type   Ҫ��������ݸ�ʽ
  * @retval ErrorStatus  ������
  *    @arg @b TRUE    д��ɹ�
  *    @arg @b FALSE   д��ʧ�� 
  * @author ZH
  * @date   2014��12��03��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus WriteSystemLog(void* data, u16 len, DATE_WRITE_TYPE op_type)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
	DIR dir;	
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	u16 i,j, w_size, w_start=0;
	OS_STK_DATA StkData;
	u8 NewFile = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStruct);
	
	res = f_opendir(&dir, SYSTEM_OPERATE_INFO_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(SYSTEM_OPERATE_INFO_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
	res = f_chmod (SYSTEM_OPERATE_INFO_PATH, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
	if (res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	sprintf((char*)f_Buffer, "%s/%02x%02x%02x.txt", SYSTEM_OPERATE_INFO_PATH, 
		RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date);		
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	if(f_size(&f_fdst) == 0)
	{//���ļ�
		NewFile = 1;
		if(LogRecorder.Number >= LOG_RECORD_MAX_NUM)
		{
			u8 deleteIndex = LOG_RECORD_MAX_NUM-1;
			s16 index;
			
			if(RTC_DateStruct.RTC_Year < 0x18)
			{//��RTC������ɵ�ʱ�䲻׼�������⴦��
				for(index=LOG_RECORD_MAX_NUM-1; index>=0; index--)
				{
					if(memcmp((char*)&LogRecorder.FileName[index], "18", 2) >= 0)
					{
						deleteIndex = index;
						break;
					}
				}
			}

			memset((char*)f_Buffer, 0, sizeof(f_Buffer));
			sprintf((char*)f_Buffer, "%s/", SYSTEM_OPERATE_INFO_PATH);
			memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], (char*)&LogRecorder.FileName[deleteIndex], sizeof(LogRecorder.FileName[0]));
			strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
			f_unlink((const char*)f_Buffer);
			LogRecorder.Number--;
		}					
	}
	f_lseek(&f_fdst,f_size(&f_fdst));
	if(NEW_STR_INFO == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);			
		
		res = f_write(&f_fdst, data, len, &bw);		
	}
	else if(STR_INFO == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
		f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
		res = f_write(&f_fdst, data, len, &bw);
	}
	else if(U16_DATA == op_type)
	{
		f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
		f_printf(&f_fdst, "%d", *((u16*)data));            
	}
	else if(HEX_ARRAY == op_type)
	{
		if(len > 50)
		{
			w_size = 50;
			w_start = len-50;
			sprintf((char*)f_Buffer,"\r\n����%d�ֽ�,����Ϊ��50�ֽ�:\r\n", len);	
		}
		else
		{
			w_size = len;
			sprintf((char*)f_Buffer,"\r\n����%d�ֽ�:\r\n", len);		
		}
		f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
		j = 0;
		for(i=0;i<w_size;i++)
		{
			j += sprintf((char*)&f_Buffer[j], "%02X ", *((u8*)data+w_start+i));
		}
		f_write(&f_fdst, (char*)f_Buffer, j, &bw);
	}
	else if(HEX_ARRAY_NO_TITLE == op_type)
	{
		if(len > 50)
		{
			w_size = 50;
		}
		else
		{
			w_size = len;
		}
		j = 0;
		for(i=0;i<w_size;i++)
		{
			j += sprintf((char*)&f_Buffer[j], "%02X ", *((u8*)data+i));
		}
		f_write(&f_fdst, "\r\n", strlen("\r\n"), &bw);
		f_write(&f_fdst, (char*)f_Buffer, j, &bw);
	}	
	else if(H32_ARRAY == op_type)
	{       
		f_write(&f_fdst, "\r\n", strlen("\r\n"), &bw);

		for(i=0;i<len;i++)
		{
            f_printf(&f_fdst, "%lu ", *((u32*)data+i));
		}		
	}
	else if(TEST_INFO == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		
		f_write(&f_fdst, data, len, &bw);
		
		sprintf((char*)f_Buffer,"\r\n���ߵ�ѹ%dmV\r\n���ߵ���%duA", GetBusVoltage(10,NULL), GetBusCurrent(10,NULL));   
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
        
        sprintf((char*)f_Buffer,"\r\n�豸��ַ%c", device_info.name);
        f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		
		sprintf((char*)f_Buffer,"\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);			
		
		f_write(&f_fdst, "\r\n����汾:", strlen("\r\n����汾:"), &bw);	
		res = f_write(&f_fdst, VERSION_STRING, strlen(VERSION_STRING), &bw);	

		if(Det.ErrorCount>0)
		{
			sprintf((char*)f_Buffer,"\r\nע���׹�%d, �����׹�:%d\r\n�����������:",Det.RegCount, Det.ErrorCount);
			f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
			w_size = 0;
			for(i=0;i<Det.ErrorCount;i++)
			{
				j =  GetErrorDetIndex(i);
				w_size += sprintf((char*)&f_Buffer[w_size], "\r\n%d\t%d-%d\t%02X%02X%02X%02X%02X%02X%02X%02X\t%02X",
                    i+1,
                    Det.Info[j].Hole.Name, Det.Info[j].Hole.Inside, 
                    Det.Info[j].Serl.U8Buffer[0],Det.Info[j].Serl.U8Buffer[1],Det.Info[j].Serl.U8Buffer[2],Det.Info[j].Serl.U8Buffer[3],
                    Det.Info[j].Pwd.U8Buffer[0],Det.Info[j].Pwd.U8Buffer[1],Det.Info[j].Pwd.U8Buffer[2],Det.Info[j].Pwd.U8Buffer[3],
                    Det.Info[j].Err);
				if(w_size>400)
				{
					f_write(&f_fdst, (char*)f_Buffer, w_size, &bw);
					w_size = 0;
				}
			}
			if(w_size>0)
			{
				f_write(&f_fdst, (char*)f_Buffer, w_size, &bw);
			}			
		}
		else
		{
			sprintf((char*)f_Buffer,"\r\nע���׹�%d, �����׹�:%d",Det.RegCount, Det.ErrorCount);
			f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
		}
		if(GetRetryErrCount() != 0)
		{
			u16 RetryErrCount = 0;
			
			sprintf((char*)f_Buffer,"\r\n\r\n%d���׹ܴ�������:", GetRetryErrCount());
			f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
			w_size = 0;
			
			for(i=0; i < DET_MAX_SIZE; i++)
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if(Det.Info[i].RetryErr != 0)
					{
						RetryErrCount++;
						
						w_size += sprintf((char*)&f_Buffer[w_size], "\r\n%d\t%d-%d\t%02X%02X%02X%02X%02X%02X%02X%02X\t%02X", 
                            RetryErrCount,
                            Det.Info[i].Hole.Name, Det.Info[i].Hole.Inside, 
                            Det.Info[i].Serl.U8Buffer[0],Det.Info[i].Serl.U8Buffer[1],Det.Info[i].Serl.U8Buffer[2],Det.Info[i].Serl.U8Buffer[3],
                            Det.Info[i].Pwd.U8Buffer[0],Det.Info[i].Pwd.U8Buffer[1],Det.Info[i].Pwd.U8Buffer[2],Det.Info[i].Pwd.U8Buffer[3],
                            Det.Info[i].RetryErr);
						if(w_size>400)
						{
							f_write(&f_fdst, (char*)f_Buffer, w_size, &bw);
							w_size = 0;
						}					
					}
				}
			}
			if(w_size>0)
			{
				f_write(&f_fdst, (char*)f_Buffer, w_size, &bw);
			}			
		}
		f_write(&f_fdst, "\r\n��������(uA)\r\n", strlen("\r\n��������(uA)\r\n"), &bw);
		for(i=0; i<DET_WAVE_POINT_COUNT;i++)
		{
			int value;
			value = DetWave[i];
			
			if((u16)(adc_adjust.low1current.k*1000) == 0)
			{
				value = (s16)(value*DEFAULT_I_LOW1_PARAM);
			}
			else
			{
				if(adc_adjust.low1current.b&0x8000)
				{
					value = (s16)((float)value*adc_adjust.low1current.k - (adc_adjust.low1current.b&0x7FFF));
				}
				else
				{
					value = (s16)((float)value*adc_adjust.low1current.k + (adc_adjust.low1current.b&0x7FFF));
				}		
			}
			if((i+1) == DET_WAVE_POINT_COUNT)
			{
				f_printf(&f_fdst, "%d\r\n", value); 
			}
			else
			{
				f_printf(&f_fdst, "%d, ", value); 
			}			
		}
		f_write(&f_fdst, "\r\n", strlen("\r\n"), &bw);	
		
	}
	else if(DEV_START == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
		
		f_write(&f_fdst, "\r\n�豸����", strlen("\r\n�豸����"), &bw);	
		f_write(&f_fdst, "\r\n����汾:", strlen("\r\n����汾:"), &bw);	
		f_write(&f_fdst, VERSION_STRING, strlen(VERSION_STRING), &bw);
		f_write(&f_fdst, "\r\nӲ���汾:", strlen("\r\nӲ���汾:"), &bw);	
		f_write(&f_fdst, device_info.hw_version, strlen((char*)device_info.hw_version), &bw);
		f_write(&f_fdst, "\r\n�豸���:", strlen("\r\n�豸���:"), &bw);	
		f_write(&f_fdst, device_info.mac, strlen((char*)device_info.mac), &bw);
        
        f_write(&f_fdst, "\r\nSN:", strlen("\r\nSN:"), &bw);
        GetSTM32ChipIDStr(f_Buffer);
        f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);		
		
		if(DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType)
		{
			sprintf((char*)f_Buffer,"\r\n���ܰ�");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);		
		}
		else if(DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)
		{
			sprintf((char*)f_Buffer,"\r\n�����𱬰�");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);		
		}
		else
		{
			sprintf((char*)f_Buffer,"\r\n��Կ��");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		}
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			sprintf((char*)f_Buffer,"\r\nʹ��ģʽ:������");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		}
		else if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
		{
			sprintf((char*)f_Buffer,"\r\nʹ��ģʽ:������");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		}
		else if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
		{
			sprintf((char*)f_Buffer,"\r\nʹ��ģʽ:������");
			f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		}
		sprintf((char*)f_Buffer,"\r\nע���׹�%d",Det.RegCount);
		f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);		
	}
	else if(TASK_DATA == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
		if ((data != NULL) && (len != 0))
		{
			f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
			res = f_write(&f_fdst, data, len, &bw);		
		}

		f_write(&f_fdst, "\r\n�����ջʹ�����:", strlen("\r\n�����ջʹ�����:"), &bw);
		for(i=4; i<OS_LOWEST_PRIO-2;i++)
		{
			if(OS_ERR_NONE == OSTaskStkChk(i, &StkData))
			{
				f_printf(&f_fdst, "\r\n���񼶱�:%d, ����:%d, ��ʹ��:%d", i, StkData.OSFree, StkData.OSUsed); 
			}		
		}	
	}
	else if(BLAST_SINGLE_DATA == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
		if ((data != NULL) && (len != 0))
		{
			f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
			res = f_write(&f_fdst, data, len, &bw);		
		}
        
		sprintf((char*)f_Buffer,"\r\n�������ź�ʱ�˿ڵ�ѹ�������(%d�������1ms):",BlastSingleMonitor.Count);
		res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	

		for(i=0; i<BlastSingleMonitor.Count;i++)
		{
			sprintf((char*)f_Buffer,"\r\n��ѹֵ:%dmV", GetBusVoltageFromAdcValue(BlastSingleMonitor.LinABAdc[i])); 
            f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);            
		}

		sprintf((char*)f_Buffer,"\r\n------Start------\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);

		sprintf((char*)f_Buffer,"%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
        
		sprintf((char*)f_Buffer,"\r\n����״̬��%dmV %duA", GetBusVoltage(10,NULL), GetBusCurrent(10,NULL));   
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);        
	}
	else if(CURRENT_STATUS == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		
		f_write(&f_fdst, data, len, &bw);
		
		sprintf((char*)f_Buffer,"\r\n���ߵ�ѹ%dmV\r\n���ߵ���%duA", Op_Data.LinABVoltage, Op_Data.LinABCurrent);   
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
        
		sprintf((char*)f_Buffer,"\r\n%02x��%02x��%02x��%02xʱ%02x��%02x��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);			
		
		sprintf((char*)f_Buffer,"\r\nע���׹�%d, �����׹�:%d",Det.RegCount, Det.ErrorCount);
		f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);		
	}	
	else if(BLAST_RECORDER_COPY == op_type)
	{
        u8 FileBuffer[50];
        u16 FileIndex, ReadBytes;
        u32 FileSize;
        u32 ReadIndex;
        
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		
		sprintf((char*)f_Buffer,"�𱬼�¼�������£�\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);

        f_close(&f_fdst);
        FileSystemUnlock();//�ͷ���Դ������ɶԳ���
        
		//�����𱬼�¼����
        sprintf((char*)FileBuffer, "%02d%02d%02d%02d%02d%02d",BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);
        
        FileIndex = GetBlastRecorderIndexFromFileName(FileBuffer);
        if(FileIndex >= BlastRecorder.Number)
        {
            return ERROR;
        }
        FileSize = GetBlastRecorderFileSize(FileIndex);
        
        ReadIndex = 0;
        ReadBytes = sizeof(f_Buffer);
        
        while(ReadIndex < FileSize)
        {
            if ((ReadIndex + ReadBytes) > FileSize)
            {
                ReadBytes = FileSize - ReadIndex;
            }
            //�����𱬼�¼����
            if(ERROR == GetBlastRecorderData(FileIndex, ReadIndex, ReadBytes, f_Buffer))
            {
                return ERROR;
            }
            
            //���´���־�ļ������𱬼�¼����д����־�ļ�
            FileSystemLock();//�����ļ���Դ������ɶԳ���
            sprintf((char*)FileBuffer, "%s/%02x%02x%02x.txt", SYSTEM_OPERATE_INFO_PATH, 
                RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date);		
            res = f_open(&f_fdst, (const char*)FileBuffer, FA_OPEN_ALWAYS | FA_WRITE);
            
            f_lseek(&f_fdst,f_size(&f_fdst));
            
            f_write(&f_fdst, f_Buffer, ReadBytes, &bw);
            
            f_close(&f_fdst);
            FileSystemUnlock();//�ͷ���Դ������ɶԳ���
            
            ReadIndex += ReadBytes;
        }
        if(NewFile)
        {
            LogRecordFileInit();
        }
        return SUCCESS;	
	}	
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	if(NewFile)
	{
		LogRecordFileInit();
	}
	return SUCCESS;		
}

ErrorStatus FileSystemTest(unsigned char disk)
{
	FIL fdst;
	UINT bw;
	char Buffer[20];
	char FileName[30];
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	if(disk == 0)
	{
		strcpy(FileName, "0:���̲����ļ�.txt");
	}
	else
	{
		strcpy(FileName, "1:���̲����ļ�.txt");
	}
	
	if(f_open(&fdst, (const char*)FileName, FA_CREATE_ALWAYS | FA_WRITE | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	strcpy(Buffer, "123567890ABCDEF");
	if(f_write(&fdst, Buffer, strlen(Buffer)+1,&bw) != FR_OK)
	{
		f_close(&fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return SUCCESS;				
	}	
	f_close(&fdst);
	memset(Buffer, 0, sizeof(Buffer));
	
	if(f_open(&fdst, (const char*)FileName, FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}

	f_lseek(&fdst,0);
	f_read(&fdst,Buffer, sizeof(Buffer), &bw);
	f_close(&fdst);
	f_unlink((const char*)FileName);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	if ((strcmp(Buffer, "123567890ABCDEF") == 0) && (bw == (strlen("123567890ABCDEF")+1)))
	{
		return SUCCESS;
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ͨ���𱬼�¼�ļ�����ȡ��Ӧ���𱬼�¼����ֵ
  * @param  FileName���𱬼�¼�ļ���
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
u16 GetBlastRecorderIndexFromFileName(u8* FileName)
{
	u16 i;
	
	if(strlen((char*)FileName) != 12)
	{
		return 0xFFFF;
	}
	for(i=0;i<BlastRecorder.Number;i++)
	{
		if(memcmp(&BlastRecorder.FileName[i][0], FileName, 12) == 0)
		{
			return i;
		}
	}
	return 0xFFFF;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�𱬼�¼�ļ���С
  * @param  FileName���𱬼�¼����
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
u32 GetBlastRecorderFileSize(u16 FileIndex)
{
	u32 fsize = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	memset((char*)f_Buffer, 0, sizeof(f_Buffer));
	strcpy((char*)f_Buffer, GetBlasterPath());
	
	f_Buffer[strlen((char*)f_Buffer)] = '/';
	memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], &BlastRecorder.FileName[FileIndex][0], 12);
	strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
	
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;
	}
	fsize = f_fdst.fsize;
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	return fsize;
} 

/**
  *************************************************************************************
  * @brief  ��ȡ�𱬼�¼�ļ���С
  * @param  FileName���𱬼�¼����
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlastRecorderData(u16 FileIndex, u32 ReadIndex, u16 ReadSize, u8* Ptr)
{
	UINT  bw = 0;         /* File read/write count */
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	memset((char*)f_Buffer, 0, sizeof(f_Buffer));
	strcpy((char*)f_Buffer, GetBlasterPath());
	
	f_Buffer[strlen((char*)f_Buffer)] = '/';
	memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], &BlastRecorder.FileName[FileIndex][0], 12);
	strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
	
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_lseek(&f_fdst, ReadIndex) != FR_OK)
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	if(f_read(&f_fdst,Ptr, ReadSize, &bw) != FR_OK)
	{		
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	if(bw != ReadSize)
	{
		return ERROR;		
	}
	return SUCCESS;
} 
/**
  *************************************************************************************
  * @brief  ��ȡ�𱬼�¼�ļ���С
  * @param  FileName���𱬼�¼����
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetLogRecorderData(u16 FileIndex, u32 ReadIndex, u16 ReadSize, u8* Ptr)
{
	UINT  bw = 0;         /* File read/write count */
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	memset((char*)f_Buffer, 0, sizeof(f_Buffer));
	strcpy((char*)f_Buffer, SYSTEM_OPERATE_INFO_PATH);
	f_Buffer[strlen((char*)f_Buffer)] = '/';
	memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], &LogRecorder.FileName[FileIndex][0], 6);
	strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
	
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING|FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_lseek(&f_fdst, ReadIndex) != FR_OK)
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	if(f_read(&f_fdst,Ptr, ReadSize, &bw) != FR_OK)
	{		
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;		
	}
	
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	if(bw != ReadSize)
	{
		return ERROR;		
	}
	return SUCCESS;
} 

/**
  *************************************************************************************
  * @brief  ͨ����־�ļ�����ȡ��Ӧ���𱬼�¼����ֵ
  * @param  FileName����־�ļ���
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
u16 GetLogRecorderIndexFromFileName(u8* FileName)
{
	u16 i;
	
	if(strlen((char*)FileName) != 6)
	{
		return 0xFFFF;
	}
	for(i=0;i<LogRecorder.Number;i++)
	{
		if(memcmp(&LogRecorder.FileName[i][0], FileName, 6) == 0)
		{
			return i;
		}
	}
	return 0xFFFF;
}

/**
  *************************************************************************************
  * @brief  ��ȡ��־�ļ���С
  * @param  FileIndex����־�ļ�����
  * @retval ���
  * @author ZH
  * @date   2017��6��20��
  * @note   
  *************************************************************************************  
  */
u32 GetLogRecorderFileSize(u16 FileIndex)
{
	u32 fsize = 0;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	memset((char*)f_Buffer, 0, sizeof(f_Buffer));
	strcpy((char*)f_Buffer, SYSTEM_OPERATE_INFO_PATH);
	
	f_Buffer[strlen((char*)f_Buffer)] = '/';
	memcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], &LogRecorder.FileName[FileIndex][0], 6);
	strcpy((char*)&f_Buffer[strlen((char*)f_Buffer)], ".txt");
	
	if(f_open(&f_fdst, (const char*)f_Buffer, FA_READ) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return 0;
	}
	fsize = f_fdst.fsize;
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	return fsize;
}  
 
