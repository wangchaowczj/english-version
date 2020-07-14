#include "includes.h"

static u32 DetFreqFromHead = 0;  //芯片频率
static u8 DetFreqMeasureFlagForTimeRd = 0;
static u32 DetFreqFromTimeRd = 0;  //芯片频率

DET_PROTOCOL DetProtocol;


/**
  *************************************************************************************
  * @brief  配置外部中断方式
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年07月15日
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
  * @brief  雷管相关的硬件初始化
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月13日
  * @note   无
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

	//雷管通信信号发送引脚
	GPIO_InitStructure.GPIO_Pin = DET_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(DET_TX_PORT, &GPIO_InitStructure);

	//雷管通信信号发送引脚
	GPIO_InitStructure.GPIO_Pin = DET_TX1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(DET_TX1_PORT, &GPIO_InitStructure);

	SET_LIN_REVERSE();
	
	//雷管通信信号接收引脚
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

	//高压切换引脚
	GPIO_InitStructure.GPIO_Pin = HV_SW_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(HV_SW_PORT, &GPIO_InitStructure);
	
	CLOSE_HV_SW();
}

/**
  *************************************************************************************
  * @brief  雷管相关的定时器配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月13日
  * @note   无
  *************************************************************************************  
  */
void DetTimerConfig(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;//设置预分频：不预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; //TIME5和TIME2都是32位定时器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_Cmd(TIM2, DISABLE);	//需要时再启动
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/***发送定时器中断配置，配合接收FALL和RISE信号***/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  *************************************************************************************
  * @brief  从返回的同步头中计算出芯片频率
  * @param  无 
  * @retval 芯片频率，单位HZ
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
u32 GetDetFreqFromHead(void)
{
	return DetFreqFromHead;
}

/**
  *************************************************************************************
  * @brief  设置延期时间回读命令测量频率标志
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void SetDetFreqMeasureFlagForTimeRd(void)
{
	DetFreqMeasureFlagForTimeRd = 1;
}
/**
  *************************************************************************************
  * @brief  清除延期时间回读命令测量频率标志
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void ClearDetFreqMeasureFlagForTimeRd(void)
{
	DetFreqMeasureFlagForTimeRd = 0;
}
/**
  *************************************************************************************
  * @brief  从返回的延期时间中计算出芯片频率
  * @param  无 
  * @retval 芯片频率，单位HZ
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
u32 GetDetFreqFromTimeRd(void)
{
	return DetFreqFromTimeRd;
}

/**
  *************************************************************************************
  * @brief  从延期时间回读指令后计算出芯片当前的延期计数值
  * @param  无 
  * @retval 芯片频率，单位HZ
  * @author ZH
  * @date   2015年8月7日
  * @note   必需在延期时间回读指令后调用
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
  * @brief  从延期时间回读指令后计算出芯片当前的分段延期值
  * @param  无 
  * @retval 分段延期值
  * @author ZH
  * @date   2015年8月7日
  * @note   必需在延期时间回读指令后调用
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

	max_value = *ptr; //最大值
	min_value = *ptr; //最小值
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
	return (max_value - min_value);	//返回相差值
}
u8 GetBitNum(u32 sum_count, u32 bit_count)
{
	u8 bit_num = 0;
	u32 temp = 0;

	bit_num = (u8)(sum_count/bit_count);
	temp = sum_count%bit_count;
	if((temp*2) > bit_count)  //最后的余数超过一半时认为是完整的1位
	{
		bit_num++;	
	}
	return bit_num;	
}
/**
  *************************************************************************************
  * @brief  获取相应字节的偶检验位
  * @param  Byte 字节
  * @retval 奇检验位
  * @author ZH
  * @date   2015年8月7日
  * @note   无
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
  * @brief  发送雷管波形差接收返回数据
  * @param  ErrorRetry 接收错误时重试次数
  * @param  RecBytes 需要接收的字节数(同步头算其中1个字节)
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_DetRecAnalyze(void)
{
	u8 ByteIndex = 0, OddBit, EvenBit;
	u32 DetBitClk;
	u16 i,j, MaskBit, U16Value, BitNumber;
	u32 DetFreq = 0;  //芯片频率
	u32 U32Value = 0;
	double FValue = 0;
	
	//根据同步头计算出每位对应的计数值及芯片频率
	if(DetProtocol.RecBuffer.Count >= 5)
	{//至少要有3个同步头
		memset((char*)&DetProtocol.RecData.Data[0], 0, sizeof(DetProtocol.RecData.Data));
		
		if((U32DataCMP((u32*)&DetProtocol.RecBuffer.Timer[0],5)) > T40US)//得到的同步头数据中，16个CLK相差,保证不大于2周期
		{
			return ERROR;	//同步头数据不稳定
		}
		
		DetBitClk = ((u32)DetProtocol.RecBuffer.Timer[0] + (u32)DetProtocol.RecBuffer.Timer[1] + 
			(u32)DetProtocol.RecBuffer.Timer[2] + (u32)DetProtocol.RecBuffer.Timer[3] + 
			(u32)DetProtocol.RecBuffer.Timer[4])/5;//返回波形中每位对应的计数值(对应16个芯片周期)

		FValue = (double) DET_CMD_TIMER_FREQ*(double)16/(double)DetBitClk;//芯片频率，单位HZ
		DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
		if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
		{//同步头数据不稳定
			return ERROR;	
		}
		DetFreqFromHead = DetFreq;
		
		ByteIndex = 0;
		MaskBit = 0x0001;
		U16Value = 0x00;
		for(i=0;i<DetProtocol.RecBuffer.Count;i++)
		{
			if(DetProtocol.RecBuffer.Timer[i] <= (DetBitClk/2))
			{//宽度不足同步头的一半，认为是干扰
				return ERROR;
			}
			
			BitNumber = GetBitNum(DetProtocol.RecBuffer.Timer[i], DetBitClk);//转换为0或1的位个数
			for(j=0;j<BitNumber;j++)
			{
				if((i&0x01) == 0)
				{//对应高电平
					U16Value |= MaskBit; 
				}
				MaskBit <<= 1;//芯片先返回低位
				if(MaskBit&0x400)
				{//一个字节8位+2位检验位
					//先是奇检验位再偶检验位
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
						{//同步头
							return ERROR;
						}						
						if(0 != DetFreqMeasureFlagForTimeRd)
						{//当前为延时时间回读频率测量
							if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
							{//芯片收到写延期后只返回3个同步头
								if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
									(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
									(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
								{//测频前固定写入延期为0x0000FF
									U32Value = 0;
									for(i=0;i<11;i++)
									{
										U32Value += DetProtocol.RecData.Data[i];										
									}
									FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//芯片频率，单位HZ, 608 = (3*10+8)*16
									DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
									if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
									{//数据不稳定
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
					
					//开始下一个字节
					U16Value = 0x00;
					MaskBit = 0x0001;
				}			
			}
		}
		if(ByteIndex < DetProtocol.RecData.ExpentCount)
		{//与期望的字节不符
			if(((ByteIndex+1) == DetProtocol.RecData.ExpentCount) && (MaskBit&0x200))
			{//最后一位偶检验位如果是0的话，有可能刚好少接收到一次中断
				OddBit = (U16Value>>8)&0x01;
				if((OddBit == 1) && (0 == GetEvenBit(U16Value&0xFF)))
				{
					DetProtocol.RecData.Data[ByteIndex++] = U16Value&0xFF;

					if((DetProtocol.RecData.Data[0]&0x3F) != 0x15)
					{//同步头
						return ERROR;
					}					
					if(0 != DetFreqMeasureFlagForTimeRd)
					{//当前为延时时间回读频率测量
						if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
						{//芯片收到写延期后只返回3个同步头
							if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
								(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
								(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
							{//测频前固定写入延期为0x0000FF
								U32Value = 0;
								for(i=0;i<11;i++)
								{
									U32Value += DetProtocol.RecData.Data[i];										
								}
								FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//芯片频率，单位HZ, 608 = (3*10+8)*16
								DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
								if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
								{//数据不稳定
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
  * @brief  发送雷管波形差接收返回数据
  * @param  ErrorRetry 接收错误时重试次数
  * @param  RecBytes 需要接收的字节数(同步头算其中1个字节)
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
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
		{//错误重发间隔
			OSTimeDlyHMSM(0,0,0,20);//20ms	
		}
		
		DetProtocol.RecBuffer.Count = 0;
		DetProtocol.RecData.ExpentCount = RecBytes;//要接收到字节数量
		DetProtocol.SendBuffer.Index = 0;
		DetProtocol.SendBuffer.State = DET_SEND;
		
		SET_LIN_OPEN();
				
		DetTimerConfig();  //配置定时器2
		NVIC_EnableIRQ(TIM2_IRQn);//使能TIM2中断
		TIM_ARRPreloadConfig(TIM2, ENABLE);//开启ARR预装载缓冲器	
		TIM_SetCounter(TIM2, 0xFFFFFFFFUL-DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);				
		TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);		
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
		SET_LIN_REVERSE();
		
		TIM_Cmd(TIM2, ENABLE);  //开启时钟					// 启动发送 		
		
		while(DET_SEND == DetProtocol.SendBuffer.State) { ; }// 等待全部指令码发送完成
		if(DetProtocol.RecData.ExpentCount != 0)
		{//需要接收
            u8 bitCount = 10;            
			WaitRecFlag = 1;
			while(WaitRecFlag)
			{
				WaitRecFlag = 0;
				U16Value1 = DetProtocol.RecBuffer.Count;
				c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
				while(OSTimeGet() < (c_time+7/(1000/OS_TICKS_PER_SEC)))//5ms内都没有收到边沿，说明后续也不会再有数据或者是缓冲区已经满
				{
					U16Value2 = DetProtocol.RecBuffer.Count;
					if(U16Value2 != U16Value1)
					{//接收新的边沿，重新计时
						WaitRecFlag = 1;
						break;
					}
					else if((U16Value2/bitCount) >= DetProtocol.RecData.ExpentCount)
					{//接收完成
						SystemDelay100us(3);
						break;
					}				
				}
			}
		}

		// 复位各状态		
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		TIM_Cmd(TIM2, DISABLE);  //关闭时钟
		TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清除溢出中断标志
		NVIC_DisableIRQ(TIM2_IRQn);//失能外部中断
	
		ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, DISABLE);		
		SET_LIN_OPEN();	
		//分析接收到的数据
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
  * @brief  发送雷管波形差接收返回数据
  * @param  ErrorRetry 接收错误时重试次数
  * @param  RecBytes 需要接收的字节数(同步头算其中1个字节)
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_DetSendAndRec(u8 ErrorRetry, u8 RecBytes)
{ 
	return DetSendAndRec(ErrorRetry, RecBytes, XA2_DET);
}
