#ifndef _GPS_H_
#define _GPS_H_

//GPS对应的串口
#define GPS_COM					USART6					//串口3
#define GPS_COM_RCC				RCC_APB2Periph_USART6	//串口3时钟
#define GPS_COM_PORT			GPIOG					//PG
#define GPS_COM_PORT_RCC		RCC_AHB1Periph_GPIOG	//PB时钟
#define GPS_COM_RX_PIN			GPIO_Pin_9				//输入脚为PG9
#define GPS_COM_RX_PINSOURCE	GPIO_PinSource9
#define GPS_COM_TX_PIN			GPIO_Pin_14				//输出脚为PG14
#define GPS_COM_TX_PINSOURCE	GPIO_PinSource14
#define GPS_COM_BAUD            9600

//GPS对应的IO控制口(输出)
#define GPS_PPS_PORT_RCC		RCC_AHB1Periph_GPIOG
#define GPS_PPS_PORT			GPIOG
#define GPS_PPS_PIN				GPIO_Pin_13
#define GET_GPS_LED_STATE()		GPIO_ResetBits(GPS_PPS_PORT, GPS_PPS_PIN)

#define GPS_OFF_PORT_RCC        RCC_AHB1Periph_GPIOF
#define GPS_OFF_PORT		    GPIOF
#define GPS_OFF_PIN		        GPIO_Pin_2
#define GPS_OFF_OFF()	        GPIO_ResetBits(GPS_OFF_PORT, GPS_OFF_PIN)
#define GPS_OFF_ON()		    GPIO_SetBits(GPS_OFF_PORT, GPS_OFF_PIN)	

#define GPS_RESET_PORT_RCC		RCC_AHB1Periph_GPIOB
#define GPS_RESET_PORT			GPIOB
#define GPS_RESET_PIN			GPIO_Pin_15
#define GPS_RESET_H()			GPIO_SetBits(GPS_RESET_PORT, GPS_RESET_PIN)		
#define GPS_RESET_L()			GPIO_ResetBits(GPS_RESET_PORT, GPS_RESET_PIN)	


#define GPS_PWR_EN_PORT_RCC		RCC_AHB1Periph_GPIOB
#define GPS_PWR_EN_PORT			GPIOB
#define GPS_PWR_EN_PIN			GPIO_Pin_12

#define GPS_PWR_EN_OFF()		GPIO_SetBits(GPS_PWR_EN_PORT, GPS_PWR_EN_PIN)		//关闭GPS电源
#define GPS_PWR_EN_ON()			GPIO_ResetBits(GPS_PWR_EN_PORT, GPS_PWR_EN_PIN)	//打开GPS电源
#define GET_GPS_POWER_STATE()      GPIO_ReadOutputDataBit(GPS_PWR_EN_PORT, GPS_PWR_EN_PIN)

typedef struct
{
	uint8_t Longitude[21];		//经度,经度格式为"11620.10921E"
	uint8_t Latitude[20];		//纬度,格式为"3957.78001N"
	uint16_t Year;			//年
	uint8_t Month;			//月
	uint8_t Day;			//日
	uint8_t Hour;			//时
	uint8_t Minute;			//分
	uint8_t Second;			//秒
}GPS_INFO;

#define GPS_BUFFER_SIZE   128
typedef enum 
{
    GPS_RECEIVING = 0x00,
    GPS_WAIT_ANALYZE,
    GPS_ANALYZING
}GPS_BUFFER_STATUS;
typedef struct
{
    GPS_BUFFER_STATUS Status;
	u8 Buffer[GPS_BUFFER_SIZE];
	u16 Bytes;    
}GPS_BUFFER;


extern GPS_INFO GPS_Info;
extern GPS_BUFFER GPS_Buffer;

extern void GPSConfig(void);
extern void GPSUartCallback(void);
extern void ReceiveGPSInfoStart(void);
extern void ReceiveGPSInfoStop(void);
extern u8 GetSatelliteCount(void);
extern u8 GetCurrentGPSInfo(GPS_INFO* Ptr);
extern u8 GetLastGPSInfo(GPS_INFO* Ptr);
extern u8 GPSInfoIsPass(GPS_INFO* Ptr);
extern double GPSFormatConvert(u8* gpsFormat, u8 Bytes);
extern double GetDistance(double longitude1, double latitude1, double longitude2, double latitude2);
extern u8 GPSIsWorking(void);
extern ErrorStatus RMC_FormatAnalyze(void);
extern u8 GetGPSPassCount(void);
extern void SetGPSPassCount(u8 Count);

#endif
