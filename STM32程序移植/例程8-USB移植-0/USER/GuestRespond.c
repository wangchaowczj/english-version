#include "includes.h"

STR_COMM  GusetSendBuffer;
STR_COMM  GusetTempBuffer;
STR_COMM  HidData;

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
	buffer->uch_buf[buffer->bytenum++] = DeviceInfo.name;
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
//void  GuestRespond(STR_COMM *buffer_in, STR_COMM *buffer_out)
//{
//	u16 Cmd = 0, HostPack = 0;
////	static u16 GuestPack = 0, LastValue;
//	u8 U8Value;
//	u16 U16Value1, U16Value2, U16Value3, U16Value4;
//	u32 U32Value;
//	u8 Temp1[100]/*, Temp2[100]*/;
//	
//	memset((char*)buffer_out, 0, sizeof(STR_COMM));
//	
//	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == DeviceInfo.name) || (buffer_in->uch_buf[0] == 0x00)))
//	{
//		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
//		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
//		switch(Cmd)
//		{
//			case 0x0000://������汾
//				if(HostPack == 0xFFFF)
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);
//					
//					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
//					
//					buffer_out->bytenum += strlen(VERSION_STRING);
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}
//				break;		
//			case 0x0001://��ID
//				if(HostPack == 0xFFFF)
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					U16Value1 = GetBufferStringLenght(&DeviceInfo.ID[0], sizeof(DeviceInfo.ID));
//					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.ID[0], U16Value1);
//					
//					buffer_out->bytenum += U16Value1;
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
//				}
//				break;
//			case 0x0002://дID
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
//					if(U16Value1 <= sizeof(DeviceInfo.ID))
//					{
//						memset(&DeviceInfo.ID[0], 0, sizeof(DeviceInfo.ID));
//						if (U16Value1 > 0)
//						{
//							memcpy((char*)&DeviceInfo.ID[0], (char*)&buffer_in->uch_buf[5], U16Value1);
//						}						

//						if(ERROR != WriteFactoryParam())
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
//						}
//						else
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
//						}
//					}
//					else
//					{//Ҫд���ID����̫��
//						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
//					}
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}				
//				break;
//			case 0x0003://��Ӳ���汾
//				if(HostPack == 0xFFFF)
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					U16Value1 = GetBufferStringLenght(&DeviceInfo.HW[0], sizeof(DeviceInfo.HW));
//					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.HW[0], U16Value1);
//					
//					buffer_out->bytenum += U16Value1;
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}
//				break;
//			case 0x0004://дӲ���汾
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
//					if(U16Value1 <= sizeof(DeviceInfo.HW))
//					{
//						memset(&DeviceInfo.HW[0], 0, sizeof(DeviceInfo.HW));
//						if (U16Value1 > 0)
//						{
//							memcpy((char*)&DeviceInfo.HW[0], (char*)&buffer_in->uch_buf[5], U16Value1);
//						}						

//						if(ERROR != WriteFactoryParam())
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
//						}
//						else
//						{
//							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
//						}
//					}
//					else
//					{//Ҫд���ID����̫��
//						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
//					}
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}				
//				break;
//			case 0x0005://�ر����ߵ�ѹ
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					SET_LIN_SHORT();
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
//				}				
//				break;					
//			case 0x0006://�����ߵ�ѹ(��ѹ)
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					OPEN_PW_5V();
//					SET_LIN_OPEN();
//					CLOSE_HV_SW();				
//					OPEN_PW_LV();
//					OPEN_PW_HV();				
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
//				}				
//				break;	
//			case 0x0007://�����ߵ�ѹ(��ѹ)
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					OPEN_PW_5V();
//					SET_LIN_OPEN();			
//					OPEN_PW_LV();
//					OPEN_PW_HV();					
//					OPEN_HV_SW();
//					
//					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
//				}				
//				break;	
//			case 0x0008://�����ߵ�ѹ
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}					
//					if(U32Value >= 10)
//					{//ͨ����Ϊ0-9
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;					
//					}
//					ConnectChannel(U32Value);
//					OSTimeDly(5);
//					
//					U16Value1 = GetLinABVoltage(U32Value, &U16Value2);
//					
//					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//						"%u\r%u\r", U16Value1, U16Value2);					
//				}				
//				break;					
//			case 0x0009://�����ߵ���
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					if(U32Value >= 10)
//					{//ͨ����Ϊ0-9
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;					
//					}
//					ConnectChannel(U32Value);
//					OSTimeDly(5);
//					
//					U16Value1 = GetCurrentValue(10, &U16Value2);						
//										
//					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//						"%u\r%u\r", U16Value1, U16Value2);					
//				}				
//				break;
//			case 0x000A://���ص�ѹ
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					U16Value1 = GetBatVoltage(&U16Value2);						
//										
//					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//						"%u\r%u\r", U16Value1, U16Value2);					
//				}				
//				break;				
//			case 0x000B://��У׼����
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					U8Value = (u16)U32Value;//K
//					if(U8Value == 1)
//					{
//						U16Value1 = (u16)(AdcAdjust.Current.k*1000);
//						U16Value2 = AdcAdjust.Current.b;					
//					}
//					else if(U8Value == 2)
//					{
//						U16Value1 = (u16)(AdcAdjust.LinAB1To8.k*1000);
//						U16Value2 = AdcAdjust.LinAB1To8.b;					
//					}
//					else if(U8Value == 3)
//					{
//						U16Value1 = (u16)(AdcAdjust.LinAB9.k*1000);
//						U16Value2 = AdcAdjust.LinAB9.b;					
//					}
//					else if(U8Value == 4)
//					{
//						U16Value1 = (u16)(AdcAdjust.LinAB10.k*1000);
//						U16Value2 = AdcAdjust.LinAB10.b;					
//					}					
//					else if(U8Value == 5)
//					{
//						U16Value1 = (u16)(AdcAdjust.Bat.k*1000);
//						U16Value2 = AdcAdjust.Bat.b;						
//					}
//					else
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;						
//					}
//					memset(&Temp1[0], 0, sizeof(Temp1));
//					memcpy(&Temp1[0], &AdcAdjust.Time[0], sizeof(AdcAdjust.Time));
//					
//					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//						"%u\r%u\r%s\r", U16Value1, U16Value2,Temp1);
//				}			
//				break;
//			case 0x000C://дADУ׼����
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
//					
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					U8Value = (u16)U32Value;//ͨ��				
//					
//					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					U16Value1 = (u16)U32Value;//K
//					
//					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					U16Value2 = (u16)U32Value;	//B	
//					
//					//У׼ʱ��
//					if(ERROR == GetDataFromSeparator('\r', 4, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value3,&U16Value4))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;						
//					}
//					if(U16Value4 !=  sizeof(AdcAdjust.Time))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;						
//					}
//					
//					if(U8Value == 1)
//					{
//						AdcAdjust.Current.k = U16Value1/1000.0;
//						AdcAdjust.Current.b = U16Value2;					
//					}
//					else if(U8Value == 2)
//					{
//						AdcAdjust.LinAB1To8.k = U16Value1/1000.0;
//						AdcAdjust.LinAB1To8.b = U16Value2;				
//					}
//					else if(U8Value == 3)
//					{
//						AdcAdjust.LinAB9.k = U16Value1/1000.0;
//						AdcAdjust.LinAB9.b = U16Value2;					
//					}
//					else if(U8Value == 4)
//					{
//						AdcAdjust.LinAB10.k = U16Value1/1000.0;
//						AdcAdjust.LinAB10.b = U16Value2;					
//					}					
//					else if(U8Value == 5)
//					{
//						AdcAdjust.Bat.k = U16Value1/1000.0;
//						AdcAdjust.Bat.b = U16Value2;					
//					}
//					else
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;						
//					}					
//					memcpy(&AdcAdjust.Time[0], &buffer_in->uch_buf[5+U16Value3], U16Value4);
//		
//					if(ERROR == WriteFactoryParam())
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//					}
//					else
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//					}
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}			
//				break;				
//			case 0x000D://����ADУ׼����
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������	
//					
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//					U8Value = (u16)U32Value;//ͨ��	
//					if(U8Value == 1)
//					{
//						AdcAdjust.Current.k = 0;
//						AdcAdjust.Current.b = 0;				
//					}
//					else if(U8Value == 2)
//					{
//						AdcAdjust.LinAB1To8.k = 0;
//						AdcAdjust.LinAB1To8.b = 0;				
//					}
//					else if(U8Value == 3)
//					{
//						AdcAdjust.LinAB9.k = 0;
//						AdcAdjust.LinAB9.b = 0;					
//					}
//					else if(U8Value == 4)
//					{
//						AdcAdjust.LinAB10.k = 0;
//						AdcAdjust.LinAB10.b = 0;					
//					}					
//					else if(U8Value == 5)
//					{
//						AdcAdjust.Bat.k = 0;
//						AdcAdjust.Bat.b = 0;					
//					}
//					else
//					{
//						memset((char*)&AdcAdjust, 0, sizeof(ADC_ADJUST));
//					}					
//					memset(AdcAdjust.Time, 0, sizeof(AdcAdjust.Time));
//					

//					if(ERROR == WriteFactoryParam())
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//					}
//					else
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
//					}
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//				}			
//				break;
//			case 0x000E://����֧�ֵĹ�������
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
//						if(ERROR == WriteFactoryParam())
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
//			case 0x000F://��֧�ֵĹ�������
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
//			case 0x0010://��оƬ����
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
//					
//					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
//							"%d\r", 0);
//				}
//				break;
//			case 0x0011://����оƬ����
//				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
//				{
//					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������		
//					
//					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
//					{
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;
//					}
//                    if ((U32Value != 0) && (U32Value != 1))
//                    {
//						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
//						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
//						break;                    
//                    }
////                    DeviceInfo.DetClass = U32Value;
//                    if(ERROR != WriteFactoryParam())
//                    {
//                        buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
//                    }
//                    else
//                    {
//                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
//                    }
//                    
//					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;                    
//				}
//				break;				
//			default:
//				break;
//		}
//		if(buffer_in->uch_buf[0] == 0x00)
//		{
//			buffer_out->bytenum = 0;
//		}
//	}
//}


