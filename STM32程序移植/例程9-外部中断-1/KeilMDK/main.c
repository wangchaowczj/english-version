#include "includes.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    ExtiConfig();
    LEDConfig();
    LED3_ON();
    LcdInit();
    I2C_GPIO_Config(); 
    while(1)
	{
	System72MDelay1ms(1000);
	}
}

