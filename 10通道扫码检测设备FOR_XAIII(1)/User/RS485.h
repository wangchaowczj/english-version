#ifndef _RS485_H_
#define _RS485_H_

#define RS485_COMM             USART3
#define RS485_COMM_AF          GPIO_AF_USART3
#define RS485_COMM_PORT        GPIOB
#define RS485_COMM_RCC		   RCC_APB1Periph_USART3	//´®¿Ú3Ê±ÖÓ
#define RS485_COMM_PORT_RCC    RCC_AHB1Periph_GPIOB
#define RS485_COMM_TX_PIN      GPIO_Pin_10
#define RS485_COMM_RX_PIN      GPIO_Pin_11
#define RS485_COMM_BAUD        9600


#define RS485_DIR_PORT         GPIOD
#define RS485_DIR_PORT_RCC     RCC_AHB1Periph_GPIOD
#define RS485_DIR_PIN          GPIO_Pin_2 

#define SET_RS485_SEND()	       GPIO_SetBits(RS485_DIR_PORT, RS485_DIR_PIN)
#define SET_RS485_RECEIVE()	       GPIO_ResetBits(RS485_DIR_PORT, RS485_DIR_PIN)

extern STR_COMM  RS485Data ;

extern void RS485Config(void);
extern void RS485SendData(u8* ptr, u16 bytes);
#endif
