#include <stm32f10x.h>
#include "includes.h"




int main(void) 
{

    
    LcdInit();
    I2C_GPIO_Config();     
    LEDConfig();
    GET_USB_CONFIG();
//    ITX_OFF();
//    VSHIFT_ON();
    
    
    while(1)
    {
        

        LED1_ON(); 
        System72MDelay1ms(200);
        LED1_OFF(); 
        System72MDelay1ms(200);
        
        if(USB_ST==1)
        {
        LED4_ON();
        LED3_OFF();
        }
        else
        {
        LED3_ON();
        LED4_OFF();
        }
        
        
    }	
}



