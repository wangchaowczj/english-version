#include "includes.h"

void LEDConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC, ENABLE);	
	

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = LED1_PIN;	
	GPIO_Init(LED1_PORT, &GPIO_InitStruct);
		
	
	LED1_OFF();	
}



