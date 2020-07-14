#include "includes.h"

u32 SystemSleepCount = 0;
u8 SystemSleepEnableStatus = 1;
/**
  *************************************************************************************
  * @brief  ˯��ģʽ����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��01��
  * @note   ��
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
	
	/*�ⲿ�½����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ⲿ�½����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}
/**
  *************************************************************************************
  * @brief  ��ȡϵͳ˯�ߵδ�ʱ������ֵ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
u32 GetSystemSleepCount(void)
{
	return SystemSleepCount;
}
/**
  *************************************************************************************
  * @brief  �����µ�ϵͳ˯�ߵδ�ʱ������ֵ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
void SetSystemSleepCount(u32 NewCount)
{
	SystemSleepCount = NewCount;
}

/**
  *************************************************************************************
  * @brief  �ж�ϵͳ˯�ߵδ�ʱ������ֵ�Ƿ񳬳�����ֵ
  * @param  CurrentCount ��ǰϵͳ�δ�ʱ������ֵ
  * @retval 0 ��û�г�ʱ�� 1�Ѿ����߳�ʱ
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
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
  * @brief  ��ֹ����˯��״̬
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
void SetSystemSleepDisable(void)
{
	SystemSleepEnableStatus = 0;
}
/**
  *************************************************************************************
  * @brief  �������˯��״̬
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
void SetSystemSleepEnable(void)
{
	SystemSleepEnableStatus = 1;
}
/**
  *************************************************************************************
  * @brief  ��ȡ˯��״̬����
  * @param  ��
  * @retval 0 ��ֹ��1����
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
u8 SystemSleepIsEnable(void)
{
	return SystemSleepEnableStatus;
}

/**
  *************************************************************************************
  * @brief  ����˯��״̬
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��22��
  * @note   ��
  *************************************************************************************  
  */
void SystemEnterSleepMode(void)
{	
	u8 HW_Status;
	
	/*����˯��ǰ�ر��������*/
	LcdSetBright(0);//�ڲ�������OSSchedLock
	
	OSSchedLock();
	
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk; //����˯��ǰ�رյδ�ʱ��
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
	
	/*�ص�����Ҫ�Ķ˿�ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, DISABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, DISABLE);
	
	RTC_Set_WakeUp(SYSTEM_ENTER_POWEROFF_MODE_COUNT);//����ʱ��
	
	/*-------����˯��--------*/
	__WFI();//ִ�к����Ͻ���˯��, ��MCU����˯�ߣ��ɼ�С����45mA(�����ѹ4V)����	
	/*-------�˳�˯��--------*/
	
//	OSTimeDlyHMSM(0,0,0,100);
	RTC_WakeUpCmd(DISABLE);
	SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk; //�˳�˯�ߺ����ϴ򿪵δ�ʱ��
	
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
	/*�ָ�˯��ǰ������״̬*/
	LcdRA8875Init();
		
	OSSchedUnlock();
	
	LcdSetBright(UserParam.LcdBright);
}

