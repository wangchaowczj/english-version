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
#define LED1_GREEN_PORT                  GPIOE
#define LED1_GREEN_PORT_RCC              RCC_AHB1Periph_GPIOE
#define LED1_GREEN_PIN                   GPIO_Pin_6
#define OPEN_LED1_GREEN()      	         GPIO_ResetBits(LED1_GREEN_PORT, LED1_GREEN_PIN)
#define CLOSE_LED1_GREEN()               GPIO_SetBits(LED1_GREEN_PORT, LED1_GREEN_PIN)
#define SET_LED1_GREEN_PIN_TOGGLE()      (LED1_GREEN_PORT->ODR ^=LED1_GREEN_PIN)

#define LED1_RED_PORT                   GPIOE
#define LED1_RED_PORT_RCC               RCC_AHB1Periph_GPIOE
#define LED1_RED_PIN                    GPIO_Pin_3
#define OPEN_LED1_RED()      	        GPIO_ResetBits(LED1_RED_PORT, LED1_RED_PIN)
#define CLOSE_LED1_RED()                GPIO_SetBits(LED1_RED_PORT, LED1_RED_PIN)
#define SET_LED1_RED_PIN_TOGGLE()       (LED1_RED_PORT->ODR ^=LED1_RED_PIN)

#define LED2_GREEN_PORT                  GPIOE
#define LED2_GREEN_PORT_RCC              RCC_AHB1Periph_GPIOE
#define LED2_GREEN_PIN                   GPIO_Pin_2
#define OPEN_LED2_GREEN()      	         GPIO_ResetBits(LED2_GREEN_PORT, LED2_GREEN_PIN)
#define CLOSE_LED2_GREEN()               GPIO_SetBits(LED2_GREEN_PORT, LED2_GREEN_PIN)
#define SET_LED2_GREEN_PIN_TOGGLE()      (LED2_GREEN_PORT->ODR ^=LED2_GREEN_PIN)

#define LED2_RED_PORT                   GPIOC
#define LED2_RED_PORT_RCC               RCC_AHB1Periph_GPIOC
#define LED2_RED_PIN                    GPIO_Pin_13
#define OPEN_LED2_RED()      	        GPIO_ResetBits(LED2_RED_PORT, LED2_RED_PIN)
#define CLOSE_LED2_RED()                GPIO_SetBits(LED2_RED_PORT, LED2_RED_PIN)
#define SET_LED2_RED_PIN_TOGGLE()       (LED2_RED_PORT->ODR ^=LED2_RED_PIN)

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
extern u8 SystemHardwareInit(void);
extern ErrorStatus FileSystemFormatAt45(void);
extern ErrorStatus FileSystemFormatSdCard(void);

#endif
