/*
 * FILE               : adc.h
 * DESCRIPTION        : This file is iCore3 adc file header.
 * Author             : zh
 * Copyright          :
 *
 * History
 * --------------------
 * Rev                : 0.00
 * Date               : 07/31/2019
 *
 * create.
 * --------------------
 */
#ifndef __ADC_H__
#define __ADC_H__

//--------------------------- Define ---------------------------//

#define ADC_REF 2.495 * 0.9767

#define SEL_A_ON  HAL_GPIO_WritePin(GPIOI, SEL_A_Pin, GPIO_PIN_SET)
#define SEL_A_OFF HAL_GPIO_WritePin(GPIOI, SEL_A_Pin, GPIO_PIN_RESET)

#define SEL_B_ON  HAL_GPIO_WritePin(GPIOI, SEL_B_Pin, GPIO_PIN_SET)
#define SEL_B_OFF HAL_GPIO_WritePin(GPIOI, SEL_B_Pin, GPIO_PIN_RESET)

#define SEL_C_ON  HAL_GPIO_WritePin(GPIOI, SEL_C_Pin, GPIO_PIN_SET)
#define SEL_C_OFF HAL_GPIO_WritePin(GPIOI, SEL_C_Pin, GPIO_PIN_RESET)

#define CHANNEL_0_ON SEL_C_OFF;\
                     SEL_B_OFF;\
										 SEL_A_OFF

#define CHANNEL_1_ON SEL_C_OFF;\
                     SEL_B_OFF;\
										 SEL_A_ON
										 
#define CHANNEL_2_ON SEL_C_OFF;\
                     SEL_B_ON;\
										 SEL_A_OFF		

#define CHANNEL_3_ON SEL_C_OFF;\
                     SEL_B_ON;\
										 SEL_A_ON

#define CHANNEL_4_ON SEL_C_ON;\
                     SEL_B_OFF;\
										 SEL_A_OFF

#define CHANNEL_5_ON SEL_C_ON;\
                     SEL_B_OFF;\
										 SEL_A_ON
										 
#define CHANNEL_6_ON SEL_C_ON;\
                     SEL_B_ON;\
										 SEL_A_OFF	

#define CHANNEL_7_ON SEL_C_ON;\
                     SEL_B_ON;\
										 SEL_A_ON	

//-------------------------- Typedef----------------------------//

typedef  struct {
	int (* read)(int);
	int (* read_mux)(void);
	double value[9];

}SYS_ADC_T;

extern SYS_ADC_T my_adc;
#endif //__ADC_H__
