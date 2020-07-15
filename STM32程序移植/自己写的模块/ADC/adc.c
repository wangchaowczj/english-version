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
}

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

//判断电流脉冲函数
//函数功能：判断第一发雷管接入网络后的电流脉冲
//触发阈值：100uA（阈值没有准确测定，只是用万用表测到的粗略值）
//返回值：返回标志位
void DeterminePulse(void)
{
	u16 temp;
	int value = 0;
	temp = GetAdcMeasureValue(ADC_I2_CHANNEL, 1);
	if(temp > 376) //100uA对应AD转换到的数字量是376
		{
			value = 1;
		} 
	else
		{
			value = 0;	
		}
	return value;		
}

//判断上电函数
//函数功能：判断电源电压是否接入
//判断阈值：电池的最低电压10.5V
//如果电源供电则发送一个信号让蜂鸣器响一下，可以通过写标志位的方式来写函数。比如上电前标志位为0，上电后标志位变为1，然后触发蜂鸣器，蜂鸣器响应以后就变回0.就是一个上升沿判断。
//之后该标志位一直为0.就不会再触发了。两个条件，条件1是上升沿，条件2是标志位
//返回值：返回标志位

void DeterminePower(void)
{
	u16 temp;
	int value = 0;
	temp = GetAdcMeasureValue(ADC_12V_CHANNEL,1);
	if(temp > 2606)	//10.5V对应AD转换后的数字量是2606
		{
			value = 1;
		}
	else
		{
			value = 0;	
		}
	return value;
}
     
//用于判断是否触发蜂鸣器
//函数功能：在遇到第一次上电和接入第一发雷管后蜂鸣器响一下
//判断条件：一个是电流或者电压，另一个就是人为设置的标志位，有两个，一个是电源标志位，单片机上电前是0，上电后是1，响应之后变成0，另一个是雷管电流标志位（小量程）
//标志位不能同时写在同一个函数里面，应该写在两个函数里，要不然标志位可能会变得不起作用

void SoundBee(void)
{
	int i,j,value_power,value_pulse,power_flag,pulse_flag;
	value_power = DeterminePower();
	value_pulse = DeterminePulse();
	if(value_power == 1)
		{
			if(i > 10)
				{
					i = 10;	
					power_flag = 1;
				}
			else
				{
					i+=1;	
					power_flag = 0;
				}	
		}
	else
		{
			i = 0;	
		}	
	if(value_pluse == 1)
		{
			if(j > 10)
				{
					j = 10;	
					pulse_flag = 1;
				}
			else
				{
					i+=1;	
					pulse_flag = 0;
				}	
		}
	else
		{
			i = 0;	
		}	
	if((power_flag & value_power)|(pulse_flag & value_pluse)
		{
			OUT_ON();
			System72MDelay1ms(200);
			OUT_OFF();				
		}	
}                                                                              