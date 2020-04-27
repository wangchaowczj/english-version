#include "includes.h"

static u8 DetCmdDelayInterval = 1;
static u32 DetFreqFromHead = 0;  //оƬƵ��
static u32 DetFreqFromTimeRd = 0;  //оƬƵ��

DET_PROTOCOL DetProtocol;
u32 TestErrorCount = 0;

/**
  *************************************************************************************
  * @brief  �򿪷�������ʱ�����ڼ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��03��14��
  * @note   �����ش���
  *************************************************************************************  
  */
void OpenDetCmdDelayInterval(void)
{
	DetCmdDelayInterval = 1;
}
/**
  *************************************************************************************
  * @brief  �رշ�������ʱ�����ڼ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��03��14��
  * @note   �����ش���
  *************************************************************************************  
  */
void CloseDetCmdDelayInterval(void)
{
	DetCmdDelayInterval = 0;
}
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

//==================================================================================================
//| �������� | DetSendConfig
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����GPIO
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void DetSendConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(DET_RECEIVE_RES_PORT_RCC | BLAST_SIGNAL_SEND_PORT_RCC | DET_CMD_SEND_PORT_RCC | 
	DOUBLE_KEY_SW_PORT_RCC|ADJUST_G_PORT_RCC|DOUBLE_KEY1_SIM_SW_PORT_RCC|DOUBLE_KEY2_SIM_SW_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***���������״̬�л��ܽ�**/
	GPIO_InitStructure.GPIO_Pin = DET_RECEIVE_RES_PIN; 
	GPIO_Init(DET_RECEIVE_RES_PORT, &GPIO_InitStructure);
	
	/***�׹�����͹ܽ�**/
	GPIO_InitStructure.GPIO_Pin = BLAST_SIGNAL_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(BLAST_SIGNAL_SEND_PORT, &GPIO_InitStructure);
	
	/***�׹�����͹ܽ�**/
	GPIO_InitStructure.GPIO_Pin = DET_CMD_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(DET_CMD_SEND_PORT, &GPIO_InitStructure);
		
	/***��Ϊ����ʱ��˫�����ӿ���IO**/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY_SW_PIN;  
	GPIO_Init(DOUBLE_KEY_SW_PORT, &GPIO_InitStructure);	

	/***��Ϊ�ӻ�ʱģ��˫������ʹ�ܿ���**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY1_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY1_SIM_SW_PORT, &GPIO_InitStructure);

	/***��Ϊ�ӻ�ʱģ��˫������ʹ�ܿ���**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY2_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY2_SIM_SW_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
	
	/***�Ƚ�����λ�ܽ�**/
	GPIO_InitStructure.GPIO_Pin = ADJUST_G_PIN; 
	GPIO_Init(ADJUST_G_PORT, &GPIO_InitStructure);	
	
	CLOSE_DOUBLE_KEY_SW();
	CLOSE_DOUBLE_KEY1_SIM_SW();
	CLOSE_DOUBLE_KEY2_SIM_SW();
	
	/***�׹�����͹ܽų�ʼ��ƽ**/
	DET_COM_SEND_PIN_INIT();
	
	/***����������**/
	CLOSE_DOUBLE_KEY_SW();	
	
	/***���źų�ʼ��ƽ**/
	BLAST_SIGNAL_PIN_INIT();	
	
	/***���յ���Ĭ��Ϊ��**/
	OPEN_DET_RECEIVE_RES();
	
	/***���ձȽ���Ĭ��Ϊ��**/
	ADJUST_G_RESET();
}

//==================================================================================================
//| �������� | DetReceiverConfig
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����GPIO,PC.6 PC.7��Ϊ�����׹��ź�
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void DetReceiverConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	 /* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);	
		
	/* Configure PC6 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*�ⲿ�½����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line6; //FALL
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
			
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;	//RISE
	EXTI_Init(&EXTI_InitStructure);

	
	//FALL,RISE�źţ��ⲿ�½����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
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
void DetHardwareInit(void)
{
	DetSendConfig();
	DetReceiverConfig();
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
	
	TIM_Cmd(TIM2, DISABLE);	//��Ҫʱ������
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/***���Ͷ�ʱ���ж����ã���Ͻ���FALL��RISE�ź�***/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

/**
  *************************************************************************************
  * @brief  �ӷ��ص�ͬ��ͷ�м����оƬƵ��
  * @param  �� 
  * @retval оƬƵ�ʣ���λHZ
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u32 GetDetFreqFromHead(void)
{
	return DetFreqFromHead;
}

/**
  *************************************************************************************
  * @brief  �ӷ��ص�����ʱ���м����оƬƵ��
  * @param  �� 
  * @retval оƬƵ�ʣ���λHZ
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u32 GetDetFreqFromTimeRd(void)
{
	return DetFreqFromTimeRd;
}
/**
  *************************************************************************************
  * @brief  ����д���־
  * @param  �� 
  * @retval оƬƵ�ʣ���λHZ
  * @author ZH
  * @date   2015��8��7��
  * @note   ����������ʱ��ض�ָ������
  *************************************************************************************  
  */
u8 GetDetWriteDelayFLag(void)
{
	return (DetProtocol.RecData.Data[3]&0x80);
}
/**
  *************************************************************************************
  * @brief  ������ʱ��ض�ָ�������оƬ��ǰ�����ڼ���ֵ
  * @param  �� 
  * @retval оƬƵ�ʣ���λHZ
  * @author ZH
  * @date   2015��8��7��
  * @note   ����������ʱ��ض�ָ������
  *************************************************************************************  
  */
u32 GetDetDelayClock(void)
{
	u32 U32Value;
	
	U32Value = DetProtocol.RecData.Data[1] | ((u32)DetProtocol.RecData.Data[2]<<8) | ((u32)DetProtocol.RecData.Data[3]<<16);
	U32Value &= 0x3FFFFF;
	return U32Value;
}
/**
  *************************************************************************************
  * @brief  ������ʱ��ض�ָ�������оƬ��ǰ�ķֶ�����ֵ
  * @param  �� 
  * @retval �ֶ�����ֵ
  * @author ZH
  * @date   2015��8��7��
  * @note   ����������ʱ��ض�ָ������
  *************************************************************************************  
  */
u16 GetDetFixedDelay(void)
{
	u16 U16Value;
	
	U16Value = DetProtocol.RecData.Data[4] | ((u16)DetProtocol.RecData.Data[5]<<8);
	U16Value &= 0x7FFF;
	return U16Value;
}

//u16 U16DataCMP(const u16* ptr, u8 length)
//{
//	u16 max_value, min_value ;
//	u8 i ;

//	max_value = *ptr; //���ֵ
//	min_value = *ptr; //��Сֵ
//	for(i=1;i<length;i++)
//	{
//		if(*(ptr+i)>max_value)
//		{
//			max_value = *(ptr+i) ;
//		}
//		else if(*(ptr+i) < min_value)
//		{
//			min_value = *(ptr+i) ;
//		}
//	}
//	return (max_value - min_value);	//�������ֵ
//}
u8 GetBitNum(u32 sum_count, u32 bit_count)
{
	u8 bit_num = 0;
	u32 temp = 0;

	bit_num = (u8)(sum_count/bit_count);
	temp = sum_count%bit_count;
	if((temp*2) > bit_count)  //������������һ��ʱ��Ϊ��������1λ
	{
		bit_num++;	
	}
	return bit_num;	
}
/**
  *************************************************************************************
  * @brief  ��ȡ��Ӧ�ֽڵ�ż����λ
  * @param  Byte �ֽ�
  * @retval �����λ
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u8 GetEvenBit(u8 Byte)
{
	u8 Count = 0,i;
	
	for(i=0;i<8;i++)
	{
		if(Byte&0x01)
		{
			Count++;
		}
		Byte >>= 1;
	}
	return (Count&0x01);	
}
/**
  *************************************************************************************
  * @brief  �����׹ܲ��β���շ�������
  * @param  ErrorRetry ���մ���ʱ���Դ���
  * @param  RecBytes ��Ҫ���յ��ֽ���(ͬ��ͷ������1���ֽ�)
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u8 DetRecAnalyze(u8 FreqMeasureFlagForTimeRd)
{
	u8 ByteIndex = 0, OddBit, EvenBit;
	u16 i,j, MaskBit, U16Value, DetBitClk, BitNumber;
	u32 DetFreq = 0;  //оƬƵ��
	u32 U32Value = 0;
	double FValue = 0;

	//�൱�ڰ�2·�źŻ�ԭ��1·�ź�,���ÿ���RISE�պñ�FALL��һ�����������Ϊ�������յ������Ѿ���֤�˿϶��ܽ��յ���������
	DetProtocol.RecBuffer.Count = 0;
	for(i=0;i < DetProtocol.Fall.Count;i++)
	{
		DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = DetProtocol.Fall.Timer[i] - DetProtocol.Rise.Timer[i];//��Ӧ�ߵ�ƽ
		if((i == (DetProtocol.Fall.Count-1)) &&
			(DetProtocol.Fall.Count == DetProtocol.Rise.Count))
		{				
			break;
		}			
		DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = DetProtocol.Rise.Timer[i+1] - DetProtocol.Fall.Timer[i];//��Ӧ�͵�ƽ
	}
	
	//����ͬ��ͷ�����ÿλ��Ӧ�ļ���ֵ��оƬƵ��
	if(DetProtocol.RecBuffer.Count >= 5)
	{//����Ҫ��3��ͬ��ͷ
		memset((char*)&DetProtocol.RecData.Data[0], 0, sizeof(DetProtocol.RecData.Data));
		
		if((U32DataCMP((u32*)&DetProtocol.RecBuffer.Timer[0],5)) > T40US)//�õ���ͬ��ͷ�����У�16��CLK���,��֤������2����
		{
			return 1;	//ͬ��ͷ���ݲ��ȶ�
		}
		U32Value = DetProtocol.Fall.Timer[2]-DetProtocol.Rise.Timer[0];
		DetBitClk = U32Value/5;//���ز�����ÿλ��Ӧ�ļ���ֵ(��Ӧ16��оƬ����)

		FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)80/(double)U32Value;//оƬƵ�ʣ���λHZ
		DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
		if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
		{//ͬ��ͷ���ݲ��ȶ�
			return 2;	
		}
		DetFreqFromHead = DetFreq;
		
		ByteIndex = 0;
		MaskBit = 0x0001;
		U16Value = 0x00;
		for(i=0;i<DetProtocol.RecBuffer.Count;i++)
		{
			if(DetProtocol.RecBuffer.Timer[i] <= (DetBitClk/2))
			{//��Ȳ���ͬ��ͷ��һ�룬��Ϊ�Ǹ���
				return 3;
			}
			
			BitNumber = GetBitNum(DetProtocol.RecBuffer.Timer[i], DetBitClk);//ת��Ϊ0��1��λ����
			for(j=0;j<BitNumber;j++)
			{
				if((i&0x01) == 0)
				{//��Ӧ�ߵ�ƽ
					U16Value |= MaskBit; 
				}
				MaskBit <<= 1;//оƬ�ȷ��ص�λ
				if(MaskBit&0x400)
				{//һ���ֽ�8λ+2λ����λ
					//���������λ��ż����λ
					OddBit = (U16Value>>8)&0x01;
					EvenBit = (U16Value>>9)&0x01;
					if((OddBit == EvenBit) || (EvenBit != GetEvenBit(U16Value&0xFF)))
					{
						return 4;
					}

					DetProtocol.RecData.Data[ByteIndex++] = U16Value&0xFF;					
					
					if(ByteIndex >= DetProtocol.RecData.ExpentCount)
					{
						if((DetProtocol.RecData.Data[0]&0x3F) != 0x15)
						{//ͬ��ͷ������3������4��
							return 5;
						}
						
						if((0 != FreqMeasureFlagForTimeRd) && (DetProtocol.RecData.ExpentCount == 6))
						{//��ǰΪ��ʱʱ��ض�Ƶ�ʲ���
							if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
							{//оƬ�յ�д���ں�ֻ����3��ͬ��ͷ
								if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
									(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
									(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
								{//��Ƶǰ�̶�д������Ϊ0x0000FE
									U32Value = DetProtocol.Fall.Timer[7]-DetProtocol.Rise.Timer[0];

									FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//оƬƵ�ʣ���λHZ, 608 = (3*10+8)*16
									DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
									if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
									{//���ݲ��ȶ�
										return 6;	
									}
									DetFreqFromTimeRd = DetFreq;
									return 0;
								}
							}
							return 7;
						}
						return 0;
					}	
					
					//��ʼ��һ���ֽ�
					U16Value = 0x00;
					MaskBit = 0x0001;
				}			
			}
		}
		if(ByteIndex < DetProtocol.RecData.ExpentCount)
		{//���������ֽڲ���
			if(((ByteIndex+1) == DetProtocol.RecData.ExpentCount) && (MaskBit&0x200))
			{//���һλż����λ�����0�Ļ����п��ܸպ��ٽ��յ�һ���ж�
				OddBit = (U16Value>>8)&0x01;
				if((OddBit == 1) && (0 == GetEvenBit(U16Value&0xFF)))
				{
					DetProtocol.RecData.Data[ByteIndex++] = U16Value&0xFF;

					if((DetProtocol.RecData.Data[0]&0x3F) != 0x15)
					{//ͬ��ͷ������3������4��
						return 8;
					}					

					if((0 != FreqMeasureFlagForTimeRd) && (DetProtocol.RecData.ExpentCount == 6))
					{//��ǰΪ��ʱʱ��ض�Ƶ�ʲ���
						if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
						{//оƬ�յ�д���ں�ֻ����3��ͬ��ͷ
							if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
								(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
								(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
							{//��Ƶǰ�̶�д������Ϊ0x0000FE
								U32Value = DetProtocol.Fall.Timer[7]-DetProtocol.Rise.Timer[0];
								
								FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//оƬƵ�ʣ���λHZ, 608 = (3*10+8)*16
								DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
								if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
								{//���ݲ��ȶ�
									return 9;	
								}
								DetFreqFromTimeRd = DetFreq;
								return 0;
							}
						}
						return 10;
					}					
					return 0;
				}
				else
				{
					return 11;
				}				
			}
			else
			{
				return 12;
			}
		}
		else
		{
			return 13;
		}		
	}
	
	return 14;
}
/**
  *************************************************************************************
  * @brief  �Խ��յ���ԭʼ�ź����½����˲�����
  * @param  FreqMeasureFlagForTimeRd ��Ƶ��־
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus FilterAnalyzeRetry(u8 FreqMeasureFlagForTimeRd)
{
    u16 i, j;
    u32 HeadWith;
    u8 Result;
    
    if((DetProtocol.RealRise.Count < 3) || (DetProtocol.RealFall.Count < 3))
    {
        return ERROR;
    }
    //��ԭʼ�ź����½����˲����ٳ��Է���    
    //��ȷ��ͬ��ͷ�Ŀ�ȣ�Ȼ������ÿһλ�Ŀ��Ӧ����ͬ��ͷ��ȵ�������(����һ����ƫ��)
    for(i=0; i<DetProtocol.RealRise.Count;i++)
    {
        for(j=0; j<DetProtocol.RealFall.Count;j++)
        {
            if(DetProtocol.RealFall.Timer[j] > DetProtocol.RealRise.Timer[i])
            {
                HeadWith = DetProtocol.RealFall.Timer[j] - DetProtocol.RealRise.Timer[i];
                if ((HeadWith > 11200) && (HeadWith < 26880))//ÿλͬ��ͷ��Ӧ�Ŀ����133us-320us��50-120Kʱ
                {
                    u16 a = i+1, b = j+1;
                    u32 With;
                    double BitCountFloat;
                    u16 BitCount;
                    
                    DetProtocol.Fall.Count = 0;
                    DetProtocol.Rise.Count = 0;                  
                    
                    DetProtocol.Rise.Timer[DetProtocol.Rise.Count++] = DetProtocol.RealRise.Timer[i];
                    DetProtocol.Fall.Timer[DetProtocol.Fall.Count++] = DetProtocol.RealFall.Timer[j]; 
                    while((a < DetProtocol.RealRise.Count) && (b < DetProtocol.RealFall.Count))
                    {
                        if(DetProtocol.Rise.Count == DetProtocol.Fall.Count)
                        {
                            for(; a<DetProtocol.RealRise.Count; a++)
                            {
                                if(DetProtocol.RealRise.Timer[a] > DetProtocol.Fall.Timer[DetProtocol.Fall.Count-1])
                                {
                                    With = DetProtocol.RealRise.Timer[a] - DetProtocol.Fall.Timer[DetProtocol.Fall.Count-1];
                                    BitCountFloat = (double)With/HeadWith;
                                    BitCount = (int)BitCountFloat;
                                    if((BitCountFloat- BitCount) > 0.85)
                                    {
                                        BitCount++;
                                    }
                                    else if((BitCountFloat- BitCount) > 0.15)
                                    {
                                        continue;
                                    }
                                    if((BitCount < 1) || (BitCount >= 11))//����Э�飬���ز���Ϊ��żУ�飬���Բ�������11λ0����1
                                    {
                                        continue;
                                    }
                                    if((DetProtocol.Rise.Count < 3) && (BitCount != 1))
                                    {
                                        continue;
                                    }
                                    if(DetProtocol.Rise.Count >= DET_REC_DATA_MAX)
                                    {
                                        continue;
                                    }
                                    DetProtocol.Rise.Timer[DetProtocol.Rise.Count++] = DetProtocol.RealRise.Timer[a];
                                    a++;
                                    break; 
                                }
                            }                         
                        }
                        if(DetProtocol.Rise.Count == (DetProtocol.Fall.Count+1))
                        {
                            for(; b<DetProtocol.RealFall.Count; b++)
                            {
                                if(DetProtocol.RealFall.Timer[b] > DetProtocol.Rise.Timer[DetProtocol.Rise.Count-1])
                                {
                                    With = DetProtocol.RealFall.Timer[b] - DetProtocol.Rise.Timer[DetProtocol.Rise.Count-1];
                                    BitCountFloat = (double)With/HeadWith;
                                    BitCount = (int)BitCountFloat;
                                    if((BitCountFloat- BitCount) > 0.85)
                                    {
                                        BitCount++;
                                    }
                                    else if((BitCountFloat- BitCount) > 0.15)
                                    {
                                        continue;
                                    }
                                    if((DetProtocol.Fall.Count < 3) && (BitCount != 1))
                                    {
                                        continue;
                                    }
                                    if(DetProtocol.Fall.Count >= DET_REC_DATA_MAX)
                                    {
                                        continue;
                                    }                                    
                                    DetProtocol.Fall.Timer[DetProtocol.Fall.Count++] = DetProtocol.RealFall.Timer[b];
                                    b++;
                                    break; 
                                }
                            }                            
                        }
                        if((DetProtocol.Rise.Count == 3) && (DetProtocol.Fall.Count == 3))
                        {
                            HeadWith = 0;
                            HeadWith += DetProtocol.Fall.Timer[0] - DetProtocol.Rise.Timer[0];
                            HeadWith += DetProtocol.Rise.Timer[1] - DetProtocol.Fall.Timer[0];
                            HeadWith += DetProtocol.Fall.Timer[1] - DetProtocol.Rise.Timer[1];
                            HeadWith += DetProtocol.Rise.Timer[2] - DetProtocol.Fall.Timer[1];
                            HeadWith += DetProtocol.Fall.Timer[2] - DetProtocol.Rise.Timer[2];
                            HeadWith /= 5;
                        }
                    }
                    Result = DetRecAnalyze(FreqMeasureFlagForTimeRd);
                    if(Result == 0)
                    {
                        return SUCCESS;
                    }
                }
            }
        }
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  �����׹ܲ��β���շ�������
  * @param  ErrorRetry ���մ���ʱ���Դ���
  * @param  RecBytes ��Ҫ���յ��ֽ���(ͬ��ͷ������1���ֽ�)
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetSendAndRec(u8 ErrorRetry, u8 RecBytes, u8 FreqMeasureFlagForTimeRd)
{ 
	u8 ResultCode = 0 ;
	u8 U16Value1, U16Value2;
	u32 c_time;
	u8 WaitRecFlag = 0;
	
    CLOSE_DET_RECEIVE_RES();//��·���յ���
	if(DetCmdDelayInterval)
	{
		if((DET_BLAST == DetProtocol.Mode) || (DET_CMD_BLAST == DetProtocol.Mode))
		{
			System168MDelay1ms(5);//��ʱһ���Ը���
		}
		else
		{
			OSTimeDlyHMSM(0,0,0,20);//���������л�������
		}	
	}
	
	do
	{
        CLOSE_DET_RECEIVE_RES();//��·���յ���
		if(0 != ResultCode)
		{//�����ط����
			OSTimeDlyHMSM(0,0,0,10);//10ms	
		}

		DetProtocol.Fall.Count = 0;
		DetProtocol.Rise.Count = 0;
		DetProtocol.RealFall.Count = 0;
		DetProtocol.RealRise.Count = 0;        
		DetProtocol.RecBuffer.Count = 0;
		DetProtocol.RecData.ExpentCount = RecBytes;//Ҫ���յ��ֽ�����
		DetProtocol.SendBuffer.Index = 0;
		DetProtocol.SendBuffer.State = DET_SEND;

		/***�׹�����͹ܽų�ʼ��ƽ**/
		DET_COM_SEND_PIN_INIT();
		
		/***���źų�ʼ��ƽ**/
		BLAST_SIGNAL_PIN_INIT();				
		
		DetTimerConfig();  //���ö�ʱ��2
		NVIC_EnableIRQ(TIM2_IRQn);//ʹ��TIM2�ж�
		TIM_ARRPreloadConfig(TIM2, ENABLE);//����ARRԤװ�ػ�����	
		TIM_SetCounter(TIM2, 0xFFFFFFFFUL-DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);				
		TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);		
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
		if((DET_CMD == DetProtocol.Mode)||(DET_FIXED_PULSE == DetProtocol.Mode))
		{			
			DET_COM_SEND_TOGGLE();
		}		
		else
		{
			BLAST_SIGNAL_TOGGLE();	
		}
		
		TIM_Cmd(TIM2, ENABLE);  //����ʱ��					// �������� 		
		
		while(DET_SEND == DetProtocol.SendBuffer.State)// �ȴ�ȫ��ָ���뷢�����
		{
			if(DET_BLAST == DetProtocol.Mode)
			{//�������ź�ʱ����¼�˿ڵ�ѹ״̬
				if(BlastSingleMonitor.Count < BLAST_SINGLE_MONITOR_BUFFER_SIZE)
				{
					BlastSingleMonitor.LinABAdc[BlastSingleMonitor.Count++] = GetLinABAdcValue(); 
					System168MDelay1ms(1);		
				}			
			}
			else if(DET_FIXED_PULSE == DetProtocol.Mode)
			{
				OSTimeDly(1);
			}
		}
		if(DetProtocol.RecData.ExpentCount != 0)
		{//��Ҫ����
			WaitRecFlag = 1;
			while(WaitRecFlag)
			{
				WaitRecFlag = 0;
				c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
				U16Value1 = DetProtocol.Fall.Count;//
				while(OSTimeGet() < (c_time+7/(1000/OS_TICKS_PER_SEC)))//7ms�ڶ�û���յ����أ�˵������Ҳ�����������ݻ����ǻ������Ѿ���
				{
					U16Value2 = DetProtocol.Fall.Count;
					if(U16Value2 != U16Value1)
					{//�����µı��أ����¼�ʱ
						WaitRecFlag = 1;
						break;
					}
					else if((U16Value2/5) >= DetProtocol.RecData.ExpentCount)
					{//�������
						System168MDelay100us(3);
						break;
					}				
				}
			}
		}

		// ��λ��״̬		
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
		TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
		NVIC_DisableIRQ(TIM2_IRQn);//ʧ���ⲿ�ж�	
		ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, DISABLE);
		ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, DISABLE);
		
		if((DET_CMD == DetProtocol.Mode)||(DET_FIXED_PULSE == DetProtocol.Mode))
		{
			SET_LV_6_7V(); //�л�������״̬	
		}
		else
		{
			;	
		}		
		/***�׹�����͹ܽų�ʼ��ƽ**/
		DET_COM_SEND_PIN_INIT();
		
		/***���źų�ʼ��ƽ**/
		BLAST_SIGNAL_PIN_INIT();
        
		if(DetProtocol.RecData.ExpentCount == 0)
        {//����������ʱ,��һ��������
            System168MDelay10us(2);        
        }

        OPEN_DET_RECEIVE_RES();
        
		//�������յ�������
		if(DetProtocol.RecData.ExpentCount != 0)
		{            
			ResultCode = DetRecAnalyze(FreqMeasureFlagForTimeRd);            
			if(ResultCode != 0)
			{
				if((DetProtocol.Hook == HOOK_CHECK_IN) && (DetProtocol.SendCmd.Buffer[0] == XA3_CHECK_IN) && 
                    (DetProtocol.RecData.ExpentCount == 1) && (DetProtocol.Rise.Count != 0))
				{
					DetProtocol.RecData.Data[0] = DET_RECEIVE_HEAD_AFTER_TIME;					
					ResultCode = 0;
				}		
			}
			if(ResultCode != 0)
			{                 
				//��¼��ǰ���ݵ���־�У����ڷ�������
				if((DetProtocol.Rise.Count != 0) || (DetProtocol.Fall.Count != 0))
				{
					char buffer[64];

					sprintf(buffer, "Code=%d Rise=%d  Fall=%d  Rec=%d Expent=%d", 
						ResultCode, DetProtocol.Rise.Count, DetProtocol.Fall.Count, 
						DetProtocol.RecBuffer.Count, DetProtocol.RecData.ExpentCount);
					WriteSystemLog(buffer, strlen(buffer), STR_INFO);
					
					WriteSystemLog((u32*)DetProtocol.RecBuffer.Timer, DetProtocol.RecBuffer.Count, H32_ARRAY);
					WriteSystemLog((u32*)DetProtocol.Rise.Timer, DetProtocol.Rise.Count, H32_ARRAY);
					WriteSystemLog((u32*)DetProtocol.Fall.Timer, DetProtocol.Fall.Count, H32_ARRAY);
					WriteSystemLog((u8*)DetProtocol.SendCmd.Buffer, DetProtocol.SendCmd.Count, HEX_ARRAY_NO_TITLE);
					WriteSystemLog((u32*)DetProtocol.RealRise.Timer, DetProtocol.RealRise.Count, H32_ARRAY);
					WriteSystemLog((u32*)DetProtocol.RealFall.Timer, DetProtocol.RealFall.Count, H32_ARRAY);
                    
                    if((DetProtocol.RealRise.Count > 2) && (DetProtocol.RealFall.Count > 2))
                    {
                        if(FilterAnalyzeRetry(FreqMeasureFlagForTimeRd) == ERROR)//�Խ��յ���ԭʼ�ź����½��й��˽���
                        {
                            WriteSystemLog("���¹��˺���Ȼ����", strlen("���¹��˺���Ȼ����"), STR_INFO);
                        }
                        else
                        {
                            ResultCode = 0;
                            WriteSystemLog("���¹��˺���ȷ", strlen("���¹��˺���ȷ"), STR_INFO);
                            WriteSystemLog((u32*)DetProtocol.RecBuffer.Timer, DetProtocol.RecBuffer.Count, H32_ARRAY);
                            WriteSystemLog((u32*)DetProtocol.Rise.Timer, DetProtocol.Rise.Count, H32_ARRAY);
                            WriteSystemLog((u32*)DetProtocol.Fall.Timer, DetProtocol.Fall.Count, H32_ARRAY);
                        }					
                    }                    
                }
			}
		}
		DetProtocol.SendBuffer.State = DET_END;
	}
	while((ResultCode != 0) && (ErrorRetry--));
	
	return (ResultCode == 0)?SUCCESS:ERROR;
}
ErrorStatus XA3_DetSendAndRec(u8 ErrorRetry, u8 RecBytes)
{
    return DetSendAndRec(ErrorRetry, RecBytes, 0);
}
