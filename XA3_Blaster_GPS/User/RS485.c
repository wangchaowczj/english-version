#include "includes.h"

static u8 RS485MasterHoldFlag = 0;//�ӻ�����ȷ����־
static u16 RS485DirCount = 0;//�޼���RS485���ն˵�ƽ�ȶ�ʱ�����ֵ
static u16 RS485MasterHoldCount = 0;//�����˱��ַ���״̬ʱ�����ֵ
static RS485_MASTER_SLAVE RS485MasterSW = RS485_SLAVE;//�޼���RS485���ն˵�ƽ�жϿ���
STR_COMM  RS485Data = {0,RECIEVING,0};

//==================================================================================================
//| �������� | RS485Config 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����485ģ��
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
void RS485Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/***GPIOʱ������***/
	RCC_AHB1PeriphClockCmd(RS485_COMM_PORT_RCC | RS485_CONNECT_PORT_RCC | 
		RS485_STATE_PORT_RCC | RS485_DIR_PORT_RCC, ENABLE);
	
	/***���ô���1ʱ��***/
	RCC_APB2PeriphClockCmd(RS485_COMM_RCC, ENABLE);	
	
	/***�ܽ�����***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485_SW��������0����1����***/
	GPIO_InitStructure.GPIO_Pin = RS485_CONNECT_PIN;
	GPIO_Init(RS485_CONNECT_PORT, &GPIO_InitStructure);
	
	/***RS485״̬�ж���������***/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Pin = RS485_STATE_PIN;
	
	GPIO_Init(RS485_STATE_PORT, &GPIO_InitStructure);
	
	/***485���棬���ͻ��߽���***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***RS485 0���գ�1����***/
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
	GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);	
	
	/***PA.8����Ϊ�����غ��½��ض��������ⲿ�ж�ģʽ***/	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line8; 
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�����غ��½��ض���Ч  
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	
//	
//	EXTI_ClearITPendingBit(EXTI_Line8);	
	
	/***485������ƣ��ⲿ�ж�***/
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//���ж��д򿪶�ʱ��2�жϣ����Ҫ��TIMER2���ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
		
	SET_RS485_RECEIVE();//485Ĭ��Ϊ����״̬
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
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = RS485_COMM_TX_PIN;	
	GPIO_Init(RS485_COMM_PORT, &GPIO_InitStructure);
    
    RS485_COMM_TX_PIN_LOW();
}
//==================================================================================================
//| �������� | SetRS485Slave 
//|----------|--------------------------------------------------------------------------------------
//| �������� | RS485��Ϊ�������ر��޼���RS485�����ж�
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
void SetRS485Master(void)
{
	RS485MasterSW = RS485_MASTER;
	RS485DirCount = 0;
	SET_RS485_SEND();//485����Ϊ����״̬
}

//==================================================================================================
//| �������� | SetRS485Slave 
//|----------|--------------------------------------------------------------------------------------
//| �������� | RS485��Ϊ�ӻ������޼���485�����ж�
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
void SetRS485Slave(void)
{
	RS485MasterSW = RS485_SLAVE;
	RS485DirCount = 0;
	SET_RS485_RECEIVE();//485Ĭ��Ϊ����״̬
}

//==================================================================================================
//| �������� | GetRS485MasterSlaveState 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���ص�ǰRS485����/��״̬
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | RS485����/��״̬
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
RS485_MASTER_SLAVE GetRS485MasterSlaveState(void)
{
	return RS485MasterSW;
}

//==================================================================================================
//| �������� | RS485DirCountDecrement 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���ݼ���ֵ����ǰ״̬����RS485����ʱ�ķ���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | �δ�ʱ������
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void RS485DirCountDecrement(void)
{
	static u8 DirOldStatus = 0;
	
	if(RS485_SLAVE == GetRS485MasterSlaveState())//����RS485�޼����ж�
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
			if(RS485DirCount >= CONNECT_RS485_TIME)//��ƽ�ȶ�ʱ���㹻
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
		{//��������״̬�����������ʱ�䴦�ڽ���״̬Ҳ����������ʱ���ӻ������жϿ��ܾͻ᲻��ȷ
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
		{//��������״̬
			if(RS485MasterHoldFlag == 0)
			{//��������״̬Ҫ����һ��ʱ����ܷ������ݣ��ӻ����м����ж�
				if(RS485MasterHoldCount < (CONNECT_RS485_TIME+100))
				{
					RS485MasterHoldCount++;
				}
				else
				{
					RS485MasterHoldFlag = 1;//�ӻ��Ѿ�ȷ���˼���
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
//| �������� | ResetRS485DirCount 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��λ������
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
void ResetRS485DirCount(void)
{
	RS485DirCount = 0;
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
	SET_RS485_RECEIVE();//����״̬	
}

