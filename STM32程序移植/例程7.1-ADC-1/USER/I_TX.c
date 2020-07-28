#include "includes.h"

void ITX_CONFIG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ITX_PORT_RCC , ENABLE);
		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //
	GPIO_InitStructure.GPIO_Pin = ITX_PIN; 
	GPIO_Init(ITX_PORT, &GPIO_InitStructure);
	ITX_ON();		
}

