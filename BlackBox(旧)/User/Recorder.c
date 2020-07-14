#include "includes.h"

static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */   
static  OS_EVENT   *FileSystemMutex;

LOG_RECORDER LogRecorder;


void FileSystemLock(void)
{
	INT8U  err;
	
    if(FileSystemMutex == NULL)
    {
        FileSystemMutex = OSMutexCreate(RES_FILE_MUTEX_PRIO, &err);
    }
    OSMutexPend(FileSystemMutex,0,&err);
}
void FileSystemUnlock(void)
{	
	OSMutexPost(FileSystemMutex);
}

/**
  *************************************************************************************
  * @brief  判断一个文件是否存在
  * @param  PathName 路径 
  * @retval 结果
  *         @arg @b ERROR   文件不存在
  *         @arg @b SUCCESS 文件存在
  * @author ZH
  * @date   2014年12月06日
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
  * @brief  读一个文件大小
  * @param  PathName 路径 
  * @retval 结果
  *         @arg @b ERROR   文件不存在
  *         @arg @b SUCCESS 文件存在
  * @author ZH
  * @date   2018年05月10日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetFileSize(char *FilePathName, u32* size)
{	
	FileSystemLock();//申请文件资源，必需成对出现
	if(FR_OK == f_open(&f_fdst, FilePathName, FA_OPEN_EXISTING | FA_READ))
	{
		*size = f_size(&f_fdst);
		f_close(&f_fdst);
		FileSystemUnlock();//释放资源，必需成对出现
		return SUCCESS;
	}
	FileSystemUnlock();//释放资源，必需成对出现
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  读出厂设置的参数值
  * @param  无 
  * @retval 无 
  * @author ZH
  * @date   2014年10月16日
  * @note   参数值MCU内部FLASH的最后一页，2K大小
  *************************************************************************************  
  */
void ReadFactoryConfigParam(void)
{
//	u8 temp[20];
//	
//	//设备ID
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DEVICE_ID_FILE_NAME, device_info.mac, sizeof(device_info.mac)))
//	{
//		memset(device_info.mac, 0, sizeof(device_info.mac));
//	}
//	
//	//设备硬件版本
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, HW_VERSION_FILE_NAME, device_info.hw_version, sizeof(device_info.hw_version)))
//	{
//		memset(device_info.hw_version, 0, sizeof(device_info.hw_version));
//	}
//	
//	//设备类型
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DEVICE_TYPE_FILE_NAME, &device_info.DeviceType, sizeof(device_info.DeviceType)))
//	{
//		memset(&device_info.DeviceType, 0, sizeof(device_info.DeviceType));
//	}	
//	if((device_info.DeviceType != DEVICE_TYPE_SAFE_MONITOR) && 
//        (device_info.DeviceType != DEVICE_TYPE_CHINESE_GENERAL) && 
//        (device_info.DeviceType != DEVICE_TYPE_SAFE_MONITOR_OFFLINE))
//    {
//        device_info.DeviceType = DEVICE_TYPE_SAFE_MONITOR;
//    }
//    
//	//安管服务器网络IP及端口
//	if(ERROR == ReadSpecifiedFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&device_info.Server, sizeof(device_info.Server)))
//	{
//		device_info.Server.Ip = IpAddrStringToU32("60.205.57.100");
//		device_info.Server.Port = 20601;
//		memset(&device_info.Server.Date[0], '0',sizeof(device_info.Server.Date));
//	}
//	
//	//设备保留配置参数
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, CTRL_BYTES_FILE_NAME, (u8*)&device_info.config, sizeof(device_info.config)))
//	{
//		memset((u8*)&device_info.config, 0, sizeof(device_info.config));
//	}

//	//单发雷管电流范围
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DET_CURRENT_FILE_NAME, (u8*)&temp[0], 4))
//	{	
//		device_info.single_current_min = 10;
//		device_info.single_current_max = 35;	
//	}
//	else
//	{
//		device_info.single_current_min = (u16)(((u16)temp[0]<<8) + temp[1]);
//		device_info.single_current_max = (u16)(((u16)temp[2]<<8) + temp[3]);		
//	}	
//	//雷管频率范围
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DET_FREQ_FILE_NAME, (u8*)&temp[0], 8))
//	{
//		device_info.det_freq_min = 55000;
//		device_info.det_freq_max = 110000;
//	}
//	else
//	{
//		device_info.det_freq_min = (u32)(((u32)temp[0]<<24) + ((u32)temp[1]<<16) + ((u32)temp[2]<<8) + temp[3]);		
//		device_info.det_freq_max = (u32)(((u32)temp[4]<<24) + ((u32)temp[5]<<16) + ((u32)temp[6]<<8) + temp[7]);	
//	}	


//	//设备电池电压校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.bat.k = 0;
//		adc_adjust.bat.b = 0;
//	}
//	else
//	{
//		adc_adjust.bat.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.bat.b = (u16)(((u16)temp[2]<<8) + temp[3]);		
//	}
//	
//	//总线电压校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.linab.k = 0;
//		adc_adjust.linab.b = 0;
//	}
//	else
//	{
//		adc_adjust.linab.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.linab.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
//	}

//	//低压下电流小量程校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.low1current.k = 0;
//		adc_adjust.low1current.b = 0;
//	}
//	else
//	{
//		adc_adjust.low1current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.low1current.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
//	}

//	//低压下电流大量程校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.low2current.k = 0;
//		adc_adjust.low2current.b = 0;
//	}
//	else
//	{
//		adc_adjust.low2current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.low2current.b = (u16)(((u16)temp[2]<<8) + temp[3]);	
//	}
//	
//	//高压下电流小量程校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.high1current.k = 0;
//		adc_adjust.high1current.b = 0;
//	}
//	else
//	{
//		adc_adjust.high1current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.high1current.b = (u16)(((u16)temp[2]<<8) + temp[3]);
//	}	

//	//高压下电流大量程校准参数
//	if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME, (u8*)&temp[0], 4))
//	{
//		adc_adjust.high2current.k = 0;
//		adc_adjust.high2current.b = 0;
//	}
//	else
//	{
//		adc_adjust.high2current.k = (((u16)temp[0]<<8) + temp[1])/1000.0;
//		adc_adjust.high2current.b = (u16)(((u16)temp[2]<<8) + temp[3]);
//	}
//	//延期抽检配置
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, DELAY_SAMPLE_FILE_NAME, &Op_Data.DelaySampleCfg, sizeof(Op_Data.DelaySampleCfg)))
//	{
//		memset(&Op_Data.DelaySampleCfg, 0, sizeof(Op_Data.DelaySampleCfg));
//	}
//	
//	//扫码模块配置
//	if(ERROR == ReadSpecifiedFileData(FACTORY_CFG_FILE_PATH, SCAN_FILE_NAME, &device_info.HasScan, sizeof(device_info.HasScan)))
//	{
//		device_info.HasScan = 1;//默认带扫码头
//	}

//	if(ERROR == ReadSpecifiedFileData(DET_CODE_WRITE_FILE_PATH, DET_CODE_WRITE_FILE_NAME, (u8*)&Op_Data.NewWriteSerl, sizeof(Op_Data.NewWriteSerl)))
//	{
//		Op_Data.NewWriteSerl = 0;
//	}   
}
/**
  *************************************************************************************
  * @brief  初始化日志记录数据
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年5月09日
  * @note   日志记录数据保存在结构体LogRecorder中
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
	
	
	FileSystemLock();//申请文件资源，必需成对出现
	
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
				{//文件名格式检查
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
							{//符合要求的文件数未超过要求
//								FileSystemUnlock();//释放资源，必需成对出现
//								if(ConfigFileVerify(FilePath, (u8*)fn, 0) == SUCCESS)
//								{//文件校验通过，没有被修改
									memcpy(&LogRecorder.FileName[LogRecorder.Number][0], fn, strlen("141206"));
									LogRecorder.Number++;	
//									FileSystemLock();//申请文件资源，必需成对出现
									continue;								
//								}
//								else
//								{
//									FileSystemLock();//申请文件资源，必需成对出现
//								}
							}
						}
					}
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
	
	//文件名从大到小排序
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
		if(flag_bit)//提前结束排序
		{
			break ;
		}		
	}
	FileSystemUnlock();//释放资源，必需成对出现
}
/**
  *************************************************************************************
  * @brief  保存系统运行日志
  * @param  data 要保存的数据数据指针
  * @param  len   要保存的数据长度
  * @param  op_type   要保存的数据格式
  * @retval ErrorStatus  保存结果
  *    @arg @b TRUE    写入成功
  *    @arg @b FALSE   写入失败 
  * @author ZH
  * @date   2014年12月03日
  * @note   无
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
	u8 NewFile = 0;
	
	FileSystemLock();//申请文件资源，必需成对出现
	
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStruct);
	
	res = f_opendir(&dir, SYSTEM_OPERATE_INFO_PATH);
	if (res != FR_OK)
	{//创建目录
		res = f_mkdir(SYSTEM_OPERATE_INFO_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//释放资源，必需成对出现
			return ERROR;
		}
	}
	res = f_chmod (SYSTEM_OPERATE_INFO_PATH, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
	if (res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	sprintf((char*)f_Buffer, "%s/%02x%02x%02x.txt", SYSTEM_OPERATE_INFO_PATH, 
		RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date);		
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	
	if(f_size(&f_fdst) == 0)
	{//新文件
		NewFile = 1;
		if(LogRecorder.Number >= LOG_RECORD_MAX_NUM)
		{
			u8 deleteIndex = LOG_RECORD_MAX_NUM-1;
			s16 index;
			
			if(RTC_DateStruct.RTC_Year < 0x18)
			{//对RTC掉电造成的时间不准进行特殊处理
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
    
	if(DET_DATA_TYPE == op_type)
    {
        DET_DATA_STRUCT *DetDataStruct = (DET_DATA_STRUCT*)data;

        if (DetDataStruct->Type == DET_CMD_DATA)
        {//命令
            sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x时%02x分%02x秒",
                RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
            res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw); 

            j = 0;
            for(i=0;i<DetDataStruct->Data.Cmd.Count;i++)
            {
                j += sprintf((char*)&f_Buffer[j], "%02X ", DetDataStruct->Data.Cmd.Data[i]);
            }
            f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
            
            if(ERROR == GetDetCmdNameString(DetDataStruct->Data.Cmd.Data[0], (char*)f_Buffer))//标注出是什么命令
            {
                strcpy((char*)f_Buffer, "未知命令");
            }
            f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
        }
        else if (DetDataStruct->Type == DET_BLAST_CMD_DATA)
        {//起爆信号
            sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x时%02x分%02x秒",
                RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
            f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw); 

            j = 0;
            for(i=0;i<DetDataStruct->Data.BlastCmd.Count;i++)
            {
                j += sprintf((char*)&f_Buffer[j], "%dus ", (int)(DetDataStruct->Data.BlastCmd.Data[i]/84));
            }
            f_write(&f_fdst, "起爆信号", strlen("起爆信号"), &bw);	
        }
    }
	else if(NEW_STR_INFO == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x年%02x月%02x日%02x时%02x分%02x秒",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		res = f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);			
		
		res = f_write(&f_fdst, data, len, &bw);		
	}
	else if(STR_INFO == op_type)
	{
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
			sprintf((char*)f_Buffer,"\r\n数据%d字节,以下为后50字节:\r\n", len);	
		}
		else
		{
			w_size = len;
			sprintf((char*)f_Buffer,"\r\n数据%d字节:\r\n", len);		
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
	else if(DEV_START == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n%02x年%02x月%02x日%02x时%02x分%02x秒",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
		
		f_write(&f_fdst, "\r\n设备开机", strlen("\r\n设备开机"), &bw);	
		f_write(&f_fdst, "\r\n软件版本:", strlen("\r\n软件版本:"), &bw);	
		f_write(&f_fdst, VERSION_STRING, strlen(VERSION_STRING), &bw);
		f_write(&f_fdst, "\r\n硬件版本:", strlen("\r\n硬件版本:"), &bw);	
		f_write(&f_fdst, device_info.hw_version, strlen((char*)device_info.hw_version), &bw);
		f_write(&f_fdst, "\r\n设备编号:", strlen("\r\n设备编号:"), &bw);	
		f_write(&f_fdst, device_info.mac, strlen((char*)device_info.mac), &bw);
		
	}
	else if(CURRENT_STATUS == op_type)
	{
		sprintf((char*)f_Buffer,"\r\n------Start------\r\n");
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);
		
		f_write(&f_fdst, data, len, &bw);
		
		f_printf(&f_fdst, "\r\n总线电压%dmV", GetBusVoltage(10,NULL));   
		
		sprintf((char*)f_Buffer,"\r\n%02x年%02x月%02x日%02x时%02x分%02x秒",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month, 
			RTC_DateStruct.RTC_Date, RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);				
	}	

	f_close(&f_fdst);
	FileSystemUnlock();//释放资源，必需成对出现
	
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
	
	FileSystemLock();//申请文件资源，必需成对出现
	if(disk == 0)
	{
		strcpy(FileName, "0:磁盘测试文件.txt");
	}
	else
	{
		strcpy(FileName, "1:磁盘测试文件.txt");
	}
	
	if(f_open(&fdst, (const char*)FileName, FA_CREATE_NEW | FA_WRITE | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}	
	strcpy(Buffer, "123567890ABCDEF");
	if(f_write(&fdst, Buffer, strlen(Buffer)+1,&bw) != FR_OK)
	{
		f_close(&fdst);
		FileSystemUnlock();//释放资源，必需成对出现
		return SUCCESS;				
	}	
	f_close(&fdst);
	memset(Buffer, 0, sizeof(Buffer));
	
	if(f_open(&fdst, (const char*)FileName, FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}

	f_lseek(&fdst,0);
	f_read(&fdst,Buffer, sizeof(Buffer), &bw);
	f_close(&fdst);
	f_unlink((const char*)FileName);
	
	FileSystemUnlock();//释放资源，必需成对出现
	
	if ((strcmp(Buffer, "123567890ABCDEF") == 0) && (bw == (strlen("123567890ABCDEF")+1)))
	{
		return SUCCESS;
	}
	return ERROR;	
}

