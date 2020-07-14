#include "includes.h"


/**
  *************************************************************************************
  * @brief  RS485Config
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年9月10日
  * @note   无
  *************************************************************************************  
  */
void RS485Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIO时钟配置***/
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC | RS485_DIR_PORT_RCC, ENABLE);
	
	/***设置串口1时钟***/
	RCC_APB2PeriphClockCmd(RS485_COMM_RCC, ENABLE);	

	
	/***485方面，发送或者接收***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485 0接收；1发送***/
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
	GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);	
	
	/***设置引脚映射，TX脚和RX脚***/
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource6, RS485_COMM_AF); 
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource7, RS485_COMM_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_TX_PIN;	
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);

	//RX引脚配置
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_RX_PIN;
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);

	/***串口配置***/
	USART_InitStructure.USART_BaudRate = RS485_COMM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	/***串口初始化***/
	USART_Init(RS485_COMM, &USART_InitStructure);
	
	USART_ClearITPendingBit(RS485_COMM, USART_IT_RXNE);
	
	/***允许串口***/
	USART_Cmd(RS485_COMM, ENABLE);
	
	USART_ITConfig(RS485_COMM, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/***设置串口中断***/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
		
	SET_RS485_RECEIVE();//485默认为接收状态
}

//==================================================================================================
//| 函数名称 | RS485SendData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过RS485发送数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指向要发送的数据缓冲区
//|          | bytes 发送数据的字节数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void RS485SendData(u8* ptr, u16 bytes)
{
	u16 i;
	
	SET_RS485_SEND();//485设置为发送状态		
	SystemDelay1us();	
	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS485_COMM, *ptr++);
		while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TC) == RESET);	
	SET_RS485_RECEIVE();//接收状态	
}
//==================================================================================================
//| 函数名称 | RS485AsRS232SendData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过RS485做为232时发送数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指向要发送的数据缓冲区
//|          | bytes 发送数据的字节数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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
//| 函数名称 | RS232Config 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置232模块
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void RS232Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIO时钟配置***/
	RCC_AHB1PeriphClockCmd(RS232_COMM_PORT_RCC , ENABLE);
	
	/***设置串口1时钟***/
	RCC_APB1PeriphClockCmd(RS232_COMM_RCC, ENABLE);	
	
	
	/***设置引脚映射，TX脚和RX脚***/
	GPIO_PinAFConfig(RS232_COMM_PORT, GPIO_PinSource10, RS232_COMM_AF); 
	GPIO_PinAFConfig(RS232_COMM_PORT, GPIO_PinSource11, RS232_COMM_AF);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = RS232_COMM_TX_PIN;	
	GPIO_Init(RS232_COMM_PORT, &GPIO_InitStructure);

	//RX引脚配置
	GPIO_InitStructure.GPIO_Pin = RS232_COMM_RX_PIN;
	GPIO_Init(RS232_COMM_PORT, &GPIO_InitStructure);

	/***串口配置***/
	USART_InitStructure.USART_BaudRate = RS232_COMM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	/***串口初始化***/
	USART_Init(RS232_COMM, &USART_InitStructure);
	
	USART_ClearITPendingBit(RS232_COMM, USART_IT_RXNE);
	
	/***允许串口***/
	USART_Cmd(RS232_COMM, ENABLE);
	
	USART_ITConfig(RS232_COMM, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/***设置串口中断***/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
}

//==================================================================================================
//| 函数名称 | RS232SendData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过RS232发送数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指向要发送的数据缓冲区
//|          | bytes 发送数据的字节数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-01 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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
	
	//串口配置
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//串口初始化
	USART_Init(USARTx, &USART_InitStructure);	
}



