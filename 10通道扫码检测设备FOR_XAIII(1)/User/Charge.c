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
	
	//������Ϊ512mA
	RegValue = 0x00;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x02, &RegValue, 1);
}
void ChargerCurrent2A(void)
{
	u8 RegValue = 0x00;

	//������Ϊ2A
	RegValue = 0x60;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x02, &RegValue, 1);
}

void ChargerRegInit(void)
{
	u8 RegValue = 0x00;
	
	//�򿪳�����
	RegValue = 0x9B;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x01, &RegValue, 1);
	RegValue = 0x5F;
	Charge_I2C_WriteData(CHARGE_DEVICE_ADDR,0x01, &RegValue, 1);
}


//==================================================================================================
//| �������� | DeviceChargeConfig 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���õ�Դģ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void DeviceChargeConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIOʱ������***/
	RCC_AHB1PeriphClockCmd(KEY_PW_PORT_RCC | PW_INT_PORT_RCC | 
		PW_KILL_PORT_RCC | PW_STATE_PORT_RCC|
		CHARGE_PG_PORT_RCC | CHARGE_PSEL_PORT_RCC | CHARGE_STATUS_PORT_RCC | 
		CHARGE_INT_PORT_RCC | CHARGE_OTG_PORT_RCC | CHARGE_CE_PORT_RCC | USB_SW_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/***��Դ���ж�***/	
	GPIO_InitStructure.GPIO_Pin = PW_INT_PIN;	
	GPIO_Init(PW_INT_PORT, &GPIO_InitStructure);
	
	/***��Դ״̬***/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = PW_STATE_PIN;	
	GPIO_Init(PW_STATE_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***��Դ����***/
	GPIO_InitStructure.GPIO_Pin = PW_KILL_PIN;	
	GPIO_Init(PW_KILL_PORT, &GPIO_InitStructure);

	/***����������***/
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
	
	/***���豸��Դ***/
	POWER_ON();    
	
	/***�Ƚ�ֹ���ٴ򿪣���һ���л�����***/
	CHARGE_OFF();

	/***PB.5Ϊ�ػ��жϣ�����Ϊ�½��ض��������ⲿ�ж�ģʽ***/
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½�����Ч 
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	EXTI_ClearITPendingBit(EXTI_Line5);	
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
		
	/***�ػ��ⲿ�ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}
//==================================================================================================
//| �������� | CheckDeviceChargePowerOnState 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����Ƿ����ڹػ�״̬�³��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 0  ���� ��1 ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus CheckDeviceChargePowerOnState(void)
{
	//if(GET_USB_ID() == USB_CONNET_GUEST)
	if(GET_CHARGE_IN_STATE() != USB_NOT_CHARGE)
	{//�Ѳ�������
		//if(GET_KEY_PW_STATE() != KEY_DOWN)
		{//���ػ�������
			if(GET_PW_STATE() != PW_STATE_ON)
			{//�ػ�״̬
				return SUCCESS;//�ػ����
			}
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  �����״̬
  * @param  ��
  * @retval ���״̬ 
  *      @arg 0 ���ڳ��
  *      @arg 1 û�п�ʼ��� 
  *      @arg 2 ������   
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
u8 CheckChargeState(void)
{	
	if((GET_CHARGE_IN_STATE() == USB_NOT_CHARGE) || (OpData.AdjustStatus != 0))
	{
		
		CHARGE_OFF();
		return 1;//û�п�ʼ���
	}
	CHARGE_ON();		
	if(GET_CHARGE_STATUS_PIN())
	{
		ChargerRegInit();
	}
	System168MDelay100us(1);
	if(GET_CHARGE_STATUS_PIN())
	{			
		return 2;//������		
	}
	return 0;//���ڳ��			
}

/**
  *************************************************************************************
  * @brief  �����״̬
  * @param  ��
  * @retval ���״̬ 
  *      @arg 0 ���ڳ��
  *      @arg 1 û�п�ʼ��� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
		return 0;//���ڳ��
	}
	if((HighFlag == 1) && (LowFlag == 0))
	{
		return 1;//�����ɻ��߽�ֹ���״̬
	}
	return 2;//����״̬������û�нӵ��
}

