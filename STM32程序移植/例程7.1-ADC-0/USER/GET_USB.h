#ifndef _GET_USB_
#define _GET_USB_

#define GET_USB_PORT             GPIOC
#define GET_USB_PORT_RCC         RCC_APB2Periph_GPIOC
#define GET_USB_PIN              GPIO_Pin_11


extern void GET_USB_CONFIG(void);
#endif