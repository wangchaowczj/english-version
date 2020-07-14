#ifndef _AD_H_
#define _AD_H_

//©���������л����š���6.7_CONVERT����PC9
#define VSHIFT_PORT             	GPIOC
#define VSHIFT_PORT_RCC         	RCC_APB2Periph_GPIOC
#define VSHIFT_PIN              	GPIO_Pin_9
#define VSHIFT_L() 								GPIO_ResetBits(VSHIFT_PORT,VSHIFT_PIN) //���6.7V
#define VSHIFT_H() 								GPIO_SetBits(VSHIFT_PORT,VSHIFT_PIN)   //���5V

//©��������5Vʹ�����š���5V_SW����PA6
#define SW5V_PORT             		GPIOA
#define SW5V_PORT_RCC         		RCC_APB2Periph_GPIOA
#define SW5V_PIN              		GPIO_Pin_6
#define SW5V_L() 									GPIO_ResetBits(SW5V_PORT,SW5V_PIN) 			//��ֹ���5V
#define SW5V_H() 									GPIO_SetBits(SW5V_PORT,SW5V_PIN)   			//�������5V
                                                                      		
//LINEA��LINEB�����ѹ��������                                        		
#define ITX_PORT             			GPIOC                               		
#define ITX_PORT_RCC         			RCC_APB2Periph_GPIOC                		
#define ITX_PIN              			GPIO_Pin_7                          		
#define LINEA_OUT() 							GPIO_SetBits(ITX_PORT,ITX_PIN) 					//LINEA�����6.7V/5V��ѹ,LINEB�˲������ѹ������Ϊ��·��һ����
#define LINEB_OUT() 							GPIO_ResetBits(ITX_PORT,ITX_PIN)  			//LINEB�����6.7V/5V��ѹ��LINEA�˲������ѹ������Ϊ��·��һ����

/***ADC GPIO����***/
#define ADC1_RCC               		RCC_APB2Periph_ADC1

//ADC123_IN1 �����������
#define AD_I1_PORT             		GPIOA
#define AD_I1_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I1_PIN              		GPIO_Pin_1
#define ADC_I1_CHANNEL		   			1

//ADC123_IN2 �����С����
#define AD_I2_PORT             		GPIOA
#define AD_I2_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I2_PIN              		GPIO_Pin_2
#define ADC_I2_CHANNEL		   			2

//ADC123_IN10 ��Դ��ѹ
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

//ADC123_IN13 ���������ѹ
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
		ADJUST_PARAMETER bat;			//��ص�ѹУ׼
		ADJUST_PARAMETER linab;		//���ߵ�ѹУ׼
		ADJUST_PARAMETER current1;//����У׼������������0-10mA��
		ADJUST_PARAMETER current2;//����У׼������С����0-1mA��	
}ADC_ADJUST;
extern ADC_ADJUST adc_adjust; //ADCУ׼����

#define DEFAULT_ADC_PARAM 		0.004028  //���в��Ե�ѹ��ѹ��5:1����λΪmV
#define DEFAULT_I1_PARAM			2.6596    //��������Ϊ49.9R��С���̣��Ŵ���10����ѹ�����20K:30.9K
#define DEFAULT_I2_PARAM      0.2660		//��������Ϊ49.9R�������̣��Ŵ���100������ѹ�����20K:30.9K

void AdcConfig(void);
u16 GetAdcMeasureValue(uint8_t adc1_channel, u8 retry);
