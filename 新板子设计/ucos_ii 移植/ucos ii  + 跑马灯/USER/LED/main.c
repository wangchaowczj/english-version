#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{
	delay_init();
	LED_Init();
	while(1)
		{
		LED0=0;
		LED3=1
		LED4=0;
		delay_ms(300);
		LED0=1;
		LED3=0
		LED4=1;
		delay_ms(300);
		LED0=1;
		LED3=1
		LED4=0;
		delay_ms(300);	
		}
}