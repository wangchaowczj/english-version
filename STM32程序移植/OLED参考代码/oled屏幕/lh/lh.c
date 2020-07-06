#include "led.h"

void LED_Init()
{
	GPIO_InitTypeDef l;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_Init(GPIOA,&l);
	
}