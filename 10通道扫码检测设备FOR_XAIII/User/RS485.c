#include "includes.h"

STR_COMM  RS485Data = {0,RECIEVING,0};

//==================================================================================================
//| 函数名称 | RS485Config 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置485模块
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
void RS485Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIO时钟配置***/
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC | RS485_DIR_PORT_RCC, ENABLE);
	
	/***设置串口3时钟***/
	RCC_APB1PeriphClockCmd(RS485_COMM_RCC, ENABLE);	
	
	/***485方面，发送或者接收***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485 0接收；1发送***/
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
	GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);		
	
	/***设置引脚映射，TX脚和RX脚***/
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource10, RS485_COMM_AF); 
	GPIO_PinAFConfig(RS485_COMM_PORT, GPIO_PinSource11, RS485_COMM_AF);

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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/***设置串口中断***/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//在中断中打开定时器2中断，因此要比TIMER2优先级低
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
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
	System168MDelay10us(1);	

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS485_COMM, *ptr++);
		while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TC) == RESET);	
	SET_RS485_RECEIVE();//接收状态	
}

