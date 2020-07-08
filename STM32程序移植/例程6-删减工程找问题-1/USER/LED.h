#ifndef _LED_H_
#define _LED_H_


#define LED1_PORT          GPIOB
#define LED1_PORT_RCC      RCC_APB2Periph_GPIOB
#define LED1_PIN           GPIO_Pin_7
#define LED1_ON()          GPIO_ResetBits(LED1_PORT, LED1_PIN) 
#define LED1_OFF()         GPIO_SetBits(LED1_PORT, LED1_PIN)


void LEDConfig(void);

#endif













