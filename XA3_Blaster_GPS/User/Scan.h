#ifndef _SCAN_H_
#define _SCAN_H_

#define SCAN_COM					USART3					//串口2
#define SCAN_COM_RCC				RCC_APB1Periph_USART3	//串口2时钟
#define SCAN_COM_PORT				GPIOB					//PA
#define SCAN_COM_PORT_RCC			RCC_AHB1Periph_GPIOB	//PB时钟
#define SCAN_COM_RX_PIN				GPIO_Pin_11				//输入脚为PB11
#define SCAN_COM_RX_PINSOURCE		GPIO_PinSource11
#define SCAN_COM_TX_PIN				GPIO_Pin_10				//输出脚为PB10
#define SCAN_COM_TX_PINSOURCE		GPIO_PinSource10
#define SCAN_COM_BAUD            	9600

//从电源开启至完成启动的时间小于200ms
//在关闭电源输入后,在关闭电源输入后,建议间隔500ms以上才允许再次开启电源输入。

#define SCAN_POWER_PORT				GPIOB
#define SCAN_POWER_PORT_RCC			RCC_AHB1Periph_GPIOB
#define SCAN_POWER_PIN				GPIO_Pin_4
#define CLOSE_SCAN_POWER()          GPIO_SetBits(SCAN_POWER_PORT, SCAN_POWER_PIN)	
#define OPEN_SCAN_POWER()           GPIO_ResetBits(SCAN_POWER_PORT, SCAN_POWER_PIN)
#define GET_SCAN_POWER_STATE()      GPIO_ReadOutputDataBit(SCAN_POWER_PORT, SCAN_POWER_PIN)

//触发键输入信号，保持低电平10ms 以上可触发读码
//识读过程需要经过采集图像、条码识别和解译等步骤，建议两次触发信号的间隔时间不低于50ms
#define SCAN_SIGNAL_PORT			GPIOB
#define SCAN_SIGNAL_PORT_RCC		RCC_AHB1Periph_GPIOB
#define SCAN_SIGNAL_PIN				GPIO_Pin_5
#define SCAN_START()          		GPIO_ResetBits(SCAN_SIGNAL_PORT, SCAN_SIGNAL_PIN)
#define SCAN_STOP()           		GPIO_SetBits(SCAN_SIGNAL_PORT, SCAN_SIGNAL_PIN)	

//关闭电源时要把其他引脚拉低或者配置为输入，不然会有电流倒灌
//#define CLOSE_SCAN()                CLOSE_SCAN_POWER();\
//                                    SCAN_START()
#define CLOSE_SCAN()				SCAN_STOP();			
							
#define OPEN_SCAN()                SCAN_STOP();\
							       OPEN_SCAN_POWER() 

#define SCAN_DATA_MAX_LEN   128
typedef struct
{
	u8 Bytes;
	u8 Buffer[SCAN_DATA_MAX_LEN];
}SCAN_DATA;
extern SCAN_DATA  ScanData;

extern void ScanGpioPinConfig(void);
extern void ScanSendData(u8* ptr, u16 bytes);
extern u8 ScanHardwareState(void);
extern void ScanWork(u32 timer_out, u8 IsZEBRA);
#endif
