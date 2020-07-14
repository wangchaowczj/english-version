#include "includes.h"

ErrorStatus GetChargeHardwareState(void)
{
	u8 RegValue = 0x00;
	
	if(Charge_I2C_ReadData(CHARGE_DEVICE_ADDR,0x01, &RegValue, 1))
	{
		return SUCCESS;
	}
	return ERROR;
}
void ChargerCurrent512mA(void)
{
	u8 RegValue = 0x00;
	
	//充电电流为512mA
	RegValue = 0x00;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x02, &RegValue, 1);
}
void ChargerCurrent2A(void)
{
	u8 RegValue = 0x00;

	//充电电流为2A
	RegValue = 0x60;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x02, &RegValue, 1);
}

void ChargerRegInit(void)
{
	u8 RegValue = 0x00;
	
	//打开充电便能
	RegValue = 0x9B;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x01, &RegValue, 1);
	RegValue = 0x5F;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x01, &RegValue, 1);
}


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
		PW_KILL_PORT_RCC | PW_STATE_PORT_RCC|
		CHARGE_PG_PORT_RCC | CHARGE_PSEL_PORT_RCC | CHARGE_STATUS_PORT_RCC | 
		CHARGE_INT_PORT_RCC | CHARGE_OTG_PORT_RCC | CHARGE_CE_PORT_RCC | USB_SW_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

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
	
	/***电源控制***/
	GPIO_InitStructure.GPIO_Pin = PW_KILL_PIN;	
	GPIO_Init(PW_KILL_PORT, &GPIO_InitStructure);

	/***充电控制引脚***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_PG_PIN;	
	GPIO_Init(CHARGE_PG_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_STATUS_PIN;	
	GPIO_Init(CHARGE_STATUS_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_INT_PIN;	
	GPIO_Init(CHARGE_INT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_PSEL_PIN;	
	GPIO_Init(CHARGE_PSEL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_OTG_PIN;	
	GPIO_Init(CHARGE_OTG_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CHARGE_CE_PIN;	
	GPIO_Init(CHARGE_CE_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	
	GPIO_InitStructure.GPIO_Pin = USB_SW_PIN;	
	GPIO_Init(USB_SW_PORT, &GPIO_InitStructure);
	
	/***打开设备电源***/
	POWER_ON();    
	
	/***先禁止，再打开，有一个切换过程***/
	CHARGE_OFF();

	/***PB.5为关机中断，配置为下降沿都触发的外部中断模式***/
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿有效 
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	EXTI_ClearITPendingBit(EXTI_Line5);	
	
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
	//if(GET_USB_ID() == USB_CONNET_GUEST)
	if(GET_CHARGE_IN_STATE() != USB_NOT_CHARGE)
	{//已插入充电器
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
  *      @arg 1 没有开始充电 
  *      @arg 2 充电完成   
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
u8 CheckChargeState(void)
{	
	if((GET_CHARGE_IN_STATE() == USB_NOT_CHARGE) || (OpData.AdjustStatus != 0))
	{
		
		CHARGE_OFF();
		return 1;//没有开始充电
	}
	CHARGE_ON();		
	if(GET_CHARGE_STATUS_PIN())
	{
		ChargerRegInit();
	}
	System168MDelay100us(1);
	if(GET_CHARGE_STATUS_PIN())
	{			
		return 2;//充电完成		
	}
	return 0;//正在充电			
}

/**
  *************************************************************************************
  * @brief  检测充电状态
  * @param  无
  * @retval 充电状态 
  *      @arg 0 正在充电
  *      @arg 1 没有开始充电 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
u8 CheckChargeStatusFromStatusPin(void)
{
	u8 LowFlag = 0;
	u8 HighFlag = 0;
	u32 TimeOut;
	
	TimeOut = OSTimeGet() + 50;
	while(OSTimeGet() < TimeOut)
	{
		if(GET_CHARGE_STATUS_PIN())
		{
			HighFlag = 1;
		}
		else
		{
			LowFlag = 1;
		}
		if((HighFlag == 1) && (LowFlag == 1))
		{
			break;
		}
	}
	if((HighFlag == 0) && (LowFlag == 1))
	{
		return 0;//正在充电
	}
	if((HighFlag == 1) && (LowFlag == 0))
	{
		return 1;//充电完成或者禁止充电状态
	}
	return 2;//错误状态，可能没有接电池
}

