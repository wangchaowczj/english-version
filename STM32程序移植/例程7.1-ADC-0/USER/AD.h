#ifndef _AD_H_
#define _AD_H_

#define ADC1_RCC               		RCC_APB2Periph_ADC1

//ADC123_IN1 测电流量程一
#define AD_I1_PORT             		GPIOA
#define AD_I1_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I1_PIN              		GPIO_Pin_1
#define ADC_I1_CHANNEL		   			1

//ADC123_IN2 测电流量程二
#define AD_I2_PORT             		GPIOA
#define AD_I2_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I2_PIN              		GPIO_Pin_2
#define ADC_I2_CHANNEL		   			2

//ADC123_IN10 电源电压
#define AD_12V_PORT            		GPIOC
#define AD_12V_PORT_RCC        		RCC_APB2Periph_GPIOC
#define AD_12V_PIN             		GPIO_Pin_0
#define ADC_12V_CHANNEL		   			10

//ADC123_IN11 LINEA
#define AD_LINEA_PORT            	GPIOC
#define AD_LINEA_PORT_RCC        	RCC_APB2Periph_GPIOC
#define AD_LINEA_PIN             	GPIO_Pin_1
#define ADC_LINEA_CHANNEL		  	 	11

//ADC123_IN12 LINEB
#define AD_LINEB_PORT               GPIOC
#define AD_LINEB_PORT_RCC           RCC_APB2Periph_GPIOC
#define AD_LINEB_PIN                GPIO_Pin_2
#define ADC_LINEB_CHANNEL		   		12

//ADC123_IN13 总线输出电压
#define AD_COMOUT_PORT              GPIOC
#define AD_COMOUT_PORT_RCC          RCC_APB2Periph_GPIOC
#define AD_COMOUT_PIN               GPIO_Pin_3
#define ADC_COMOUT_CHANNEL		   	    13



void AdcConfig(void);
u16 GetAdcMeasureValue(uint8_t adc1_channel, u8 retry);
u16 MeasureVoltage (uint8_t adc1_channel,u8 retry);
extern u16 GetBusLeakCurrent(u16* AdcValue);
void DeterminePower(void);
u16 GetPowerFlag(void);




















#endif


