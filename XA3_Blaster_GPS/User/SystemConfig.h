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
#define LED1_PORT                  GPIOC
#define LED1_PORT_RCC              RCC_AHB1Periph_GPIOC
#define LED1_PIN                   GPIO_Pin_2
#define OPEN_LED1()      	       GPIO_ResetBits(LED1_PORT, LED1_PIN)
#define CLOSE_LED1()               GPIO_SetBits(LED1_PORT, LED1_PIN)
#define SET_LED1_PIN_TOGGLE()      (LED1_PORT->ODR ^=LED1_PIN)

#define LED2_PORT                  GPIOC
#define LED2_PORT_RCC              RCC_AHB1Periph_GPIOC
#define LED2_PIN                   GPIO_Pin_3
#define OPEN_LED2()      	       GPIO_ResetBits(LED2_PORT, LED2_PIN)
#define CLOSE_LED2()               GPIO_SetBits(LED2_PORT, LED2_PIN)
#define SET_LED2_PIN_TOGGLE()      (LED2_PORT->ODR ^=TEST_POINT1_PIN)

#define TRIGGER_SINGLE_PORT       	   GPIOG
#define TRIGGER_SINGLE_PORT_RCC    	   RCC_AHB1Periph_GPIOG
#define TRIGGER_SINGLE_PIN             GPIO_Pin_8
#define TRIGGER_SINGLE_PIN_L()         GPIO_ResetBits(TRIGGER_SINGLE_PORT, TRIGGER_SINGLE_PIN)
#define TRIGGER_SINGLET_PIN_H()        GPIO_SetBits(TRIGGER_SINGLE_PORT, TRIGGER_SINGLE_PIN)
#define TRIGGER_SINGLE_PIN_TOGGLE()    (TRIGGER_SINGLE_PORT->ODR ^=TRIGGER_SINGLE_PIN)

//震动模块
#define MOTO_PORT                  GPIOF
#define MOTO_PORT_RCC              RCC_AHB1Periph_GPIOF
#define MOTO_PIN                   GPIO_Pin_1
#define OPEN_MOTO()      	       GPIO_SetBits(MOTO_PORT, MOTO_PIN)
#define CLOSE_MOTO()               GPIO_ResetBits(MOTO_PORT, MOTO_PIN)
#define SET_MOTO_PIN_TOGGLE()      (MOTO_PORT->ODR ^=MOTO_PIN)


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
