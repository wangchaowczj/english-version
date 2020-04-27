#ifndef _RS485_H_
#define _RS485_H_

#define RS485_COMM             USART1
#define RS485_COMM_AF          GPIO_AF_USART1
#define RS485_COMM_PORT        GPIOB
#define RS485_COMM_RCC		   RCC_APB2Periph_USART1	//串口1时钟
#define RS485_COMM_PORT_RCC    RCC_AHB1Periph_GPIOB
#define RS485_COMM_TX_PIN      GPIO_Pin_6
#define RS485_COMM_RX_PIN      GPIO_Pin_7
#define RS485_COMM_BAUD        9600


#define RS485_COMM_TX_PIN_LOW()             GPIO_ResetBits(RS485_COMM_PORT, RS485_COMM_TX_PIN)
#define RS485_COMM_TX_PIN_HIGH()            GPIO_SetBits(RS485_COMM_PORT, RS485_COMM_TX_PIN)
#define RS485_COMM_TX_PIN_TOGGLE()          GPIO_ToggleBits(RS485_COMM_PORT, RS485_COMM_TX_PIN)

#define RS485_CONNECT_PORT     GPIOB
#define RS485_CONNECT_PORT_RCC RCC_AHB1Periph_GPIOB
#define RS485_CONNECT_PIN      GPIO_Pin_9

#define RS485_STATE_PORT       GPIOA
#define RS485_STATE_PORT_RCC   RCC_AHB1Periph_GPIOA
#define RS485_STATE_PIN        GPIO_Pin_8 

#define RS485_DIR_PORT         GPIOF
#define RS485_DIR_PORT_RCC     RCC_AHB1Periph_GPIOF
#define RS485_DIR_PIN          GPIO_Pin_11 

#define CONNECT_RS485_IS_NSTD       0
#define CONNECT_RS485_TIME         5000

#define GET_RS485_CONNECT_STATE()  GPIO_ReadInputDataBit(RS485_STATE_PORT, RS485_STATE_PIN)
#define SET_RS485_CONNECT_STD()    GPIO_ResetBits(RS485_CONNECT_PORT, RS485_CONNECT_PIN)
#define SET_RS485_CONNECT_NSTD()   GPIO_SetBits(RS485_CONNECT_PORT, RS485_CONNECT_PIN)

#define SET_RS485_SEND()	       GPIO_SetBits(RS485_DIR_PORT, RS485_DIR_PIN)
#define SET_RS485_RECEIVE()	       GPIO_ResetBits(RS485_DIR_PORT, RS485_DIR_PIN)
#define RS485_IS_RECEIVE       0
#define GET_RS485_SEND_OR_RECEIVE_STATUS()	       GPIO_ReadOutputDataBit(RS485_DIR_PORT, RS485_DIR_PIN)

typedef enum
{
	RS485_SLAVE=0,   //从机
	RS485_MASTER   //主机  
}RS485_MASTER_SLAVE;

extern STR_COMM  RS485Data ;

extern void RS485Config(void);
extern void RS485TxPinExitUartConfig(void);
extern void SetRS485Master(void);
extern void SetRS485Slave(void);
extern RS485_MASTER_SLAVE GetRS485MasterSlaveState(void);
extern void RS485DirCountDecrement(void);
extern void ResetRS485DirCount(void);
extern void RS485SendData(u8* ptr, u16 bytes);
#endif
