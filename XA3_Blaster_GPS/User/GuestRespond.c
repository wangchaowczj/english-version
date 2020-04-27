#include "includes.h"
#include "GuiIncludes.h"

STR_COMM  GusetSendBuffer;
STR_COMM  GusetTempBuffer;

/**
  *************************************************************************************
  * @brief  获取常规的响应数据
  * @param  buffer：数据缓冲区
  * @param  Cmd：命令
  * @param  PackIndex：分包号  
  * @retval 无 
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
void GetGeneralRespondData(STR_COMM* buffer, u16 Cmd, u16 PackIndex)
{
	buffer->bytenum = 0;
	buffer->uch_buf[buffer->bytenum++] = device_info.name;
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd>>8);
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd&0xFF);
	buffer->uch_buf[buffer->bytenum++] = (PackIndex>>8)&0xFF;
	buffer->uch_buf[buffer->bytenum++] = PackIndex&0xFF;
}
/**
  *************************************************************************************
  * @brief  获取缓冲区中的字符串长度
  * @param  buffer：数据缓冲区
  * @param  MaxLenght：数据缓冲区长度 
  * @retval 字符串长度 
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
u16 GetBufferStringLenght(u8* Buffer, u16 MaxLenght)
{
	u16 i;
	
	for(i=0;i<MaxLenght;i++)
	{
		if(Buffer[i] == 0x00)
		{
			break;
		}
	}
	return i;
}

/**
  *************************************************************************************
  * @brief  从缓冲区中的数值
  * @param  buffer：数据缓冲区
  * @param  MaxLenght：数据缓冲区长度 
  * @retval 数值
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
u32 GetDecValueFromBuffer(u8* Buffer, u16 MaxLenght)
{
	u8 i;
	u32 Value = 0;
	
	for(i=0;i<MaxLenght;i++)
	{
		if ((Buffer[i] < '0') || (Buffer[i] > '9'))
		{
			break;
		}
		Value *= 10;
		Value += (Buffer[i]-'0');
	}
	return Value;
}

/**
  *************************************************************************************
  * @brief  从分隔符中取数据
  * @param  Separator：分隔符
  * @param  SeparatorNumber：分隔符序号
  * @param  Buffer：原始数据
  * @param  Bytes：原始数据长度
  * @param  StartIndex：得到的数据在Buffer中的起始索引值
  * @param  DataBytes：得到的数据长度  
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   “123\rABCD\rGL\r”,如想取出"ABCD"，Separator SeparatorNumber 分别为'\r',2
  *************************************************************************************  
  */
ErrorStatus GetDataFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u16* StartIndex, u16* DataBytes)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				*StartIndex = LastIndex+1;
				*DataBytes = i-LastIndex-1;
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//分隔符不足
}

/**
  *************************************************************************************
  * @brief  从分隔符中取10进制数值
  * @param  Separator：分隔符
  * @param  SeparatorNumber：分隔符序号
  * @param  Buffer：原始数据
  * @param  Bytes：原始数据长度
  * @param  Value：数值
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   “123\rABCD\rGL\r”,如想取出"123"，Separator SeparatorNumber 分别为'\r',1
  *************************************************************************************  
  */
ErrorStatus GetDecValueFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u32* Value)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	u8 temp[20];
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				memset(temp,0,20);
				memcpy(&temp[0], &Buffer[LastIndex+1], i-LastIndex-1);
				sscanf((const char*)&temp[0],"%uld",Value);
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//分隔符不足
}

/**
  *************************************************************************************
  * @brief  USB数据响应
  * @param  buffer_in：接收到的数据
  * @param  buffer_out：响应的数据
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
void  GuestRespond(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 Cmd = 0, HostPack = 0;
//	static u16 GuestPack = 0, LastValue;
	u8 U8Value;
	u16 U16Value1, U16Value2;
	u32 U32Value;
	u8 Temp1[100]/*, Temp2[100]*/;
//	u16 i,j;
	ErrorStatus Result;
	OS_STK_DATA StkData;
	
	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == device_info.name) || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
			case 0x0200://读软件版本
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0301://读SN码，也就是STM32芯片惟一ID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
                    GetSTM32ChipIDStr(&buffer_out->uch_buf[buffer_out->bytenum]);
					buffer_out->bytenum += 24;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;                
			case 0x0201://读ID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&device_info.mac[0], sizeof(device_info.mac));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&device_info.mac[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0202://写ID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(device_info.mac))
					{
						memset(&device_info.mac[0], 0, sizeof(device_info.mac));
						if (U16Value1 > 0)
						{
							memcpy((char*)&device_info.mac[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						

						if(ERROR != CreateNewFileData(FACTORY_CFG_FILE_PATH, DEVICE_ID_FILE_NAME, &device_info.mac[0], sizeof(device_info.mac)))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
						}
					}
					else
					{//要写入的ID长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0203://读硬件版本
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&device_info.hw_version[0], sizeof(device_info.hw_version));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&device_info.hw_version[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0204://写硬件版本
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(device_info.hw_version))
					{
						memset(&device_info.hw_version[0], 0, sizeof(device_info.hw_version));
						if (U16Value1 > 0)
						{
							memcpy((char*)&device_info.hw_version[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}
						
						if(ERROR != CreateNewFileData(FACTORY_CFG_FILE_PATH, HW_VERSION_FILE_NAME, &device_info.hw_version[0], sizeof(device_info.hw_version)))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
						}				
					}
					else
					{//要写入的ID长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';						
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}				
				break;
			case 0x0210://读设备参数
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r%lu\r%lu\r%lu\r%lu\r",
						device_info.DeviceType ,
						device_info.single_current_min, device_info.single_current_max,
						device_info.det_freq_min, device_info.det_freq_max);
				}				
				break;
			case 0x0211://保存设备参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据					
					
					//设备类型
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					U32Value &= 0xFF;
					
					//如果设备类型被改变,用户必需重启设备,先暂时设置起爆记录数为0，避免上传记录时出错
					if (U32Value != device_info.DeviceType)
					{
						BlastRecorder.Number = 0;
					}

					device_info.DeviceType = U32Value;
					
					if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, DEVICE_TYPE_FILE_NAME, (u8*)&device_info.DeviceType, sizeof(device_info.DeviceType)))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					
					//单发电流最小、最大值
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					device_info.single_current_min = U32Value;	
					
					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					device_info.single_current_max = U32Value;
					
					Temp1[0] = device_info.single_current_min>>8;
					Temp1[1] = device_info.single_current_min&0xFF;
					Temp1[2] = device_info.single_current_max>>8;
					Temp1[3] = device_info.single_current_max&0xFF;
					if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, DET_CURRENT_FILE_NAME, (u8*)&Temp1[0], 4))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}

					//芯片频率最小、最大值
					if(ERROR == GetDecValueFromSeparator('\r',4,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					device_info.det_freq_min = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',5,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
						
					}
					device_info.det_freq_max = U32Value;
					
					Temp1[0] = device_info.det_freq_min>>24;
					Temp1[1] = device_info.det_freq_min>>16;
					Temp1[2] = device_info.det_freq_min>>8;
					Temp1[3] = device_info.det_freq_min&0xFF;
					Temp1[4] = device_info.det_freq_max>>24;
					Temp1[5] = device_info.det_freq_max>>16;
					Temp1[6] = device_info.det_freq_max>>8;
					Temp1[7] = device_info.det_freq_max&0xFF;
					if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, DET_FREQ_FILE_NAME, (u8*)&Temp1[0], 8))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
			case 0x0214://读设备RTC时间
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%04d\r%02d\r%02d\r%02d\r%02d\r%02d\r",
						RTC_Type.year ,RTC_Type.month,RTC_Type.day,RTC_Type.hour,RTC_Type.minute,RTC_Type.second);
				}					
				break;
			case 0x0215://写设备RTC时间
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					RTC_Type_Temp.year = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					RTC_Type_Temp.month = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r', 3, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}					
					RTC_Type_Temp.day = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r', 4, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					RTC_Type_Temp.hour = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r', 5, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					RTC_Type_Temp.minute = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r', 6, &buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					RTC_Type_Temp.second = U32Value;

					if(0 != RTC_Write_Time())
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';											
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';						
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0216://设备电压控制
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if (buffer_in->uch_buf[5] == '1')
					{//关闭总线电压
						CLOSE_BUS_VOLTAGE();						
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//输出低压状态
						OPEN_BUS_6_7V();
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//打开高压使能开关
						OPEN_HV_SW_INSIDE();
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//关闭高压使能开关
						CLOSE_HV_SW_INSIDE();
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//打开高压输出开关
						OPEN_HV_SW();
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//关闭高压输出开关
						CLOSE_HV_SW();
					}
					else if(buffer_in->uch_buf[5] == '7')
					{//关闭高压输出开关及高压使能开关
						CLOSE_HV_SW();
						CLOSE_HV_SW_INSIDE();
					}					
					else
					{						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}			
				break;
			case 0x0217://读总线电压控制状态
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(buffer_in->uch_buf[5] == '1')
					{//读总线电压控制状态
						if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
						{//当前总线电压已经关闭
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						}
						else if(GET_HV_SW_CTRL_STATE() == 0)
						{//高压还没输出,为低压状态
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						}					
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//内部高压开关使能状态
						if(GET_HV_ENABLE_SW_STATE() != 0)
						{//已经打开使能
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//是否已经按下安全锁
						if(GetChannelxVoltage(ADC_CH_11_3V, 10)>=2000)
						{//已经按下安全锁
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
					}					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0218://读设备电压或电流值
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(buffer_in->uch_buf[5] == '1')
					{//读总线电流测量值(包括电流值及AD测量值)
						U16Value1 = GetBusCurrent(10, &U16Value2);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//读总线电压测量值(包括电流值及AD测量值)
						U16Value1 = GetBusVoltage(10, &U16Value2);
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//读电池电压测量值(包括电流值及AD测量值)						
						//U16Value1 = GetBatteryVoltage(&U16Value2);
						//电池电压通过LTC2942测量得到
						U16Value1 = Op_Data.BatVoltage;
						U16Value2 = Op_Data.BatVoltage*0.2442002442;//1000/4095
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//读总线漏电测量值(包括电流值及AD测量值)
						U16Value1 = GetBusLeakCurrent(&U16Value2);
					}					
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);					
				}				
				break;
			case 0x0219://删除AD校准参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(buffer_in->uch_buf[5] == '1')
					{//删除总线电流AD校验参数(低压小量程)
						adc_adjust.low1current.k = 0;
						adc_adjust.low1current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//删除总线电流AD校验参数(低压大量程)
						adc_adjust.low2current.k = 0;
						adc_adjust.low2current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//删除总线电流AD校验参数(高压小量程)
						adc_adjust.high1current.k = 0;
						adc_adjust.high1current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//删除总线电流AD校验参数(高压大量程)
						adc_adjust.high2current.k = 0;
						adc_adjust.high2current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//删除总线电压AD校验参数
						adc_adjust.linab.k = 0;
						adc_adjust.linab.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//删除电池电压AD校验参数
//						adc_adjust.bat.k = 0;
//						adc_adjust.bat.b = 0;
//						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME);
//						f_unlink((const char*)Temp1);
						buffer_out->bytenum = 0;
						break;//电池电压测量改用LTC2942,不需要校准
					}					
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x021A://读AD校准参数
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(buffer_in->uch_buf[5] == '1')
					{//读总线电流AD校验参数(低压小量程)
						U16Value1 = (u16)(adc_adjust.low1current.k*1000);
						U16Value2 = adc_adjust.low1current.b;
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//读总线电流AD校验参数(低压大量程)
						U16Value1 = (u16)(adc_adjust.low2current.k*1000);
						U16Value2 = adc_adjust.low2current.b;					
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//读总线电流AD校验参数(高压小量程)
						U16Value1 = (u16)(adc_adjust.high1current.k*1000);
						U16Value2 = adc_adjust.high1current.b;						
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//读总线电流AD校验参数(高压大量程)
						U16Value1 = (u16)(adc_adjust.high2current.k*1000);
						U16Value2 = adc_adjust.high2current.b;						
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//读总线电压AD校验参数
						U16Value1 = (u16)(adc_adjust.linab.k*1000);
						U16Value2 = adc_adjust.linab.b;							
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//读电池电压AD校验参数
//						U16Value1 = (u16)(adc_adjust.bat.k*1000);
//						U16Value2 = adc_adjust.bat.b;
						buffer_out->bytenum = 0;
						break;//电池电压测量改用LTC2942,不需要校准
					}					
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);
				}			
				break;
			case 0x021B://写AD校准参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && 
					(buffer_in->uch_buf[6] == 0x0D) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					//校准时间
					if(ERROR == GetDataFromSeparator('\r', 4, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					//校准时间格式为"14-01-25 15:24:35"
					if ((U16Value2 != 17) || (ERROR == CreateNewFileData(ADJUST_FILE_PATH, ADJUST_TIME_FILE_NAME,&buffer_in->uch_buf[5+U16Value1], U16Value2)))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = (u16)U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = (u16)U32Value;
					
					Temp1[0] = U16Value1>>8;
					Temp1[1] = U16Value1&0xFF;
					Temp1[2] = U16Value2>>8;
					Temp1[3] = U16Value2&0xFF;
					
					if(buffer_in->uch_buf[5] == '1')
					{//总线电流AD校验参数(低压小量程)
						adc_adjust.low1current.k = U16Value1/1000.0;
						adc_adjust.low1current.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//总线电流AD校验参数(低压大量程)
						adc_adjust.low2current.k = U16Value1/1000.0;
						adc_adjust.low2current.b = U16Value2;					
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);				
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//总线电流AD校验参数(高压小量程)
						adc_adjust.high1current.k = U16Value1/1000.0;
						adc_adjust.high1current.b = U16Value2;					
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//总线电流AD校验参数(高压大量程)
						adc_adjust.high2current.k = U16Value1/1000.0;
						adc_adjust.high2current.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//总线电压AD校验参数
						adc_adjust.linab.k = U16Value1/1000.0;
						adc_adjust.linab.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//电池电压AD校验参数
//						adc_adjust.bat.k = U16Value1/1000.0;
//						adc_adjust.bat.b = U16Value2;
//						Result = CreateNewFileData(ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);
						buffer_out->bytenum = 0;	
						break;//电池电压测量改用LTC2942,不需要校准
					}					
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					if(ERROR == Result)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}			
				break;
			case 0x021C://设备充电功能强制开关,校准电池电压时使用
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && 
					(buffer_in->uch_buf[6] == 0x0D))
				{				
					if(buffer_in->uch_buf[5] == '1')
					{//进入电压电流校准状态
						Op_Data.AdjustStatus = 1;						
					}
					else
					{//设置允许设备充电
						Op_Data.AdjustStatus = 0;						
					}
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x021D://主从机起爆控制
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 7) && 
					(buffer_in->uch_buf[buffer_in->bytenum-1] == 0x0D))
				{
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						break;
					}				
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					switch(U32Value)
					{
						case 0://关闭设备
							WM_SetFocus(DlghWin.Status);
							GUI_SendKeyMsg(GUI_KEY_POWER, 0);
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;					
						case 1://读注册雷管、错误雷管、雷管类型
							if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
							{
								buffer_out->bytenum = 0;
								break;
							}
							if(U32Value == 1)
							{
								ShowRemoteBlastSlaveStateDlg(4);								
							}
							if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
							{
								if(Op_Data.SetSlaveIndex != Op_Data.CurrentSlaveIndex)
								{//如果内存中的雷管数据不是指定表号的数据则需要重新读取
									ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
								}					
							}							
							buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
								"%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, Det.Type);
							break;
						case 2://读已注册雷管内码，用于授权验证
							//读第几发雷管
							if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
							{
								buffer_out->bytenum = 0;
								break;
							}
							if(U32Value >= Det.RegCount)
							{
								buffer_out->bytenum = 0;
								break;
							}

							//雷管内码\孔号\频率\延期\状态码
							buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
								"%02X%02X%02X%02X%02X%02X%02X%02X\r%u\r%u\r%u\r%u\r%u\r", 
								Det.Info[U32Value].Serl.U8Buffer[0], Det.Info[U32Value].Serl.U8Buffer[1], 
                                Det.Info[U32Value].Serl.U8Buffer[2], Det.Info[U32Value].Serl.U8Buffer[3],
								Det.Info[U32Value].Pwd.U8Buffer[0], Det.Info[U32Value].Pwd.U8Buffer[1], 
                                Det.Info[U32Value].Pwd.U8Buffer[2], Det.Info[U32Value].Pwd.U8Buffer[3],
								Det.Info[U32Value].Hole.Name, Det.Info[U32Value].Hole.Inside,
								Det.Info[U32Value].Freq.U32Value, Det.Info[U32Value].Delay,
								Det.Info[U32Value].Err);
							break;
						case 3://开始起爆流程雷管测试
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							if(Op_Data.BlastStatus != DET_TESTING)
							{
								Op_Data.BlastStatus = DET_TESTING;
								ShowWaitLinCurrentReadyDlg(2);							
							}
							break;
						case 4://读起爆流程雷管测试状态
							if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
							{
								if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
								{//已经测试完成
									buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
										"1\r%u\r%u\r%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, 
										BlastInfo.V_BeforeTest, BlastInfo.I_BeforeTest, BlastInfo.I_Leak);
									break;
								}								
							}
							buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
								"2\r%u\r%u\r%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, 
								BlastInfo.V_BeforeTest, BlastInfo.I_BeforeTest, BlastInfo.I_Leak);
							break;							
						case 5://快速测试
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							if(Op_Data.BlastStatus != DET_TESTING)
							{
								Op_Data.BlastStatus = DET_TESTING;
								
								ShowWaitLinCurrentReadyDlg(0);								
							}							
							break;
						case 6://开始对网络进行充电
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
							BlastInfo.V_BeforeFire = 0xFFFF;
							BlastInfo.I_BeforeFire = 0xFFFF;
							OPEN_DOUBLE_KEY1_SIM_SW();
							if(Op_Data.BlastStatus != DET_CHARGING)
							{
								Op_Data.BlastStatus = DET_CHARGING;
								ShowProgbarChargeDlg();	
							}													
							break;
						case 7://读网络进行充电状态
							if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
							{//已经充电完成
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"1\r%u\r%u\r%u\r%u\r", BlastInfo.V_AfterCharge, BlastInfo.I_AfterCharge, BlastInfo.V_BeforeCharge, BlastInfo.I_BeforeCharge);							
							}
							else
							{
								buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							}							
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
							break;
						case 8://发送起爆信号
							if ((BlastInfo.V_BeforeFire == 0xFFFF) && (0 != GET_LINAB_SHORT_SW_CTRL_STATE()))
							{//当前总线电压没关闭
								//保存发送起爆信号之前的总线电压电流值
								BlastInfo.V_BeforeFire = Op_Data.LinABVoltage;
								BlastInfo.I_BeforeFire = Op_Data.LinABCurrent;
								
								OPEN_DOUBLE_KEY2_SIM_SW();
								OPEN_BUS_6_7V();//把总线切到低压下再发送起爆信号
													
								System168MDelay1ms(20);
								CreateStartBlastTask();	
								OPEN_HV_SW();//起爆完成，总线电压恢复到高压状态
								SaveBlasteFireTime();//保存起爆时间
								WriteBlasterRecord();//保存起爆记录
                                WriteSystemLog(NULL,0,BLAST_RECORDER_COPY);//把起爆记录保存到日志中
                                
								Op_Data.BusMonitorDisableShow = 1;//总线异常时不显示
                                
								WriteSystemLog(NULL,0,BLAST_SINGLE_DATA);//保存起爆信号监测数据
								WriteSystemLog("高压调压电路DA最小值:",strlen("高压调压电路DA最小值:"), NEW_STR_INFO);
								WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
								
								ShowRemoteBlastSlaveStateDlg(6);
								
								buffer_out->uch_buf[buffer_out->bytenum++] = '1';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;								
							}							
							break;
						case 9://读起爆结果
							if(BlastInfo.V_BeforeFire != 0xFFFF)
							{
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"1\r%u\r%u\r", BlastInfo.V_BeforeFire, BlastInfo.I_BeforeFire);
							}
							else if(0 == GET_LINAB_SHORT_SW_CTRL_STATE())
							{//总线异常
								buffer_out->uch_buf[buffer_out->bytenum++] = '2';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							}
							else
							{//没有收到主机的起爆命令
								buffer_out->uch_buf[buffer_out->bytenum++] = '3';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							}							
							break;
						case 10://读起爆过程中的电压电流数据
							//测试后的电压电流、充电后的电压电流、起爆前的电压电流
							if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
							{
								if(Op_Data.SetSlaveIndex >= SlaveInfo.Number)
								{
									Op_Data.SetSlaveIndex = 0;
								}	
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"%u\r%u\r%u\r%u\r%u\r%u\r%u\r", 
									SlaveInfo.Info[Op_Data.SetSlaveIndex].V_BeforeTest, SlaveInfo.Info[Op_Data.SetSlaveIndex].I_BeforeTest,
									SlaveInfo.Info[Op_Data.SetSlaveIndex].V_AfterCharge, SlaveInfo.Info[Op_Data.SetSlaveIndex].I_AfterCharge,
									SlaveInfo.Info[Op_Data.SetSlaveIndex].V_BeforeFire, SlaveInfo.Info[Op_Data.SetSlaveIndex].I_BeforeFire,
									SlaveInfo.Info[Op_Data.SetSlaveIndex].I_Leak);																
							}
							else
							{
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"%u\r%u\r%u\r%u\r%u\r%u\r%u\r", 
									BlastInfo.V_BeforeTest, BlastInfo.I_BeforeTest,
									BlastInfo.V_AfterCharge, BlastInfo.I_AfterCharge,
									BlastInfo.V_BeforeFire, BlastInfo.I_BeforeFire, BlastInfo.I_Leak);							
							}
							break;
						case 11:
							if(TaskIsExist(APP_RS485_TASK_PRIO))
							{
								Op_Data.BlastStatus = NOT_START;
								OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
								CLOSE_HV_SW();
                                OSTimeDly(20);
								AllDisCharge();
								
								ShowRemoteBlastSlaveStateDlg(3);
								
								buffer_out->uch_buf[buffer_out->bytenum++] = '1';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							}
							else
							{
								buffer_out->bytenum = 0;
							}
							break;
						default:
							buffer_out->bytenum = 0;
							break;
					}
				}					
				break;
			case 0x021E://读已授权的GPS信息
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum],
						"%s\r%s\r%04d\r%02d\r%02d\r%02d\r%02d\r%02d\r", 
						Op_Data.Gps.Longitude,Op_Data.Gps.Latitude,
						Op_Data.Gps.Year,Op_Data.Gps.Month,Op_Data.Gps.Day,
						Op_Data.Gps.Hour,Op_Data.Gps.Minute,Op_Data.Gps.Second);
				}
				break;
			case 0x021F://写已授权的GPS信息
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
					
					if(ERROR == GetDataFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 >= sizeof(Op_Data.Gps.Longitude))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}

					memcpy(Op_Data.Gps.Longitude,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(ERROR == GetDataFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 >= sizeof(Op_Data.Gps.Latitude))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}

					memcpy(Op_Data.Gps.Latitude,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					
					if(ERROR == GetDataFromSeparator('\r', 3, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != strlen("20150831141620"))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					Op_Data.Gps.Year = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1],4);
					Op_Data.Gps.Month = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+4],2);
					Op_Data.Gps.Day = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+6],2);
					Op_Data.Gps.Hour = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+8],2);
					Op_Data.Gps.Minute = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+10],2);
					Op_Data.Gps.Second = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+12],2);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
			case 0x0220://读授权的密钥ID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.Mac[0], sizeof(BlastInfo.DkeyInfo.Mac));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Mac[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0221://写授权的密钥ID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(BlastInfo.DkeyInfo.Mac))
					{
						memset(&BlastInfo.DkeyInfo.Mac[0], 0, sizeof(BlastInfo.DkeyInfo.Mac));
						if (U16Value1 > 0)
						{
							memcpy((char*)&BlastInfo.DkeyInfo.Mac[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					else
					{//要写入的ID长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0222://读授权的用户名
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.Operator[0], sizeof(BlastInfo.DkeyInfo.Operator));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Operator[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0223://写授权的密钥用户名
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(BlastInfo.DkeyInfo.Operator))
					{
						memset(&BlastInfo.DkeyInfo.Operator[0], 0, sizeof(BlastInfo.DkeyInfo.Operator));
						if (U16Value1 > 0)
						{
							memcpy((char*)&BlastInfo.DkeyInfo.Operator[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					else
					{//要写入的长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0224://读授权的授权码
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.License[0], sizeof(BlastInfo.DkeyInfo.License));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Operator[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0225://写授权的授权码
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(BlastInfo.DkeyInfo.License))
					{
						memset(&BlastInfo.DkeyInfo.License[0], 0, sizeof(BlastInfo.DkeyInfo.License));
						if (U16Value1 > 0)
						{
							memcpy((char*)&BlastInfo.DkeyInfo.License[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					else
					{//要写入长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0226://读授权的数据上传类型
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r",BlastInfo.DkeyInfo.UploadType);
				}
				break;
			case 0x0227://写授权的数据上传类型
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
					BlastInfo.DkeyInfo.UploadType = buffer_in->uch_buf[5];		
				}
				break;
			case 0x0228://手持机写入验证信息
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					//用户名
					if(ERROR == GetDataFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
						break;
					}
					if(U16Value2 >= sizeof(BlastInfo.DkeyInfo.Operator))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
						break;						
					}
					memset(&BlastInfo.DkeyInfo.Operator[0], 0, sizeof(BlastInfo.DkeyInfo.Operator));
					memcpy(&BlastInfo.DkeyInfo.Operator[0], &buffer_in->uch_buf[5+U16Value1], U16Value2);
					
					//授权码
					if(ERROR == GetDataFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
						break;
					}
					if(U16Value2 > sizeof(BlastInfo.DkeyInfo.License))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
						break;						
					}
					memset(&BlastInfo.DkeyInfo.License[0], 0, sizeof(BlastInfo.DkeyInfo.License));
					memcpy(&BlastInfo.DkeyInfo.License[0], &buffer_in->uch_buf[5+U16Value1], U16Value2);
					
					//数据上传类型
					if(ERROR == GetDecValueFromSeparator('\r', 3, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					BlastInfo.DkeyInfo.UploadType = '0' + U32Value;	
					
					//GPS,经度
					memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
					
					if(ERROR == GetDataFromSeparator('\r', 4, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 >= sizeof(Op_Data.Gps.Longitude))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
	
					memcpy(Op_Data.Gps.Longitude,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					//GPS,纬度
					if(ERROR == GetDataFromSeparator('\r', 5, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 >= sizeof(Op_Data.Gps.Latitude))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}

					memcpy(Op_Data.Gps.Latitude,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					//GPS,定位时间
					if(ERROR == GetDataFromSeparator('\r', 6, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != strlen("20150831141620"))
					{
						memset((char*)&Op_Data.Gps,0,sizeof(Op_Data.Gps));
						
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					Op_Data.Gps.Year = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1],4);
					Op_Data.Gps.Month = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+4],2);
					Op_Data.Gps.Day = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+6],2);
					Op_Data.Gps.Hour = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+8],2);
					Op_Data.Gps.Minute = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+10],2);
					Op_Data.Gps.Second = GetDecValueFromBuffer(&buffer_in->uch_buf[5+U16Value1+12],2);																
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	

					Op_Data.BlastVerifyResult = '1';//已授权
				}
				break;
			case 0x0229://读起爆流程状态
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					Op_Data.BlastVerifyResult = buffer_in->uch_buf[5];	
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.License[0], sizeof(BlastInfo.DkeyInfo.License));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Operator[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r%d\r%d\r", (u16)Op_Data.BlastStatus, SlaveInfo.RegCount, SlaveInfo.ErrorCount);										
					}
					else
					{
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r%d\r%d\r", (u16)Op_Data.BlastStatus, Det.RegCount, Det.ErrorCount);					
					}
				}
				break;
			case 0x022A://写起爆记录上传结果
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
					Op_Data.BlastInfoUpdateResult = buffer_in->uch_buf[5];		
				}
				break;
			case 0x022B://删除所有雷管数据
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(DeleteAllDetInfo() == ERROR)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
				break;
			case 0x022C://写入雷管数据
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					u16 DetIndex = 0;
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据


					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					if(U32Value >= DET_MAX_SIZE)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					DetIndex = U32Value;//起始索引
					U8Value = 0;
					while(DetIndex < DET_MAX_SIZE)
					{
						//雷管内码\孔号\频率\延期\状态码
						if(ERROR == GetDataFromSeparator('\r', 2+6*U8Value, &buffer_in->uch_buf[5], buffer_in->bytenum-5, &U16Value1, &U16Value2))
						{
							if(U8Value == 0)
							{
								buffer_out->uch_buf[buffer_out->bytenum++] = '2';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;							
							}

							break;
						}
						if(U16Value2 != 16)
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}

						HexStrToCharStr(&buffer_in->uch_buf[5+U16Value1], U16Value2, Temp1);
						if(CheckOverlap(Temp1) == ERROR)
						{
							if(DetIndex != Op_Data.overlaped)//通信过程，出现重试时有用
							{
								buffer_out->uch_buf[buffer_out->bytenum++] = '3';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
								break;							
							}							
						}						
						Det.Info[DetIndex].Serl.U8Buffer[0] = Temp1[0];
						Det.Info[DetIndex].Serl.U8Buffer[1] = Temp1[1];
						Det.Info[DetIndex].Serl.U8Buffer[2] = Temp1[2];
						Det.Info[DetIndex].Serl.U8Buffer[3] = Temp1[3];
						Det.Info[DetIndex].Pwd.U8Buffer[0] = Temp1[4];
						Det.Info[DetIndex].Pwd.U8Buffer[1] = Temp1[5];
						Det.Info[DetIndex].Pwd.U8Buffer[2] = Temp1[6];
						Det.Info[DetIndex].Pwd.U8Buffer[3] = Temp1[7];
						if(ERROR == GetDecValueFromSeparator('\r', 3+6*U8Value, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}
						Det.Info[DetIndex].Hole.Name = (u16)U32Value;
						
						if(ERROR == GetDecValueFromSeparator('\r', 4+6*U8Value, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}
						Det.Info[DetIndex].Hole.Inside = (u8)U32Value;
						
						if(ERROR == GetDecValueFromSeparator('\r', 5+6*U8Value, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}
						Det.Info[DetIndex].Freq.U32Value = U32Value;	
						
						if(ERROR == GetDecValueFromSeparator('\r', 6+6*U8Value, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}
						Det.Info[DetIndex].Delay = U32Value;
						
						if(ERROR == GetDecValueFromSeparator('\r', 7+6*U8Value, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;
						}						
						Det.Info[DetIndex].Err = U32Value;
						
						DetIndex++;
						U8Value++;
					}
					if(buffer_out->bytenum == 5)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
					}
					else
					{
						ReadAllDetInfo(0);
					}
				}
				break;
			case 0x022D://读当前注册雷管的雷管信息,一次可以读多发
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex != Op_Data.CurrentSlaveIndex)
						{//如果内存中的雷管数据不是指定表号的数据则需要重新读取
							ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
						}					
					}
					
					//起始索引
					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;
					//数量
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;
					
					if(((U16Value1+U16Value2) > Det.RegCount) || (U16Value2 > 10))//每次不能超过10发
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
					while(U16Value2--)
					{
						//雷管内码\孔号\频率\延期\状态码
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%02X%02X%02X%02X%02X%02X%02X%02X\r%u\r%u\r%u\r%u\r%u\r", 
							Det.Info[U16Value1].Serl.U8Buffer[0], Det.Info[U16Value1].Serl.U8Buffer[1],
                            Det.Info[U16Value1].Serl.U8Buffer[2], Det.Info[U16Value1].Serl.U8Buffer[3],
							Det.Info[U16Value1].Pwd.U8Buffer[0], Det.Info[U16Value1].Pwd.U8Buffer[1],
                            Det.Info[U16Value1].Pwd.U8Buffer[2], Det.Info[U16Value1].Pwd.U8Buffer[3],
							Det.Info[U16Value1].Hole.Name, Det.Info[U16Value1].Hole.Inside,
							Det.Info[U16Value1].Freq.U32Value, Det.Info[U16Value1].Delay,
							Det.Info[U16Value1].Err);
							
						U16Value1++;
					}					
				}
				break;	
			case 0x022E://返回当前的注册数、错误数、雷管类型
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex != Op_Data.CurrentSlaveIndex)
						{//如果内存中的雷管数据不是指定表号的数据则需要重新读取
							ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
						}					
					}					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, Det.Type);						
				}							
				break;
			case 0x022F://读每个孔的设计雷管数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					memcpy(&buffer_out->uch_buf[5], &Det.HoleNumber[0], DET_MAX_SIZE);
					
					buffer_out->bytenum += DET_MAX_SIZE;
				}
				else if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					u16 StartIndex, Count;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartIndex = U32Value;//起始孔索引
					
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Count = U32Value;//数量
					
					memcpy(&buffer_out->uch_buf[5], &Det.HoleNumber[StartIndex], Count);
					
					buffer_out->bytenum += Count;
				}				
				break;
			case 0x0230://写每个孔的设计雷管数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					u16 StartIndex, Count;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartIndex = U32Value;//起始孔索引
					
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Count = U32Value;//数量
					
					GetDataFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1, &U16Value2);
					
					U32Value = U16Value1+U16Value2+1;
					
					memcpy(&Det.HoleNumber[StartIndex], &buffer_in->uch_buf[5+U32Value], Count);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;				
			case 0x0231://保存雷管数据及每个孔的设计雷管数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == SaveAllDetInfo(0))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}			
				
				break;
			case 0x0232://读设备类型、使用方式、带载数量
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], "%d\r%d\r%d\r", 
						device_info.DeviceType, device_info.DeviceUsages, SlaveInfo.Number);		
				}				
				break;
			case 0x0233://设置要读取的表号(级联设备)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if((DEVICE_USAGES_MASTER != device_info.DeviceUsages) && (buffer_in->uch_buf[5] != 'A'))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}
					else
					{
						if((buffer_in->uch_buf[5] >= 'A') && ((buffer_in->uch_buf[5] - 'A') < SlaveInfo.Number))
						{
							Op_Data.SetSlaveIndex = buffer_in->uch_buf[5] - 'A';
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							
							if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
							{
								ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
							}							
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
				}				
				break;
			case 0x0234://读从机的软件版本和ID，需要提前设置要读取的表号(0x0233 级联设备)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
//					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex >= SlaveInfo.Number)
						{
							Op_Data.SetSlaveIndex = 0;
						}
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
						
						U16Value1 = GetBufferStringLenght(&SlaveInfo.Info[Op_Data.SetSlaveIndex].Version[0], sizeof(SlaveInfo.Info[0].Version));
						strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&SlaveInfo.Info[Op_Data.SetSlaveIndex].Version[0], U16Value1);
						buffer_out->bytenum += U16Value1;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;

						U16Value1 = GetBufferStringLenght(&SlaveInfo.Info[Op_Data.SetSlaveIndex].ID[0], sizeof(SlaveInfo.Info[0].ID));
						strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&SlaveInfo.Info[Op_Data.SetSlaveIndex].ID[0], U16Value1);
						buffer_out->bytenum += U16Value1;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
					}		
				}				
				break;
			case 0x0235://设置服务器IP地址及端口号
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					//服务器网络参数
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					device_info.Server.Ip = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					device_info.Server.Port = U32Value;	

					if(ERROR == GetDataFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}					
					if(U16Value2 != sizeof(device_info.Server.Date))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					memcpy(device_info.Server.Date, &buffer_in->uch_buf[5+U16Value1], U16Value2);
					
					if(ERROR == CreateNewFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&device_info.Server, sizeof(device_info.Server)))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}				
				break;
			case 0x0236://读服务器IP地址及端口号
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					memset((u8*)&Temp1[0], 0, sizeof(Temp1));
					memcpy((u8*)&Temp1[0], device_info.Server.Date, sizeof(device_info.Server.Date));
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%lu\r%lu\r%s\r", device_info.Server.Ip, device_info.Server.Port, (char*)&Temp1[0]);		
				}				
				break;	
			case 0x0237://读延期抽检配置
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据					
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r", Op_Data.DelaySampleCfg);		
				}				
				break;
			case 0x0238://写延期抽检配置
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 7))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Op_Data.DelaySampleCfg = U32Value;
					
					if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, DELAY_SAMPLE_FILE_NAME, (u8*)&Op_Data.DelaySampleCfg, sizeof(Op_Data.DelaySampleCfg)))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}				
				break;
			case 0x0239://根据孔号范围读设备已注册的雷管数量
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					HOLE StartHole, EndHole;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartHole.Name = U32Value;
					StartHole.Inside = 1;
					
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					EndHole.Name = U32Value;				
					EndHole.Inside = NET_PARAM_MAX;
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"1\r%u\r%u\r", GetCountSpecifiedHoleRange(&StartHole, &EndHole), GetHoleIndexSpecifiedStartHole(&StartHole, 1));					
				}
				break;				
			case 0x023A://删除指定孔范围的雷管
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					HOLE StartHole, EndHole;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartHole.Name = U32Value;
					StartHole.Inside = 1;
					
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					EndHole.Name = U32Value;
					EndHole.Inside = NET_PARAM_MAX;
					
					DeleteDetInfo(&StartHole, &EndHole);
					AllRegDetRankBaseOnHoleInfo();
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
			case 0x023B://读起爆流程授权状态
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastVerifyResult);		
				}
				break;
			case 0x023C://设置起爆流程授权状态
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					Op_Data.BlastVerifyResult = buffer_in->uch_buf[5];		
				}
				break;	
			case 0x023D://读起爆时间
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%04d%02d%02d%02d%02d%02d\r", BlastInfo.Time.year, BlastInfo.Time.month, BlastInfo.Time.day,
							BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
				}
				break;	
			case 0x023E://读起爆数据上传结果
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastInfoUpdateResult);	
				}
				break;
			case 0x023F://读起爆记录条数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", BlastRecorder.Number);	
				}
				break;
			case 0x0240://读起爆记录文件名，一次可以读多条
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;//起始索引
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//数量,每次最多20
					if((U16Value2 <= 0) || (U16Value2 > 20) || ((U16Value1+U16Value2) > BlastRecorder.Number))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;		
						
					U8Value = 0;
					while(U8Value != U16Value2)
					{
						memcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], &BlastRecorder.FileName[U16Value1][0], 12);
						buffer_out->bytenum += 12;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
						U16Value1++;
						U8Value++;
					}
				}
				break;
			case 0x0241://读指定文件名的起爆记录大小
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDataFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != 12)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					memset(Temp1, 0, sizeof(Temp1));
					memcpy(Temp1,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					
					U16Value1 = GetBlastRecorderIndexFromFileName(Temp1);
					if(U16Value1 >= BlastRecorder.Number)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"1\r%u\r", GetBlastRecorderFileSize(U16Value1));						
				}
				break;
			case 0x0242://读指定文件名的起爆记录数据
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					u16 FileIndex;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDataFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != 12)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					memset(Temp1, 0, sizeof(Temp1));
					memcpy(Temp1,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					
					FileIndex = GetBlastRecorderIndexFromFileName(Temp1);
					if(FileIndex >= BlastRecorder.Number)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					
					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//要读的字节数
					
					//文件内索引
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
															
					if(ERROR == GetBlastRecorderData(FileIndex, U32Value, U16Value2, &buffer_out->uch_buf[buffer_out->bytenum]))
					{
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
						buffer_out->uch_buf[buffer_out->bytenum++] = '5';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					buffer_out->bytenum += U16Value2;						
				}
				break;	
			case 0x0243://读扫码模块配置,0表示没有扫码模块，1表示有扫码模块
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", device_info.HasScan);					
				}
				break;
			case 0x0244://设置扫码模块配置
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					device_info.HasScan = U32Value;					
					if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, SCAN_FILE_NAME, (u8*)&device_info.HasScan, sizeof(device_info.HasScan)))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}			
				break;
//			case 0x0245://设置支持的工厂代码
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					u8 i;
//					
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		

//					memset((char*)&FactoryCodeSupport[0], 0, FACTORY_CODE_SUPPORT_MAX);
//					for(i=0; i<FACTORY_CODE_SUPPORT_MAX;i++)
//					{
//						if(ERROR == GetDecValueFromSeparator('\r',1+i,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//							break;
//						}
//						FactoryCodeSupport[i] = (u8)U32Value;						
//					}
//					if(i == FACTORY_CODE_SUPPORT_MAX)
//					{
//						if(ERROR == CreateNewFileData(FACTORY_CFG_FILE_PATH, FACTORY_CODE_SUPPORT_FILE_NAME, (u8*)&FactoryCodeSupport[0], FACTORY_CODE_SUPPORT_MAX))
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						}
//						else
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//						}
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
//					}
//				}				
//				break;
//			case 0x0246://读支持的工厂代码
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					u8 i;
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
//					
//					for(i=0; i<FACTORY_CODE_SUPPORT_MAX;i++)
//					{
//						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//							"%d\r", FactoryCodeSupport[i]);						
//					}
//				}
//				break;
			case 0x024B://级联模式时，如果是安管三码绑定，必需等APP验证所有雷管后才能进入充电流程
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    Op_Data.WaitDetVerify = U32Value;
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x024C://级联模式时，如果是安管三码绑定，必需等APP验证所有雷管后才能进入充电流程
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                        "%d\r", Op_Data.WaitDetVerify);	
				}
				break;
			case 0x024D://下载授权文件1
                if ((HostPack != 0xFFFF) && (buffer_in->bytenum > 9))
				{//下载数据
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    U32Value = 0;
                    for(U8Value=0; U8Value<4;U8Value++)//前4字节表示文件开始保存的索引
                    {
                        U32Value <<= 8;
                        U32Value += buffer_in->uch_buf[5+U8Value];
                    }
                    U16Value1 = buffer_in->bytenum-9;
                    
                    if(WriteAccreditInfoFileForDL(0, U32Value, U16Value1, &buffer_in->uch_buf[9]) == ERROR)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                    }
                    else
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    }
                        
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
                else if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{//下载结束
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    WriteAccreditInfoFileForDL(0, 0, 0, NULL);
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
				break;
			case 0x024E://下载授权文件2
                if ((HostPack != 0xFFFF) && (buffer_in->bytenum > 9))
				{//下载数据
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    U32Value = 0;
                    for(U8Value=0; U8Value<4;U8Value++)//前4字节表示文件开始保存的索引
                    {
                        U32Value <<= 8;
                        U32Value += buffer_in->uch_buf[5+U8Value];
                    }
                    U16Value1 = buffer_in->bytenum-9;
                    
                    if(WriteAccreditInfoFileForDL(1, U32Value, U16Value1, &buffer_in->uch_buf[9]) == ERROR)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                    }
                    else
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    }
                        
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
                else if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{//下载结束
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    WriteAccreditInfoFileForDL(1, 0, 0, NULL);
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
				break;
			case 0x024F://设置指定文件名的起爆记录的上传状态
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDataFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != 12)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					memset(Temp1, 0, sizeof(Temp1));
					memcpy(Temp1,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					for(U16Value1 = 0; U16Value1<BlastRecorder.Number;U16Value1++)
                    {
                        if(memcmp(Temp1, BlastRecorder.FileName[U16Value1], 12) == 0)
                        {
                            break;
                        }
                    }
                    if(U16Value1 >= BlastRecorder.Number)
                    {
                    	buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
                    }
                    if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    
					if(ERROR == UpdateBlasterRecordUploadState(U32Value, U16Value1))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;				
				}
				break;
			case 0x0250://读起爆流程状态,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", (u16)Op_Data.BlastStatus);
				}
				break; 
			case 0x0251://读起爆验证状态,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastVerifyResult);
				}
				break;
			case 0x0252://读雷管状态代码,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据		
					
                    if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    U16Value1 = U32Value;//起始索引
                    if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    U16Value2 = U32Value;//数量
                    if((U16Value1 + U16Value2) > Det.RegCount)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '3';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    while(U16Value2--)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = Det.Info[U16Value1++].Err;
                    }
				}
				break;
			case 0x0255://读日志条数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", LogRecorder.Number);	
				}
				break;
			case 0x0256://读日志文件名，一次可以读多条
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;//起始索引
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//数量,每次最多20
					if((U16Value2 <= 0) || (U16Value2 > 20) || ((U16Value1+U16Value2) > LogRecorder.Number))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;		
						
					U8Value = 0;
					while(U8Value != U16Value2)
					{
						memcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], &LogRecorder.FileName[U16Value1][0], 6);
						buffer_out->bytenum += 6;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
						U16Value1++;
						U8Value++;
					}
				}
				break;
			case 0x0257://读指定文件名的日志大小
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDataFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != 6)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					memset(Temp1, 0, sizeof(Temp1));
					memcpy(Temp1,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					
					U16Value1 = GetLogRecorderIndexFromFileName(Temp1);
					if(U16Value1 >= LogRecorder.Number)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"1\r%u\r", GetLogRecorderFileSize(U16Value1));						
				}
				break;
			case 0x0258://读指定文件名的日志数据
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					u16 FileIndex;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDataFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U16Value1, &U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					if(U16Value2 != 6)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					memset(Temp1, 0, sizeof(Temp1));
					memcpy(Temp1,&buffer_in->uch_buf[5+U16Value1],U16Value2);
					
					FileIndex = GetLogRecorderIndexFromFileName(Temp1);
					if(FileIndex >= LogRecorder.Number)
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '4';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;							
					}
					
					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//要读的字节数
					
					//文件内索引
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
															
					if(ERROR == GetLogRecorderData(FileIndex, U32Value, U16Value2, &buffer_out->uch_buf[buffer_out->bytenum]))
					{
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
						buffer_out->uch_buf[buffer_out->bytenum++] = '5';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					buffer_out->bytenum += U16Value2;						
				}
				break;                 
			case 0xFFFF://点名指令，用于获取设备通信地址
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
				}				
				break;
			default:
				break;
		}
		if((buffer_in->uch_buf[0] == 0x00) && (Cmd != 0xFFFF))
		{
			buffer_out->bytenum = 0;
		}
	}
}


