#include "includes.h"

MY_WIFI MyWifi;

static  OS_EVENT   *WifiMutex;
static void WifiInitOS(void)
{
	INT8U  err;
	
	WifiMutex = OSMutexCreate(RES_WIFI_MUTEX_PRIO, &err);
}
void WifiLock(void)
{
	INT8U  err;
	
	OSMutexPend(WifiMutex,0,&err);
}
void WifiUnlock(void)
{	
	OSMutexPost(WifiMutex);
}
ErrorStatus FindStringFromBuffer(u8*str,u16 str_bytes, u8*buf, u16 buf_bytes)
{
	u16 i;

	for(i=0; i<buf_bytes; i++)
	{
		if((i+str_bytes) <= buf_bytes)
		{
			if(0 == strncmp((char*)str, (char*)(buf+i), str_bytes))
			{
				return SUCCESS;
			}
		}
		else
		{
			break;
		}
	}
	return ERROR;	
}
u8 IPAddrU32ToString(u32 IpAddrU32, u8* IpAddrStr)
{
	return (u8)sprintf((char*)IpAddrStr, "%d.%d.%d.%d", (IpAddrU32>>24)&0xFF, (IpAddrU32>>16)&0xFF,(IpAddrU32>>8)&0xFF, IpAddrU32&0xFF);
}
u32 IpAddrStringToU32(u8* IpAddrStr)
{
	//60.247.90.218
	u8 i, Value;
	u32 IpU32 = 0, Temp = 0;
	
	for(i=0; i<strlen((const char*)IpAddrStr); i++)
	{
		Value = *(IpAddrStr+i);
		if(Value == '.')
		{				
			IpU32 += Temp;
			IpU32 <<= 8;
			Temp = 0;
		}
		else
		{
			Temp *= 10;
			Temp += (Value - '0');	
		}
	}
	IpU32 +=Temp;
	return IpU32;
}

void WifiGpioPinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(WIFI_NRESET_PORT_RCC| WIFI_NLINK_PORT_RCC |
		WIFI_NREADY_PORT_RCC | WIFI_NRELOAD_PORT_RCC | WIFI_POWER_PORT_RCC, ENABLE);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = WIFI_NRESET_PIN;	
	GPIO_Init(WIFI_NRESET_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WIFI_POWER_PIN;	
	GPIO_Init(WIFI_POWER_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	
	GPIO_InitStructure.GPIO_Pin = WIFI_NLINK_PIN;	
	GPIO_Init(WIFI_NLINK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WIFI_NREADY_PIN;	
	GPIO_Init(WIFI_NREADY_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WIFI_NRELOAD_PIN;	
	GPIO_Init(WIFI_NRELOAD_PORT, &GPIO_InitStructure);	
	
	CLOSE_WIFI_POWER();
	WIFI_NRESET_PIN_L();
}
void SetWifiUartBaudRate(u32 baudRate)
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
	USART_Init(WIFI_COM, &USART_InitStructure);	
}
void WifiUartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//设置串口时钟
	RCC_AHB1PeriphClockCmd(WIFI_COM_PORT_RCC, ENABLE);	//PA时钟
	RCC_APB1PeriphClockCmd(WIFI_COM_RCC, ENABLE);		//串口2时钟

	//设置引脚映射，TX脚和RX脚
	GPIO_PinAFConfig(WIFI_COM_PORT, WIFI_COM_RX_PINSOURCE, GPIO_AF_USART2); 
	GPIO_PinAFConfig(WIFI_COM_PORT, WIFI_COM_TX_PINSOURCE, GPIO_AF_USART2);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX引脚配置
	GPIO_InitStructure.GPIO_Pin = WIFI_COM_TX_PIN;	
	GPIO_Init(WIFI_COM_PORT, &GPIO_InitStructure);

	//RX引脚配置
	GPIO_InitStructure.GPIO_Pin = WIFI_COM_RX_PIN;
	GPIO_Init(WIFI_COM_PORT, &GPIO_InitStructure);

	//串口配置
	SetWifiUartBaudRate(WIFI_COM_BAUD_EXPECT);	

	USART_ClearITPendingBit(WIFI_COM, USART_IT_RXNE);
	USART_ITConfig(WIFI_COM, USART_IT_RXNE, ENABLE);

	/***设置串口中断***/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(WIFI_COM, ENABLE);
}

void WifiConfig(void)
{
	WifiGpioPinConfig();	
	WifiUartConfig();
	
	WifiInitOS();
}
void WifiSendData(u8* ptr, u16 bytes)
{
	u16 i;

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(WIFI_COM, *ptr++);
		while(USART_GetFlagStatus(WIFI_COM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(WIFI_COM, USART_FLAG_TC) == RESET);
}

ErrorStatus WifiWaitReceive(uint8_t *ptr_rx, uint16_t *bytes_rx, uint16_t wait_bytes, u32 timer_out)
{
	u16 num;
	u8 dly =100;
	u32 c_time;
	
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
	{
		if(MyWifi.RecData.Bytes >= wait_bytes)
		{		
			while(dly--)
			{
				num = MyWifi.RecData.Bytes;
                System168MDelay1ms(2);
//				OSTimeDlyHMSM(0,0,0,2);
				if(num == MyWifi.RecData.Bytes)
				{//已经没有数据返回
					break;
				}
			}
			if(MyWifi.RecData.Bytes <= (*bytes_rx))
			{
				memcpy(ptr_rx, &MyWifi.RecData.Buffer[0], MyWifi.RecData.Bytes);
				*bytes_rx = MyWifi.RecData.Bytes;
				return SUCCESS;			
			}
			else
			{
				return ERROR;
			}
		}
		System168MDelay1ms(1);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ)
		{
			return ERROR;
		}
	}
	return ERROR;
}
ErrorStatus WifiTransmitter(u8 *ptr_rx, u16* bytes_rx, u8 *ptr_tx, u16 bytes_tx, u16 wait_bytes,u32 timer_out)
{	
	ErrorStatus Result;
	
	WifiLock();

	MyWifi.RecData.Bytes = 0;
	memset(&MyWifi.RecData.Buffer[0], 0, BUF_MAX);
	
	WifiSendData(ptr_tx, bytes_tx);
	
	Result = WifiWaitReceive(ptr_rx, bytes_rx, wait_bytes, timer_out);
	
	WifiUnlock();
	return Result;
}

ErrorStatus WifiReset(void)
{
	u8 i = 0;
	
	OPEN_WIFI_POWER();
	WIFI_NRESET_PIN_L();
	OSTimeDlyHMSM(0,0,0,30);
	WIFI_NRESET_PIN_H();
	OSTimeDlyHMSM(0,0,0,800);

	while(GET_WIFI_NREADY_STATE() != WIFI_START_SUCCESS)
	{
		i++;
		if(i >= 50)
		{
			return ERROR;
		}	
		OSTimeDlyHMSM(0,0,0,100);
	}
	return SUCCESS;
}
ErrorStatus WifiSetCmdMode(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	
	while(Retry--)
	{
		while(1)
		{
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "+++",strlen("+++"),1,500))
			{
				break;
			}
			if((Len == 3) && (Buffer[0] == '+') && (Buffer[1] == '+') && (Buffer[2] == '+'))
			{//当前已经在命令模式
				WifiTransmitter(Buffer,&Len, "\n",strlen("\n"),1,1000);//消除重复进入命令模式的影响，否则下一条命令会出错
				OSTimeDlyHMSM(0,0,0,50);//延时不能少
			}
			else if((Len == 1) && (Buffer[0] == 'a'))
			{//当前为透传模式
				Len = sizeof(Buffer);
				if(ERROR == WifiTransmitter(Buffer,&Len, "a",strlen("a"),1,500))
				{
					break;
				}
				if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
				{
					break;
				}	
			}
			else
			{
				return ERROR;
			}	
			return SUCCESS;	
		}
		OSTimeDlyHMSM(0,0,0,100);
	}
	return ERROR;
}
ErrorStatus WifiExitCmdMode(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	
	while(Retry--)
	{
		//进入透传模式
		WifiTransmitter(Buffer,&Len, "\n",strlen("\n"),1,1000);//消除重复进入命令模式的影响，否则下一条命令会出错
		OSTimeDlyHMSM(0,0,0,100);
		Len = sizeof(Buffer);
		if(ERROR == WifiTransmitter(Buffer,&Len, "AT+ENTM\n",strlen("AT+ENTM\n"), strlen("AT+ENTM")+3,500))
		{
			return ERROR;
		}
		if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
		{
			return ERROR;
		}		
		OSTimeDlyHMSM(0,0,0,60);
		Len = sizeof(Buffer);
		if(ERROR != WifiTransmitter(Buffer,&Len, "+++",strlen("+++"),1,500))
		{
			if((Len == 3) && (Buffer[0] == '+') && (Buffer[1] == '+') && (Buffer[2] == '+'))
			{//当前在命令模式
				WifiTransmitter(Buffer,&Len, "\n",strlen("\n"),1,1000);//消除重复进入命令模式的影响，否则下一条命令会出错
			}
			else if((Len == 1) && (Buffer[0] == 'a'))
			{//当前为透传模式
				WifiTransmitter(Buffer,&Len, "\n",strlen("\n"),1,1000);//消除重复进入命令模式的影响，否则下一条命令会出错
				OSTimeDlyHMSM(0,0,0,100);
				return SUCCESS;
			}
			else
			{
				;
			}
		}

		OSTimeDlyHMSM(0,0,0,100);
	}
	return ERROR;	
}
void WifiVersion(void)
{
	u8 Buffer[50];
	u16 Len;
//	u8 Retry = 3;
	
	//确保模式进入了命令模式
	if(ERROR == WifiSetCmdMode())
	{
		return ;
	}
	memset(Buffer, 0, sizeof(Buffer));
	Len = sizeof(Buffer);
	if(ERROR == WifiTransmitter(Buffer,&Len, "AT+VER\n",strlen("AT+VER\n"),1,500))
	{
		return;
	}
	if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
	{
		return;
	}
	memset(Buffer, 0, sizeof(Buffer));
	Len = sizeof(Buffer);
	if(ERROR == WifiTransmitter(Buffer,&Len, "AT+LVER\n",strlen("AT+LVER\n"),1,500))
	{
		return;
	}
	if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
	{
		return;
	}	
}
ErrorStatus SetWifiUartBaud(u32 baud)
{
	u8 Cmd[50];
	u8 Buffer[50];
	u16 Len;
	
	sprintf((char*)Cmd,"AT+UART=%u,8,1,NONE,NFC\n", baud);
	Len = sizeof(Buffer);
	if(ERROR == WifiTransmitter(Buffer,&Len, Cmd,strlen((const char*)Cmd), strlen((const char*)Cmd)+3,1000))
	{
		return ERROR;
	}
	if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
	{
		return ERROR;
	}
	SetWifiUartBaudRate(baud);
	if(ERROR == WifiReset())//复位后生效
	{
		return ERROR;
	}
	if(ERROR == WifiSetCmdMode())
	{
		return ERROR;
	}
	return SUCCESS;
}
ErrorStatus WifiInit(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 2;
	
	while(Retry--)	
	{
		while(1)
		{
			if(ERROR == WifiReset())
			{
				break;
			}
			SetWifiUartBaudRate(WIFI_COM_BAUD_EXPECT);
			if(ERROR == WifiSetCmdMode())
			{
				SetWifiUartBaudRate(WIFI_COM_BAUD_DEFAULT);
				if(ERROR == WifiSetCmdMode())
				{
					break;
				}
				else if(ERROR == SetWifiUartBaud(WIFI_COM_BAUD_EXPECT))
				{//修改WIFI模块的波特率
					break;
				}
			}
			//强制设置成115200
//			if(ERROR == SetWifiUartBaud(WIFI_COM_BAUD_DEFAULT))
//			{
//				break;
//			}			
					
			//只用SOCKET,不用SOCKETB
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+UART\n",strlen("AT+UART\n"), 10,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}
			
			//只用SOCKET,不用SOCKETB
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPDISB=OFF\n",strlen("AT+TCPDISB=OFF\n"), strlen("AT+TCPDISB=OFF")+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}			
			
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+WMODE\n",strlen("AT+WMODE\n"), strlen("AT+WMODE\n")+2,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("STA", strlen("STA"), Buffer, Len))
			{
				Len = sizeof(Buffer);
				if(ERROR == WifiTransmitter(Buffer,&Len, "AT+WMODE=STA\n",strlen("AT+WMODE=STA\n"), strlen("AT+WMODE=STA\n")+3,1000))
				{
					break;
				}
				if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
				{
					break;
				}
				if(ERROR == WifiReset())
				{
					break;
				}		
				//复位后生效
				if(ERROR == WifiSetCmdMode())
				{
					break;
				}
			}
			
			return SUCCESS;	
		}
		OSTimeDlyHMSM(0,0,0,100);
	}
	return ERROR;
}

void WifiScanAP(AP_SCAN_RESULT* Result)
{
	u8 Buffer[1024],Temp[33],Bytes=0;
	u16 Len, i;
	u16 FlagNumber = 0;
	
	//确保模式进入了命令模式
	if(ERROR == WifiSetCmdMode())
	{
		return ;
	}
	
	memset((char*)Result, 0, sizeof(AP_SCAN_RESULT));
	memset((char*)Buffer, 0, sizeof(Buffer));
	memset((char*)Temp, 0, sizeof(Temp));
	
	Len = sizeof(Buffer);
	if(ERROR == WifiTransmitter(Buffer,&Len, "AT+WSCAN\n",strlen("AT+WSCAN\n"), strlen("AT+WSCAN\n")+4,5000))
	{
		return ;
	}
	WifiLock();
	for(i=0;i<100;i++)
	{
		if ((MyWifi.RecData.Buffer[MyWifi.RecData.Bytes-4] == 0x0A) && 
			(MyWifi.RecData.Buffer[MyWifi.RecData.Bytes-3] == 0x0D) && 
			(MyWifi.RecData.Buffer[MyWifi.RecData.Bytes-2] == 0x0D) && 
			(MyWifi.RecData.Buffer[MyWifi.RecData.Bytes-1] == 0x0A))
		{
			memcpy(Buffer, &MyWifi.RecData.Buffer[0], MyWifi.RecData.Bytes);
			Len = MyWifi.RecData.Bytes;
			break;
		}
        WifiUnlock();
		OSTimeDlyHMSM(0,0,0,50);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ)
		{
			return;
		}
        WifiLock();
	}
	WifiUnlock();
	if(i >= 100)
	{
		return ;
	}
	/*
	格式如：
	41 54 2B 57 53 43 41 4E 0A 0D 
	A9 2B 6F 6B 3D 0A 0D 
	43 68 2C 53 53 49 44 2C 42 53 53 49 44 2C 53 65 63 75 72 69 74 79 2C 49 6E 64 69 63 61 74 6F 72 0D 0A 
	31 2C 68 75 61 78 69 6E 2C 31 34 3A 37 35 3A 39 30 3A 41 45 3A 38 41 3A 44 32 2C 57 50 41 50 53 4B 57 50 41 32 50 53 4B 2F 41 45 53 2C 31 30 30 0A 0D 
	31 2C 72 61 6D 62 6F 2C 35 30 3A 42 44 3A 35 46 3A 34 41 3A 33 37 3A 38 30 2C 57 50 41 50 53 4B 57 50 41 32 50 53 4B 2F 41 45 53 2C 31 0A 0D 
	36 2C 48 65 6C 6C 65 6E 27 73 20 57 6F 68 6E 75 6E 67 2C 30 30 3A 32 33 3A 43 44 3A 39 39 3A 37 35 3A 32 34 2C 57 50 41 32 50 53 4B 2F 41 45 53 2C 34 30 0A 0D 
	31 31 2C 73 6C 71 67 63 2C 44 30 3A 43 37 3A 43 30 3A 33 30 3A 39 33 3A 38 30 2C 57 50 41 50 53 4B 57 50 41 32 50 53 4B 2F 41 45 53 2C 35 34 0A 0D 
	31 31 2C 54 50 2D 4C 49 4E 4B 5F 33 35 37 39 31 45 2C 41 38 3A 35 37 3A 34 45 3A 33 35 3A 37 39 3A 31 45 2C 57 50 41 50 53 4B 57 50 41 32 50 53 4B 2F 41 45 53 2C 32 31 0A 0D 0D 0A 
	第1个和第2个(最后一个)0D 0A 之间是扫描到的多条网络信息,每条网络信号用0A 0D分隔；网络信号中的数据项用‘,’分隔
	*/
	//1,huaxin,14:75:90:AE:8A:D2,WPAPSKWPA2PSK/AES,100
	if (Len > 60)
	{
		for(i=10;i<Len;i++)
		{
			if ((Buffer[i] == 0x0D) && (Buffer[i+1] == 0x0A))
			{
				if(memcmp("Indicator",&Buffer[i-strlen("Indicator")], strlen("Indicator")) == 0)
                {
                    break;                
                }
			}
		}
		if(i == Len)
		{
			return ;
		}
		for(;i<Len;i++)
		{
			if ((Buffer[i] == 0x0A) && (Buffer[i+1] == 0x0D))
			{
				i++;
				FlagNumber = 0;
				Bytes = 0;
				memset((char*)Temp, 0, sizeof(Temp));
			}
			else if(Buffer[i] == ',')
			{
				FlagNumber++;
				if(FlagNumber == 2)
				{//网络名
					memcpy(&Result->ScanSum[Result->ScanNumber].ssid, &Temp[0], Bytes);
                    Result->ScanSum[Result->ScanNumber].ssid[32] = 0x00;
				}
				else if(FlagNumber == 4)
				{//加密方式
					if (ERROR != FindStringFromBuffer("WPA2PSK", strlen("WPA2PSK"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Auth, "WPA2PSK");
					}
					else if (ERROR != FindStringFromBuffer("WPAPSK", strlen("WPAPSK"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Auth, "WPAPSK");
					}
					else if (ERROR != FindStringFromBuffer("SHARED", strlen("SHARED"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Auth, "SHARED");
					}
					else if (ERROR != FindStringFromBuffer("OPEN", strlen("OPEN"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Auth, "OPEN");
					}
					else
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Auth, "WPA2PSK");
					}

					if (ERROR != FindStringFromBuffer("AES", strlen("AES"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Encry, "AES");
					}
					else if (ERROR != FindStringFromBuffer("TKIP", strlen("TKIP"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Encry, "TKIP");
					}
					else if (ERROR != FindStringFromBuffer("WEP-A", strlen("WEP-A"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Encry, "WEP-A");
					}
					else if (ERROR != FindStringFromBuffer("WEP-H", strlen("WEP-H"), Temp, Bytes))
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Encry, "WEP-H");
					}
					else
					{
						strcpy((char*)Result->ScanSum[Result->ScanNumber].Encry, "NONE");
					}
					
					Result->ScanNumber++;
					if(Result->ScanNumber >= AP_SCAN_RESULT_MAX)
					{
						return ;
					}
				}
				Bytes = 0;
			}
			else
			{
				if(Bytes < 31)
				{
					Temp[Bytes++] = Buffer[i];				
				}
				else
				{//数据不对
					memset((char*)Result, 0, sizeof(AP_SCAN_RESULT));
					return;
				}
			}
		}
	}
}
ErrorStatus WifiConnectAP(u8* SSID, u8* Passwords, u8* Auth, u8* Encry)
{
	u8 Cmd[100]/*, Number = 0*/;
	u8 Buffer[100];
	u16 Len;
	u8 Retry = 3;
	
	while(Retry--)	
	{
		while(1)
		{
			//确保模式进入了命令模式
			if(ERROR == WifiSetCmdMode())
			{
				break;
			}
			
			//选择连接的无线网络
			sprintf((char*)Cmd,"AT+WSSSID=%s\n", SSID);
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, Cmd,strlen((const char*)Cmd), strlen((const char*)Cmd)+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}
			
			//设置无线网络加密方式
			sprintf((char*)Cmd,"AT+WSKEY=%s,%s,%s\n", (char*)Auth, (char*)Encry, (char*)Passwords);
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, Cmd,strlen((const char*)Cmd), strlen((const char*)Cmd)+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}
			else if (ERROR != FindStringFromBuffer("+ERR=", strlen("+ERR="), Buffer, Len))
			{
				Retry = 0;
				break;
			}
			if(ERROR == WifiReset())
			{
				break;			
			}	
			//复位后生效
			if(ERROR == WifiSetCmdMode())
			{
				break;
			}
			return SUCCESS;			
		}
		OSTimeDlyHMSM(0,0,0,500);
	}
	WifiReset();
	WifiSetCmdMode();
	return ERROR;
}
ErrorStatus GetWifiCurrentAPInfo(u8* SSID, u8* Passwords, u8* Auth, u8* Encry)
{
	u8 Buffer[100], Flag, index;
	u16 Len,i;
	u8 Retry = 3;
	
	while(Retry--)	
	{
		while(1)
		{
			SSID[0] = 0;
			Passwords[0] = 0;
			Auth[0] = 0;
			Encry[0] = 0;

			//确保模式进入了命令模式
			if(ERROR == WifiSetCmdMode())
			{
				break;
			}
			
			//无线网络
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+WSSSID\n",strlen("AT+WSSSID\n"), strlen("AT+WSSSID\n")+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok=", strlen("+ok="), Buffer, Len))
			{
				break;
			}
			Flag = 0xFF;
			for(i=strlen("AT+WSSSID\n");i<Len;i++)
			{
				if(Flag == 0xFF)
				{
					if(Buffer[i] == '=')
					{
						Flag = 0;
						index = 0;
					}			
				}
				else
				{
					if ((Buffer[i] == '\r') || (Buffer[i] == '\n'))
					{
						break;
					}
					else
					{
						if(index<32)
						{
							SSID[index++] = Buffer[i];
							SSID[index] = 0x00;				
						}
					}
				}

			}	

			//无线网络加密方式
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+WSKEY\n",strlen("AT+WSKEY\n"), strlen("AT+WSKEY\n")+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok=", strlen("+ok="), Buffer, Len))
			{
				break;
			}
			Flag = 0xFF;
			for(i=strlen("AT+WSKEY\n");i<Len;i++)
			{
				if(Flag == 0xFF)
				{
					if(Buffer[i] == '=')
					{
						Flag = 0;
						index = 0;
					}			
				}
				else
				{
					if ((Buffer[i] == '\r') || (Buffer[i] == '\n'))
					{
						break;
					}
					else if(Buffer[i] == ',')
					{
						Flag++;
						index = 0;
					}
					else
					{
						if(Flag==0)
						{
							if(index<9)
							{
								Auth[index++] = Buffer[i];
								Auth[index] = 0x00;				
							}				
						}
						else if(Flag==1)
						{
							if(index<9)
							{
								Encry[index++] = Buffer[i];
								Encry[index] = 0x00;				
							}				
						}
						else if(Flag==2)
						{
							if(index<31)
							{
								Passwords[index++] = Buffer[i];
								Passwords[index] = 0x00;				
							}				
						}				
					}
				}
			}	
			return SUCCESS;		
		}
		OSTimeDlyHMSM(0,0,0,500);
	}	
	return ERROR;
}
ErrorStatus CloseWifiSocket(void)
{
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
		
	while ((Retry--) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))	
	{
		while(1)
		{
			//确保模式进入了命令模式
			if(ERROR == WifiSetCmdMode())
			{
				break;
			}
			
			//该命令不需要重启
			Len = sizeof(Buffer);
			//if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPDISB=OFF\n",strlen("AT+TCPDISB=OFF\n"), strlen("AT+TCPDISB=OFF")+3,1000))
			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPDIS=OFF\n",strlen("AT+TCPDIS=OFF\n"), strlen("AT+TCPDIS=OFF")+3,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}

			return SUCCESS;		
		}
		OSTimeDlyHMSM(0,0,0,500);
	}	
	return ERROR;
}
ErrorStatus OpenWifiSocket(u32 IP, u16 Port)
{
	u8 Cmd[50], Number = 0;
	u8 Buffer[50];
	u16 Len;
	u8 Retry = 3;
	
	while ((Retry--) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))	
	{
		while(1)
		{			
			//确保模式进入了命令模式
			if(ERROR == WifiSetCmdMode())
			{
				break;
			}

			//设置套接字的网络参数
			IPAddrU32ToString(IP,&Buffer[0]);
			sprintf((char*)Cmd,"AT+NETP=TCP,CLIENT,%d,%s\n", Port, &Buffer[0]);			
			Len = sizeof(Buffer);
			if(ERROR == WifiTransmitter(Buffer,&Len, &Cmd[0],strlen((const char*)Cmd), strlen((const char*)Cmd)+5,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}	

			if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPDIS=ON\n",strlen("AT+TCPDIS=ON\n"), strlen("AT+TCPDIS=ON")+5,1000))
			{
				break;
			}
			if (ERROR == FindStringFromBuffer("+ok", strlen("+ok"), Buffer, Len))
			{
				break;
			}			
			while(Number < 30)
			{
				OSTimeDlyHMSM(0,0,0,100);
				
				//查询套接字是否已经连接
				Len = sizeof(Buffer);
				//if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPLKB\n",strlen("AT+TCPLKB\n"), strlen("AT+TCPLKB")+3,500))
				if(ERROR == WifiTransmitter(Buffer,&Len, "AT+TCPLK\n",strlen("AT+TCPLK\n"), strlen("AT+TCPLK")+3,500))
				{
					return ERROR;
				}
				if (ERROR != FindStringFromBuffer("+ok=on", strlen("+ok=on"), Buffer, Len))
				{
					if(ERROR != WifiExitCmdMode())//进入透传模式
					{
						return SUCCESS;					
					}
				}	
				Number++;
			}
			break;			
		}
	}	
	return ERROR;
}

//透传模式下数据的发送与接收
ErrorStatus WifiTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out)
{
	u16 num;
	u8 dly =100;
	u32 c_time;
	
	WifiLock();
	
	while(retry--)
	{
		MyWifi.RecData.Bytes = 0;
		memset(&MyWifi.RecData.Buffer[0], 0, BUF_MAX);	
		
		WifiSendData(tbuf, tlen);

		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
		{
			if(MyWifi.RecData.Bytes > 0)
			{		
				while(dly--)
				{
					num = MyWifi.RecData.Bytes;
					System168MDelay1ms(5);
					if(num == MyWifi.RecData.Bytes)
					{//已经没有数据返回
						break;
					}
				}

				memcpy(rbuf, &MyWifi.RecData.Buffer[0], MyWifi.RecData.Bytes);
				*rlen = MyWifi.RecData.Bytes;
				
				WifiUnlock();
				return SUCCESS;			
			}
            WifiUnlock();
			OSTimeDly(1);
			if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ)
			{
				OSTaskDel(OS_PRIO_SELF);
				return ERROR;
			}
            WifiLock();
		}
	}
	
	WifiUnlock();
	
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  根据保存的WIFI密码, 自动连接WIFI
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年5月8日
  * @note   
  *************************************************************************************  
  */
void AutoConnect(void)
{
	u8 i;
	u32 c_time;
	u8 password[100];
	
	if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP))
	{
		for(i=0;i<MyWifi.ScanResult.ScanNumber;i++)
		{
			memset(password, 0, sizeof(password));
			if(ERROR != ReadWifiInfo(MyWifi.ScanResult.ScanSum[i].ssid, password))
			{
				if(ERROR != WifiConnectAP(MyWifi.ScanResult.ScanSum[i].ssid, password,MyWifi.ScanResult.ScanSum[i].Auth,MyWifi.ScanResult.ScanSum[i].Encry))//连接网络
				{
					//如果已经连到网络，则保存WIFI密码
					c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
					while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
					{
						OSTimeDly(10);
						if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
						{
							GetWifiCurrentAPInfo(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry);
							return ;
						}				
					}								
				}			
			}
		}
	}
	else
	{
		GetWifiCurrentAPInfo(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry);
	}
}
//命令模式下数据发送与接收
//ErrorStatus WifiTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out)
//{
//	u8 Cmd[50], Size, Index;
//	static u8 Buffer[1048];
//	u16 Len,i,DataLen;
//	u32 c_time;
//	
//	while(retry--)
//	{
//		if (WIFI_CONNECT_AP == GET_WIFI_NLINK_STATE())
//		{
//			//发送数据，每次最多只能发送1000字节
//			Index=0;
//			while(((Index+1)*1000) <= tlen)
//			{
//				//sprintf((char*)Cmd, "AT+SNDB=%d\n", 1000);
//				sprintf((char*)Cmd, "AT+SEND=%d\n", 1000);
//				Len = sizeof(Buffer);
//				if(ERROR == WifiTransmitter(Buffer,&Len, Cmd,strlen((const char*)Cmd), strlen((const char*)Cmd)+2,500))
//				{
//					return ERROR;
//				}
//				if (ERROR == FindStringFromBuffer(">", strlen(">"), Buffer, Len))
//				{
//					return ERROR;
//				}
//				WifiSendData(tbuf+ Index*1000, Size);	
//				Index++;
//			}

//			//sprintf((char*)Cmd, "AT+SNDB=%d\n", tlen%1000);
//			sprintf((char*)Cmd, "AT+SEND=%d\n", tlen%1000);
//			Len = sizeof(Buffer);
//			if(ERROR == WifiTransmitter(Buffer,&Len, Cmd,strlen((const char*)Cmd), strlen((const char*)Cmd)+2,500))
//			{
//				return ERROR;
//			}
//			if (ERROR == FindStringFromBuffer(">", strlen(">"), Buffer, Len))
//			{
//				return ERROR;
//			}
//			WifiSendData(tbuf+ Index*1000, tlen%1000);	
//			
//			//接收数据		
//			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
//			while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
//			{
//				OSTimeDlyHMSM(0,0,0,50);
//				memset((char*)&MyWifi.RecData,0,sizeof(MyWifi.RecData));
//				Len = sizeof(Buffer);
//				//if(ERROR == WifiTransmitter(Buffer,&Len, "AT+RCVB=1000\n",strlen("AT+RCVB=1000\n"), strlen("AT+RCVB=1000\n")+strlen("+ok="),4000))
//				if(ERROR == WifiTransmitter(Buffer,&Len, "AT+RECV=100\n",
//					strlen("AT+RECV=100\n"), strlen("AT+RECV=100\n")+strlen("+ok="),4000))
//				{
//					return ERROR;
//				}
//				if (ERROR == FindStringFromBuffer("+ok=", strlen("+ok="), Buffer, Len))
//				{
//					return ERROR;
//				}
//				DataLen = 0xFF;
//				//for(i=strlen("AT+RCVB=1000\n"); i<Len; i++)
//				for(i=strlen("AT+RECV=1000,1\n"); i<Len; i++)
//				{
//					if(DataLen == 0xFF)
//					{
//						if(Buffer[i] == '=')
//						{
//							DataLen = 0;
//						}					
//					}
//					else
//					{
//						if(Buffer[i] == ',')
//						{
//							if(DataLen != 0)
//							{
//								if(Len > (DataLen+i+1))
//								{
//									*rlen = DataLen;
//									memcpy(rbuf, &Buffer[i+1], DataLen);
//									return SUCCESS;
//								}							
//							}

//							break;	
//						}
//						else 
//						{
//							if((Buffer[i]>='0') && (Buffer[i] <= '9'))
//							{
//								DataLen *= 10;
//								DataLen += Buffer[i] - '0';							
//							}
//							else
//							{
//								break;
//							}
//						}					
//					}
//				}			
//			}		
//		}	
//	}
//	return ERROR;
//}

