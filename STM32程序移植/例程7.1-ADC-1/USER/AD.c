#include "includes.h"


//==================================================================================================
//| �������� | AdcConfig 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ADC���ú���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void AdcConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	//GPIO��ADC1ʱ������
	RCC_APB2PeriphClockCmd(AD_I1_PORT_RCC| AD_12V_PORT_RCC| ADC1_RCC, ENABLE);

	//GPIO����ΪADCģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
			
	GPIO_InitStruct.GPIO_Pin = AD_12V_PIN|AD_LINEA_PIN|AD_LINEB_PIN|AD_COMOUT_PIN;
	GPIO_Init(AD_12V_PORT, &GPIO_InitStruct);
    
 	GPIO_InitStruct.GPIO_Pin = AD_I1_PIN|AD_I2_PIN;
	GPIO_Init(AD_I1_PORT, &GPIO_InitStruct); 
     	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//�ֲ�Ҫ��ADC��Ƶ�ʲ��ܳ�ʱ14M
	
	//ADC1��������
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//ģ��ת��Ϊ��ͨ��ģʽ
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;	  //������ת��
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	 //�����ⲿ��������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;	  //ͨ��Ϊ1
	ADC_Init(ADC1, &ADC_InitStruct);
	
	  
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);//У׼��ʽΪ��λУ׼

	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λУ׼����
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	 
	
}

//==================================================================================================
//| �������� | U16DataFallRank 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ������(16λ)�Ӵ�С��˳������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr_s ָ�룬ָ������ǰ������
//|          | ptr_d ָ�룬ָ������������
//|          | lenghtҪ��������ݸ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght)
{
	u16 i,j,temp;
	u8 flag_bit;
	
	if(lenght == 0)
	{
		return ;
	}
	memcpy((u8*)ptr_d, (u8*)ptr_s, lenght*2);
	for(i=0; i < lenght-1; i++)
	{
		flag_bit = 1;
		for(j=0; j < lenght-i-1; j++)
		{
			 if(*(ptr_d+j)<*(ptr_d+j+1))
			 {
				 flag_bit = 0 ;
				 temp = *(ptr_d+j) ;
				 *(ptr_d+j) = *(ptr_d+j+1);
				 *(ptr_d+j+1)= temp ;
			 }
		}
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
}
//==================================================================================================
//| �������� | GetAdcMeasureValue 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡADC����ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | adc1_channel  Ҫ������ͨ����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetAdcMeasureValue(uint8_t adc1_channel, u8 retry)
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
	
	ADC_RegularChannelConfig(ADC1, (uint8_t)adc1_channel, 1, ADC_SampleTime_239Cycles5 );
 	for(i=0; i<retry; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //���ת���Ƿ�ʼ��1��ʾ��ʼ	
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //���ת���Ƿ�ʼ��1��ʾ��ʼ
		
		ad_temp[i] = ADC_GetConversionValue(ADC1);  //��ȡDRʱ�Զ����־
		ad_value += ad_temp[i];	
		if(i != (retry-1))//���һ�β�������ʱ
		{
			System72MDelay10us(1);
		}		
	}
	
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	ADC_ClearFlag(ADC1,ADC_FLAG_STRT);//��������־
	
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
	
	
	return ad_value;
}

//==================================================================================================
//| �������� | GetAdcMeasureValue 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡADC����ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | adc1_channel  Ҫ������ͨ����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 MeasureVoltage (uint8_t adc1_channel,u8 retry)
{
	double temp;
	temp=(double)GetAdcMeasureValue(adc1_channel, retry);
	temp=temp*1.611328125;
	return ((u16)temp);
}

u16 GetBusLeakCurrent(u16* AdcValue)
{
	u8 i;
	u16 ad_temp[10],rank_temp[10];
	s32 value,ad_value = 0;
	
	SW5V_H();  //
	VSHIFT_H();//
	
	System72MDelay1ms(2);//AD??????
	
	for(i=0;i<10;i++)
	{
	System72MDelay100us(5);
	value = GetAdcMeasureValue(ADC_I2_CHANNEL,1);//??????????
	if(value < 3561)//???????1mA,?????3.029V,??95%???????,3.029*95%/3.3*4096=3561
		{
			value *= 0.2660;	
		}
	else
		{
		value = GetAdcMeasureValue(ADC_I1_CHANNEL,1);	
		value *= 2.6596;	
		}
	if(value < 0)
		{
			value = 0;	
		}	
		ad_temp[i] = (u16)value;
	}		
	SW5V_L();  //??5V??
	VSHIFT_L();//???6.7V
	
	U16DataFallRank(ad_temp, rank_temp, 10);//??????
	for(i=2; i < 8; i++)//??????????????????
	{
		ad_value += rank_temp[i];
	}
	ad_value /= 6;
	
	if(NULL != AdcValue)
	{
		*AdcValue = ad_value;
	}
	return ad_value;
}

u16 GetPowerFlag(void)
{
	u16 temp;
	u16 value;
	temp = GetAdcMeasureValue(ADC_12V_CHANNEL,1);
	if(temp > 2606)	//10.5V??AD????????2606
		{
			value = 1;
		}
	else
		{
			value = 0;	
		}
        
	return value;
}

















