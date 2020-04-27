#ifndef _GPRS_H_
#define _GPRS_H_


//GPRS对应的串口
#define GPRS_COM				USART2					//串口2
#define GPRS_COM_RCC			RCC_APB1Periph_USART2	//串口2时钟
#define GPRS_COM_PORT			GPIOA					//PA
#define GPRS_COM_PORT_RCC		RCC_AHB1Periph_GPIOA	//PA时钟
#define GPRS_COM_RX_PIN			GPIO_Pin_3				//输入脚为PA3
#define GPRS_COM_RX_PINSOURCE	GPIO_PinSource3
#define GPRS_COM_TX_PIN			GPIO_Pin_2				//输出脚为PA2
#define GPRS_COM_TX_PINSOURCE	GPIO_PinSource2


//GPRS对应的IO控制/状态口
#define GPRS_SLEEP_STATE_PORT_RCC	 RCC_AHB1Periph_GPIOH
#define GPRS_SLEEP_STATE_PORT		 GPIOH
#define GPRS_SLEEP_STATE_PIN		 GPIO_Pin_6
#define GPRS_SLEEP_STATE             0
#define GET_GPRS_SLEEP_STATE()		 GPIO_ReadInputDataBit(GPRS_SLEEP_STATE_PORT, GPRS_SLEEP_STATE_PIN)		//低电平是睡眠状态，高电平是唤醒状态

#define GPRS_WAKEUP_OUT_PORT_RCC	 RCC_AHB1Periph_GPIOH
#define GPRS_WAKEUP_OUT_PORT		 GPIOH
#define GPRS_WAKEUP_OUT_PIN		     GPIO_Pin_8
#define GPRS_WAKEUP_OUT_STATE        1   //唤醒主机时GPRS模块输出1秒高电平，再变低电平(平时为低)
#define GET_GPRS_WAKEUP_OUT_STATE()	 GPIO_ReadInputDataBit(GPRS_WAKEUP_OUT_PORT, GPRS_WAKEUP_OUT_PIN)		//低电平是睡眠状态，高电平是唤醒状态

#define GPRS_WAKEUP_IN_PORT_RCC		 RCC_AHB1Periph_GPIOH
#define GPRS_WAKEUP_IN_PORT			 GPIOH
#define GPRS_WAKEUP_IN_PIN			 GPIO_Pin_7
#define GPRS_SLEEP_MODE_ENABLE()	 GPIO_ResetBits(GPRS_WAKEUP_IN_PORT, GPRS_WAKEUP_IN_PIN)//允许进入睡眠模式
#define GPRS_SLEEP_MODE_DISABLE()	 GPIO_SetBits(GPRS_WAKEUP_IN_PORT, GPRS_WAKEUP_IN_PIN)	//禁止进入睡眠模式

#define GPRS_RSTIN_N_PORT_RCC		 RCC_AHB1Periph_GPIOH
#define GPRS_RSTIN_N_PORT			 GPIOH
#define GPRS_RSTIN_N_PIN			 GPIO_Pin_5
#define GPRS_RSTIN_N_L()			 GPIO_ResetBits(GPRS_RSTIN_N_PORT, GPRS_RSTIN_N_PIN)//复位信息有效，需要50-100ms
#define GPRS_RSTIN_N_H()			 GPIO_SetBits(GPRS_RSTIN_N_PORT, GPRS_RSTIN_N_PIN)	//复位信息无效

#define GPRS_POWER_ON_OFF_PORT_RCC   RCC_AHB1Periph_GPIOH
#define GPRS_POWER_ON_OFF_PORT		 GPIOH
#define GPRS_POWER_ON_OFF_PIN		 GPIO_Pin_4
#define GPRS_POWER_ON_OFF_OFF()		 GPIO_ResetBits(GPRS_POWER_ON_OFF_PORT, GPRS_POWER_ON_OFF_PIN)	//关闭GPRS内部电源
#define GPRS_POWER_ON_OFF_ON()		 GPIO_SetBits(GPRS_POWER_ON_OFF_PORT, GPRS_POWER_ON_OFF_PIN)	//打开GPRS内部电源

#define GPRS_POWER_PORT_RCC          RCC_AHB1Periph_GPIOH
#define GPRS_POWER_PORT		         GPIOH
#define GPRS_POWER_PIN		         GPIO_Pin_3
#define GPRS_POWER_OFF()			 GPIO_ResetBits(GPRS_POWER_PORT, GPRS_POWER_PIN)	//关闭GPRS外部电源
#define GPRS_POWER_ON()			     GPIO_SetBits(GPRS_POWER_PORT, GPRS_POWER_PIN)		//打开GPRS外部电源
#define GET_GPRS_POWER_STATE()       GPIO_ReadOutputDataBit(GPRS_POWER_PORT, GPRS_POWER_PIN)


typedef enum {
    GPRS_FAILURE,        // 执行失败
    GPRS_SUCESS,         // 执行成功
	GPRS_NET_ERROR,      // 网络连接错误执行
    GPRS_GETDATA         // 有数据返回
} GPRS_STATUS;

typedef struct
{
	u8 GPRS_Co_Info[60];    //运营商信息
	u8 GPRS_Signal;			//信号质量	
	u8 mb_addr;
	GPRS_STATUS Result;
}GPRS_INFO;
extern GPRS_INFO GPRS_info;

//#define GPRS_IP_ADDR		"\"socktcp://60.247.90.218:20600\""
extern GPRS_STATUS FindStringFromBuffer(u8*str,u16 str_bytes, u8*buf, u16 buf_bytes);

extern STR_COMM gprs_data;

extern void GPRSConfig(void);
extern void GPRSReset(void);
extern GPRS_STATUS GPRSPowerOnInit(u8 ctrl_byte);
extern GPRS_STATUS GPRSNetInit(u8* ip_addr);
extern GPRS_STATUS GPRSInit(u8* ip_addr);
extern GPRS_STATUS GPRSSendMessage(u8* tel, u8* msg_data);
extern GPRS_STATUS GPRSCheckSignal(u8 *gprs_signal);
extern u8 GPRSSearchNetwork(u8 *buf);
extern GPRS_STATUS GPRSTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out);
//extern GPRS_STATUS GPRSNetTest(void);

#endif
