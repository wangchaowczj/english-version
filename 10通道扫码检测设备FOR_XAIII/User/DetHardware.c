#include "includes.h"

static u32 DetFreqFromHead = 0;  //оƬƵ��
static u8 DetFreqMeasureFlagForTimeRd = 0;
static u32 DetFreqFromTimeRd = 0;  //оƬƵ��

DET_PROTOCOL DetProtocol;


/**
  *************************************************************************************
  * @brief  �����ⲿ�жϷ�ʽ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��07��15��
  * @note   
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
void DetHardwareInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(DET_TX_PORT_RCC | DET_RX_PORT_RCC | DET_TX1_PORT_RCC | HV_SW_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	

	//�׹�ͨ���źŷ�������
	GPIO_InitStructure.GPIO_Pin = DET_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(DET_TX_PORT, &GPIO_InitStructure);

	//�׹�ͨ���źŷ�������
	GPIO_InitStructure.GPIO_Pin = DET_TX1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(DET_TX1_PORT, &GPIO_InitStructure);

	SET_LIN_REVERSE();
	
	//�׹�ͨ���źŽ�������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource15);
	
	GPIO_InitStructure.GPIO_Pin = DET_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(DET_RX_PORT, &GPIO_InitStructure);
	
	
	EXTI_InitStruct.EXTI_Line = DET_RX_EXTI_LINE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Enable and set EXTI9_5 Interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);	

	//��ѹ�л�����
	GPIO_InitStructure.GPIO_Pin = HV_SW_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(HV_SW_PORT, &GPIO_InitStructure);
	
	CLOSE_HV_SW();
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
  * @brief  ��������ʱ��ض��������Ƶ�ʱ�־
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void SetDetFreqMeasureFlagForTimeRd(void)
{
	DetFreqMeasureFlagForTimeRd = 1;
}
/**
  *************************************************************************************
  * @brief  �������ʱ��ض��������Ƶ�ʱ�־
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void ClearDetFreqMeasureFlagForTimeRd(void)
{
	DetFreqMeasureFlagForTimeRd = 0;
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
u16 XA_GetDetFixedDelay(void)
{
	u16 U16Value;
	
	U16Value = DetProtocol.RecData.Data[4] | ((u16)DetProtocol.RecData.Data[5]<<8);
	U16Value &= 0x7FFF;
	return U16Value;
}

u16 U16DataCMP(const u16* ptr, u8 length)
{
	u16 max_value, min_value ;
	u8 i ;

	max_value = *ptr; //���ֵ
	min_value = *ptr; //��Сֵ
	for(i=1;i<length;i++)
	{
		if(*(ptr+i)>max_value)
		{
			max_value = *(ptr+i) ;
		}
		else if(*(ptr+i) < min_value)
		{
			min_value = *(ptr+i) ;
		}
	}
	return (max_value - min_value);	//�������ֵ
}
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
ErrorStatus XA2_DetRecAnalyze(void)
{
	u8 ByteIndex = 0, OddBit, EvenBit;
	u32 DetBitClk;
	u16 i,j, MaskBit, U16Value, BitNumber;
	u32 DetFreq = 0;  //оƬƵ��
	u32 U32Value = 0;
	double FValue = 0;
	
	//����ͬ��ͷ�����ÿλ��Ӧ�ļ���ֵ��оƬƵ��
	if(DetProtocol.RecBuffer.Count >= 5)
	{//����Ҫ��3��ͬ��ͷ
		memset((char*)&DetProtocol.RecData.Data[0], 0, sizeof(DetProtocol.RecData.Data));
		
		if((U32DataCMP((u32*)&DetProtocol.RecBuffer.Timer[0],5)) > T40US)//�õ���ͬ��ͷ�����У�16��CLK���,��֤������2����
		{
			return ERROR;	//ͬ��ͷ���ݲ��ȶ�
		}
		
		DetBitClk = ((u32)DetProtocol.RecBuffer.Timer[0] + (u32)DetProtocol.RecBuffer.Timer[1] + 
			(u32)DetProtocol.RecBuffer.Timer[2] + (u32)DetProtocol.RecBuffer.Timer[3] + 
			(u32)DetProtocol.RecBuffer.Timer[4])/5;//���ز�����ÿλ��Ӧ�ļ���ֵ(��Ӧ16��оƬ����)

		FValue = (double) DET_CMD_TIMER_FREQ*(double)16/(double)DetBitClk;//оƬƵ�ʣ���λHZ
		DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
		if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
		{//ͬ��ͷ���ݲ��ȶ�
			return ERROR;	
		}
		DetFreqFromHead = DetFreq;
		
		ByteIndex = 0;
		MaskBit = 0x0001;
		U16Value = 0x00;
		for(i=0;i<DetProtocol.RecBuffer.Count;i++)
		{
			if(DetProtocol.RecBuffer.Timer[i] <= (DetBitClk/2))
			{//��Ȳ���ͬ��ͷ��һ�룬��Ϊ�Ǹ���
				return ERROR;
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
						return ERROR;
					}

					DetProtocol.RecData.Data[ByteIndex++] = U16Value&0xFF;
									
					if(ByteIndex >= DetProtocol.RecData.ExpentCount)
					{
						if((DetProtocol.RecData.Data[0]&0x3F) != 0x15)
						{//ͬ��ͷ
							return ERROR;
						}						
						if(0 != DetFreqMeasureFlagForTimeRd)
						{//��ǰΪ��ʱʱ��ض�Ƶ�ʲ���
							if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
							{//оƬ�յ�д���ں�ֻ����3��ͬ��ͷ
								if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
									(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
									(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
								{//��Ƶǰ�̶�д������Ϊ0x0000FF
									U32Value = 0;
									for(i=0;i<11;i++)
									{
										U32Value += DetProtocol.RecData.Data[i];										
									}
									FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//оƬƵ�ʣ���λHZ, 608 = (3*10+8)*16
									DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
									if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
									{//���ݲ��ȶ�
										return ERROR;	
									}
									DetFreqFromTimeRd = DetFreq;
									return SUCCESS;
								}
							}
							return ERROR;
						}
						return SUCCESS;
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
					{//ͬ��ͷ
						return ERROR;
					}					
					if(0 != DetFreqMeasureFlagForTimeRd)
					{//��ǰΪ��ʱʱ��ض�Ƶ�ʲ���
						if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
						{//оƬ�յ�д���ں�ֻ����3��ͬ��ͷ
							if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
								(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
								(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
							{//��Ƶǰ�̶�д������Ϊ0x0000FF
								U32Value = 0;
								for(i=0;i<11;i++)
								{
									U32Value += DetProtocol.RecData.Data[i];										
								}
								FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//оƬƵ�ʣ���λHZ, 608 = (3*10+8)*16
								DetFreq = (u32)(FValue+0.5);//оƬƵ�ʣ���λHZ(��������)
								if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
								{//���ݲ��ȶ�
									return ERROR;	
								}
								DetFreqFromTimeRd = DetFreq;
								return SUCCESS;
							}
						}
						return ERROR;
					}
					return SUCCESS;
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
ErrorStatus DetSendAndRec(u8 ErrorRetry, u8 RecBytes, DET_CLASS DetClass)
{ 
	ErrorStatus Result = SUCCESS ;
	u16 U16Value1, U16Value2;
	u32 c_time;
	u8 WaitRecFlag = 0;	

	OSTimeDlyHMSM(0,0,0,5);
	do
	{
		if(ERROR == Result)
		{//�����ط����
			OSTimeDlyHMSM(0,0,0,20);//20ms	
		}
		
		DetProtocol.RecBuffer.Count = 0;
		DetProtocol.RecData.ExpentCount = RecBytes;//Ҫ���յ��ֽ�����
		DetProtocol.SendBuffer.Index = 0;
		DetProtocol.SendBuffer.State = DET_SEND;
		
		SET_LIN_OPEN();
				
		DetTimerConfig();  //���ö�ʱ��2
		NVIC_EnableIRQ(TIM2_IRQn);//ʹ��TIM2�ж�
		TIM_ARRPreloadConfig(TIM2, ENABLE);//����ARRԤװ�ػ�����	
		TIM_SetCounter(TIM2, 0xFFFFFFFFUL-DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);				
		TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);		
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
		SET_LIN_REVERSE();
		
		TIM_Cmd(TIM2, ENABLE);  //����ʱ��					// �������� 		
		
		while(DET_SEND == DetProtocol.SendBuffer.State) { ; }// �ȴ�ȫ��ָ���뷢�����
		if(DetProtocol.RecData.ExpentCount != 0)
		{//��Ҫ����
            u8 bitCount = 10;            
			WaitRecFlag = 1;
			while(WaitRecFlag)
			{
				WaitRecFlag = 0;
				U16Value1 = DetProtocol.RecBuffer.Count;
				c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
				while(OSTimeGet() < (c_time+7/(1000/OS_TICKS_PER_SEC)))//5ms�ڶ�û���յ����أ�˵������Ҳ�����������ݻ����ǻ������Ѿ���
				{
					U16Value2 = DetProtocol.RecBuffer.Count;
					if(U16Value2 != U16Value1)
					{//�����µı��أ����¼�ʱ
						WaitRecFlag = 1;
						break;
					}
					else if((U16Value2/bitCount) >= DetProtocol.RecData.ExpentCount)
					{//�������
						SystemDelay100us(3);
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
	
		ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, DISABLE);		
		SET_LIN_OPEN();	
		//�������յ�������
		if(DetProtocol.RecData.ExpentCount != 0)
		{
			Result = XA2_DetRecAnalyze();
		}		
	}
	while((Result == ERROR) && (ErrorRetry--));
	
	return Result;
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
ErrorStatus XA2_DetSendAndRec(u8 ErrorRetry, u8 RecBytes)
{ 
	return DetSendAndRec(ErrorRetry, RecBytes, XA2_DET);
}
