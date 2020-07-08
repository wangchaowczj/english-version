/**
  *************************************************************************************
  * @file    Uart.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为Uart.c的头文件
  *************************************************************************************
  */
#ifndef _UART_H_
#define _UART_H_



//主机485配置
#define RS485_UART_DIR_PORT          GPIOA
#define RS485_UART_DIR_PORT_RCC      RCC_APB2Periph_GPIOA
#define RS485_UART_DIR_PIN           GPIO_Pin_8
#define SET_RS485_UART_R()           GPIO_ResetBits(RS485_UART_DIR_PORT, RS485_UART_DIR_PIN) 
#define SET_RS485_UART_T()           GPIO_SetBits(RS485_UART_DIR_PORT, RS485_UART_DIR_PIN)

#define UART_TX_PORT                 GPIOA
#define UART_TX_PORT_RCC             RCC_APB2Periph_GPIOA
#define UART_TX_PIN                  GPIO_Pin_9
#define UART_TX_H()          		 GPIO_SetBits(UART_TX_PORT, UART_TX_PIN)

#define UART_RX_PORT                 GPIOA
#define UART_RX_PORT_RCC             RCC_APB2Periph_GPIOA
#define UART_RX_PIN                  GPIO_Pin_10

#define RS485_UART_SW_PORT           GPIOA
#define RS485_UART_SW_PORT_RCC       RCC_APB2Periph_GPIOA
#define RS485_UART_SW_PIN            GPIO_Pin_12
#define SET_RS485_SW_L()             GPIO_ResetBits(RS485_UART_SW_PORT, RS485_UART_SW_PIN) 
#define SET_RS485_SW_H()             GPIO_SetBits(RS485_UART_SW_PORT, RS485_UART_SW_PIN)

#define RS485_UART_S_PORT            GPIOA
#define RS485_UART_S_PORT_RCC        RCC_APB2Periph_GPIOA
#define RS485_UART_S_PIN             GPIO_Pin_11
#define READ_RS485_UART_S()          GPIO_ReadInputDataBit(RS485_UART_S_PORT,RS485_UART_S_PIN)
//从机485配置
#define RS4852_UART_DIR_PORT         GPIOA
#define RS4852_UART_DIR_PORT_RCC     RCC_APB2Periph_GPIOA
#define RS4852_UART_DIR_PIN          GPIO_Pin_1
#define SET_RS4852_UART_R()          GPIO_ResetBits(RS4852_UART_DIR_PORT, RS4852_UART_DIR_PIN) 
#define SET_RS4852_UART_T()          GPIO_SetBits(RS4852_UART_DIR_PORT, RS4852_UART_DIR_PIN)

#define UART2_TX_PORT                GPIOA
#define UART2_TX_PORT_RCC            RCC_APB2Periph_GPIOA
#define UART2_TX_PIN                 GPIO_Pin_2

#define UART2_RX_PORT                GPIOA
#define UART2_RX_PORT_RCC            RCC_APB2Periph_GPIOA
#define UART2_RX_PIN                 GPIO_Pin_3
#define READ_UART2_STA()             GPIO_ReadInputDataBit(UART2_RX_PORT,UART2_RX_PIN)

#define RS4852_UART_SW_PORT          GPIOA
#define RS4852_UART_SW_PORT_RCC      RCC_APB2Periph_GPIOA
#define RS4852_UART_SW_PIN           GPIO_Pin_5
#define SET_RS4852_SW_L()            GPIO_ResetBits(RS4852_UART_SW_PORT, RS4852_UART_SW_PIN) 
#define SET_RS4852_SW_H()            GPIO_SetBits(RS4852_UART_SW_PORT, RS4852_UART_SW_PIN)

#define RS4852_UART_S_PORT           GPIOA
#define RS4852_UART_S_PORT_RCC       RCC_APB2Periph_GPIOA
#define RS4852_UART_S_PIN            GPIO_Pin_4
#define READ_RS4852_UART_S()         GPIO_ReadInputDataBit(RS4852_UART_S_PORT,RS4852_UART_S_PIN)


extern void UartConfig(void);
extern void UartSendData(u8* dat,u16 len);
extern void Uart2SendData(u8* dat,u16 len);

#endif




