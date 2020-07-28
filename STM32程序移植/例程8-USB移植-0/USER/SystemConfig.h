/**
  *************************************************************************************
  * @file    SystemConfig.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为SystemConfig.c的头文件
  *************************************************************************************
  */
#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SystemConfig
  *@{
  */
/**@defgroup SystemConfig_PinDefines
  *@brief 引脚定义
  *@{
  */


#define TEST_POINT1_PORT                  GPIOC
#define TEST_POINT1_PORT_RCC              RCC_APB2Periph_GPIOC
#define TEST_POINT1_PIN                   GPIO_Pin_4
#define SET_TEST_POINT1_PIN_L()      	  GPIO_ResetBits(TEST_POINT1_PORT, TEST_POINT1_PIN)
#define SET_TEST_POINT1_PIN_H()           GPIO_SetBits(TEST_POINT1_PORT, TEST_POINT1_PIN)
#define SET_TEST_POINT1_PIN_TOGGLE()      (TEST_POINT1_PORT->ODR ^=TEST_POINT1_PIN)

#define TEST_POINT2_PORT                  GPIOC
#define TEST_POINT2_PORT_RCC              RCC_APB2Periph_GPIOC
#define TEST_POINT2_PIN                   GPIO_Pin_5
#define SET_TEST_POINT2_PIN_L()      	  GPIO_ResetBits(TEST_POINT2_PORT, TEST_POINT2_PIN)
#define SET_TEST_POINT2_PIN_H()           GPIO_SetBits(TEST_POINT2_PORT, TEST_POINT2_PIN)
#define SET_TEST_POINT2_PIN_TOGGLE()      (TEST_POINT2_PORT->ODR ^=TEST_POINT2_PIN)

#define CHARGE_STA_PORT       			  GPIOD
#define CHARGE_STA_PORT_RCC    			  RCC_APB2Periph_GPIOD
#define CHARGE_STA_PIN         		      GPIO_Pin_2
#define CHARGE_STA()					  GPIO_ReadInputDataBit(CHARGE_STA_PORT,CHARGE_STA_PIN)

#define SYS_PW_PORT              		  GPIOB
#define SYS_PW_PORT_RCC           		  RCC_APB2Periph_GPIOB
#define SYS_PW_PIN             			  GPIO_Pin_3
#define SYS_PW_OFF()      	  			  GPIO_ResetBits(SYS_PW_PORT, SYS_PW_PIN)
#define SYS_PW_ON()                		  GPIO_SetBits(SYS_PW_PORT, SYS_PW_PIN)

#define SYS_PW_KEY_PORT       			  GPIOB
#define SYS_PW_KEY_PORT_RCC    			  RCC_APB2Periph_GPIOB
#define SYS_PW_KEY_PIN         		      GPIO_Pin_4
#define SYS_PW_KEY_STA()				  GPIO_ReadInputDataBit(SYS_PW_KEY_PORT,SYS_PW_KEY_PIN)
#define SYS_PW_KEY_EXTI_SOURCE	    	  GPIO_PortSourceGPIOB
#define SYS_PW_KEY_EXTI_SOURCE_PIN		  GPIO_PinSource4
#define SYS_PW_KEY_EXTI_LINE			  EXTI_Line4

#define RF_SX1278_RESET_PORT              GPIOA
#define RF_SX1278_RESET_RCC           	  RCC_APB2Periph_GPIOA
#define RF_SX1278_RESET_IO             	  GPIO_Pin_3
#define RF_SX1278_RESET_L()      	  	  GPIO_ResetBits(RF_SX1278_RESET_PORT, RF_SX1278_RESET_IO)
#define RF_SX1278_RESET_H()               GPIO_SetBits(RF_SX1278_RESET_PORT, RF_SX1278_RESET_IO)

#define RF_SX1278_DIO0_PORT      		  GPIOB
#define RF_SX1278_DIO0_PORT_RCC    		  RCC_APB2Periph_GPIOB
#define RF_SX1278_DIO0_PIN         		  GPIO_Pin_10
#define RF_SX1278_DIO0_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO0_PORT,RF_SX1278_DIO0_PIN)

#define RF_SX1278_DIO1_PORT      		  GPIOB
#define RF_SX1278_DIO1_PORT_RCC    		  RCC_APB2Periph_GPIOB
#define RF_SX1278_DIO1_PIN         		  GPIO_Pin_1
#define RF_SX1278_DIO1_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO1_PORT,RF_SX1278_DIO1_PIN)

#define RF_SX1278_DIO2_PORT      		  GPIOB
#define RF_SX1278_DIO2_PORT_RCC    		  RCC_APB2Periph_GPIOB
#define RF_SX1278_DIO2_PIN         		  GPIO_Pin_0
#define RF_SX1278_DIO2_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO2_PORT,RF_SX1278_DIO2_PIN)

#define RF_SX1278_DIO3_PORT      		  GPIOC
#define RF_SX1278_DIO3_PORT_RCC    		  RCC_APB2Periph_GPIOC
#define RF_SX1278_DIO3_PIN         		  GPIO_Pin_4
#define RF_SX1278_DIO3_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO3_PORT,RF_SX1278_DIO3_PIN)

#define RF_SX1278_DIO4_PORT      		  GPIOC
#define RF_SX1278_DIO4_PORT_RCC    		  RCC_APB2Periph_GPIOC
#define RF_SX1278_DIO4_PIN         		  GPIO_Pin_5
#define RF_SX1278_DIO4_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO4_PORT,RF_SX1278_DIO4_PIN)

#define RF_SX1278_DIO5_PORT      		  GPIOC
#define RF_SX1278_DIO5_PORT_RCC    		  RCC_APB2Periph_GPIOC
#define RF_SX1278_DIO5_PIN         		  GPIO_Pin_6
#define RF_SX1278_DIO5_STA()			  GPIO_ReadInputDataBit(RF_SX1278_DIO5_PORT,RF_SX1278_DIO5_PIN)
/**
 *@} SystemConfig_PinDefines
 */
/**
 *@} SystemConfig
 */
/**
 *@} PowerBoard
 */ 
extern void SystemGpioConfig(void);
extern ErrorStatus SystemHardwareInit(void);
extern void SystemPoint2Config(void);


#endif
