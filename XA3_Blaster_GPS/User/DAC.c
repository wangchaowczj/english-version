#include "includes.h"

static u16 DacLvValueMin = 0;
static u16 DacHvValueMin = 0;


/**
  *************************************************************************************
  * @brief  dac配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DacConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(DAC_LV_PORT_RCC | DAC_HV_PORT_RCC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = DAC_LV_PIN;
	GPIO_Init(DAC_LV_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DAC_HV_PIN;
	GPIO_Init(DAC_HV_PORT,&GPIO_InitStructure);
		
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_None;    //不使用触发功能 TEN1=0
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//用来设置屏蔽/幅值选择器，这个变量只在使用波形发生器的时候才有用，这里我们设置为0即可
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;    //DAC1输出缓存关闭
	
	DAC_Init(DAC_LV_CHANNEL,&DAC_InitStruct);
	
	DAC_Init(DAC_HV_CHANNEL,&DAC_InitStruct);
	
	DAC_Cmd(DAC_LV_CHANNEL,ENABLE);
	
	DAC_Cmd(DAC_HV_CHANNEL,ENABLE);
	
	setDacLvValueMinFromVoltage(LIN_NORMAL_LOW_VOLTAGE_ADJUST_MAX);
	setDacHvValueMinFromVoltage(LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX);

	DacOutLVVoltage(LIN_NORMAL_LOW_VOLTAGE_L_GOAL);
	DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
}
/**
  *************************************************************************************
  * @brief  DAC启动输出
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   
  *************************************************************************************  
  */
void DacStartOutput(uint32_t DAC_Channel, u16 AdcValue)
{
	AdcValue &= 0xFFF;
	if(DAC_GetDataOutputValue(DAC_Channel) != AdcValue)
	{
		if(DAC_Channel == DAC_Channel_1)
		{
			DAC_SetChannel1Data(DAC_Align_12b_R, AdcValue);//12位右对齐，数值小于4096
		}
		else if(DAC_Channel == DAC_Channel_2)
		{
			DAC_SetChannel2Data(DAC_Align_12b_R, AdcValue);//12位右对齐，数值小于4096
		}		
	}	
	DAC_Cmd(DAC_Channel, ENABLE);//PA4
	
  	DAC_SoftwareTriggerCmd(DAC_Channel, ENABLE);
}
/**
  *************************************************************************************
  * @brief  获取Dac值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note  
  *************************************************************************************  
  */
u16 GetDacCurrentValue(uint32_t DAC_Channel)
{
	return DAC_GetDataOutputValue(DAC_Channel);
}
/**
  *************************************************************************************
  * @brief  设置LV最小电压
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
void SetLVVoltageMin(void)
{
	DacStartOutput(DAC_LV_CHANNEL, 0xFFF);	
}

/**
  *************************************************************************************
  * @brief  设置LV最大电压
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
void SetLVVoltageMax(void)
{
	DacStartOutput(DAC_LV_CHANNEL, getDacLvValueMin());	
}
/**
  *************************************************************************************
  * @brief  设置LV电压递增
  * @param  DacStep 每次DAC值减小多少
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
ErrorStatus SetLVVoltageUp(u8 DacStep)
{
	u16 DAdValue;
	
	DAdValue = GetDacCurrentValue(DAC_LV_CHANNEL);
	if(DAdValue == 0)
	{//ADC的值越小，调压电路产生的电压越大，当前电压已经达到最大值
		return ERROR;
	}
	else
	{
		if(DAdValue >= DacStep)
		{
			DAdValue -= DacStep;
		}
		else
		{
			DAdValue = 0;
		}
		if(DAdValue < getDacLvValueMin())
		{
			DAdValue = getDacLvValueMin();
		}		
		DacStartOutput(DAC_LV_CHANNEL, DAdValue);
		return SUCCESS;		
	}
}
/**
  *************************************************************************************
  * @brief  设置LV电压递减
  * @param  DacStep 每次DAC值增加多少
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
ErrorStatus SetLVVoltageDown(u8 DacStep)
{
	u16 DAdValue;
	
	DAdValue = GetDacCurrentValue(DAC_LV_CHANNEL);
	if(DAdValue >= 0xFFF)
	{//ADC的值越小，调压电路产生的电压越大，当前电压已经达到最大值
		return ERROR;
	}
	else
	{	
		DAdValue += DacStep;
		if(DAdValue > 0x0FFF)
		{
			DAdValue = 0x0FFF;
		}
		if(DAdValue < getDacLvValueMin())
		{
			DAdValue = getDacLvValueMin();
		}		
		DacStartOutput(DAC_LV_CHANNEL, DAdValue);
		return SUCCESS;		
	}
}
/**
  *************************************************************************************
  * @brief  DAC控制输出电压值
  * @param  Voltage VCC_LV待输出电压值(mv)
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   DAC引脚输出电压：Vout = 参考电压×DAC数据寄存器值/4095
  *************************************************************************************  
  */
u16 DacOutLVVoltage(u16 Voltage)
{
	u16 DacValue;
	
	if(Voltage > 8500)
	{
		Voltage = 8500;
	}
	else if(Voltage < 4000)
	{
		Voltage = 4000;
	}

	DacValue = 3397.5009 - 0.2797950*Voltage;	
	if(DacValue < getDacLvValueMin())
	{
		DacValue = getDacLvValueMin();
	}
	
	if(DacValue != DAC_GetDataOutputValue(DAC_LV_CHANNEL))
	{
		DAC_SetChannel2Data(DAC_Align_12b_R, DacValue);	
	}	
	return Voltage;
}


/**
  *************************************************************************************
  * @brief  设置HV最小电压
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
void SetHVVoltageMin(void)
{
	DacStartOutput(DAC_HV_CHANNEL, 0xFFF);	
}

/**
  *************************************************************************************
  * @brief  设置HV最大电压
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
void SetHVVoltageMax(void)
{
	DacStartOutput(DAC_HV_CHANNEL, getDacHvValueMin());	
}
/**
  *************************************************************************************
  * @brief  设置HV电压递增
  * @param  DacStep 每次DAC值减小多少
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
ErrorStatus SetHVVoltageUp(u8 DacStep)
{
	u16 DAdValue;
	
	DAdValue = GetDacCurrentValue(DAC_HV_CHANNEL);
	if((DAdValue == 0) || (DAdValue <= getDacHvValueMin()))
	{//ADC的值越小，调压电路产生的电压越大，当前电压已经达到最大值
		return ERROR;
	}
	else
	{
		if(DAdValue >= DacStep)
		{
			DAdValue -= DacStep;
		}
		else
		{
			DAdValue = 0;
		}
		if(DAdValue < getDacHvValueMin())
		{
			DAdValue = getDacHvValueMin();
		}
		
		DacStartOutput(DAC_HV_CHANNEL, DAdValue);
		return SUCCESS;		
	}
}
/**
  *************************************************************************************
  * @brief  设置HV电压递减
  * @param  DacStep 每次DAC值增加多少
  * @retval 无
  * @author ZH
  * @date   2017年2月24日
  * @note   ADC的值越大，调压电路产生的电压越小
  *************************************************************************************  
  */
ErrorStatus SetHVVoltageDown(u8 DacStep)
{
	u16 DAdValue;
	
	DAdValue = GetDacCurrentValue(DAC_HV_CHANNEL);
	if(DAdValue >= 0xFFF)
	{//ADC的值越小，调压电路产生的电压越大，当前电压已经达到最大值
		return ERROR;
	}
	else
	{	
		DAdValue += DacStep;
		if(DAdValue > 0x0FFF)
		{
			DAdValue = 0x0FFF;
		}
		if(DAdValue < getDacHvValueMin())
		{
			DAdValue = getDacHvValueMin();
		}		
		DacStartOutput(DAC_HV_CHANNEL, DAdValue);
		return SUCCESS;		
	}
}
/**
  *************************************************************************************
  * @brief  DAC控制输出电压值
  * @param  Voltage VCC_LV待输出电压值(mv)
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   DAC引脚输出电压：Vout = 参考电压×DAC数据寄存器值/4095
  *************************************************************************************  
  */
u16 DacOutHVVoltage(u16 Voltage)
{
	u16 DacValue;
	
	if(Voltage > 15000)
	{
		Voltage = 15000;
	}
	else if(Voltage < 8000)
	{
		Voltage = 8000;
	}

	DacValue = 3237.941 - 0.1500723*Voltage;
	if(Voltage != DAC_GetDataOutputValue(DAC_HV_CHANNEL))
	{
		DAC_SetChannel1Data(DAC_Align_12b_R, DacValue);
	}		
	return Voltage;
}

u16 getCurrentHvDacValue(void)
{
	return DAC_GetDataOutputValue(DAC_HV_CHANNEL);
}
void LVVoltageOutputAdjust(u16 CurrentVoltage, u16 GoalVoltage)
{													
	if(CurrentVoltage < GoalVoltage)
	{
		if(abs(GoalVoltage - CurrentVoltage) >= 300)
		{//快速调整
			SetLVVoltageUp(30);
		}					
		else if(abs(GoalVoltage - CurrentVoltage) >= 100)
		{//快速调整
			SetLVVoltageUp(15);
		}
		else
		{
			SetLVVoltageUp(1);						
		}
	}
	else if(CurrentVoltage > GoalVoltage)
	{
		if(abs(CurrentVoltage - GoalVoltage) >= 300)
		{//快速调整
			SetLVVoltageDown(30);
		}					
		else if(abs(CurrentVoltage - GoalVoltage) >= 100)
		{//快速调整
			SetLVVoltageDown(15);
		}
		else
		{
			SetLVVoltageDown(1);					
		}
	}
}
void HVVoltageOutputAdjust(u16 CurrentVoltage, u16 GoalVoltage)
{
	if(CurrentVoltage < GoalVoltage)
	{
		if(abs(GoalVoltage - CurrentVoltage) >= 300)
		{//快速调整
			SetHVVoltageUp(30);
		}								
		else if(abs(GoalVoltage - CurrentVoltage) >= 100)
		{//快速调整
			SetHVVoltageUp(15);
		}
		else
		{
			SetHVVoltageUp(1);					
		}
	}
	else if(CurrentVoltage > GoalVoltage)
	{
		if((CurrentVoltage - GoalVoltage) >= 300)
		{//快速调整
			SetHVVoltageDown(30);
		}								
		else if((CurrentVoltage - GoalVoltage) >= 100)
		{//快速调整
			SetHVVoltageDown(15);
		}
		else
		{
			SetHVVoltageDown(1);					
		}
	}
}

/**
  *************************************************************************************
  * @brief 获取低压时DAC的最大允许输出值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   
  *************************************************************************************  
  */
u16 getDacLvValueMin(void)
{
	return DacLvValueMin;
}
/**
  *************************************************************************************
  * @brief  设置低压时DAC的最大允许输出值
  * @param  最大允许输出值
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   
  *************************************************************************************  
  */
void setDacLvValueMin(u16 DacValue)
{
	if(DacValue > 0xFFF)
	{
		DacLvValueMin = 0xFFF;
	}
	else
	{
		DacLvValueMin = DacValue;
	}	
}

/**
  *************************************************************************************
  * @brief  设置高压时DAC的最大允许输出值
  * @param  最大电压
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   硬件最高是9.7V, DAC是实测出来的，相同的DA值，设备的差异应该不超过0.1V
  *************************************************************************************  
  */
void setDacLvValueMinFromVoltage(u16 VoltageMax)
{
	if(VoltageMax >= 9000)
	{
		DacLvValueMin = 328;
	}
	else if(VoltageMax >= 8900)
	{
		DacLvValueMin = 338;
	}	
	else if(VoltageMax >= 8800)
	{
		DacLvValueMin = 448;
	}
	else if(VoltageMax >= 8700)
	{
		DacLvValueMin = 478;
	}
	else if(VoltageMax >= 8600)
	{
		DacLvValueMin = 538;
	}	
	else if(VoltageMax >= 8500)
	{
		DacLvValueMin = 598;
	}
	else if(VoltageMax >= 8400)
	{
		DacLvValueMin = 628;
	}	
	else if(VoltageMax >= 8300)
	{
		DacLvValueMin = 688;
	}
	else if(VoltageMax >= 8200)
	{
		DacLvValueMin = 748;
	}	
	else if(VoltageMax >= 8100)
	{
		DacLvValueMin = 808;
	}	
	else if(VoltageMax >= 8000)
	{
		DacLvValueMin = 838;
	}
	else if(VoltageMax >= 7900)
	{
		DacLvValueMin = 898;
	}
	else if(VoltageMax >= 7800)
	{
		DacLvValueMin = 928;
	}
	else if(VoltageMax >= 7700)
	{
		DacLvValueMin = 988;
	}	
	else if(VoltageMax >= 7600)
	{
		DacLvValueMin = 1048;
	}
	else if(VoltageMax >= 7500)
	{
		DacLvValueMin = 1108;
	}
	else if(VoltageMax >= 7400)
	{
		DacLvValueMin = 1138;
	}
	else if(VoltageMax >= 7300)
	{
		DacLvValueMin = 1198;
	}	
	else if(VoltageMax >= 7200)
	{
		DacLvValueMin = 1258;
	}
	else if(VoltageMax >= 7100)
	{
		DacLvValueMin = 1290;
	}		
	else
	{
		DacLvValueMin = 1348;//7V
	}	
}

/**
  *************************************************************************************
  * @brief 获取高压时DAC的最大允许输出值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   
  *************************************************************************************  
  */
u16 getDacHvValueMin(void)
{
	return DacHvValueMin;
}
/**
  *************************************************************************************
  * @brief  设置高压时DAC的最大允许输出值
  * @param  最大输出值
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   
  *************************************************************************************  
  */
void setDacHvValueMin(u16 DacValue)
{
	if(DacValue > 0xFFF)
	{
		DacHvValueMin = 0xFFF;
	}
	else
	{
		DacHvValueMin = DacValue;
	}	
}

/**
  *************************************************************************************
  * @brief  设置高压时DAC的最大允许输出值
  * @param  最大电压
  * @retval 无
  * @author ZH
  * @date   2018年1月2日
  * @note   硬件保护是12.5V, DAD是实测出来的，相同的DA值，设备的差异应该不超过0.1V
  *************************************************************************************  
  */
void setDacHvValueMinFromVoltage(u16 VoltageMax)
{
	if(VoltageMax >= 13000)
	{
		DacHvValueMin = 1102;
	}
	else if(VoltageMax >= 12900)
	{
		DacHvValueMin = 1131;
	}	
	else if(VoltageMax >= 12800)
	{
		DacHvValueMin = 1151;
	}
	else if(VoltageMax >= 12700)
	{
		DacHvValueMin = 1179;
	}
	else if(VoltageMax >= 12600)
	{
		DacHvValueMin = 1208;
	}	
	else if(VoltageMax >= 12500)
	{
		DacHvValueMin = 1230;
	}
	else if(VoltageMax >= 12400)
	{
		DacHvValueMin = 1250;
	}	
	else if(VoltageMax >= 12300)
	{
		DacHvValueMin = 1273;
	}
	else if(VoltageMax >= 12200)
	{
		DacHvValueMin = 1304;
	}	
	else if(VoltageMax >= 12100)
	{
		DacHvValueMin = 1330;
	}	
	else if(VoltageMax >= 12000)
	{
		DacHvValueMin = 1353;
	}
	else if(VoltageMax >= 11900)
	{
		DacHvValueMin = 1385;
	}
	else if(VoltageMax >= 11800)
	{
		DacHvValueMin = 1403;
	}
	else if(VoltageMax >= 11700)
	{
		DacHvValueMin = 1429;
	}	
	else if(VoltageMax >= 11600)
	{
		DacHvValueMin = 1460;
	}
	else if(VoltageMax >= 11500)
	{
		DacHvValueMin = 1490;
	}
	else if(VoltageMax >= 11400)
	{
		DacHvValueMin = 1515;
	}
	else if(VoltageMax >= 11300)
	{
		DacHvValueMin = 1545;
	}	
	else if(VoltageMax >= 11200)
	{
		DacHvValueMin = 1570;
	}
	else if(VoltageMax >= 11100)
	{
		DacHvValueMin = 1627;
	}		
	else
	{
		DacHvValueMin = 1620;//11V
	}	
}
void VoltageOutputAdjust(void)
{	
	if(ERROR != BusStatusIsIdle())
	{//不处于雷管通信状态			
		/**********************调整端口电压*********************/
		if((0 == Op_Data.AdjustStatus) && BusCurrentInRange(Op_Data.LinABCurrent) && (OSTimeGet() > Op_Data.LinAdjustTime))
		{
			if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
			{//当前总线电压已经关闭
				DacOutLVVoltage(LIN_NORMAL_LOW_VOLTAGE_L_GOAL);
				DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
			}
			else
			{
				if(GET_HV_SW_CTRL_STATE() == 0)
				{//当前为端口电压为低压状态
					LVVoltageOutputAdjust(Op_Data.LinABVoltage, Op_Data.LinNormalLowVoltageGoal);													
					
					if(0 != GET_HV_ENABLE_SW_STATE())
					{//高压开关已经使能						
						if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2000)
						{//已经按下高压键
							if((0 != GET_11_3V_CTRL_STATE()))
							{//升压电路已经使能
								HVVoltageOutputAdjust(GetChannelxVoltage(ADC_CH_11_3V, 10), Op_Data.LinNormalHighVoltageGoal);
								if(Op_Data.DacHvValueMin > getCurrentHvDacValue())
								{
									Op_Data.DacHvValueMin = getCurrentHvDacValue();
								}								
							}							
						}					
					}
				}
				else
				{//当前端口为高压状态
					HVVoltageOutputAdjust(Op_Data.LinABVoltage, Op_Data.LinNormalHighVoltageGoal);	
					if(Op_Data.DacHvValueMin > getCurrentHvDacValue())
					{
						Op_Data.DacHvValueMin = getCurrentHvDacValue();
					}
				}
			}			
		}			
	}
}
