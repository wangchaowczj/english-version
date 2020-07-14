/**
  *************************************************************************************
  * @file    SystemConfig.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SystemConfigģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"


/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SystemConfig
  *@brief ϵͳ����
  *@{
  */
/**@defgroup SystemConfig_Functions
  *@brief 
  *@{
  */
/**
  *************************************************************************************
  * @brief  �����ж˿�ʱ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��08��
  * @note   ��
  *************************************************************************************  
  */
void OpenAllPortClock(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
		RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
}  
/**
  *************************************************************************************
  * @brief  LED��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��4��10��
  * @note   ��
  *************************************************************************************  
  */
void LedPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***���������״̬�л��ܽ�**/
	GPIO_InitStructure.GPIO_Pin = LED1_PIN; 
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED2_PIN; 
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);
	
	CLOSE_LED1();
	CLOSE_LED2();
}

/**
  *************************************************************************************
  * @brief  FactoryPinConfig
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��4��10��
  * @note   
  *************************************************************************************  
  */
void FactoryPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(MCU_IO_IN1_PORT_RCC | IO_RELAY1_PORT_RCC | MCU_IO_IN2_PORT_RCC | IO_RELAY2_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

    CLOSE_IO_RELAY1();
    CLOSE_IO_RELAY2();
    
	GPIO_InitStructure.GPIO_Pin = IO_RELAY1_PIN; 
	GPIO_Init(IO_RELAY1_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = IO_RELAY2_PIN; 
	GPIO_Init(IO_RELAY2_PORT, &GPIO_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, GPIO_PinSource1);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, GPIO_PinSource2);
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
	GPIO_InitStructure.GPIO_Pin = MCU_IO_IN1_PIN;
	GPIO_Init(MCU_IO_IN1_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = MCU_IO_IN2_PIN;
	GPIO_Init(MCU_IO_IN2_PORT, &GPIO_InitStructure);	

	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	
	EXTI_InitStruct.EXTI_Line = MCU_IO_IN1_EXTI_LINE;
	EXTI_Init(&EXTI_InitStruct);
    
	EXTI_InitStruct.EXTI_Line = MCU_IO_IN2_EXTI_LINE;
	EXTI_Init(&EXTI_InitStruct);
    
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);	
    
    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    

}
 
/**
  *************************************************************************************
  * @brief  ϵͳ����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void SystemGpioConfig(void)
{
	//�δ�ʱ������	
	SysTickConfig();//1ms�ж�һ��	
	OpenAllPortClock();
	LedPinConfig();	
	SystemSleepModeConfig();	
	SoundConfig();
    RS485Config();
    RS232Config();
    FactoryPinConfig();
	
	PowerConfig();	
	DetHardwareInit();	
	AdcConfig();	

	Ch374HardWareConfig();
	LCD_FSMC_Config();
	LcdRA8875Init();
	
	ReadFactoryParam();
	UserParam.LcdBright = 1; 
    if(GET_AC_IN_STATE() != NOT_AC)
    {
        SetSystemSleepDisable();//220V����ʱ����������
    }
}
/**
  *************************************************************************************
  * @brief  Ӳ����ʼ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */

u8 SystemHardwareInit(void)
{	
	static FATFS SdCardFatfs;
	FATFS *fs;
	FRESULT res;
	DWORD fre_clust;
	DIR dir;

	OPEN_LED1();
	OPEN_LED2();
	Channel.Count = DET_CHANNEL_MAX;
	PackageKeyInit();
	DeviceInfo.name = 'A';

	Sound(1);
	OSTimeDlyHMSM(0,0,0,100);
	CLOSE_LED1();
	CLOSE_LED2();
	
	if(ERROR == RTC_Config())
	{
		return 1;
	}	
	//TF������
	if(ERROR == SdCardPowerOn())
	{
		OSTimeDlyHMSM(0,0,0,100);
		if(ERROR == SdCardPowerOn())
		{
			return 2;		
		}
	}

	//����sd���ļ�ϵͳ
	if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS�ļ�ϵͳ
	{
		return 3;//�ļ�ϵͳ�쳣
	}

	res = f_getfree("0:", &fre_clust, &fs);//��������((fs->n_fatent - 2) * fs->csize)/2;ʣ��������(fre_clust * fs->csize)/2;
	if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//û�и�ʽ����������С��5M
	{
		return 3;//�ļ�ϵͳ�쳣
	}
	/***ȷ���ļ�ϵͳȷʵ�Ѿ���ʽ��,��д�����ļ�ʱ��������ͼ�������ļ���***/
	res = f_opendir(&dir, CONFIG_PARAM_FILE_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{//����Ŀ¼ʧ�������¸�ʽ��
			return 3;//�ļ�ϵͳ�쳣
		}
	}
	
	if(ERROR == Ch374HardwareState())
	{
        OSTimeDlyHMSM(0,0,0,100);
        if(ERROR == Ch374HardwareState())
        {
            return 4;//USBоƬ����������        
        }

	}

	
	DetTestRecordFileInit();
    DetDataBindingRecordFileInit();
	ReadUserConfigParam();	
    DetTestParamInit();
//	ReadDetTestQueueInfo();
	ClearDetInfo();
	LcdSetBright(UserParam.LcdBright);
	return 0;
}
/**
 *@} SystemConfig_Functions
 */
/**
 *@} SystemConfig
 */
/**
 *@} PowerBoard
 */

