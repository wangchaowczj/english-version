/*
 * FILE								: axp152.h
 * DESCRIPTION				: This file is axp152 driver file header.
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
#ifndef __axp152_h__
#define __axp152_h__

//----------------------- Include files ------------------------//

//--------------------------- Define ---------------------------//

#define	AXP152_CHIP_VERSION 				0x03
#define	AXP152_DCDC1_VOLTAGE 				0x26
#define	AXP152_DCDC2_VOLTAGE 				0x23
#define	AXP152_DCDC3_VOLTAGE 				0x27
#define	AXP152_DCDC4_VOLTAGE 				0x2B
#define	AXP152_DLDO1_VOLTAGE 				0x29
#define	AXP152_DLDO2_VOLTAGE 	 			0x2A
#define	AXP152_ALDO1_ALD02_VOLTAGE  0x28
#define	AXP152_SHUTDOWN 						0x32

#define AXP152_POWEROFF			(1 << 7)

/* For axp_gpio.c */
#define AXP152_I2C_ADDR							0x60
#define AXP_GPIO0_CTRL							0x90
#define AXP_GPIO1_CTRL							0x91
#define AXP_GPIO2_CTRL							0x92
#define AXP_GPIO3_CTRL							0x93
#define AXP_GPIO_CTRL_OUTPUT_LOW		0x00 /* Drive pin low */
#define AXP_GPIO_CTRL_OUTPUT_HIGH		0x01 /* Drive pin high */
#define AXP_GPIO_CTRL_INPUT					0x02 /* Input */
#define AXP_GPIO_STATE							0x97
#define AXP_GPIO_STATE_OFFSET				0x00

//------------------------- Data struct ------------------------//

typedef  struct {
int (*initialize)(void);
int (*set_dcdc1)(unsigned int mvolt);
int (*set_dcdc2)(unsigned int mvolt);
int (*set_dcdc3)(unsigned int mvolt);
int (*set_dcdc4)(unsigned int mvolt);
int (*set_dldo1)(unsigned int mvolt);
int (*set_dldo2)(unsigned int mvolt);
int (*set_aldo1)(unsigned int mvolt);
int (*set_aldo2)(unsigned int mvolt);
}AXP152_T;

//----------------------- Extern function ----------------------//

extern AXP152_T axp152;

#endif //__axp152_h__
