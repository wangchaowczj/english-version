/*
 * FILE                : adc.c
 * DESCRIPTION         : --
 * Author              : zh.
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 07/31/2019
 *
 * create.
 * --------------------
 */
//------------------------ Include files ------------------------//

#include "my_adc.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"

//--------------------- Function Prototype ----------------------//

static int read(int);
static int read_mux(void);

//-------------------------- Variable ---------------------------//

SYS_ADC_T my_adc = {
	.read = read,
	.read_mux = read_mux,
};

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

//--------------------------- Function --------------------------//
/*
 * Name                  : sort
 * Description           : ---
 * Author                : zh.
 *
 * History
 * ----------------------
 * Rev                   : 0.00
 * Date                  : 06/14/2017
 *
 * create.
 * ----------------------
 */
void sort(unsigned short int a[], int n)
{
	int i, j, t;
	
	//元素从小到大排列
	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (a[j] > a[j + 1]) {
				t = a[j];
				a[j] = a[j + 1];
				a[j + 1] = t;
			}
		}
	}
}
/*
 * Name                  : read
 * Description           : ---
 * Author                : zh.
 *
 * History
 * ----------------------
 * Rev                   : 0.00
 * Date                  : 07/31/2019
 *
 * create.
 * ----------------------
 */
int read(int channel)
{
	int i;
	unsigned long int temp = 0;
	unsigned short int data[50];
	ADC_ChannelConfTypeDef channel_config;
	int channel_remap[2] = {ADC_CHANNEL_16,ADC_CHANNEL_1};
	
	channel_config.Channel = channel_remap[channel];
	channel_config.Offset = 0;
	channel_config.Rank = ADC_REGULAR_RANK_1;
	channel_config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  channel_config.SingleDiff = ADC_SINGLE_ENDED;
  channel_config.OffsetNumber = ADC_OFFSET_NONE;	
	
	for(i = 0;i < 50;i ++){
		if(channel == 0){
			HAL_ADC_ConfigChannel(&hadc1,&channel_config);
			HAL_ADC_Start(&hadc1);
			while(!__HAL_ADC_GET_FLAG(&hadc1,ADC_FLAG_EOC));
			data[i] = HAL_ADC_GetValue(&hadc1);
		}else if(channel == 1){
			HAL_ADC_ConfigChannel(&hadc3,&channel_config);
			HAL_ADC_Start(&hadc3);
			while(!__HAL_ADC_GET_FLAG(&hadc3,ADC_FLAG_EOC));
			data[i] = HAL_ADC_GetValue(&hadc3);		
		}
	}
	sort(data,50);
	for(i = 20;i < 30;i++){
		temp += data[i];
	}

	temp = temp / 10;
	if(channel == 0){
		my_adc.value[channel] = temp * ADC_REF / 65536;
	}
	
	return temp;
}
/*
 * Name                  : read
 * Description           : ---
 * Author                : zh.
 *
 * History
 * ----------------------
 * Rev                   : 0.00
 * Date                  : 12/17/2019
 *
 * create.
 * ----------------------
 */
static int read_mux(void)
{
	CHANNEL_0_ON;
	my_adc.value[1] = my_adc.read(1) * ADC_REF / 65536;
	
	CHANNEL_1_ON;
	my_adc.value[2] = my_adc.read(1) * ADC_REF / 65536;
	
	CHANNEL_2_ON;
	my_adc.value[3] = my_adc.read(1) * ADC_REF / 65536;
	
	CHANNEL_3_ON;
	my_adc.value[4] = my_adc.read(1) * ADC_REF / 65536;	
	
	CHANNEL_4_ON;
	my_adc.value[5] = my_adc.read(1) * ADC_REF / 65536;	
	
	CHANNEL_5_ON;
	my_adc.value[6] = my_adc.read(1) * ADC_REF / 65536;	
	
	CHANNEL_6_ON;
	my_adc.value[7] = my_adc.read(1) * ADC_REF / 65536;	
	
	CHANNEL_7_ON;
	my_adc.value[8] = my_adc.read(1) * ADC_REF / 65536;	
	
	return 0;
}
