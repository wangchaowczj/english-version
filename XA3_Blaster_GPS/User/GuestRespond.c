#include "includes.h"
#include "GuiIncludes.h"

STR_COMM  GusetSendBuffer;
STR_COMM  GusetTempBuffer;

/**
  *************************************************************************************
  * @brief  ��ȡ�������Ӧ����
  * @param  buffer�����ݻ�����
  * @param  Cmd������
  * @param  PackIndex���ְ���  
  * @retval �� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  ��ȡ�������е��ַ�������
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval �ַ������� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  �ӻ������е���ֵ
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval ��ֵ
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  �ӷָ�����ȡ����
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  StartIndex���õ���������Buffer�е���ʼ����ֵ
  * @param  DataBytes���õ������ݳ���  
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"ABCD"��Separator SeparatorNumber �ֱ�Ϊ'\r',2
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
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  �ӷָ�����ȡ10������ֵ
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  Value����ֵ
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"123"��Separator SeparatorNumber �ֱ�Ϊ'\r',1
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
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  USB������Ӧ
  * @param  buffer_in�����յ�������
  * @param  buffer_out����Ӧ������
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
			case 0x0200://������汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0301://��SN�룬Ҳ����STM32оƬΩһID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
                    GetSTM32ChipIDStr(&buffer_out->uch_buf[buffer_out->bytenum]);
					buffer_out->bytenum += 24;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;                
			case 0x0201://��ID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&device_info.mac[0], sizeof(device_info.mac));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&device_info.mac[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0202://дID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					{//Ҫд���ID����̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0203://��Ӳ���汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&device_info.hw_version[0], sizeof(device_info.hw_version));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&device_info.hw_version[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0204://дӲ���汾
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					{//Ҫд���ID����̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';						
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}				
				break;
			case 0x0210://���豸����
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r%lu\r%lu\r%lu\r%lu\r",
						device_info.DeviceType ,
						device_info.single_current_min, device_info.single_current_max,
						device_info.det_freq_min, device_info.det_freq_max);
				}				
				break;
			case 0x0211://�����豸����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������					
					
					//�豸����
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					U32Value &= 0xFF;
					
					//����豸���ͱ��ı�,�û����������豸,����ʱ�����𱬼�¼��Ϊ0�������ϴ���¼ʱ����
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
					
					//����������С�����ֵ
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

					//оƬƵ����С�����ֵ
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
			case 0x0214://���豸RTCʱ��
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%04d\r%02d\r%02d\r%02d\r%02d\r%02d\r",
						RTC_Type.year ,RTC_Type.month,RTC_Type.day,RTC_Type.hour,RTC_Type.minute,RTC_Type.second);
				}					
				break;
			case 0x0215://д�豸RTCʱ��
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0216://�豸��ѹ����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if (buffer_in->uch_buf[5] == '1')
					{//�ر����ߵ�ѹ
						CLOSE_BUS_VOLTAGE();						
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//�����ѹ״̬
						OPEN_BUS_6_7V();
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//�򿪸�ѹʹ�ܿ���
						OPEN_HV_SW_INSIDE();
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//�رո�ѹʹ�ܿ���
						CLOSE_HV_SW_INSIDE();
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//�򿪸�ѹ�������
						OPEN_HV_SW();
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//�رո�ѹ�������
						CLOSE_HV_SW();
					}
					else if(buffer_in->uch_buf[5] == '7')
					{//�رո�ѹ������ؼ���ѹʹ�ܿ���
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
			case 0x0217://�����ߵ�ѹ����״̬
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(buffer_in->uch_buf[5] == '1')
					{//�����ߵ�ѹ����״̬
						if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
						{//��ǰ���ߵ�ѹ�Ѿ��ر�
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						}
						else if(GET_HV_SW_CTRL_STATE() == 0)
						{//��ѹ��û���,Ϊ��ѹ״̬
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '3';
						}					
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//�ڲ���ѹ����ʹ��״̬
						if(GET_HV_ENABLE_SW_STATE() != 0)
						{//�Ѿ���ʹ��
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						}
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//�Ƿ��Ѿ����°�ȫ��
						if(GetChannelxVoltage(ADC_CH_11_3V, 10)>=2000)
						{//�Ѿ����°�ȫ��
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
			case 0x0218://���豸��ѹ�����ֵ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(buffer_in->uch_buf[5] == '1')
					{//�����ߵ�������ֵ(��������ֵ��AD����ֵ)
						U16Value1 = GetBusCurrent(10, &U16Value2);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//�����ߵ�ѹ����ֵ(��������ֵ��AD����ֵ)
						U16Value1 = GetBusVoltage(10, &U16Value2);
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//����ص�ѹ����ֵ(��������ֵ��AD����ֵ)						
						//U16Value1 = GetBatteryVoltage(&U16Value2);
						//��ص�ѹͨ��LTC2942�����õ�
						U16Value1 = Op_Data.BatVoltage;
						U16Value2 = Op_Data.BatVoltage*0.2442002442;//1000/4095
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//������©�����ֵ(��������ֵ��AD����ֵ)
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
			case 0x0219://ɾ��ADУ׼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(buffer_in->uch_buf[5] == '1')
					{//ɾ�����ߵ���ADУ�����(��ѹС����)
						adc_adjust.low1current.k = 0;
						adc_adjust.low1current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//ɾ�����ߵ���ADУ�����(��ѹ������)
						adc_adjust.low2current.k = 0;
						adc_adjust.low2current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//ɾ�����ߵ���ADУ�����(��ѹС����)
						adc_adjust.high1current.k = 0;
						adc_adjust.high1current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//ɾ�����ߵ���ADУ�����(��ѹ������)
						adc_adjust.high2current.k = 0;
						adc_adjust.high2current.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//ɾ�����ߵ�ѹADУ�����
						adc_adjust.linab.k = 0;
						adc_adjust.linab.b = 0;
						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME);
						f_unlink((const char*)Temp1);						
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//ɾ����ص�ѹADУ�����
//						adc_adjust.bat.k = 0;
//						adc_adjust.bat.b = 0;
//						sprintf((char*)Temp1,"%s/%s",ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME);
//						f_unlink((const char*)Temp1);
						buffer_out->bytenum = 0;
						break;//��ص�ѹ��������LTC2942,����ҪУ׼
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
			case 0x021A://��ADУ׼����
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(buffer_in->uch_buf[5] == '1')
					{//�����ߵ���ADУ�����(��ѹС����)
						U16Value1 = (u16)(adc_adjust.low1current.k*1000);
						U16Value2 = adc_adjust.low1current.b;
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//�����ߵ���ADУ�����(��ѹ������)
						U16Value1 = (u16)(adc_adjust.low2current.k*1000);
						U16Value2 = adc_adjust.low2current.b;					
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//�����ߵ���ADУ�����(��ѹС����)
						U16Value1 = (u16)(adc_adjust.high1current.k*1000);
						U16Value2 = adc_adjust.high1current.b;						
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//�����ߵ���ADУ�����(��ѹ������)
						U16Value1 = (u16)(adc_adjust.high2current.k*1000);
						U16Value2 = adc_adjust.high2current.b;						
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//�����ߵ�ѹADУ�����
						U16Value1 = (u16)(adc_adjust.linab.k*1000);
						U16Value2 = adc_adjust.linab.b;							
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//����ص�ѹADУ�����
//						U16Value1 = (u16)(adc_adjust.bat.k*1000);
//						U16Value2 = adc_adjust.bat.b;
						buffer_out->bytenum = 0;
						break;//��ص�ѹ��������LTC2942,����ҪУ׼
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
			case 0x021B://дADУ׼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && 
					(buffer_in->uch_buf[6] == 0x0D) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					//У׼ʱ��
					if(ERROR == GetDataFromSeparator('\r', 4, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1,&U16Value2))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					//У׼ʱ���ʽΪ"14-01-25 15:24:35"
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
					{//���ߵ���ADУ�����(��ѹС����)
						adc_adjust.low1current.k = U16Value1/1000.0;
						adc_adjust.low1current.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_LOW1_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);
					}
					else if(buffer_in->uch_buf[5] == '2')
					{//���ߵ���ADУ�����(��ѹ������)
						adc_adjust.low2current.k = U16Value1/1000.0;
						adc_adjust.low2current.b = U16Value2;					
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_LOW2_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);				
					}
					else if(buffer_in->uch_buf[5] == '3')
					{//���ߵ���ADУ�����(��ѹС����)
						adc_adjust.high1current.k = U16Value1/1000.0;
						adc_adjust.high1current.b = U16Value2;					
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_HIGH1_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}
					else if(buffer_in->uch_buf[5] == '4')
					{//���ߵ���ADУ�����(��ѹ������)
						adc_adjust.high2current.k = U16Value1/1000.0;
						adc_adjust.high2current.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, CURRENT_HIGH2_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}					
					else if(buffer_in->uch_buf[5] == '5')
					{//���ߵ�ѹADУ�����
						adc_adjust.linab.k = U16Value1/1000.0;
						adc_adjust.linab.b = U16Value2;						
						Result = CreateNewFileData(ADJUST_FILE_PATH, LINAB_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);					
					}
					else if(buffer_in->uch_buf[5] == '6')
					{//��ص�ѹADУ�����
//						adc_adjust.bat.k = U16Value1/1000.0;
//						adc_adjust.bat.b = U16Value2;
//						Result = CreateNewFileData(ADJUST_FILE_PATH, BAT_ADJUST_FILE_NAME, (u8*)&Temp1[0], 4);
						buffer_out->bytenum = 0;	
						break;//��ص�ѹ��������LTC2942,����ҪУ׼
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
			case 0x021C://�豸��繦��ǿ�ƿ���,У׼��ص�ѹʱʹ��
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && 
					(buffer_in->uch_buf[6] == 0x0D))
				{				
					if(buffer_in->uch_buf[5] == '1')
					{//�����ѹ����У׼״̬
						Op_Data.AdjustStatus = 1;						
					}
					else
					{//���������豸���
						Op_Data.AdjustStatus = 0;						
					}
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x021D://���ӻ��𱬿���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 7) && 
					(buffer_in->uch_buf[buffer_in->bytenum-1] == 0x0D))
				{
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						break;
					}				
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					switch(U32Value)
					{
						case 0://�ر��豸
							WM_SetFocus(DlghWin.Status);
							GUI_SendKeyMsg(GUI_KEY_POWER, 0);
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							break;					
						case 1://��ע���׹ܡ������׹ܡ��׹�����
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
								{//����ڴ��е��׹����ݲ���ָ����ŵ���������Ҫ���¶�ȡ
									ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
								}					
							}							
							buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
								"%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, Det.Type);
							break;
						case 2://����ע���׹����룬������Ȩ��֤
							//���ڼ����׹�
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

							//�׹�����\�׺�\Ƶ��\����\״̬��
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
						case 3://��ʼ�������׹ܲ���
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							if(Op_Data.BlastStatus != DET_TESTING)
							{
								Op_Data.BlastStatus = DET_TESTING;
								ShowWaitLinCurrentReadyDlg(2);							
							}
							break;
						case 4://���������׹ܲ���״̬
							if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
							{
								if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
								{//�Ѿ��������
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
						case 5://���ٲ���
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							if(Op_Data.BlastStatus != DET_TESTING)
							{
								Op_Data.BlastStatus = DET_TESTING;
								
								ShowWaitLinCurrentReadyDlg(0);								
							}							
							break;
						case 6://��ʼ��������г��
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
						case 7://��������г��״̬
							if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
							{//�Ѿ�������
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"1\r%u\r%u\r%u\r%u\r", BlastInfo.V_AfterCharge, BlastInfo.I_AfterCharge, BlastInfo.V_BeforeCharge, BlastInfo.I_BeforeCharge);							
							}
							else
							{
								buffer_out->uch_buf[buffer_out->bytenum++] = '2';
							}							
							buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;						
							break;
						case 8://�������ź�
							if ((BlastInfo.V_BeforeFire == 0xFFFF) && (0 != GET_LINAB_SHORT_SW_CTRL_STATE()))
							{//��ǰ���ߵ�ѹû�ر�
								//���淢�����ź�֮ǰ�����ߵ�ѹ����ֵ
								BlastInfo.V_BeforeFire = Op_Data.LinABVoltage;
								BlastInfo.I_BeforeFire = Op_Data.LinABCurrent;
								
								OPEN_DOUBLE_KEY2_SIM_SW();
								OPEN_BUS_6_7V();//�������е���ѹ���ٷ������ź�
													
								System168MDelay1ms(20);
								CreateStartBlastTask();	
								OPEN_HV_SW();//����ɣ����ߵ�ѹ�ָ�����ѹ״̬
								SaveBlasteFireTime();//������ʱ��
								WriteBlasterRecord();//�����𱬼�¼
                                WriteSystemLog(NULL,0,BLAST_RECORDER_COPY);//���𱬼�¼���浽��־��
                                
								Op_Data.BusMonitorDisableShow = 1;//�����쳣ʱ����ʾ
                                
								WriteSystemLog(NULL,0,BLAST_SINGLE_DATA);//�������źż������
								WriteSystemLog("��ѹ��ѹ��·DA��Сֵ:",strlen("��ѹ��ѹ��·DA��Сֵ:"), NEW_STR_INFO);
								WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
								
								ShowRemoteBlastSlaveStateDlg(6);
								
								buffer_out->uch_buf[buffer_out->bytenum++] = '1';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;								
							}							
							break;
						case 9://���𱬽��
							if(BlastInfo.V_BeforeFire != 0xFFFF)
							{
								buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
									"1\r%u\r%u\r", BlastInfo.V_BeforeFire, BlastInfo.I_BeforeFire);
							}
							else if(0 == GET_LINAB_SHORT_SW_CTRL_STATE())
							{//�����쳣
								buffer_out->uch_buf[buffer_out->bytenum++] = '2';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							}
							else
							{//û���յ�������������
								buffer_out->uch_buf[buffer_out->bytenum++] = '3';
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
							}							
							break;
						case 10://���𱬹����еĵ�ѹ��������
							//���Ժ�ĵ�ѹ����������ĵ�ѹ��������ǰ�ĵ�ѹ����
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
			case 0x021E://������Ȩ��GPS��Ϣ
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum],
						"%s\r%s\r%04d\r%02d\r%02d\r%02d\r%02d\r%02d\r", 
						Op_Data.Gps.Longitude,Op_Data.Gps.Latitude,
						Op_Data.Gps.Year,Op_Data.Gps.Month,Op_Data.Gps.Day,
						Op_Data.Gps.Hour,Op_Data.Gps.Minute,Op_Data.Gps.Second);
				}
				break;
			case 0x021F://д����Ȩ��GPS��Ϣ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
			case 0x0220://����Ȩ����ԿID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.Mac[0], sizeof(BlastInfo.DkeyInfo.Mac));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Mac[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0221://д��Ȩ����ԿID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					{//Ҫд���ID����̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0222://����Ȩ���û���
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.Operator[0], sizeof(BlastInfo.DkeyInfo.Operator));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Operator[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0223://д��Ȩ����Կ�û���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					{//Ҫд��ĳ���̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0224://����Ȩ����Ȩ��
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&BlastInfo.DkeyInfo.License[0], sizeof(BlastInfo.DkeyInfo.License));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&BlastInfo.DkeyInfo.Operator[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0225://д��Ȩ����Ȩ��
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
					{//Ҫд�볤��̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0226://����Ȩ�������ϴ�����
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r",BlastInfo.DkeyInfo.UploadType);
				}
				break;
			case 0x0227://д��Ȩ�������ϴ�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
					BlastInfo.DkeyInfo.UploadType = buffer_in->uch_buf[5];		
				}
				break;
			case 0x0228://�ֳֻ�д����֤��Ϣ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					//�û���
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
					
					//��Ȩ��
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
					
					//�����ϴ�����
					if(ERROR == GetDecValueFromSeparator('\r', 3, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					BlastInfo.DkeyInfo.UploadType = '0' + U32Value;	
					
					//GPS,����
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
					//GPS,γ��
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
					//GPS,��λʱ��
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

					Op_Data.BlastVerifyResult = '1';//����Ȩ
				}
				break;
			case 0x0229://��������״̬
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
			case 0x022A://д�𱬼�¼�ϴ����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
					
					Op_Data.BlastInfoUpdateResult = buffer_in->uch_buf[5];		
				}
				break;
			case 0x022B://ɾ�������׹�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x022C://д���׹�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					u16 DetIndex = 0;
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������


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
					DetIndex = U32Value;//��ʼ����
					U8Value = 0;
					while(DetIndex < DET_MAX_SIZE)
					{
						//�׹�����\�׺�\Ƶ��\����\״̬��
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
							if(DetIndex != Op_Data.overlaped)//ͨ�Ź��̣���������ʱ����
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
			case 0x022D://����ǰע���׹ܵ��׹���Ϣ,һ�ο��Զ��෢
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex != Op_Data.CurrentSlaveIndex)
						{//����ڴ��е��׹����ݲ���ָ����ŵ���������Ҫ���¶�ȡ
							ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
						}					
					}
					
					//��ʼ����
					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;
					//����
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;
					
					if(((U16Value1+U16Value2) > Det.RegCount) || (U16Value2 > 10))//ÿ�β��ܳ���10��
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;					
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
					while(U16Value2--)
					{
						//�׹�����\�׺�\Ƶ��\����\״̬��
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
			case 0x022E://���ص�ǰ��ע���������������׹�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex != Op_Data.CurrentSlaveIndex)
						{//����ڴ��е��׹����ݲ���ָ����ŵ���������Ҫ���¶�ȡ
							ReadSlaveDetInfo(Op_Data.SetSlaveIndex);
						}					
					}					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%u\r", Det.RegCount, Det.ErrorCount, Det.Type);						
				}							
				break;
			case 0x022F://��ÿ���׵�����׹���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					memcpy(&buffer_out->uch_buf[5], &Det.HoleNumber[0], DET_MAX_SIZE);
					
					buffer_out->bytenum += DET_MAX_SIZE;
				}
				else if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					u16 StartIndex, Count;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartIndex = U32Value;//��ʼ������
					
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Count = U32Value;//����
					
					memcpy(&buffer_out->uch_buf[5], &Det.HoleNumber[StartIndex], Count);
					
					buffer_out->bytenum += Count;
				}				
				break;
			case 0x0230://дÿ���׵�����׹���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					u16 StartIndex, Count;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������

					if(ERROR == GetDecValueFromSeparator('\r', 1, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					StartIndex = U32Value;//��ʼ������
					
					if(ERROR == GetDecValueFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Count = U32Value;//����
					
					GetDataFromSeparator('\r', 2, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value1, &U16Value2);
					
					U32Value = U16Value1+U16Value2+1;
					
					memcpy(&Det.HoleNumber[StartIndex], &buffer_in->uch_buf[5+U32Value], Count);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;				
			case 0x0231://�����׹����ݼ�ÿ���׵�����׹���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0232://���豸���͡�ʹ�÷�ʽ����������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], "%d\r%d\r%d\r", 
						device_info.DeviceType, device_info.DeviceUsages, SlaveInfo.Number);		
				}				
				break;
			case 0x0233://����Ҫ��ȡ�ı��(�����豸)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0234://���ӻ�������汾��ID����Ҫ��ǰ����Ҫ��ȡ�ı��(0x0233 �����豸)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
//					if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						if(Op_Data.SetSlaveIndex >= SlaveInfo.Number)
						{
							Op_Data.SetSlaveIndex = 0;
						}
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
						
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
			case 0x0235://���÷�����IP��ַ���˿ں�
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					//�������������
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
			case 0x0236://��������IP��ַ���˿ں�
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					memset((u8*)&Temp1[0], 0, sizeof(Temp1));
					memcpy((u8*)&Temp1[0], device_info.Server.Date, sizeof(device_info.Server.Date));
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%lu\r%lu\r%s\r", device_info.Server.Ip, device_info.Server.Port, (char*)&Temp1[0]);		
				}				
				break;	
			case 0x0237://�����ڳ������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������					
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r", Op_Data.DelaySampleCfg);		
				}				
				break;
			case 0x0238://д���ڳ������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 7))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������

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
			case 0x0239://���ݿ׺ŷ�Χ���豸��ע����׹�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					HOLE StartHole, EndHole;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������

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
			case 0x023A://ɾ��ָ���׷�Χ���׹�
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7))
				{
					HOLE StartHole, EndHole;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������

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
			case 0x023B://����������Ȩ״̬
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastVerifyResult);		
				}
				break;
			case 0x023C://������������Ȩ״̬
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					Op_Data.BlastVerifyResult = buffer_in->uch_buf[5];		
				}
				break;	
			case 0x023D://����ʱ��
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%04d%02d%02d%02d%02d%02d\r", BlastInfo.Time.year, BlastInfo.Time.month, BlastInfo.Time.day,
							BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
				}
				break;	
			case 0x023E://���������ϴ����
				if (HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastInfoUpdateResult);	
				}
				break;
			case 0x023F://���𱬼�¼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", BlastRecorder.Number);	
				}
				break;
			case 0x0240://���𱬼�¼�ļ�����һ�ο��Զ�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;//��ʼ����
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//����,ÿ�����20
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
			case 0x0241://��ָ���ļ������𱬼�¼��С
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0242://��ָ���ļ������𱬼�¼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					u16 FileIndex;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
					U16Value2 = U32Value;//Ҫ�����ֽ���
					
					//�ļ�������
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
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
						buffer_out->uch_buf[buffer_out->bytenum++] = '5';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					buffer_out->bytenum += U16Value2;						
				}
				break;	
			case 0x0243://��ɨ��ģ������,0��ʾû��ɨ��ģ�飬1��ʾ��ɨ��ģ��
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", device_info.HasScan);					
				}
				break;
			case 0x0244://����ɨ��ģ������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
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
//			case 0x0245://����֧�ֵĹ�������
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					u8 i;
//					
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		

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
//			case 0x0246://��֧�ֵĹ�������
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					u8 i;
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
//					
//					for(i=0; i<FACTORY_CODE_SUPPORT_MAX;i++)
//					{
//						buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//							"%d\r", FactoryCodeSupport[i]);						
//					}
//				}
//				break;
			case 0x024B://����ģʽʱ������ǰ�������󶨣������APP��֤�����׹ܺ���ܽ���������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
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
			case 0x024C://����ģʽʱ������ǰ�������󶨣������APP��֤�����׹ܺ���ܽ���������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                        "%d\r", Op_Data.WaitDetVerify);	
				}
				break;
			case 0x024D://������Ȩ�ļ�1
                if ((HostPack != 0xFFFF) && (buffer_in->bytenum > 9))
				{//��������
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    U32Value = 0;
                    for(U8Value=0; U8Value<4;U8Value++)//ǰ4�ֽڱ�ʾ�ļ���ʼ���������
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
				{//���ؽ���
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    WriteAccreditInfoFileForDL(0, 0, 0, NULL);
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
				break;
			case 0x024E://������Ȩ�ļ�2
                if ((HostPack != 0xFFFF) && (buffer_in->bytenum > 9))
				{//��������
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    U32Value = 0;
                    for(U8Value=0; U8Value<4;U8Value++)//ǰ4�ֽڱ�ʾ�ļ���ʼ���������
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
				{//���ؽ���
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    WriteAccreditInfoFileForDL(1, 0, 0, NULL);
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
				}
				break;
			case 0x024F://����ָ���ļ������𱬼�¼���ϴ�״̬
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0250://��������״̬,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", (u16)Op_Data.BlastStatus);
				}
				break; 
			case 0x0251://������֤״̬,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
							"%d\r", Op_Data.BlastVerifyResult);
				}
				break;
			case 0x0252://���׹�״̬����,2019.07.08
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
					
                    if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    U16Value1 = U32Value;//��ʼ����
                    if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    U16Value2 = U32Value;//����
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
			case 0x0255://����־����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r", LogRecorder.Number);	
				}
				break;
			case 0x0256://����־�ļ�����һ�ο��Զ�����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = U32Value;//��ʼ����
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5,&U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = U32Value;//����,ÿ�����20
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
			case 0x0257://��ָ���ļ�������־��С
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
			case 0x0258://��ָ���ļ�������־����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					u16 FileIndex;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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
					U16Value2 = U32Value;//Ҫ�����ֽ���
					
					//�ļ�������
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
						GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
						buffer_out->uch_buf[buffer_out->bytenum++] = '5';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;						
					}
					buffer_out->bytenum += U16Value2;						
				}
				break;                 
			case 0xFFFF://����ָ����ڻ�ȡ�豸ͨ�ŵ�ַ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
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


