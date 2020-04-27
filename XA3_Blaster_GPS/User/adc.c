#include "includes.h"

ADC_ADJUST adc_adjust;				//ADC校准参数
u16 DetWave[DET_WAVE_POINT_COUNT];
static  OS_EVENT   *AdcMutex;

/**@defgroup ADC_Functions
  *@brief 函数
  *@{
  */

static void AdcLock(void)
{
	INT8U  err;
	
	if(AdcMutex == 0)
	{
		AdcMutex = OSMutexCreate(RES_ADC_MUTEX_PRIO, &err);
	}
	
	OSMutexPend(AdcMutex,0,&err);
}
static void AdcUnlock(void)
{	
	OSMutexPost(AdcMutex);
}
//==================================================================================================
//| 函数名称 | ADCConfig 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | ADC1和3功能配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void ADCConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	/****配置GPIO时钟****/
	 RCC_AHB1PeriphClockCmd(ADC_COMM_PORT_RCC | ADC_6_7V_PORT_RCC | ADC_LOW1_I_PORT_RCC | 
		ADC_LOW2_I_PORT_RCC | ADC_11_3V_PORT_RCC | 
		ADC_LINA_PORT_RCC | ADC_LINB_PORT_RCC | ADC_5V_PORT_RCC | 
		ADC_BAT_PORT_RCC|I_TEST_PORT_RCC, ENABLE);
	
	/****配置GPIO****/
	GPIO_InitStructure.GPIO_Pin = I_TEST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I_TEST_PORT, &GPIO_InitStructure);
	SET_MEASURE_LOW();
	
	GPIO_InitStructure.GPIO_Pin = ADC_COMM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(ADC_COMM_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_6_7V_PIN;
	GPIO_Init(ADC_6_7V_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_LOW1_I_PIN;
	GPIO_Init(ADC_LOW1_I_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_LOW2_I_PIN;
	GPIO_Init(ADC_LOW2_I_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_11_3V_PIN;
	GPIO_Init(ADC_11_3V_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_LINA_PIN;
	GPIO_Init(ADC_LINA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_LINB_PIN;
	GPIO_Init(ADC_LINB_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = ADC_5V_PIN;
	GPIO_Init(ADC_5V_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_BAT_PIN;
	GPIO_Init(ADC_BAT_PORT, &GPIO_InitStructure);

	
	/****配置ADC1时钟****/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 , ENABLE);
	
	/****配置ADC3时钟****/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC3 , ENABLE);	

	/****配置ADC模式****/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//不使用DMA
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStructure); 
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //采样结果为12位
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //1次只采集1个通道
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//不连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不使用外部触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1; //要使用的AD通道数
	ADC_Init(ADC1, &ADC_InitStructure);	
	ADC_Init(ADC3, &ADC_InitStructure);	

	/****启用ADC1****/
	ADC_Cmd(ADC1, ENABLE);
	
	/****启用ADC3****/
	ADC_Cmd(ADC3, ENABLE);	
}

//==================================================================================================
//| 函数名称 | GetADCValue 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据输入的通道获取对应AD值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ADC_Channel_x取值范围：
//|          | ADC_CH_BAT        电池电压测量
//|          | ADC_CH_5V         +5V电压测量
//|          | ADC_CH_COMM       COMM保护输出电压测量
//|          | ADC_CH_6_7V       +6.7V电压测量
//|          | ADC_CH_LOW1_I     低压电流测量1路
//|          | ADC_CH_LOW2_I     低压电流测量2路
//|          | ADC_CH_HIGH1_I    高压电流测量1路
//|          | ADC_CH_HIGH2_I    高压电流测量2路
//|          | ADC_CH_11_3V      +11.3V电压测量
//|          | ADC_CH_11_3VOUT   +11.3V保护输出电压测量
//|          | ADC_CH_LINA       总线A电压测量
//|          | ADC_CH_LINB       总线B电压测量
//|          | retry 测量次数，取值范围1-10
//|          | 测量次数不大于5时，返回值为平均值，大于5次时舍去2个最大值2个最小值后再求平均值
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | AD测量值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetADCValue(u8 ADC_Channel_x, u8 retry)
{
	u16 ad_value = 0;
	ADC_TypeDef* ADCx;
	u16 ad_temp[10], rank_temp[10];
	u8 i;
	
	if(retry == 0)
	{
		return 0;
	}
	else if(retry > 10)
	{
		retry = 10;
	}
	
	switch(ADC_Channel_x)
	{
		case ADC_CH_6_7V:
		case ADC_CH_LOW1_I:
		case ADC_CH_LOW2_I:
			ADCx = ADC3;
			break;
		default:
			ADCx = ADC1;
			break;
	}
	
	/****配置要转换的通道****/
	ADC_RegularChannelConfig(ADCx, ADC_Channel_x&0x3F, 1, ADC_SampleTime_3Cycles);
	/****软件启动转换****/
	ADC_SoftwareStartConv(ADCx);   
	
	/****等待软件完成****/
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC)!=SET);
	
	/****读取测量值，同时自动清除标志位****/
	ADC_GetConversionValue(ADCx);
	for(i=0; i<retry; i++)
	{			
		/****软件启动转换****/
		ADC_SoftwareStartConv(ADCx);   
		
		/****等待软件完成****/
		while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC)!=SET);
		
		/****读取测量值，同时自动清除标志位****/
		ad_temp[i] = ADC_GetConversionValue(ADCx);		
		ad_value += ad_temp[i];
		if(i != (retry-1))//最后一次不用再延时
		{
			System168MDelay10us(2);
		}
	}

	if(retry > 5)
	{
		ad_value = 0;
		U16DataFallRank(ad_temp, rank_temp, retry);//从大到小排序
		for(i=2; i < retry-2; i++)//去掉2个最大值、2个最小值后求平均值
		{
			ad_value += rank_temp[i];
		}
		ad_value /= (retry-4);
	}
	else
	{
		ad_value /= retry;
	}
	
	return (ad_value);	
}
void GetDetWaveADCValue(void)
{
	u16 i;
		
	AdcLock();
	SET_MEASURE_LOW();
	OPEN_DET_RECEIVE_RES();
	
	SET_LV_5_0V();//切换到5V
	System168MDelay1ms(1);//AD采样电容放电
	
	/****配置要转换的通道****/
	ADC_RegularChannelConfig(ADC3, ADC_CH_LOW1_I&0x3F, 1, ADC_SampleTime_3Cycles);
	/****软件启动转换****/
	ADC_SoftwareStartConv(ADC3);   
	
	/****等待软件完成****/
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
	
	/****读取测量值，同时自动清除标志位****/
	ADC_GetConversionValue(ADC3);
	for(i=0; i<DET_WAVE_POINT_COUNT; i++)
	{			
		/****软件启动转换****/
		ADC_SoftwareStartConv(ADC3);   
		
		/****等待软件完成****/
		while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
		
		/****读取测量值，同时自动清除标志位****/
		DetWave[i] = ADC_GetConversionValue(ADC3);
		if(i != (DET_WAVE_POINT_COUNT-1))//最后一次不用再延时
		{
			System168MDelay10us(2);
		}
	}
	SET_LV_6_7V();	//切换到6.7
	AdcUnlock();//释放资源	
}
/**
  *************************************************************************************
  * @brief  ADC测量值转换成电压或者电流
  * @param[in]  Adc：ADC测量值  
  * @param[in]  Adjust：校准参数
  * @param[in]  Default：默认参数  
  * @retval 电压或者电流值 
  * @author ZH
  * @date   2018年7月9日
  * @note   
  *************************************************************************************  
  */
u16 Adc2VoltageOrCurrent(u16 Adc, ADJUST_PARAMETER* Adjust, float Default)
{
	int value;
	
	if((u16)(Adjust->k*1000) == 0)
	{
		value = (int)(Adc*Default);
	}
	else
	{
		if(Adjust->b&0x8000)
		{
			value = (int)((float)Adc*Adjust->k - (Adjust->b&0x7FFF));
		}
		else
		{
			value = (int)((float)Adc*Adjust->k + (Adjust->b&0x7FFF));
		}		
	}
	if(value < 0)
	{
		value = 0;
	}
	return value;
}
//==================================================================================================
//| 函数名称 | GetBatteryVoltage 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取电池电压值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 |电池电压值，单位mV
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 经过校准
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetBatteryVoltage(u16* AdcValue)
{
	u16 Value;
	double  bat;
	
	AdcLock();
	
	Value = GetADCValue(ADC_CH_BAT,10);
	if(Value != NULL)
	{
		*AdcValue = Value;
	}
	bat = Adc2VoltageOrCurrent(Value, &adc_adjust.bat, DEFAULT_ADC_PARAM);
	
	AdcUnlock();//释放资源
	
	return ((u16)bat);
}
/**
  *************************************************************************************
  * @brief  LINA-LINB之间的AD测量差值
  * @param  无
  * @retval AD测量差值
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
s16 GetLinABAdcValue(void)
{
	u16 lina,linb;
	s16 value = 0;

	AdcLock();//申请资源
	lina = GetADCValue(ADC_CH_LINA, 1);
	linb = GetADCValue(ADC_CH_LINB, 1);	
	value = lina - linb;
	AdcUnlock();//释放资源
	return value;
}
/**
  *************************************************************************************
  * @brief  根据ADC测量值，计算总线电压值(有正负值)
  * @param  无
  * @retval 电压值
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
s16 GetBusVoltageFromAdcValue(s16 AdcValue)
{
	u16 Value = 0;
	int voltage;
	
	if(AdcValue<0)
	{
		Value = -AdcValue;
	}
	else
	{
		Value = AdcValue;
	}

	if((u16)(adc_adjust.linab.k*1000) == 0)
	{
		voltage = (int)(Value*DEFAULT_ADC_PARAM);
	}
	else
	{
		if(adc_adjust.linab.b&0x8000)
		{
			voltage = (s16)((float)Value*adc_adjust.linab.k - (adc_adjust.linab.b&0x7FFF));
		}
		else
		{
			voltage = (s16)((float)Value*adc_adjust.linab.k + (adc_adjust.linab.b&0x7FFF));
		}	
	}	
	
	if(voltage<0)
	{
		voltage = 0;
	}
	
	if(AdcValue < 0)
	{
		voltage = -voltage;
	}
	
	return voltage;
}
//==================================================================================================
//| 函数名称 | GetBusVoltage 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取LINA和LINB间电压差值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 电池电压值，单位mV
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetBusVoltage(u8 retry, u16* AdcValue)
{
	u16 value = 0;
	u8 i;
	u16 ad_temp[10], rank_temp[10];

	
	if ((retry == 0) || (retry >10))
	{
		retry = 10;
	}
	for(i = 0; i<retry; i++)
	{
		ad_temp[i] = abs(GetLinABAdcValue());	
		if((i+1) <retry)
		{
			System168MDelay1ms(1);		
		}		
	}
	if(retry > 5)
	{
		U16DataFallRank(ad_temp, rank_temp, retry);//从大到小排序
		for(i=2; i < retry-2; i++)//去掉2个最大值、2个最小值后求平均值
		{
			value += rank_temp[i];
		}
		value /= (retry-4);		
	}
	else
	{
		for(i=0; i < retry; i++)//求平均值
		{
			value += ad_temp[i];
		}
		value /= retry;
	}
	if(AdcValue != NULL)
	{
		*AdcValue = value;
	}
	return abs(GetBusVoltageFromAdcValue(value));
}


/**
  *************************************************************************************
  * @brief  判断总线是否已经短路
  * @param[in]  无
  * @retval 1 已经短路， 0 没有短路 
  * @author ZH
  * @date   2018年7月6日
  * @note   无
  *************************************************************************************  
  */
u8 BusIsShort(void)
{
	u16 lina,linb;

	AdcLock();//申请资源
	lina = GetADCValue(ADC_CH_LINA, 10);
	linb = GetADCValue(ADC_CH_LINB, 10);	
	AdcUnlock();//释放资源
	lina = lina*DEFAULT_ADC_PARAM;
	linb = linb*DEFAULT_ADC_PARAM;
	if ((lina > 1000) && (linb > 1000))
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  判断总线电流是否量程范围内
  * @param[in]  BusCurrent 当前总线电流
  * @retval 0 不在范围内， 1 在范围内 
  * @author ZH
  * @date   2018年7月6日
  * @note   无
  *************************************************************************************  
  */
u8 BusCurrentInRange(u16 BusCurrent)
{
	u16 MaxCurrent;
	
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//当前总线电压已经关闭
		return 1;
	}
	if(GET_HV_SW_CTRL_STATE() == 0)
	{//高压还没输出
		MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);
	}
	else
	{
		MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.high2current, DEFAULT_I_HIGH2_PARAM);
	}
	if(BusCurrent >= MaxCurrent)
	{
		return 0;
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  获取当前总线电流量程最大值
  * @param[in]  无
  * @retval 当前总线电流量程最大值
  * @author ZH
  * @date   2018年7月6日
  * @note   无
  *************************************************************************************  
  */
u16 BusCurrentRangeMax(void)
{
	u16 MaxCurrent;
	
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//当前总线电压已经关闭
		MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);
	}
	else
	{
		if(GET_HV_SW_CTRL_STATE() == 0)
		{//高压还没输出
			MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);
		}
		else
		{
			MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.high2current, DEFAULT_I_HIGH2_PARAM);
		}	
	}

	return MaxCurrent;
}


//==================================================================================================
//| 函数名称 | GetChannelxVoltage 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取某个通道的电压值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ADC_CH_BAT        电池电压测量
//|          | ADC_CH_5V         +5V电压测量
//|          | ADC_CH_COMM       COMM保护输出电压测量
//|          | ADC_CH_6_7V       +6.7V电压测量
//|          | ADC_CH_11_3V      +11.3V电压测量
//|          | ADC_CH_11_3VOUT   +11.3V保护输出电压测量
//|----------|--------------------------------------------------------------------------------------        
//| 返回参数 |电池电压值，单位mV
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 未经过校准
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry)
{
	double bat;
	
	AdcLock();//申请资源
	
	bat = (double)GetADCValue(ADC_Channel_x, retry);
	bat *= DEFAULT_ADC_PARAM ;//分压比80.6K/20K
	
	AdcUnlock();//释放资源
	
	return ((u16)bat);
}

//==================================================================================================
//| 函数名称 | GetBusLowCurrent 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取LIN低压下电流值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 电流值，单位uA
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetBusLowCurrent(u8 rd_count, u16* AdcValue)
{
	u16 value;

	AdcLock();//申请资源
	
	SET_MEASURE_LOW();
	
	value = GetADCValue(ADC_CH_LOW1_I,rd_count);//判断当前处于小量程还是大量程
	if(NULL != AdcValue)
	{
		*AdcValue = value;	//返回ADC测量值，主要用于电流校准
	}

	if(value < 3351)//小量程满量程是对应3V，大于90%时(对应2.7V)切换量程：3*90%/3.3*4096 = 3351
	{
		value = Adc2VoltageOrCurrent(value, &adc_adjust.low1current, DEFAULT_I_LOW1_PARAM);		
	}
	else
	{
		value = GetADCValue(ADC_CH_LOW2_I,rd_count);
		if(NULL != AdcValue)
		{
			*AdcValue = value;	//返回ADC测量值，主要用于电流校准
		}
		value = Adc2VoltageOrCurrent(value, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);		
	}
	
	AdcUnlock();//释放资源
	
	return value;
}


//==================================================================================================
//| 函数名称 | GetLinHighCurrent 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取LIN高压下电流值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 电流值，单位uA
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetBusHighCurrent(u8 rd_count, u16* AdcValue)
{
	u16 value;

	AdcLock();//申请资源
	
	SET_MEASURE_HIGH();
	
	value = GetADCValue(ADC_CH_LOW1_I,rd_count);//判断当前处于小量程还是大量程
	if(NULL != AdcValue)
	{
		*AdcValue = value;	//返回ADC测量值，主要用于电流校准
	}	
	if(value  <  3351)//小量程满量程是对应3V，大于90%时(对应2.7V)切换量程：3*90%/3.3*4096 = 3351
	{
		value = Adc2VoltageOrCurrent(value, &adc_adjust.high1current, DEFAULT_I_HIGH1_PARAM);	
	}
	else
	{
		value = GetADCValue(ADC_CH_LOW2_I,rd_count);
		if(NULL != AdcValue)
		{
			*AdcValue = value;	//返回ADC测量值，主要用于电流校准
		}
		value = Adc2VoltageOrCurrent(value, &adc_adjust.high2current, DEFAULT_I_HIGH2_PARAM);	
	}
		
	AdcUnlock();//释放资源
	
	return value;
}

u16 GetBusCurrent(u8 rd_count, u16* AdcValue)
{
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//当前总线电压已经关闭
		return 0;
	}
//	while(GET_DET_RECEIVE_RES_STATE() != DET_RECEIVE_RES_ON)
//	{
//		OSTimeDly(1);
//	}
	if(GET_HV_SW_CTRL_STATE() == 0)
	{//高压还没输出
		return GetBusLowCurrent(rd_count, AdcValue);
	}
	else
	{
		return GetBusHighCurrent(rd_count, AdcValue);
	}
}
//==================================================================================================
//| 函数名称 | GetBusLeakCurrent 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取总线漏电流
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 总线漏电流
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-05 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetBusLeakCurrent(u16* AdcValue)
{
	u8 i;
	u16 ad_temp[10], rank_temp[10];
	s32 value, ad_value = 0;

//	while(GET_DET_RECEIVE_RES_STATE() != DET_RECEIVE_RES_ON)
//	{
//		OSTimeDly(1);
//	}
	AdcLock();//申请资源
	SET_MEASURE_LOW();
//	CLOSE_DET_RECEIVE_RES();//短路接收电阻
//	OSTimeDlyHMSM(0,0,2,0);
	OPEN_DET_RECEIVE_RES();
	
	SET_LV_5_0V();//切换到5V
	System168MDelay1ms(2);//AD采样电容放电
	for(i=0; i<10; i++)
	{
		System168MDelay100us(5);//延时500us
		//ad_temp[i] = GetBusLowCurrent(1, NULL);不能使用这个，因为在该函数中无法申请到ADC资源
		value = GetADCValue(ADC_CH_LOW1_I,1);//判断当前处于小量程还是大量程
		if(value < 3537)//小量程满量程是对应3V，大于95%时切换量程：3*95%/3.3*4096 = 3537
		{
			value = Adc2VoltageOrCurrent(value, &adc_adjust.low1current, DEFAULT_I_LOW1_PARAM);			
		}
		else
		{
			value = GetADCValue(ADC_CH_LOW2_I,1);
			value = Adc2VoltageOrCurrent(value, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);		
		}
		if(value<0)
		{
			value = 0;
		}		
		ad_temp[i] = (u16)value;
	}
	SET_LV_6_7V();	//切换到6.7
	
	
	U16DataFallRank(ad_temp, rank_temp, 10);//从大到小排序
	for(i=2; i < 8; i++)//去掉2个最大值、2个最小值后求平均值
	{
		ad_value += rank_temp[i];
	}
	ad_value /= 6;
	
	AdcUnlock();//释放资源
	if(NULL != AdcValue)
	{
		*AdcValue = ad_value;
	}
	return ad_value;
}

//==================================================================================================
//| 函数名称 | CheckHVProtectPoint 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检测高压保护点电压是否正常
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | time_out 检测电压超时时间，单位ms
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 0    电压正常 
//|          | 1    电压异常
//|          | 0xFF 未检测完成
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-05 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus CheckHVProtectPoint(void)
{
	u16 temp;
	u32 r_t;

	WriteSystemLog("开始检测高压芯片是否正常",strlen("开始检测高压芯片是否正常"), NEW_STR_INFO);
	WriteSystemLog("目标电压值:",sizeof("目标电压值:"), NEW_STR_INFO);	
	WriteSystemLog(&Op_Data.LinNormalHighVoltageGoal,sizeof(Op_Data.LinNormalHighVoltageGoal), U16_DATA);
	
	//测量输出电压是否正常
	r_t = OSTimeGet();//保存当前时间	
	while(OSTimeGet() <= (r_t+10000/(1000/OS_TICKS_PER_SEC)))
	{
		temp = GetChannelxVoltage(ADC_CH_11_3V, 3);
//		if((LIN_NORMAL_HIGH_VOLTAGE_L <= temp) && (temp <= LIN_NORMAL_HIGH_VOLTAGE_H))
		if (abs(temp-Op_Data.LinNormalHighVoltageGoal) < 100)
		{
			OSTimeDlyHMSM(0,0,0,500);
			temp = GetChannelxVoltage(ADC_CH_11_3V, 10);
			if (abs(temp-Op_Data.LinNormalHighVoltageGoal) < 100)
			{
				WriteSystemLog("高压芯片正常!",strlen("高压芯片正常!"), NEW_STR_INFO);
				return SUCCESS;//电压正常
			}
		}

		OSTimeDlyHMSM(0,0,0,100);		
	}

	WriteSystemLog("高压电路异常,电压值:",strlen("高压电路异常,电压值:"), NEW_STR_INFO);
	WriteSystemLog(&temp,sizeof(temp), U16_DATA);			
	return ERROR ;//电压异常
}


