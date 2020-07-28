#ifndef _LED_H_
#define _LED_H_


#define LED1_PORT          GPIOC
#define LED1_PORT_RCC      RCC_APB2Periph_GPIOC
#define LED1_PIN           GPIO_Pin_5
#define LED1_ON()          GPIO_ResetBits(LED1_PORT, LED1_PIN) 
#define LED1_OFF()         GPIO_SetBits(LED1_PORT, LED1_PIN)


void LEDConfig(void);

#endif













