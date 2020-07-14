#include "includes.h"

//==================================================================================================
//| �������� | PowerConfig 
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
void PowerConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	RCC_AHB1PeriphClockCmd(PW_LV_EN_PORT_RCC | PW_5V_EN_PORT_RCC | 
		PW_HV_EN_PORT_RCC | PW_INT_PORT_RCC | PW_KILL_PORT_RCC | PW_STATE_PORT_RCC|PW_USB_PORT_RCC, ENABLE);	//GPIOʱ������
	
	RCC_AHB1PeriphClockCmd(CH_SW1_PORT_RCC | CH_SW2_PORT_RCC | CH_SW3_PORT_RCC | CH_SW4_PORT_RCC | 
		CH_SW5_PORT_RCC | CH_SW6_PORT_RCC | CH_SW7_PORT_RCC | CH_SW8_PORT_RCC | CH_SW9_PORT_RCC | CH_SW10_PORT_RCC, ENABLE);
		
	/***�������/��������***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = PW_HV_EN_PIN;
	GPIO_Init(PW_HV_EN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PW_LV_EN_PIN;
	GPIO_Init(PW_LV_EN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PW_5V_EN_PIN;
	GPIO_Init(PW_5V_EN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PW_USB_PIN;	
	GPIO_Init(PW_USB_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/***��Դ���ж�***/	
	GPIO_InitStructure.GPIO_Pin = PW_INT_PIN;	
	GPIO_Init(PW_INT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***AC��Դ�������***/	
	GPIO_InitStructure.GPIO_Pin = AC_IN_PIN;	
	GPIO_Init(AC_IN_PORT, &GPIO_InitStructure);	
	
	/***��Դ״̬***/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = PW_STATE_PIN;	
	GPIO_Init(PW_STATE_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***��Դ����***/
	POWER_ON();  
	GPIO_InitStructure.GPIO_Pin = PW_KILL_PIN;	
	GPIO_Init(PW_KILL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = CH_SW1_PIN;	
	GPIO_Init(CH_SW1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW2_PIN;	
	GPIO_Init(CH_SW2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW3_PIN;	
	GPIO_Init(CH_SW3_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW4_PIN;	
	GPIO_Init(CH_SW4_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW5_PIN;	
	GPIO_Init(CH_SW5_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW6_PIN;	
	GPIO_Init(CH_SW6_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW7_PIN;	
	GPIO_Init(CH_SW7_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW8_PIN;	
	GPIO_Init(CH_SW8_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW9_PIN;	
	GPIO_Init(CH_SW9_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CH_SW10_PIN;	
	GPIO_Init(CH_SW10_PORT, &GPIO_InitStructure);

	/***PB.0Ϊ�ػ��жϣ�����Ϊ�½��ض��������ⲿ�ж�ģʽ***/
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½�����Ч 
//	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//	EXTI_Init(&EXTI_InitStructure);
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//�ж��г�ͻ
//	EXTI_ClearITPendingBit(EXTI_Line0);	
//	
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//		
//	/***�ػ��ⲿ�ж�***/
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);	
	
	/***���豸��Դ***/
	POWER_ON();   
	
	CLOSE_PW_HV();
	OPEN_PW_LV();
	OPEN_PW_5V();
	OPEN_PW_USB();
	
	ConnectChannel(0);
}

void ConnectChannel(u8 Channel)
{	
	switch(Channel)
	{
		case 0:
			CONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 1:			
			DISCONNECT_CH1();
			CONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 2:		
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			CONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();						
			break;
		case 3:		
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			CONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 4:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			CONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 5:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			CONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 6:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			CONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 7:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			CONNECT_CH8();
			DISCONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 8:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			CONNECT_CH9();
			DISCONNECT_CH10();			
			break;
		case 9:			
			DISCONNECT_CH1();
			DISCONNECT_CH2();
			DISCONNECT_CH3();
			DISCONNECT_CH4();
			DISCONNECT_CH5();
			DISCONNECT_CH6();
			DISCONNECT_CH7();
			DISCONNECT_CH8();
			DISCONNECT_CH9();
			CONNECT_CH10();		
			break;			
	}	
}
