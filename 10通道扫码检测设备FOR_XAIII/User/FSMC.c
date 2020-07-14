#include "includes.h"


void LCD_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
//	FSMC_NORSRAMTimingInitTypeDef  p;
	FSMC_NORSRAMTimingInitTypeDef  timingWrite;
	FSMC_NORSRAMTimingInitTypeDef  timingRead;
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | LCD_POWER_PORT_RCC, ENABLE);		//D0��D15��IO��ʱ�Ӵ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);								//FSMC��ʱ�Ӵ�

	//****GPIO����****//
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);	//D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);	//D1
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		//D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		//D3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		//D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		//D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		//D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);	//D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);	//D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);	//D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);	//D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);	//D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);	//D12
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		//D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		//D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);	//D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);	//A17(D/C)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		//NOE(RD)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		//NWE(WR)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);		//NE1(CS)


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
								GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(LCD_TOUCH_STATE_PORT_RCC | LCD_STATE_PORT_RCC | 
		LCD_RESET_PORT_RCC | LCD_DIMM_PORT_RCC, ENABLE);	//���ƿڵ�IOʱ�Ӵ�


	//****�����������****//
	GPIO_InitStructure.GPIO_Pin = LCD_DIMM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_DIMM_PORT, &GPIO_InitStructure);
	
	//****PD11��IO_LCDC_RST		LCD����ģ�鸴λ����****//
	GPIO_InitStructure.GPIO_Pin = LCD_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_RESET_PORT, &GPIO_InitStructure);
	
	//****PE2��LCDC_TE		LCDˢ������ж�����****//
	GPIO_InitStructure.GPIO_Pin = LCD_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_STATE_PORT, &GPIO_InitStructure);
	
	//ͨ����ѯ��ʽȷ�ϴ���״̬
	GPIO_InitStructure.GPIO_Pin = LCD_TOUCH_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_TOUCH_STATE_PORT, &GPIO_InitStructure);
	
	//LCD��Դ����
	GPIO_InitStructure.GPIO_Pin = LCD_POWER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_POWER_PORT, &GPIO_InitStructure);
	
	//****FSMC����****//	
//	p.FSMC_AddressSetupTime = 5;			//��ַ����ʱ��
//	p.FSMC_AddressHoldTime = 0;				//��ַ����ʱ��
//	p.FSMC_DataSetupTime = 9;				//���ݽ���ʱ��
//	p.FSMC_BusTurnAroundDuration = 5;		//���߻ָ�ʱ��
//	p.FSMC_CLKDivision = 0;					//ʱ�ӷ�Ƶ
//	p.FSMC_DataLatency = 0;					//���ݱ���ʱ��
//	p.FSMC_AccessMode = FSMC_AccessMode_A;	//ģʽA
	timingWrite.FSMC_AddressSetupTime = 4;
	timingWrite.FSMC_AddressHoldTime = 0;
	timingWrite.FSMC_DataSetupTime = 6;
	timingWrite.FSMC_BusTurnAroundDuration = 1;
	timingWrite.FSMC_CLKDivision = 0;
	timingWrite.FSMC_DataLatency = 0;
	timingWrite.FSMC_AccessMode = FSMC_AccessMode_A;

	timingRead.FSMC_AddressSetupTime = 4;
	timingRead.FSMC_AddressHoldTime = 0;
	timingRead.FSMC_DataSetupTime = 8;
	timingRead.FSMC_BusTurnAroundDuration = 1;
	timingRead.FSMC_CLKDivision = 0;
	timingRead.FSMC_DataLatency = 0;
	timingRead.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;							//NOR FLASH��BANK1
	//MUXENλ	�������˸�λ�󣬵�ַ�ĵ�16λ�����ݽ������������ߣ���λ����NOR��PSRM�洢����Ч��
	//0����ַ/���ݲ����á�		1����ַ/���ݸ����������ߣ����Ǹ�λ���Ĭ��״̬��
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;		//���������ַ�߲�����
	//MTYPλ	�����ⲿ�洢��������
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;					//�洢�����ͣ�ԭ��ΪFSMC_MemoryType_SRAM
	//MWIDλ	�����ⲿ�洢�����ߵĿ�ȣ��������������͵Ĵ洢����
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;			//���ݿ��Ϊ16λ
	//BURSTENλ	���������洢�����г���ģʽ���ʣ���λ��������洢����ͬ������ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;		//ʹ���첽дģʽ����ֹͻ��ģʽ
	//ASYNCWAITλ
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	//WAITPOL	���ô洢�������ĵȴ��źŵļ��ԣ���λ���ڴ洢���ĳ���ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	//����Ա������ֻ��ͻ��ģʽ����Ч���ȴ��źż���Ϊ��
	//WRAPMOD	��λ�����������Ƿ�֧�ְѷǶ����AHB��������ָ��2�����Բ�������λ���ڴ洢���ĳ���ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;					//��ֹ�Ƕ���ͻ��ģʽ
	//WAITCFG	������洢�����ڳ��鴫��ģʽʱ��NWAIT�ź�ָʾ������洢�������������Ƿ���Ч���Ƿ���Ҫ����ȴ����ڡ�
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;	//����Ա���ý���ͻ��ģʽ����Ч��NWAIT�ź���ʲôʱ�ڲ���
	//WREN		��λָʾFSMC�Ƿ�����/��ֹ�Դ洢����д������
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			//дʹ��
	//WAITEN	������洢�����ڳ��鴫��ģʽʱ����һλ����/��ֹͨ��NWAIT�źŲ���ȴ�״̬��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;				//����Ա������ֻ��ͻ��ģʽ����Ч������NWAIT�ź�
	//EXTMOD	��λ����FSMCʹ��FSMC_BWTR�Ĵ��������������дʹ�ò�ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;				//������չģʽ
	//CBURSTRW	����дʹ��λ
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;				//��ֹͻ��д����
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &timingRead;							//���ö�дʱ��
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &timingWrite;								//����дʱ��

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);		//ʹ��FSMC Bank1_SRAM Bank

	LCD_DIMM_PIN_L();//Ĭ�ϰѱ���ص�
	CLOSE_LCD_POWER();
}
