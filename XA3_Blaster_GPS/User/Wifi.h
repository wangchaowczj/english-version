#ifndef _WIFI_H_
#define _WIFI_H_

#define WIFI_COM					USART2					//串口2
#define WIFI_COM_RCC				RCC_APB1Periph_USART2	//串口2时钟
#define WIFI_COM_PORT				GPIOA					//PA
#define WIFI_COM_PORT_RCC			RCC_AHB1Periph_GPIOA	//PB时钟
#define WIFI_COM_RX_PIN				GPIO_Pin_3				//输入脚为PA3
#define WIFI_COM_RX_PINSOURCE		GPIO_PinSource3
#define WIFI_COM_TX_PIN				GPIO_Pin_2				//输出脚为PA2
#define WIFI_COM_TX_PINSOURCE		GPIO_PinSource2
#define WIFI_COM_BAUD_DEFAULT       115200 //WIFI模块出厂默认波特率
#define WIFI_COM_BAUD_EXPECT        230400//实际使用的波特率

#define WIFI_POWER_PORT				GPIOH
#define WIFI_POWER_PORT_RCC			RCC_AHB1Periph_GPIOH
#define WIFI_POWER_PIN				GPIO_Pin_3
#define CLOSE_WIFI_POWER()          GPIO_SetBits(WIFI_POWER_PORT, WIFI_POWER_PIN)	
#define OPEN_WIFI_POWER()           GPIO_ResetBits(WIFI_POWER_PORT, WIFI_POWER_PIN)
#define GET_WIFI_POWER_STATE()      GPIO_ReadOutputDataBit(WIFI_POWER_PORT, WIFI_POWER_PIN)

#define WIFI_NRESET_PORT			GPIOH
#define WIFI_NRESET_PORT_RCC		RCC_AHB1Periph_GPIOH
#define WIFI_NRESET_PIN				GPIO_Pin_15
//拉低10MS以上可复位
#define WIFI_NRESET_PIN_H()         GPIO_SetBits(WIFI_NRESET_PORT, WIFI_NRESET_PIN)	
#define WIFI_NRESET_PIN_L()         GPIO_ResetBits(WIFI_NRESET_PORT, WIFI_NRESET_PIN)

#define WIFI_NLINK_PORT				GPIOH
#define WIFI_NLINK_PORT_RCC			RCC_AHB1Periph_GPIOH
#define WIFI_NLINK_PIN				GPIO_Pin_14
#define WIFI_CONNECT_AP             0//低电平表示已经连上路由
#define GET_WIFI_NLINK_STATE()      GPIO_ReadInputDataBit(WIFI_NLINK_PORT, WIFI_NLINK_PIN)

#define WIFI_NREADY_PORT			GPIOH
#define WIFI_NREADY_PORT_RCC		RCC_AHB1Periph_GPIOH
#define WIFI_NREADY_PIN				GPIO_Pin_13
#define WIFI_START_SUCCESS          0//低电平表示模块正常启动并工作在正常模式
#define GET_WIFI_NREADY_STATE()      GPIO_ReadInputDataBit(WIFI_NREADY_PORT, WIFI_NREADY_PIN)

#define WIFI_NRELOAD_PORT			GPIOH
#define WIFI_NRELOAD_PORT_RCC		RCC_AHB1Periph_GPIOH
#define WIFI_NRELOAD_PIN			GPIO_Pin_6
//拉低4秒以上则恢复到出厂设置
#define WIFI_NRELOAD_PIN_H()         GPIO_ResetBits(WIFI_NRELOAD_PORT, WIFI_NRELOAD_PIN)
#define WIFI_NRELOAD_PIN_L()         GPIO_SetBits(WIFI_NRELOAD_PORT, WIFI_NRELOAD_PIN)	

#define WIFI_BUF_SIZE   1024
typedef struct
{
	u8 Buffer[WIFI_BUF_SIZE];
	u16 Bytes;
}WIFI_BUF;

typedef struct
{
	u8 ssid[33];//网络名
	u8 Auth[10];//OPEN SHARED WPAPSK WPA2PSK
	u8 Encry[10];//NONE WEP-H WEP-A TKIP AES
} AP_SCAN_SUMMARY;

#define AP_SCAN_RESULT_MAX        7
typedef struct
{
	u8  ScanNumber;
	AP_SCAN_SUMMARY  ScanSum[AP_SCAN_RESULT_MAX];
}AP_SCAN_RESULT;
typedef struct
{
	u8 Ssid[50];
	u8 Passwords[50];
	u8 Auth[10];//OPEN SHARED WPAPSK WPA2PSK
	u8 Encry[10];//NONE WEP-H WEP-A TKIP AES	
	AP_SCAN_RESULT ScanResult;	
	WIFI_BUF RecData;
}MY_WIFI;
extern MY_WIFI MyWifi;

extern void WifiLock(void);
extern void WifiUnlock(void);
extern void WifiConfig(void);
extern u8 IPAddrU32ToString(u32 IpAddrU32, u8* IpAddrStr);
extern u32 IpAddrStringToU32(u8* IpAddrStr);
extern ErrorStatus WifiInit(void);
extern ErrorStatus GetWifiCurrentAPInfo(u8* SSID, u8* Passwords, u8* Auth, u8* Encry);
extern void WifiScanAP(AP_SCAN_RESULT* Result);
extern ErrorStatus WifiConnectAP(u8* SSID, u8* Passwords, u8* Auth, u8* Encry);
extern ErrorStatus CloseWifiSocket(void);
extern ErrorStatus OpenWifiSocket(u32 IP, u16 Port);
extern ErrorStatus WifiTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out);
extern ErrorStatus FindStringFromBuffer(u8*str,u16 str_bytes, u8*buf, u16 buf_bytes);
extern void AutoConnect(void);


#endif
