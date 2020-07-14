#ifndef _ADC_H_
#define _ADC_H_

/***ADC GPIO定义***/
#define ADC_LINAB_PORT          GPIOA
#define ADC_LINAB_PORT_RCC      RCC_AHB1Periph_GPIOA
#define ADC_LINAB_PIN           GPIO_Pin_5   //ADC12_IN5

#define ADC_BAT_PORT            GPIOA
#define ADC_BAT_PORT_RCC        RCC_AHB1Periph_GPIOA
#define ADC_BAT_PIN             GPIO_Pin_6 //ADC12_IN6


/***ADCX测量通道定义,bit7-bit6表示ADCx,bit5-bit0表示通道号***/
#define ADC_CH_LINAB            0x85 //ADC2_IN5
#define ADC_CH_BAT              0x86 //ADC2_IN6








typedef struct 
{
	float k ;
	u16 b ;	
}ADJUST_PARAMETER;

typedef struct
{
	ADJUST_PARAMETER bat;         //电池电压校准
	ADJUST_PARAMETER linab;       //总线电压电压校准
}ADC_ADJUST;
extern ADC_ADJUST adc_adjust;     //ADC校准参数

#define DEFAULT_ADC_PARAM     4.0525            //参考电压3.3V， 分压比20K/80.6K，换算后单位为mV
#define DEFAULT_I_LOW1_PARAM  0.4873            //采样电阻200欧，电压放大1.24*10倍， 分压比为20/30K
#define DEFAULT_I_LOW2_PARAM  (DEFAULT_I_LOW1_PARAM*10)  //采样电阻200欧，电压放大1.24*1倍， 分压比为100/150K

#define DEFAULT_I_HIGH1_PARAM  0.4873                      //采样电阻200欧，电压放大1.24*10倍， 分压比为100/150K
#define DEFAULT_I_HIGH2_PARAM  (DEFAULT_I_HIGH1_PARAM*10)  //采样电阻200欧，电压放大1.24*1倍， 分压比为100/150K



extern void ADCConfig(void);
extern void DMAConfig(void);
extern u16 GetADCValue(u8 ADC_Channel_x, u8 retry);
extern u16 GetBatteryVoltage(u16* AdcValue);
extern s16 GetLinABAdcValue(void);
extern s16 GetBusVoltageFromAdcValue(s16 AdcValue);
extern u16 GetBusVoltage(u8 retry, u16* AdcValue);
extern u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry);

#endif
