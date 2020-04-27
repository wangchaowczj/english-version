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

	//���ô���ʱ��
	RCC_AHB1PeriphClockCmd(PW_5V_SW_PORT_RCC | LINAB_SHORT_SW_RCC |
		LINAB_5V_CTRL_RCC | LINAB_6_7V_CTRL_RCC | PW_1_2V_EN_PORT_RCC | PW_6_7V_SW_RCC | PW_11_3V_SW_RCC | 
		PW_6_7V_5V_RCC | HV_ENABLE_RCC | PW_HV_SW_RCC, ENABLE);	//GPIOʱ������

	/***�������/��������***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	/***+5V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_5V_SW_PIN;	
	GPIO_Init(PW_5V_SW_PORT, &GPIO_InitStructure);
	
	/***IO_6.7OVER***/
	GPIO_InitStructure.GPIO_Pin = LINAB_5V_CTRL_PIN;	
	GPIO_Init(LINAB_5V_CTRL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LINAB_6_7V_CTRL_PIN;	
	GPIO_Init(LINAB_6_7V_CTRL_PORT, &GPIO_InitStructure);
	
	/***IO_11.3V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_11_3V_SW_PIN;	
	GPIO_Init(PW_11_3V_SW_PORT, &GPIO_InitStructure);

	/***HV_ENABLE***/
	GPIO_InitStructure.GPIO_Pin = HV_ENABLE_PIN;	
	GPIO_Init(HV_ENABLE_PORT, &GPIO_InitStructure);
	
	/***HV_SW***/
	GPIO_InitStructure.GPIO_Pin = PW_HV_SW_PIN;	
	GPIO_Init(PW_HV_SW_PORT, &GPIO_InitStructure);	
	
	/***IO_6.7V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_6_7V_SW_PIN;
	GPIO_Init(PW_6_7V_SW_PORT, &GPIO_InitStructure);

	/***IO_6.7V_SW***/
	GPIO_InitStructure.GPIO_Pin = LINAB_SHORT_SW_PIN;
	GPIO_Init(LINAB_SHORT_SW_PORT, &GPIO_InitStructure);
	
	/***�ڲ�����***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***�˿ڵ�ѹ6.7V��5V�л�***/
	GPIO_InitStructure.GPIO_Pin = PW_6_7V_5V_PIN;
	GPIO_Init(PW_6_7V_5V_PORT, &GPIO_InitStructure);	
		
	GPIO_InitStructure.GPIO_Pin = PW_1_2V_EN_PIN;
	GPIO_Init(PW_1_2V_EN_PORT, &GPIO_InitStructure);


	/***�򿪴�1.2V����Һ������***/
	OPEN_1_2V();
	
	/***���ڲ�5V��ѹ***/
	OPEN_5V_SW();
	
	/***�رո�ѹ���л�����ѹ***/
	CLOSE_BUS_HV();
	
	/***���ߵ�ѹ***/
	//OPEN_BUS_6_7V();
	CLOSE_BUS_VOLTAGE();
}

