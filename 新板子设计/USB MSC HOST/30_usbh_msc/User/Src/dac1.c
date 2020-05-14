/*
 * FILE                : dac1.c
 * DESCRIPTION         : This file is iCore4 dac driver demo.
 * Author              : zh
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 08/18/2017
 *
 * create.
 * --------------------
 */
//------------------------ Include files ------------------------//
#include "dac.h"
#include "dac1.h"

#include "stm32h7xx_hal.h"

//--------------------------- Variable --------------------------// 
static void set_voltage(double);
DAC_T dac1 = {	
	.set_voltage = set_voltage
	};

//--------------------------- Function --------------------------//

/*
 * Name               : set_voltage
 * Description        : ---
 * Author             : zh
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/03/2016
 *
 * create.
 * -------------------
 */
void set_voltage(double voltage)
{	
	unsigned short int temp;
	
	temp = voltage * 4095 / 2.5;
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
}
