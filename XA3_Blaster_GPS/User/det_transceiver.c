#include "includes.h"

DET_COMM det_comm;


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
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/***����TIM8ʱ�ӣ����ڷ�������***/
  	RCC_APB2PeriphClockCmd(DET_SEND_TIMER_RCC, ENABLE);
	
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
	
	/***�׹�����͹ܽų�ʼΪ�ߵ�ƽ**/
	DET_COM_SEND_PIN_INIT();
	

	TIM_TimeBaseStructure.TIM_Prescaler = 0;//����Ԥ��Ƶ����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = DET_SEND_TIMER_50US_VALUE;   //����ʱ����0������16799����Ϊ16800�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(DET_SEND_TIMER, &TIM_TimeBaseStructure);

	/***�ݲ�������ʱ��***/
	TIM_Cmd(DET_SEND_TIMER, DISABLE);
	
	/***����������**/
	CLOSE_DOUBLE_KEY_SW();
	
	
	/***���źų�ʼ��ƽ**/
	BLAST_SIGNAL_PIN_INIT();	
	
	/***���յ���Ĭ��Ϊ��**/
	OPEN_DET_RECEIVE_RES();
	
	/***���ձȽ���Ĭ��Ϊ��**/
	ADJUST_G_RESET();
	
	/***���Ͷ�ʱ���ж�����***/
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
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
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	 /* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

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

	TIM_TimeBaseStructure.TIM_Prescaler = 0;//����Ԥ��Ƶ����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; //TIME5��TIME2����32λ��ʱ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM2, DISABLE);	//��Ҫʱ������
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//FALL,RISE�źţ��ⲿ�½����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	//��ʱ���жϣ���Ͻ���FALL��RISE�ź�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��Ҫ��UART3�ߣ�����ռ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//==================================================================================================
//| �������� | DetTransmitStart
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����׹�����͹���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------        
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |   
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static void DetTransmitStart(void)
{
	det_comm.send_index = 0;
	det_comm.receive_state = 0;
	det_comm.blast_signal_flag = 0;
	det_comm.enable_receive = 0;
	
	/***�׹�����͹ܽų�ʼΪ�ߵ�ƽ**/
	DET_COM_SEND_PIN_INIT();
	CLOSE_DET_RECEIVE_RES();//��·���յ���
	TIM_SetCounter(DET_SEND_TIMER,0);//
	TIM_ClearFlag(DET_SEND_TIMER, TIM_FLAG_Update);
	TIM_ITConfig(DET_SEND_TIMER, TIM_IT_Update, ENABLE);	
	TIM_SetAutoreload(DET_SEND_TIMER, WAVE_WIDTH_100US);
	TIM_Cmd(DET_SEND_TIMER, ENABLE);	
	
	//�ȴ��������
	while(det_comm.send_index != 0xFFFF);
	System168MDelay100us(1);	//��ʱ100us
	OPEN_DET_RECEIVE_RES();
}

//==================================================================================================
//| �������� | SendDetBlastSignal 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �������ź�
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void SendDetBlastSignal(void)
{
	/**���ö�ʱ��**/
	TIM_SetCounter(TIM2, 0);	
	TIM_SetAutoreload(TIM2, BLAST_SIGNAL_SEND_TIMER_10_2MS_VALUE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	/***�׹�����͹ܽų�ʼΪ�ߵ�ƽ**/
	DET_COM_SEND_PIN_INIT();
	
	/***���źų�ʼ��ƽ**/
	BLAST_SIGNAL_PIN_INIT();
	
	/**������ʱ��**/
	det_comm.blast_signal_flag = 1;
	det_comm.blast_signal_toggle_count = 0;
	TIM_Cmd(TIM2, ENABLE);	
	
	/***�ȴ��������***/
	while(det_comm.blast_signal_flag == 1)
	{//ͨ���Զ˿ڵ�ѹ���в�����ȷ�����ź��Ѿ�����
		if(BlastSingleMonitor.Count < BLAST_SINGLE_MONITOR_BUFFER_SIZE)
		{
			BlastSingleMonitor.LinABAdc[BlastSingleMonitor.Count++] = GetLinABAdcValue(); 
			System168MDelay1ms(1);		
		}
	}
	/***���źų�ʼ��ƽ**/
	BLAST_SIGNAL_PIN_INIT();	
	//Op_Data.DoubleKeySwitch = 0;//��������˫��	
	//CLOSE_DOUBLE_KEY_SW();
}

//==================================================================================================
//| �������� | GetBitNum
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����ܼ���ֵ��ÿλ��ռ�ļ���ֵ�������λ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | sum_count:�ܼ���ֵ
//|          | bit_count:ÿλ��ռ�ļ���ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��λ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |   
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u8 GetBitNum(u32 sum_count, u32 bit_count)
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

//==================================================================================================
//| �������� | DetCmdCode
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���׹������ֽڱ���ɷ��㷢�͵�2��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | cmd_ptr:��Ҫ���͵����ݻ�������ַ
//|          | length:��Ҫ���͵����ݻ���������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��ÿ���������ʼλ��ͬ���ֽڶ�һ������˲�����д���ڷ���ʱ�������д���    
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void DetCmdCode(const u8* cmd_ptr, u8 length)
{
	u8 i,j,temp,verify_bit=0;

	det_comm.send_length = 0;
	det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US; 

	for(i=0;i<9;i++)
	{
		det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
		det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;	
	}

	for(i=0;i<length;i++)
	{
		temp = *cmd_ptr++ ;
		verify_bit = 0;
		for(j=0;j<8;j++)
		{
			if(temp&0x01) //�ȷ��͵�λ
			{
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
				verify_bit++;
			}
			else
			{			 
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
			}
			temp>>=1;
		}
		if(verify_bit&0x01)	//żУ��
		{
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
		}
		else
		{
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
		}
	}
}

//==================================================================================================
//| �������� | DetTransmit
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����ݽ��б������
//|----------|--------------------------------------------------------------------------------------
//| ������� | cmd_ptr:��Ҫ���͵����ݻ�������ַ
//|          | length:��Ҫ���͵����ݻ���������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------        
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |      
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void DetTransmit(const u8* cmd_ptr, const u8 length)
{
	/***�׹�����͹ܽų�ʼΪ�ߵ�ƽ**/
	DET_COM_SEND_PIN_INIT();
	System168MDelay1ms(10);//��ʱ10ms
	DetCmdCode(cmd_ptr, length);//�������ֽڱ���ɷ��㷢�͵�����	
	SET_CMD_SEND_STATE();//����Ϊ����״̬
	DetTransmitStart();//��������
}

//==================================================================================================
//| �������� | DetReceiver 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹ܷ������ݽ��պ��� 
//|----------|--------------------------------------------------------------------------------------
//| ������� | receive_ptr:���ջ�������ַ
//|          | mode: bit7��ʾ�Ƿ���Ҫ��Ƶ,
//|          | Bit0-3��ʾ��Ҫ�������ݵĳ���(������оƬƵ�����ݵ�4�ֽ�����)
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | EB_DET_TRANSCEIVER_SUCCESS-���ݽ��ճɹ�
//|          | EB_DET_TRANSCEIVER_FAIL-���ݽ���ʧ��
//|----------|--------------------------------------------------------------------------------------        
//| ������� | ��д�ˣ�    ʱ�䣺2013-08-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 1���и������������£�������ɽ���ʧ��
//|          | 2�����峤�Ȳ��ܳ��ڶ�ʱ�����ܱ�ʾ�ĳ���
//|          | 3���˺��������׹��ź�������ⲿ�жϴ���
//|          | 4����Ҫ��Ƶʱ������ͨ��д����ָ��д��0x0F0F0F
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u8 DetReceiver(u8* receive_ptr, u8 mode)
{
	u32 temp_buffer[70];
	u8 bit_buffer[70];
	u8 receive_buffer[10];
	u32 det_bit_clk = 0; //16��CLK��Ӧ�ļ���ֵ
	u32 det_4bit_clk = 0;//64��CLK��Ӧ�ļ���ֵ
	u8 i,j,wave_cnt = 0;
	u32 freq = 0;  //оƬƵ��
	u8 sum_num = 0,mask_bit = 0;//���ڰ�ʱ������ת��Ϊ�ֽ�����
	double freq_buffer = 0;

	/**�������ɺ�оƬ127��ʱ�Ӻ󷵻ز���***/
	TIM_SetCounter(TIM2, 0);

	/**�ڽ��յ�2��RISE�½��غ����¼��㳬ʱʱ��**/
	TIM_SetAutoreload(TIM2, TIME2_DET_165CLK);//��֤�����㹻���յ�2��RISE�ź�
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_Cmd(TIM2, ENABLE);	//������ʱ��

	det_comm.receive_bytes = mode & 0x0F;  //Ҫ���յ������ֽڣ�������ͬ��ͷ

	det_comm.receive_edg_max_num = (det_comm.receive_bytes+1)*4;
	det_comm.receive_rise_length = 0;
	det_comm.receive_fall_length = 0; //ʵ�ʲ�����½��ظ���
	memset(det_comm.rise_receive, 0, sizeof(det_comm.rise_receive));
	memset(det_comm.fall_receive, 0, sizeof(det_comm.fall_receive));
	
	SET_CMD_RECEIVE_STATE();//����Ϊ����״̬
	ADJUST_G_RESET();//�ñȽ�����λһ��
	System168MDelay100us(4);	//��ʱ400us


	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line7);
	
	det_comm.enable_receive = 1;//����FALL��RISE�½����ж�
	while(det_comm.receive_state == 0);//�ȴ��������
	det_comm.enable_receive = 0;
	//SET_LV_6_7V(); //�л�������״̬
	SET_CMD_SEND_STATE();//���յ��账�ڱ���·״̬����߶˿ڵ�ѹ
	
	if(det_comm.receive_rise_length != det_comm.receive_fall_length)
	{
	   if(det_comm.receive_fall_length > det_comm.receive_rise_length)//��������FALL���½��ظ��������ܱ�RISE�ĸ�����
	   {
			return EB_DET_TRANSCEIVER_FAIL;			
	   }
	   if((det_comm.receive_rise_length - det_comm.receive_fall_length)>1)//��������RISE���½��ظ�������FALL�ĸ�����1
	   {
			return EB_DET_TRANSCEIVER_FAIL;			
	   }
	}

	if((det_comm.receive_rise_length>=3)&&(det_comm.receive_fall_length>=3)) //�����Ѿ���⵽3��ͬ��ͷ
	{
		/*�Եõ������ݽ��б���ļ��*/
		for(i=0; i < det_comm.receive_rise_length-1; i++)
		{
			if(det_comm.rise_receive[i] >= det_comm.rise_receive[i+1]) //ȷ��Ϊ˳�����
			{
				return EB_DET_TRANSCEIVER_FAIL;	//�����쳣
			}
		}
		for(i=0; i < det_comm.receive_fall_length-1; i++)
		{
			if(det_comm.fall_receive[i]>=det_comm.fall_receive[i+1])//ȷ��Ϊ˳�����
			{
				return EB_DET_TRANSCEIVER_FAIL;	//�����쳣
			}
			if(det_comm.rise_receive[i] >= det_comm.fall_receive[i])//ȷ���Ƚ��յ��������ٽ��յ��½��أ��������
			{
				return EB_DET_TRANSCEIVER_FAIL;	//�����쳣
			} 
			if(det_comm.rise_receive[i+1] <= det_comm.fall_receive[i])//ȷ���Ƚ��յ��������ٽ��յ��½��أ��������
			{
				return EB_DET_TRANSCEIVER_FAIL;	//�����쳣
			}
		}

		temp_buffer[0] = det_comm.rise_receive[1] - det_comm.rise_receive[0];
		temp_buffer[1] = det_comm.rise_receive[2] - det_comm.rise_receive[1];
		temp_buffer[2] = det_comm.fall_receive[1] - det_comm.fall_receive[0];
		temp_buffer[3] = det_comm.fall_receive[2] - det_comm.fall_receive[1];

		if((U32DataCMP(temp_buffer,4)) > 1000)//�õ���ͬ��ͷ�����У�32��CLK���5000������(Լ59us)����,��֤��С��2����
		{
			return EB_DET_TRANSCEIVER_FAIL;	//ͬ��ͷ���ݲ��ȶ�
		}
		//����ͬ��ͷ�����ÿλ��Ӧ�ļ���ֵ��оƬƵ��()
		temp_buffer[0] = (temp_buffer[0]+temp_buffer[1]+temp_buffer[2]+temp_buffer[3])/4;//32��CLK��Ӧ�ļ���ֵ
		det_bit_clk = temp_buffer[0]/2;//���ز�����ÿλ��Ӧ�ļ���ֵ(��Ӧ16��оƬ����)
		freq_buffer = (double) TIME3_FREQ/((double)(temp_buffer[0]/(double)32));//оƬƵ�ʣ���λHZ
		freq = (u32)(freq_buffer+0.5);//оƬƵ�ʣ���λHZ(��������)
		if((freq<DET_RECEIVE_FREQ_MIN)||(freq>DET_RECEIVE_FREQ_MAX))//40K-120K
		{
			return EB_DET_TRANSCEIVER_FAIL;	//ͬ��ͷ���ݲ��ȶ�
		}
		
		//�൱�ڰ�2·�źŻ�ԭ��1·�ź�
		for(i=0;i < det_comm.receive_fall_length;i++)
		{
			temp_buffer[wave_cnt++] = det_comm.fall_receive[i] - det_comm.rise_receive[i];//��Ӧ�ߵ�ƽ
			if((i == (det_comm.receive_fall_length-1)) &&
				(det_comm.receive_fall_length == det_comm.receive_rise_length))
			{				
				break;
			}			
			temp_buffer[wave_cnt++] = det_comm.rise_receive[i+1] - det_comm.fall_receive[i];//��Ӧ�͵�ƽ
		}
		
		//�����ݵ������������͵�ƽ���߸ߵ�ƽ��ʱ����ô��Ҫ��������0����1���
		if(det_comm.receive_rise_length>det_comm.receive_fall_length)
		{
			//�����յ�����1����1���
			memset(receive_buffer,0xFF,sizeof(receive_buffer));
		}
		else
		{	//�����յ�����0����0���
			memset(receive_buffer, 0, sizeof(receive_buffer));
		}
		for(i=0; i < wave_cnt; i++)
		{   //��ʱ������ת��Ϊ�ֽ����ݣ�ͬ��ͷҲ��1���ֽ����ݽ���ת��
			bit_buffer[i] = GetBitNum(temp_buffer[i], det_bit_clk);//ת��Ϊ0��1��λ����
			if(i&1)
			{//��Ӧ�͵�ƽ
				for(j=0; j<bit_buffer[i]; j++)
				{
					mask_bit = ~(1<<(sum_num%8));	//оƬ�ȷ��ص�λ
					receive_buffer[sum_num/8] &= mask_bit;	 
					sum_num++;//�Ѿ�����ת����λ��
				}
			}
			else
			{//��Ӧ�ߵ�ƽ
				for(j=0; j<bit_buffer[i]; j++)
				{
					mask_bit = (1<<(sum_num%8));			//оƬ�ȷ��ص�λ
					receive_buffer[sum_num/8] |= mask_bit;	 
					sum_num++;//�Ѿ�����ת����λ��
				}
			}
		}				
		
		if((receive_buffer[0]&0x3F)!=0x15)//��֤3��ͬ��ͷ�Ƿ���ȷ
		{
			return EB_DET_TRANSCEIVER_FAIL;	//ͬ��ͷ���ݴ���
		}
		if(mode & 0x80)	 //��Ƶ
		{
			if((receive_buffer[2]==0x0F) && (receive_buffer[3]==0x0F) && (receive_buffer[4]==0x0F))
			{
				sum_num = 0;
				for(i=0; i<wave_cnt; i++)
				{
					sum_num += bit_buffer[i];
					if(sum_num == 20)//ͬ��ͷռ8λ��״̬�ֽ�8λ������1��ǰ���ֽ�4λ��20λ
					{
						//��Ƶʱ��ͨ��д����д��0x0F0F0F
						if(((i+4) < wave_cnt) && 
							(4 == GetBitNum(temp_buffer[i+1], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+2], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+3], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+4], det_bit_clk)))
						{
							//64��CLK��Ӧ�ļ���ֵ
							det_4bit_clk = (temp_buffer[i+1] + temp_buffer[i+2] + temp_buffer[i+3] + temp_buffer[i+4])/4;
							freq_buffer = (double) TIME3_FREQ/((double)(det_4bit_clk/(double)64));//оƬƵ�ʣ���λHZ
							freq = (u32)(freq_buffer+.55);//оƬƵ�ʣ���λHZ(��������)
							memcpy((u8*)receive_ptr, (u8*)&freq, sizeof(freq));
							return EB_DET_TRANSCEIVER_SUCCESS;	
						}
						else
						{
							break;
						}					
					}
					else if(sum_num > 20)
					{
						return EB_DET_TRANSCEIVER_FAIL;	//���ݴ���
					}
				}				
			}
			return EB_DET_TRANSCEIVER_FAIL;	//���ݴ���
		}
		else
		{
			memcpy((u8*)receive_ptr,(u8*)&receive_buffer[1],det_comm.receive_bytes);
			return EB_DET_TRANSCEIVER_SUCCESS;	
		}						
	}

	return EB_DET_TRANSCEIVER_FAIL;	//ͬ��ͷ������3��	
}
