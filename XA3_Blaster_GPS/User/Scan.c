#include "includes.h"


SCAN_DATA  ScanData;


/**
  *************************************************************************************
  * @brief  扫码模块IO口配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年10月28日
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
  * @brief  扫码模块串口引角配置为输入
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年10月28日
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
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_TX_PIN;	
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//RX引脚配置
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_RX_PIN;
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);	
}

/**
  *************************************************************************************
  * @brief  扫码模块串口配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
void ScanUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//设置串口时钟
	RCC_AHB1PeriphClockCmd(SCAN_COM_PORT_RCC, ENABLE);	//PA时钟
	RCC_APB1PeriphClockCmd(SCAN_COM_RCC, ENABLE);		//串口2时钟

	//设置引脚映射，TX脚和RX脚
	GPIO_PinAFConfig(SCAN_COM_PORT, SCAN_COM_RX_PINSOURCE, GPIO_AF_USART3); 
	GPIO_PinAFConfig(SCAN_COM_PORT, SCAN_COM_TX_PINSOURCE, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_TX_PIN;	
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//RX引脚配置
	GPIO_InitStructure.GPIO_Pin = SCAN_COM_RX_PIN;
	GPIO_Init(SCAN_COM_PORT, &GPIO_InitStructure);

	//串口配置
	USART_InitStructure.USART_BaudRate = SCAN_COM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//串口初始化
	USART_Init(SCAN_COM, &USART_InitStructure);

	USART_ClearITPendingBit(SCAN_COM, USART_IT_RXNE);
	USART_ITConfig(SCAN_COM, USART_IT_RXNE, ENABLE);

	/***设置串口中断***/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(SCAN_COM, ENABLE);
}


/**
  *************************************************************************************
  * @brief  向扫码模块发送数据
  * @param  ptr  数据缓冲区首地址
  * @param  bytes  字节数
  * @retval 无
  * @author ZH
  * @date   2015年10月28日
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
  * @brief  判断是不是斑马扫码头SE2707
  * @param  无
  * @retval 1 是， 其他 不是
  * @author ZH
  * @date   2019年5月15日
  * @note   
  *************************************************************************************  
  */
u8 IsZEBRA(void)
{
    u8 Buffer[] = {0x0A,0x80,0x04,0x00,0x00,0x06,0x20,0x00,0xFF,0xFF,0xFD,0x4E};
    u32 c_time;
    
    memset(ScanData.Buffer, 0, sizeof(ScanData.Buffer));
    ScanData.Bytes = 0;
	ScanSendData(Buffer, sizeof(Buffer));//读寄存器，用于验证硬件连接无误和读操作成功
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
  * @brief  判断扫码模块是否可以正常通信
  * @param  无
  * @retval 0 通信异常， 3 斑马扫码头 其他值 有扫码头
  * @author ZH
  * @date   2016年7月23日
  * @note   
  *************************************************************************************  
  */
u8 ScanHardwareState(void)
{
	u8 Buffer[] = {0x7E,0x00,0x07,0x01,0x00,0x0A,0x01,0xEE,0x8A};
	u32 c_time;
	
	OPEN_SCAN();
	OSTimeDlyHMSM(0,0,5,0);	//3秒不行,4秒可以
	ScanUartConfig();
	OSTimeDlyHMSM(0,0,0,5);	
	
	ScanData.Bytes = 0;
	ScanSendData(Buffer, sizeof(Buffer));//读寄存器，用于验证硬件连接无误和读操作成功
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
    //查询设备是否处于连接状态,适用于新大陆的EM3296
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
  * @brief  扫码模块工作
  * @param  超时时间
  * @retval 无
  * @author ZH
  * @date   2015年10月28日
  * @note   连续2次扫码得到的数据一致时才停止扫码，数据保存在ScanData
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
	
	//启动一次扫描	
	ScanData.Bytes = 0;
	SCAN_START();
    
	//等待扫描完成
    c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
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
		{//已经没有数据返回
			break;
		}
        if(IsZEBRA && ((OSTimeGet()-newTime) > 9500))
        {//斑马扫码头，超时时间最多9.9s
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
	{//电源已经关闭，数据可能是不完整的
		ScanData.Bytes = 0;
	}
//	ScanExitUartConfig();//如何不配置为输入，在扫码模块电源关掉时，模块没有真正掉电，重新上电会异常
}
