#include "includes.h"


/**
  *************************************************************************************
  * @brief  RS485Config
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��9��10��
  * @note   ��
  *************************************************************************************  
  */
void RS485Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIOʱ������***/
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC | RS485_DIR_PORT_RCC, ENABLE);
	
	/***���ô���1ʱ��***/
	RCC_APB2PeriphClockCmd(RS485_COMM_RCC, ENABLE);	

	
	/***485���棬���ͻ��߽���***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485 0���գ�1����***/
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
	GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);	
	
	/***��������ӳ�䣬TX�ź�RX��***/
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource6, RS485_COMM_AF); 
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource7, RS485_COMM_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_TX_PIN;	
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_RX_PIN;
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);

	/***��������***/
	USART_InitStructure.USART_BaudRate = RS485_COMM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	/***���ڳ�ʼ��***/
	USART_Init(RS485_COMM, &USART_InitStructure);
	
	USART_ClearITPendingBit(RS485_COMM, USART_IT_RXNE);
	
	/***������***/
	USART_Cmd(RS485_COMM, ENABLE);
	
	USART_ITConfig(RS485_COMM, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/***���ô����ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
		
	SET_RS485_RECEIVE();//485Ĭ��Ϊ����״̬
}

//==================================================================================================
//| �������� | RS485SendData 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ͨ��RS485��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ��Ҫ���͵����ݻ�����
//|          | bytes �������ݵ��ֽ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void RS485SendData(u8* ptr, u16 bytes)
{
	u16 i;
	
	SET_RS485_SEND();//485����Ϊ����״̬		
	SystemDelay1us();	
	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS485_COMM, *ptr++);
		while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TC) == RESET);	
	SET_RS485_RECEIVE();//����״̬	
}
//==================================================================================================
//| �������� | RS485AsRS232SendData 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ͨ��RS485��Ϊ232ʱ��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ��Ҫ���͵����ݻ�����
//|          | bytes �������ݵ��ֽ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void RS485AsRS232SendData(u8* ptr, u16 bytes)
{
	u16 i;
	
	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS485_COMM, *ptr++);
		while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TC) == RESET);		
}

//==================================================================================================
//| �������� | RS232Config 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����232ģ��
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
void RS232Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIOʱ������***/
	RCC_AHB1PeriphClockCmd(RS232_COMM_PORT_RCC , ENABLE);
	
	/***���ô���1ʱ��***/
	RCC_APB1PeriphClockCmd(RS232_COMM_RCC, ENABLE);	
	
	
	/***��������ӳ�䣬TX�ź�RX��***/
	GPIO_PinAFConfig(RS232_COMM_PORT, GPIO_PinSource10, RS232_COMM_AF); 
	GPIO_PinAFConfig(RS232_COMM_PORT, GPIO_PinSource11, RS232_COMM_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = RS232_COMM_TX_PIN;	
	GPIO_Init(RS232_COMM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = RS232_COMM_RX_PIN;
	GPIO_Init(RS232_COMM_PORT, &GPIO_InitStructure);

	/***��������***/
	USART_InitStructure.USART_BaudRate = RS232_COMM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	/***���ڳ�ʼ��***/
	USART_Init(RS232_COMM, &USART_InitStructure);
	
	USART_ClearITPendingBit(RS232_COMM, USART_IT_RXNE);
	
	/***������***/
	USART_Cmd(RS232_COMM, ENABLE);
	
	USART_ITConfig(RS232_COMM, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/***���ô����ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
}

//==================================================================================================
//| �������� | RS232SendData 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ͨ��RS232��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ��Ҫ���͵����ݻ�����
//|          | bytes �������ݵ��ֽ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void RS232SendData(u8* ptr, u16 bytes)
{
	u16 i;
	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS232_COMM, *ptr++);
		while(USART_GetFlagStatus(RS232_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS232_COMM, USART_FLAG_TC) == RESET);		
}

void SetUartBaudRate(USART_TypeDef* USARTx, u32 baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	
	//��������
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//���ڳ�ʼ��
	USART_Init(USARTx, &USART_InitStructure);	
}



