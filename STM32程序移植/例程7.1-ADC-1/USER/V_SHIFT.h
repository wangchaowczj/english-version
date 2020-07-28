#ifndef _V_SHIFT_
#define _V_SHIFT_

#define VSHIFT_PORT             GPIOC
#define VSHIFT_PORT_RCC         RCC_APB2Periph_GPIOC
#define VSHIFT_PIN              GPIO_Pin_9
#define VSHIFT_L() GPIO_ResetBits(VSHIFT_PORT,VSHIFT_PIN) 
#define VSHIFT_H() GPIO_SetBits(VSHIFT_PORT,VSHIFT_PIN)  

#define SW5V_PORT             GPIOA
#define SW5V_PORT_RCC         RCC_APB2Periph_GPIOA
#define SW5V_PIN              GPIO_Pin_6
#define SW5V_L() GPIO_ResetBits(SW5V_PORT,SW5V_PIN) //I_TX«Â¡„
#define SW5V_H() GPIO_SetBits(SW5V_PORT,SW5V_PIN)  //I_TX÷√1

extern void VSHIFT_CONFIG(void);

#endif

