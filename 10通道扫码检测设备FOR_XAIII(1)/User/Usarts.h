#ifndef _USARTS_H_
#define _USARTS_H_

#define RS485_COMM             USART1
#define RS485_COMM_AF          GPIO_AF_USART1
#define RS485_COMM_PORT        GPIOB
#define RS485_COMM_RCC		   RCC_APB2Periph_USART1	//串口1时钟
#define RS485_COMM_PORT_RCC    RCC_AHB1Periph_GPIOB
#define RS485_COMM_TX_PIN      GPIO_Pin_6
#define RS485_COMM_RX_PIN      GPIO_Pin_7
#define RS485_COMM_BAUD        115200

#define RS485_DIR_PORT         GPIOB
#define RS485_DIR_PORT_RCC     RCC_AHB1Periph_GPIOB
#define RS485_DIR_PIN          GPIO_Pin_5 

#define SET_RS485_SEND()	       GPIO_SetBits(RS485_DIR_PORT, RS485_DIR_PIN)
#define SET_RS485_RECEIVE()	       GPIO_ResetBits(RS485_DIR_PORT, RS485_DIR_PIN)
#define RS485_IS_RECEIVE       0
#define GET_RS485_SEND_OR_RECEIVE_STATUS()	       GPIO_ReadOutputDataBit(RS485_DIR_PORT, RS485_DIR_PIN)

#define RS232_COMM             UART4
#define RS232_COMM_AF          GPIO_AF_UART4
#define RS232_COMM_PORT        GPIOC
#define RS232_COMM_RCC		   RCC_APB1Periph_UART4	//串口1时钟
#define RS232_COMM_PORT_RCC    RCC_AHB1Periph_GPIOC
#define RS232_COMM_TX_PIN      GPIO_Pin_10
#define RS232_COMM_RX_PIN      GPIO_Pin_11
#define RS232_COMM_BAUD        115200


extern void RS485Config(void);
extern void RS485SendData(u8* ptr, u16 bytes);
extern void RS232Config(void);
extern void RS232SendData(u8* ptr, u16 bytes);
extern void SetUartBaudRate(USART_TypeDef* USARTx, u32 baudRate);
#endif
