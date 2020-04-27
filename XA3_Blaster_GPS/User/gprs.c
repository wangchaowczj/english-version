#include "includes.h"

STR_COMM gprs_data;
GPRS_INFO GPRS_info;


//==================================================================================================
//| 函数名称 | GPS_Port_Config
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPS的IO口配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 |
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   |
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
static void GRPSPortConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//睡眠状态引脚
	RCC_AHB1PeriphClockCmd(GPRS_SLEEP_STATE_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_SLEEP_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPRS_SLEEP_STATE_PORT, &GPIO_InitStructure);

	//GPRS唤醒主机引脚
	RCC_AHB1PeriphClockCmd(GPRS_WAKEUP_OUT_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_WAKEUP_OUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPRS_WAKEUP_OUT_PORT, &GPIO_InitStructure);

	//睡眠模式使能开关
	RCC_AHB1PeriphClockCmd(GPRS_WAKEUP_IN_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_WAKEUP_IN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPRS_WAKEUP_IN_PORT, &GPIO_InitStructure);

	//模块复位引脚
	RCC_AHB1PeriphClockCmd(GPRS_RSTIN_N_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_RSTIN_N_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPRS_RSTIN_N_PORT, &GPIO_InitStructure);
	
	//模块内部电源控制引脚
	RCC_AHB1PeriphClockCmd(GPRS_POWER_ON_OFF_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_POWER_ON_OFF_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPRS_POWER_ON_OFF_PORT, &GPIO_InitStructure);	
	
	//模块外部电源控制引脚
	RCC_AHB1PeriphClockCmd(GPRS_POWER_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPRS_POWER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPRS_POWER_PORT, &GPIO_InitStructure);		

	GPRS_SLEEP_MODE_DISABLE();
	GPRS_RSTIN_N_H();
	GPRS_POWER_ON_OFF_OFF();
	GPRS_POWER_OFF();
}
//==================================================================================================
//| 函数名称 | USART2_Config
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 串口2配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 |
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   |
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
static void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//设置串口时钟
	RCC_AHB1PeriphClockCmd(GPRS_COM_PORT_RCC, ENABLE);	//PA时钟
	RCC_APB1PeriphClockCmd(GPRS_COM_RCC, ENABLE);		//串口2时钟

	//设置引脚映射，TX脚和RX脚,RTS脚和CTS脚
	GPIO_PinAFConfig(GPRS_COM_PORT, GPRS_COM_RX_PINSOURCE, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPRS_COM_PORT, GPRS_COM_TX_PINSOURCE, GPIO_AF_USART2);

	//TX和RTS引脚配置
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPRS_COM_TX_PIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPRS_COM_PORT, &GPIO_InitStructure);

	//RX和CTS引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPRS_COM_RX_PIN;
	GPIO_Init(GPRS_COM_PORT, &GPIO_InitStructure);

	//串口配置
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//串口初始化
	USART_Init(GPRS_COM, &USART_InitStructure);

	//允许串口
	USART_Cmd(GPRS_COM, ENABLE);

	USART_ClearITPendingBit(GPRS_COM, USART_IT_RXNE);
	USART_ITConfig(GPRS_COM, USART_IT_RXNE, ENABLE);
	
	//设置串口中断
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

//==================================================================================================
//| 函数名称 | GPSConfig
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPS模块配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-14 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void GPRSConfig(void)
{
	USART2_Config();
	GRPSPortConfig();
}

//==================================================================================================
//| 函数名称 | GPRS_COM_SendData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 向GPRS模块发送数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指向要发送的数据缓冲区
//|          | bytes 发送数据的字节数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void GPRS_COM_SendData(u8* ptr, u16 bytes)
{
	u16 i;

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(GPRS_COM, *ptr++);
		while(USART_GetFlagStatus(GPRS_COM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(GPRS_COM, USART_FLAG_TC) == RESET);
}

//==================================================================================================
//| 函数名称 | GPRSWaitReceive 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 从GPRS模块中接收数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr_rx 接收到的数据指针
//|          | bytes_rx 接收到的数据长度
//|          | wait_bytes 至少等待的数据长度
//|          | timer_out 接收等待的超时时间
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  接收到数据
//|          | GPRS_FAILURE  没有接收到数据
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSWaitReceive(uint8_t *ptr_rx, uint16_t *bytes_rx, uint16_t wait_bytes, u32 timer_out)
{
	u16 num;
	u8 dly =100;
	u32 c_time;
	
//	gprs_data.bytenum = 0;	
//	memset(&gprs_data.uch_buf[0], 0, BUF_MAX);
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))
	{
		if(gprs_data.bytenum >= wait_bytes)
		{		
			while(dly--)
			{
				num = gprs_data.bytenum;
				OSTimeDlyHMSM(0,0,0,50);
				if(num == gprs_data.bytenum)
				{//已经没有数据返回
					break;
				}
			}
			memcpy(ptr_rx, gprs_data.uch_buf, gprs_data.bytenum);
			*bytes_rx = gprs_data.bytenum;
			return GPRS_SUCESS;
		}
		OSTimeDlyHMSM(0,0,0,20);
	}
	return GPRS_FAILURE;
}

//==================================================================================================
//| 函数名称 | GPRSTransmitter 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过GPRS模块发送并接收数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr_rx   接收到的数据指针
//|          | bytes_rx 接收到的数据长度指针
//|          | ptr_tx   要发送的数据指针
//|          | bytes_tx 要发送的数据长度
//|          | wait_bytes 至少等待的数据长度
//|          | timer_out接收等待的超时时间
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_FAILURE 接收失败 ，GPRS_SUCESS接收到指定长度数据
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSTransmitter(u8 *ptr_rx, u16* bytes_rx, u8 *ptr_tx, u16 bytes_tx, u16 wait_bytes,u32 timer_out)
{
	gprs_data.bytenum = 0;	
	memset(&gprs_data.uch_buf[0], 0, BUF_MAX);
	GPRS_COM_SendData(ptr_tx, bytes_tx);
	return GPRSWaitReceive(ptr_rx, bytes_rx, wait_bytes, timer_out);
}

//==================================================================================================
//| 函数名称 | GPRSPowerOn 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPRS模块打开
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSPowerOn(void)
{	
	u8 flag = 0,i;
	u16 dly = 0;
	
	GPRS_POWER_OFF();
	GPRS_RSTIN_N_L();
	GPRS_SLEEP_MODE_ENABLE();
	OSTimeDlyHMSM(0,0,0,150);
	GPRS_SLEEP_MODE_DISABLE();
	GPRS_RSTIN_N_H();
	OSTimeDlyHMSM(0,0,2,0);//延时2秒
	GPRS_POWER_ON_OFF_ON();
	GPRS_POWER_ON();
	OSTimeDlyHMSM(0,0,3,500);//手册要求大于3秒
	GPRS_POWER_ON_OFF_OFF();
	OSTimeDlyHMSM(0,0,1,500);//手册要求0.5-1秒
	GPRS_POWER_ON_OFF_ON();
	
	do
	{
		gprs_data.bytenum = 0;
		
		//延时1秒
		i = 50;
		while(i--)
		{
			OSTimeDlyHMSM(0,0,0,20);
			if((flag == 0) && (gprs_data.bytenum != 0))
			{//模块上电有返回数据
				flag = 1;			
			}
		}

		dly++;
	}while((gprs_data.bytenum != 0) || (dly < 15));	
	if(0 == flag)
	{//模块没有返回数据，可能是焊接不好或者上电不正常
		return GPRS_FAILURE;
	}
	OSTimeDlyHMSM(0,0,5,0);//延时5秒
	return GPRS_SUCESS;
}

//==================================================================================================
//| 函数名称 | GPRSReset 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPRS模块复位
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void GPRSReset(void)
{
	GPRS_RSTIN_N_H();						//RST引脚拉高
	GPRS_RSTIN_N_L();						//RST引脚拉低
	OSTimeDlyHMSM(0,0,0,150);		        //手册要求延时50-100毫秒
	GPRS_RSTIN_N_H();						//RST引脚拉高
	OSTimeDlyHMSM(0,0,1,0);//延时1秒
}

//==================================================================================================
//| 函数名称 | FindStringFromBuffer 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 在缓冲区中查找指定格式的字符串
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | str 字符串指针
//|          | str_bytes 字符串长度
//|          | buf 缓冲区指针
//|          | buf_bytes缓冲区长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  找到字符串
//|          | GPRS_FAILURE  没有找到字符串
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS FindStringFromBuffer(u8*str,u16 str_bytes, u8*buf, u16 buf_bytes)
{
	u16 i;

	for(i=0; i<buf_bytes; i++)
	{
		if((i+str_bytes) <= buf_bytes)
		{
			if(0 == strncmp((char*)str, (char*)(buf+i), str_bytes))
			{
				return GPRS_SUCESS;
			}
		}
		else
		{
			break;
		}
	}
	return GPRS_FAILURE;	
}

//==================================================================================================
//| 函数名称 | GPRSGetConnect 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 连接GPRS模块
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  连接成功
//|          | GPRS_FAILURE  连接失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 用于测试GPRS模块AT命令
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSGetConnect(void)
{
	uint16_t relen;
	uint8_t tbuf[10] = "AT\r";
	uint8_t rbuf[50];

	if (GPRS_FAILURE == GPRSTransmitter(rbuf,&relen,tbuf,strlen((char*)tbuf),strlen((char*)tbuf)+4, 100))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//****接收的数据判断****//
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	return GPRS_SUCESS;
}

//==================================================================================================
//| 函数名称 | SetGPRSUnsolicite 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 设置GPRS模块主动上报结果
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  连接成功
//|          | GPRS_FAILURE  连接失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-21 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS SetGPRSUnsolicite(void)
{
	uint16_t relen;
	uint8_t tbuf[100] = "AT^CURC=2,4000000000,4000000000\r";//只保留AT^IPDATA
	uint8_t rbuf[512];
	
	if (GPRS_FAILURE == GPRSTransmitter(rbuf,&relen,tbuf,strlen((char*)tbuf),strlen((char*)tbuf)+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	
	//****接收的数据判断****//
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	return GPRS_SUCESS;	
}

//==================================================================================================
//| 函数名称 | GPRSSearchNetwork 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 查找网络信息关返回手机卡的运营商信息
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | buf 运营商信息指针
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 0  查找成功
//|          | 1  查找失败
//|          | 2  没有找到SIM卡
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u8 GPRSSearchNetwork(u8 *buf)
{
	uint16_t relen;
	uint8_t tbuf[100] = "AT+COPS?\r";
	uint8_t rbuf[512];
	uint16_t sign_num1,sign_num2,i;

	if (GPRS_FAILURE == GPRSTransmitter(rbuf,&relen,tbuf, strlen((char*)tbuf),
		strlen((char*)tbuf)+8/*至少应该包含有"+COPS:"和"OK"*/, 120000))		//手册中说2分钟内返回都算正常
	{
		return 1;//没有搜到运营商信息。
	}
	//****接收的数据判断****//
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf, relen))		//查询收到的字符中是否包含OK
	{
		if (GPRS_FAILURE == FindStringFromBuffer("ERROR: SIM failure", strlen("ERROR: SIM failure"), rbuf, relen))
		{
			return 1;//没有搜到运营商信息。
		}
		return 2;//没有SIM卡
	}
	
	//****在收到的字符中找两个"字符。两个"字符中间的就是运营商的信息****//
	sign_num1 = 0xFFFF;		//第一个"字符位置
	sign_num2 = 0xFFFF;		//第二个"字符位置
	for (i=0;i<relen;i++)
	{
		if (rbuf[i] == '"')
		{
			if (sign_num1 != 0xFFFF)
			{
				sign_num2 = i;		//找到第二个"字符后，退出。
				break;
			}
			else
			{
				sign_num1 = i;
			}
		}
	}
	if (sign_num2 != 0xFFFF)				//找到运营商信息
	{
		memcpy(buf,&rbuf[sign_num1+1],(sign_num2-sign_num1-1));
		*(buf+sign_num2-sign_num1-1) = 0x00;
		return 0;
	}
	return 1;		//没有搜到运营商信息。	
}


//==================================================================================================
//| 函数名称 | GPRSCheckSignal 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 测试信号强度
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | gprs_signal 信号强度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  查找成功
//|          | GPRS_FAILURE  查找失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSCheckSignal(u8 *gprs_signal)
{
	uint16_t relen;
	uint8_t tbuf[100] = "AT+CSQ\r";
	uint8_t rbuf[512];
	uint16_t sign_num1,sign_num2,i;
	uint8_t Signal = 0;

	*gprs_signal = 0;
	if (GPRS_FAILURE == GPRSTransmitter(rbuf,&relen,tbuf,strlen((char*)tbuf),
		strlen((char*)tbuf)+7/*至少应该包含有"+CSQ:"和"OK"*/, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	
	//****在收到的字符中找:和,字符。两个字符中间的就是信号质量信息****//
	sign_num1 = 0;		//字符:位置
	sign_num2 = 0;		//字符,位置
	for (i=0;i<relen;i++)
	{
		if (rbuf[i] == ':')			//信号质量开始
		{
			sign_num1 = i;
		}
		else if (rbuf[i] == ',')		//信号质量结束
		{
			sign_num2 = i;
			break;
		}
	}

	if ((sign_num1 != 0) && (sign_num2 != 0))		//找到信号质量信息
	{
		sign_num1++;								//指向第一个字符
		while(sign_num1 != sign_num2)
		{
			if ((rbuf[sign_num1] >=0x30) && (rbuf[sign_num1] <=0x39))		//有可能出现不是数字的情况，那是因为存在空格
			{
				Signal *= 10;								//进一位
				Signal += (rbuf[sign_num1]-0x30);			//保存当前字符
			}
			sign_num1++;							//指向下一个字符
			if (sign_num1 > sign_num2)				//如果出现这种情况，说明取的字符不对，返回错误。
			{
				return GPRS_FAILURE;
			}
		}
		*gprs_signal = Signal;			//返回信号质量
		return GPRS_SUCESS;
	}
	return GPRS_FAILURE;
}


//==================================================================================================
//| 函数名称 | GPRSSetAPN 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 设置网络接入点
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | NetType  0表示联通，1表示电信，其他表示移动
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-14 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static GPRS_STATUS GPRSSetAPN(u8 NetType)
{
	uint16_t relen;
	uint8_t tbuf1[50] = "AT^IPINIT=\"3GNET\"\r";//联通2G用UNINET 联通3G用3GNET
	uint8_t tbuf2[50] = "AT^IPINIT=\"CTNET\"\r";//电信CTNET
	uint8_t tbuf3[50] = "AT^IPINIT=\"CMNET\"\r";//移动CMNET
	uint8_t rbuf[512];
	uint8_t* ptr;

	
	//根据不同运营商设置APN
	if (0 == NetType)
	{//联通
		ptr = &tbuf1[0];
	}
	else if (1 == NetType)
	{//电信
		ptr = &tbuf2[0];
	}
	else
	{//移动
		ptr = &tbuf3[0];
	}
	//设置APN
	if (GPRS_FAILURE == GPRSTransmitter(rbuf,&relen,ptr,strlen((char*)ptr),strlen((char*)ptr)+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_SUCESS == FindStringFromBuffer("OK", strlen("OK"), rbuf, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_SUCESS;
	}
	else if(GPRS_SUCESS == FindStringFromBuffer("The network has been opened already", strlen("The network has been opened already"), rbuf, relen))
	{
		return GPRS_SUCESS;
	}	
	return GPRS_FAILURE;
}



//==================================================================================================
//| 函数名称 | GPRSOpenNet 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 打开无线网络
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ip_addr  IP地址字符串，格式如"60.247.90.218:20600"
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSOpenNet(u8* ip_addr)
{
	uint16_t relen;
	uint8_t tbuf[100] = "AT^IPOPEN=1,\"TCP\",\"";//格式为AT^IPOPEN=1,"TCP","60.247.90.218",20600
	uint8_t rbuf[512];
	uint8_t i, len;
		
	if(0 == strlen((char*)ip_addr))
	{
		return GPRS_FAILURE;
	}
	
	len = strlen((char*)tbuf);
	for(i=0; i<strlen((char*)ip_addr);i++)
	{
		if(*(ip_addr+i) != ':')
		{
			tbuf[len++] = *(ip_addr+i);
		}
		else
		{
			tbuf[len++] = '\"';
			tbuf[len++] = ',';
		}
	}
	tbuf[len++] = '\r';
	tbuf[len] = 0x00;
	
	//打开网络连接
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,tbuf,strlen((char*)tbuf),strlen((char*)tbuf)+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断，
	if (GPRS_SUCESS == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_SUCESS;
	}
	else if (GPRS_SUCESS == FindStringFromBuffer("ERROR: The link has been established already", strlen("ERROR: The link has been established already"), rbuf,relen))
	{
		return GPRS_SUCESS;
	}
	else
	{
		return GPRS_FAILURE;
	}
}

//==================================================================================================
//| 函数名称 | GPRSCloseNet 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 关闭无线网络
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSCloseNet(void)
{
	uint16_t relen;
	uint8_t tbuf[100] = "AT^IPCLOSE=1\r";
	uint8_t rbuf[512];

	GPRSCheckSignal(&GPRS_info.GPRS_Signal);

	//断开网络连接
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,tbuf,strlen((char*)tbuf),strlen((char*)tbuf)+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_SUCESS == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_SUCESS;
	}
	else
	{
		return GPRS_FAILURE;
	}
}


//==================================================================================================
//| 函数名称 | GPRSNetInit 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPRS模块网络初始化
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ip_addr  IP地址字符串,格式如"60.247.90.218:20600"
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSNetInit(u8* ip_addr)
{
	u32 c_time, TimeOut;
	
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	TimeOut = (c_time+65000UL/(1000/OS_TICKS_PER_SEC));
	while(OSTimeGet() < TimeOut)
	{
		if(GPRS_SUCESS == GPRSCloseNet())
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,20);		//延时20毫秒
	}
	OSTimeDlyHMSM(0,0,0,100);//延时100毫秒
	//****打开网络连接****//
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	TimeOut = (c_time+125000UL/(1000/OS_TICKS_PER_SEC));
	while(OSTimeGet() < TimeOut)
	{
		if (GPRS_SUCESS == GPRSOpenNet(ip_addr))
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,100);//延时100毫秒
	}
	if (OSTimeGet() >= TimeOut)
	{
		WriteSystemLog("GPRS模块打开网络连接失败!",sizeof("GPRS模块打开网络连接失败!"), NEW_STR_INFO);
		return GPRS_FAILURE;
	}
	WriteSystemLog("GPRS模块初始化成功!",sizeof("GPRS模块初始化成功!"), NEW_STR_INFO);
	OSTimeDlyHMSM(0,0,1,0);//延时1秒	
	return GPRS_SUCESS;		
}

//==================================================================================================
//| 函数名称 | GPRSPowerOnInit 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPRS模块上电初始化
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ctrl_byte  控制字节，1等待网络搜索完成，0 不等待网络搜索完成
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSPowerOnInit(u8 ctrl_byte)
{	
	u8 Result;
	u32 c_time, TimeOut;
	
	/****GPRS设备上电****/
	if(GPRS_FAILURE == GPRSPowerOn())
	{
		if(GPRS_FAILURE == GPRSPowerOn())
		{
			WriteSystemLog("GPRS模块设备上电失败!",sizeof("GPRS模块设备上电失败!"), NEW_STR_INFO);
			return GPRS_FAILURE;
		}
	}
	
	//设置模块主动上报结果
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	TimeOut = (c_time+60000UL/(1000/OS_TICKS_PER_SEC));
	while(OSTimeGet() < TimeOut)
	{
		if(GPRS_SUCESS == SetGPRSUnsolicite())
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,100);//延时100毫秒
	}
	if (OSTimeGet() >= TimeOut)
	{
		WriteSystemLog("GPRS模块设置主动上报结果失败!",sizeof("GPRS模块设置主动上报结果失败!"), NEW_STR_INFO);
		return GPRS_FAILURE;
	}
	if(ctrl_byte != 0)
	{
		//****等待连接到运营商，该搜索可能会持续很长时间，手册中说2分钟内返回都是有效的，需要耐心等待****//
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		TimeOut = (c_time+120000UL/(1000/OS_TICKS_PER_SEC));
		while(OSTimeGet() < TimeOut)
		{
			memset(GPRS_info.GPRS_Co_Info, 0, sizeof(GPRS_info.GPRS_Co_Info));
			Result = GPRSSearchNetwork(GPRS_info.GPRS_Co_Info);//保存运营商信息内容
			if (0 == Result)
			{//成功
				break;
			}
			else if (2 == Result)
			{
				memset(GPRS_info.GPRS_Co_Info, 0, sizeof(GPRS_info.GPRS_Co_Info));
				WriteSystemLog("GPRS模块没有插入SIM卡!",sizeof("GPRS模块没有插入SIM卡!"), NEW_STR_INFO);
				return GPRS_FAILURE;				
			}
			OSTimeDlyHMSM(0,0,0,100);//延时100毫秒
		}
		if (OSTimeGet() >= TimeOut)
		{
			memset(GPRS_info.GPRS_Co_Info, 0, sizeof(GPRS_info.GPRS_Co_Info));
			WriteSystemLog("GPRS模块搜索运营商信息失败!",sizeof("GPRS模块搜索运营商信息失败!"), NEW_STR_INFO);
			return GPRS_FAILURE;
		}
		//****查询信号质量****//
		if(GPRS_FAILURE == GPRSCheckSignal(&GPRS_info.GPRS_Signal))
		{
			WriteSystemLog("GPRS模块查询信号强度失败!",sizeof("GPRS模块查询信号强度失败!"), NEW_STR_INFO);
			return GPRS_FAILURE;
		}
	}

	return GPRS_SUCESS;
}
//==================================================================================================
//| 函数名称 | GPRSInit 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | GPRS模块初始化
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ip_addr IP地址字符串,格式如"60.247.90.218:20600"
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-14 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSInit(u8* ip_addr)
{
	uint8_t nettype = 0;		//网络运营商
	u32 c_time, TimeOut;

	WriteSystemLog("GPRS模块开始初始化!",sizeof("GPRS模块开始初始化!"), NEW_STR_INFO);
	
	memset(GPRS_info.GPRS_Co_Info,0x00,sizeof(GPRS_info.GPRS_Co_Info));
	GPRS_info.GPRS_Signal = 0x00;
		
	if(GPRS_FAILURE == GPRSPowerOnInit(1))
	{
		return GPRS_FAILURE;
	}
	
	//****网络连接初始化****//
	if(memcmp(GPRS_info.GPRS_Co_Info,"CHN-UNICOM", strlen("CHN-UNICOM")) == 0)
	{//联通"China Unicom"
		nettype = 0;
	}
	else if (memcmp(GPRS_info.GPRS_Co_Info,"CHINA TELECOM", strlen("CHINA TELECOM")) == 0)
	{//电信
		nettype = 1;
	}
	else
	{//移动"CHINA MOBILE"
		nettype = 2;
	}
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	TimeOut = (c_time+65000UL/(1000/OS_TICKS_PER_SEC));
	while(OSTimeGet() < TimeOut)
	{
		if (GPRS_SUCESS == GPRSSetAPN(nettype))//设置接入点
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,100);//延时100毫秒
	}
	if (OSTimeGet() >= TimeOut)
	{
		WriteSystemLog("GPRS模块设置接入点失败!",sizeof("GPRS模块设置接入点失败!"), NEW_STR_INFO);
		return GPRS_FAILURE;
	}
	return GPRSNetInit(ip_addr);
}

//==================================================================================================
//| 函数名称 | GPRSSendMessage 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 发送短信
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | tel  手机号
//|          | msg_data  短信内容字符串形式
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-07-14 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSSendMessage(u8* tel, u8* msg_data)
{
	uint16_t relen,i,j;
	uint8_t tbuf[100] = {0x00};
	uint8_t finish_flag[2] = {0x1A,0x00};
	uint8_t rbuf[512];

	if(strlen((const char*)tel) != 11)
	{//手机号为11位
		return GPRS_FAILURE;
	}
	//文本模式
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,"AT+CMGF=1\r",strlen("AT+CMGF=1\r"),strlen("AT+CMGF=0\r")+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,"AT+CSMP=,,0,8\r",strlen("AT+CSMP=,,0,8\r"),strlen("AT+CSMP=,,0,8\r")+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	//设置短信服务中心号码,号码与手机号归属地相关，根据测试不设置也可以。
//	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,"AT+CSCA=\"8613010112500\",145\r",strlen("AT+CSCA=\"8613010112500\",145\r"),strlen("AT+CSCA=\"8613010112500\",145\r")+4, 30000))		//数据接收失败
//	{
//		return GPRS_FAILURE;
//	}
//	//接收的数据判断
//	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
//	{
//		return GPRS_FAILURE;
//	}
	//UNICODE
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r"),strlen("AT+CSCS=\"UCS2\"\r")+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}	
	
	//设置对方手机号
	i=0;
	memset(tbuf, 0x00,sizeof(tbuf));
	strcpy((char*)tbuf,"AT+CMGS=\"");
	i=strlen((const char*)tbuf);
	for(j=0;j<strlen((const char*)tel);j++)
	{
		if(((*(tel+j)) >= '0') && ((*(tel+j)) <= '9'))
		{
			tbuf[i++] = '0';
			tbuf[i++] = '0';
			tbuf[i++] = '3';
			tbuf[i++] = (*(tel+j));
		}
		else
		{
			return GPRS_FAILURE;
		}
	}
	tbuf[i++] = '\"';
	tbuf[i++] = '\r';
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen, tbuf,strlen((const char*)tbuf),strlen((const char*)tbuf)+4, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	//接收的数据判断
	if (GPRS_FAILURE == FindStringFromBuffer(">", strlen(">"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	//发送的短信内容，UNICODE代码形式
	AnsiToUnicodeHexString(msg_data, tbuf);
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,tbuf,strlen((const char*)tbuf),strlen((const char*)tbuf), 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}
	if (GPRS_FAILURE == GPRSTransmitter(rbuf, &relen,finish_flag,strlen((const char*)finish_flag),strlen((const char*)finish_flag)+3, 30000))		//数据接收失败
	{
		return GPRS_FAILURE;
	}	
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), rbuf,relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}	
	return GPRS_SUCESS;
}

//==================================================================================================
//| 函数名称 | ArrivalDataAnalyse 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对接收到的数据进行解析
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | tbuf  模块返回的数据
//|          | tlen  模块返回的数据长度
//|          | rbuf  解析后的数据
//|          | rlen  解析后的数据长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS ArrivalDataAnalyse(uint8_t *tbuf, uint16_t tlen, uint8_t* rbuf, uint16_t* rlen)
{
	u16 i,len = 0, index1,index2;
	
	//<CR><LF>^IPDATA: <link_id>,<data_len>,<data><CR><LF>
	//^IPDATA: 1,8,12345678
	if(tlen>12)
	{
		for(i=0; i<tlen-8; i++)
		{
			if(*(tbuf+i) == '^')
			{
				if(strncmp((char*)(tbuf+i),(char*)"^IPDATA:", strlen("^IPDATA:")) == 0)
				{
					index1 = 0xFFFF;
					index2 = 0xFFFF;
					for(; i < tlen-1; i++)
					{
						if(*(tbuf+i) == ',')
						{
							if(0xFFFF == index1)
							{
								index1 = i;							
							}
							else
							{
								index2 = i;	
								break;							
							}
						}
					}
					if(i >= (tlen-1))
					{
						return GPRS_FAILURE;
					}
					
					for(i = index1 +1; i < index2; i++)
					{
						if((*(tbuf+i) >= '0') && (*(tbuf+i) <= '9'))
						{
							len *= 10;
							len += (*(tbuf+i)-0x30);
						}						
					}
					if((len == 0) || (len > 512))
					{//数据不对
						return GPRS_FAILURE;
					}
					else if((tlen-index2) > len)
					{
						memcpy(rbuf, tbuf+index2+1,len);
						*rlen = len;
						return GPRS_SUCESS;
					}
					else
					{
						return GPRS_FAILURE;
					}				
				}
			}

		}
	}
	return GPRS_FAILURE;
}
//==================================================================================================
//| 函数名称 | GPRSGetData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 读取GPRS模块接收到数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | receive_buf  接收到数据指针
//|          | receive_len  接收到数据长度
//|          | timer_out    接收超时时间
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSGetData(u8 *receive_buf, u16 *receive_len, u16 timer_out)
{
	uint8_t rbuf[512];
	uint16_t relen;

	if(NULL != receive_len)
	{
		*receive_len = 0;
	}

	if(0 == timer_out)
	{//接收等待时间为0说明不需要接收
		return GPRS_SUCESS;
	}
	gprs_data.bytenum = 0;	
	memset(&gprs_data.uch_buf[0], 0, BUF_MAX);
	if(GPRS_FAILURE == GPRSWaitReceive(rbuf,&relen, 12, timer_out))
	{
		return GPRS_FAILURE;
	}
	if (GPRS_SUCESS == FindStringFromBuffer("^IPDATA:", strlen("IPDATA:"),rbuf,relen))
	{
		if(GPRS_SUCESS == ArrivalDataAnalyse(rbuf, relen, receive_buf, receive_len))
		{
			return GPRS_SUCESS;//模块已经接收到数据
		}		
	}
	return GPRS_FAILURE;
}

//==================================================================================================
//| 函数名称 | GPRSSendData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过GPRS模块发送数据(需要预先连接好网络)
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | tbuf  数据指针
//|          | tlen  数据长度
//|          | rbuf  接收到的数据指针长度
//|          | tlen  接收到的数据长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  发送成功，但没有接收到数据
//|          | GPRS_GETDATA 发送成功，并且模块已经接收到数据
//|          | GPRS_FAILURE  发送失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-19 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 非透传模式
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSSendData(uint8_t *tbuf, uint16_t tlen, uint8_t* rbuf, uint16_t* rlen)
{
	uint8_t temp_buf1[100] ;	
	uint8_t r_temp[512];
	uint16_t relen;
	uint16_t send_len = tlen;

	if (tlen == 0)
	{
		return GPRS_FAILURE;
	}
	
	//发送指令，通知模块要开始发送几个字节
	send_len = sprintf((char*)temp_buf1, "AT^IPSENDEX=1,2,%d",tlen);
	temp_buf1[send_len++] = '\r';
	temp_buf1[send_len++] = 0x00;
	if (GPRS_FAILURE == GPRSTransmitter(r_temp, &relen,temp_buf1,strlen((char*)temp_buf1),strlen((char*)temp_buf1)+4, 3000))
	{
		return GPRS_FAILURE;
	}	
	//接收的数据判断
	if (GPRS_SUCESS != FindStringFromBuffer("OK", strlen("OK"), r_temp, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}	
	
	//这里开始，发送数据
	if (GPRS_FAILURE == GPRSTransmitter(r_temp, &relen,tbuf,tlen, 13,30000))
	{
		return GPRS_FAILURE;
	}	
	//接收的数据判断
	if (GPRS_FAILURE == FindStringFromBuffer("OK", strlen("OK"), r_temp, relen))		//查询收到的字符中是否包含OK
	{
		return GPRS_FAILURE;
	}
	if (GPRS_SUCESS == FindStringFromBuffer("^IPDATA:", strlen("IPDATA:"),r_temp,relen))
	{
		if(GPRS_SUCESS == ArrivalDataAnalyse(r_temp, relen, rbuf, rlen))
		{
			return GPRS_GETDATA;//模块已经接收到数据
		}		
	}
	//模块还没有接收到数据
	return GPRS_SUCESS;
}

//==================================================================================================
//| 函数名称 | GPRSTransferData 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通过GPRS模块发送数据并接收数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | tbuf  发送数据指针
//|          | tlen  发送数据长度
//|          | rbuf  接收数据指针
//|          | rlen  接收数据长度
//|          | retry  重试次数
//|          | timer_out  超时时间
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | GPRS_SUCESS  成功
//|          | GPRS_FAILURE  失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-05-17
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 非透传模式
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
GPRS_STATUS GPRSTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out)
{
	GPRS_STATUS result;

	while(retry--)
	{
		gprs_data.bytenum = 0;
		result = GPRSSendData(tbuf,tlen, rbuf, rlen);
		if (GPRS_GETDATA == result)
		{//已经接收到数据
			return GPRS_SUCESS;
		}
		else if (GPRS_SUCESS == result)
		{
			result = GPRSGetData(rbuf, rlen, timer_out);
			if (GPRS_SUCESS == result)		//数据接收失败
			{
				return GPRS_SUCESS;
			}
		}
		else
		{
			;
		}
	}
	return GPRS_FAILURE;
}

//GPRS_STATUS GPRSNetTest(void)
//{
//	uint16_t relen;
//	uint8_t tbuf1[30] = {0x00,0xC0,0x41,0x77,0x4D,0x41,0x73,0x41,0x41,0x53,0xDF,0x8B,0xC0,0x00};
//	uint8_t rbuf[256];
//	u8 i = 1;

//	while(i--)
//	{
//		tbuf1[0] = i+1;
//		if (GPRS_SUCESS != GPRSTransferData(tbuf1, strlen((char*)tbuf1), rbuf, &relen, 3, 2000))		//数据接收失败
//		{			
//			return GPRS_FAILURE;
//		}
//	}

//	return GPRS_SUCESS;		
//}
