/*
 * FILE								: i2c_b.h
 * DESCRIPTION				: This file is i2c_b driver file header.
 * Author							: zh.
 * Copyright					:
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 *
 * create.
 * -------------------
 */
//-------------------Define-----------------------------//

#ifndef __i2c_b_h__
#define __i2c_b_h__

//-----------------Include files-------------------------//

#include "stm32h7xx_hal.h"
#include "i2c.h"
#include "gpio.h"
//------------------Define-------------------------------//

//----------------- Typedef------------------------------//
typedef  struct {
	int (* initialize)(void);
	unsigned char (* read)(void);
	int (* write)(unsigned char data);
	int (* start)(void);
	int (* stop)(void);
	int (* check_ack)(void);
	int (* ack)(char);
	int ack_flag;
	int (* read_nbyte)(unsigned char, unsigned char, unsigned char *, unsigned char);
	int (* write_nbyte)(unsigned char, unsigned char, unsigned char *, unsigned char);
}MY_I2C_B_T;

//----------------- Extern ------------------------------//

extern MY_I2C_B_T i2c_b;

#endif //__i2c_b_h__
