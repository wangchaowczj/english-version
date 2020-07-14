#include "includes.h"

ADC_ADJUST adc_adjust;				//ADCУ׼����
static  OS_EVENT   *AdcMutex;
__IO uint16_t ADCLinABValue;
#define ADC1_DR_ADDRESS          ((uint32_t)0x4001204C)

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
/**
  *************************************************************************************
  * @brief  ADC��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��8��22��
  * @note   ��
  *************************************************************************************  
  */
void ADCConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	/****����GPIOʱ��****/
	 RCC_AHB1PeriphClockCmd(ADC_LINAB_PORT_RCC | ADC_BAT_PORT_RCC, ENABLE);
	
	/****����GPIO****/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	GPIO_InitStructure.GPIO_Pin = ADC_LINAB_PIN;
	GPIO_Init(ADC_LINAB_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_BAT_PIN;
	GPIO_Init(ADC_BAT_PORT, &GPIO_InitStructure);

	
	/****����ADC1ʱ��****/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC2 , ENABLE);

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
	ADC_Init(ADC2, &ADC_InitStructure);	

	/****����ADC2****/
	ADC_Cmd(ADC2, ENABLE);	
}
void DMAConfig(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;


    /* ʹ������ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* DMA2 Stream0 channel0 ����-------------------------------------------------- */
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCLinABValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);

    /* DMA2_Stream0 enable */
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /****************************************************************************   
    PCLK2 = HCLK / 2 
    ����ѡ�����2��Ƶ
    ADCCLK = PCLK2 /8 = HCLK / 8 = 168 / 8 = 21M
    ADC����Ƶ�ʣ� Sampling Time + Conversion Time = 480 + 12 cycles = 492cyc
                Conversion Time = 21MHz / 492cyc = 42.6ksps. 
    *****************************************************************************/

    /* ADC Common ���� ----------------------------------------------------------*/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC1 regular channel 12 configuration ************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* ADC1 regular channel18 (VBAT) configuration ******************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);

    /* ʹ�� ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE); 

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC1 **************************************************************/
    ADC_Cmd(ADC1, ENABLE);

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConv(ADC1);
}

/**
  *************************************************************************************
  * @brief  ���������ͨ����ȡ��ӦADֵ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��8��22��
  * @note   ��
  *************************************************************************************  
  */
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
		case ADC_CH_BAT:
        case ADC_CH_LINAB:
			ADCx = ADC2;
			break;
		default:
			ADCx = ADC2;
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
//	lina = GetADCValue(ADC_CH_LINA, 1);
//	linb = GetADCValue(ADC_CH_LINB, 1);	
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



