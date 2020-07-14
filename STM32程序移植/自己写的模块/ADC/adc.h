#ifndef _AD_H_
#define _AD_H_

//漏电流测试切换引脚——6.7_CONVERT——PC9
#define VSHIFT_PORT             	GPIOC
#define VSHIFT_PORT_RCC         	RCC_APB2Periph_GPIOC
#define VSHIFT_PIN              	GPIO_Pin_9
#define VSHIFT_L() 								GPIO_ResetBits(VSHIFT_PORT,VSHIFT_PIN) //输出6.7V
#define VSHIFT_H() 								GPIO_SetBits(VSHIFT_PORT,VSHIFT_PIN)   //输出5V

//漏电流测试5V使能引脚——5V_SW——PA6
#define SW5V_PORT             		GPIOA
#define SW5V_PORT_RCC         		RCC_APB2Periph_GPIOA
#define SW5V_PIN              		GPIO_Pin_6
#define SW5V_L() 									GPIO_ResetBits(SW5V_PORT,SW5V_PIN) 			//禁止输出5V
#define SW5V_H() 									GPIO_SetBits(SW5V_PORT,SW5V_PIN)   			//允许输出5V
                                                                      		
//LINEA和LINEB输出电压控制引脚                                        		
#define ITX_PORT             			GPIOC                               		
#define ITX_PORT_RCC         			RCC_APB2Periph_GPIOC                		
#define ITX_PIN              			GPIO_Pin_7                          		
#define LINEA_OUT() 							GPIO_SetBits(ITX_PORT,ITX_PIN) 					//LINEA端输出6.7V/5V电压,LINEB端不输出电压而是作为回路的一部分
#define LINEB_OUT() 							GPIO_ResetBits(ITX_PORT,ITX_PIN)  			//LINEB端输出6.7V/5V电压，LINEA端不输出电压而是作为回路的一部分

/***ADC GPIO定义***/
#define ADC1_RCC               		RCC_APB2Periph_ADC1

//ADC123_IN1 测电流大量程
#define AD_I1_PORT             		GPIOA
#define AD_I1_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I1_PIN              		GPIO_Pin_1
#define ADC_I1_CHANNEL		   			1

//ADC123_IN2 测电流小量程
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
#define AD_LINEB_PORT             GPIOC
#define AD_LINEB_PORT_RCC         RCC_APB2Periph_GPIOC
#define AD_LINEB_PIN              GPIO_Pin_2
#define ADC_LINEB_CHANNEL		   		12

//ADC123_IN13 总线输出电压
#define AD_COMOUT_PORT            GPIOC
#define AD_COMOUT_PORT_RCC        RCC_APB2Periph_GPIOC
#define AD_COMOUT_PIN             GPIO_Pin_3
#define ADC_COMOUT_CHANNEL		   	13

typedef struct    
{                 
	float k ;       
	u16 b ;	        
}ADJUST_PARAMETER;

typedef struct
{
		ADJUST_PARAMETER bat;			//电池电压校准
		ADJUST_PARAMETER linab;		//总线电压校准
		ADJUST_PARAMETER current1;//电流校准参数（大量程0-10mA）
		ADJUST_PARAMETER current2;//电流校准参数（小量程0-1mA）	
}ADC_ADJUST;
extern ADC_ADJUST adc_adjust; //ADC校准参数

#define DEFAULT_ADC_PARAM 		0.004028  //所有测试电压分压比5:1，单位为mV
#define DEFAULT_I1_PARAM			2.6596    //采样电阻为49.9R，小量程，放大倍数10，分压电阻比20K:30.9K
#define DEFAULT_I2_PARAM      0.2660		//采样电阻为49.9R，大量程，放大倍数100倍，分压电阻比20K:30.9K

void AdcConfig(void);
u16 GetAdcMeasureValue(uint8_t adc1_channel, u8 retry);
