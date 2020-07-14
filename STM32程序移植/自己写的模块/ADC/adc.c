#include "includes.h"

void AdcConfig(void)                                                                          
{                                                                                             
	GPIO_InitTypeDef GPIO_InitStruct;                                                           
	ADC_InitTypeDef ADC_InitStruct;                                                             
	                                                                                            
	//GPIO时钟配置                                                                   
	RCC_APB2PeriphClockCmd(AD_I1_PORT_RCC| AD_12V_PORT_RCC| ADC1_RCC, ENABLE);                  
                                                                                              
	//GPIO配置为ADC模式                                                                    
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;                                              
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;                                                  
			                                                                                        
	GPIO_InitStruct.GPIO_Pin = AD_12V_PIN|AD_LINEA_PIN|AD_LINEB_PIN|AD_COMOUT_PIN;              
	GPIO_Init(AD_12V_PORT, &GPIO_InitStruct);                                                   
                                                                                              
 	GPIO_InitStruct.GPIO_Pin = AD_I1_PIN|AD_I2_PIN;                                             
	GPIO_Init(AD_I1_PORT, &GPIO_InitStruct);                                                    
     	                                                                                        
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//采样频率不得超过14M
	                                                                                            
	//ADC1参数配置                                                                 
  ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//                          
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//               
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;	  //                         
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	 //
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;                                         
	ADC_InitStruct.ADC_NbrOfChannel = 1;	  //                                       
	ADC_Init(ADC1, &ADC_InitStruct);                                                            
	                                                                                            
	                                                                                            
	/* Enable ADC1 */                                                                           
	ADC_Cmd(ADC1, ENABLE);                                                         
	                                                                                            
	/* Enable ADC1 reset calibaration register */                                               
	ADC_ResetCalibration(ADC1);                                     
                                                                                              
	/* Check the end of ADC1 reset calibration register */                                      
	while(ADC_GetResetCalibrationStatus(ADC1));
	                                                                                            
	/* Start ADC1 calibaration */                                                               
	ADC_StartCalibration(ADC1);                                                                 
                                                                                              
	/* Check the end of ADC1 calibration */                                                     
	while(ADC_GetCalibrationStatus(ADC1));	                                                    
	                                                                                            
}   

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
		if(flag_bit)//提前结束排序                   
		{                                                   
			break ;                                           
		}		                                                
	}                                                     
} 

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
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //检查转换是否开始，1表示开始
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);                                                 
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //检查转换是否开始，1表示开始
		                                                                                        
		ad_temp[i] = ADC_GetConversionValue(ADC1);  //读取DR时自动清标志               
		ad_value += ad_temp[i];	                                                                
		if(i != (retry-1))//最后一次不用延时                                     
		{                                                                                       
			System72MDelay10us(1);                                                                
		}		                                                                                    
	}                                                                                         
	                                                                                          
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);                                                  
	ADC_ClearFlag(ADC1,ADC_FLAG_STRT);//软件清除标志                                     
	                                                                                          
	if(retry > 5)                                                                             
	{                                                                                         
		ad_value = 0;                                                                           
		U16DataFallRank(ad_temp, rank_temp, retry);//从大到小排序                       
		for(i=2; i < retry-2; i++)//去掉两个最大值两个最小值后求平均            
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

void GetWaveAdcValue(void)
{
		u16 i;
		SW5V_H();  //使能5V电压，做好准备
		VSHIFT_H();//切换到5V
		
		System72MDelay1ms(1);//AD采样电容放电
		
		ADC_RegularChannelConfig(ADC1, (uint8_t)adc1_channel, 1, ADC_SampleTime_239Cycles5 );     
	 	for(i=0; i<retry; i++)                                                                    
		{                                                                                         
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);                                                 
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //检查转换是否开始，1表示开始
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);                                                 
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //检查转换是否开始，1表示开始
			                                                                                        
			ad_temp[i] = ADC_GetConversionValue(ADC1);  //读取DR时自动清标志               
			ad_value += ad_temp[i];	                                                                
			if(i != (retry-1))//最后一次不用延时                                     
			{                                                                                       
				System72MDelay10us(1);                                                                
			}		                                                                                    
		}                  
		
		VSHIFT_L();
		SW5V_L();			
}
//ADC测量值转换成电压或者电流（校准或校准前）
u16 Adc2VoltageOrCurrent(u16 Adc, ADJUST_PARAMETER* Adjust, float Default)
{
	int value;
	
	if((u16)(Adjust->k*1000) == 0)
	{
		value = (int)(Adc*Default);
	}
	else
	{
		if(Adjust->b&0x8000)//人工去判断截距是正还是负
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

//获得电池电压值（未写，对海哥的程序表示疑惑,按理说有之前Adc2VoltageOrCurrent这个函数就够了）

//测量linea和lineb之间的差值
s16 GetLinABAdcValue(void)
{
	u16 lina,linb;
	s16 value = 0;


	lina = GetAdcMeasureValue(ADC_LINEA_CHANNEL, 1);
	linb = GetAdcMeasureValue(ADC_LINEB_CHANNEL, 1);	
	value = lina - linb;
	return value;  
//判断总线电流是否短路	
u8 BusIsShort(void)
{
	u16 lina,linb;
	
	lina = GetAdcMeasureValue(ADC_LINEA_CHANNEL, 10);
	linb = GetAdcMeasureValue(ADC_LINEB_CHANNEL, 10); 
	lina = lina*DEFAULT_ADC_PARAM*1000;
	linb = linb*DEFAULT_ADC_PARAM*1000;
	if(lina > 1000 && linb > 1000)
	{
		return 1;
	}
		return 0;
}
//获得漏电流
u16 GetBusLeakCurrent(u16* AdcValue)
{
	u8 i;
	u16 ad_temp[10],rank_temp[10];
	s32 value,ad_value = 0;
	
	SW5V_H();  //使能5V电压，做好准备
	VSHIFT_H();//切换到5V
	
	System72MDelay1ms(2);//AD采样电容放电
	
	for(i=0;i<10;i++)
	{
	System72MDelay100us(5);
	value = GetAdcMeasureValue(ADC_I2_CHANNEL，1)；//判断当前处于哪个量程
	if(value < 3561)//小量程满量程是1mA,对应电压是3.029V,大于95%的时候切换量程，3.029*95%/3.3*4096=3561
		{
			value = Adc2VoltageOrCurrent(value, &adc_adjust.current2, DEFAULT_I2_PARAM);	
		}
	else
		{
		value = GetAdcMeasureValue(ADC_I1_CHANNEL，1);	
		value = Adc2VoltageOrCurrent(value, &adc_adjust.current1, DEFAULT_I1_PARAM);	
		}
	if(value < 0)
		{
			value = 0;	
		}	
		ad_temp[i] = (u16)value;
	}		
	SW5V_L();  //关闭5V输出
	VSHIFT_L();//切换到6.7V
	
	U16DataFallRank(ad_temp, rank_temp, 10);//从大到小排序
	for(i=2; i < 8; i++)//去掉两个最大值两个最小值后求取平均值
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

                                                                                   