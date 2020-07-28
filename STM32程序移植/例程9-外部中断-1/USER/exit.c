#include "includes.h"

void ExtiConfig(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用功能时钟
    //GPIOC.11中断线以及中断初始化配置 下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //中断线所在通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//中断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//中断抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);
    
}

void EXTI15_10_IRQHandler(void)
{
    System72MDelay1ms(10);

        LED1_ON();
        I2C_GPIO_Config();  //这个文件需要随时初始化
        LcdInit();          //这个文件需要随时初始化
        LcdShowStrCenter("111");
        System72MDelay1ms(1000);
        LED1_OFF();
        
    
    EXTI_ClearITPendingBit(EXTI_Line11);
}

