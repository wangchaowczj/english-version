/**
  *************************************************************************************
  * @file    SystemConfig.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SystemConfig模块源文件
  *************************************************************************************
  */
#include "includes.h"


/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SystemConfig
  *@brief 系统配置
  *@{
  */
/**@defgroup SystemConfig_Functions
  *@brief 
  *@{
  */
/**
  *************************************************************************************
  * @brief  打开所有端口时钟
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月08日
  * @note   无
  *************************************************************************************  
  */
void OpenAllPortClock(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
		RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
}  
/**
  *************************************************************************************
  * @brief  LED引脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年4月10日
  * @note   无
  *************************************************************************************  
  */
void LedPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***发送与接收状态切换管脚**/
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
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年4月10日
  * @note   
  *************************************************************************************  
  */
void FactoryPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/***时钟配置***/
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
  * @brief  系统配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
void SystemGpioConfig(void)
{
	//滴答定时器配置	
	SysTickConfig();//1ms中断一次	
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
        SetSystemSleepDisable();//220V供电时，不用休眠
    }
}
/**
  *************************************************************************************
  * @brief  硬件初始化
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
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
	//TF卡配置
	if(ERROR == SdCardPowerOn())
	{
		OSTimeDlyHMSM(0,0,0,100);
		if(ERROR == SdCardPowerOn())
		{
			return 2;		
		}
	}

	//加载sd卡文件系统
	if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS文件系统
	{
		return 3;//文件系统异常
	}

	res = f_getfree("0:", &fre_clust, &fs);//总容量：((fs->n_fatent - 2) * fs->csize)/2;剩余容量：(fre_clust * fs->csize)/2;
	if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//没有格式化或者容量小于5M
	{
		return 3;//文件系统异常
	}
	/***确认文件系统确实已经格式化,在写配置文件时将不在试图创建该文件夹***/
	res = f_opendir(&dir, CONFIG_PARAM_FILE_PATH);
	if (res != FR_OK)
	{//创建目录
		res = f_mkdir(CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{//创建目录失败则重新格式化
			return 3;//文件系统异常
		}
	}
	
	if(ERROR == Ch374HardwareState())
	{
        OSTimeDlyHMSM(0,0,0,100);
        if(ERROR == Ch374HardwareState())
        {
            return 4;//USB芯片连接有问题        
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

