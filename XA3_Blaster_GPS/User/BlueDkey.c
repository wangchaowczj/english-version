#include "includes.h"

STR_COMM BlueTempBuffer;

/**
  *************************************************************************************
  * @brief  数据打包函数
  * @param[in]  data_in：  打包前的数据结构
  * @param[out] data_out： 打包后的数据结构 
  * @retval 无 
  * @author ZH
  * @date   2015年12月30日
  * @note   无
  *************************************************************************************  
  */
void BlueDkeyPack(STR_COMM* data_in, STR_COMM* data_out)
{
	if(data_in->bytenum >= 0x01)
	{
		//和校验
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
  * @brief  数据解包函数
  * @param[in]  data_in：  解包前的数据结构
  * @param[out] data_out： 解包后的数据结构 
  * @retval 解包结果
  *         @arg @b ERROR   解包失败
  *         @arg @b SUCCESS 解包成功  
  * @author ZH
  * @date   2015年12月30日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BlueDkeyUnpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u8 Verify;
	
	//slip解包
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
  * @brief  读密钥的字符串(通用)
  * @param  Cmd  命令
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @param  TimerOut 超时时间，MS为单位 
  * @retval 结果
  * @author ZH
  * @date   2015年12月30日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = Cmd;//命令码低字节
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
						
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
								if(MaxBytes >= (BlueTempBuffer.bytenum-1))//后边还有1个0x0D,所以要多减1个字节
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
  * @brief  读密钥的软件版本
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度   
  * @retval 结果
  * @author ZH
  * @date   2015年12月30日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeySoftwareVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetBlueDkeyGeneralString(0x00, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥ID
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年12月30日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetBlueDkeyId(u8* StrPtr, u8 MaxBytes)
{
	return GetBlueDkeyGeneralString(0x01, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥电池电压
  * @param  BatVoltage 电池电压mV
  * @retval 结果
  * @author ZH
  * @date   2016年04月09日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0A;//命令码
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
  * @brief  读密钥电池电压
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2016年04月09日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x08;//命令码
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
  * @brief  设置蓝牙密钥中服务器参数
  * @param  IP IP地址
  * @param  Port 端口号
  * @param  Date 日期(14字节)  
  * @retval 结果
  * @author ZH
  * @date   2016年04月09日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x09;//命令码
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%u\r%u\r%s", IP, Port, Date);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
  * @brief  读密钥定位信息
  * @param  gps 获取到的GPS信息
  * @retval 结果 0 定位成功 ，1定位失败， 0xFF通讯异常
  * @author ZH
  * @date   2019年07月15日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0C;//命令码
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
								return 0xFF;//通讯异常						
							}
                            if(U32Value != 1)
                            {
                                return 1;//定位失败
                            }
                            if(gps != NULL)
                            {
                                u8 Buffer[3];
                                
                                if(ERROR == GetDataFromSeparator('\r',2,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U16Value1, &U16Value2))
                                {
                                    return 0xFF;//通讯异常				
                                }
                                memset(gps->Longitude, 0, sizeof(gps->Longitude));
                                memcpy(gps->Longitude, &BlueTempBuffer.uch_buf[1+U16Value1], U16Value2);
                                
                                if(ERROR == GetDataFromSeparator('\r',3,&BlueTempBuffer.uch_buf[1],BlueTempBuffer.bytenum-5,&U16Value1, &U16Value2))
                                {
                                    return 0xFF;//通讯异常				
                                }
                                memset(gps->Latitude, 0, sizeof(gps->Latitude));
                                memcpy(gps->Latitude, &BlueTempBuffer.uch_buf[1+U16Value1], U16Value2);
                                
                                //定位时间"170927153641"
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
							return 0;//定位成功									
						}
						return 0xFF;//通讯异常
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0xFF;//通讯异常
}
/**
  *************************************************************************************
  * @brief  读密钥定位信息
  * @param  gps 获取到的GPS信息
  * @retval 结果
  * @author ZH
  * @date   2019年07月15日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x0B;//命令码
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
					
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
								return 0xFF;//通讯异常						
							}
                            if(U32Value != 1)
                            {
                                return 1;//密钥的RTC异常
                            }
                            if(rtc != NULL)
                            {
                                u8 Buffer[3];
                                
                                //定位时间"170927153641"
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
							return 0;//获取密钥的RTC时间成功									
						}
						return 0xFF;//通讯异常
					}
					break;
				}
				OSTimeDly(2);
			}					
		}
	}
	return 0xFF;//通讯异常
}
/**
  *************************************************************************************
  * @brief  读密钥信息
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年12月30日
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
	
	//读密钥中的服务器地址
//	memset(temp,0,sizeof(temp));
//	if(ERROR == GetBlueDkeyServerParam(&U32Value, &U16Value, &temp[0]))
//	{
//		return ERROR;
//	}
//	Result = memcmp(&device_info.Server.Date[0], temp, sizeof(device_info.Server.Date));
//	if((device_info.Server.Ip != U32Value) || (device_info.Server.Port != U16Value) || (Result != 0))
//	{//密钥和起爆设备中的服务器参数(含时间)不一致。
//		if(Result < 0)
//		{//密钥中的时间更加新
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
  * @brief  根据密钥中的RTC时间，校准设备时间
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年12月30日
  * @note   仅限带GPS的密钥
  *************************************************************************************  
  */
ErrorStatus SetRtcByBlueDkey(void)
{
    //读出密钥的RTC时间
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
  * @brief  根据密钥中的RTC时间，校准设备时间
  * @param  无
  * @retval 0xFF 通讯异常，0有效，1定位失败，2位置信息已经失效
  * @author ZH
  * @date   2015年12月30日
  * @note   仅限带GPS的密钥
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
    
    //24小时内有效
    if(GPSInfoTimeIsPass(gps, &rtc))
    {
        return 0;
    }
    return 2;
}
/**
  *************************************************************************************
  * @brief  用户名密码验证
  * @param  UserName 用户名
  * @param  Password 密码
  * @param  State 用户状态(剩余重试次数)，密码错误时有效:
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过 
  *       @arg @b ‘2’已经自锁
  *       @arg @b ‘3’用户名错误
  *       @arg @b ‘4’密码错误    
  * @author ZH
  * @date   2015年12月30日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x07;//命令码
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%s\r%s", UserName, Password);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
  * @brief  用户名密码验证
  * @param  UserName 用户名
  * @param  Password 密码
  * @param  State 用户状态(剩余重试次数)，密码错误时有效:
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过 
  *       @arg @b ‘2’已经自锁
  *       @arg @b ‘3’用户名错误
  *       @arg @b ‘4’密码错误    
  * @author ZH
  * @date   2015年12月30日
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
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x06;//命令码
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%s\r%s\r%s\r%s", OldUserName, OldPassword, NewUserName, NewPassword);
			
			BlueData.status = DECODING;
			BlueDkeyPack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;			
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
						
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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

