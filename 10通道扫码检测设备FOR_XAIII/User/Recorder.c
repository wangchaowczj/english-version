#include "includes.h"

extern const char* FactoryCode2String(u8 InterfaceCode);
extern const FACTORY_NAME_CFG FactoryNameArray[];

static FATFS MscFatfs;
static  FIL Msc_fdst;      /* file objects */
static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */   
static  OS_EVENT   *FileSystemMutex = 0;

#define ONE_RECORDER_BYTES    13

DET_TEST_RECORDER DetTestRecorder;
DET_DATA_BINDING_RECORDER DetDataBindingRecorder;

void FileSystemLock(void)
{
	INT8U  err;
	
	if(FileSystemMutex == 0)
	{
		FileSystemMutex = OSMutexCreate(RES_FILE_MUTEX_PRIO, &err);
	}	
	OSMutexPend(FileSystemMutex,0,&err);
}
void FileSystemUnlock(void)
{
	if(FileSystemMutex != 0)
	{
		OSMutexPost(FileSystemMutex);	
	}
}

//==================================================================================================
//| 函数名称 | GetSector
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据输入的地址返回对应有的FLASH扇区号
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 | 	无
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	Address FLASH地址
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	FLASH 扇区号
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 |  无
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：郑海    时间：2013-05-22 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else
  {
    sector = FLASH_Sector_11;  
  }
  return sector;
}
//==================================================================================================
//| 函数名称 | ReadAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 读取保存在AT45DB中的数据并判断CRC校验字节
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | addr    要读取的数据起始地址
//|          | length  要读取的数据长度(不包括2字节CRC16校验)
//|          | ptr     要保存的数据缓冲区
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | TRUE    读取成功，校验通过
//|          | FALSE   校验失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 函数使用SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus ReadFlashInfoCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i;
	u8 crc16[2],temp[2];

	if(length > 0)
	{
		for(i=0;i<length;i++)
		{
			*(ptr+i) = *(__IO uint8_t*)(addr+i);
		}			
		temp[0] = *(__IO uint8_t*)(addr+length);
		temp[1] = *(__IO uint8_t*)(addr+length+1);
		
		GetCrc16Value(0xFFFF,crc16, ptr, length);
		if((crc16[0] == temp[0]) && (crc16[1] == temp[1]))
		{
			return SUCCESS;
		}
		memset(ptr, 0, length);
	}
	return ERROR;
}

//==================================================================================================
//| 函数名称 | WriteAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 保存数据到AT45DB中并增加CRC校验字节
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | addr    要写入的起始地址
//|          | length  要写入的数据长度(不包括2字节CRC16校验)
//|          | ptr     要写入的数据缓冲区
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | TRUE    写成功
//|          | FALSE   写失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 函数使用SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus WriteFlashInfoCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i;
	u8 crc16[2];
	
	FLASH_Unlock();
		
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 	
	for(i=0;i<length;i++)
	{
		if(FLASH_COMPLETE != FLASH_ProgramByte(addr+i, *(ptr+i)))
		{
			return ERROR;
		}
	}
	
	GetCrc16Value(0xFFFF,crc16, ptr, length);
	for(i=0;i<2;i++)
	{
		if(FLASH_COMPLETE != FLASH_ProgramByte(addr+length+i, crc16[i]))
		{
			return ERROR;
		}		
	}	

	return SUCCESS;
}

void ReadFactoryParam(void)
{
	u8 temp[20];
	
	//设备ID
	ReadFlashInfoCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE, DeviceInfo.ID);
	
	//设备硬件版本
	ReadFlashInfoCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW);
	
	ReadFlashInfoCRC16(FACTORY_CFG_ADJUST_CURRENT_ADDR, FACTORY_CFG_ADJUST_CURRENT_SIZE, &temp[0]);	
	AdcAdjust.Current.k = ((temp[1]<<8)+temp[0])/1000.0;
	AdcAdjust.Current.b = ((temp[3]<<8)+temp[2]);

	ReadFlashInfoCRC16(FACTORY_CFG_ADJUST_1TO8_ADDR, FACTORY_CFG_ADJUST_1TO8_SIZE, &temp[0]);	
	AdcAdjust.LinAB1To8.k = ((temp[1]<<8)+temp[0])/1000.0;
	AdcAdjust.LinAB1To8.b = ((temp[3]<<8)+temp[2]);
	
	ReadFlashInfoCRC16(FACTORY_CFG_ADJUST_9_ADDR, FACTORY_CFG_ADJUST_9_SIZE, &temp[0]);	
	AdcAdjust.LinAB9.k = ((temp[1]<<8)+temp[0])/1000.0;
	AdcAdjust.LinAB9.b = ((temp[3]<<8)+temp[2]);

	ReadFlashInfoCRC16(FACTORY_CFG_ADJUST_10_ADDR, FACTORY_CFG_ADJUST_10_SIZE, &temp[0]);	
	AdcAdjust.LinAB10.k = ((temp[1]<<8)+temp[0])/1000.0;
	AdcAdjust.LinAB10.b = ((temp[3]<<8)+temp[2]);
	
	ReadFlashInfoCRC16(FACTORY_CFG_ADJUST_BAT_ADDR, FACTORY_CFG_ADJUST_BAT_SIZE, &temp[0]);	
	AdcAdjust.Bat.k = ((temp[1]<<8)+temp[0])/1000.0;
	AdcAdjust.Bat.b = ((temp[3]<<8)+temp[2]);
	
	ReadFlashInfoCRC16(DEVICE_ADJUST_TIME_ADD, DEVICE_ADJUST_TIME_ADD_LEN, AdcAdjust.Time);	
	
//	if(ReadFlashInfoCRC16(FACTORY_CFG_FACTORY_CODE_ADDR, FACTORY_CFG_FACTORY_CODE_SIZE, temp) == ERROR)
//	{//采用默认支持的工厂代码
//		u8 i=0;
//		
//		memset(FactoryCodeSupport, 0, FACTORY_CODE_SUPPORT_MAX);
//		
//		FactoryCodeSupport[i++] = 44;//卫东机械厂对应0x01
//		FactoryCodeSupport[i++] = 34;//江西钢丝厂对应0x02
//		FactoryCodeSupport[i++] = 56;//四川宜宾威力对应0x03
//		FactoryCodeSupport[i++] = 10;//内蒙红旗对应0x04
//        FactoryCodeSupport[i++] = 36;//山东圣世达对应0x05
//        FactoryCodeSupport[i++] = 53;//广西金建华对应0x06
//	}
//	else
//	{
//		memcpy(FactoryCodeSupport, temp, FACTORY_CODE_SUPPORT_MAX);
//	}
//    if(ReadFlashInfoCRC16(FACTORY_CFG_DET_CLASS_ADDR, FACTORY_CFG_DET_CLASS_SIZE, temp) == ERROR)
//    {
//        DeviceInfo.DetClass = 0;
//    }
//    else
//    {
//        DeviceInfo.DetClass = temp[0];
//    }
}

ErrorStatus WriteFactoryParam(void)
{
	u8 temp[20];
	
	//擦除FLASH
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	if(FLASH_COMPLETE != FLASH_EraseSector(GetSector(FACTORY_CFG_START_ADDR), VoltageRange_3))
	{
		return ERROR;		
	}

	//设备ID
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE,DeviceInfo.ID))
	{
		return ERROR;
	}	
	
	//设备硬件版本
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW))
	{
		return ERROR;
	}

	//写校准参数
	temp[0] = ((u16)(AdcAdjust.Current.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.Current.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.Current.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.Current.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_CURRENT_ADDR, FACTORY_CFG_ADJUST_CURRENT_SIZE, temp))
	{
		return ERROR;
	}
	//写校准参数
	temp[0] = ((u16)(AdcAdjust.LinAB1To8.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB1To8.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB1To8.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB1To8.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_1TO8_ADDR, FACTORY_CFG_ADJUST_1TO8_SIZE, temp))
	{
		return ERROR;
	}
	//写校准参数
	temp[0] = ((u16)(AdcAdjust.LinAB9.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB9.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB9.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB9.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_9_ADDR, FACTORY_CFG_ADJUST_9_SIZE, temp))
	{
		return ERROR;
	}
	//写校准参数
	temp[0] = ((u16)(AdcAdjust.LinAB10.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB10.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB10.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB10.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_10_ADDR, FACTORY_CFG_ADJUST_10_SIZE, temp))
	{
		return ERROR;
	}
	//写校准参数
	temp[0] = ((u16)(AdcAdjust.Bat.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.Bat.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.Bat.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.Bat.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_BAT_ADDR, FACTORY_CFG_ADJUST_BAT_SIZE, temp))
	{
		return ERROR;
	}	
	//写校准时间
	if(ERROR == WriteFlashInfoCRC16(DEVICE_ADJUST_TIME_ADD, DEVICE_ADJUST_TIME_ADD_LEN,AdcAdjust.Time))
	{
		return ERROR;
	}	
	
	memset(temp, 0, sizeof(temp));
	memcpy(temp,FactoryCodeSupport,FACTORY_CODE_SUPPORT_MAX);
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_FACTORY_CODE_ADDR, FACTORY_CFG_FACTORY_CODE_SIZE, temp))
	{
		return ERROR;
	}
//    memset(temp, 0, sizeof(temp));
//    temp[0] = DeviceInfo.DetClass;
//	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_DET_CLASS_ADDR, FACTORY_CFG_DET_CLASS_SIZE, temp))
//	{
//		return ERROR;
//	}
	
	FLASH_Lock();	
	return SUCCESS;
}
void U32ToAscii(u32 value, u8* str)
{
	u8 i = 9;
	u8 temp[11];
	
	temp[10] = 0x00;
	do
	{
		temp[i--] = value%10 + '0'; //逐位转换，采用倒序存储字符，省去了后续的字符颠倒工作
	}while((value /= 10)>0);
	strcpy((char*)str, (char*)&temp[i+1]);
}

u32 AsciiToU32(u8* str)
{
	u32 value = 0;
	
	while(*str != NULL)
	{
		value *= 10;
		value += *str-'0';
		str++;
	}
	return value;
}
ErrorStatus ReadKeyString(u8* path,u8* key, u8* buff, u16 buff_size)
{
	ErrorStatus result = ERROR;
	u8 crc16[2];
	FRESULT f_res;         /* FatFs function common result code */
	
	f_res = f_open(&f_fdst, (const char*)path, FA_OPEN_EXISTING | FA_READ);
	if(f_res)
	{
		return result;
	}
	f_lseek(&f_fdst,0);
	while(NULL != f_gets((char*)f_Buffer, 512, &f_fdst))
	{
		if(memcmp(&f_Buffer[1], key, strlen((char*)key))==0)
		{
			if(('[' == f_Buffer[0]) && (']'==f_Buffer[strlen((char*)key)+1]) && ('\n'==f_Buffer[strlen((char*)key)+2]))
			{//找到相应的键值
				if(NULL != f_gets((char*)f_Buffer, 512, &f_fdst))
				{
					if(buff == NULL)
					{//不需要保存数据
						result = SUCCESS;
						break;						
					}						
					else						
					{
						if((buff_size > strlen((char*)f_Buffer)) && (strlen((char*)f_Buffer) < 50))
						{
							GetCrc16Value(0xFFFF, &crc16[0],f_Buffer,strlen((char*)f_Buffer));//对数据生成CRC16校验值
							if(NULL != f_gets((char*)&f_Buffer[200], 10, &f_fdst))//读取配置文件中的CRC16校验值
							{
								sprintf((char*)&f_Buffer[250],"%02X%02X\n",crc16[0],crc16[1]);
								if(strcmp((const char*)&f_Buffer[200], (const char*)&f_Buffer[250])==0)//校验值比较
								{
									memcpy(buff, &f_Buffer[0], strlen((char*)f_Buffer)+1);	
									result = SUCCESS;
									break;								
								}								
							}
						}
						else
						{//输入的缓冲区空间不足
							result = ERROR;			
							break;							
						}
					}			
				}
				else
				{//有键值，但没数据
					result = ERROR;		
					break;					
				}
			}
		}
	}
	f_close(&f_fdst);		
	return result;
}
ErrorStatus ReadKeyU32Value(u8* path, u8* key, u32* value)
{
	u8 buf[11];//0xFFFFFFFF对应的10进制数据为4294967295，占10位
	
	if(ERROR != ReadKeyString(path,key, buf, sizeof(buf)))
	{
		*value = AsciiToU32(&buf[0]);
		return SUCCESS;
	}
	return ERROR;	
}
ErrorStatus WriteKeyString(u8* path,u8* key, u8* buf)
{
	u16 index = 0;
	UINT  f_rw;         /* File read/write count */	 
	
	if(strlen((const char*)buf) > 50)
	{
		return ERROR;
	}
	if(f_open(&f_fdst, (const char*)path, FA_OPEN_ALWAYS | FA_WRITE | FA_READ))
	{
		return ERROR;
	}
	
	f_lseek(&f_fdst,0);
	while(NULL != f_gets((char*)f_Buffer, 512, &f_fdst))
	{
		if(memcmp(&f_Buffer[1], key, strlen((char*)key))==0)
		{
			if(('[' == f_Buffer[0]) && (']'==f_Buffer[strlen((char*)key)+1]))
			{
				break;
			}
		}
		index = f_tell(&f_fdst);
	}
	f_lseek(&f_fdst,index);
	
	//写入键值
	memset(f_Buffer, 0, sizeof(f_Buffer));
	f_Buffer[0] = '[';
	strcpy((char*)&f_Buffer[1], (char*)key);
	f_Buffer[strlen((const char*)f_Buffer)] = ']';
	f_Buffer[strlen((const char*)f_Buffer)] = '\n';
	f_puts((char*)f_Buffer, &f_fdst);	
	
	//写入数据及校验值
	memset(f_Buffer, 0, sizeof(f_Buffer));	
	strcpy((char*)&f_Buffer[0], (char*)buf);
	f_Buffer[50] = '\n';
	GetCrc16Value(0xFFFF, &f_Buffer[100], buf, strlen((const char*)buf));
	sprintf((char*)&f_Buffer[51], "%02X%02X\n",f_Buffer[100], f_Buffer[101]);
	f_write(&f_fdst, (char*)f_Buffer, 56, &f_rw);//简化操作，写入固定长度
	f_close(&f_fdst);
	return SUCCESS;	
}
ErrorStatus WriteKeyU32Value(u8* path,u8* key, u32 value)
{
	u8 buf[11];//0xFFFFFFFF对应的10进制数据为4294967295，占10位
	
	U32ToAscii(value, buf);
	return WriteKeyString(path, key, buf);
}
/**
  *************************************************************************************
  * @brief  读用户设置的参数值
  * @param  无 
  * @retval 无 
  * @author ZH
  * @date   2014年9月28日
  * @note   参数值保存在文件系统中的配置参数.txt
  *************************************************************************************  
  */
void ReadUserConfigParam(void)
{
	u32 value;
	u8 Buff[11];
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "雷管类型",&value))
	{
		UserParam.DetType = value;
	}
	else
	{
		UserParam.DetType = DET_TYPE_XA_02;
	}
    
	if((value != DET_TYPE_XA_02) && (value != DET_TYPE_XA_06)  && 
		(value != DET_TYPE_XA_21))
	{
		UserParam.DetType = DET_TYPE_XA_02; 
	}		
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "内码写入",&value))
	{
		UserParam.DetCodeWrite = value;
	}
	else
	{
		UserParam.DetCodeWrite = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "检测环节",&value))
	{
		UserParam.Stage = (USE_STAGE_CFG)value;
	}
	else
	{
		UserParam.Stage = FINISHED;
	}
	if ((UserParam.Stage != ENTER_FACTORY) && (UserParam.Stage != SEMIFINISHED) && (UserParam.Stage != FINISHED) && (UserParam.Stage != DET_CODE_VERIFY))
	{
		UserParam.Stage = FINISHED;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "声音", &value))
	{
		UserParam.Sound = (SOUND_CFG)value;
	}
	else
	{
		UserParam.Sound = FINISHED_RING;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "屏幕亮度",&value))
	{
		UserParam.LcdBright = value;
	}
	else
	{
		UserParam.LcdBright = LCD_BRIGHT_RANK-1;
	}

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "检测时屏幕",&value))
	{
		UserParam.DetTestLcdStatus = (DET_TEST_LCD_STATUS_CFG)value;
	}
	else
	{
		UserParam.DetTestLcdStatus = DET_TEST_LCD_STATUS_ON;
	}

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "工厂代码",&value))
	{
		UserParam.FactoryCode = value;
	}
	else
	{
		UserParam.FactoryCode = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "生产时间",&value))
	{
		UserParam.ProductTime = value;
	}
	else
	{
		UserParam.ProductTime = 0;
	}
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "特征号",&value))
	{
		UserParam.Feature = value;
	}
	else
	{
		UserParam.Feature = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "雷管延期",&value))
	{
		UserParam.DetDelay = value;
	}
	else
	{
		UserParam.DetDelay = 0;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "延期检测",&value))
	{
		UserParam.CheckDelay = value;
	}
	else
	{
		UserParam.CheckDelay = 1;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "延期写入",&value))
	{
		UserParam.DelayWrite = value;
	}
	else
	{
		UserParam.DelayWrite = 0;
	}		
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "内码检验",&value))
	{
		UserParam.CheckDetCode = value;
	}
	else
	{
		UserParam.CheckDetCode = 1;
	}
	if(XA2_DetIsFixDelay(UserParam.DetType) == 0)
	{//可编程雷管
		UserParam.DetDelay = 0;
	}
	else if (UserParam.DetDelay  > GetMaxDelay(UserParam.DetType))
	{
		UserParam.DetDelay = 0;
	}    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "通道数量",&value))
	{
		Channel.Count = value;      
	}
	else
	{
		Channel.Count = 10;
	}
    if((Channel.Count != 1) && (Channel.Count != 5) && (Channel.Count != 10))//目前仅支持10通道、5通道和单通道模式
    {
        Channel.Count = 10;
    }
    
 	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "联网检测",&value))
	{
		UserParam.NetMode = value;
        if(UserParam.NetMode > 1)
        {
            UserParam.NetMode = 1;
        }
	}
	else
	{
		UserParam.NetMode = 0;
	}   

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "联网管厂",&value))
	{
		UserParam.NetModeFactoryCode = value;
	}
	else
	{
		UserParam.NetModeFactoryCode = FactoryNameArray[0].Code;
	}
    if(FactoryCode2String(UserParam.NetModeFactoryCode) == NULL)
    {
        UserParam.NetModeFactoryCode = FactoryNameArray[0].Code;
    }

	if(ERROR != ReadKeyString(CONFIG_PARAM_FILE_COMPLETE_PATH, "起始管码前8位",Buff,11))
	{
		memcpy(UserParam.DetOutsideStartHead, &Buff[0],sizeof(Buff));
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "起始管码后5位",&value))
	{
		UserParam.DetOutsideStartBoxIndex = value;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "盒内数量",&value))
	{
		UserParam.InBoxNumber = value;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "自动返回",&value))
	{
		UserParam.AutoBack = value;
	}
	else
	{
		UserParam.AutoBack = 0;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "历史检测数量",&value))
	{
		UserParam.DetDetectorPassCount = value;
	}
	else
	{
		UserParam.DetDetectorPassCount = 0;
	}
}
void DetTestParamInit(void)
{
	if(ReadDetTestParamCfg() == ERROR)
	{//默认参数
		//入厂检
		test_param_cfg[0].det_lv_current_min = 1200;
		test_param_cfg[0].det_lv_current_max = 2800;
		test_param_cfg[0].det_lv_current_time_out = 3;
		test_param_cfg[0].det_hv_current_min = 1200;
		test_param_cfg[0].det_hv_current_max = 3000;
		test_param_cfg[0].det_hv_current_time_out = 15;
		test_param_cfg[0].det_freq_min = 62000UL;
		test_param_cfg[0].det_freq_max = 103000UL;
		test_param_cfg[0].test_ctrl = 0x03;
		
		//半成品
		test_param_cfg[1].det_lv_current_min = 1200;
		test_param_cfg[1].det_lv_current_max = 3000;
		test_param_cfg[1].det_lv_current_time_out = 3;
		test_param_cfg[1].det_hv_current_min = 1200;
		test_param_cfg[1].det_hv_current_max = 3200;
		test_param_cfg[1].det_hv_current_time_out = 15;
		test_param_cfg[1].det_freq_min = 61000UL;
		test_param_cfg[1].det_freq_max = 104000UL;
		test_param_cfg[1].test_ctrl = 0x0F;
		
		//成品
		test_param_cfg[2].det_lv_current_min = 1000;
		test_param_cfg[2].det_lv_current_max = 3200;
		test_param_cfg[2].det_lv_current_time_out = 3;
		test_param_cfg[2].det_hv_current_min = 1000;
		test_param_cfg[2].det_hv_current_max = 3400;
		test_param_cfg[2].det_hv_current_time_out = 15;
		test_param_cfg[2].det_freq_min = 60000UL;
		test_param_cfg[2].det_freq_max = 105000UL;
		test_param_cfg[2].test_ctrl = 0x03;
	}
	
	if(UserParam.Stage == ENTER_FACTORY)
	{
		memcpy((char*)&test_param, (char*)&test_param_cfg[0], sizeof(TEST_PARAM));
	}
	else if(UserParam.Stage == SEMIFINISHED)
	{
		memcpy((char*)&test_param, (char*)&test_param_cfg[1], sizeof(TEST_PARAM));
	}
	else
	{
		memcpy((char*)&test_param, (char*)&test_param_cfg[2], sizeof(TEST_PARAM));
	}
}
/**
  *************************************************************************************
  * @brief  初始化抽检记录数据
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年6月09日
  * @note   抽检记录数据保存在结构体DetDetectRecorder中
  *************************************************************************************  
  */
void DetTestRecordFileInit(void)
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
	
	
	FileSystemLock();//申请文件资源，必需成对出现

	memset((u8*)&DetTestRecorder, 0, sizeof(DET_TEST_RECORDER));
	
    res = f_opendir(&dir, DET_TEST_RECORD_FILE_PATH);                       /* Open the directory */
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
							if(DetTestRecorder.Number < DET_TEST_RECORDER_MAX_NUM)
							{//符合要求的文件数未超过要求
//								FileSystemUnlock();//释放资源，必需成对出现
//								if(DetDetectRecordFileVerify((u8*)fn) != ERROR)
//								{//文件校验通过，没有被修改
									memcpy(&DetTestRecorder.FileName[DetTestRecorder.Number][0], fn, strlen("141206"));
									DetTestRecorder.Number++;	
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
				sprintf(buffer, "%s/%s",DET_TEST_RECORD_FILE_PATH, fn);
				res = f_unlink(buffer);
				if (res != FR_OK)
				{
					f_chmod (buffer, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
					res = f_unlink(buffer);
					if (res != FR_OK)
					{
						DetTestRecorder.Number = 0;											
					}					
				}				
            }
        }
    }
	
	//文件名从大到小排序
	for(i=0;i<DetTestRecorder.Number-1;i++)
	{
		flag_bit = 1;
		for(j=0;j<(DetTestRecorder.Number-i-1);j++)
		{
			if(memcmp((const char *)&DetTestRecorder.FileName[j+1][0], (const char *)&DetTestRecorder.FileName[j][0], strlen("141206")) > 0)
			{
				flag_bit = 0 ;
				memcpy((char *)buffer, (const char *)&DetTestRecorder.FileName[j+1][0],strlen("141206"));
				memcpy(&DetTestRecorder.FileName[j+1][0], (const char *)&DetTestRecorder.FileName[j][0], strlen("141206"));
				memcpy(&DetTestRecorder.FileName[j][0], (char *)buffer, strlen("141206"));
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
  * @brief  BJ芯片三码绑定初始化记录数据
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年6月09日
  * @note   抽检记录数据保存在结构DetDataBindingRecorder
  *************************************************************************************  
  */
void DetDataBindingRecordFileInit(void)
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
	
	
	FileSystemLock();//申请文件资源，必需成对出现

	memset((u8*)&DetDataBindingRecorder, 0, sizeof(DET_DATA_BINDING_RECORDER));
	
    res = f_opendir(&dir, DET_DATA_BINDING_RECORD_FILE_PATH);                       /* Open the directory */
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
							if(DetDataBindingRecorder.Number < DET_DATA_BINDING_RECORDER_MAX_NUM)
							{//符合要求的文件数未超过要求
//								FileSystemUnlock();//释放资源，必需成对出现
//								if(DetDetectRecordFileVerify((u8*)fn) != ERROR)
//								{//文件校验通过，没有被修改
									memcpy(&DetDataBindingRecorder.FileName[DetDataBindingRecorder.Number][0], fn, strlen("141206"));
									DetDataBindingRecorder.Number++;	
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
				sprintf(buffer, "%s/%s",DET_TEST_RECORD_FILE_PATH, fn);
				res = f_unlink(buffer);
				if (res != FR_OK)
				{
					f_chmod (buffer, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
					res = f_unlink(buffer);
					if (res != FR_OK)
					{
						DetDataBindingRecorder.Number = 0;											
					}					
				}				
            }
        }
    }
	
	//文件名从大到小排序
	for(i=0;i<DetDataBindingRecorder.Number-1;i++)
	{
		flag_bit = 1;
		for(j=0;j<(DetDataBindingRecorder.Number-i-1);j++)
		{
			if(memcmp((const char *)&DetDataBindingRecorder.FileName[j+1][0], (const char *)&DetDataBindingRecorder.FileName[j][0], strlen("141206")) > 0)
			{
				flag_bit = 0 ;
				memcpy((char *)buffer, (const char *)&DetDataBindingRecorder.FileName[j+1][0],strlen("141206"));
				memcpy(&DetDataBindingRecorder.FileName[j+1][0], (const char *)&DetDataBindingRecorder.FileName[j][0], strlen("141206"));
				memcpy(&DetDataBindingRecorder.FileName[j][0], (char *)buffer, strlen("141206"));
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
  * @brief  删除指定的目录下的所有文件
  * @param  Path 指定目录
  * @retval 无
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
void DeleteAllFileSpecifiedPath(char* Path)
{
	char buffer[256];
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif
	
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
				sprintf(buffer, "%s/%s",Path, fn);
				f_unlink(buffer);		
            }
        }
    }	
}
/**
  *************************************************************************************
  * @brief  删除记录文件
  * @param  Index 组索引
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteDetTestRecordFile(u16 Index)
{
	char Buffer[50];
	char temp[20];
	u16 Value;
	
	if(Index == 0xFFFF)
	{
		DeleteAllFileSpecifiedPath(DET_TEST_RECORD_FILE_PATH);
		DetTestRecordFileInit();
		if(DetTestRecorder.Number != 0)
		{
			return ERROR;
		}
		else
		{
			return SUCCESS;
		}		
	}
	else if(Index >= DetTestRecorder.Number)
	{
		return ERROR;
	}
	else 
	{
		Value = DetTestRecorder.Number;
		
		memset(temp,0,sizeof(temp));
		memcpy(temp, &DetTestRecorder.FileName[Index],sizeof(DetTestRecorder.FileName[Index]));
		sprintf(Buffer, "%s/%s.txt",DET_TEST_RECORD_FILE_PATH, temp);
		f_unlink(Buffer);
		
		DetTestRecordFileInit();
		if(DetTestRecorder.Number != (Value-1))
		{
			return ERROR;
		}
		else
		{
			return SUCCESS;
		}
	}
}
/**
  *************************************************************************************
  * @brief  删除三码绑定记录文件
  * @param  Index 组索引
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteDetDataBindingRecordFile(u16 Index)
{
	char Buffer[50];
	char temp[20];
	u16 Value;
	
	if(Index == 0xFFFF)
	{
		DeleteAllFileSpecifiedPath(DET_DATA_BINDING_RECORD_FILE_PATH);
		DetDataBindingRecordFileInit();
		if(DetDataBindingRecorder.Number != 0)
		{
			return ERROR;
		}
		else
		{
			return SUCCESS;
		}		
	}
	else if(Index >= DetDataBindingRecorder.Number)
	{
		return ERROR;
	}
	else 
	{
		Value = DetDataBindingRecorder.Number;
		
		memset(temp,0,sizeof(temp));
		memcpy(temp, &DetDataBindingRecorder.FileName[Index],sizeof(DetDataBindingRecorder.FileName[Index]));
		sprintf(Buffer, "%s/%s.txt",DET_DATA_BINDING_RECORD_FILE_PATH, temp);
		f_unlink(Buffer);
		
		DetDataBindingRecordFileInit();
		if(DetDataBindingRecorder.Number != (Value-1))
		{
			return ERROR;
		}
		else
		{
			return SUCCESS;
		}
	}
}
ErrorStatus SaveDetTestParamCfg(void)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
	
	FileSystemLock();//申请文件资源，必需成对出现
	
	f_mkdir(CONFIG_PARAM_FILE_PATH);	
	
	res=f_open(&f_fdst,TEST_PARAM_FILE_COMPLETE_PATH,FA_WRITE|FA_CREATE_ALWAYS); 
	if(res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_lseek(&f_fdst,0);
		
	res=f_write(&f_fdst, (char*)&test_param_cfg[0], sizeof(TEST_PARAM)*3, (UINT*)&bw); //写入目的文件
	if(res || (bw != sizeof(TEST_PARAM)*3))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_close(&f_fdst);
	FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}

ErrorStatus ReadDetTestParamCfg(void)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
	
	FileSystemLock();//申请文件资源，必需成对出现
	
	res = f_open(&f_fdst,TEST_PARAM_FILE_COMPLETE_PATH,FA_READ|FA_OPEN_EXISTING); 
	if(res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	if(f_size(&f_fdst) != (sizeof(TEST_PARAM)*3))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	
	f_lseek(&f_fdst,0);	
	
	memset((char*)&test_param_cfg[0],0,sizeof(TEST_PARAM)*3);
	res = f_read(&f_fdst, (char*)&test_param_cfg[0], sizeof(TEST_PARAM)*3,(UINT*)&bw);
	if((res != FR_OK) || ((sizeof(TEST_PARAM)*3) != bw))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;	
	}
	
	f_close(&f_fdst);
	
	FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}
//ErrorStatus SaveDetTestQueueInfo(void)
//{
//    FRESULT res;         /* FatFs function common result code */
//    UINT  bw;         /* File read/write count */
//	
//	FileSystemLock();//申请文件资源，必需成对出现
//	
//	f_mkdir(DET_INFO_QUEUE_FILE_PATH);	
//	
//	res=f_open(&f_fdst,DET_INFO_QUEUE_FILE_COMPLETE_PATH,FA_WRITE|FA_CREATE_ALWAYS); 
//	if(res != FR_OK)
//	{
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
//	f_lseek(&f_fdst,0);
//	res=f_write(&f_fdst, (char*)&OpData.DetDetectorPassCount, sizeof(OpData.DetDetectorPassCount), (UINT*)&bw); //写入目的文件
//	if(res || (bw != sizeof(OpData.DetDetectorPassCount)))
//	{
//		f_close(&f_fdst);
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
//	
//	res=f_write(&f_fdst, (char*)&DetInfoQueue, sizeof(DET_INFO_QUEUE), (UINT*)&bw); //写入目的文件
//	if(res || (bw != sizeof(DET_INFO_QUEUE)))
//	{
//		f_close(&f_fdst);
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
//	f_close(&f_fdst);
//	FileSystemUnlock();//释放资源，必需成对出现
//	return SUCCESS;
//}

//ErrorStatus ReadDetTestQueueInfo(void)
//{
//    FRESULT res;         /* FatFs function common result code */
//    UINT  bw;         /* File read/write count */
//	
//	FileSystemLock();//申请文件资源，必需成对出现
//	ClearDetInfoQueue(&DetInfoQueue);
//	OpData.DetDetectorPassCount = 0;
//	res = f_open(&f_fdst,DET_INFO_QUEUE_FILE_COMPLETE_PATH,FA_READ|FA_OPEN_EXISTING); 
//	if(res != FR_OK)
//	{
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
//	if(f_size(&f_fdst) != (sizeof(OpData.DetDetectorPassCount)+sizeof(DET_INFO_QUEUE)))
//	{
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
//	
//	f_lseek(&f_fdst,0);	
//	res = f_read(&f_fdst, (char*)&OpData.DetDetectorPassCount, sizeof(OpData.DetDetectorPassCount),(UINT*)&bw);
//	if((res != FR_OK) || (sizeof(OpData.DetDetectorPassCount) != bw))
//	{
//		ClearDetInfoQueue(&DetInfoQueue);
//		OpData.DetDetectorPassCount = 0;
//		
//		f_close(&f_fdst);
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;	
//	}
//	
//	memset((char*)&DetInfoQueue,0,sizeof(DET_INFO_QUEUE));
//	res = f_read(&f_fdst, (char*)&DetInfoQueue, sizeof(DET_INFO_QUEUE),(UINT*)&bw);
//	if((res != FR_OK) || (sizeof(DET_INFO_QUEUE) != bw))
//	{
//		ClearDetInfoQueue(&DetInfoQueue);
//		OpData.DetDetectorPassCount = 0;
//		
//		f_close(&f_fdst);
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;	
//	}
//	
//	f_close(&f_fdst);
//	
//	FileSystemUnlock();//释放资源，必需成对出现
//	return SUCCESS;
//}

/**
  *************************************************************************************
  * @brief  复制文件
  * @param  psrc 源文件路径
  * @param  pdst 目标文件路径 
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
u8 FileCopy(u8 *psrc,u8 *pdst)
{
	u8 res;
	u16 br=0;
	u16 bw=0;
	DWORD NextIndex = 0;
	u8 DelayCount = 0;
	
	f_unlink((const TCHAR*)pdst);
	res = f_open(&f_fdst,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING); //打开只读文件
	if(res != FR_OK)
	{
		return res;
	}
	res=f_open(&Msc_fdst,(const TCHAR*)pdst,FA_WRITE|FA_OPEN_ALWAYS); 
	if(res != FR_OK)
	{
		f_close(&f_fdst);
		return res;
	}	
	while(1)
	{
		f_lseek(&f_fdst,NextIndex);
		res=f_read(&f_fdst,f_Buffer,sizeof(f_Buffer),(UINT*)&br); //源头读出512字节
		if(res||(br==0))
		{
			f_close(&f_fdst);
			break;
		}		

		f_lseek(&Msc_fdst,NextIndex);
		res=f_write(&Msc_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //写入目的文件
		if(res||(bw!=br))
		{
			f_close(&f_fdst);
			break;
		}
		NextIndex = f_size(&Msc_fdst);
		
		DelayCount++;
		if(DelayCount > 200)
		{
			DelayCount = 0;
			OSTimeDly(2);
		}		
	}
	f_close(&f_fdst);
	f_close(&Msc_fdst);
	return res;
}
/**
  *************************************************************************************
  * @brief  复制抽检记录文件到U盘
  * @param  Index 记录文件索引 
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyDetTestFileToMsc(u16 Index)
{
	char Buffer1[50],FileName[17], Buffer2[50],DstPath[30];
	
	FileSystemLock();//申请文件资源，必需成对出现
 	/* 挂载文件系统 */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));
	strcpy(DstPath, "1:扫码设备(MH)导出数据");
	f_mkdir((const char*)DstPath);			
	
	//只拷贝1组数据
	memset(FileName,0, sizeof(FileName));
	memcpy(FileName, &DetTestRecorder.FileName[Index], sizeof(DetTestRecorder.FileName[0]));
	
	sprintf(Buffer1,"%s/%s.txt", DET_TEST_RECORD_FILE_PATH, FileName);
	sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
	if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}	
	
	
	/* 卸载文件系统 */
	f_mount(&MscFatfs,"1:", 1);
	FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  复制抽检记录文件到U盘
  * @param  Index 记录文件索引 
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyDetDataBindingFileToMsc(u16 Index)
{
	char Buffer1[50],FileName[17], Buffer2[50],DstPath[30];
	
	FileSystemLock();//申请文件资源，必需成对出现
 	/* 挂载文件系统 */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));
	strcpy(DstPath, "1:扫码设备(MH)三码绑定");
	f_mkdir((const char*)DstPath);			
	
	//只拷贝1组数据
	memset(FileName,0, sizeof(FileName));
	memcpy(FileName, &DetDataBindingRecorder.FileName[Index], sizeof(DetDataBindingRecorder.FileName[0]));
	
	sprintf(Buffer1,"%s/%s.txt", DET_DATA_BINDING_RECORD_FILE_PATH, FileName);
	sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
	if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}	
	
	
	/* 卸载文件系统 */
	f_mount(&MscFatfs,"1:", 1);
	FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  从U盘复制HEX文件到TF卡
  * @param  Index 记录文件索引 
  * @retval 结果
  * @author ZH
  * @date   2015年6月23日
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyHexFileToSdCard(void)
{
	char FileName[17], DstPath[30];
	
	FileSystemLock();//申请文件资源，必需成对出现
 	/* 挂载文件系统 */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));

	strcpy(FileName, "1:CPU.hex");
	strcpy(DstPath, "0:TEMP.hex");	

	
	if (FR_OK != FileCopy((u8*)&FileName[0], (u8*)&DstPath[0]))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	/* 卸载文件系统 */
	f_mount(&MscFatfs,"1:", 1);


	f_unlink("0:CPU.hex");
	if(FR_OK != f_rename("0:TEMP.hex","0:CPU.hex"))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;	
	}

	
	FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}

ErrorStatus ReadDetTestRecordTestCount(u8* Name, u32* TotilNum, u32* ErrNum)
{
	u8 res;
	UINT  rw;
	
	FileSystemLock();//申请文件资源，必需成对出现
	
	sprintf((char*)f_Buffer, "%s/%s.txt", DET_TEST_RECORD_FILE_PATH, Name);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_lseek(&f_fdst,0);
	if(NULL != f_gets((char*)f_Buffer, sizeof(f_Buffer), &f_fdst))
	{
		rw = strlen((char*)f_Buffer);
		if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
		{
			f_Buffer[rw-1] = 0x00;
			rw--;
		}
		if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
		{
			f_Buffer[rw-1] = 0x00;
			rw--;
		}

		if(0 == memcmp("总数:",f_Buffer, strlen("总数:")))
		{
			*TotilNum = AsciiToU32(&f_Buffer[strlen("总数:")]);
			if(NULL != f_gets((char*)f_Buffer, sizeof(f_Buffer), &f_fdst))
			{
				rw = strlen((char*)f_Buffer);
				if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
				{
					f_Buffer[rw-1] = 0x00;
					rw--;
				}
				if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
				{
					f_Buffer[rw-1] = 0x00;
					rw--;
				}

				if(0 == memcmp("不合格数:",f_Buffer, strlen("不合格数:")))
				{
					*ErrNum = AsciiToU32(&f_Buffer[strlen("不合格数:")]);	
					f_close(&f_fdst);
					
					FileSystemUnlock();//释放资源，必需成对出现
					return SUCCESS;
				}
			}
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//释放资源，必需成对出现
	return ERROR;
}
ErrorStatus ReadDetDataBindingRecordTestCount(u8* Name, u32* TotilNum)
{
	u8 res;
	UINT  rw;
	
	FileSystemLock();//申请文件资源，必需成对出现
	
	sprintf((char*)f_Buffer, "%s/%s.txt", DET_DATA_BINDING_RECORD_FILE_PATH, Name);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_lseek(&f_fdst,0);
	if(NULL != f_gets((char*)f_Buffer, sizeof(f_Buffer), &f_fdst))
	{
		rw = strlen((char*)f_Buffer);
		if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
		{
			f_Buffer[rw-1] = 0x00;
			rw--;
		}
		if((rw > 0) && ((f_Buffer[rw-1] == 0x0D) || (f_Buffer[rw-1] == 0x0A)))
		{
			f_Buffer[rw-1] = 0x00;
			rw--;
		}

		if(0 == memcmp("三码绑定总数:",f_Buffer, strlen("三码绑定总数:")))
		{
			*TotilNum = AsciiToU32(&f_Buffer[strlen("三码绑定总数:")]);
            f_close(&f_fdst);
            
            FileSystemUnlock();//释放资源，必需成对出现
            return SUCCESS;
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//释放资源，必需成对出现
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  保存雷管测试结果
  * @param  无
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月12日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus WriteDetTestRecord(void)
{
	DIR dir;	
	u8 res;
	u8 UpdataFlag = 0;
	u32 TodayCount, TodayErrorCount;
	RTC_Typedef time;
	u8 i,Buffer[20];
	u8 type;
	
	time = RTC_Type;
	sprintf((char*)Buffer, "%02d%02d%02d",time.year-2000, time.month, time.day);
	if(ERROR == ReadDetTestRecordTestCount(Buffer, &TodayCount, &TodayErrorCount))
	{
		TodayCount = 0;
		TodayErrorCount = 0;
	}
	
	FileSystemLock();//申请文件资源，必需成对出现
	res = f_opendir(&dir, DET_TEST_RECORD_FILE_PATH);
	if (res != FR_OK)
	{//创建目录
		res = f_mkdir(DET_TEST_RECORD_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//释放资源，必需成对出现
			return ERROR;
		}
	}
	res = f_chmod (DET_TEST_RECORD_FILE_PATH, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
	if (res != FR_OK)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}

	sprintf((char*)f_Buffer, "%s/%02d%02d%02d.txt", DET_TEST_RECORD_FILE_PATH, time.year-2000, time.month, time.day);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_lseek(&f_fdst,f_size(&f_fdst));
	if(0==f_tell(&f_fdst))
	{//该文件为新建，需要写入表头
		UpdataFlag = 1;//一条新记录
		
		f_printf(&f_fdst, "总数:%07ld\r\n", TodayCount);
		f_printf(&f_fdst, "不合格数:%07ld\r\n", TodayErrorCount);
		f_puts("\r\n检测时间\t检测环节\t通道\t  雷管内码  \t   雷管外码   \t芯片频率\t延期\t类型\t静态电流(正向)\t静态电流(反向)\t充电电流(正向)\t充电电流(反向)\t状态码",&f_fdst);
	}
	
	//时间
	f_printf(&f_fdst, "\r\n%02d:%02d:%02d\t", time.hour, time.minute, time.second);
	
	//检测环节
	if(UserParam.Stage == FINISHED)
	{
		f_printf(&f_fdst, "%s\t", "成品检  ");
	}
	else if(UserParam.Stage == SEMIFINISHED)
	{
		f_printf(&f_fdst, "%s\t", "半成品检");
	}
	else if(UserParam.Stage == DET_CODE_VERIFY)
	{
		f_printf(&f_fdst, "%s\t", "内码验证");
	}
	else if(UserParam.Stage == ENTER_FACTORY)
	{
		f_printf(&f_fdst, "%s\t", "入厂检  ");
	}  
	else
	{
		f_printf(&f_fdst, "%s\t", "--------");
	}
    
	for(i = 0; i < Channel.Count; i++)
	{
		TodayCount++;
		if(DET_PASS != Channel.XA2_Info[i].Status)
		{
			TodayErrorCount++;
		}
		if(i != 0)
		{
			f_puts("\r\n\t\t\t\t",&f_fdst);
		}
		f_printf(&f_fdst, "%d\t", i+1);//通道
		
		//内码
		f_printf(&f_fdst, "%02X%02X%02X%02X%02X%02X%02X%02X%\t",
			Channel.XA2_Info[i].Serl.U8Buffer[0],Channel.XA2_Info[i].Serl.U8Buffer[1],Channel.XA2_Info[i].Serl.U8Buffer[2],
		Channel.XA2_Info[i].Serl.U8Buffer[3],Channel.XA2_Info[i].Pwd.U8Buffer[0],Channel.XA2_Info[i].Pwd.U8Buffer[1],
		Channel.XA2_Info[i].Pwd.U8Buffer[2],Channel.XA2_Info[i].Pwd.U8Buffer[3]);
		//外码
		if(Channel.BarCode[i].Status == BARCODE_WR_END)
		{
			f_printf(&f_fdst, "%s\t", Channel.BarCode[i].CodeExternal);
		}
		else
		{
			f_printf(&f_fdst, "%s\t", "-------------");
		}		
		
		//频率
		f_printf(&f_fdst, "%dHz\t\t", Channel.XA2_Info[i].freq);

		//延期
		f_printf(&f_fdst, "%d\t", UserParam.DetDelay);			
		
		//类型

		type = ((Channel.XA2_Info[i].Pwd.U8Buffer[1] & 0x03)<<3) + ((Channel.XA2_Info[i].Pwd.U8Buffer[2] & 0xE0)>>5);
		f_printf(&f_fdst, "0x%02X\t", type);

		//静态电流(正向)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].lv_current/100, Channel.XA2_Info[i].lv_current%100);

		//静态电流(反向)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].lv_reverse_current/100, Channel.XA2_Info[i].lv_reverse_current%100);
		
		//充电电流(正向)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].hv_current/100, Channel.XA2_Info[i].hv_current%100);

		//充电电流(反向)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].hv_reverse_current/100, Channel.XA2_Info[i].hv_reverse_current%100);
		

		//状态码
		f_printf(&f_fdst, "0x%02X", (u8)Channel.XA2_Info[i].Status);	
	}

	
	f_printf(&f_fdst, "%s", "\r\n");

	f_lseek(&f_fdst,0);
	f_printf(&f_fdst, "总数:%07ld\r\n", TodayCount);
	f_printf(&f_fdst, "不合格数:%07ld\r\n", TodayErrorCount);	
	f_close(&f_fdst);
	
	FileSystemUnlock();//释放资源，必需成对出现
	
	if(UpdataFlag)
	{
		if(DetTestRecorder.Number >= DET_TEST_RECORDER_MAX_NUM)
		{
			DeleteDetTestRecordFile(DET_TEST_RECORDER_MAX_NUM-1);//删除最旧的一条记录
		}
		else
		{
			DetTestRecordFileInit();
		}
	}
	
	return SUCCESS;
}


