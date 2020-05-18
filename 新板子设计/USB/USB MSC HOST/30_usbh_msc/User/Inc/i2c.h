/*
 * FILE								: i2c.h
 * DESCRIPTION				: This file is i2c driver file header.
 * Author							: zh.
 * Copyright					:
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 *
 * create.
 * -------------------
 */
#ifndef __i2c_h__
#define __i2c_h__

//----------------------- Include files ------------------------//

#include "stm32h7xx_hal.h"

//-------------------------Define-------------------------------//


#define SCL_OUTPUT      		GPIO_uInitStructure.Pin = SCL_Pin;\
		GPIO_uInitStructure.Mode = GPIO_MODE_OUTPUT_PP;\
		GPIO_uInitStructure.Pull = GPIO_PULLUP;\
		GPIO_uInitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;\
		HAL_GPIO_Init(SCL_GPIO_Port,&GPIO_uInitStructure)

#define SDA_OUTPUT      GPIO_uInitStructure.Pin = SDA_Pin;\
		GPIO_uInitStructure.Mode = GPIO_MODE_OUTPUT_PP;\
		GPIO_uInitStructure.Pull = GPIO_PULLUP;\
		GPIO_uInitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;\
		HAL_GPIO_Init(SDA_GPIO_Port,&GPIO_uInitStructure)

#define SDA_INPUT       	GPIO_uInitStructure.Pin = SDA_Pin;\
		GPIO_uInitStructure.Mode = GPIO_MODE_INPUT;\
		GPIO_uInitStructure.Pull = GPIO_PULLUP;\
		GPIO_uInitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;\
		HAL_GPIO_Init(SDA_GPIO_Port,&GPIO_uInitStructure)

#define SCL_ON          HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_SET)
#define SCL_OFF         HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_RESET)

#define SDA_ON          HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_SET)
#define SDA_OFF         HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_RESET)

#define SDA_DATA        HAL_GPIO_ReadPin(SDA_GPIO_Port,SDA_Pin)

//------------------------- Data struct ------------------------//
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
}MY_I2C_T;

//---------------------------- Extern --------------------------//
extern GPIO_InitTypeDef   GPIO_uInitStructure;
extern MY_I2C_T i2c;

#endif //__i2c_h__
