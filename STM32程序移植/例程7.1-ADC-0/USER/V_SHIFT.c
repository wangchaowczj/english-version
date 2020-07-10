#include"includes.h"

void VSHIFT_CONFIG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(VSHIFT_PORT_RCC|SW5V_PORT_RCC , ENABLE);
		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //
	GPIO_InitStructure.GPIO_Pin = VSHIFT_PIN; 
	GPIO_Init(VSHIFT_PORT, &GPIO_InitStructure);
    VSHIFT_H();
    
    GPIO_InitStructure.GPIO_Pin = SW5V_PIN; 
    GPIO_Init(SW5V_PORT, &GPIO_InitStructure);
	SW5V_H();	
}

