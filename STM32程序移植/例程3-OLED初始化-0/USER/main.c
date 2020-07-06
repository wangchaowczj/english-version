#include "includes.h"

int main(void)
{
    I2C_GPIO_Config();    

    LcdInit();
    LcdShowStr2Center("µÂÖÝöï³Ì","V1.0");
    
    while(1)
    {
        ;
    }     
}

