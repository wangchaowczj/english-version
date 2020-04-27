#include "includes.h"

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
		PW_KILL_PORT_RCC | CHARGE_STAT_PORT_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/***���״̬***/
	GPIO_InitStructure.GPIO_Pin = CHARGE_STAT_PIN;	
	GPIO_Init(CHARGE_STAT_PORT, &GPIO_InitStructure);	

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

	/***���豸��Դ***/
	POWER_ON();  
	
	/***��Դ����***/
	GPIO_InitStructure.GPIO_Pin = PW_KILL_PIN;	
	GPIO_Init(PW_KILL_PORT, &GPIO_InitStructure);	
	
	/***���豸��Դ***/
	POWER_ON();    
	

	/***PG.9Ϊ�ػ��жϣ�����Ϊ�½��ض��������ⲿ�ж�ģʽ***/
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½�����Ч 
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource9);
	EXTI_ClearITPendingBit(EXTI_Line9);	
	
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
	if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
	{//���ڳ��
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
  *      @arg 1 ������
  *      @arg 2 û�в�������
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
u8 CheckChargeState(void)
{ 
	if(GET_CHARGE_IN_STATE() == USB_NOT_CHARGE)
	{	
		return 2;//û�в�������
	}
    //���״̬����һֱΪ��ʱ��ʾ���ڳ�磬һֱΪ�߱�ʾ�Ѿ�����
    if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
    {
        return 0;//���ڳ��
    }
	return 1;//������
}

