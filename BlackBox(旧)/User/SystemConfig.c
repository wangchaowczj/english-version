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
  * @brief  LED测试引脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
void LedPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(LED1_GREEN_PORT_RCC | LED1_RED_PORT_RCC | LED2_GREEN_PORT_RCC | LED2_RED_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    
	GPIO_InitStructure.GPIO_Pin = LED1_GREEN_PIN; 
	GPIO_Init(LED1_GREEN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_RED_PIN; 
	GPIO_Init(LED1_RED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED2_GREEN_PIN; 
	GPIO_Init(LED2_GREEN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED2_RED_PIN; 
	GPIO_Init(LED2_RED_PORT, &GPIO_InitStructure);
    
	CLOSE_LED1_GREEN();
	CLOSE_LED1_RED();
	CLOSE_LED2_GREEN();
	CLOSE_LED2_RED();
}

ErrorStatus FileSystemFormatAt45(void)
{
    if(FR_OK == f_mkfs("1:", 0, 512))
    {
        return SUCCESS;
    }
    return ERROR;
}
ErrorStatus FileSystemFormatSdCard(void)
{
    if(FR_OK == f_mkfs("0:", 0, 512))
    {
        return SUCCESS;
    }
    return ERROR;
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
//	USB_AutoCheck_Config();
	SysTickConfig();//1ms中断一次
    DetPinConfig();
    DetTimerConfig();
    DetRecStatusInit();
//	PowerConfig();			//电源模块配置
	LedPinConfig();
	ADCConfig();
    DMAConfig();
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
//	DIR dir;
//		
//	POWER_ON();
//	OPEN_5V_SW();
//	
//	SoundOutput(0);
//	OSTimeDlyHMSM(0,0,0,100);	
//	
	if(ERROR == RTC_Config())
	{
		return 1;
	}
	
	//TF卡配置
	if(ERROR == SdCardPowerOn())
	{
		OSTimeDlyHMSM(0,0,0,200);
		if(ERROR == SdCardPowerOn())
		{
			return 3;		
		}
	}

	//加载sd卡文件系统
	if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS文件系统
	{	
		if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS文件系统
		{
			return 4;//文件系统异常		
		}
	}

	res = f_getfree("0:", &fre_clust, &fs);//总容量：((fs->n_fatent - 2) * fs->csize)/2;剩余容量：(fre_clust * fs->csize)/2;
	if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//没有格式化或者容量小于5M
	{
		return 4;//文件系统异常
	}
//	/***确认文件系统确实已经格式化***/
//	res = f_opendir(&dir, SYSTEM_OPERATE_INFO_PATH);
//	if (res != FR_OK)
//	{//创建目录
//		res = f_mkdir(SYSTEM_OPERATE_INFO_PATH);
//		if (res != FR_OK)
//		{//创建目录失败则重新格式化
//			return 4;//文件系统异常
//		}
//	}
//	
//	if(LTC2942Init() == ERROR)
//	{
//		return 5;//电源管理芯片异常
//	}
//    if(FileSystemTest(0) == ERROR)
//    {
//        return 6;
//    }
//    if(FileSystemTest(1) == ERROR)
//    {
//        return 7;
//    }
//	ReadFactoryConfigParam();	
//	ReadNetParam();//读孔网参数
//	ReadUserConfigParam();



//	PackageKeyInit();	
//	WriteSystemLog(NULL,0,DEV_START);//记录开机信息


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

