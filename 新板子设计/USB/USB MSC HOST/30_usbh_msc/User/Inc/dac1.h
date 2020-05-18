/*
 * FILE               : dac1.h
 * DESCRIPTION        : This file is dac1.c file header.
 * Author             : zh
 * Copyright          :
 *
 * History
 * --------------------
 * Rev                : 0.00
 * Date               : 08/18/2017
 *
 * create.
 * --------------------
 */
#ifndef __dac_h__
#define __dac_h__

//----------------------- Include files ------------------------//

//--------------------------- Define ---------------------------//				
						
//-------------------------- Typedef ---------------------------//
typedef struct{
	void(* set_voltage)(double);
}DAC_T;

//-------------------------- Extern ----------------------------//
extern DAC_T dac1;

#endif // __dac_h__
