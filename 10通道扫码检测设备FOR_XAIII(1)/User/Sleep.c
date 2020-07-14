#include "includes.h"

u32 SystemSleepCount = 0;
u8 SystemSleepEnableStatus = 1;
/**
  *************************************************************************************
  * @brief  睡眠模式配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月01日
  * @note   无
  *************************************************************************************  
  */
void SystemSleepModeConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(SLEEP_WAKE_PORT_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
	GPIO_InitStruct.GPIO_Pin = SLEEP_WAKE_PIN;
	GPIO_Init(SLEEP_WAKE_PORT, &GPIO_InitStruct);	
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);
	
	/*外部下降沿中断*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//外部下降沿中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}
/**
  *************************************************************************************
  * @brief  获取系统睡眠滴答定时器计数值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
u32 GetSystemSleepCount(void)
{
	return SystemSleepCount;
}
/**
  *************************************************************************************
  * @brief  设置新的系统睡眠滴答定时器计数值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
void SetSystemSleepCount(u32 NewCount)
{
	SystemSleepCount = NewCount;
}

/**
  *************************************************************************************
  * @brief  判断系统睡眠滴答定时器计数值是否超出设置值
  * @param  CurrentCount 当前系统滴答定时器计数值
  * @retval 0 还没有超时， 1已经休眠超时
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
u8 SystemSleepCountIsTimeout(u32 CurrentCount)
{	
	if(CurrentCount > (SystemSleepCount+SYSTEM_ENTER_SLEEP_MODE_COUNT))
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  禁止进入睡眠状态
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
void SetSystemSleepDisable(void)
{
	SystemSleepEnableStatus = 0;
}
/**
  *************************************************************************************
  * @brief  允许进入睡眠状态
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
void SetSystemSleepEnable(void)
{
	SystemSleepEnableStatus = 1;
}
/**
  *************************************************************************************
  * @brief  获取睡眠状态开关
  * @param  无
  * @retval 0 禁止，1允许
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
u8 SystemSleepIsEnable(void)
{
	return SystemSleepEnableStatus;
}

/**
  *************************************************************************************
  * @brief  进入睡眠状态
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月22日
  * @note   无
  *************************************************************************************  
  */
void SystemEnterSleepMode(void)
{	
	u8 HW_Status;
	
	/*进入睡眼前关闭相关外设*/
	LcdSetBright(0);//内部调用了OSSchedLock
	
	OSSchedLock();
	
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk; //进入睡眠前关闭滴答定时器
	SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));	
	
	CLOSE_LED1();
	OPEN_LED2();
		
	CLOSE_LCD_POWER();	
	
	HW_Status = GET_HV_SW_STATUS();
	CLOSE_HV_SW();
	
	CLOSE_PW_USB();
	CLOSE_PW_5V();
	CLOSE_PW_LV();
	CLOSE_PW_HV();		
	
	/*关掉不必要的端口时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, DISABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, DISABLE);
	
	RTC_Set_WakeUp(SYSTEM_ENTER_POWEROFF_MODE_COUNT);//唤醒时间
	
	/*-------进入睡眠--------*/
	__WFI();//执行后，马上进入睡眠, 仅MCU进入睡眠，可减小电流45mA(供电电压4V)左右	
	/*-------退出睡眠--------*/
	
//	OSTimeDlyHMSM(0,0,0,100);
	RTC_WakeUpCmd(DISABLE);
	SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk; //退出睡眠后马上打开滴答定时器
	
	OpenAllPortClock();
	
	CLOSE_LED1();
	CLOSE_LED2();
		
	OPEN_PW_USB();
	OPEN_PW_HV();
	OPEN_PW_LV();
	OPEN_PW_5V();
	
	if(HW_Status)
	{
		OPEN_HV_SW();
	}
	/*恢复睡眠前的外设状态*/
	LcdRA8875Init();
		
	OSSchedUnlock();
	
	LcdSetBright(UserParam.LcdBright);
}

