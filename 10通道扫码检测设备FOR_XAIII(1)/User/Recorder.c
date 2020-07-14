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
//| �������� | GetSector
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������ĵ�ַ���ض�Ӧ�е�FLASH������
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� | 	��
//|----------|--------------------------------------------------------------------------------------
//| ������� |	Address FLASH��ַ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	FLASH ������
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� |  ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-05-22 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | ReadAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ������AT45DB�е����ݲ��ж�CRCУ���ֽ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | addr    Ҫ��ȡ��������ʼ��ַ
//|          | length  Ҫ��ȡ�����ݳ���(������2�ֽ�CRC16У��)
//|          | ptr     Ҫ��������ݻ�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | TRUE    ��ȡ�ɹ���У��ͨ��
//|          | FALSE   У��ʧ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ����ʹ��SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | WriteAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �������ݵ�AT45DB�в�����CRCУ���ֽ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | addr    Ҫд�����ʼ��ַ
//|          | length  Ҫд������ݳ���(������2�ֽ�CRC16У��)
//|          | ptr     Ҫд������ݻ�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | TRUE    д�ɹ�
//|          | FALSE   дʧ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ����ʹ��SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
	
	//�豸ID
	ReadFlashInfoCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE, DeviceInfo.ID);
	
	//�豸Ӳ���汾
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
//	{//����Ĭ��֧�ֵĹ�������
//		u8 i=0;
//		
//		memset(FactoryCodeSupport, 0, FACTORY_CODE_SUPPORT_MAX);
//		
//		FactoryCodeSupport[i++] = 44;//������е����Ӧ0x01
//		FactoryCodeSupport[i++] = 34;//������˿����Ӧ0x02
//		FactoryCodeSupport[i++] = 56;//�Ĵ��˱�������Ӧ0x03
//		FactoryCodeSupport[i++] = 10;//���ɺ����Ӧ0x04
//        FactoryCodeSupport[i++] = 36;//ɽ��ʥ�����Ӧ0x05
//        FactoryCodeSupport[i++] = 53;//�����𽨻���Ӧ0x06
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
	
	//����FLASH
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	if(FLASH_COMPLETE != FLASH_EraseSector(GetSector(FACTORY_CFG_START_ADDR), VoltageRange_3))
	{
		return ERROR;		
	}

	//�豸ID
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE,DeviceInfo.ID))
	{
		return ERROR;
	}	
	
	//�豸Ӳ���汾
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW))
	{
		return ERROR;
	}

	//дУ׼����
	temp[0] = ((u16)(AdcAdjust.Current.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.Current.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.Current.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.Current.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_CURRENT_ADDR, FACTORY_CFG_ADJUST_CURRENT_SIZE, temp))
	{
		return ERROR;
	}
	//дУ׼����
	temp[0] = ((u16)(AdcAdjust.LinAB1To8.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB1To8.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB1To8.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB1To8.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_1TO8_ADDR, FACTORY_CFG_ADJUST_1TO8_SIZE, temp))
	{
		return ERROR;
	}
	//дУ׼����
	temp[0] = ((u16)(AdcAdjust.LinAB9.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB9.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB9.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB9.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_9_ADDR, FACTORY_CFG_ADJUST_9_SIZE, temp))
	{
		return ERROR;
	}
	//дУ׼����
	temp[0] = ((u16)(AdcAdjust.LinAB10.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.LinAB10.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.LinAB10.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.LinAB10.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_10_ADDR, FACTORY_CFG_ADJUST_10_SIZE, temp))
	{
		return ERROR;
	}
	//дУ׼����
	temp[0] = ((u16)(AdcAdjust.Bat.k*1000))&0xFF;
	temp[1] = ((u16)(AdcAdjust.Bat.k*1000))>>8;
	temp[2] = ((u16)AdcAdjust.Bat.b)&0xFF;
	temp[3] = ((u16)AdcAdjust.Bat.b)>>8;
	if(ERROR == WriteFlashInfoCRC16(FACTORY_CFG_ADJUST_BAT_ADDR, FACTORY_CFG_ADJUST_BAT_SIZE, temp))
	{
		return ERROR;
	}	
	//дУ׼ʱ��
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
		temp[i--] = value%10 + '0'; //��λת�������õ���洢�ַ���ʡȥ�˺������ַ��ߵ�����
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
			{//�ҵ���Ӧ�ļ�ֵ
				if(NULL != f_gets((char*)f_Buffer, 512, &f_fdst))
				{
					if(buff == NULL)
					{//����Ҫ��������
						result = SUCCESS;
						break;						
					}						
					else						
					{
						if((buff_size > strlen((char*)f_Buffer)) && (strlen((char*)f_Buffer) < 50))
						{
							GetCrc16Value(0xFFFF, &crc16[0],f_Buffer,strlen((char*)f_Buffer));//����������CRC16У��ֵ
							if(NULL != f_gets((char*)&f_Buffer[200], 10, &f_fdst))//��ȡ�����ļ��е�CRC16У��ֵ
							{
								sprintf((char*)&f_Buffer[250],"%02X%02X\n",crc16[0],crc16[1]);
								if(strcmp((const char*)&f_Buffer[200], (const char*)&f_Buffer[250])==0)//У��ֵ�Ƚ�
								{
									memcpy(buff, &f_Buffer[0], strlen((char*)f_Buffer)+1);	
									result = SUCCESS;
									break;								
								}								
							}
						}
						else
						{//����Ļ������ռ䲻��
							result = ERROR;			
							break;							
						}
					}			
				}
				else
				{//�м�ֵ����û����
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
	u8 buf[11];//0xFFFFFFFF��Ӧ��10��������Ϊ4294967295��ռ10λ
	
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
	
	//д���ֵ
	memset(f_Buffer, 0, sizeof(f_Buffer));
	f_Buffer[0] = '[';
	strcpy((char*)&f_Buffer[1], (char*)key);
	f_Buffer[strlen((const char*)f_Buffer)] = ']';
	f_Buffer[strlen((const char*)f_Buffer)] = '\n';
	f_puts((char*)f_Buffer, &f_fdst);	
	
	//д�����ݼ�У��ֵ
	memset(f_Buffer, 0, sizeof(f_Buffer));	
	strcpy((char*)&f_Buffer[0], (char*)buf);
	f_Buffer[50] = '\n';
	GetCrc16Value(0xFFFF, &f_Buffer[100], buf, strlen((const char*)buf));
	sprintf((char*)&f_Buffer[51], "%02X%02X\n",f_Buffer[100], f_Buffer[101]);
	f_write(&f_fdst, (char*)f_Buffer, 56, &f_rw);//�򻯲�����д��̶�����
	f_close(&f_fdst);
	return SUCCESS;	
}
ErrorStatus WriteKeyU32Value(u8* path,u8* key, u32 value)
{
	u8 buf[11];//0xFFFFFFFF��Ӧ��10��������Ϊ4294967295��ռ10λ
	
	U32ToAscii(value, buf);
	return WriteKeyString(path, key, buf);
}
/**
  *************************************************************************************
  * @brief  ���û����õĲ���ֵ
  * @param  �� 
  * @retval �� 
  * @author ZH
  * @date   2014��9��28��
  * @note   ����ֵ�������ļ�ϵͳ�е����ò���.txt
  *************************************************************************************  
  */
void ReadUserConfigParam(void)
{
	u32 value;
	u8 Buff[11];
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�׹�����",&value))
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
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "����д��",&value))
	{
		UserParam.DetCodeWrite = value;
	}
	else
	{
		UserParam.DetCodeWrite = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��⻷��",&value))
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
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "����", &value))
	{
		UserParam.Sound = (SOUND_CFG)value;
	}
	else
	{
		UserParam.Sound = FINISHED_RING;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��Ļ����",&value))
	{
		UserParam.LcdBright = value;
	}
	else
	{
		UserParam.LcdBright = LCD_BRIGHT_RANK-1;
	}

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "���ʱ��Ļ",&value))
	{
		UserParam.DetTestLcdStatus = (DET_TEST_LCD_STATUS_CFG)value;
	}
	else
	{
		UserParam.DetTestLcdStatus = DET_TEST_LCD_STATUS_ON;
	}

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��������",&value))
	{
		UserParam.FactoryCode = value;
	}
	else
	{
		UserParam.FactoryCode = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "����ʱ��",&value))
	{
		UserParam.ProductTime = value;
	}
	else
	{
		UserParam.ProductTime = 0;
	}
    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "������",&value))
	{
		UserParam.Feature = value;
	}
	else
	{
		UserParam.Feature = 0;
	}
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�׹�����",&value))
	{
		UserParam.DetDelay = value;
	}
	else
	{
		UserParam.DetDelay = 0;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "���ڼ��",&value))
	{
		UserParam.CheckDelay = value;
	}
	else
	{
		UserParam.CheckDelay = 1;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "����д��",&value))
	{
		UserParam.DelayWrite = value;
	}
	else
	{
		UserParam.DelayWrite = 0;
	}		
	
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�������",&value))
	{
		UserParam.CheckDetCode = value;
	}
	else
	{
		UserParam.CheckDetCode = 1;
	}
	if(XA2_DetIsFixDelay(UserParam.DetType) == 0)
	{//�ɱ���׹�
		UserParam.DetDelay = 0;
	}
	else if (UserParam.DetDelay  > GetMaxDelay(UserParam.DetType))
	{
		UserParam.DetDelay = 0;
	}    
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "ͨ������",&value))
	{
		Channel.Count = value;      
	}
	else
	{
		Channel.Count = 10;
	}
    if((Channel.Count != 1) && (Channel.Count != 5) && (Channel.Count != 10))//Ŀǰ��֧��10ͨ����5ͨ���͵�ͨ��ģʽ
    {
        Channel.Count = 10;
    }
    
 	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�������",&value))
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

	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�����ܳ�",&value))
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

	if(ERROR != ReadKeyString(CONFIG_PARAM_FILE_COMPLETE_PATH, "��ʼ����ǰ8λ",Buff,11))
	{
		memcpy(UserParam.DetOutsideStartHead, &Buff[0],sizeof(Buff));
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��ʼ�����5λ",&value))
	{
		UserParam.DetOutsideStartBoxIndex = value;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��������",&value))
	{
		UserParam.InBoxNumber = value;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "�Զ�����",&value))
	{
		UserParam.AutoBack = value;
	}
	else
	{
		UserParam.AutoBack = 0;
	}
	if(ERROR != ReadKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, "��ʷ�������",&value))
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
	{//Ĭ�ϲ���
		//�볧��
		test_param_cfg[0].det_lv_current_min = 1200;
		test_param_cfg[0].det_lv_current_max = 2800;
		test_param_cfg[0].det_lv_current_time_out = 3;
		test_param_cfg[0].det_hv_current_min = 1200;
		test_param_cfg[0].det_hv_current_max = 3000;
		test_param_cfg[0].det_hv_current_time_out = 15;
		test_param_cfg[0].det_freq_min = 62000UL;
		test_param_cfg[0].det_freq_max = 103000UL;
		test_param_cfg[0].test_ctrl = 0x03;
		
		//���Ʒ
		test_param_cfg[1].det_lv_current_min = 1200;
		test_param_cfg[1].det_lv_current_max = 3000;
		test_param_cfg[1].det_lv_current_time_out = 3;
		test_param_cfg[1].det_hv_current_min = 1200;
		test_param_cfg[1].det_hv_current_max = 3200;
		test_param_cfg[1].det_hv_current_time_out = 15;
		test_param_cfg[1].det_freq_min = 61000UL;
		test_param_cfg[1].det_freq_max = 104000UL;
		test_param_cfg[1].test_ctrl = 0x0F;
		
		//��Ʒ
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
  * @brief  ��ʼ������¼����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��6��09��
  * @note   ����¼���ݱ����ڽṹ��DetDetectRecorder��
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
	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���

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
							if(DetTestRecorder.Number < DET_TEST_RECORDER_MAX_NUM)
							{//����Ҫ����ļ���δ����Ҫ��
//								FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//								if(DetDetectRecordFileVerify((u8*)fn) != ERROR)
//								{//�ļ�У��ͨ����û�б��޸�
									memcpy(&DetTestRecorder.FileName[DetTestRecorder.Number][0], fn, strlen("141206"));
									DetTestRecorder.Number++;	
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
	
	//�ļ����Ӵ�С����
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
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}
/**
  *************************************************************************************
  * @brief  BJоƬ����󶨳�ʼ����¼����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��6��09��
  * @note   ����¼���ݱ����ڽṹDetDataBindingRecorder
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
	
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���

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
							if(DetDataBindingRecorder.Number < DET_DATA_BINDING_RECORDER_MAX_NUM)
							{//����Ҫ����ļ���δ����Ҫ��
//								FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//								if(DetDetectRecordFileVerify((u8*)fn) != ERROR)
//								{//�ļ�У��ͨ����û�б��޸�
									memcpy(&DetDataBindingRecorder.FileName[DetDataBindingRecorder.Number][0], fn, strlen("141206"));
									DetDataBindingRecorder.Number++;	
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
	
	//�ļ����Ӵ�С����
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
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
}
/**
  *************************************************************************************
  * @brief  ɾ��ָ����Ŀ¼�µ������ļ�
  * @param  Path ָ��Ŀ¼
  * @retval ��
  * @author ZH
  * @date   2015��6��23��
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
  * @brief  ɾ����¼�ļ�
  * @param  Index ������
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
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
  * @brief  ɾ������󶨼�¼�ļ�
  * @param  Index ������
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
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
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	f_mkdir(CONFIG_PARAM_FILE_PATH);	
	
	res=f_open(&f_fdst,TEST_PARAM_FILE_COMPLETE_PATH,FA_WRITE|FA_CREATE_ALWAYS); 
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst,0);
		
	res=f_write(&f_fdst, (char*)&test_param_cfg[0], sizeof(TEST_PARAM)*3, (UINT*)&bw); //д��Ŀ���ļ�
	if(res || (bw != sizeof(TEST_PARAM)*3))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}

ErrorStatus ReadDetTestParamCfg(void)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	res = f_open(&f_fdst,TEST_PARAM_FILE_COMPLETE_PATH,FA_READ|FA_OPEN_EXISTING); 
	if(res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	if(f_size(&f_fdst) != (sizeof(TEST_PARAM)*3))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	
	f_lseek(&f_fdst,0);	
	
	memset((char*)&test_param_cfg[0],0,sizeof(TEST_PARAM)*3);
	res = f_read(&f_fdst, (char*)&test_param_cfg[0], sizeof(TEST_PARAM)*3,(UINT*)&bw);
	if((res != FR_OK) || ((sizeof(TEST_PARAM)*3) != bw))
	{
		f_close(&f_fdst);
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;	
	}
	
	f_close(&f_fdst);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
//ErrorStatus SaveDetTestQueueInfo(void)
//{
//    FRESULT res;         /* FatFs function common result code */
//    UINT  bw;         /* File read/write count */
//	
//	FileSystemLock();//�����ļ���Դ������ɶԳ���
//	
//	f_mkdir(DET_INFO_QUEUE_FILE_PATH);	
//	
//	res=f_open(&f_fdst,DET_INFO_QUEUE_FILE_COMPLETE_PATH,FA_WRITE|FA_CREATE_ALWAYS); 
//	if(res != FR_OK)
//	{
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//		return ERROR;
//	}
//	f_lseek(&f_fdst,0);
//	res=f_write(&f_fdst, (char*)&OpData.DetDetectorPassCount, sizeof(OpData.DetDetectorPassCount), (UINT*)&bw); //д��Ŀ���ļ�
//	if(res || (bw != sizeof(OpData.DetDetectorPassCount)))
//	{
//		f_close(&f_fdst);
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//		return ERROR;
//	}
//	
//	res=f_write(&f_fdst, (char*)&DetInfoQueue, sizeof(DET_INFO_QUEUE), (UINT*)&bw); //д��Ŀ���ļ�
//	if(res || (bw != sizeof(DET_INFO_QUEUE)))
//	{
//		f_close(&f_fdst);
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//		return ERROR;
//	}
//	f_close(&f_fdst);
//	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//	return SUCCESS;
//}

//ErrorStatus ReadDetTestQueueInfo(void)
//{
//    FRESULT res;         /* FatFs function common result code */
//    UINT  bw;         /* File read/write count */
//	
//	FileSystemLock();//�����ļ���Դ������ɶԳ���
//	ClearDetInfoQueue(&DetInfoQueue);
//	OpData.DetDetectorPassCount = 0;
//	res = f_open(&f_fdst,DET_INFO_QUEUE_FILE_COMPLETE_PATH,FA_READ|FA_OPEN_EXISTING); 
//	if(res != FR_OK)
//	{
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//		return ERROR;
//	}
//	if(f_size(&f_fdst) != (sizeof(OpData.DetDetectorPassCount)+sizeof(DET_INFO_QUEUE)))
//	{
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
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
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
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
//		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//		return ERROR;	
//	}
//	
//	f_close(&f_fdst);
//	
//	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
//	return SUCCESS;
//}

/**
  *************************************************************************************
  * @brief  �����ļ�
  * @param  psrc Դ�ļ�·��
  * @param  pdst Ŀ���ļ�·�� 
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
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
	res = f_open(&f_fdst,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING); //��ֻ���ļ�
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
		res=f_read(&f_fdst,f_Buffer,sizeof(f_Buffer),(UINT*)&br); //Դͷ����512�ֽ�
		if(res||(br==0))
		{
			f_close(&f_fdst);
			break;
		}		

		f_lseek(&Msc_fdst,NextIndex);
		res=f_write(&Msc_fdst,f_Buffer,(UINT)br,(UINT*)&bw); //д��Ŀ���ļ�
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
  * @brief  ���Ƴ���¼�ļ���U��
  * @param  Index ��¼�ļ����� 
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyDetTestFileToMsc(u16 Index)
{
	char Buffer1[50],FileName[17], Buffer2[50],DstPath[30];
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
 	/* �����ļ�ϵͳ */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));
	strcpy(DstPath, "1:ɨ���豸(MH)��������");
	f_mkdir((const char*)DstPath);			
	
	//ֻ����1������
	memset(FileName,0, sizeof(FileName));
	memcpy(FileName, &DetTestRecorder.FileName[Index], sizeof(DetTestRecorder.FileName[0]));
	
	sprintf(Buffer1,"%s/%s.txt", DET_TEST_RECORD_FILE_PATH, FileName);
	sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
	if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	
	
	/* ж���ļ�ϵͳ */
	f_mount(&MscFatfs,"1:", 1);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  ���Ƴ���¼�ļ���U��
  * @param  Index ��¼�ļ����� 
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyDetDataBindingFileToMsc(u16 Index)
{
	char Buffer1[50],FileName[17], Buffer2[50],DstPath[30];
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
 	/* �����ļ�ϵͳ */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));
	strcpy(DstPath, "1:ɨ���豸(MH)�����");
	f_mkdir((const char*)DstPath);			
	
	//ֻ����1������
	memset(FileName,0, sizeof(FileName));
	memcpy(FileName, &DetDataBindingRecorder.FileName[Index], sizeof(DetDataBindingRecorder.FileName[0]));
	
	sprintf(Buffer1,"%s/%s.txt", DET_DATA_BINDING_RECORD_FILE_PATH, FileName);
	sprintf(Buffer2,"%s/%s.txt", DstPath, FileName);
	if (FR_OK != FileCopy((u8*)&Buffer1[0], (u8*)&Buffer2[0]))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}	
	
	
	/* ж���ļ�ϵͳ */
	f_mount(&MscFatfs,"1:", 1);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  ��U�̸���HEX�ļ���TF��
  * @param  Index ��¼�ļ����� 
  * @retval ���
  * @author ZH
  * @date   2015��6��23��
  * @note   
  *************************************************************************************  
  */
ErrorStatus CopyHexFileToSdCard(void)
{
	char FileName[17], DstPath[30];
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
 	/* �����ļ�ϵͳ */
	if (f_mount(&MscFatfs,"1:", 1) != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	memset(DstPath,0, sizeof(DstPath));

	strcpy(FileName, "1:CPU.hex");
	strcpy(DstPath, "0:TEMP.hex");	

	
	if (FR_OK != FileCopy((u8*)&FileName[0], (u8*)&DstPath[0]))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	/* ж���ļ�ϵͳ */
	f_mount(&MscFatfs,"1:", 1);


	f_unlink("0:CPU.hex");
	if(FR_OK != f_rename("0:TEMP.hex","0:CPU.hex"))
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;	
	}

	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return SUCCESS;
}

ErrorStatus ReadDetTestRecordTestCount(u8* Name, u32* TotilNum, u32* ErrNum)
{
	u8 res;
	UINT  rw;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer, "%s/%s.txt", DET_TEST_RECORD_FILE_PATH, Name);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
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

		if(0 == memcmp("����:",f_Buffer, strlen("����:")))
		{
			*TotilNum = AsciiToU32(&f_Buffer[strlen("����:")]);
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

				if(0 == memcmp("���ϸ���:",f_Buffer, strlen("���ϸ���:")))
				{
					*ErrNum = AsciiToU32(&f_Buffer[strlen("���ϸ���:")]);	
					f_close(&f_fdst);
					
					FileSystemUnlock();//�ͷ���Դ������ɶԳ���
					return SUCCESS;
				}
			}
		}
	}
	f_close(&f_fdst);
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	return ERROR;
}
ErrorStatus ReadDetDataBindingRecordTestCount(u8* Name, u32* TotilNum)
{
	u8 res;
	UINT  rw;
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	
	sprintf((char*)f_Buffer, "%s/%s.txt", DET_DATA_BINDING_RECORD_FILE_PATH, Name);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
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

		if(0 == memcmp("���������:",f_Buffer, strlen("���������:")))
		{
			*TotilNum = AsciiToU32(&f_Buffer[strlen("���������:")]);
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
  * @brief  �����׹ܲ��Խ��
  * @param  ��
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��12��
  * @note   ��
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
	
	FileSystemLock();//�����ļ���Դ������ɶԳ���
	res = f_opendir(&dir, DET_TEST_RECORD_FILE_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(DET_TEST_RECORD_FILE_PATH);
		if (res != FR_OK)
		{
			FileSystemUnlock();//�ͷ���Դ������ɶԳ���
			return ERROR;
		}
	}
	res = f_chmod (DET_TEST_RECORD_FILE_PATH, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
	if (res != FR_OK)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}

	sprintf((char*)f_Buffer, "%s/%02d%02d%02d.txt", DET_TEST_RECORD_FILE_PATH, time.year-2000, time.month, time.day);
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//�ͷ���Դ������ɶԳ���
		return ERROR;
	}
	f_lseek(&f_fdst,f_size(&f_fdst));
	if(0==f_tell(&f_fdst))
	{//���ļ�Ϊ�½�����Ҫд���ͷ
		UpdataFlag = 1;//һ���¼�¼
		
		f_printf(&f_fdst, "����:%07ld\r\n", TodayCount);
		f_printf(&f_fdst, "���ϸ���:%07ld\r\n", TodayErrorCount);
		f_puts("\r\n���ʱ��\t��⻷��\tͨ��\t  �׹�����  \t   �׹�����   \tоƬƵ��\t����\t����\t��̬����(����)\t��̬����(����)\t������(����)\t������(����)\t״̬��",&f_fdst);
	}
	
	//ʱ��
	f_printf(&f_fdst, "\r\n%02d:%02d:%02d\t", time.hour, time.minute, time.second);
	
	//��⻷��
	if(UserParam.Stage == FINISHED)
	{
		f_printf(&f_fdst, "%s\t", "��Ʒ��  ");
	}
	else if(UserParam.Stage == SEMIFINISHED)
	{
		f_printf(&f_fdst, "%s\t", "���Ʒ��");
	}
	else if(UserParam.Stage == DET_CODE_VERIFY)
	{
		f_printf(&f_fdst, "%s\t", "������֤");
	}
	else if(UserParam.Stage == ENTER_FACTORY)
	{
		f_printf(&f_fdst, "%s\t", "�볧��  ");
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
		f_printf(&f_fdst, "%d\t", i+1);//ͨ��
		
		//����
		f_printf(&f_fdst, "%02X%02X%02X%02X%02X%02X%02X%02X%\t",
			Channel.XA2_Info[i].Serl.U8Buffer[0],Channel.XA2_Info[i].Serl.U8Buffer[1],Channel.XA2_Info[i].Serl.U8Buffer[2],
		Channel.XA2_Info[i].Serl.U8Buffer[3],Channel.XA2_Info[i].Pwd.U8Buffer[0],Channel.XA2_Info[i].Pwd.U8Buffer[1],
		Channel.XA2_Info[i].Pwd.U8Buffer[2],Channel.XA2_Info[i].Pwd.U8Buffer[3]);
		//����
		if(Channel.BarCode[i].Status == BARCODE_WR_END)
		{
			f_printf(&f_fdst, "%s\t", Channel.BarCode[i].CodeExternal);
		}
		else
		{
			f_printf(&f_fdst, "%s\t", "-------------");
		}		
		
		//Ƶ��
		f_printf(&f_fdst, "%dHz\t\t", Channel.XA2_Info[i].freq);

		//����
		f_printf(&f_fdst, "%d\t", UserParam.DetDelay);			
		
		//����

		type = ((Channel.XA2_Info[i].Pwd.U8Buffer[1] & 0x03)<<3) + ((Channel.XA2_Info[i].Pwd.U8Buffer[2] & 0xE0)>>5);
		f_printf(&f_fdst, "0x%02X\t", type);

		//��̬����(����)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].lv_current/100, Channel.XA2_Info[i].lv_current%100);

		//��̬����(����)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].lv_reverse_current/100, Channel.XA2_Info[i].lv_reverse_current%100);
		
		//������(����)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].hv_current/100, Channel.XA2_Info[i].hv_current%100);

		//������(����)
		f_printf(&f_fdst, "%d.%02d\t\t", Channel.XA2_Info[i].hv_reverse_current/100, Channel.XA2_Info[i].hv_reverse_current%100);
		

		//״̬��
		f_printf(&f_fdst, "0x%02X", (u8)Channel.XA2_Info[i].Status);	
	}

	
	f_printf(&f_fdst, "%s", "\r\n");

	f_lseek(&f_fdst,0);
	f_printf(&f_fdst, "����:%07ld\r\n", TodayCount);
	f_printf(&f_fdst, "���ϸ���:%07ld\r\n", TodayErrorCount);	
	f_close(&f_fdst);
	
	FileSystemUnlock();//�ͷ���Դ������ɶԳ���
	
	if(UpdataFlag)
	{
		if(DetTestRecorder.Number >= DET_TEST_RECORDER_MAX_NUM)
		{
			DeleteDetTestRecordFile(DET_TEST_RECORDER_MAX_NUM-1);//ɾ����ɵ�һ����¼
		}
		else
		{
			DetTestRecordFileInit();
		}
	}
	
	return SUCCESS;
}


