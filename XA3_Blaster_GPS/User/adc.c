#include "includes.h"

ADC_ADJUST adc_adjust;				//ADCУ׼����
u16 DetWave[DET_WAVE_POINT_COUNT];
static  OS_EVENT   *AdcMutex;

/**@defgroup ADC_Functions
  *@brief ����
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
//| �������� | ADCConfig 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ADC1��3��������
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
void ADCConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	/****����GPIOʱ��****/
	 RCC_AHB1PeriphClockCmd(ADC_COMM_PORT_RCC | ADC_6_7V_PORT_RCC | ADC_LOW1_I_PORT_RCC | 
		ADC_LOW2_I_PORT_RCC | ADC_11_3V_PORT_RCC | 
		ADC_LINA_PORT_RCC | ADC_LINB_PORT_RCC | ADC_5V_PORT_RCC | 
		ADC_BAT_PORT_RCC|I_TEST_PORT_RCC, ENABLE);
	
	/****����GPIO****/
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

	
	/****����ADC1ʱ��****/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 , ENABLE);
	
	/****����ADC3ʱ��****/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC3 , ENABLE);	

	/****����ADCģʽ****/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//��ʹ��DMA
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStructure); 
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //�������Ϊ12λ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //1��ֻ�ɼ�1��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//������ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ʹ���ⲿ����
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1; //Ҫʹ�õ�ADͨ����
	ADC_Init(ADC1, &ADC_InitStructure);	
	ADC_Init(ADC3, &ADC_InitStructure);	

	/****����ADC1****/
	ADC_Cmd(ADC1, ENABLE);
	
	/****����ADC3****/
	ADC_Cmd(ADC3, ENABLE);	
}

//==================================================================================================
//| �������� | GetADCValue 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���������ͨ����ȡ��ӦADֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ADC_Channel_xȡֵ��Χ��
//|          | ADC_CH_BAT        ��ص�ѹ����
//|          | ADC_CH_5V         +5V��ѹ����
//|          | ADC_CH_COMM       COMM���������ѹ����
//|          | ADC_CH_6_7V       +6.7V��ѹ����
//|          | ADC_CH_LOW1_I     ��ѹ��������1·
//|          | ADC_CH_LOW2_I     ��ѹ��������2·
//|          | ADC_CH_HIGH1_I    ��ѹ��������1·
//|          | ADC_CH_HIGH2_I    ��ѹ��������2·
//|          | ADC_CH_11_3V      +11.3V��ѹ����
//|          | ADC_CH_11_3VOUT   +11.3V���������ѹ����
//|          | ADC_CH_LINA       ����A��ѹ����
//|          | ADC_CH_LINB       ����B��ѹ����
//|          | retry ����������ȡֵ��Χ1-10
//|          | ��������������5ʱ������ֵΪƽ��ֵ������5��ʱ��ȥ2�����ֵ2����Сֵ������ƽ��ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | AD����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
	
	/****����Ҫת����ͨ��****/
	ADC_RegularChannelConfig(ADCx, ADC_Channel_x&0x3F, 1, ADC_SampleTime_3Cycles);
	/****�������ת��****/
	ADC_SoftwareStartConv(ADCx);   
	
	/****�ȴ�������****/
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC)!=SET);
	
	/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
	ADC_GetConversionValue(ADCx);
	for(i=0; i<retry; i++)
	{			
		/****�������ת��****/
		ADC_SoftwareStartConv(ADCx);   
		
		/****�ȴ�������****/
		while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC)!=SET);
		
		/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
		ad_temp[i] = ADC_GetConversionValue(ADCx);		
		ad_value += ad_temp[i];
		if(i != (retry-1))//���һ�β�������ʱ
		{
			System168MDelay10us(2);
		}
	}

	if(retry > 5)
	{
		ad_value = 0;
		U16DataFallRank(ad_temp, rank_temp, retry);//�Ӵ�С����
		for(i=2; i < retry-2; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
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
	
	SET_LV_5_0V();//�л���5V
	System168MDelay1ms(1);//AD�������ݷŵ�
	
	/****����Ҫת����ͨ��****/
	ADC_RegularChannelConfig(ADC3, ADC_CH_LOW1_I&0x3F, 1, ADC_SampleTime_3Cycles);
	/****�������ת��****/
	ADC_SoftwareStartConv(ADC3);   
	
	/****�ȴ�������****/
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
	
	/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
	ADC_GetConversionValue(ADC3);
	for(i=0; i<DET_WAVE_POINT_COUNT; i++)
	{			
		/****�������ת��****/
		ADC_SoftwareStartConv(ADC3);   
		
		/****�ȴ�������****/
		while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
		
		/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
		DetWave[i] = ADC_GetConversionValue(ADC3);
		if(i != (DET_WAVE_POINT_COUNT-1))//���һ�β�������ʱ
		{
			System168MDelay10us(2);
		}
	}
	SET_LV_6_7V();	//�л���6.7
	AdcUnlock();//�ͷ���Դ	
}
/**
  *************************************************************************************
  * @brief  ADC����ֵת���ɵ�ѹ���ߵ���
  * @param[in]  Adc��ADC����ֵ  
  * @param[in]  Adjust��У׼����
  * @param[in]  Default��Ĭ�ϲ���  
  * @retval ��ѹ���ߵ���ֵ 
  * @author ZH
  * @date   2018��7��9��
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
//| �������� | GetBatteryVoltage 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ��ص�ѹֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� |��ص�ѹֵ����λmV
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ����У׼
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
	
	AdcUnlock();//�ͷ���Դ
	
	return ((u16)bat);
}
/**
  *************************************************************************************
  * @brief  LINA-LINB֮���AD������ֵ
  * @param  ��
  * @retval AD������ֵ
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
s16 GetLinABAdcValue(void)
{
	u16 lina,linb;
	s16 value = 0;

	AdcLock();//������Դ
	lina = GetADCValue(ADC_CH_LINA, 1);
	linb = GetADCValue(ADC_CH_LINB, 1);	
	value = lina - linb;
	AdcUnlock();//�ͷ���Դ
	return value;
}
/**
  *************************************************************************************
  * @brief  ����ADC����ֵ���������ߵ�ѹֵ(������ֵ)
  * @param  ��
  * @retval ��ѹֵ
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
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
//| �������� | GetBusVoltage 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡLINA��LINB���ѹ��ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��ص�ѹֵ����λmV
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
		U16DataFallRank(ad_temp, rank_temp, retry);//�Ӵ�С����
		for(i=2; i < retry-2; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
		{
			value += rank_temp[i];
		}
		value /= (retry-4);		
	}
	else
	{
		for(i=0; i < retry; i++)//��ƽ��ֵ
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
  * @brief  �ж������Ƿ��Ѿ���·
  * @param[in]  ��
  * @retval 1 �Ѿ���·�� 0 û�ж�· 
  * @author ZH
  * @date   2018��7��6��
  * @note   ��
  *************************************************************************************  
  */
u8 BusIsShort(void)
{
	u16 lina,linb;

	AdcLock();//������Դ
	lina = GetADCValue(ADC_CH_LINA, 10);
	linb = GetADCValue(ADC_CH_LINB, 10);	
	AdcUnlock();//�ͷ���Դ
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
  * @brief  �ж����ߵ����Ƿ����̷�Χ��
  * @param[in]  BusCurrent ��ǰ���ߵ���
  * @retval 0 ���ڷ�Χ�ڣ� 1 �ڷ�Χ�� 
  * @author ZH
  * @date   2018��7��6��
  * @note   ��
  *************************************************************************************  
  */
u8 BusCurrentInRange(u16 BusCurrent)
{
	u16 MaxCurrent;
	
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//��ǰ���ߵ�ѹ�Ѿ��ر�
		return 1;
	}
	if(GET_HV_SW_CTRL_STATE() == 0)
	{//��ѹ��û���
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
  * @brief  ��ȡ��ǰ���ߵ����������ֵ
  * @param[in]  ��
  * @retval ��ǰ���ߵ����������ֵ
  * @author ZH
  * @date   2018��7��6��
  * @note   ��
  *************************************************************************************  
  */
u16 BusCurrentRangeMax(void)
{
	u16 MaxCurrent;
	
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//��ǰ���ߵ�ѹ�Ѿ��ر�
		MaxCurrent = Adc2VoltageOrCurrent(0xFF0, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);
	}
	else
	{
		if(GET_HV_SW_CTRL_STATE() == 0)
		{//��ѹ��û���
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
//| �������� | GetChannelxVoltage 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡĳ��ͨ���ĵ�ѹֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ADC_CH_BAT        ��ص�ѹ����
//|          | ADC_CH_5V         +5V��ѹ����
//|          | ADC_CH_COMM       COMM���������ѹ����
//|          | ADC_CH_6_7V       +6.7V��ѹ����
//|          | ADC_CH_11_3V      +11.3V��ѹ����
//|          | ADC_CH_11_3VOUT   +11.3V���������ѹ����
//|----------|--------------------------------------------------------------------------------------        
//| ���ز��� |��ص�ѹֵ����λmV
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | δ����У׼
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry)
{
	double bat;
	
	AdcLock();//������Դ
	
	bat = (double)GetADCValue(ADC_Channel_x, retry);
	bat *= DEFAULT_ADC_PARAM ;//��ѹ��80.6K/20K
	
	AdcUnlock();//�ͷ���Դ
	
	return ((u16)bat);
}

//==================================================================================================
//| �������� | GetBusLowCurrent 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡLIN��ѹ�µ���ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ����ֵ����λuA
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetBusLowCurrent(u8 rd_count, u16* AdcValue)
{
	u16 value;

	AdcLock();//������Դ
	
	SET_MEASURE_LOW();
	
	value = GetADCValue(ADC_CH_LOW1_I,rd_count);//�жϵ�ǰ����С���̻��Ǵ�����
	if(NULL != AdcValue)
	{
		*AdcValue = value;	//����ADC����ֵ����Ҫ���ڵ���У׼
	}

	if(value < 3351)//С�����������Ƕ�Ӧ3V������90%ʱ(��Ӧ2.7V)�л����̣�3*90%/3.3*4096 = 3351
	{
		value = Adc2VoltageOrCurrent(value, &adc_adjust.low1current, DEFAULT_I_LOW1_PARAM);		
	}
	else
	{
		value = GetADCValue(ADC_CH_LOW2_I,rd_count);
		if(NULL != AdcValue)
		{
			*AdcValue = value;	//����ADC����ֵ����Ҫ���ڵ���У׼
		}
		value = Adc2VoltageOrCurrent(value, &adc_adjust.low2current, DEFAULT_I_LOW2_PARAM);		
	}
	
	AdcUnlock();//�ͷ���Դ
	
	return value;
}


//==================================================================================================
//| �������� | GetLinHighCurrent 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡLIN��ѹ�µ���ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ����ֵ����λuA
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetBusHighCurrent(u8 rd_count, u16* AdcValue)
{
	u16 value;

	AdcLock();//������Դ
	
	SET_MEASURE_HIGH();
	
	value = GetADCValue(ADC_CH_LOW1_I,rd_count);//�жϵ�ǰ����С���̻��Ǵ�����
	if(NULL != AdcValue)
	{
		*AdcValue = value;	//����ADC����ֵ����Ҫ���ڵ���У׼
	}	
	if(value  <  3351)//С�����������Ƕ�Ӧ3V������90%ʱ(��Ӧ2.7V)�л����̣�3*90%/3.3*4096 = 3351
	{
		value = Adc2VoltageOrCurrent(value, &adc_adjust.high1current, DEFAULT_I_HIGH1_PARAM);	
	}
	else
	{
		value = GetADCValue(ADC_CH_LOW2_I,rd_count);
		if(NULL != AdcValue)
		{
			*AdcValue = value;	//����ADC����ֵ����Ҫ���ڵ���У׼
		}
		value = Adc2VoltageOrCurrent(value, &adc_adjust.high2current, DEFAULT_I_HIGH2_PARAM);	
	}
		
	AdcUnlock();//�ͷ���Դ
	
	return value;
}

u16 GetBusCurrent(u8 rd_count, u16* AdcValue)
{
	if (0 == GET_LINAB_SHORT_SW_CTRL_STATE())
	{//��ǰ���ߵ�ѹ�Ѿ��ر�
		return 0;
	}
//	while(GET_DET_RECEIVE_RES_STATE() != DET_RECEIVE_RES_ON)
//	{
//		OSTimeDly(1);
//	}
	if(GET_HV_SW_CTRL_STATE() == 0)
	{//��ѹ��û���
		return GetBusLowCurrent(rd_count, AdcValue);
	}
	else
	{
		return GetBusHighCurrent(rd_count, AdcValue);
	}
}
//==================================================================================================
//| �������� | GetBusLeakCurrent 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ����©����
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ����©����
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-05 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
	AdcLock();//������Դ
	SET_MEASURE_LOW();
//	CLOSE_DET_RECEIVE_RES();//��·���յ���
//	OSTimeDlyHMSM(0,0,2,0);
	OPEN_DET_RECEIVE_RES();
	
	SET_LV_5_0V();//�л���5V
	System168MDelay1ms(2);//AD�������ݷŵ�
	for(i=0; i<10; i++)
	{
		System168MDelay100us(5);//��ʱ500us
		//ad_temp[i] = GetBusLowCurrent(1, NULL);����ʹ���������Ϊ�ڸú������޷����뵽ADC��Դ
		value = GetADCValue(ADC_CH_LOW1_I,1);//�жϵ�ǰ����С���̻��Ǵ�����
		if(value < 3537)//С�����������Ƕ�Ӧ3V������95%ʱ�л����̣�3*95%/3.3*4096 = 3537
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
	SET_LV_6_7V();	//�л���6.7
	
	
	U16DataFallRank(ad_temp, rank_temp, 10);//�Ӵ�С����
	for(i=2; i < 8; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
	{
		ad_value += rank_temp[i];
	}
	ad_value /= 6;
	
	AdcUnlock();//�ͷ���Դ
	if(NULL != AdcValue)
	{
		*AdcValue = ad_value;
	}
	return ad_value;
}

//==================================================================================================
//| �������� | CheckHVProtectPoint 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����ѹ�������ѹ�Ƿ�����
//|----------|--------------------------------------------------------------------------------------
//| ������� | time_out ����ѹ��ʱʱ�䣬��λms
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 0    ��ѹ���� 
//|          | 1    ��ѹ�쳣
//|          | 0xFF δ������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-09-05 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus CheckHVProtectPoint(void)
{
	u16 temp;
	u32 r_t;

	WriteSystemLog("��ʼ����ѹоƬ�Ƿ�����",strlen("��ʼ����ѹоƬ�Ƿ�����"), NEW_STR_INFO);
	WriteSystemLog("Ŀ���ѹֵ:",sizeof("Ŀ���ѹֵ:"), NEW_STR_INFO);	
	WriteSystemLog(&Op_Data.LinNormalHighVoltageGoal,sizeof(Op_Data.LinNormalHighVoltageGoal), U16_DATA);
	
	//���������ѹ�Ƿ�����
	r_t = OSTimeGet();//���浱ǰʱ��	
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
				WriteSystemLog("��ѹоƬ����!",strlen("��ѹоƬ����!"), NEW_STR_INFO);
				return SUCCESS;//��ѹ����
			}
		}

		OSTimeDlyHMSM(0,0,0,100);		
	}

	WriteSystemLog("��ѹ��·�쳣,��ѹֵ:",strlen("��ѹ��·�쳣,��ѹֵ:"), NEW_STR_INFO);
	WriteSystemLog(&temp,sizeof(temp), U16_DATA);			
	return ERROR ;//��ѹ�쳣
}


