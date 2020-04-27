#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#define BLUE_COM					UART4					//串口4
#define BLUE_COM_RCC				RCC_APB1Periph_UART4	//串口4时钟
#define BLUE_COM_PORT				GPIOA					//PA
#define BLUE_COM_PORT_RCC			RCC_AHB1Periph_GPIOA	//PB时钟
#define BLUE_COM_RX_PIN				GPIO_Pin_1				//输入脚为PA1
#define BLUE_COM_RX_PINSOURCE		GPIO_PinSource1
#define BLUE_COM_TX_PIN				GPIO_Pin_0				//输出脚为PA0
#define BLUE_COM_TX_PINSOURCE		GPIO_PinSource0
#define BLUE_COM_BAUD            	115200


#define BLUE_PW_PORT      					GPIOH
#define BLUE_PW_PORT_RCC  					RCC_AHB1Periph_GPIOH
#define BLUE_PW_PIN       					GPIO_Pin_2
#define OPEN_BLUE_POWER()     				GPIO_ResetBits(BLUE_PW_PORT, BLUE_PW_PIN)
#define CLOSE_BLUE_POWER()      			GPIO_SetBits(BLUE_PW_PORT, BLUE_PW_PIN) 
#define GET_BLUE_POWER_STATUS()      		GPIO_ReadOutputDataBit(BLUE_PW_PORT, BLUE_PW_PIN)

#define BLUE_AT_CMD_PORT      				GPIOB
#define BLUE_AT_CMD_PORT_RCC  				RCC_AHB1Periph_GPIOB
#define BLUE_AT_CMD_PIN       				GPIO_Pin_3
#define BLUE_ENTER_AT_CMD_MODE()     		GPIO_SetBits(BLUE_AT_CMD_PORT, BLUE_AT_CMD_PIN)
#define BLUE_EXIT_AT_CMD_MODE()      		GPIO_ResetBits(BLUE_AT_CMD_PORT, BLUE_AT_CMD_PIN) 
#define EXIT_AT_CMD_MODE_STATUS             0
#define GET_BLUE_AT_CMD_MODE_STATUS()      	GPIO_ReadOutputDataBit(BLUE_AT_CMD_PORT, BLUE_AT_CMD_PIN)

#define BLUE_CONNECT_STATUS_PORT       		GPIOH
#define BLUE_CONNECT_STATUS_PORT_RCC  		RCC_AHB1Periph_GPIOH
#define BLUE_CONNECT_STATUS_PIN       		GPIO_Pin_8
#define BLUE_NOT_CONNECT                    0
#define GET_BLUE_CONNECT_STATUS() 			GPIO_ReadInputDataBit(BLUE_CONNECT_STATUS_PORT, BLUE_CONNECT_STATUS_PIN)

typedef enum
{
	BLUE_UNKNOWN = 0,//状态未知
	BLUE_SLAVE,//从机状态
	BLUE_MASTER,//主机状态
	BLUE_ERROR,//异常
}BLUE_STATUS;

#define BLUE_SLAVE_LIST_MAX   8
typedef struct
{
	u8 Count;
	struct
	{
		u8 Name[21];
		u8 Addr[13];//每个蓝牙地址为12字节的字符串，多定义1个字节，方便使用
	}Info[BLUE_SLAVE_LIST_MAX];
}BLUE_SLAVE_LIST;
extern BLUE_SLAVE_LIST BlueSlaveList;

extern STR_COMM  BlueData;

extern void BluePinConfig(void);
extern void BlueSendData(u8* ptr, u16 bytes);
extern ErrorStatus BlueHostInit(void);
extern ErrorStatus BlueSlaveInit(void);
extern void BlueScanSlave(void);
extern ErrorStatus BlueConnectSlave(u8 Index);
extern ErrorStatus BlueHostDisConnect(void);
extern void SetBlueCurrentHostSlaveStatus(u8 Status);
extern u8 GetBlueCurrentHostSlaveStatus(void);
extern void BlueSlaveFilter(u8* Feature);
extern ErrorStatus GetBlueVersion(u8* Version);
extern BLUE_STATUS GetBlueStatus(void);

#endif
