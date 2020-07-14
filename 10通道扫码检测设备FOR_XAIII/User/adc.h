/**
  *************************************************************************************
  * @file    ADC.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ���ļ�ΪADC.c��ͷ�ļ�
  *************************************************************************************
  */
#ifndef _ADC_H_
#define _ADC_H_

/**
 *@defgroup PowerBoard
 *@{
 */
/**
 *@defgroup ADC
 *@{
 */
/**
 *@defgroup ADC_PinDefines
 *@brief ADC���Ŷ���
 *@{
 */
												

/***ADC GPIO����***/
#define ADC_CH10_LINA_PORT         		GPIOF
#define ADC_CH10_LINA_PORT_RCC     		RCC_AHB1Periph_GPIOF
#define ADC_CH10_LINA_PIN          		GPIO_Pin_3   //ADC3_IN9

#define ADC_CH10_LINB_PORT       		GPIOF
#define ADC_CH10_LINB_PORT_RCC   		RCC_AHB1Periph_GPIOF
#define ADC_CH10_LINB_PIN        		GPIO_Pin_4   //ADC3_IN14

#define ADC_CH9_LINA_PORT         		GPIOF
#define ADC_CH9_LINA_PORT_RCC     		RCC_AHB1Periph_GPIOF
#define ADC_CH9_LINA_PIN          		GPIO_Pin_5 //ADC3_IN15

#define ADC_CH9_LINB_PORT      			GPIOF
#define ADC_CH9_LINB_PORT_RCC  			RCC_AHB1Periph_GPIOF
#define ADC_CH9_LINB_PIN       			GPIO_Pin_6 //ADC3_IN4

#define ADC_CH1_TO_8_LINA_PORT          GPIOF
#define ADC_CH1_TO_8_LINA_PORT_RCC      RCC_AHB1Periph_GPIOF
#define ADC_CH1_TO_8_LINA_PIN           GPIO_Pin_7 //ADC3_IN5

#define ADC_CH1_TO_8_LINB_PORT          GPIOF
#define ADC_CH1_TO_8_LINB_PORT_RCC      RCC_AHB1Periph_GPIOF
#define ADC_CH1_TO_8_LINB_PIN           GPIO_Pin_8 //ADC3_IN6

#define ADC_BAT_PORT            		GPIOF
#define ADC_BAT_PORT_RCC        		RCC_AHB1Periph_GPIOF
#define ADC_BAT_PIN             		GPIO_Pin_9 //ADC3_IN7

#define ADC_5V_PORT            			GPIOF
#define ADC_5V_PORT_RCC        			RCC_AHB1Periph_GPIOF
#define ADC_5V_PIN             			GPIO_Pin_10 //ADC3_IN8

#define ADC_LV_PORT            			GPIOA
#define ADC_LV_PORT_RCC        			RCC_AHB1Periph_GPIOA
#define ADC_LV_PIN             			GPIO_Pin_0 //ADC123_IN0

#define ADC_SCAN_PORT            		GPIOA
#define ADC_SCAN_PORT_RCC        		RCC_AHB1Periph_GPIOA
#define ADC_SCAN_PIN             		GPIO_Pin_1 //ADC123_IN1

#define ADC_LINE_I_PORT            		GPIOA
#define ADC_LINE_I_PORT_RCC        		RCC_AHB1Periph_GPIOA
#define ADC_LINE_I_PIN             		GPIO_Pin_2 //ADC123_IN2

#define ADC_HV_PORT            			GPIOA
#define ADC_HV_PORT_RCC        			RCC_AHB1Periph_GPIOA
#define ADC_HV_PIN             			GPIO_Pin_3 //ADC123_IN3

//ͨ��1-8ѡ����������
#define ADC_CH_CTRL_A_PORT         				GPIOA
#define ADC_CH_CTRL_A_PORT_RCC     				RCC_AHB1Periph_GPIOA
#define ADC_CH_CTRL_A_PIN          				GPIO_Pin_8
#define ADC_CH_CTRL_A_L()                   	GPIO_ResetBits(ADC_CH_CTRL_A_PORT, ADC_CH_CTRL_A_PIN)
#define ADC_CH_CTRL_A_H()                    	GPIO_SetBits(ADC_CH_CTRL_A_PORT, ADC_CH_CTRL_A_PIN)

#define ADC_CH_CTRL_B_PORT         				GPIOA
#define ADC_CH_CTRL_B_PORT_RCC     				RCC_AHB1Periph_GPIOA
#define ADC_CH_CTRL_B_PIN          				GPIO_Pin_9
#define ADC_CH_CTRL_B_L()                   	GPIO_ResetBits(ADC_CH_CTRL_B_PORT, ADC_CH_CTRL_B_PIN)
#define ADC_CH_CTRL_B_H()                    	GPIO_SetBits(ADC_CH_CTRL_B_PORT, ADC_CH_CTRL_B_PIN)

#define ADC_CH_CTRL_C_PORT         				GPIOA
#define ADC_CH_CTRL_C_PORT_RCC     				RCC_AHB1Periph_GPIOA
#define ADC_CH_CTRL_C_PIN          				GPIO_Pin_10
#define ADC_CH_CTRL_C_L()                   	GPIO_ResetBits(ADC_CH_CTRL_C_PORT, ADC_CH_CTRL_C_PIN)
#define ADC_CH_CTRL_C_H()                    	GPIO_SetBits(ADC_CH_CTRL_C_PORT, ADC_CH_CTRL_C_PIN)

#define ADC_CH_CTRL_OE_PORT         			GPIOA
#define ADC_CH_CTRL_OE_PORT_RCC     			RCC_AHB1Periph_GPIOA
#define ADC_CH_CTRL_OE_PIN          			GPIO_Pin_11
#define ADC_CH_CTRL_OE_L()                   	GPIO_ResetBits(ADC_CH_CTRL_OE_PORT, ADC_CH_CTRL_OE_PIN)
#define ADC_CH_CTRL_OE_H()                    	GPIO_SetBits(ADC_CH_CTRL_OE_PORT, ADC_CH_CTRL_OE_PIN)

#define ADC_SEL_CH1()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_L();\
												ADC_CH_CTRL_B_L();\
												ADC_CH_CTRL_C_L()
												
#define ADC_SEL_CH2()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_H();\
												ADC_CH_CTRL_B_L();\
												ADC_CH_CTRL_C_L() 

#define ADC_SEL_CH3()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_L();\
												ADC_CH_CTRL_B_H();\
												ADC_CH_CTRL_C_L()
												
#define ADC_SEL_CH4()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_H();\
												ADC_CH_CTRL_B_H();\
												ADC_CH_CTRL_C_L()

#define ADC_SEL_CH5()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_L();\
												ADC_CH_CTRL_B_L();\
												ADC_CH_CTRL_C_H()
												
#define ADC_SEL_CH6()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_H();\
												ADC_CH_CTRL_B_L();\
												ADC_CH_CTRL_C_H() 

#define ADC_SEL_CH7()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_L();\
												ADC_CH_CTRL_B_H();\
												ADC_CH_CTRL_C_H()
												
#define ADC_SEL_CH8()           				ADC_CH_CTRL_OE_L();\
												ADC_CH_CTRL_A_H();\
												ADC_CH_CTRL_B_H();\
												ADC_CH_CTRL_C_H()												
/**
 *@} ADC_PinDefines
 */

/**
 *@defgroup ADC_DefaultParam
 *@brief ADC��ѹ����Ĭ�ϲ���
 *@{
 */
            
#define AD_5V_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)
#define AD_HV_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)
#define AD_SCAN_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)
#define AD_BAT_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)
#define AD_LINA_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)  //2.41699 //��ѹ��Ϊ1/(2+1)
#define AD_LINB_DEFAULT_PARAM          	 4.02832//2.41699
#define AD_LV_DEFAULT_PARAM          	 4.02832  //��ѹ��Ϊ1/(1+4)


#define AD_CURRENT_DEFAULT_PARAM      1.5012

/**
 *@} ADC_DefaultParam
 */
/**
 *@defgroup ADC_DataStruct
 *@brief ���ݽṹ����
 *@{
 */
 
/**
 *@brief ADCͨ������
 */  
typedef enum 
{
	ADC_CH_LV = 0,
	ADC_CH_SCAN,
	ADC_CH_LINE_I,
	ADC_CH_HV,
	ADC_CH_CH9_LINB,
	ADC_CH_CH1_TO_8_LINA,
	ADC_CH_CH1_TO_8_LINB,
	ADC_CH_BAT,
	ADC_CH_5V,
	ADC_CH_CH10_LINA,
	ADC_CH_CH10_LINB = 14,
	ADC_CH_CH9_LINA
}ADC3_CHANNEL;

/**
 *@brief ADCУ׼�����ṹ����
 */
typedef struct 
{
	float k ;
	u16 b ;	
}ADJUST_PARAMETER;

/**
 *@brief ADCУ׼��������
 */
typedef struct
{
	ADJUST_PARAMETER Current; /**<����У׼����*/
	ADJUST_PARAMETER LinAB1To8;/**<�˿ڵ�ѹУ׼����*/
	ADJUST_PARAMETER LinAB9;
	ADJUST_PARAMETER LinAB10;
	ADJUST_PARAMETER Bat;/**<��ص�ѹУ׼����*/
	u8 Time[14];
}ADC_ADJUST;
extern ADC_ADJUST AdcAdjust;     /**<ADCУ׼����*/

typedef enum 
{
	S_UNDONE = 0,    /**<û��ʼ�Լ�*/
	S_ERROR = 1,     /**<�Լ��쳣*/
	S_SUCCESS = 2,   /**<�Լ�����*/
	S_DOING = 0xFF   /**<�Լ������*/
}SELFTEST_STATE;

//5.5
#define LINAB_LV_VOLTAGE_MIN  5100  //�����һ��ͨ����·������ͨ��Ҳ�ᱻ���ͣ����0.2V����׼����Щ��������
#define LINAB_LV_VOLTAGE_MAX  5800
//11.5
#define LINAB_HV_VOLTAGE_MIN  11200
#define LINAB_HV_VOLTAGE_MAX  11800
/**
 *@} ADC_DataStruct
 */

/**
  *@}
  */
/**
  *@}
  */
extern void AdcConfig(void);
extern void SetAdcCh1To8Connect(u8 Channel);
extern u16 GetCurrentValue(u8 num, u16* AdcMeasureValue);
extern u16 GetLinABVoltage(u8 Channel, u16* AdcMeasureValue);
extern u16 GetBatVoltage(u16* AdcMeasureValue);
extern u16 GetVoltageValue(ADC3_CHANNEL channel);
extern u8 DeviceSelfTest(void);

#endif
