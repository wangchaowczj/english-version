/**
  *************************************************************************************
  * @file    Uart.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ����ģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"


/**
  *************************************************************************************
  * @brief  ����2���ú�����������9600��8λ����λ��1λֹͣλ������У��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ����2����԰�ͨ��
  *************************************************************************************  
  */ 
void UartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RS485_UART_DIR_PORT_RCC | UART_TX_PORT_RCC| UART_RX_PORT_RCC  
		|RS4852_UART_DIR_PORT_RCC | UART2_TX_PORT_RCC| UART2_RX_PORT_RCC  
		| RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//����RS485 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = RS485_UART_DIR_PIN;
	GPIO_Init(RS485_UART_DIR_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = RS485_UART_SW_PIN;
	GPIO_Init(RS485_UART_SW_PORT, &GPIO_InitStruct);
	
	SET_RS485_UART_R();
    SET_RS485_SW_L();

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = RS485_UART_S_PIN;
	GPIO_Init(RS485_UART_S_PORT, &GPIO_InitStruct);
	

	GPIO_InitStruct.GPIO_Pin = UART_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART_RX_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = UART_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART_TX_PORT, &GPIO_InitStruct); 

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStructure);
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 	
	
	
	//�ӻ�RS4852 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = RS4852_UART_DIR_PIN;
	GPIO_Init(RS4852_UART_DIR_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = RS4852_UART_SW_PIN;
	GPIO_Init(RS4852_UART_SW_PORT, &GPIO_InitStruct);
	
	SET_RS4852_UART_R();
    SET_RS4852_SW_H();

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = RS4852_UART_S_PIN;
	GPIO_Init(RS4852_UART_S_PORT, &GPIO_InitStruct);
	

	GPIO_InitStruct.GPIO_Pin = UART2_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART2_RX_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = UART2_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART2_TX_PORT, &GPIO_InitStruct);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART2, ENABLE);
	//�����ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 	
}

/**
  *************************************************************************************
  * @brief  ����2���ݷ��ͺ���
  * @param[in]  dat��Ҫ���͵������׵�ַ
  * @param[in]  len��Ҫ���͵����ݳ���
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ����2����԰�ͨ�ţ����ж�ģʽ
  *************************************************************************************  
  */ 
void UartSendData(u8* dat,u16 len)
{
    u16 i = 0;

    SET_RS485_UART_T();

	OSTimeDlyHMSM(0,0,0,2);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART1, *(dat + i));
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
    }
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 	

    SET_RS485_UART_R();
}
/**
  *@} Uart_Functions
  */
void Uart2SendData(u8* dat,u16 len)
{
    u16 i = 0;

    SET_RS4852_UART_T();

	OSTimeDlyHMSM(0,0,0,2);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART2, *(dat + i));
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
    }
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 	

    SET_RS4852_UART_R();
}


