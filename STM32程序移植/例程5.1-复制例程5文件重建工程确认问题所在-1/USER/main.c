#include "includes.h"
#include <stm32f10x.h>

int main(void)
{

	LEDConfig();
	while(1)
		{
        LED1_ON(); 
        System72MDelay1ms(300);
        LED1_OFF(); 
        System72MDelay1ms(300);
        
        LED3_ON(); 
        System72MDelay1ms(300);
        LED3_OFF(); 
        System72MDelay1ms(300);
        
        LED4_ON(); 
        System72MDelay1ms(300);
        LED4_OFF(); 
        System72MDelay1ms(300);
/*也可以这样写
    while(1)
	{
	GPIO_SetBits(GPIOB,GPIO_PIN_0);
	...
	}
*/
            
		}
}

