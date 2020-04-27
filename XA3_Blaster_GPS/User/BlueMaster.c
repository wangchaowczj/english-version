#include "includes.h"

/**
  *************************************************************************************
  * @brief  通过蓝牙读从设备的软件版本
  * @param  add：要读的设备地址
  * @param  Version：缓冲区
  * @param  MaxLength：缓冲区长度  
  * @retval 结果
  * @author ZH
  * @date   2016年7月013日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveVersion(u8 add, u8* Version, u8 MaxLength)
{
	u32 c_time;
	u16 StartIndex = 0;
	u16 Bytes = 0;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x00;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x00) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDataFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&StartIndex, &Bytes))
								{
									return ERROR;
								}
								if(Bytes > MaxLength)
								{
									return ERROR;
								}
								memset(Version, 0, MaxLength);
								memcpy(Version, &BlueTempBuffer.uch_buf[5+StartIndex], Bytes);
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙读从设备的雷管数量及类型
  * @param  add：要读的设备地址
  * @param  RegNumber：雷管数量
  * @param  ErrNumber：错误数量
  * @param  Type：雷管类型
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetNumberAndType(u8 add, u16* RegNumber, u16* ErrNumber, u8* Type)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2E;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2E) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*RegNumber = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 2, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*ErrNumber = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 3, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*Type = (u8)U32Value;							
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙读从设备的雷管数据
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetData(u8 add, u16 StartIndex, u8 Number)
{
	u32 c_time;
	u16 Index, Bytes; 
	u32 U32Value;
	u8 Retry = 3;
	u16 DetIndex;
	u8 Buffer[20];
	u8 U8Value;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2D;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%d\r%d\r", StartIndex, Number);
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);

			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2D) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
								{
									return ERROR;
								}
								if(U32Value != 1)
								{
									return ERROR;
								}
								
								
								U8Value = 0;
								while(U8Value < Number)
								{
									//雷管内码\孔号\频率\延期\状态码
									if(ERROR == GetDataFromSeparator('\r', 2+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &Index, &Bytes))
									{
										return ERROR;
									}
									if(Bytes != 16)
									{
										return ERROR;
									}
									HexStrToCharStr(&BlueTempBuffer.uch_buf[5+Index], Bytes, Buffer);
									
									DetIndex = GetEmptyDetIndex();
									
									Det.Info[DetIndex].Serl.U8Buffer[0] = Buffer[0];
									Det.Info[DetIndex].Serl.U8Buffer[1] = Buffer[1];
									Det.Info[DetIndex].Serl.U8Buffer[2] = Buffer[2];
									Det.Info[DetIndex].Serl.U8Buffer[3] = Buffer[3];
									Det.Info[DetIndex].Pwd.U8Buffer[0] = Buffer[4];
									Det.Info[DetIndex].Pwd.U8Buffer[1] = Buffer[5];
									Det.Info[DetIndex].Pwd.U8Buffer[2] = Buffer[6];
									Det.Info[DetIndex].Pwd.U8Buffer[3] = Buffer[7];
									if(ERROR == GetDecValueFromSeparator('\r', 3+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Hole.Name = (u16)U32Value;
									if(ERROR == GetDecValueFromSeparator('\r', 4+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Hole.Inside = (u8)U32Value;
									
									if(ERROR == GetDecValueFromSeparator('\r', 5+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Freq.U32Value = U32Value;	
									
									if(ERROR == GetDecValueFromSeparator('\r', 6+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Delay = U32Value;	
									
									if(ERROR == GetDecValueFromSeparator('\r', 7+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Err = (u8)U32Value;	

									U8Value++;
								}
								
						
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙读从设备的雷管数据
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
u8 WriteBlueSlaveDetData(u8 add, u16 LocalIndex, u16 SlaveIndex, u8 Number)
{
	u32 c_time;
	u8 Retry = 3;
	u8 U8Value;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2C;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%d\r", SlaveIndex);
			U8Value = 0;
			while(U8Value < Number)
			{
				//雷管内码\孔号\频率\延期\状态码
				BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%02X%02X%02X%02X%02X%02X%02X%02X\r%u\r%u\r%u\r%u\r%u\r", 
					Det.Info[LocalIndex + U8Value].Serl.U8Buffer[0], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[1], 
                    Det.Info[LocalIndex + U8Value].Serl.U8Buffer[2], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[3],
					Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[0], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[1], 
                    Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[2], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[3],
					Det.Info[LocalIndex + U8Value].Hole.Name, Det.Info[LocalIndex + U8Value].Hole.Inside,
					Det.Info[LocalIndex + U8Value].Freq.U32Value, Det.Info[LocalIndex + U8Value].Delay,
					Det.Info[LocalIndex + U8Value].Err);
					
				U8Value++;
			}		
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x2C) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								if(BlueTempBuffer.uch_buf[5] == '1')
								{
									return 0;	
								}
								if(BlueTempBuffer.uch_buf[5] == '3')
								{//有重号雷管
									return 1;	
								}					
							}			
						}
						return 2;
					}
					break;
				}
				OSTimeDly(2);
			}			
		}
	}
	return 3;	
}
/**
  *************************************************************************************
  * @brief  通过蓝牙读从设备的每个孔设计雷管数
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveHoleNumberData(u8 add, u16 StartHoleIndex, u16 Count)
{
	u32 c_time;
	u8 Retry = 3;

	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2F;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%u\r%u\r", StartHoleIndex,Count);
				
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == (5+Count))
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2F) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF))
							{
								memcpy(&Det.HoleNumber[StartHoleIndex], &BlueTempBuffer.uch_buf[5], Count);
								
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙读从设备的每个孔设计雷管数
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus WriteBlueSlaveHoleNumberData(u8 add, u16 StartHoleIndex, u16 Count)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x30;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		

			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHoleIndex, Count);	
					
			memcpy(&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], &Det.HoleNumber[StartHoleIndex], Count);
			BlueTempBuffer.bytenum += Count;
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x30) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙读从设备的每个孔设计雷管数
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus WriteBlueSlaveHoleNumberDataSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x3B;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHole,EndHole);			
			
			memcpy(&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], &Det.HoleNumber[StartHole-1], EndHole-StartHole+1);
			BlueTempBuffer.bytenum += EndHole-StartHole+1;
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x3B) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙删除从设备的雷管信息
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteBlueSlaveDetData(u8 add)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2B;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x2B) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙删除从设备的雷管信息
  * @param  add：要读的设备地址
  * @param  StartHole：起始孔
  * @param  EndHole：截止孔
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteBlueSlaveDetDataSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x3A;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%u\r%u\r",  StartHole, EndHole);	
				
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x3A) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙删除从设备的雷管信息
  * @param  add：要读的设备地址
  * @param  StartHole：起始雷管索引
  * @param  Number：要读的数量
  * @param  Data：返回数据
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus SaveBlueSlaveDetAndHoleData(u8 add)
{
	u32 c_time;
	u8 Retry = 3;

	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x31;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x31) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙从其他设备恢复雷管信息
  * @param  add：要读的设备地址
  * @retval 结果 0通信失败，1成功，2保存数据失败，3类型错误
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */

u8 BlueReadDetInfoBackUp(u8 add)
{
	ErrorStatus Result;
	u16 RegNumber, ErrNumber;
	u16 StartIndex = 0,i;
	u8 U8Value;

	Result = GetBlueSlaveDetNumberAndType(add, &RegNumber, &ErrNumber, &U8Value);
	if(Result == ERROR)
	{
		return 0;
	}

	memset((char*)&Det.Info[0], 0, sizeof(Det.Info));
	memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
	Det.ErrorCount = 0;
	Det.RegCount = 0;
	
	U8Value =  RegNumber/5;
	while(U8Value--)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, 5);//每次读5发雷管
		if(Result == ERROR)
		{
			return 0;
		}
		StartIndex += 5;
	}
	if(RegNumber%5)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, RegNumber%5);//每次读5发雷管
		if(Result == ERROR)
		{
			return 0;
		}	
	}
	for(i=0;i< DET_MAX_SIZE;i++)
	{
		if(ERROR != GetDetRegStatus(i))
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				memset((char*)&Det.Info[0],0,sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				return 3;//类型不符合要求
			}			
		}
		if(Det.Info[i].Delay > Det.DlyMax)
		{
			Det.Info[i].Delay =  Det.DlyMax;
		}
	}

	//读孔内雷管数
	Result = GetBlueSlaveHoleNumberData(add, 0, DET_MAX_SIZE);
	if(Result == ERROR)
	{
		return 0;
	}
	if(ERROR == SaveAllDetInfo(0))
	{
		return 2;
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  通过蓝牙把雷管信息备份到其他设备
  * @param  add：要读的设备地址
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus BlueDetInfoBackUp(u8 add)
{
	ErrorStatus Result;
	u16 StartIndex = 0;
	u8 U8Value, U8Result;	
	
	//先删除原有雷管
	Result = DeleteBlueSlaveDetData(add);
	if(Result == ERROR)
	{
		return ERROR;
	}
	//导出雷管内码及延期数据
	U8Value =  Det.RegCount/5;
	while(U8Value--)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, StartIndex, 5);//每次读5发雷管
		if(U8Result != 0)
		{
			return ERROR;
		}
		StartIndex += 5;
	}
	if(Det.RegCount%5)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, StartIndex, Det.RegCount%5);//每次读5发雷管
		if(U8Result != 0)
		{
			return ERROR;
		}	
	}
	//导出每个孔的设计雷管数
	Result = WriteBlueSlaveHoleNumberData(add, 0, DET_MAX_SIZE);
	if(Result == ERROR)
	{
		return ERROR;
	}
	//控制从机保存数据
	Result = SaveBlueSlaveDetAndHoleData(add);
	if(Result == ERROR)
	{
		return ERROR;
	}	
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  通过蓝牙读从设备的雷管数量及类型
  * @param  add：要读的设备地址
  * @param  StartHole：起始孔号
  * @param  EndHole：截止孔号  
  * @param  RegNumber：雷管数量
  * @retval 结果
  * @author ZH
  * @date   2016年11月28日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetNumberSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole, u16* RegNumber, u16* StartIndex)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//命令码高字节
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x39;//命令码低字节
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
			
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHole,EndHole);	
					
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//数据打包
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+1000/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum > 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x39) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								if(U32Value != 1)
								{
									return ERROR;
								}
								if(ERROR == GetDecValueFromSeparator('\r', 2, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}			
								*RegNumber = U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 3, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}								
								*StartIndex = U32Value;	
								
								return SUCCESS;						
							}			
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
  * @brief  通过蓝牙从其他设备插入雷管信息
  * @param  add：要读的设备地址
  * @retval 结果 0通信失败，1成功，2保存数据失败，3类型错误, 4其他设备没有数据,5有重码
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
u8 BlueReadDetInfoInsert(u8 add, u16 StartHole, u16 EndHole)
{
	ErrorStatus Result;
	u16 RegNumber;
	u16 StartIndex = 0,i;
	u8 U8Value;
	
	Result = GetBlueSlaveDetNumberSpecifiedHoleRange(add, StartHole, EndHole, &RegNumber, &StartIndex);
	if(Result == ERROR)
	{
		return 0;
	}
	if(RegNumber == 0)
	{
		return 4;
	}
	//删除指定孔范围的雷管	
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if ((Det.Info[i].Hole.Name >= StartHole) && (Det.Info[i].Hole.Name <= EndHole))
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;			
			}		
		}
	}
	GetDetNum();
	AllRegDetRankBaseOnHoleInfo();
	
	U8Value =  RegNumber/5;
	while(U8Value--)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, 5);//每次读5发雷管
		if(Result == ERROR)
		{
			return 0;
		}
		StartIndex += 5;
	}
	if(RegNumber%5)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, RegNumber%5);//每次读5发雷管
		if(Result == ERROR)
		{
			return 0;
		}	
	}
	for(i=0;i< DET_MAX_SIZE;i++)
	{
		if(ERROR != GetDetRegStatus(i))
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				memset((char*)&Det.Info[0],0,sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				return 3;//类型不符合要求
			}			
		}
		if(Det.Info[i].Delay > Det.DlyMax)
		{
			Det.Info[i].Delay =  Det.DlyMax;
		}
	}
	if(DetInfoHasSame())
	{
		memset((char*)&Det.Info[0],0,sizeof(Det.Info));
		Det.ErrorCount = 0;
		Det.RegCount = 0;
		return 5;//有相同内码	
	}
	//读孔内雷管数
	Result = GetBlueSlaveHoleNumberData(add, StartHole-1, EndHole-StartHole+1);
	if(Result == ERROR)
	{
		return 0;
	}	
	if(ERROR == SaveAllDetInfo(0))
	{
		return 2;
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  通过蓝牙把雷管信息备份到其他设备
  * @param  add：要读的设备地址
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
u8 BlueDetInfoInsert(u8 add, u16 StartHole, u16 EndHole)
{
	ErrorStatus Result;
	u16 StartIndex = 0, Count, SlaveIndex;
	u16 ErrNumber;
	u8 U8Value, U8Result;	
	HOLE Start;
	HOLE End;
	
	Start.Name = StartHole;
	Start.Inside = 1;
	End.Name = EndHole;
	End.Inside = NET_PARAM_MAX;	
	
	Count = GetCountSpecifiedHoleRange(&Start, &End);
	if(Count == 0)
	{
		return 0;
	}
	//先删除原有雷管
	Result = DeleteBlueSlaveDetDataSpecifiedHoleRange(add, StartHole, EndHole);
	if(Result == ERROR)
	{
		return 1;
	}
	
	Result = GetBlueSlaveDetNumberAndType(add, &SlaveIndex, &ErrNumber, &U8Value);
	if(Result == ERROR)
	{
		return 1;
	}
	
	StartIndex = GetHoleIndexSpecifiedStartHole(&Start, 1);
	
	//导出雷管内码及延期数据
	U8Value =  Count/5;
	while(U8Value--)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, SlaveIndex, 5);//每次读5发雷管
		if(U8Result != 0)
		{
			if(U8Result == 1)
			{
				return 3; //有重号雷管
			}
			return 1;
		}
		StartIndex += 5;
		SlaveIndex += 5;
	}
	if(Count%5)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, SlaveIndex, Count%5);//每次读5发雷管
		if(U8Result != 0)
		{
			if(U8Result == 1)
			{
				return 3; //有重号雷管
			}
			return 1;
		}	
	}
	//导出每个孔的设计雷管数
	Result = WriteBlueSlaveHoleNumberData(add, StartHole-1, EndHole-StartHole+1);
	if(Result == ERROR)
	{
		return 1;
	}
	//控制从机保存数据
	Result = SaveBlueSlaveDetAndHoleData(add);
	if(Result == ERROR)
	{
		return 1;
	}	
	return 2;
}
