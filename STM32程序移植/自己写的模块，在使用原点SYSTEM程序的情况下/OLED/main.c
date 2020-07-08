#include "includes.h"

int main(void)
{
    I2C_GPIO_Config();    

    LcdInit();    
    while(1)
    {
   
    LcdShowStr2Center("鲲程电子","版本号V1.0");  
    }     
}

