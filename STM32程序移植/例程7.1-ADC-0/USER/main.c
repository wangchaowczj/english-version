#include <stm32f10x.h>
#include "includes.h"




int main(void) 
{
    u8 adc_LINEA,adc_LINEB;
    float V_LINEA,V_LINEB;
    
    static char Temp1[10],Temp2[5];
    LcdInit();
    I2C_GPIO_Config();    
    LcdInit(); 
    LEDConfig();
    AdcConfig();
    ITX_CONFIG();
    VSHIFT_CONFIG();
//    ITX_OFF();
//    VSHIFT_ON();
    
    
    while(1)
    {
        
        adc_LINEA = GetAdcMeasureValue(ADC_LINEA_CHANNEL,10);
        adc_LINEB = GetAdcMeasureValue(ADC_LINEB_CHANNEL,10);
        
        V_LINEA = (float)adc_LINEA*(3.3/250)*5;
        V_LINEB = (float)adc_LINEB*(3.3/250)*5;
        sprintf(Temp1,"A%.1fVB%.1fV",V_LINEA,V_LINEB);
        //sprintf(Temp2,"LINEB:%.1fV",V_LINEB);
        
        LED1_ON(); 
        System72MDelay1ms(200);
        LED1_OFF(); 
        System72MDelay1ms(200);
        LcdShowStr2Center(Temp1,"NULL");
        
//        ITX_OFF();
//        System72MDelay1ms(800);
//        System72MDelay1ms(800);
//        LED3_ON(); 
//        System72MDelay1ms(300);
//        LED3_OFF(); 
//        System72MDelay1ms(300);
//        
//        LED4_ON(); 
//        System72MDelay1ms(300);
//        LED4_OFF(); 
//        System72MDelay1ms(300);
    }
  
//    while(1)
//    {

//    OSTimeDlyHMSM(0,0,0,100);
//    //LED1_OFF();
////    OSTimeDlyHMSM(0,0,0,400);
////    LcdShowStr2Center("öï³Ìµç×Ó ","V1.0");
//    }

	
}



