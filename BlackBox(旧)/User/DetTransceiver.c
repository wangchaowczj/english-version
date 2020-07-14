#include "includes.h"

DET_PROTOCOL DetProtocol;
/**
  *************************************************************************************
  * @brief  �����ⲿ�жϷ�ʽ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��07��15��
  * @note   �����ش���
  *************************************************************************************  
  */
void ExtiConfig(uint32_t EXTI_Line, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_ClearFlag(EXTI_Line);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger;  
	EXTI_InitStructure.EXTI_LineCmd = EXTI_LineCmd;
	EXTI_Init(&EXTI_InitStructure);
}

/**
  *************************************************************************************
  * @brief  �׹���ص�Ӳ����ʼ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��13��
  * @note   ��
  *************************************************************************************  
  */
void DetPinConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(DET_RX_PORT_RCC | DET_TX_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

    DET_ACK_INIT();
    
	/***���͹ܽ�**/
	GPIO_InitStructure.GPIO_Pin = DET_TX_PIN; 
	GPIO_Init(DET_TX_PORT, &GPIO_InitStructure);
    
	 /* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource9);
		
	/***���չܽ�**/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = DET_RX_PIN;
	GPIO_Init(DET_RX_PORT, &GPIO_InitStructure);

	/*�ⲿ�½����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ⲿ�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

/**
  *************************************************************************************
  * @brief  �׹���صĶ�ʱ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��13��
  * @note   ��
  *************************************************************************************  
  */
void DetTimerConfig(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;//����Ԥ��Ƶ����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; //TIME5��TIME2����32λ��ʱ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/***���Ͷ�ʱ���ж����ã���Ͻ����ź�***/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
    
    TIM_Cmd(TIM2, ENABLE);
}

/**
  *************************************************************************************
  * @brief  ��ȡżУ��λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��8��16��
  * @note   ��
  *************************************************************************************  
  */
u8 GetEventBit(u8 Cmd)
{
    u8 i;
    u8 Count = 0;
    u8 Bit = 0x01;
    
    for(i=0; i<8; i++)
    {
        if(Cmd&Bit)
        {
            Count++;
        }
        Bit<<=1;
    }
    return Count&0x01;
}
/**
  *************************************************************************************
  * @brief  �׹���صĶ�ʱ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��13��
  * @note   ��
  *************************************************************************************  
  */
void DetAckTimerConfig(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;//����Ԥ��Ƶ����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; //TIME5��TIME2����32λ��ʱ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
    
    TIM_Cmd(TIM5, ENABLE);
}
void StartDetAck(void)
{
    DetProtocol.Ack.Bit = 0;
    DetProtocol.Ack.Index = 0;
    DetProtocol.Ack.Count = 0;
    DetProtocol.Ack.Data[DetProtocol.Ack.Count++] = 0x55;
    
    DetAckTimerConfig();  //���ö�ʱ��3
    NVIC_EnableIRQ(TIM5_IRQn);//ʹ��TIM2�ж�
    TIM_ARRPreloadConfig(TIM5, ENABLE);//����ARRԤװ�ػ�����	
    TIM_SetCounter(TIM5, 0xFFFFFFFF-TNUS(3000));				
    TIM_SetAutoreload(TIM5, 0xFFFFFFFF);
    
    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
}
void DetRecStatusInit(void)
{
    DetProtocol.Mode = DET_UNKNOW;
    TIM_SetCounter(TIM2, 0);
    TIM_SetAutoreload(TIM2, 0xFFFFFFFF);
    DetProtocol.Rec.Count = 0;
    DetProtocol.Cmd.Count = 0;
    DetProtocol.Cmd.Bit = 0;
}
