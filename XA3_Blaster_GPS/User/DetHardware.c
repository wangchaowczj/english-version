#include "includes.h"

static u8 DetCmdDelayInterval = 1;
static u32 DetFreqFromHead = 0;  //芯片频率
static u32 DetFreqFromTimeRd = 0;  //芯片频率

DET_PROTOCOL DetProtocol;
u32 TestErrorCount = 0;

/**
  *************************************************************************************
  * @brief  打开发送命令时的延期间隔
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年03月14日
  * @note   交变沿触发
  *************************************************************************************  
  */
void OpenDetCmdDelayInterval(void)
{
	DetCmdDelayInterval = 1;
}
/**
  *************************************************************************************
  * @brief  关闭发送命令时的延期间隔
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年03月14日
  * @note   交变沿触发
  *************************************************************************************  
  */
void CloseDetCmdDelayInterval(void)
{
	DetCmdDelayInterval = 0;
}
/**
  *************************************************************************************
  * @brief  配置外部中断方式
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年07月15日
  * @note   交变沿触发
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
//| 函数名称 | DetSendConfig
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置GPIO
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DetSendConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(DET_RECEIVE_RES_PORT_RCC | BLAST_SIGNAL_SEND_PORT_RCC | DET_CMD_SEND_PORT_RCC | 
	DOUBLE_KEY_SW_PORT_RCC|ADJUST_G_PORT_RCC|DOUBLE_KEY1_SIM_SW_PORT_RCC|DOUBLE_KEY2_SIM_SW_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***发送与接收状态切换管脚**/
	GPIO_InitStructure.GPIO_Pin = DET_RECEIVE_RES_PIN; 
	GPIO_Init(DET_RECEIVE_RES_PORT, &GPIO_InitStructure);
	
	/***雷管命令发送管脚**/
	GPIO_InitStructure.GPIO_Pin = BLAST_SIGNAL_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(BLAST_SIGNAL_SEND_PORT, &GPIO_InitStructure);
	
	/***雷管命令发送管脚**/
	GPIO_InitStructure.GPIO_Pin = DET_CMD_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(DET_CMD_SEND_PORT, &GPIO_InitStructure);
		
	/***作为起爆器时起爆双键电子控制IO**/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY_SW_PIN;  
	GPIO_Init(DOUBLE_KEY_SW_PORT, &GPIO_InitStructure);	

	/***作为从机时模拟双键控制使能开关**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY1_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY1_SIM_SW_PORT, &GPIO_InitStructure);

	/***作为从机时模拟双键控制使能开关**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY2_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY2_SIM_SW_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
	
	/***比较器复位管脚**/
	GPIO_InitStructure.GPIO_Pin = ADJUST_G_PIN; 
	GPIO_Init(ADJUST_G_PORT, &GPIO_InitStructure);	
	
	CLOSE_DOUBLE_KEY_SW();
	CLOSE_DOUBLE_KEY1_SIM_SW();
	CLOSE_DOUBLE_KEY2_SIM_SW();
	
	/***雷管命令发送管脚初始电平**/
	DET_COM_SEND_PIN_INIT();
	
	/***允许发送命令**/
	CLOSE_DOUBLE_KEY_SW();	
	
	/***起爆信号初始电平**/
	BLAST_SIGNAL_PIN_INIT();	
	
	/***接收电阻默认为打开**/
	OPEN_DET_RECEIVE_RES();
	
	/***接收比较器默认为高**/
	ADJUST_G_RESET();
}

//==================================================================================================
//| 函数名称 | DetReceiverConfig
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置GPIO,PC.6 PC.7作为接收雷管信号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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

	/*外部下降沿中断*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line6; //FALL
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
			
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;	//RISE
	EXTI_Init(&EXTI_InitStructure);

	
	//FALL,RISE信号，外部下降沿中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
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
	DetSendConfig();
	DetReceiverConfig();
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
  * @brief  延期写入标志
  * @param  无 
  * @retval 芯片频率，单位HZ
  * @author ZH
  * @date   2015年8月7日
  * @note   必需在延期时间回读指令后调用
  *************************************************************************************  
  */
u8 GetDetWriteDelayFLag(void)
{
	return (DetProtocol.RecData.Data[3]&0x80);
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

//	max_value = *ptr; //最大值
//	min_value = *ptr; //最小值
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
//	return (max_value - min_value);	//返回相差值
//}
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
u8 DetRecAnalyze(u8 FreqMeasureFlagForTimeRd)
{
	u8 ByteIndex = 0, OddBit, EvenBit;
	u16 i,j, MaskBit, U16Value, DetBitClk, BitNumber;
	u32 DetFreq = 0;  //芯片频率
	u32 U32Value = 0;
	double FValue = 0;

	//相当于把2路信号还原回1路信号,不用考虑RISE刚好比FALL多一个的情况，因为结束接收的条件已经保证了肯定能接收到所有数据
	DetProtocol.RecBuffer.Count = 0;
	for(i=0;i < DetProtocol.Fall.Count;i++)
	{
		DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = DetProtocol.Fall.Timer[i] - DetProtocol.Rise.Timer[i];//对应高电平
		if((i == (DetProtocol.Fall.Count-1)) &&
			(DetProtocol.Fall.Count == DetProtocol.Rise.Count))
		{				
			break;
		}			
		DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = DetProtocol.Rise.Timer[i+1] - DetProtocol.Fall.Timer[i];//对应低电平
	}
	
	//根据同步头计算出每位对应的计数值及芯片频率
	if(DetProtocol.RecBuffer.Count >= 5)
	{//至少要有3个同步头
		memset((char*)&DetProtocol.RecData.Data[0], 0, sizeof(DetProtocol.RecData.Data));
		
		if((U32DataCMP((u32*)&DetProtocol.RecBuffer.Timer[0],5)) > T40US)//得到的同步头数据中，16个CLK相差,保证不大于2周期
		{
			return 1;	//同步头数据不稳定
		}
		U32Value = DetProtocol.Fall.Timer[2]-DetProtocol.Rise.Timer[0];
		DetBitClk = U32Value/5;//返回波形中每位对应的计数值(对应16个芯片周期)

		FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)80/(double)U32Value;//芯片频率，单位HZ
		DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
		if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
		{//同步头数据不稳定
			return 2;	
		}
		DetFreqFromHead = DetFreq;
		
		ByteIndex = 0;
		MaskBit = 0x0001;
		U16Value = 0x00;
		for(i=0;i<DetProtocol.RecBuffer.Count;i++)
		{
			if(DetProtocol.RecBuffer.Timer[i] <= (DetBitClk/2))
			{//宽度不足同步头的一半，认为是干扰
				return 3;
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
						return 4;
					}

					DetProtocol.RecData.Data[ByteIndex++] = U16Value&0xFF;					
					
					if(ByteIndex >= DetProtocol.RecData.ExpentCount)
					{
						if((DetProtocol.RecData.Data[0]&0x3F) != 0x15)
						{//同步头必需是3个或者4个
							return 5;
						}
						
						if((0 != FreqMeasureFlagForTimeRd) && (DetProtocol.RecData.ExpentCount == 6))
						{//当前为延时时间回读频率测量
							if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
							{//芯片收到写延期后只返回3个同步头
								if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
									(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
									(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
								{//测频前固定写入延期为0x0000FE
									U32Value = DetProtocol.Fall.Timer[7]-DetProtocol.Rise.Timer[0];

									FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//芯片频率，单位HZ, 608 = (3*10+8)*16
									DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
									if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
									{//数据不稳定
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
					{//同步头必需是3个或者4个
						return 8;
					}					

					if((0 != FreqMeasureFlagForTimeRd) && (DetProtocol.RecData.ExpentCount == 6))
					{//当前为延时时间回读频率测量
						if((DetProtocol.RecData.Data[0]&0x7F) == 0x55)
						{//芯片收到写延期后只返回3个同步头
							if((DetProtocol.RecData.Data[1] == (DET_FREQ_MEASURE_WRITE_VALUE&0xFF))&&
								(DetProtocol.RecData.Data[2] == ((DET_FREQ_MEASURE_WRITE_VALUE>>8)&0xFF))&&
								(DetProtocol.RecData.Data[3] == (((DET_FREQ_MEASURE_WRITE_VALUE>>16)&0xFF)|0x80)))
							{//测频前固定写入延期为0x0000FE
								U32Value = DetProtocol.Fall.Timer[7]-DetProtocol.Rise.Timer[0];
								
								FValue = (uint64_t) DET_CMD_TIMER_FREQ*(uint64_t)608/(double)U32Value;//芯片频率，单位HZ, 608 = (3*10+8)*16
								DetFreq = (u32)(FValue+0.5);//芯片频率，单位HZ(四舍五入)
								if((DetFreq < DET_RECEIVE_FREQ_MIN) || (DetFreq > DET_RECEIVE_FREQ_MAX))//40K-120K
								{//数据不稳定
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
  * @brief  对接收到的原始信号重新进行滤波分析
  * @param  FreqMeasureFlagForTimeRd 测频标志
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
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
    //对原始信号重新进行滤波后再尝试分析    
    //先确定同步头的宽度，然后其他每一位的宽度应该是同步头宽度的整数倍(允许一定的偏差)
    for(i=0; i<DetProtocol.RealRise.Count;i++)
    {
        for(j=0; j<DetProtocol.RealFall.Count;j++)
        {
            if(DetProtocol.RealFall.Timer[j] > DetProtocol.RealRise.Timer[i])
            {
                HeadWith = DetProtocol.RealFall.Timer[j] - DetProtocol.RealRise.Timer[i];
                if ((HeadWith > 11200) && (HeadWith < 26880))//每位同步头对应的宽度是133us-320us，50-120K时
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
                                    if((BitCount < 1) || (BitCount >= 11))//根据协议，返回波形为奇偶校验，所以不会连续11位0或者1
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
ErrorStatus DetSendAndRec(u8 ErrorRetry, u8 RecBytes, u8 FreqMeasureFlagForTimeRd)
{ 
	u8 ResultCode = 0 ;
	u8 U16Value1, U16Value2;
	u32 c_time;
	u8 WaitRecFlag = 0;
	
    CLOSE_DET_RECEIVE_RES();//短路接收电阻
	if(DetCmdDelayInterval)
	{
		if((DET_BLAST == DetProtocol.Mode) || (DET_CMD_BLAST == DetProtocol.Mode))
		{
			System168MDelay1ms(5);//延时一致性更好
		}
		else
		{
			OSTimeDlyHMSM(0,0,0,20);//其他任务有机会运行
		}	
	}
	
	do
	{
        CLOSE_DET_RECEIVE_RES();//短路接收电阻
		if(0 != ResultCode)
		{//错误重发间隔
			OSTimeDlyHMSM(0,0,0,10);//10ms	
		}

		DetProtocol.Fall.Count = 0;
		DetProtocol.Rise.Count = 0;
		DetProtocol.RealFall.Count = 0;
		DetProtocol.RealRise.Count = 0;        
		DetProtocol.RecBuffer.Count = 0;
		DetProtocol.RecData.ExpentCount = RecBytes;//要接收到字节数量
		DetProtocol.SendBuffer.Index = 0;
		DetProtocol.SendBuffer.State = DET_SEND;

		/***雷管命令发送管脚初始电平**/
		DET_COM_SEND_PIN_INIT();
		
		/***起爆信号初始电平**/
		BLAST_SIGNAL_PIN_INIT();				
		
		DetTimerConfig();  //配置定时器2
		NVIC_EnableIRQ(TIM2_IRQn);//使能TIM2中断
		TIM_ARRPreloadConfig(TIM2, ENABLE);//开启ARR预装载缓冲器	
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
		
		TIM_Cmd(TIM2, ENABLE);  //开启时钟					// 启动发送 		
		
		while(DET_SEND == DetProtocol.SendBuffer.State)// 等待全部指令码发送完成
		{
			if(DET_BLAST == DetProtocol.Mode)
			{//发送起爆信号时，记录端口电压状态
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
		{//需要接收
			WaitRecFlag = 1;
			while(WaitRecFlag)
			{
				WaitRecFlag = 0;
				c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
				U16Value1 = DetProtocol.Fall.Count;//
				while(OSTimeGet() < (c_time+7/(1000/OS_TICKS_PER_SEC)))//7ms内都没有收到边沿，说明后续也不会再有数据或者是缓冲区已经满
				{
					U16Value2 = DetProtocol.Fall.Count;
					if(U16Value2 != U16Value1)
					{//接收新的边沿，重新计时
						WaitRecFlag = 1;
						break;
					}
					else if((U16Value2/5) >= DetProtocol.RecData.ExpentCount)
					{//接收完成
						System168MDelay100us(3);
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
		ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, DISABLE);
		ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, DISABLE);
		
		if((DET_CMD == DetProtocol.Mode)||(DET_FIXED_PULSE == DetProtocol.Mode))
		{
			SET_LV_6_7V(); //切换到正常状态	
		}
		else
		{
			;	
		}		
		/***雷管命令发送管脚初始电平**/
		DET_COM_SEND_PIN_INIT();
		
		/***起爆信号初始电平**/
		BLAST_SIGNAL_PIN_INIT();
        
		if(DetProtocol.RecData.ExpentCount == 0)
        {//不接收数据时,晚一点晚到电阻
            System168MDelay10us(2);        
        }

        OPEN_DET_RECEIVE_RES();
        
		//分析接收到的数据
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
				//记录当前数据到日志中，便于分析问题
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
                        if(FilterAnalyzeRetry(FreqMeasureFlagForTimeRd) == ERROR)//对接收到的原始信号重新进行过滤解析
                        {
                            WriteSystemLog("重新过滤后依然错误", strlen("重新过滤后依然错误"), STR_INFO);
                        }
                        else
                        {
                            ResultCode = 0;
                            WriteSystemLog("重新过滤后正确", strlen("重新过滤后正确"), STR_INFO);
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
