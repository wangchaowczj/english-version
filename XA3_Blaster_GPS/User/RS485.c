#include "includes.h"

static u8 RS485MasterHoldFlag = 0;//从机极性确定标志
static u16 RS485DirCount = 0;//无极性RS485接收端电平稳定时间计数值
static u16 RS485MasterHoldCount = 0;//主机端保持发送状态时间计数值
static RS485_MASTER_SLAVE RS485MasterSW = RS485_SLAVE;//无极性RS485接收端电平判断开关
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
//	EXTI_InitTypeDef EXTI_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIO时钟配置***/
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC | RS485_CONNECT_PORT_RCC | 
		RS485_STATE_PORT_RCC | RS485_DIR_PORT_RCC, ENABLE);
	
	/***设置串口1时钟***/
	RCC_APB2PeriphClockCmd(RS485_COMM_RCC, ENABLE);	
	
	/***管脚配置***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485_SW引脚配置0反向；1正向***/
	GPIO_InitStructure.GPIO_Pin = RS485_CONNECT_PIN;
	GPIO_Init(RS485_CONNECT_PORT, &GPIO_InitStructure);
	
	/***RS485状态判断引脚配置***/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Pin = RS485_STATE_PIN;
	
	GPIO_Init(RS485_STATE_PORT, &GPIO_InitStructure);
	
	/***485方面，发送或者接收***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485 0接收；1发送***/
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
	GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);	
	
	/***PA.8配置为上升沿和下降沿都触发的外部中断模式***/	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line8; 
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿和下降沿都有效  
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	
//	
//	EXTI_ClearITPendingBit(EXTI_Line8);	
	
	/***485方向控制，外部中断***/
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//在中断中打开定时器2中断，因此要比TIMER2优先级低
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
		
	SET_RS485_RECEIVE();//485默认为接收状态
}
void RS485TxPinExitUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RS485_COMM_RCC, DISABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_TX_PIN;	
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);
    
    RS485_COMM_TX_PIN_LOW();
}
//==================================================================================================
//| 函数名称 | SetRS485Slave 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | RS485作为主机，关闭无极性RS485极性判断
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
void SetRS485Master(void)
{
	RS485MasterSW = RS485_MASTER;
	RS485DirCount = 0;
	SET_RS485_SEND();//485设置为发送状态
}

//==================================================================================================
//| 函数名称 | SetRS485Slave 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | RS485作为从机，打开无极性485极性判断
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
void SetRS485Slave(void)
{
	RS485MasterSW = RS485_SLAVE;
	RS485DirCount = 0;
	SET_RS485_RECEIVE();//485默认为接收状态
}

//==================================================================================================
//| 函数名称 | GetRS485MasterSlaveState 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 返回当前RS485的主/从状态
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | RS485的主/从状态
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
RS485_MASTER_SLAVE GetRS485MasterSlaveState(void)
{
	return RS485MasterSW;
}

//==================================================================================================
//| 函数名称 | RS485DirCountDecrement 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据计数值及当前状态设置RS485发送时的方向
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 滴答定时器调用
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void RS485DirCountDecrement(void)
{
	static u8 DirOldStatus = 0;
	
	if(RS485_SLAVE == GetRS485MasterSlaveState())//允许RS485无极性判断
	{
		u8 DirNewStatus = GET_RS485_CONNECT_STATE();
		
		if(DirOldStatus != DirNewStatus)
		{
			RS485DirCount = 0;
			DirOldStatus = DirNewStatus;
		}
		
		if(RS485DirCount < CONNECT_RS485_TIME)
		{
			RS485DirCount++;
			if(RS485DirCount >= CONNECT_RS485_TIME)//电平稳定时间足够
			{
				if(DirNewStatus == CONNECT_RS485_IS_NSTD)
				{
					SET_RS485_CONNECT_NSTD();
				}
				else
				{
					SET_RS485_CONNECT_STD();
				}
			}		
		}
		RS485MasterHoldFlag = 0;
		RS485MasterHoldCount = 0;		
	}
	else
	{
		if(GET_RS485_SEND_OR_RECEIVE_STATUS() == RS485_IS_RECEIVE)
		{//主机接收状态，如果主机长时间处于接收状态也不发送数据时，从机极性判断可能就会不正确
			if(RS485MasterHoldFlag == 1)
			{//
				if(RS485MasterHoldCount < (CONNECT_RS485_TIME-100))
				{
					RS485MasterHoldCount++;
				}
				else
				{
					RS485MasterHoldFlag = 0;
					RS485MasterHoldCount = 0;
				}			
			}
			else
			{
				RS485MasterHoldCount = 0;
			}			
		}		
		else
		{//主机发送状态
			if(RS485MasterHoldFlag == 0)
			{//主机发送状态要保持一段时间才能发送数据，从机进行极性判断
				if(RS485MasterHoldCount < (CONNECT_RS485_TIME+100))
				{
					RS485MasterHoldCount++;
				}
				else
				{
					RS485MasterHoldFlag = 1;//从机已经确定了极性
					RS485MasterHoldCount = 0;
				}			
			}
			else
			{
				RS485MasterHoldCount = 0;
			}
		}
	}
}

//==================================================================================================
//| 函数名称 | ResetRS485DirCount 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 复位计数器
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
void ResetRS485DirCount(void)
{
	RS485DirCount = 0;
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
	if(RS485_MASTER == GetRS485MasterSlaveState())
	{
		while(RS485MasterHoldFlag == 0)
		{
			OSTimeDly(1);
		}	
	}

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(RS485_COMM, *ptr++);
		while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(RS485_COMM, USART_FLAG_TC) == RESET);	
	SET_RS485_RECEIVE();//接收状态	
}

