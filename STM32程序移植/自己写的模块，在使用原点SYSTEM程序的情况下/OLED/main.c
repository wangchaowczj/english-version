#include "includes.h"

int main(void)
{
    I2C_GPIO_Config();    

    LcdInit();    
    while(1)
    {
   
    LcdShowStr2Center("��̵���","�汾��V1.0");  
    }     
}

