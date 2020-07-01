#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "LED.h"

int main(void)
{
	delay_init();
	LED_Init();
	while(1)
		{
		LED1=0;
		LED3=1;
		LED4=1;
        //OUT=0;
		delay_ms(300);
		LED1=1;
		LED3=0;
		LED4=1;
        //OUT=1;
		delay_ms(300);
		LED1=1;
		LED3=1;
		LED4=0;
        //OUT=0;
		delay_ms(300);
        //OUT=0;
        //delay_ms(14000);
            
		}
}

