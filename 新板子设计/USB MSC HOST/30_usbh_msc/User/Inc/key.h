/*
 * FILE                : key.h
 * DESCRIPTION         : This file is for key.c
 * Author              : zh.
 * Copyright           : 
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 02/27/2020
 *
 * create.
 * --------------------
 */
#ifndef __key_h__
#define __key_h__

//--------------------------- Define ---------------------------//

#define ARM_KEY_VALUE HAL_GPIO_ReadPin(ARM_KEY_GPIO_Port,ARM_KEY_Pin)

#define ARM_KEY 1

#define KEY_NONE 255

//----------------------- Include files ------------------------//

//-------------------------- Typedef ---------------------------//
typedef struct {
	unsigned char (* read)(void);
	void(* process)(void);
	unsigned char value;
	unsigned char bak_value;
}KEY_T;

//--------------------------- Extern ---------------------------//
extern KEY_T key;

#endif //__key_h__
