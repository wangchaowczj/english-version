#ifndef _ADC_H_
#define _ADC_H_

//高低压电流测试切换引脚
#define I_TEST_PORT         GPIOF
#define I_TEST_PORT_RCC     RCC_AHB1Periph_GPIOF
#define I_TEST_PIN          GPIO_Pin_0   //
#define SET_MEASURE_LOW()   GPIO_SetBits(I_TEST_PORT, I_TEST_PIN)
#define SET_MEASURE_HIGH()  GPIO_ResetBits(I_TEST_PORT, I_TEST_PIN)

/***ADC GPIO定义***/
#define ADC_COMM_PORT         GPIOB
#define ADC_COMM_PORT_RCC     RCC_AHB1Periph_GPIOB
#define ADC_COMM_PIN          GPIO_Pin_1   //ADC12_IN9

#define ADC_6_7V_PORT         GPIOF
#define ADC_6_7V_PORT_RCC     RCC_AHB1Periph_GPIOF
#define ADC_6_7V_PIN          GPIO_Pin_6   //ADC3_IN4

#define ADC_LOW1_I_PORT       GPIOF
#define ADC_LOW1_I_PORT_RCC   RCC_AHB1Periph_GPIOF
#define ADC_LOW1_I_PIN        GPIO_Pin_7   //ADC3_IN5

#define ADC_LOW2_I_PORT       GPIOF
#define ADC_LOW2_I_PORT_RCC   RCC_AHB1Periph_GPIOF
#define ADC_LOW2_I_PIN        GPIO_Pin_8  //ADC3_IN6


#define ADC_11_3V_PORT         GPIOC
#define ADC_11_3V_PORT_RCC     RCC_AHB1Periph_GPIOC
#define ADC_11_3V_PIN          GPIO_Pin_5 //ADC12_IN15

#define ADC_LINA_PORT          GPIOA
#define ADC_LINA_PORT_RCC      RCC_AHB1Periph_GPIOA
#define ADC_LINA_PIN           GPIO_Pin_6 //ADC12_IN6

#define ADC_LINB_PORT          GPIOC
#define ADC_LINB_PORT_RCC      RCC_AHB1Periph_GPIOC
#define ADC_LINB_PIN           GPIO_Pin_0 //ADC123_IN10

#define ADC_5V_PORT            GPIOC
#define ADC_5V_PORT_RCC        RCC_AHB1Periph_GPIOC
#define ADC_5V_PIN             GPIO_Pin_4 //ADC12_IN14

#define ADC_BAT_PORT            GPIOC
#define ADC_BAT_PORT_RCC        RCC_AHB1Periph_GPIOC
#define ADC_BAT_PIN             GPIO_Pin_1 //ADC123_IN11


/***ADCX测量通道定义,bit7-bit6表示ADCx,bit5-bit0表示通道号***/
#define ADC_CH_COMM       	 0x49 //ADC1_IN9
#define ADC_CH_11_3V         0x4F //ADC1_IN15
#define ADC_CH_LINA          0x46 //ADC1_IN6
#define ADC_CH_LINB          0x4A //ADC1_IN10
#define ADC_CH_5V            0xCE //ADC1_IN14
#define ADC_CH_BAT           0xCB //ADC1_IN11

#define ADC_CH_6_7V          0xC4 //ADC3_IN4
#define ADC_CH_LOW1_I        0xC5 //ADC3_IN5
#define ADC_CH_LOW2_I        0xC6 //ADC3_IN6







typedef struct 
{
	float k ;
	u16 b ;	
}ADJUST_PARAMETER;

typedef struct
{
	ADJUST_PARAMETER bat;         //电池电压校准
	ADJUST_PARAMETER linab;       //总线电压电压校准
	ADJUST_PARAMETER low1current; //低压下电流校准参数(小量程)
	ADJUST_PARAMETER low2current; //低压下电流校准参数(大量程)
	ADJUST_PARAMETER high1current;//高压压下电流校准参数(小量程)
	ADJUST_PARAMETER high2current;//高压下电流校准参数(大量程)	
}ADC_ADJUST;
extern ADC_ADJUST adc_adjust;     //ADC校准参数

#define DEFAULT_ADC_PARAM     4.0525            //参考电压3.3V， 分压比20K/80.6K，换算后单位为mV
#define DEFAULT_I_LOW1_PARAM  0.4873            //采样电阻200欧，电压放大1.24*10倍， 分压比为20/30K
#define DEFAULT_I_LOW2_PARAM  (DEFAULT_I_LOW1_PARAM*10)  //采样电阻200欧，电压放大1.24*1倍， 分压比为100/150K

#define DEFAULT_I_HIGH1_PARAM  0.4873                      //采样电阻200欧，电压放大1.24*10倍， 分压比为100/150K
#define DEFAULT_I_HIGH2_PARAM  (DEFAULT_I_HIGH1_PARAM*10)  //采样电阻200欧，电压放大1.24*1倍， 分压比为100/150K


#define DET_WAVE_POINT_COUNT       150
extern u16 DetWave[];

//#define LIN_SHORT_CURRENT  15000
#define LIN_NORMAL_LOW_VOLTAGE_LIMIT   	300
#define LIN_NORMAL_LOW_VOLTAGE_L_GOAL   7000
#define LIN_NORMAL_LOW_VOLTAGE_H_GOAL   7000
#define LIN_NORMAL_LOW_VOLTAGE_ADJUST_MAX   8000
#define LIN_NORMAL_LOW_VOLTAGE_DET_ANALYS_GOAL   5500 //雷管分析时电压

#define LIN_NORMAL_HIGH_VOLTAGE_LIMIT   800
#define LIN_NORMAL_HIGH_VOLTAGE_GOAL    11400
#define LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX   12200
#define LIN_NORMAL_HIGH_VOLTAGE_FACTORY_GOAL    10400

extern void ADCConfig(void);
extern u16 GetADCValue(u8 ADC_Channel_x, u8 retry);
extern u16 GetBatteryVoltage(u16* AdcValue);
extern s16 GetLinABAdcValue(void);
extern s16 GetBusVoltageFromAdcValue(s16 AdcValue);
extern u16 GetBusVoltage(u8 retry, u16* AdcValue);
extern u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry);
extern u16 GetBusLowCurrent(u8 rd_count, u16* AdcValue);
extern u16 GetBusHighCurrent(u8 rd_count, u16* AdcValue);
extern u16 GetBusCurrent(u8 rd_count, u16* AdcValue);
extern u16 GetBusLeakCurrent(u16* AdcValue);
extern ErrorStatus CheckHVProtectPoint(void);
extern u8 BusIsShort(void);
extern u8 BusCurrentInRange(u16 BusCurrent);
extern u16 BusCurrentRangeMax(void);
extern void GetDetWaveADCValue(void);

#endif
