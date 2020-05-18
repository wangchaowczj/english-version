/*
 * FILE								: fpga_ps.h
 * DESCRIPTION				: this file is for fpga_ps.h
 * Author							: zh.
 * Copyright					:
 *
 * History
 * --------------------
 * Rev								: 0.00
 * Date								: 06/15/2017
 *
 * create.
 * --------------------
 */

#ifndef __fpga_ps_h__
#define __fpga_ps_h__

#include "fatfs.h"

//-----------------define-------------------------------//

#define NCONFIG_ON	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET)
#define NCONFIG_OFF	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET)

#define DCLK_ON			HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, GPIO_PIN_SET)
#define DCLK_OFF		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, GPIO_PIN_RESET)

#define DATA0_ON		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET)
#define DATA0_OFF		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET)

#define NSTATUS			HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4)
#define CONFIG_DONE	HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)

//-----------------Include files-------------------------//

//----------------- Typedef------------------------------//
typedef struct {
	int (* initialize)(void);
	int (* from_tf)(void);
}FPGA_PS_T;

//----------------- Extern ------------------------------//
extern FPGA_PS_T fpga_ps;

#endif //__fpga_ps_h__


