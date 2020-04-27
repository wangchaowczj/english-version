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
  * @brief  ����������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void MotoPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(MOTO_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	
	GPIO_InitStructure.GPIO_Pin = MOTO_PIN; 
	GPIO_Init(MOTO_PORT, &GPIO_InitStructure);

	CLOSE_MOTO();
}
/**
  *************************************************************************************
  * @brief  LED������������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
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
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***���������״̬�л��ܽ�**/
	GPIO_InitStructure.GPIO_Pin = LED1_PIN; 
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED2_PIN; 
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);

	OPEN_LED1();
	OPEN_LED2();	
}

/**
  *************************************************************************************
  * @brief  �����ź���������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��01��10��
  * @note   ��
  *************************************************************************************  
  */
void TriggerSinglePinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(TRIGGER_SINGLE_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	
	GPIO_InitStructure.GPIO_Pin = TRIGGER_SINGLE_PIN; 
	GPIO_Init(TRIGGER_SINGLE_PORT, &GPIO_InitStructure);
	TRIGGER_SINGLE_PIN_L();	
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
	USB_AutoCheck_Config();
	RS485Config();
	SysTickConfig();//1ms�ж�һ��
	DeviceChargeConfig();
	LCD_FSMC_Config();		//LCD�˿�����
	PowerConfig();			//��Դģ������
    MotoPinConfig();
	LedPinConfig();
	DetHardwareInit();	   //�׹�����
	ADCConfig();	
	GPSConfig();
	SoundConfig();
	BluePinConfig();	
	WifiConfig();
	TriggerSinglePinConfig();
	DacConfig();
	I2C_GPIO_Config();
	ScanGpioPinConfig();
	LcdBrightInitOS();
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
	static FATFS AT45fatfs;
	static FATFS SdCardFatfs;
	FATFS *fs;
	FRESULT res;
	DWORD fre_clust;
	DIR dir;
		
	POWER_ON();
	OPEN_5V_SW();
	
	SoundOutput(0);
	OSTimeDlyHMSM(0,0,0,100);	
	
	if(ERROR == RTC_Config())
	{
		return 1;
	}

	if(ERROR == FlashHardwareInit())	
	{
		return 2;
	}
	//����AT45DB�ļ�ϵͳ
	if(FR_OK != f_mount(&AT45fatfs, "1:", 1))            //FATFS�ļ�ϵͳ
	{
		//�����ļ�ϵͳ
		if(FR_OK != f_mount(&AT45fatfs, "1:", 1))            //FATFS�ļ�ϵͳ
		{
			return 2;//�ļ�ϵͳ�쳣
		}		
	}
	
	/***ȷ���ļ�ϵͳȷʵ�Ѿ���ʽ��***/
	res = f_opendir(&dir, USER_CONFIG_PARAM_FILE_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(USER_CONFIG_PARAM_FILE_PATH);
		if (res != FR_OK)
		{//����Ŀ¼ʧ�������¸�ʽ��
			return 2;//�ļ�ϵͳ�쳣		
		}
	}

	
	//TF������
	if(ERROR == SdCardPowerOn())
	{
		OSTimeDlyHMSM(0,0,0,200);
		if(ERROR == SdCardPowerOn())
		{
			return 3;		
		}
	}

	//����sd���ļ�ϵͳ
	if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS�ļ�ϵͳ
	{	
		if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS�ļ�ϵͳ
		{
			return 4;//�ļ�ϵͳ�쳣		
		}
	}

	res = f_getfree("0:", &fre_clust, &fs);//��������((fs->n_fatent - 2) * fs->csize)/2;ʣ��������(fre_clust * fs->csize)/2;
	if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//û�и�ʽ����������С��5M
	{
		return 4;//�ļ�ϵͳ�쳣
	}
	/***ȷ���ļ�ϵͳȷʵ�Ѿ���ʽ��***/
	res = f_opendir(&dir, SYSTEM_OPERATE_INFO_PATH);
	if (res != FR_OK)
	{//����Ŀ¼
		res = f_mkdir(SYSTEM_OPERATE_INFO_PATH);
		if (res != FR_OK)
		{//����Ŀ¼ʧ�������¸�ʽ��
			return 4;//�ļ�ϵͳ�쳣
		}
	}
	
	if(LTC2942Init() == ERROR)
	{
		return 5;//��Դ����оƬ�쳣
	}
    if(FileSystemTest(0) == ERROR)
    {
        return 6;
    }
    if(FileSystemTest(1) == ERROR)
    {
        return 7;
    }
	ReadFactoryConfigParam();	
	ReadNetParam();//����������
	ReadUserConfigParam();
	LcdBrightRefreshProcess(1);//�����������������ˢ��һ��

    if(CheckHardwareVersion() == 0)
    {
        return 8;
    }
	
	Det.DlyMax = GetDetMaxDelay(Det.Type);
	ReadAllDetInfo(0);//���׹���Ϣ������ֵ
	ValidateDetTypeAndDelay();
	BlastRecordFileInit();//��ʼ���𱬼�¼����,�����ȶ�����ǰ������ģʽ����
	LogRecordFileInit();
	DeleteAllFileSpecifiedPath(SLAVE_DET_INFO_FILE_PATH, NULL);

	PackageKeyInit();	
//	WriteSystemLog(NULL,0,DEV_START);//��¼������Ϣ
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

