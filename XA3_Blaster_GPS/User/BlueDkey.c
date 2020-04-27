#include "includes.h"

STR_COMM BlueTempBuffer;

/**
  *************************************************************************************
  * @brief  ���ݴ������
  * @param[in]  data_in��  ���ǰ�����ݽṹ
  * @param[out] data_out�� ���������ݽṹ 
  * @retval �� 
  * @author ZH
  * @date   2015��12��30��
  * @note   ��
  *************************************************************************************  
  */
void BlueDkeyPack(STR_COMM* data_in, STR_COMM* data_out)
{
	if(data_in->bytenum >= 0x01)
	{
		//��У��
		data_in->uch_buf[data_in->bytenum++] = Get_Sum_Verify(&data_in->uch_buf[0], data_in->bytenum);
		
		//slip
		Slip_Pack(data_in, data_out);	
	}
	else
	{
		data_out->bytenum = 0x00;
	}
}

/**
  *************************************************************************************
  * @brief  ���ݽ������
  * @param[in]  data_in��  ���ǰ�����ݽṹ
  * @param[out] data_out�� ���������ݽṹ 
  * @retval ������
  *         @arg @b ERROR   ���ʧ��
  *         @arg @b SUCCESS ����ɹ�  
  * @author ZH
  * @date   2015��12��30��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BlueDkeyUnpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u8 Verify;
	
	//slip���
	if(ERROR == Slip_Unpack(data_in, data_out))
	{
		return ERROR;
	}
	
	Verify = Get_Sum_Verify(&data_out->uch_buf[0], data_out->bytenum-1);
	if(Verify == data_out->uch_buf[data_out->bytenum-1])
	{
		data_out->bytenum--;
		
		data_in->bytenum = 0;
		data_in->status = RECIEVING;
		return SUCCESS;			
	}
	return ERROR;
}  
 
/**
  *************************************************************************************
  * @brief  ����Կ���ַ���(ͨ��)
  * @param  Cmd  ����
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @param  TimerOut ��ʱʱ�䣬MSΪ��λ 
  * @retval ���
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyGeneralString(u8 Cmd, u8* StrPtr, u8 MaxBytes, u16 TimerOut)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = Cmd;//��������ֽ�
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
						
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+TimerOut/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if(BlueTempBuffer.bytenum >= 1)
						{
							if(Cmd == BlueTempBuffer.uch_buf[0])
							{
								if(MaxBytes >= (BlueTempBuffer.bytenum-1))//��߻���1��0x0D,����Ҫ���1���ֽ�
								{
									if(StrPtr != NULL)
									{
										memset(StrPtr, 0, MaxBytes);
										memcpy(StrPtr, &BlueTempBuffer.uch_buf[1], BlueTempBuffer.bytenum-1);								
									}
									return SUCCESS;
								}						
							}			
						}
					}			
				}
				OSTimeDly(2);
			}					
		}	
	}

	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ����Կ������汾
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��   
  * @retval ���
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeySoftwareVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetBlueDkeyGeneralString(0x00, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����ԿID
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyId(u8* StrPtr, u8 MaxBytes)
{
	return GetBlueDkeyGeneralString(0x01, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����Կ��ص�ѹ
  * @param  BatVoltage ��ص�ѹmV
  * @retval ���
  * @author ZH
  * @date   2016��04��09��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyBatVoltage(u16* BatVoltage)
{
	u32 c_time;
	u8 Retry = 3;
	int Value = 0;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0A;//������
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum <= 5) && (0x0A == BlueTempBuffer.uch_buf[0]))
						{						
							BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum] = 0;
							sscanf((char*)&BlueTempBuffer.uch_buf[1], "%d", &Value);
							*BatVoltage = Value;
							return SUCCESS;									
						}
						return ERROR;
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ����Կ��ص�ѹ
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2016��04��09��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyServerParam(u32* IP, u16* Port, u8* Date)
{
	u32 c_time;
	u8 Retry = 3;
	u32 U32Value = 0;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x08;//������
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum >= 19) && (BlueTempBuffer.bytenum <= 32) && 
							(0x08 == BlueTempBuffer.uch_buf[0]) && (0x0D == BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum - 15]))
						{						
							if(ERROR == GetDecValueFromSeparator('\r',1,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;						
							}
							*IP = U32Value;
							if(ERROR == GetDecValueFromSeparator('\r',2,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;						
							}
							*Port = (u16)U32Value;
							memcpy(Date, &BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum - 14], 14);
							return SUCCESS;									
						}
						return ERROR;
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ����������Կ�з���������
  * @param  IP IP��ַ
  * @param  Port �˿ں�
  * @param  Date ����(14�ֽ�)  
  * @retval ���
  * @author ZH
  * @date   2016��04��09��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus SetBlueDkeyServerParam(u32 IP, u16 Port, u8* Date)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x09;//������
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%u\r%u\r%s", IP, Port, Date);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum == 2) && (0x09 == BlueTempBuffer.uch_buf[0]) && ('1' == BlueTempBuffer.uch_buf[1]))
						{						
							return SUCCESS;									
						}
						return ERROR;
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ����Կ��λ��Ϣ
  * @param  gps ��ȡ����GPS��Ϣ
  * @retval ��� 0 ��λ�ɹ� ��1��λʧ�ܣ� 0xFFͨѶ�쳣
  * @author ZH
  * @date   2019��07��15��
  * @note   
  *************************************************************************************  
  */ 
u8 GetBlueDkeyGPS(GPS_INFO* gps)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0C;//������
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum > 2) && (0x0C == BlueTempBuffer.uch_buf[0]))
						{
                            u32 U32Value;
                            u16 U16Value1, U16Value2;
                            
							if(ERROR == GetDecValueFromSeparator('\r',1,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U32Value))
							{
								return 0xFF;//ͨѶ�쳣						
							}
                            if(U32Value != 1)
                            {
                                return 1;//��λʧ��
                            }
                            if(gps != NULL)
                            {
                                u8 Buffer[3];
                                
                                if(ERROR == GetDataFromSeparator('\r',2,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U16Value1, &U16Value2))
                                {
                                    return 0xFF;//ͨѶ�쳣				
                                }
                                memset(gps->Longitude, 0, sizeof(gps->Longitude));
                                memcpy(gps->Longitude, &BlueTempBuffer.uch_buf[1+U16Value1], U16Value2);
                                
                                if(ERROR == GetDataFromSeparator('\r',3,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U16Value1, &U16Value2))
                                {
                                    return 0xFF;//ͨѶ�쳣				
                                }
                                memset(gps->Latitude, 0, sizeof(gps->Latitude));
                                memcpy(gps->Latitude, &BlueTempBuffer.uch_buf[1+U16Value1], U16Value2);
                                
                                //��λʱ��"170927153641"
                                Buffer[2] = 0;
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-12];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-11];
                                gps->Year = 2000+AsciiToU32(Buffer);
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-10];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-9];
                                gps->Month = AsciiToU32(Buffer);
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-8];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-7];
                                gps->Day = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-6];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-5];
                                gps->Hour = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-4];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-3];
                                gps->Minute = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-2];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1];
                                gps->Second = AsciiToU32(Buffer);
                            }
							return 0;//��λ�ɹ�									
						}
						return 0xFF;//ͨѶ�쳣
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0xFF;//ͨѶ�쳣
}
/**
  *************************************************************************************
  * @brief  ����Կ��λ��Ϣ
  * @param  gps ��ȡ����GPS��Ϣ
  * @retval ���
  * @author ZH
  * @date   2019��07��15��
  * @note   
  *************************************************************************************  
  */ 
u8 GetBlueDkeyRtc(RTC_Typedef* rtc)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0B;//������
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum > 1) && (0x0B == BlueTempBuffer.uch_buf[0]))
						{
                            u32 U32Value;
                            
							if(ERROR == GetDecValueFromSeparator('\r',1,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U32Value))
							{
								return 0xFF;//ͨѶ�쳣						
							}
                            if(U32Value != 1)
                            {
                                return 1;//��Կ��RTC�쳣
                            }
                            if(rtc != NULL)
                            {
                                u8 Buffer[3];
                                
                                //��λʱ��"170927153641"
                                Buffer[2] = 0;
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-12];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-11];
                                rtc->year = 2000+AsciiToU32(Buffer);
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-10];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-9];
                                rtc->month = AsciiToU32(Buffer);
                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-8];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-7];
                                rtc->day = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-6];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-5];
                                rtc->hour = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-4];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-3];
                                rtc->minute = AsciiToU32(Buffer);
                                                                
                                Buffer[0] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-2];
                                Buffer[1] = BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1];
                                rtc->second = AsciiToU32(Buffer);
                            }
							return 0;//��ȡ��Կ��RTCʱ��ɹ�									
						}
						return 0xFF;//ͨѶ�쳣
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0xFF;//ͨѶ�쳣
}
/**
  *************************************************************************************
  * @brief  ����Կ��Ϣ
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyInfo(void)
{
//	u16 U16Value;
//	u32 U32Value;
//	u8 temp[15];
//	int Result;
	
	if(ERROR == GetBlueDkeySoftwareVersion(BlastInfo.DkeyInfo.Version,sizeof(BlastInfo.DkeyInfo.Version)))
	{
		return ERROR;
	}
	if(ERROR == GetBlueDkeyId(BlastInfo.DkeyInfo.Mac,sizeof(BlastInfo.DkeyInfo.Mac)))
	{
		return ERROR;
	}
	
	//����Կ�еķ�������ַ
//	memset(temp,0,sizeof(temp));
//	if(ERROR == GetBlueDkeyServerParam(&U32Value, &U16Value, &temp[0]))
//	{
//		return ERROR;
//	}
//	Result = memcmp(&device_info.Server.Date[0], temp, sizeof(device_info.Server.Date));
//	if((device_info.Server.Ip != U32Value) || (device_info.Server.Port != U16Value) || (Result != 0))
//	{//��Կ�����豸�еķ���������(��ʱ��)��һ�¡�
//		if(Result < 0)
//		{//��Կ�е�ʱ�������
//			if((U32Value != 0) && (U16Value != 0))
//			{
//				device_info.Server.Ip = U32Value;
//				device_info.Server.Port = U16Value;
//				memcpy(&device_info.Server.Date[0], temp, sizeof(device_info.Server.Date));
//				CreateNewFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&device_info.Server, sizeof(device_info.Server));			
//			}
//		}
//		else
//		{
//			if((device_info.Server.Ip != 0) && (device_info.Server.Port != 0))
//			{
//				memset(temp,0,sizeof(temp));
//				memcpy(temp, &device_info.Server.Date[0], sizeof(device_info.Server.Date));
//				if(ERROR == SetBlueDkeyServerParam(device_info.Server.Ip, device_info.Server.Port, temp))
//				{
//					return ERROR;
//				}
//			}		
//		}
//	}
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  ������Կ�е�RTCʱ�䣬У׼�豸ʱ��
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��12��30��
  * @note   ���޴�GPS����Կ
  *************************************************************************************  
  */
ErrorStatus SetRtcByBlueDkey(void)
{
    //������Կ��RTCʱ��
    if(0 == GetBlueDkeyRtc(&RTC_Type_Temp))
    {
        if(RTC_Type_Temp.year >= 2019)
        {
            if(RTC_Write_Time() == 0)
            {
                return SUCCESS;
            }
        }
    }
    
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  ������Կ�е�RTCʱ�䣬У׼�豸ʱ��
  * @param  ��
  * @retval 0xFF ͨѶ�쳣��0��Ч��1��λʧ�ܣ�2λ����Ϣ�Ѿ�ʧЧ
  * @author ZH
  * @date   2015��12��30��
  * @note   ���޴�GPS����Կ
  *************************************************************************************  
  */
u8 BlueDkeyGpsIsPass(GPS_INFO* gps)
{
    RTC_Typedef rtc;
    u8 Result;
    
    Result = GetBlueDkeyGPS(gps);
    if(Result != 0)
    {
        return Result;
    }
    
    if(0 != GetBlueDkeyRtc(&rtc))
    {
        return 0;
    }
    
    //24Сʱ����Ч
    if(GPSInfoTimeIsPass(gps, &rtc))
    {
        return 0;
    }
    return 2;
}
/**
  *************************************************************************************
  * @brief  �û���������֤
  * @param  UserName �û���
  * @param  Password ����
  * @param  State �û�״̬(ʣ�����Դ���)���������ʱ��Ч:
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ�� 
  *       @arg @b ��2���Ѿ�����
  *       @arg @b ��3���û�������
  *       @arg @b ��4���������    
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */ 
u8 BlueDkeyUserInfoVerify(u8* UserName, u8* Password, u8* State)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x07;//������
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%s\r%s", UserName, Password);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum == 4) && (0x07 == BlueTempBuffer.uch_buf[0]) && (0x0D == BlueTempBuffer.uch_buf[2]))
						{
						
							*State = (u8)BlueTempBuffer.uch_buf[3];

							return BlueTempBuffer.uch_buf[1];									
						}
						return 0x01;
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  �û���������֤
  * @param  UserName �û���
  * @param  Password ����
  * @param  State �û�״̬(ʣ�����Դ���)���������ʱ��Ч:
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ�� 
  *       @arg @b ��2���Ѿ�����
  *       @arg @b ��3���û�������
  *       @arg @b ��4���������    
  * @author ZH
  * @date   2015��12��30��
  * @note   
  *************************************************************************************  
  */
u8 BlueDkeyModifyUserInfo(u8* OldUserName, u8* OldPassword, u8* NewUserName, u8* NewPassword, u8* State)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.status = DECODING;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x06;//������
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%s\r%s\r%s\r%s", OldUserName, OldPassword, NewUserName, NewPassword);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
						
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(BlueDkeyUnpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if ((BlueTempBuffer.bytenum == 4) && (0x06 == BlueTempBuffer.uch_buf[0]) && (0x0D == BlueTempBuffer.uch_buf[2]))
						{						
							*State = (u8)BlueTempBuffer.uch_buf[3];

							return BlueTempBuffer.uch_buf[1];									
						}
						return 0x01;
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0x00;		
}

