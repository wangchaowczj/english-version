#include "includes.h"


SCAN_DATA  ScanData;


/**
  *************************************************************************************
  * @brief  ɨ��ģ��IO������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��10��28��
  * @note   
  *************************************************************************************  
  */
void ScanGpioPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SCAN_POWER_PORT_RCC| SCAN_SIGNAL_PORT_RCC , ENABLE);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = SCAN_POWER_PIN;	
	GPIO_Init(SCAN_POWER_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SCAN_SIGNAL_PIN;	
	GPIO_Init(SCAN_SIGNAL_PORT, &GPIO_InitStructure);
	
	
	OPEN_SCAN();
}

/**
  *************************************************************************************
  * @brief  ɨ��ģ�鴮����������Ϊ����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��10��28��
  * @note   
  *************************************************************************************  
  */
void ScanExitUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(SCAN_COM_PORT_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(SCAN_COM_RCC, DISABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_TX_PIN;	
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_RX_PIN;
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);	
}

/**
  *************************************************************************************
  * @brief  ɨ��ģ�鴮������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��10��28��
  * @note   
  *************************************************************************************  
  */
void ScanUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//���ô���ʱ��
	RCC_AHB1PeriphClockCmd(SCAN_COM_PORT_RCC, ENABLE);	//PAʱ��
	RCC_APB1PeriphClockCmd(SCAN_COM_RCC, ENABLE);		//����2ʱ��

	//��������ӳ�䣬TX�ź�RX��
	GPIO_PinAFConfig(SCAN_COM_PORT, SCAN_COM_RX_PINSOURCE, GPIO_AF_USART3); 
	GPIO_PinAFConfig(SCAN_COM_PORT, SCAN_COM_TX_PINSOURCE, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_TX_PIN;	
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_RX_PIN;
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//��������
	USART_InitStructure.USART_BaudRate = SCAN_COM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//���ڳ�ʼ��
	USART_Init(SCAN_COM, &USART_InitStructure);

	USART_ClearITPendingBit(SCAN_COM, USART_IT_RXNE);
	USART_ITConfig(SCAN_COM, USART_IT_RXNE, ENABLE);

	/***���ô����ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(SCAN_COM, ENABLE);
}


/**
  *************************************************************************************
  * @brief  ��ɨ��ģ�鷢������
  * @param  ptr  ���ݻ������׵�ַ
  * @param  bytes  �ֽ���
  * @retval ��
  * @author ZH
  * @date   2015��10��28��
  * @note   
  *************************************************************************************  
  */
void ScanSendData(u8* ptr, u16 bytes)
{
	u16 i;

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(SCAN_COM, *ptr++);
		while(USART_GetFlagStatus(SCAN_COM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(SCAN_COM, USART_FLAG_TC) == RESET);
}
/**
  *************************************************************************************
  * @brief  �ж��ǲ��ǰ���ɨ��ͷSE2707
  * @param  ��
  * @retval 1 �ǣ� ���� ����
  * @author ZH
  * @date   2019��5��15��
  * @note   
  *************************************************************************************  
  */
u8 IsZEBRA(void)
{
    u8 Buffer[] = {0x0A,0x80,0x04,0x00,0x00,0x06,0x20,0x00,0xFF,0xFF,0xFD,0x4E};
    u32 c_time;
    
    memset(ScanData.Buffer, 0, sizeof(ScanData.Buffer));
    ScanData.Bytes = 0;
	ScanSendData(Buffer, sizeof(Buffer));//���Ĵ�����������֤Ӳ����������Ͷ������ɹ�
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
	{
		OSTimeDly(1);
		if(ScanData.Bytes >= 0x07)//0C 80 00 00 00 08 20 00 00 F0 00 F0 FD 6C
		{
            u16 i;
            u16 sum = 0;
            
            OSTimeDly(5);
            
            for(i=0;i<ScanData.Bytes-2;i++)
            {
                sum += ScanData.Buffer[i];
            }
            sum += ScanData.Buffer[ScanData.Bytes-2]<<8;
            sum += ScanData.Buffer[ScanData.Bytes-1];
            if(sum == 0)
            {
                return 1;
            }
			return 2;
		}
	}
    return 0;
}
/**
  *************************************************************************************
  * @brief  �ж�ɨ��ģ���Ƿ��������ͨ��
  * @param  ��
  * @retval 0 ͨ���쳣�� 3 ����ɨ��ͷ ����ֵ ��ɨ��ͷ
  * @author ZH
  * @date   2016��7��23��
  * @note   
  *************************************************************************************  
  */
u8 ScanHardwareState(void)
{
	u8 Buffer[] = {0x7E,0x00,0x07,0x01,0x00,0x0A,0x01,0xEE,0x8A};
	u32 c_time;
	
	OPEN_SCAN();
	OSTimeDlyHMSM(0,0,5,0);	//3�벻��,4�����
	ScanUartConfig();
	OSTimeDlyHMSM(0,0,0,5);	
	
	ScanData.Bytes = 0;
	ScanSendData(Buffer, sizeof(Buffer));//���Ĵ�����������֤Ӳ����������Ͷ������ɹ�
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
	{
		OSTimeDly(1);
		if(ScanData.Bytes == 0x07)//0x02 0x00 0x00 0x01 0x3E 0xE4 0xAC
		{
            if(IsZEBRA())
            {
                CLOSE_SCAN();
//                ScanExitUartConfig();
                return 3;
            }
			CLOSE_SCAN();
//			ScanExitUartConfig();
			return 1;
		}
	}
    //��ѯ�豸�Ƿ�������״̬,�������´�½��EM3296
    Buffer[0] = '?';
    ScanData.Bytes = 0;
	ScanSendData(Buffer, 1);
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
	{
		OSTimeDly(1);
		if((ScanData.Bytes == 1) && (ScanData.Buffer[0] == '!'))
		{
			CLOSE_SCAN();
//			ScanExitUartConfig();
			return 2;
		}
	}
	CLOSE_SCAN();
//	ScanExitUartConfig();
	return 0;
}

/**
  *************************************************************************************
  * @brief  ɨ��ģ�鹤��
  * @param  ��ʱʱ��
  * @retval ��
  * @author ZH
  * @date   2015��10��28��
  * @note   ����2��ɨ��õ�������һ��ʱ��ֹͣɨ�룬���ݱ�����ScanData
  *************************************************************************************  
  */
void ScanWork(u32 timer_out, u8 IsZEBRA)
{
	u16 Bytes = 0;
    u32 c_time;
    u32 newTime;
    
    SCAN_STOP();
	ScanUartConfig();
	OSTimeDlyHMSM(0,0,0,5);
	
	//����һ��ɨ��	
	ScanData.Bytes = 0;
	SCAN_START();
    
	//�ȴ�ɨ�����
    c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
    newTime = c_time;
	while ((0 == GET_SCAN_POWER_STATE()) && (OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC))))
	{
        if(OSTaskDelReq(OS_PRIO_SELF))
        {
            ScanData.Bytes = 0;
            break;
        }
		Bytes = ScanData.Bytes;
		OSTimeDlyHMSM(0,0,0,5);
		if ((Bytes != 0) && (Bytes == ScanData.Bytes))
		{//�Ѿ�û�����ݷ���
			break;
		}
        if(IsZEBRA && ((OSTimeGet()-newTime) > 9500))
        {//����ɨ��ͷ����ʱʱ�����9.9s
            SCAN_STOP();
            ScanData.Bytes = 0;
            OSTimeDlyHMSM(0,0,0,10);
            newTime = OSTimeGet();
            SCAN_START();
        }
	}
    if(OSTimeGet() > (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
    {
        ScanData.Bytes = 0;
    }
	SCAN_STOP();	
	OSTimeDlyHMSM(0,0,0,60);
	if(0 != GET_SCAN_POWER_STATE())
	{//��Դ�Ѿ��رգ����ݿ����ǲ�������
		ScanData.Bytes = 0;
	}
//	ScanExitUartConfig();//��β�����Ϊ���룬��ɨ��ģ���Դ�ص�ʱ��ģ��û���������磬�����ϵ���쳣
}
