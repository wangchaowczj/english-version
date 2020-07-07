#include "includes.h"

int main(void)
{
    I2C_GPIO_Config();    

    LcdInit();    
    while(1)
    {
   
    LcdShowStr2Center("φο°ζ±ΎΊΕ","V1.0");  
    }     
}

