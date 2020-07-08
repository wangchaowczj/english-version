#include "includes.h"




int main(void)
{
      

   
    while(1)
    {
        I2C_GPIO_Config();  
        LcdInit(); 
    LcdShowStr2Center("öï³Ìµç×Ó","V1.0");  
    }     
}



