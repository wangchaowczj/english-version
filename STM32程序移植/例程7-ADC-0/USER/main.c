#include "includes.h"
#include <stm32f10x.h>

int main(void)
{
u8 adcx;
static char chrl[5];
//    float temp;
    
	LEDConfig();
    I2C_GPIO_Config();
    LcdInit();
    AdcConfig();
    
      
	while(1)
		{

        adcx = GetAdcMeasureValue(ADC_5V_CHANNEL,5);
        sprintf(chrl,"%d uA",adcx);  
            
        LED1_ON();
        LcdShowStr2Center("LINEA V:",chrl);            
        System72MDelay1ms(200);
        LED1_OFF(); 
        System72MDelay1ms(200);        
//        LED3_ON(); 
//        LcdShowStr2Center("LINEA V:",chrl);
//        System72MDelay1ms(200);
//        LED3_OFF(); 
//        System72MDelay1ms(200);        
//        LED4_ON(); 
//        LcdShowStr2Center("LINEA V:",chrl);
//        System72MDelay1ms(200);
//        LED4_OFF(); 
//        System72MDelay1ms(200);
        
        
        

            
		}
}

