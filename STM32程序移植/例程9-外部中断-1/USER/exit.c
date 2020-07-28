#include "includes.h"

void ExtiConfig(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʹ�ܸ��ù���ʱ��
    //GPIOC.11�ж����Լ��жϳ�ʼ������ �½��ش���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //�ж�������ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//�ж���ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);
    
}

void EXTI15_10_IRQHandler(void)
{
    System72MDelay1ms(10);

        LED1_ON();
        I2C_GPIO_Config();  //����ļ���Ҫ��ʱ��ʼ��
        LcdInit();          //����ļ���Ҫ��ʱ��ʼ��
        LcdShowStrCenter("111");
        System72MDelay1ms(1000);
        LED1_OFF();
        
    
    EXTI_ClearITPendingBit(EXTI_Line11);
}

