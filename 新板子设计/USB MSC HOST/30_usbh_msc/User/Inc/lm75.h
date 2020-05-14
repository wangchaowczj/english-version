/*
 * FILE								: lm75.h
 * DESCRIPTION				: This file is lm75 driver file header.
 * Author							: zh.
 * Copyright					:
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/30/2019
 *
 * create.
 * -------------------
 */
#ifndef __lm75_h__
#define __lm75_h__

//-----------------Include files-------------------------//

//------------------Define-------------------------------//

#define LM75	0X90
 
//----------------- Typedef------------------------------//
typedef  struct{
	int (* initialize)(void);
	float (* read)(void);
	float temperature_value;
}LM75_T;
//----------------- Extern ------------------------------//
extern LM75_T lm75;
#endif //__lm75_h__
