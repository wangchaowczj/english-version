#ifndef _ADC_H_
#define _ADC_H_

/***ADC GPIO����***/
#define ADC_LINAB_PORT          GPIOA
#define ADC_LINAB_PORT_RCC      RCC_AHB1Periph_GPIOA
#define ADC_LINAB_PIN           GPIO_Pin_5   //ADC12_IN5

#define ADC_BAT_PORT            GPIOA
#define ADC_BAT_PORT_RCC        RCC_AHB1Periph_GPIOA
#define ADC_BAT_PIN             GPIO_Pin_6 //ADC12_IN6


/***ADCX����ͨ������,bit7-bit6��ʾADCx,bit5-bit0��ʾͨ����***/
#define ADC_CH_LINAB            0x85 //ADC2_IN5
#define ADC_CH_BAT              0x86 //ADC2_IN6








typedef struct 
{
	float k ;
	u16 b ;	
}ADJUST_PARAMETER;

typedef struct
{
	ADJUST_PARAMETER bat;         //��ص�ѹУ׼
	ADJUST_PARAMETER linab;       //���ߵ�ѹ��ѹУ׼
}ADC_ADJUST;
extern ADC_ADJUST adc_adjust;     //ADCУ׼����

#define DEFAULT_ADC_PARAM     4.0525            //�ο���ѹ3.3V�� ��ѹ��20K/80.6K�������λΪmV
#define DEFAULT_I_LOW1_PARAM  0.4873            //��������200ŷ����ѹ�Ŵ�1.24*10���� ��ѹ��Ϊ20/30K
#define DEFAULT_I_LOW2_PARAM  (DEFAULT_I_LOW1_PARAM*10)  //��������200ŷ����ѹ�Ŵ�1.24*1���� ��ѹ��Ϊ100/150K

#define DEFAULT_I_HIGH1_PARAM  0.4873                      //��������200ŷ����ѹ�Ŵ�1.24*10���� ��ѹ��Ϊ100/150K
#define DEFAULT_I_HIGH2_PARAM  (DEFAULT_I_HIGH1_PARAM*10)  //��������200ŷ����ѹ�Ŵ�1.24*1���� ��ѹ��Ϊ100/150K



extern void ADCConfig(void);
extern void DMAConfig(void);
extern u16 GetADCValue(u8 ADC_Channel_x, u8 retry);
extern u16 GetBatteryVoltage(u16* AdcValue);
extern s16 GetLinABAdcValue(void);
extern s16 GetBusVoltageFromAdcValue(s16 AdcValue);
extern u16 GetBusVoltage(u8 retry, u16* AdcValue);
extern u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry);

#endif
