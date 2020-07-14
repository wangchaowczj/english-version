/**
  *************************************************************************************
  * @file    SystemConfig.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ���ļ�ΪSystemConfig.c��ͷ�ļ�
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
  *@brief ���Ŷ���
  *@{
  */
#define LED1_PORT           			GPIOE
#define LED1_PORT_RCC      				RCC_AHB1Periph_GPIOE
#define LED1_PIN            			GPIO_Pin_2
#define OPEN_LED1()      				GPIO_ResetBits(LED1_PORT, LED1_PIN)
#define CLOSE_LED1()           			GPIO_SetBits(LED1_PORT, LED1_PIN)
#define LED1_TOGGLE()      				(LED1_PORT->ODR ^=LED1_PIN)

#define LED2_PORT           			GPIOE
#define LED2_PORT_RCC      				RCC_AHB1Periph_GPIOE
#define LED2_PIN            			GPIO_Pin_3
#define OPEN_LED2()      				GPIO_ResetBits(LED2_PORT, LED2_PIN)
#define CLOSE_LED2()           			GPIO_SetBits(LED2_PORT, LED2_PIN)
#define LED2_TOGGLE()      				(LED2_PORT->ODR ^=LED2_PIN)

//������Ϣ�����ƿ�ʼ��⣬����ģʽʱ�����жϷ�ʽ��������ģʽʱ��������ͨ��̤����ʹ��
#define MCU_IO_IN1_PORT                 GPIOF
#define MCU_IO_IN1_PORT_RCC             RCC_AHB1Periph_GPIOF
#define MCU_IO_IN1_PIN                  GPIO_Pin_1
#define GET_MCU_IO_IN1_PIN()            GPIO_ReadInputDataBit(MCU_IO_IN1_PORT, MCU_IO_IN1_PIN)
#define MCU_IO_IN1_EXTI_LINE			EXTI_Line1

#define MCU_IO_IN2_PORT                 GPIOF
#define MCU_IO_IN2_PORT_RCC             RCC_AHB1Periph_GPIOF
#define MCU_IO_IN2_PIN                  GPIO_Pin_2
#define GET_MCU_IO_IN2_PIN()            GPIO_ReadInputDataBit(MCU_IO_IN2_PORT, MCU_IO_IN2_PIN)
#define MCU_IO_IN2_EXTI_LINE			EXTI_Line2
  
//����͵�ƽʱJ24��2��3�������ߵ�ƽʱJ24��2��1����										
#define IO_RELAY1_PORT           		GPIOG
#define IO_RELAY1_PORT_RCC      		RCC_AHB1Periph_GPIOG
#define IO_RELAY1_PIN            		GPIO_Pin_0
#define IO_RELAY1_L()      			    GPIO_ResetBits(IO_RELAY1_PORT, IO_RELAY1_PIN)
#define IO_RELAY1_H()           		GPIO_SetBits(IO_RELAY1_PORT, IO_RELAY1_PIN)
#define IO_RELAY1_TOGGLE()      		(IO_RELAY1_PORT->ODR ^=IO_RELAY1_PIN)
#define OPEN_IO_RELAY1()                IO_RELAY1_H()
#define CLOSE_IO_RELAY1()               IO_RELAY1_L()

//����͵�ƽʱJ24��5��6�������ߵ�ƽʱJ24��5��4����										
#define IO_RELAY2_PORT           		GPIOG
#define IO_RELAY2_PORT_RCC      		RCC_AHB1Periph_GPIOG
#define IO_RELAY2_PIN            		GPIO_Pin_1
#define IO_RELAY2_L()      			    GPIO_ResetBits(IO_RELAY2_PORT, IO_RELAY2_PIN)
#define IO_RELAY2_H()           		GPIO_SetBits(IO_RELAY2_PORT, IO_RELAY2_PIN)
#define IO_RELAY2_TOGGLE()      		(IO_RELAY2_PORT->ODR ^=IO_RELAY2_PIN)
#define OPEN_IO_RELAY2()               IO_RELAY2_H()
#define CLOSE_IO_RELAY2()               IO_RELAY2_L()

/**
 *@} SystemConfig_PinDefines
 */
/**
 *@} SystemConfig
 */
/**
 *@} PowerBoard
 */ 
extern void OpenAllPortClock(void);
extern void SystemGpioConfig(void);
extern u8 SystemHardwareInit(void);

#endif
