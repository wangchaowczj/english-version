#include <stm32f10x.h>
#include "includes.h"




int main(void) 
{
    u16 ADC_LINEA,ADC_LINEB,ADC_12V,ADC_I1,ADC_I2,ADC_COMOUT;
    float V_LINEA,V_LINEB,V_12V,V_I1,V_I2,V_COMOUT,I_I1,I_I2;
    
    static char Temp1[5];
    static char Temp2[5];
    static char Temp3[1];
    static char Temp4[1];
    
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_8);
    
    LcdInit();
    I2C_GPIO_Config();    
    LcdInit(); 
    LEDConfig();
    AdcConfig();
    ITX_CONFIG();
    VSHIFT_CONFIG();
    GET_USB_CONFIG();
//    ITX_OFF();
//    VSHIFT_ON();
    
    
    while(1)
    {
        
        ADC_LINEA = GetAdcMeasureValue(ADC_LINEA_CHANNEL,10);
        ADC_LINEB = GetAdcMeasureValue(ADC_LINEB_CHANNEL,10);
        ADC_12V = GetAdcMeasureValue(ADC_12V_CHANNEL,10);
        ADC_I1 = GetAdcMeasureValue(ADC_I1_CHANNEL,10);
        ADC_I2 = GetAdcMeasureValue(ADC_I1_CHANNEL,10);
        ADC_COMOUT = GetAdcMeasureValue(ADC_COMOUT_CHANNEL,10);
        
        
        V_LINEA = (float)ADC_LINEA*0.004028;
        V_LINEB = (float)ADC_LINEB*0.004028;
        V_12V = (float)ADC_12V*0.004028;
        I_I1 = (float)ADC_I1*2.6596;
        I_I2 = (float)ADC_I2*0.2660;
        V_COMOUT = (float)ADC_COMOUT*0.004028;
        
//        sprintf(Temp1,"%fV",V_I1);
        sprintf(Temp2,"%.1fuA", V_LINEA);
//        sprintf(Temp3,"I2:%.1fV",V_I2);
//        sprintf(Temp4,"COMOUT:%.1fV",V_COMOUT);

        LcdShowStrCenter(Temp2);
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
  
//    while(1)
//    {

//    OSTimeDlyHMSM(0,0,0,100);
//    //LED1_OFF();
////    OSTimeDlyHMSM(0,0,0,400);
////    LcdShowStr2Center("öï³Ìµç×Ó ","V1.0");
//    }

	
}



