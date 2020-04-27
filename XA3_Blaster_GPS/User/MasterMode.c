#include "includes.h"

static STR_COMM  RS485TempBuffer = {0,RECIEVING,0};

SLAVE_INFO SlaveInfo;

/**
  *************************************************************************************
  * @brief  把从机索引值转换为地址
  * @param  SlaveIndex：从机索引  
  * @retval 地址
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
u8 GetAddrNameFromIndex(u8 SlaveIndex)
{
	if(SlaveIndex != 0xFF)
	{
		return (SlaveIndex<26)?'A'+SlaveIndex:'a'+(SlaveIndex-26);
	}
	else
	{
		return 0x00;
	}	
}

/**
  *************************************************************************************
  * @brief  把从机地址转换为索引值
  * @param  AddrName：地址 
  * @retval 从机索引 
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
u8 GetIndexFromAddrName(u8 AddrName)
{
	if ((AddrName >= 'A') && (AddrName <= 'Z'))
	{
		return (AddrName-'A');
	}
	else if ((AddrName >= 'a') && (AddrName <= 'z'))
	{
		return (26+AddrName-'a');
	}
	else
	{
		return 0x00;
	}	
}
/**
  *************************************************************************************
  * @brief  获取从机地址
  * @param  Index：设备索引  
  * @param  Result： 
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveAddr(u8* AddIndex)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//协议地址
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包			
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 5)
					{
						if ((RS485TempBuffer.uch_buf[1] == 0xFF)  && 
							(RS485TempBuffer.uch_buf[2] == 0xFF) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							*AddIndex = GetIndexFromAddrName(RS485TempBuffer.uch_buf[0]);
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  设置从机RTC
  * @param  Index：要读的设备索引  
  * @param  Result： 
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveRtc(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		if(Index == 0xFF)
		{//广播指令
			RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//协议地址
		}
		else
		{
			RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		}
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x15;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RTC_Read_Time();
		
		OSSchedLock();
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r%d\r%d\r%d\r%d\r%d\r", 
			RTC_Type.year,RTC_Type.month,RTC_Type.day,RTC_Type.hour,RTC_Type.minute,RTC_Type.second);
		
		OSSchedUnlock();
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		if(GetAddrNameFromIndex(Index) == 0x00)
		{//广播指令
			SET_RS485_SEND();//485设置为发送状态
			return SLAVE_TRUE;
		}
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x15) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{							
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  通过485读从设备的总线电压控制状态
  * @param  Index：要读的设备索引
  * @param  State：要读的设备索引
  *  @arg @b '1' 当前总线电压已经关闭
  *  @arg @b '2' 高压还没输出,为低压状态
  *  @arg @b '3' 高压状态  
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBusVoltageCtrlState(u8 Index, u8* State)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x17;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '1';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x17) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							*State = RS485TempBuffer.uch_buf[5];
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}

/**
  *************************************************************************************
  * @brief  通过485读从设备的总线电压控制状态
  * @param  Index：要读的设备索引
  * @param  State：要读的设备索引
  *  @arg @b 1 读总线电流测量值
  *  @arg @b 2 读总线电压测量值
  *  @arg @b 3 读电池电压测量值
  *  @arg @b 4 读总线漏电测量值
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveVoltageOrCurrent(u8 Index, u8 Arg, u16* Value)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x18;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", Arg);
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+3500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 9)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x18) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							//返回返回的是AD测量值，只判断有没有，不使用
							if(ERROR == GetDecValueFromSeparator('\r', 2, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							*Value = (u16)U32Value;
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  通过485读从设备的软件版本
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveVerison(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x00) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							if ((RS485TempBuffer.bytenum-6) <= sizeof(SlaveInfo.Info[0].Version))
							{
								memset(SlaveInfo.Info[Index].Version, 0x00, sizeof(SlaveInfo.Info[0].Version));
								memcpy(SlaveInfo.Info[Index].Version, &RS485TempBuffer.uch_buf[5], RS485TempBuffer.bytenum-6);
								return SLAVE_TRUE;
							}
							else
							{
								return SLAVE_FALSE;
							}						
						}			
					}
					return SLAVE_FALSE;					
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  通过485读从设备的ID
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveID(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x01;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x01) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							if ((RS485TempBuffer.bytenum-6) <= sizeof(SlaveInfo.Info[0].ID))
							{
								memset(SlaveInfo.Info[Index].ID, 0x00, sizeof(SlaveInfo.Info[0].ID));
								memcpy(SlaveInfo.Info[Index].ID, &RS485TempBuffer.uch_buf[5], RS485TempBuffer.bytenum-6);
								return SLAVE_TRUE;
							}
							else
							{
								return SLAVE_FALSE;
							}						
						}			
					}
					return SLAVE_FALSE;
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  控制从设备开始某项操作
  * @param  Index：要读的设备索引
  * @param  Operate：操作
  *    @arg @b 0://关闭设备  
  *    @arg @b 3://开始起爆流程雷管测试 
  *    @arg @b 6://开始对网络进行充电   
  * @param  Result：要读的设备索引  
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveStartOperate(u8 Index, u8 Operate, u8* Result)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		if(Index == 0xFF)
		{//广播指令
			RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//协议地址
		}
		else
		{
			RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		}
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", Operate);
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		if(GetAddrNameFromIndex(Index) == 0x00)
		{//广播指令
			SET_RS485_SEND();//485设置为发送状态
			return SLAVE_TRUE;
		}
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							if(Result != NULL)
							{
								*Result = RS485TempBuffer.uch_buf[5];
							}
							
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  通过485读从设备的雷管测试结果
  * @param  Index：要读的设备索引
  * @param  Data：数据缓冲区，按顺序为：注册雷管、错误雷管、测试后电压、测试后电流、总线漏电流
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastDetTestResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '4';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if((RS485TempBuffer.uch_buf[5] == '1') || (RS485TempBuffer.uch_buf[5] == '2'))
							{
								//按顺序为：注册雷管、错误雷管、测试后电压、测试后电流、总线漏电流
								for(i= 2;i<7;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}								
								if (RS485TempBuffer.uch_buf[5] == '1')
								{
									return SLAVE_TRUE;
								}
								else
								{
									return SLAVE_VOLATILE;	//没有测试完成
								}
							}																	
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  通过485读从设备的网络充电结果
  * @param  Index：要读的设备索引
  * @param  Data：数据缓冲区，按顺序为：充电后电压、充电后电流、充电前电压、充电前电流
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastChargeResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '7';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(RS485TempBuffer.uch_buf[5] == '1')
							{
								//按顺序为：充电后电压、充电后电流、充电前电压、充电前电流
								for(i= 2;i<6;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}								
								
								return SLAVE_TRUE;	
							}
							return SLAVE_VOLATILE;	//没有完成											
						}			
					}
					return SLAVE_FALSE;
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  通过485读从设备的网络充电结果
  * @param  Index：要读的设备索引
  * @param  Data：数据缓冲区，按顺序为：起爆结果、充电后电压、充电后电流
  * @retval 结果
  * @author ZH
  * @date   2015年4月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '9';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							*Data++ = (u16)RS485TempBuffer.uch_buf[5];
							if(RS485TempBuffer.uch_buf[5] == '1')
							{
								//按顺序为：起爆前电压、起爆前电流
								for(i= 2;i<4;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}									
							}
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  读注册雷管、错误雷管、雷管类型、电池电压
  * @param  Index：要读的设备索引
  * @param  SecondArg：控制参数，0表示不更新从机界面，1表示更新
  * @retval 结果
  * @author ZH
  * @date   2015年1月06日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveTestInfo(u8 SlaveIndex, u8 SecondArg)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "1\r%d\r", SecondArg);
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum > 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].RegCount = (u16)U32Value;
							
							if(ERROR == GetDecValueFromSeparator('\r', 2, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].ErrorCount = (u16)U32Value;
							
							if(ERROR == GetDecValueFromSeparator('\r', 3, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].Type = (u8)U32Value;
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}

/**
  *************************************************************************************
  * @brief  读注册雷管、错误雷管、雷管类型、电池电压
  * @param  Index：要读的设备索引
  * @param  SecondArg：控制参数，0表示不更新从机界面，1表示更新
  * @retval 结果
  * @author ZH
  * @date   2015年1月06日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetIndexInfo(u8 SlaveIndex, u16 StartIndex, u8 Number)
{
	u32 c_time;
	u16 Index, Bytes; 
	u32 U32Value;
	u8 Retry = 5;
	u16 DetIndex;
	u8 Buffer[20];
	u8 U8Value;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2D;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r%d\r", StartIndex, Number);
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+3500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 9)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
							{
								return SLAVE_FALSE;
							}
							if(U32Value != 1)
							{
								return SLAVE_FALSE;
							}
							
							DetIndex = StartIndex;
							U8Value = 0;
							while(U8Value < Number)
							{
								//雷管内码\孔号\频率\延期\状态码
								if(ERROR == GetDataFromSeparator('\r', 2+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &Index, &Bytes))
								{
									return SLAVE_FALSE;
								}
								if(Bytes != 16)
								{
									return SLAVE_FALSE;
								}
								HexStrToCharStr(&RS485TempBuffer.uch_buf[5+Index], Bytes, Buffer);
								Det.Info[DetIndex].Serl.U8Buffer[0] = Buffer[0];
								Det.Info[DetIndex].Serl.U8Buffer[1] = Buffer[1];
								Det.Info[DetIndex].Serl.U8Buffer[2] = Buffer[2];
								Det.Info[DetIndex].Serl.U8Buffer[3] = Buffer[3];
								Det.Info[DetIndex].Pwd.U8Buffer[0] = Buffer[4];
								Det.Info[DetIndex].Pwd.U8Buffer[1] = Buffer[5];
								Det.Info[DetIndex].Pwd.U8Buffer[2] = Buffer[6];
								Det.Info[DetIndex].Pwd.U8Buffer[3] = Buffer[7];
								if(ERROR == GetDecValueFromSeparator('\r', 3+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Hole.Name = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 4+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Hole.Inside = (u8)U32Value;
								
								if(ERROR == GetDecValueFromSeparator('\r', 5+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Freq.U32Value = U32Value;	
								
								if(ERROR == GetDecValueFromSeparator('\r', 6+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Delay = U32Value;	
								
								if(ERROR == GetDecValueFromSeparator('\r', 7+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Err = (u8)U32Value;	

								DetIndex++;
								U8Value++;
							}
							
					
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;	
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  读注册雷管、错误雷管、雷管类型、电池电压
  * @param  Index：要读的设备索引
  * @param  SecondArg：控制参数，0表示不更新从机界面，1表示更新
  * @retval 结果
  * @author ZH
  * @date   2015年1月06日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetHoleNumber(u8 SlaveIndex)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2F;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包				
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == (5+DET_MAX_SIZE))
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2F) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							memcpy(&Det.HoleNumber[0], &RS485TempBuffer.uch_buf[5], DET_MAX_SIZE);
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  获取从机的雷管信息并保存到文件中
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetInfo(u8 SlaveIndex)
{
	u8 U8Value;
	u16 StartIndex = 0;
	
	memset((u8*)&Det.Info[0], 0, sizeof(Det.Info[0])*DET_MAX_SIZE);
	
	U8Value =  SlaveInfo.Info[SlaveIndex].RegCount/10;
	while(U8Value--)
	{
		if(GetSlaveDetIndexInfo(SlaveIndex, StartIndex, 10) == SLAVE_FALSE)//每次读10发雷管
		{
			return SLAVE_FALSE;
		}
		StartIndex += 10;
	}
	if(SlaveInfo.Info[SlaveIndex].RegCount%10)
	{
		if(GetSlaveDetIndexInfo(SlaveIndex, StartIndex, SlaveInfo.Info[SlaveIndex].RegCount%10) == SLAVE_FALSE)
		{
			return SLAVE_FALSE;
		}	
	}
	
	return GetSlaveDetHoleNumber(SlaveIndex);
}
void SlaveInitState(void)
{
	u8 i;
	
	OSSchedLock();
	for(i=0; i<SlaveInfo.Number; i++)
	{
		SlaveInfo.Info[i].State = SLAVE_VOLATILE;
		SlaveInfo.Info[i].WarnBit = 0;
		SlaveInfo.Info[i].ErrorBit = 0;
		SlaveInfo.Info[i].Flags = 0;
	}
	OSSchedUnlock();
}

/**
  *************************************************************************************
  * @brief  获取在线的从机数
  * @param  无 
  * @retval 从机数
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveStateCount(SLAVE_STATE State)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == State)
		{
			Count++;
		}
	}
	return Count;
}
/**
  *************************************************************************************
  * @brief  获取有异常信息的从机数
  * @param  无 
  * @retval 从机数
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveErrorCount(void)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{
			Count++;
		}
		else if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if(SlaveInfo.Info[i].ErrorBit != 0x00)
			{
				Count++;
			}
		}
	}
	return Count;	
}
u8 GetSlaveOnlineErrorIndex(void)
{
	u8 i;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{
			return i;
		}
	}
	return 0xFF;
}
u8 GetSlaveIndexFromErrorBit(u8 ErrorBit)
{
	u8 i;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if((SlaveInfo.Info[i].ErrorBit & SLAVE_ERROR_BIT_DET_ACCREDIT) == SLAVE_ERROR_BIT_DET_ACCREDIT)
			{
				return i;
			}
		}
	}
	return 0xFF;	
}
/**
  *************************************************************************************
  * @brief  获取有警示信息的从机数
  * @param  无 
  * @retval 从机数
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveWarnCount(void)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if(SlaveInfo.Info[i].WarnBit != 0x00)
			{
				Count++;
			}
		}
	}
	return Count;	
}

/**
  *************************************************************************************
  * @brief  统计雷管数量及类型
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void GetSlaveDetNumber(void)
{
	u8 i;
	
	OSSchedLock();
	SlaveInfo.RegCount = 0;
	SlaveInfo.ErrorCount = 0;
	SlaveInfo.Type = 0xFF;
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SLAVE_TRUE ==  SlaveInfo.Info[i].State)
		{
			SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
			SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;		
			if(SlaveInfo.Type == 0xFF)
			{//以第1个设备的雷管类型作为统一使用的雷管类型
				SlaveInfo.Type = SlaveInfo.Info[i].Type;
			}
//			if(SlaveInfo.Info[i].Type != SlaveInfo.Type)
//			{
//				SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_DET_TYPE;
//			}
//			else
//			{
//				SlaveInfo.Info[i].ErrorBit &= ~SLAVE_ERROR_BIT_DET_TYPE;
//			}
		}	
	}
	if(SlaveInfo.Type == 0xFF)
	{
		SlaveInfo.Type = 0;
	}
	OSSchedUnlock();
}

/**
  *************************************************************************************
  * @brief  初始化从机雷管数量
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void InitSlaveDetNumber(void)
{
	u8 i;
	
	OSSchedLock();
	SlaveInfo.RegCount = 0;
	SlaveInfo.ErrorCount = 0;
    SlaveInfo.OnlineCount = 0;
	SlaveInfo.Type = 0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		SlaveInfo.Info[i].RegCount = 0x0000;
		SlaveInfo.Info[i].ErrorCount = 0x0000;
	}
	OSSchedUnlock();
}
/**
  *************************************************************************************
  * @brief  读注册雷管、错误雷管、雷管类型、电池电压
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void GetAllSlaveInfo(u8 Arg)
{
	u16 i;
	u8 U8Value;
	SLAVE_STATE StateBuffer;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{		
		//让从机收到命令后刷屏
		StateBuffer = GetSlaveTestInfo(i, 0);		
		if(SLAVE_FALSE != StateBuffer)
		{
			//读ID
			StateBuffer = GetSlaveID(i);
		}
		if(SLAVE_FALSE != StateBuffer)
		{
			//读软件版本
			StateBuffer = GetSlaveVerison(i);
		}
		
		if(SLAVE_FALSE != StateBuffer)
		{
			//读电池电压
			StateBuffer = GetSlaveVoltageOrCurrent(i, 3, &SlaveInfo.Info[i].Battery);
		}
		if(SLAVE_FALSE != StateBuffer)
		{
			//读总线电压控制状态
			StateBuffer = GetSlaveBusVoltageCtrlState(i, &U8Value);
			if (SLAVE_FALSE != StateBuffer)
			{//总线还处于输出低压状态
				if(U8Value != '2')
				{
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
				}
				else
				{
					//读总线漏电流
					StateBuffer = GetSlaveVoltageOrCurrent(i, 4, &SlaveInfo.Info[i].I_Leak);				
				}
			}				
		}		
		OSSchedLock();
		
		if(SLAVE_FALSE != StateBuffer)
		{
			if(SlaveInfo.Info[i].Battery < 3720)
			{
				SlaveInfo.Info[i].WarnBit |= SLAVE_ERROR_BIT_BAT_LOW;
			}			
		}
		if(Arg == 0)
		{
			if(StateBuffer == SLAVE_FALSE)
			{//只有失败时才更新
				SlaveInfo.Info[i].State = SLAVE_FALSE;
			}		
		}
		else
		{//无条件更新从机状态
			SlaveInfo.Info[i].State = StateBuffer;
		}
		
		OSSchedUnlock();
	}
}

/**
  *************************************************************************************
  * @brief  控制从机雷管测试
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus AllSlaveDetTest(u8 TestType, u8 TimeShare)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u8 Result;
	u16 U16Buffer[10];

	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485设置为发送状态	
	OSTimeDly(CONNECT_RS485_TIME+100);//延长足够时间，让从机确定极性
	
	AllSlaveStatusReset();
	OSTimeDly(500);
	AllSlaveSetRtc();
	GetAllSlaveInfo(0);//读从机的基本信息
		
	//先判断当前端口是否已经保护
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
		{
			if(GetSlaveBusVoltageCtrlState(i, &Result) == SLAVE_FALSE)
			{//有从机通信失败
				SlaveInfo.Info[i].State = SLAVE_FALSE;
			}
			else if(Result == '1')
			{//从机当前端口不在低压状态
				SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
				SlaveInfo.Info[i].State = SLAVE_TRUE;
			}		
		}
        //读每个从机注册数
        if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
        {
            if(GetSlaveTestInfo(i, 0) == SLAVE_FALSE)
            {
                SlaveInfo.Info[i].State = SLAVE_FALSE;
            }
            else
            {
                SlaveInfo.Info[i].Flags = 1;//表示正在已读到注册雷管
            }
        }
	}
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{//GUI任务发现有错误时会停止当前任务
			return ERROR;
		}
		else if ((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
		{//GUI任务发现有错误时会停止当前任务
			return ERROR;
		}
	}
	AllSlaveStatusReset();
	OSTimeDly(500);	
	//控制从机开始测试雷管
    if(TimeShare)
    {
        //分时检测
        for(i=0; i<SlaveInfo.Number; i++)
        {
            if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
            {
                if(SlaveInfo.Info[i].Flags == 1)
                {
                    SlaveInfo.Info[i].ErrorCount = 0;
                    SlaveInfo.Info[i].Flags = 2;//表示正在测试雷管，用于GUI显示
                    
                    if(TestType == 0)
                    {//快速测试
                        if(SetSlaveStartOperate(i, 5, NULL) == SLAVE_FALSE)
                        {//有从机通信失败
                            SlaveInfo.Info[i].State = SLAVE_FALSE;
                            return ERROR;
                        }			
                    }
                    else
                    {//起爆流程测试
                        if(SetSlaveStartOperate(i, 3, NULL) == SLAVE_FALSE)
                        {//有从机通信失败
                            SlaveInfo.Info[i].State = SLAVE_FALSE;
                            return ERROR;
                        }			
                    }                
                }			

                OSTimeDlyHMSM(0, 0, 4, 0);
                while(SlaveInfo.Info[i].State != SLAVE_TRUE)
                {
                    OSTimeDlyHMSM(0, 0, 1, 0);
                    StateBuffer = GetSlaveBlastDetTestResult(i, &U16Buffer[0]);
                    if (StateBuffer != SLAVE_FALSE)
                    {
                        OSSchedLock();
                        SlaveInfo.Info[i].RegCount = U16Buffer[0];
                        SlaveInfo.Info[i].ErrorCount = U16Buffer[1];
                        SlaveInfo.Info[i].V_BeforeTest = U16Buffer[2];
                        SlaveInfo.Info[i].I_BeforeTest = U16Buffer[3];
                        SlaveInfo.Info[i].I_Leak = U16Buffer[4];
                        OSSchedUnlock();
                        if(StateBuffer == SLAVE_TRUE)
                        {//已经测试完成
                            //回读数据前先控制下一个表检测雷管，节省时间
                            if((i+1) < SlaveInfo.Number)
                            {
                                if(SlaveInfo.Info[i+1].Flags == 1)
                                {
                                    SlaveInfo.Info[i+1].ErrorCount = 0;
                                    SlaveInfo.Info[i+1].Flags = 2;//表示正在测试雷管，用于GUI显示
                                    
                                    if(TestType == 0)
                                    {//快速测试
                                        if(SetSlaveStartOperate(i+1, 5, NULL) == SLAVE_FALSE)
                                        {//有从机通信失败
                                            SlaveInfo.Info[i+1].State = SLAVE_FALSE;
                                            return ERROR;
                                        }			
                                    }
                                    else
                                    {//起爆流程测试
                                        if(SetSlaveStartOperate(i+1, 3, NULL) == SLAVE_FALSE)
                                        {//有从机通信失败
                                            SlaveInfo.Info[i+1].State = SLAVE_FALSE;
                                            return ERROR;
                                        }			
                                    }                
                                }                        
                            }
                        
                            OSSchedLock();
                            if(abs(SlaveInfo.Info[i].V_BeforeTest - LIN_NORMAL_LOW_VOLTAGE_H_GOAL) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
                            {//判断总线否异常
                                SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
                            }
                            if(SlaveInfo.Info[i].ErrorCount != 0)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_DET_ERROR;
                            }
                            if(SlaveInfo.Info[i].I_Leak > 500)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_BUS_LEAK;
                            }
                            OSSchedUnlock();
                
                            SlaveInfo.Info[i].Flags = 3;//表示正在回读雷管内码，用于GUI显示
                            
                            //回读所有的雷管测试数据及雷管内码保存到本地
                            if (GetSlaveDetInfo(i) == SLAVE_TRUE)
                            {
                                if(ERROR != SaveSlaveDetInfo(i))//保存到文件中
                                {
                                    //统计当前已经测试完成的雷管数
                                    OSSchedLock();
                                    SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
                                    SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;
                                    SlaveInfo.OnlineCount += GetDetOnlineCount();
                                    OSSchedUnlock();
                                }
                                else
                                {
                                    SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_SYSTEM;
                                }
                                SlaveInfo.Info[i].State = SLAVE_TRUE;
                            }
                            else
                            {//有从机通信失败
                                SlaveInfo.Info[i].State = SLAVE_FALSE;						
                            }					
                        }																					
                    }
                    else
                    {//有从机通信失败
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                    }
                    if(SlaveInfo.Info[i].State == SLAVE_FALSE)
                    {
                        return ERROR;
                    }
                    else if((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
                    {
                        return ERROR;
                    }            
                }
            }
        }        
    }
    else
    {//多个表并行检测
        //控制从机开始测试雷管
        for(i=0; i<SlaveInfo.Number; i++)
        {
            if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
            {
                if(SlaveInfo.Info[i].Flags == 1)
                {
                    SlaveInfo.Info[i].ErrorCount = 0;
                    SlaveInfo.Info[i].Flags = 2;//表示正在测试雷管，用于GUI显示
                }
                
                
                if(TestType == 0)
                {//快速测试
                    if(SetSlaveStartOperate(i, 5, NULL) == SLAVE_FALSE)
                    {//有从机通信失败
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                        return ERROR;
                    }			
                }
                else
                {//起爆流程测试
                    if(SetSlaveStartOperate(i, 3, NULL) == SLAVE_FALSE)
                    {//有从机通信失败
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                        return ERROR;
                    }			
                }		
            }
        }
        //等待操作完成
        while(1)
        {
            OSTimeDlyHMSM(0,0,1,0);
            for(i=0; i<SlaveInfo.Number; i++)
            {
                if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
                {//该从机还没有测试完成
                    StateBuffer = GetSlaveBlastDetTestResult(i, &U16Buffer[0]);
                    if (StateBuffer != SLAVE_FALSE)
                    {
                        OSSchedLock();
                        SlaveInfo.Info[i].RegCount = U16Buffer[0];
                        SlaveInfo.Info[i].ErrorCount = U16Buffer[1];
                        SlaveInfo.Info[i].V_BeforeTest = U16Buffer[2];
                        SlaveInfo.Info[i].I_BeforeTest = U16Buffer[3];
                        SlaveInfo.Info[i].I_Leak = U16Buffer[4];
                        OSSchedUnlock();
                        if(StateBuffer == SLAVE_TRUE)
                        {//已经测试完成
                            OSSchedLock();
                            if(abs(SlaveInfo.Info[i].V_BeforeTest - LIN_NORMAL_LOW_VOLTAGE_H_GOAL) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
                            {//判断总线否异常
                                SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
                            }
                            if(SlaveInfo.Info[i].ErrorCount != 0)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_DET_ERROR;
                            }
                            if(SlaveInfo.Info[i].I_Leak > 500)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_BUS_LEAK;
                            }
                            OSSchedUnlock();
                            
                            SlaveInfo.Info[i].Flags = 3;//表示正在回读雷管内码，用于GUI显示
                            
                            //回读所有的雷管测试数据及雷管内码保存到本地
                            if (GetSlaveDetInfo(i) == SLAVE_TRUE)
                            {
                                if(ERROR != SaveSlaveDetInfo(i))//保存到文件中
                                {
                                    //统计当前已经测试完成的雷管数
                                    OSSchedLock();
                                    SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
                                    SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;
                                    SlaveInfo.OnlineCount += GetDetOnlineCount();							
                                    OSSchedUnlock();
                                }
                                else
                                {
                                    SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_SYSTEM;
                                }
                                SlaveInfo.Info[i].State = SLAVE_TRUE;
                            }
                            else
                            {//有从机通信失败
                                SlaveInfo.Info[i].State = SLAVE_FALSE;						
                            }					
                        }																					
                    }
                    else
                    {//有从机通信失败
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                    }			
                }
                if(SlaveInfo.Info[i].State == SLAVE_FALSE)
                {
                    return ERROR;
                }
                else if((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
                {
                    return ERROR;
                }
            }
            
            if(GetSlaveStateCount(SLAVE_VOLATILE) == 0)
            {//所有从机都已经操作完成
                break ;
            }		
        }
    }
	GetSlaveDetNumber();
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  控制从机网络充电
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus AllSlaveCharge(void)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u8 Result;
	u16 U16Buffer[10];
	
	SlaveInitState();
	
	//先判断当前端口是否已经保护
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(GetSlaveBusVoltageCtrlState(i, &Result) == SLAVE_FALSE)
		{//有从机通信失败
			SlaveInfo.Info[i].State = SLAVE_FALSE;
			return ERROR;
		}
		else if(Result == '1')
		{//从机当前端口不在低压状态
			SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
			SlaveInfo.Info[i].State = SLAVE_TRUE;
			return ERROR;
		}
	}
	
	//控制从机开始测试雷管
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
		{
			if(SetSlaveStartOperate(i, 6, NULL) == SLAVE_FALSE)
			{//有从机通信失败
				SlaveInfo.Info[i].State = SLAVE_FALSE;
				return ERROR;
			}		
		}
	}
	
	//等待操作完成
	while(1)
	{
		OSTimeDlyHMSM(0,0,1,0);
		for(i=0; i<SlaveInfo.Number; i++)
		{
			if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
			{//该从机还没有充电完成
				StateBuffer = GetSlaveBlastChargeResult(i, &U16Buffer[0]);
				if (StateBuffer == SLAVE_TRUE)
				{//已经充电完成
					OSSchedLock();
					SlaveInfo.Info[i].V_AfterCharge = U16Buffer[0];
					SlaveInfo.Info[i].I_AfterCharge = U16Buffer[1];
					SlaveInfo.Info[i].V_BeforeCharge = U16Buffer[2];
					SlaveInfo.Info[i].I_BeforeCharge = U16Buffer[3];					
//					if(abs(SlaveInfo.Info[i].V_AfterCharge - Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
                    if((SlaveInfo.Info[i].V_AfterCharge < (LIN_NORMAL_HIGH_VOLTAGE_FACTORY_GOAL-1000)) || 
                        (SlaveInfo.Info[i].V_AfterCharge > LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX))
					{//判断总线否异常
						SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
					}
					
					SlaveInfo.Info[i].State = SLAVE_TRUE;
					OSSchedUnlock();																						
				}
				else if (StateBuffer == SLAVE_FALSE)
				{//有从机通信失败
					SlaveInfo.Info[i].State = SLAVE_FALSE;
				}			
			}
			if(SlaveInfo.Info[i].State == SLAVE_FALSE)
			{
				return ERROR;
			}
			else if((SlaveInfo.Info[i].State == SLAVE_VOLATILE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
			{
				return ERROR;
			}			
		}
		
		if(GetSlaveStateCount(SLAVE_VOLATILE) == 0)
		{//所有从机都已经操作完成
			break ;
		}		
	}
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  读从机起爆结果
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void AllSlaveBlastDet(void)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u16 U16Buffer[10];
	
	SlaveInitState();

	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	
	//等待操作完成
	for(i=0; i<SlaveInfo.Number; i++)
	{
		StateBuffer = GetSlaveBlastResult(i, &U16Buffer[0]);
		if (StateBuffer == SLAVE_TRUE)
		{//已经起爆完成
			OSSchedLock();
			if(U16Buffer[0] == '1')
			{//已经发送起爆信号
				SlaveInfo.Info[i].V_BeforeFire = U16Buffer[1];
				SlaveInfo.Info[i].I_BeforeFire = U16Buffer[2];				
			}
			else
			{
				if(U16Buffer[0] == '2')
				{//从机因为端口电压异常没有发送起爆信号
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
					SlaveInfo.Info[i].V_BeforeFire = 0xFFFE;
					SlaveInfo.Info[i].I_BeforeFire = 0xFFFE;					
				}
				else
				{//从机没有收到起爆命令
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BLAST_CMD;
					SlaveInfo.Info[i].V_BeforeFire = 0xFFFD;
					SlaveInfo.Info[i].I_BeforeFire = 0xFFFD;					
				}
			}
			SlaveInfo.Info[i].State = SLAVE_TRUE;
			OSSchedUnlock();			
		}
		else
		{//有从机通信失败
			SlaveInfo.Info[i].V_BeforeFire = 0xFFFF;
			SlaveInfo.Info[i].I_BeforeFire = 0xFFFF;		
			SlaveInfo.Info[i].State = SLAVE_FALSE;
		}					
	}
	SetSlaveStartOperate(0xFF, 0, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 0, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 0, NULL);	
}

/**
  *************************************************************************************
  * @brief  设置从机状态重新开始
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void AllSlaveStatusReset(void)
{
	SetSlaveStartOperate(0xFF, 11, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 11, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 11, NULL);	
}

/**
  *************************************************************************************
  * @brief  设置从机统一校准RTC
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年04月03日
  * @note   
  *************************************************************************************  
  */
void AllSlaveSetRtc(void)
{
	SetSlaveRtc(0xFF);
	OSTimeDly(10);
	SetSlaveRtc(0xFF);	
}

/**
  *************************************************************************************
  * @brief  通过485导入从机的雷管数据
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2017年07月26日
  * @note   
  *************************************************************************************  
  */
u8 BackupImportDetDataFromSlave(void)
{
	u8 addr = 0;

	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485设置为发送状态	
	OSTimeDly(CONNECT_RS485_TIME+100);//延长足够时间，让从机确定极性
	
	//数据备份时只能接一个设备，获取对应的通信地址
	if(GetSlaveAddr(&addr) == SLAVE_FALSE)
	{
		return 0x01;//没有找到从机，有可能没接，或者接了多个
	}
	
	if(GetSlaveVerison(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	if(FormatIsSameForDetInfo(SlaveInfo.Info[addr].Version) == 0)
	{
		return 2;//设备类型不相同
	}
	
	if(GetSlaveTestInfo(addr, 0) == SLAVE_FALSE)
	{//有从机通信失败
		return 0xFF;
	}			
	
	if(SlaveInfo.Info[addr].RegCount == 0)
	{
		return 3;//没有注册雷管
	}
	if(SlaveInfo.Info[addr].Type != Det.Type)
	{
		return 4;//雷管类型不同
	}

	if (GetSlaveDetInfo(addr) != SLAVE_TRUE)
	{
		return 0xFF;
	}
	GetDetNum();
	if(ERROR == SaveAllDetInfo(0))
	{
		return 5;//保存数据失败
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  通过485读控制从设备删除雷管数据
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2017年07月27日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDeleteDetInfo(u8 Index, u8* Result)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2B;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2B) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							*Result = RS485TempBuffer.uch_buf[5];
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  通过485读控制从设备删除雷管数据
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2017年07月27日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDetHoleNumber(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x30;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包	

		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], 
				"%u\r%u\r", 0, DET_MAX_SIZE);	
					
		memcpy(&RS485Data.uch_buf[RS485Data.bytenum], &Det.HoleNumber[0], DET_MAX_SIZE);
		RS485Data.bytenum += DET_MAX_SIZE;	
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x30) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  通过485读控制从设备删除雷管数据
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2017年07月27日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDetInfo(u8 Index, u16 LocalIndex, u16 SlaveIndex, u8 Number)
{
	u32 c_time;
	u8 U8Value, Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2C;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", SlaveIndex);
		U8Value = 0;
		while(U8Value < Number)
		{
			//雷管内码\孔号\频率\延期\状态码
			RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], 
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
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2C) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  通过485读控制从设备删除雷管数据
  * @param  Index：要读的设备索引
  * @retval 结果
  * @author ZH
  * @date   2017年07月27日
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveSaveDetInfo(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//协议地址
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//命令码高字节
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x31;//命令码低字节
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//分包包号低字节，不需要分包			
		
		Pack(&RS485Data, &RS485TempBuffer);//数据打包
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485设置为发送状态
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x31) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485设置为发送状态
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  通过485把雷管数据导出到从机
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2017年07月26日
  * @note   
  *************************************************************************************  
  */
u8 BackupExportDetDataToSlave(void)
{
	u8 addr = 0;
	u8 U8Value, Result;
	u16 StartIndex = 0;
	
	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485设置为发送状态	
	OSTimeDly(CONNECT_RS485_TIME+100);//延长足够时间，让从机确定极性	

	//数据备份时只能接一个设备
	if(GetSlaveAddr(&addr) == SLAVE_FALSE)
	{
		return 1;//没有找到从机，有可能没接，或者接了多个从机
	}
	
	if(GetSlaveVerison(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	if(FormatIsSameForDetInfo(SlaveInfo.Info[addr].Version) == 0)
	{
		return 2;//设备类型不相同
	}
	
	if(GetSlaveTestInfo(addr, 0) == SLAVE_FALSE)
	{//有从机通信失败
		return 0xFF;
	}			
	
	if(SlaveInfo.Info[addr].Type != Det.Type)
	{
		return 3;//雷管类型不同
	}
	//控制从机删除雷管
	if(SetSlaveDeleteDetInfo(addr, &Result) == SLAVE_FALSE)
	{
		return 0xFF;			
	}
	if(Result != '1')
	{
		return 4;//从机删除数据失败
	}
	
	//导出孔内雷管数
	if(SetSlaveDetHoleNumber(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	//导出雷管内码及延期数据
	U8Value =  Det.RegCount/10;
	while(U8Value--)
	{
		if(SetSlaveDetInfo(addr, StartIndex, StartIndex, 10) == SLAVE_FALSE)//每次10发雷管
		{
			return 0xFF;
		}
		StartIndex += 10;
	}
	if(Det.RegCount%10)
	{
		if(SetSlaveDetInfo(addr, StartIndex, StartIndex, Det.RegCount%10) == SLAVE_FALSE)
		{
			return 0xFF;
		}	
	}			
	//控制从机保存数据
	if(SetSlaveSaveDetInfo(addr) == SLAVE_FALSE)
	{
		return 0xFF;				
	}
	return 0;		
}



