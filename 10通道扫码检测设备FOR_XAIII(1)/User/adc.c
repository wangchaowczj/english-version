/**
  *************************************************************************************
  * @file    ADC.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ADCģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup ADC
  *@brief ADC����ģ��
  *@{
  */
/**@defgroup ADC_Variable
  *@brief ��������
  *@{
  */ 
ADC_ADJUST AdcAdjust;     /**<ADCУ׼����*/
static  OS_EVENT   *AdcMutex = 0;

/**
  *@} ADC_Variable
  */

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
	if(AdcMutex != 0)
	{
		OSMutexPost(AdcMutex);
	}	
}
/**
  *************************************************************************************
  * @brief  ADC���ú���
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void AdcConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	/****����GPIOʱ��****/
	 RCC_AHB1PeriphClockCmd(ADC_CH10_LINA_PORT_RCC | ADC_CH10_LINB_PORT_RCC | ADC_CH9_LINA_PORT_RCC | 
		ADC_CH9_LINB_PORT_RCC | ADC_CH1_TO_8_LINA_PORT_RCC | ADC_CH1_TO_8_LINB_PORT_RCC | ADC_BAT_PORT_RCC | 
		ADC_5V_PORT_RCC|ADC_LV_PORT_RCC|ADC_SCAN_PORT_RCC|ADC_LINE_I_PORT_RCC|ADC_HV_PORT_RCC, ENABLE);

	/****����ͨ��ѡ��GPIO****/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH_CTRL_A_PIN;
	GPIO_Init(ADC_CH_CTRL_A_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH_CTRL_B_PIN;
	GPIO_Init(ADC_CH_CTRL_B_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH_CTRL_C_PIN;
	GPIO_Init(ADC_CH_CTRL_C_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ADC_CH_CTRL_OE_PIN;
	GPIO_Init(ADC_CH_CTRL_OE_PORT, &GPIO_InitStructure);

	/****ADC����GPIO****/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH10_LINA_PIN;
	GPIO_Init(ADC_CH10_LINA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH10_LINB_PIN;
	GPIO_Init(ADC_CH10_LINB_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH9_LINA_PIN;
	GPIO_Init(ADC_CH9_LINA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH9_LINB_PIN;
	GPIO_Init(ADC_CH9_LINB_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH1_TO_8_LINA_PIN;
	GPIO_Init(ADC_CH1_TO_8_LINA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CH1_TO_8_LINB_PIN;
	GPIO_Init(ADC_CH1_TO_8_LINB_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = ADC_BAT_PIN;
	GPIO_Init(ADC_BAT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_5V_PIN;
	GPIO_Init(ADC_5V_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_LV_PIN;
	GPIO_Init(ADC_LV_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_SCAN_PIN;
	GPIO_Init(ADC_SCAN_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = ADC_LINE_I_PIN;
	GPIO_Init(ADC_LINE_I_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_HV_PIN;
	GPIO_Init(ADC_HV_PORT, &GPIO_InitStructure);
	
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
	ADC_Init(ADC3, &ADC_InitStructure);	

	/****����ADC3****/
	ADC_Cmd(ADC3, ENABLE);	
}
void SetAdcCh1To8Connect(u8 Channel)
{
	switch(Channel)
	{
		case 0:
			ADC_SEL_CH1();
			break;
		case 1:
			ADC_SEL_CH2();
			break;
		case 2:
			ADC_SEL_CH3();
			break;
		case 3:
			ADC_SEL_CH4();
			break;
		case 4:
			ADC_SEL_CH5();
			break;
		case 5:
			ADC_SEL_CH6();
			break;
		case 6:
			ADC_SEL_CH7();
			break;
		case 7:
			ADC_SEL_CH8();
			break;			
	}	
}
/**
  *************************************************************************************
  * @brief  ������(16λ)�Ӵ�С��˳������
  * @param  ptr_s:ָ�룬ָ������ǰ������
  * @param  ptr_d:ָ�룬ָ������������
  * @param  lenght:Ҫ��������ݸ���
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   �� 
  *************************************************************************************
  */
//void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght)
//{
//	u16 i,j,temp;
//	u8 flag_bit;
//	
//	if(lenght == 0)
//	{
//		return ;
//	}
//	memcpy((u8*)ptr_d, (u8*)ptr_s, lenght*2);
//	for(i=0; i < lenght-1; i++)
//	{
//		flag_bit = 1;
//		for(j=0; j < lenght-i-1; j++)
//		{
//			 if(*(ptr_d+j)<*(ptr_d+j+1))
//			 {
//				 flag_bit = 0 ;
//				 temp = *(ptr_d+j) ;
//				 *(ptr_d+j) = *(ptr_d+j+1);
//				 *(ptr_d+j+1)= temp ;
//			 }
//		}
//		if(flag_bit)//��ǰ��������
//		{
//			break ;
//		}		
//	}
//}

/**
  *************************************************************************************
  * @brief  ��ȡADC����ֵ
  * @param  adc1_channel:Ҫ������ͨ���š�
  *         ȡֵ��Χ����(��ԭ��ͼ��Ӧ)��
  *         @arg @b ADC_CH_I
  *         @arg @b ADC_CH_BAT
  *         @arg @b ADC_CH_LINA
  *         @arg @b ADC_CH_LINB
  *         @arg @b ADC_CH_5V
  *         @arg @b ADC_CH_11_3VOUT
  *         @arg @b ADC_CH_11_3V
  *         @arg @b ADC_CH_6_7V
  * @param  retry:���Դ���,ȡֵ��Χ1-10
  * @param  lenght:Ҫ��������ݸ���
  * @retval ��Ӧͨ����ADC����ֵ
  * @author ZH
  * @date   2014��9��10��
  * @note   1.retry��ֵΪ0ʱ������0��retry��ֵ����10ʱ��Ч��10
  *         2.retry����5ʱ����ֵΪȥ��2�����ֵ��2����Сֵ����ƽ��ֵ��
  *           retryС�ڻ����5ʱ����ֵΪƽ��ֵ
  *************************************************************************************  
  */
u16 GetAdcMeasureValue(ADC3_CHANNEL adc_channel, u8 retry)
{
	u16 ad_value = 0;
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

	/****����Ҫת����ͨ��****/
	ADC_RegularChannelConfig(ADC3, adc_channel&0x3F, 1, ADC_SampleTime_3Cycles);
	/****�������ת��****/
	ADC_SoftwareStartConv(ADC3);   
	
	/****�ȴ�������****/
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
	
	/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
	ADC_GetConversionValue(ADC3);
	for(i=0; i<retry; i++)
	{			
		/****�������ת��****/
		ADC_SoftwareStartConv(ADC3);   
		
		/****�ȴ�������****/
		while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)!=SET);
		
		/****��ȡ����ֵ��ͬʱ�Զ������־λ****/
		ad_temp[i] = ADC_GetConversionValue(ADC3);		
		ad_value += ad_temp[i];
		if(i != (retry-1))//���һ�β�������ʱ
		{
			SystemDelay10us(2);
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
  * @brief  ��ȡ��ѹ�µ���ֵ
  * @param  num ���Դ�����1-10
  * @retval ����ֵ����λ0.01uA
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
u16 GetCurrentValue(u8 num, u16* AdcMeasureValue)
{
	double  value = 0;
	
	AdcLock();
	value = (double)GetAdcMeasureValue(ADC_CH_LINE_I, num);
	AdcUnlock();
	if(AdcMeasureValue !=NULL)
	{
		*AdcMeasureValue = (u16)value;//����AD����ֵ	
	}

	if((u16)(AdcAdjust.Current.k*1000) == 0)
	{//û��У׼������Ĭ�ϲ���
		value = value*AD_CURRENT_DEFAULT_PARAM;
	}
	else
	{
		if(AdcAdjust.Current.b&0x8000)
		{//����bΪ��
			value = value*AdcAdjust.Current.k - (AdcAdjust.Current.b&0x7FFF);
		}
		else
		{
			value = value*AdcAdjust.Current.k + (AdcAdjust.Current.b&0x7FFF);
		}	
	}


	if(value < 0)
	{
		return 0;
	}
	return ((u16)value);	
}

/**
  *************************************************************************************
  * @brief  ��ȡ�˿ڵ�ѹֵ
  * @param  ��
  * @retval ����ֵ����λmV
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
u16 GetLinABVoltage(u8 Channel, u16* AdcMeasureValue)
{
	u16 lina,linb;
	double  value = 0;
	u8 i;
	u16 ad_temp[10], rank_temp[10];
	ADC3_CHANNEL AdcLinA, AdcLinB;
	ADJUST_PARAMETER* Adjust;
	
	if(Channel <= 7)
	{
		SetAdcCh1To8Connect(Channel);
		AdcLinA = ADC_CH_CH1_TO_8_LINA;
		AdcLinB = ADC_CH_CH1_TO_8_LINB;
		Adjust = &AdcAdjust.LinAB1To8;
	}
	else if(Channel == 8)
	{
		AdcLinA = ADC_CH_CH9_LINA;
		AdcLinB = ADC_CH_CH9_LINB;
		Adjust = &AdcAdjust.LinAB9;
	}
	else
	{
		AdcLinA = ADC_CH_CH10_LINA;
		AdcLinB = ADC_CH_CH10_LINB;	
		Adjust = &AdcAdjust.LinAB10;
	}
	for(i = 0; i<10; i++)
	{
		SystemDelay1ms(1);
		AdcLock();
		lina = GetAdcMeasureValue(AdcLinA,1);
		linb = GetAdcMeasureValue(AdcLinB,1);
		AdcUnlock();
		ad_temp[i] = abs(lina - linb);		
	}
	U16DataFallRank(ad_temp, rank_temp, 10);//�Ӵ�С����
	for(i=2; i < 8; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
	{
		value += rank_temp[i];
	}
	value /= 6;	
	if(AdcMeasureValue != NULL)
	{
		*AdcMeasureValue = value;
	}
	if((u16)(Adjust->k*1000) == 0)
	{//û��У׼������Ĭ�ϲ���
		value = value*AD_LINA_DEFAULT_PARAM;
	}
	else
	{
		if(Adjust->b&0x8000)
		{//����bΪ��
			value = value*Adjust->k - (Adjust->b&0x7FFF);
		}
		else
		{
			value = value*Adjust->k + (Adjust->b&0x7FFF);
		}	
	}
	
	if(value < 0)
	{
		return 0;
	}
	return ((u16)value);	
}
/**
  *************************************************************************************
  * @brief  ��ȡ��ص�ѹֵ
  * @param  ��
  * @retval ����ֵ����λmV
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
u16 GetBatVoltage(u16* AdcMeasureValue)
{
	double  value = 0;

	AdcLock();
	value = GetAdcMeasureValue(ADC_CH_BAT, 10);
	AdcUnlock();
	
	if(AdcMeasureValue != NULL)
	{
		*AdcMeasureValue = value;
	}
	if((u16)(AdcAdjust.Bat.k*1000) == 0)
	{//û��У׼������Ĭ�ϲ���
		value = value*AD_BAT_DEFAULT_PARAM;
	}
	else
	{
		if(AdcAdjust.Bat.b&0x8000)
		{//����bΪ��
			value = value*AdcAdjust.Bat.k - (AdcAdjust.Bat.b&0x7FFF);
		}
		else
		{
			value = value*AdcAdjust.Bat.k + (AdcAdjust.Bat.b&0x7FFF);
		}	
	}	
	if(value < 0)
	{
		return 0;
	}
	return ((u16)value);	
}
/**
  *************************************************************************************
  * @brief  ��ȡ��ѹֵ
  * @param  adc1_channel:Ҫ������ͨ����
  *         ȡֵ��Χ����(��ԭ��ͼ��Ӧ):
  *         @arg @b AD_REF_PLUS
  *         @arg @b AD_5V
  *         @arg @b AD_HV
  *         @arg @b AD_LINA
  *         @arg @b AD_LINB
  *         @arg @b AD_LV
  *         @arg @b AD_REF
  *         @arg @b AD_12V
  *         @arg @b AD_24V
  *         @arg @b AD_A12V
  *         @arg @b AD_BRIDGE
  *         @arg @b AD_DUAL
  * @retval ����ֵ����λmV
  * @author ZH
  * @date   2014��9��10��
  * @note   �������ֵ���ڷ�Χ��ʱ����ֵΪ0
  *************************************************************************************  
  */
u16 GetVoltageValue(ADC3_CHANNEL channel)
{
	double  value = 0;
	
	AdcLock();
	value = (double)GetAdcMeasureValue(channel,10);
	AdcUnlock();
	switch(channel)
	{
		case ADC_CH_LV:
			value *=AD_LV_DEFAULT_PARAM;
			break;
		case ADC_CH_SCAN:
			value *=AD_SCAN_DEFAULT_PARAM;
			break;
		case ADC_CH_HV:
			value *=AD_HV_DEFAULT_PARAM;
			break;
		case ADC_CH_BAT:
			value *=AD_BAT_DEFAULT_PARAM;
			break;
		case ADC_CH_5V:
			value *=AD_5V_DEFAULT_PARAM;
			break;			
		default:
			value = 0;
			break;
	}
	return (u16)value;
}

/**
  *************************************************************************************
  * @brief  �豸�Լ�
  * @param  ��
  * @retval ����
  * @author ZH
  * @date   2015��6��6��
  * @note   ��
  *************************************************************************************  
  */
u8 DeviceSelfTest(void)
{
	u8 i,j,k;
	ADC3_CHANNEL AdcLinA, AdcLinB;
	u16 U16Value, U16Buffer[4];
	
	OPEN_PW_5V();
	SET_LIN_OPEN();
	CLOSE_HV_SW();	
	OSTimeDly(100);	
	OPEN_PW_LV();
	OPEN_PW_HV();
	OSTimeDly(100);
	for(j=0;j<10;j++)
	{		
		U16Buffer[0] = 0x5555;
		U16Buffer[1] = 0xAAAA;
		U16Buffer[2] = 0xFFFF;
		U16Buffer[3] = 0x0000;
		
		for(i=0;i<4;i++)
		{		
			LcdSetPixel(i,0,U16Buffer[i]);	
		}
		LcdGetPixel(0, 0);
		for(i=0;i<4;i++)
		{
			U16Value = LcdGetPixel(i, 0);
			if(U16Value != U16Buffer[i])
			{
				break;
			}
		}
		
		if(i == 4)
		{
			break;
		}
		OSTimeDly(20);
	}
	if(j == 10)
	{		
		return 1;
	}
	
	for(k=0; k<Channel.Count; k++)
	{
		for(i=0;i<10;i++)
		{
			ConnectChannel(k);
			OSTimeDly(100);	
			U16Value = GetCurrentValue(10,NULL);
			if(U16Value < 300)
			{
				break;
			}
		}
		
		if(i>=10)
		{
			return (k<<4)+2;//��̬������
		}	
	}

	
	for(i=0;i<10;i++)
	{
		OSTimeDly(100);	
		U16Value = GetVoltageValue(ADC_CH_5V);
		if((U16Value > 4600) && (U16Value < 5400))
		{
			break;
		}
	}
	
	if(i>=10)
	{
		return 3;//5V
	}

	for(i=0;i<10;i++)
	{
		OSTimeDly(100);	
		U16Value = GetVoltageValue(ADC_CH_LV);
		if((U16Value > LINAB_LV_VOLTAGE_MIN) && (U16Value < LINAB_LV_VOLTAGE_MAX))
		{
			break;
		}
	}
	
	if(i>=10)
	{
		return 4;
	}

	for(i=0;i<10;i++)
	{
		OSTimeDly(100);	
		U16Value = GetVoltageValue(ADC_CH_HV);
		if((U16Value > LINAB_HV_VOLTAGE_MIN) && (U16Value < LINAB_HV_VOLTAGE_MAX))
		{
			break;
		}
	}
	
	if(i>=10)
	{
		return 5;
	}
	for(k=0;k<Channel.Count;k++)
	{
		for(i=0;i<10;i++)
		{
			ConnectChannel(k);
			OSTimeDly(100);	
			U16Value = GetLinABVoltage(0, NULL);
			if((U16Value > LINAB_LV_VOLTAGE_MIN) && (U16Value < LINAB_LV_VOLTAGE_MAX))
			{
				break;
			}
		}
		
		if(i>=10)
		{
			return (k<<4)+6;
		}	
	}

	for(k=0;k<Channel.Count;k++)
	{
		if(k <= 7)
		{
			SetAdcCh1To8Connect(k);
			AdcLinA = ADC_CH_CH1_TO_8_LINA;
			AdcLinB = ADC_CH_CH1_TO_8_LINB;
		}
		else if(k == 8)
		{
			AdcLinA = ADC_CH_CH9_LINA;
			AdcLinB = ADC_CH_CH9_LINB;
		}
		else
		{
			AdcLinA = ADC_CH_CH10_LINA;
			AdcLinB = ADC_CH_CH10_LINB;	
		}	
		if(GetVoltageValue(AdcLinA) > GetVoltageValue(AdcLinB))
		{
			SET_LIN_REVERSE();
			OSTimeDly(100);	
			if(GetVoltageValue(AdcLinA) > GetVoltageValue(AdcLinB))
			{
				SET_LIN_REVERSE();	
				return (k<<4)+7;
			}
		}
		else
		{
			SET_LIN_REVERSE();
			OSTimeDly(100);	
			if(GetVoltageValue(AdcLinA) < GetVoltageValue(AdcLinB))
			{
				SET_LIN_REVERSE();	
				return (k<<4)+7;
			}
		}
		SET_LIN_REVERSE();	
	}

	return 0;
}

/**
  *@} ADC_Functions
  */
/**
  *@} ADC
  */
/**
  *@} PowerBoard
  */


