#include "includes.h"

void GET_USB_CONFIG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(GET_USB_PORT_RCC , ENABLE);
		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //
	GPIO_InitStructure.GPIO_Pin = GET_USB_PIN; 
	GPIO_Init(GET_USB_PORT, &GPIO_InitStructure);
}