#include <stm32f10x.h>
#include "includes.h"




int main(void) 
{

//    I2C_GPIO_Config();    
//    LcdInit(); 
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
    }
  
//    while(1)
//    {

//    OSTimeDlyHMSM(0,0,0,100);
//    //LED1_OFF();
////    OSTimeDlyHMSM(0,0,0,400);
////    LcdShowStr2Center("öï³Ìµç×Ó ","V1.0");
//    }

	
}



