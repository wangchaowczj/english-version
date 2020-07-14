#include "includes.h"

TEST_PARAM test_param;
CHANNEL Channel;
TEST_PARAM test_param_cfg[3];//入厂检0，半成品检是1，成品检是2



//==================================================================================================
//| 函数名称 | DetCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管正向电流测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管测试代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
DET_ERR_CODE DetLVCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //低压下的正向电流值
	
	OSSchedLock();
	Channel.XA2_Info[Index].lv_current = value; //低压下的反向电流值
	OSSchedUnlock();	
	
	if(Channel.XA2_Info[Index].lv_current > test_param.det_lv_current_max)
	{
	   	return LV_CURRENT_MORE;//正向电流大
	}
	else if(Channel.XA2_Info[Index].lv_current < test_param.det_lv_current_min) 
	{
	    return LV_CURRENT_LESS;//正向电流小
	}
	else
	{
	  	return DET_PASS;//正向电流满足要求
	}
}

//==================================================================================================
//| 函数名称 | DetReverseCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管反向电流测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管测试代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
DET_ERR_CODE DetLVReverseCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //低压下的反向电流值
	
	OSSchedLock();
	Channel.XA2_Info[Index].lv_reverse_current = value; //低压下的反向电流值
	OSSchedUnlock();	
	 
	if(Channel.XA2_Info[Index].lv_reverse_current > test_param.det_lv_current_max)
	{
	   	return LV_REVERSE_CURRENT_MORE;//反向电流大
	}
	else if(Channel.XA2_Info[Index].lv_reverse_current < test_param.det_lv_current_min) 
	{
	    return LV_REVERSE_CURRENT_LESS;//反向电流小
	}
	else
	{
	  	return DET_PASS;//正向电流满足要求
	}
}

//==================================================================================================
//| 函数名称 | DetCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管正向电流测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管测试代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
DET_ERR_CODE DetHVCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //高压下的正向电流值
	
	OSSchedLock();
	Channel.XA2_Info[Index].hv_current = value; //低压下的反向电流值
	OSSchedUnlock();	

	if(Channel.XA2_Info[Index].hv_current > test_param.det_hv_current_max)
	{
	   	return HV_CURRENT_MORE;//正向电流大
	}
	else if(Channel.XA2_Info[Index].hv_current < test_param.det_hv_current_min) 
	{
	    return HV_CURRENT_LESS;//正向电流小
	}
	else
	{
	  	return DET_PASS;//正向电流满足要求
	}
}

//==================================================================================================
//| 函数名称 | DetReverseCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管反向电流测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管测试代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
DET_ERR_CODE DetHVReverseCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //高压下的反向电流值
	
	OSSchedLock();
	Channel.XA2_Info[Index].hv_reverse_current = value; //低压下的反向电流值
	OSSchedUnlock();
	
	if(Channel.XA2_Info[Index].hv_reverse_current > test_param.det_hv_current_max)
	{
	   	return HV_REVERSE_CURRENT_MORE;//正向电流大
	}
	else if(Channel.XA2_Info[Index].hv_reverse_current < test_param.det_hv_current_min) 
	{
	    return HV_REVERSE_CURRENT_LESS;//正向电流小
	}
	else
	{
	  	return DET_PASS;//正向电流满足要求
	}
}

void ClearDetInfo(void)
{
	u8 i = 0;
	
	OSSchedLock();
	for(i = 0; i < Channel.Count;i++)
	{
		memset(&Channel.XA2_Info[i], 0x00,sizeof(Channel.XA2_Info[i]));
        memset(&Channel.XA2_Info[i], 0x00,sizeof(Channel.XA2_Info[i]));
		Channel.XA2_Info[i].Status = DET_TESTING;	
	}
	OSSchedUnlock();
}
void ClearNewStatus(void)
{
	u8 i = 0;
	
	OSSchedLock();
	for(i = 0; i < Channel.Count;i++)
	{
		Channel.NewStatus[i] = DET_TESTING;	
	}
	OSSchedUnlock();
}
u32 GetDetFreqFromTimerCount(u16 count)
{
	
	//定时器3经过了6分频后为12M,count对应的是雷管返回的1位数据时间，对应雷管16个CLK
	//t = 1/12000000*count/16

	return (192000000/count);
}
DET_ERR_CODE DetFreqTest(u32 freq)
{          	   
	if(freq > test_param.det_freq_max)
	{
		return DET_FREQ_MAX;   //频率过大错误
	}
	else if(freq < test_param.det_freq_min)
	{
		return DET_FREQ_MIN;   //频率过大错误
  	}
	else
	{
		return DET_PASS;
	}
}
/**
  *************************************************************************************
  * @brief  等待电流小于指定值
  * @param  CurrentValue 电流，uA
  * @param  TimeOut 超时时间 
  * @retval SUCCESS 成功， ERROR失败
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************
  */
ErrorStatus WaitCurrentReady(u8 CurrentValue, u16 TimeOut)
{
	u32 c_time;
	
	//延时等待完成,示波器实测，至少1秒。修改硬件后，可减少到90ms左右
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GetCurrentValue(10, NULL) < (CurrentValue*100UL))
		{
			OSTimeDlyHMSM(0,0,0,5);
			if(GetCurrentValue(10, NULL) < (CurrentValue*100UL))
			{
				return SUCCESS;
			}			
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  等待接收引脚处于正常状态(低电平)
  * @param  TimeOut 超时时间
  * @retval SUCCESS 成功， ERROR失败
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************
  */
ErrorStatus WaitRxPinReady(u16 TimeOut)
{
	u32 c_time;
	
	//延时等待完成,示波器实测，至少1秒。修改硬件后，可减少到90ms左右
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GET_RX_PIN() == 0)
		{
			OSTimeDlyHMSM(0,0,0,5);
			if(GET_RX_PIN() == 0)
			{
				return SUCCESS;
			}			
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  等待接收引脚处于高电平
  * @param  TimeOut 超时时间
  * @retval SUCCESS 成功， ERROR失败
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************
  */
ErrorStatus WaitRxPinHigh(u16 TimeOut)
{
	u32 c_time;
	
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GET_RX_PIN() != 0)
		{
			return SUCCESS;			
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  判断测试参数相应屏蔽位是否已经置位
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
u8 TestParamIsSet(u32 MaskBit)
{
	if((test_param.test_ctrl & MaskBit) == MaskBit)
	{
		return 1;
	}
	return 0;
}
void SoundActionByStatus(void)
{
	u8 i=0;
	
	switch(UserParam.Sound)
	{
		case FINISHED_RING:
			TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
			break;
		case NOT_RING:
			break;
		case RIGHT_RING://全部正确时响
			for(i=0;i<Channel.Count;i++)
			{
				if(DET_PASS != Channel.XA2_Info[i].Status)
				{
					return;
				}
			}		
			TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
			break;
		case ERROR_RING://有错误时响
			for(i=0;i<Channel.Count;i++)
			{
				if(DET_PASS != Channel.XA2_Info[i].Status)
				{
					TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
					return;
				}
			}		
			break;								
	}
}

u8 GetChannelStatusCount(DET_ERR_CODE Status)
{
	u8 i=0;
    u8 Count = 0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(Status == Channel.XA2_Info[i].Status)
		{
			Count++;
		}
	}
	OSSchedUnlock();
    return Count;
}
u8 GetChannelNewStatusCount(DET_ERR_CODE Status)
{
	u8 i=0;
    u8 Count = 0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(Status == Channel.NewStatus[i])
		{
			Count++;
		}
	}
	OSSchedUnlock();
    return Count;
}
void AllChannelStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus)
{
	u8 i=0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(OldStatus == Channel.XA2_Info[i].Status)
		{
			Channel.XA2_Info[i].Status = NewStatus;
		}
	}
	OSSchedUnlock();
}
void AllChannelNewStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus)
{
	u8 i=0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(OldStatus == Channel.NewStatus[i])
		{
			Channel.NewStatus[i] = NewStatus;
		}
	}
	OSSchedUnlock();
}
/**
  *************************************************************************************
  * @brief  判断是否所有通道都已经检测完成
  * @param  无
  * @retval 0 不是所有通道都检测完成
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
u8 AllChannelTestEnd(void)
{
	u8 i;
	
	for(i=0;i<Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			return 0;			
		}
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  判断是否所有通道都已经检测完成
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
u8 AllChannelTestEndFromBuffer(DET_ERR_CODE* Status)
{
	u8 i;
	
	for(i=0;i<Channel.Count;i++)
	{
		if(DET_TESTING == Status[i])
		{
			return 0;			
		}
	}
	return 1;
}

/**
  *************************************************************************************
  * @brief  总线短路测试
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelLinShortTest(u8 type)
{
	u8 i;
	DET_ERR_CODE* Status;
      
	for(i=0;i<Channel.Count;i++)
	{
        if(type == 0)
        {
            Status = &Channel.XA2_Info[i].Status;
        }
        else
        {
            Status = &Channel.NewStatus[i];
        }    
		if(DET_TESTING == (*Status))
		{
			ConnectChannel(i);
			OSTimeDly(5);
			if(GetLinABVoltage(i, NULL) < 1000)
			{
				*Status = DET_SHORT_ERR;//总线短路
			}		
		}
	}
}

/**
  *************************************************************************************
  * @brief  总线断路测试
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelOpenCircuitTest(u8 type)
{
	u8 i;
	DET_ERR_CODE* Status;
    	
	for(i=0;i<Channel.Count;i++)
	{
        if(type == 0)
        {
            Status = &Channel.XA2_Info[i].Status;
        }
        else
        {
            Status = &Channel.NewStatus[i];
        }    
		if(DET_TESTING == (*Status))
		{
			ConnectChannel(i);
			OSTimeDly(5);
			if(GetCurrentValue(10, NULL) < 500)
			{//电流小于2uA
				*Status = DET_OPEN_CIRCUIT_ERR;//总线断路
			}			
		}
	}
}
/**
  *************************************************************************************
  * @brief  低压下电流正/反向测试
  * @param  ReverseFlag  0时测正向，1测反向
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelCurrentTest(u8 ReverseFlag)
{
	DET_ERR_CODE result = DET_PASS;
	u8 i;
	u8 Ok[DET_CHANNEL_MAX];
	u8 Less[DET_CHANNEL_MAX];
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u32 c_time, timer_out;
	
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;
		Ok[i] = 0;
		Less[i] = 0;		
	}
	
	timer_out = test_param.det_lv_current_time_out;//单位为秒
	timer_out *= 1000;//ms
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(50);	
		for(i=0; i<Channel.Count; i++)
		{
			if(DET_TESTING == Status[i])
			{
				ConnectChannel(i);
				OSTimeDly(5);
				if(ReverseFlag == 0)
				{//测正向电流
					result = DetLVCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (LV_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				else
				{//测反向电流
					result = DetLVReverseCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (LV_REVERSE_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				if ((Ok[i] >= 3) || (Less[i] >= 3))
				{
					Status[i] = result;
				}
			}				
		}
		//判断是否所有通道都已经检测完成
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_TESTING == Status[i])
		{
			if(ReverseFlag == 0)
			{
				Status[i] = LV_CURRENT_MORE;
			}
			else
			{
				Status[i] = LV_REVERSE_CURRENT_MORE;
			}		
		}
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}
}

/**
  *************************************************************************************
  * @brief  总线电压测试
  * @param  Flag  0时低压，1时高压
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelVoltageTest(u8 Flag)
{
	u16 Voltage;
	u32 c_time;
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;	
	}
	
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(10);	
		for(i=0; i<Channel.Count; i++)
		{
			if((DET_TESTING == Channel.XA2_Info[i].Status) && (DET_PASS != Status[i]))
			{
				ConnectChannel(i);
				OSTimeDly(5);
				Voltage = GetLinABVoltage(i, NULL);
				if(Flag == 0)
				{//低压
					if((Voltage > LINAB_LV_VOLTAGE_MIN) && (Voltage < LINAB_LV_VOLTAGE_MAX))
					{
						Status[i] = DET_PASS;				
					}
					else
					{
						if(Voltage < LINAB_LV_VOLTAGE_MIN)
						{
							Status[i] = LV_VOLTAGE_MIN;
						}
						else
						{
							Status[i] = LV_VOLTAGE_MAX;
						}
					}				
				}
				else
				{//高压
					if((Voltage > LINAB_HV_VOLTAGE_MIN) && (Voltage < LINAB_HV_VOLTAGE_MAX))
					{
						Status[i] = DET_PASS;					
					}
					else
					{
						if(Voltage < LINAB_HV_VOLTAGE_MIN)
						{
							Status[i] = HV_VOLTAGE_MIN;
						}
						else
						{
							Status[i] = HV_VOLTAGE_MAX;
						}
					}				
				}
			}		
		}
		//判断是否所有通道都已经检测完成
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}		
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}	
}
/**
  *************************************************************************************
  * @brief  频率测试
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelDetFreqTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			Result = DetFreqTest(Channel.XA2_Info[i].freq);
			if(Result != DET_PASS)
			{
				Channel.XA2_Info[i].Status = Result;
			}					
		}		
	}
}
void SetAllChannelBarCodeIdle(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		Channel.BarCode[i].Status = BARCODE_IDLE;
	}	
}
u8 AllChannelIsOpenCircuit(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		OSTimeDly(5);
		
		if(GetCurrentValue(10, NULL) >= 500)
		{
			return 0;
		}		
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  低压下电流正/反向测试
  * @param  ReverseFlag  0时测正向，1测反向
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void AllChannelDetHVCurrentTest(u8 ReverseFlag)
{
	DET_ERR_CODE result = DET_PASS;
	u8 i;
	u8 Ok[DET_CHANNEL_MAX];
	u8 Less[DET_CHANNEL_MAX];
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u32 c_time, timer_out;
	
	if(AllChannelTestEnd())
	{
		return;
	}
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;
		Ok[i] = 0;
		Less[i] = 0;		
	}
	
	timer_out = test_param.det_hv_current_time_out;//单位为秒
	timer_out *= 1000;//ms
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(50);	
		for(i=0; i<Channel.Count; i++)
		{
			if(DET_TESTING == Status[i])
			{
				ConnectChannel(i);
				OSTimeDly(5);
				if(ReverseFlag == 0)
				{//测正向电流
					result = DetHVCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (HV_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				else
				{//测反向电流
					result = DetHVReverseCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (HV_REVERSE_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				if ((Ok[i] >= 3) || (Less[i] >= 3))
				{
					Status[i] = result;
				}
			}				
		}
		//判断是否所有通道都已经检测完成
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_TESTING == Status[i])
		{
			if(ReverseFlag == 0)
			{
				Status[i] = HV_CURRENT_MORE;
			}
			else
			{
				Status[i] = HV_REVERSE_CURRENT_MORE;
			}		
		}
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}
}
/**
  *************************************************************************************
  * @brief  内码全0检测
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年01月19日
  * @note   无
  *************************************************************************************
  */
u8 DetCodeIsAllZero(DET_INFO* Det)
{
    if ((0 == Det->ser1) && (0 == Det->ser2) && (0 == Det->ser3) && 
        (0 == Det->fac) && (0 == Det->bat) && (0 == Det->year))
    {
        return 1;
    }    
    return 0;
}
/**
  *************************************************************************************
  * @brief  内码全0检测
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年02月10日
  * @note   无
  *************************************************************************************
  */
//void XA_AllChannelDetCodeAllZeroVerify(void)
//{
//	u8 i=0;
//	
//	for(i=0; i<Channel.Count; i++)
//	{
//		if(DET_TESTING == Channel.XA2_Info[i].Status)
//		{
//			if (DetCodeIsAllZero(&Channel.XA2_Info[i]))
//			{
//				Channel.XA2_Info[i].Status = DET_CODE_ALL_ZERO;//内码全0
//			}							
//		}		
//	}	
//}


/**
  *************************************************************************************
  * @brief  雷管检测
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年02月10日
  * @note   无
  *************************************************************************************
  */
void DetTest(void)
{
    OpData.NetModeNext = 0;
    if(UserParam.NetMode)
    {
        OPEN_IO_RELAY1();    
    }

    if(UserParam.Stage == DET_CODE_VERIFY)
	{
		XA2_DetCodeVerify();
	}
	else
	{
		XA2_DetTest();
	}
    OpData.NetModeNext = 0;
    CLOSE_IO_RELAY1();
}
/**
  *************************************************************************************
  * @brief  雷管分析,用通道1
  * @param[in]  无
  * @retval 无
  * @author ZH
  * @date   2016年12月5日
  * @note   
  *************************************************************************************  
  */
u16 DetAnalyst(void)
{
//    if(DeviceInfo.DetClass == 0)
//    {
        return XA2_DetAnalyst();
//    }

}
/**
  *************************************************************************************
  * @brief  判断各个通道是否已经接上雷管
  * @param[in]  无
  * @retval 无
  * @author ZH
  * @date   2016年12月5日
  * @note   
  *************************************************************************************  
  */
void QueryDetConnectStatus(void)
{
    XA2_QueryDetConnectStatus();
}
/**
  *************************************************************************************
  * @brief  等待各个通道雷管都下线
  * @param[in]  无
  * @retval 无
  * @author ZH
  * @date   2016年12月5日
  * @note   
  *************************************************************************************  
  */
void WaitAllChannelDetOff(void)
{
     XA2_WaitAllChannelDetOff();
}
