#ifndef _SCAN_H_
#define _SCAN_H_

#define SCAN_COM					USART3					//����2
#define SCAN_COM_RCC				RCC_APB1Periph_USART3	//����2ʱ��
#define SCAN_COM_PORT				GPIOB					//PA
#define SCAN_COM_PORT_RCC			RCC_AHB1Periph_GPIOB	//PBʱ��
#define SCAN_COM_RX_PIN				GPIO_Pin_11				//�����ΪPB11
#define SCAN_COM_RX_PINSOURCE		GPIO_PinSource11
#define SCAN_COM_TX_PIN				GPIO_Pin_10				//�����ΪPB10
#define SCAN_COM_TX_PINSOURCE		GPIO_PinSource10
#define SCAN_COM_BAUD            	9600

//�ӵ�Դ���������������ʱ��С��200ms
//�ڹرյ�Դ�����,�ڹرյ�Դ�����,������500ms���ϲ������ٴο�����Դ���롣

#define SCAN_POWER_PORT				GPIOB
#define SCAN_POWER_PORT_RCC			RCC_AHB1Periph_GPIOB
#define SCAN_POWER_PIN				GPIO_Pin_4
#define CLOSE_SCAN_POWER()          GPIO_SetBits(SCAN_POWER_PORT, SCAN_POWER_PIN)	
#define OPEN_SCAN_POWER()           GPIO_ResetBits(SCAN_POWER_PORT, SCAN_POWER_PIN)
#define GET_SCAN_POWER_STATE()      GPIO_ReadOutputDataBit(SCAN_POWER_PORT, SCAN_POWER_PIN)

//�����������źţ����ֵ͵�ƽ10ms ���Ͽɴ�������
//ʶ��������Ҫ�����ɼ�ͼ������ʶ��ͽ���Ȳ��裬�������δ����źŵļ��ʱ�䲻����50ms
#define SCAN_SIGNAL_PORT			GPIOB
#define SCAN_SIGNAL_PORT_RCC		RCC_AHB1Periph_GPIOB
#define SCAN_SIGNAL_PIN				GPIO_Pin_5
#define SCAN_START()          		GPIO_ResetBits(SCAN_SIGNAL_PORT, SCAN_SIGNAL_PIN)
#define SCAN_STOP()           		GPIO_SetBits(SCAN_SIGNAL_PORT, SCAN_SIGNAL_PIN)	

//�رյ�ԴʱҪ�������������ͻ�������Ϊ���룬��Ȼ���е�������
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
