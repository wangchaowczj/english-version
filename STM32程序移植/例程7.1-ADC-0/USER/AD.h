#ifndef _AD_H_
#define _AD_H_

#define ADC1_RCC               RCC_APB2Periph_ADC1

////ADC123_IN10
//#define AD_BAT_PORT            GPIOC
//#define AD_BAT_PORT_RCC        RCC_APB2Periph_GPIOC
//#define AD_BAT_PIN             GPIO_Pin_0
//#define ADC_BAT_CHANNEL		   10

////ADC123_IN11
//#define AD_5V_PORT             GPIOC
//#define AD_5V_PORT_RCC         RCC_APB2Periph_GPIOC
//#define AD_5V_PIN              GPIO_Pin_1
//#define ADC_5V_CHANNEL		   11

//ADC123_IN11
#define AD_LINEA_PORT            GPIOC
#define AD_LINEA_PORT_RCC        RCC_APB2Periph_GPIOC
#define AD_LINEA_PIN             GPIO_Pin_1
#define ADC_LINEA_CHANNEL		   11

//ADC123_IN12
#define AD_LINEB_PORT             GPIOC
#define AD_LINEB_PORT_RCC         RCC_APB2Periph_GPIOC
#define AD_LINEB_PIN              GPIO_Pin_2
#define ADC_LINEB_CHANNEL		   12



void AdcConfig(void);
u16 GetAdcMeasureValue(uint8_t adc1_channel, u8 retry);
u16 MeasureVoltage (uint8_t adc1_channel,u8 retry);




















#endif


