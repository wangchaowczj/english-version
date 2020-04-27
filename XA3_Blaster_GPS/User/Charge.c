#include "includes.h"

//==================================================================================================
//| 函数名称 | DeviceChargeConfig 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置电源模块
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DeviceChargeConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIO时钟配置***/
	RCC_AHB1PeriphClockCmd(KEY_PW_PORT_RCC | PW_INT_PORT_RCC | 
		PW_KILL_PORT_RCC | CHARGE_STAT_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/***充电状态***/
	GPIO_InitStructure.GPIO_Pin = CHARGE_STAT_PIN;	
	GPIO_Init(CHARGE_STAT_PORT, &GPIO_InitStructure);	

	/***电源键中断***/	
	GPIO_InitStructure.GPIO_Pin = PW_INT_PIN;	
	GPIO_Init(PW_INT_PORT, &GPIO_InitStructure);
	
	/***电源状态***/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = PW_STATE_PIN;	
	GPIO_Init(PW_STATE_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/***打开设备电源***/
	POWER_ON();  
	
	/***电源控制***/
	GPIO_InitStructure.GPIO_Pin = PW_KILL_PIN;	
	GPIO_Init(PW_KILL_PORT, &GPIO_InitStructure);	
	
	/***打开设备电源***/
	POWER_ON();    
	

	/***PG.9为关机中断，配置为下降沿都触发的外部中断模式***/
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿有效 
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource9);
	EXTI_ClearITPendingBit(EXTI_Line9);	
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
		
	/***关机外部中断***/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}
//==================================================================================================
//| 函数名称 | CheckDeviceChargePowerOnState 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检测是否是在关机状态下充电
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 0  不是 ；1 是
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus CheckDeviceChargePowerOnState(void)
{
	if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
	{//正在充电
		//if(GET_KEY_PW_STATE() != KEY_DOWN)
		{//开关机键按下
			if(GET_PW_STATE() != PW_STATE_ON)
			{//关机状态
				return SUCCESS;//关机充电
			}
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  检测充电状态
  * @param  无
  * @retval 充电状态 
  *      @arg 0 正在充电
  *      @arg 1 充电完成
  *      @arg 2 没有插入充电器
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
u8 CheckChargeState(void)
{ 
	if(GET_CHARGE_IN_STATE() == USB_NOT_CHARGE)
	{	
		return 2;//没有插入充电器
	}
    //充电状态引脚一直为低时表示正在充电，一直为高表示已经充完
    if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
    {
        return 0;//正在充电
    }
	return 1;//充电完成
}

