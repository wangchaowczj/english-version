#ifndef _ITX_H_
#define _ITX_H_

#define ITX_PORT             GPIOC
#define ITX_PORT_RCC         RCC_APB2Periph_GPIOC
#define ITX_PIN              GPIO_Pin_7
#define ITX_ON() GPIO_ResetBits(ITX_PORT,ITX_PIN) //I_TX«Â¡„
#define ITX_OFF() GPIO_SetBits(ITX_PORT,ITX_PIN)  //I_TX÷√1

extern void ITX_CONFIG(void);

#endif

