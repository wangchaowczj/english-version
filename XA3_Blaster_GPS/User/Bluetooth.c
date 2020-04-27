#include "includes.h"

#include "Bluetooth.h"
BLUE_SLAVE_LIST BlueSlaveList;
STR_COMM  BlueData = {0,RECIEVING,0};

static u8 BlueCurrentHostSlaveStatus = 0;
BLUE_STATUS BlueStatus;

void BlueUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//���ô���ʱ��
	RCC_AHB1PeriphClockCmd(BLUE_COM_PORT_RCC, ENABLE);	//PBʱ��
	RCC_APB1PeriphClockCmd(BLUE_COM_RCC, ENABLE);		//����4ʱ��

	//��������ӳ�䣬TX�ź�RX��
	GPIO_PinAFConfig(BLUE_COM_PORT, BLUE_COM_RX_PINSOURCE, GPIO_AF_UART4); 
	GPIO_PinAFConfig(BLUE_COM_PORT, BLUE_COM_TX_PINSOURCE, GPIO_AF_UART4);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = BLUE_COM_TX_PIN;	
	GPIO_Init(BLUE_COM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = BLUE_COM_RX_PIN;
	GPIO_Init(BLUE_COM_PORT, &GPIO_InitStructure);

	//��������
	USART_InitStructure.USART_BaudRate = BLUE_COM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//���ڳ�ʼ��
	USART_Init(BLUE_COM, &USART_InitStructure);

	USART_ClearITPendingBit(BLUE_COM, USART_IT_RXNE);
	USART_ITConfig(BLUE_COM, USART_IT_RXNE, ENABLE);

	/***���ô����ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(BLUE_COM, ENABLE);
}

/**
  *************************************************************************************
  * @brief  ����ģ����������
  * @param[in] ��
  * @retval ��
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
void BluePinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(BLUE_PW_PORT_RCC | BLUE_AT_CMD_PORT_RCC | BLUE_CONNECT_STATUS_PORT_RCC  , ENABLE);
	
	BlueUartConfig();
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	
	GPIO_InitStructure.GPIO_Pin = BLUE_PW_PIN; 
	GPIO_Init(BLUE_PW_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = BLUE_AT_CMD_PIN; 
	GPIO_Init(BLUE_AT_CMD_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	GPIO_InitStructure.GPIO_Pin = BLUE_CONNECT_STATUS_PIN; 
	GPIO_Init(BLUE_CONNECT_STATUS_PORT, &GPIO_InitStructure);

	CLOSE_BLUE_POWER();
	BLUE_EXIT_AT_CMD_MODE();
}

/**
  *************************************************************************************
  * @brief  ����ģ�鷢������
  * @param[in] ptr:  ���ݻ�����
  * @param[in] bytes: ���ݳ���
  * @retval ��
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
void BlueSendData(u8* ptr, u16 bytes)
{
	u16 i;
	u8 number = 0;
	
	for(i=0; i < bytes ;i++)
	{
		USART_SendData(BLUE_COM, *ptr++);
		while(USART_GetFlagStatus(BLUE_COM, USART_FLAG_TXE) == RESET);
		if((GET_BLUE_AT_CMD_MODE_STATUS() == EXIT_AT_CMD_MODE_STATUS) && (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS()))
		{//͸��״̬�����߷��ͽ�������Ҫ����ʱ
			number++;
			if(number >= 19)
			{
				number = 0;
				OSTimeDlyHMSM(0,0,0,20);
			}		
		}
	}
	while(USART_GetFlagStatus(BLUE_COM, USART_FLAG_TC) == RESET);
}

/**
  *************************************************************************************
  * @brief  �ȴ���������ģ������
  * @param[in] ptr_rx:  ���ݻ�����
  * @param[in] bytes_rx: ���ݳ���
  * @param[in] wait_bytes:  �ȴ�����������
  * @param[in] timer_out: ��ʱʱ��
  * @retval ��
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BlueWaitReceive(uint8_t *ptr_rx, uint16_t *bytes_rx, uint16_t wait_bytes, u32 timer_out)
{
	u16 num;
	u8 dly =100;
	u32 c_time;
	
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
	{
		if(BlueData.bytenum >= wait_bytes)
		{		
			while(dly--)
			{
				num = BlueData.bytenum;
				OSTimeDlyHMSM(0,0,0,5);
				if(num == BlueData.bytenum)
				{//�Ѿ�û�����ݷ���
					break;
				}
			}
			if((bytes_rx == NULL) || (BlueData.bytenum <= (*bytes_rx)))
			{
				if(ptr_rx != NULL)
				{
					memcpy(ptr_rx, &BlueData.uch_buf[0], BlueData.bytenum);
				}
				if(bytes_rx != NULL)
				{
					*bytes_rx = BlueData.bytenum;
				}
				
				return SUCCESS;			
			}
			else
			{
				return ERROR;
			}
		}
		OSTimeDlyHMSM(0,0,0,50);
		if ((OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) ||
			(GET_BLUE_POWER_STATUS() != 0))
		{
			return ERROR;
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ������ģ�鷢�Ͳ���������(AT����)
  * @param[in] ptr_rx:  ���ݻ�����
  * @param[in] bytes_rx: ���ݳ���
  * @param[in] ptr_tx:  ���ݻ�����
  * @param[in] bytes_tx: ���ݳ���  
  * @param[in] wait_bytes:  �ȴ�����������
  * @param[in] timer_out: ��ʱʱ��
  * @retval ��
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BlueATCmdTransmitter(u8 *ptr_rx, u16* bytes_rx, u8 *ptr_tx, u16 bytes_tx, u16 wait_bytes,u32 timer_out)
{	
	ErrorStatus Result;

	BlueData.bytenum = 0;
	memset(&BlueData.uch_buf[0], 0, BUF_MAX);
	
	//����ǿ��AT����ģʽ
	BLUE_ENTER_AT_CMD_MODE();
	
	BlueSendData(ptr_tx, bytes_tx);	
	Result = BlueWaitReceive(ptr_rx, bytes_rx, wait_bytes, timer_out);
	
	//�˳�ǿ��AT����ģʽ
	BLUE_EXIT_AT_CMD_MODE();
	
	return Result;
}
/**
  *************************************************************************************
  * @brief  ����ģ�鵱ǰ״̬
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��8��17��
  * @note   ��
  *************************************************************************************  
  */
BLUE_STATUS GetBlueStatus(void)
{
    return BlueStatus;
}
/**
  *************************************************************************************
  * @brief  ����ģ��������ʼ��
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��8��17��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BlueHostInit(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	
	if(0 != GET_BLUE_POWER_STATUS())
	{
		OPEN_BLUE_POWER();
		BlueData.bytenum = 0;
		Retry = 50;
		while((Retry--) && (BlueData.bytenum == 0))
		{
			OSTimeDlyHMSM(0,0,0,100);
		}
		OSTimeDlyHMSM(0,0,0,100);				
	}
	SetBlueCurrentHostSlaveStatus(1);
	Retry = 3;
	while(Retry--)
	{
		//����Ϊ����
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+ROLE=1\r\n",strlen("AT+ROLE=1\r\n"), strlen("OK+1\r\n"),500))
		{
			continue;
		}
		
		if(ERROR == FindStringFromBuffer("OK+1\r\n",strlen("OK+1\r\n"),Buffer,Len))
		{
			continue;
		}
		
		//����
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+RESET\r\n",strlen("AT+RESET\r\n"), strlen("OK\r\n"),500))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		
		if(ERROR == FindStringFromBuffer("OK\r\n",strlen("OK\r\n"),Buffer,Len))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		BlueData.bytenum = 0;
		Retry = 30;
		while((Retry--) && (BlueData.bytenum < strlen("Central\r\n")))
		{
			OSTimeDlyHMSM(0,0,0,100);
		}
		OSTimeDlyHMSM(0,0,0,100);
		BlueData.bytenum = 0;
        BlueStatus = BLUE_MASTER;
		return SUCCESS;
		
	}
	BlueStatus = BLUE_ERROR;
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ����ģ��ӻ���ʼ��
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��10��27��
  * @note   �޸�����BOX_A_001  A��ʾ���,001��ʾ�豸ID��3λ������ģ���Ʋ��ܳ���11���ַ�
  *************************************************************************************  
  */
ErrorStatus BlueSlaveInit(void)
{
	u8 Buffer[50],Buffer1[50];
	u16 Len;
	u8 Retry = 3;

	if(0 != GET_BLUE_POWER_STATUS())
	{
		OPEN_BLUE_POWER();
		BlueData.bytenum = 0;
		Retry = 50;
		while((Retry--) && (BlueData.bytenum == 0))
		{
			OSTimeDlyHMSM(0,0,0,100);
		}
		OSTimeDlyHMSM(0,0,0,100);				
	}
	SetBlueCurrentHostSlaveStatus(0);
	Retry = 3;	
	while(Retry--)
	{
		//����Ϊ�ӻ�
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+ROLE=0\r\n",strlen("AT+ROLE=0\r\n"), strlen("OK+0\r\n"),500))
		{
			continue;
		}
		
		if(ERROR == FindStringFromBuffer("OK+0\r\n",strlen("OK+0\r\n"),Buffer,Len))
		{
			continue;
		}
		
		//�޸�����BOX_A_001  A��ʾ���,001��ʾ�豸ID��3λ������ģ���Ʋ��ܳ���11���ַ�
		memset(Buffer1, 0, sizeof(Buffer1));
		sprintf((char*)Buffer1, "AT+NAME=BOX_%c_",device_info.name);
		
		Len = strlen((char*)device_info.mac);
		if(Len >= 3)
		{
			memcpy(&Buffer1[strlen((char*)Buffer1)], &device_info.mac[Len-3], 3);
		}
		else
		{
			memcpy(&Buffer1[strlen((char*)Buffer1)], &device_info.mac[0], Len);
		}
		strcpy((char*)&Buffer1[strlen((char*)Buffer1)], "\r\n");
		
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, Buffer1, strlen((char*)Buffer1), strlen("OK+BOX_"),500))
		{
			OSTimeDlyHMSM(0,0,0,100);
			continue;
		}
		if(ERROR == FindStringFromBuffer("OK+BOX_",strlen("OK+BOX_"),Buffer,Len))
		{
			OSTimeDlyHMSM(0,0,0,100);
			continue;
		}
		
		//����
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+RESET\r\n",strlen("AT+RESET\r\n"), strlen("OK\r\n"),500))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		
		if(ERROR == FindStringFromBuffer("OK\r\n",strlen("OK\r\n"),Buffer,Len))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		
		BlueData.bytenum = 0;
		Retry = 30;
		while((Retry--) && (BlueData.bytenum < strlen("Peripheral\r\n")))
		{
			OSTimeDlyHMSM(0,0,0,100);
		}
		OSTimeDlyHMSM(0,0,0,100);
		BlueData.bytenum = 0;
		
        BlueStatus = BLUE_SLAVE;
		return SUCCESS;
		
	}
	BlueStatus = BLUE_ERROR;
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ������ģ������汾��
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2017��09��18��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus GetBlueVersion(u8* Version)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	
	Retry = 3;	
	while(Retry--)
	{
		//������ģ�������汾��
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+VER\r\n",strlen("AT+VER\r\n"), strlen("OK+V1.0\r\n"),500))
		{
			continue;
		}
		if(Len != 9)
		{
			continue;
		}
		if(ERROR == FindStringFromBuffer("OK+",strlen("OK+"),Buffer,Len))
		{
			continue;
		}
		memcpy(Version, &Buffer[4], 3);
		*(Version+3) = 0x00;
		return SUCCESS;
	}
    BlueStatus = BLUE_ERROR;
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ���ݴӻ����������й���
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2016��01��15��
  * @note   ��
  *************************************************************************************  
  */
void BlueSlaveFilter(u8* Feature)
{
	u8 i,j;
	
	for(i=0; i<BlueSlaveList.Count; i++)
	{
		while(memcmp(BlueSlaveList.Info[i].Name, Feature, strlen((char*)Feature)) != 0)
		{
			for(j=i; j<BlueSlaveList.Count-1; j++)
			{
				memcpy((char*)&BlueSlaveList.Info[j], (char*)&BlueSlaveList.Info[j+1], sizeof(BlueSlaveList.Info[0]));
			}			

			BlueSlaveList.Count--;
			if(BlueSlaveList.Count == i)
			{
				return;
			}
		}
	}
}
/**
  *************************************************************************************
  * @brief  ����ģ������ɨ��ӻ�
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
void BlueScanSlave(void)
{
	u8 SlaveNumber=0;
	u16 i;
	u16 U16Value1, U16Value2;
	
	//ȷ��ģʽ����������ģʽ
	
	memset((char*)&BlueSlaveList, 0, sizeof(BLUE_SLAVE_LIST));

	if(ERROR == BlueATCmdTransmitter(NULL,NULL, "AT+SCAN\r\n",strlen("AT+SCAN\r\n"), strlen("OK+0\r\n"),3000))
	{
		return ;
	}
	

	if ((BlueData.bytenum >= strlen("OK+0\r\n")) && 
		('O' == BlueData.uch_buf[0]) && ('K' == BlueData.uch_buf[1]) && ('+' == BlueData.uch_buf[2]) &&
		('\r' == BlueData.uch_buf[4]) && ('\n' == BlueData.uch_buf[5]))		
	{
		if((BlueData.uch_buf[3] >= '0') && (BlueData.uch_buf[3] <= '9'))
		{
			SlaveNumber = BlueData.uch_buf[3]-'0';
			if((SlaveNumber != 0) && (SlaveNumber <= BLUE_SLAVE_LIST_MAX))
			{
				OSTimeDlyHMSM(0,0,0,100);
				U16Value1 = 0;
				for(i=6;i<BlueData.bytenum;i++)
				{
					if ((BlueData.uch_buf[i-1]=='\r') && (BlueData.uch_buf[i]=='\n'))
					{
						U16Value1++;
					}				
				}
				if(U16Value1 == 2*SlaveNumber)
				{
					U16Value1 = 0;
					U16Value2 = 6;
					for(i=6;i<BlueData.bytenum;i++)
					{
						if ((BlueData.uch_buf[i-1]=='\r') && (BlueData.uch_buf[i]=='\n'))
						{
							if(U16Value1 == 0)
							{//����
								if((i-U16Value2-1) < sizeof(BlueSlaveList.Info[0].Name))
								{
									memcpy(BlueSlaveList.Info[BlueSlaveList.Count].Name, &BlueData.uch_buf[U16Value2], i-U16Value2-1);
									U16Value1 = 1;							
								}

							}
							else
							{//MAC��ַ
								if((i-U16Value2-1) < sizeof(BlueSlaveList.Info[0].Addr))
								{
									memcpy(BlueSlaveList.Info[BlueSlaveList.Count].Addr, &BlueData.uch_buf[U16Value2], i-U16Value2-1);
									U16Value1 = 0;
									BlueSlaveList.Count++;							
								}
							}
							
							U16Value2 = i+1;						
						}				
					}
					
					if(BlueSlaveList.Count != SlaveNumber)
					{
						memset((char*)&BlueSlaveList, 0, sizeof(BLUE_SLAVE_LIST));
					}
				}
			}		
		}
	}
}

/**
  *************************************************************************************
  * @brief  ����ģ���������Ӵӻ�
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��10��27��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BlueConnectSlave(u8 Index)
{
	u8 Buffer[50], SendBuffer[50];
	u16 Len;
	u8 Retry = 3;
	u32 c_time;
	
	
	if((BlueSlaveList.Count == 0) || (Index >= BlueSlaveList.Count))
	{
		return ERROR;
	}
	while(Retry--)
	{
		if(ERROR == BlueHostDisConnect())
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		
		sprintf((char*)&SendBuffer[0], "AT+CON=%s\r\n", BlueSlaveList.Info[Index].Addr);
		if(strlen((char*)&SendBuffer[0]) != strlen("AT+CON=D4F51378FBA6\r\n"))
		{
			return ERROR;
		}
		
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, SendBuffer,strlen((const char*)SendBuffer), strlen("OK\r\n"),500))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}			

		if (ERROR == FindStringFromBuffer("OK\r\n", strlen("OK\r\n"), Buffer, Len))
		{
			OSTimeDlyHMSM(0,0,1,0);
			continue;
		}
		
		OSTimeDlyHMSM(0,0,0,100);
		//ͨ�������ж��Ƿ��Ѿ�������			
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+15000/(1000/OS_TICKS_PER_SEC)))	
		{
			if(GET_BLUE_CONNECT_STATUS() != BLUE_NOT_CONNECT)
			{
				return SUCCESS;
			}
			OSTimeDly(1);
		}		
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ����ģ�������Ͽ�����
  * @param[in] ��
  * @retval ���
  * @author ZH
  * @date   2015��10��27��
  * @note   ֻ������������
  *************************************************************************************  
  */
ErrorStatus BlueHostDisConnect(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	u32 c_time;
	
	while(Retry--)
	{
		if(GET_BLUE_CONNECT_STATUS() == BLUE_NOT_CONNECT)
		{
			return SUCCESS;
		}	
		//����Ϊ����
		Len = sizeof(Buffer);
		if(ERROR == BlueATCmdTransmitter(Buffer,&Len, "AT+DIS\r\n",strlen("AT+DIS\r\n"), strlen("OK\r\n"),500))
		{
			continue;
		}
		
		if(ERROR == FindStringFromBuffer("OK\r\n",strlen("OK\r\n"),Buffer,Len))
		{
			continue;
		}
		
		//ͨ�������ж��Ƿ��Ѿ�������			
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+2000/(1000/OS_TICKS_PER_SEC)))	
		{
			if(GET_BLUE_CONNECT_STATUS() == BLUE_NOT_CONNECT)
			{
				return SUCCESS;
			}
			OSTimeDly(1);
		}		
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ���浱ǰ����ģ�����ӻ�״̬
  * @param[in] Status 0 ��ʾ�ӻ���1��ʾ����
  * @retval ���
  * @author ZH
  * @date   2015��10��27��
  * @note  
  *************************************************************************************  
  */
void SetBlueCurrentHostSlaveStatus(u8 Status)
{
	BlueCurrentHostSlaveStatus = Status;
}
/**
  *************************************************************************************
  * @brief  ��ȡ��ǰ����ģ�����ӻ�״̬
  * @param[in] ��
  * @retval ���  0 ��ʾ�ӻ���1��ʾ����
  * @author ZH
  * @date   2015��10��27��
  * @note  
  *************************************************************************************  
  */
u8 GetBlueCurrentHostSlaveStatus(void)
{
	return BlueCurrentHostSlaveStatus;
}

