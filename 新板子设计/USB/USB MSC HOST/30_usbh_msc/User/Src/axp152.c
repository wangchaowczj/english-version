/*
 * FILE								: axp152.c
 * DESCRIPTION				: This file is axp152 driver.
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
//------------------------ Include files ------------------------//
#include "axp152.h"
#include "gpio.h"
#include "i2c.h"

//--------------------- Function Prototype ----------------------//
static unsigned char axp152_mvolt_to_target(int mvolt, int min, int max, int div);
static int set_dcdc1(unsigned int mvolt);
static int set_dcdc2(unsigned int mvolt);
static int set_dcdc3(unsigned int mvolt);
static int set_dcdc4(unsigned int mvolt);
static int set_dldo1(unsigned int mvolt);
static int set_dldo2(unsigned int mvolt);
static int set_aldo1(unsigned int mvolt);
static int set_aldo2(unsigned int mvolt);
static int initialize(void);

//--------------------------- Variable --------------------------// 
AXP152_T axp152 ={
.set_dcdc1 = set_dcdc1,
.set_dcdc2 = set_dcdc2,
.set_dcdc3 = set_dcdc3,
.set_dcdc4 = set_dcdc4,
.set_dldo1 = set_dldo1,
.set_dldo2 = set_dldo2,
.set_aldo1 = set_aldo1,
.set_aldo2 = set_aldo2,
.initialize      = initialize
};

//--------------------------- Function --------------------------//
/*
 * Name								: axp152_mvolt_to_target
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
static unsigned char axp152_mvolt_to_target(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return (mvolt - min) / div;
}
/*
 * Name								: set_dcdc1
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dcdc1(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 1700, 3500, 100);
	
	if(mvolt >= 2400 || mvolt <= 2800)target = target - 2;
	if(mvolt >= 3000 || mvolt <= 3500)target = target - 3;

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DCDC1_VOLTAGE,&target,1);
}
/*
 * Name								: set_dcdc2
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dcdc2(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 700, 2275, 25);

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DCDC2_VOLTAGE,&target,1);
}
/*
 * Name								: set_dcdc3
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dcdc3(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 700, 3500, 50);

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DCDC3_VOLTAGE,&target,1);
}
/*
 * Name								: set_dcdc4
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dcdc4(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 700, 3500, 25);

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DCDC4_VOLTAGE,&target,1);
}
/*
 * Name								: set_dldo1
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dldo1(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 700, 3500, 100);

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DLDO1_VOLTAGE,&target,1);	
}
/*
 * Name								: set_dldo2
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_dldo2(unsigned int mvolt)
{
	unsigned char target = axp152_mvolt_to_target(mvolt, 700, 3500, 100);

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_DLDO2_VOLTAGE,&target,1);
}
/*
 * Name								: set_aldo1
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_aldo1(unsigned int mvolt)
{
	unsigned char data;
	unsigned char target;

	if(mvolt >= 1200 && mvolt <= 2000){
		target = axp152_mvolt_to_target(mvolt, 1200, 2000, 100);
	}else if(mvolt == 2500){
		target = 0x09;
	}else if(mvolt == 2700){
		target = 0x0A;
	}else if(mvolt == 2800){
		target = 0x0B;
	}else if(mvolt >= 3000 || mvolt <= 3300){
		target = axp152_mvolt_to_target(mvolt, 3000, 3300, 100);
		target += 0x0C;
	}
	
	i2c.read_nbyte(AXP152_I2C_ADDR,AXP152_ALDO1_ALD02_VOLTAGE,&data,1);
	
	data &= 0x0F;
	
	target = target << 4;

	target &= 0xF0;

	target = data | target;
	
	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_ALDO1_ALD02_VOLTAGE,&target,1);
}
/*
 * Name								: set_aldo2
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int set_aldo2(unsigned int mvolt)
{
	unsigned char data;
	unsigned char target;

	if(mvolt >= 1200 && mvolt <= 2000){
		target = axp152_mvolt_to_target(mvolt, 1200, 2000, 100);
	}else if(mvolt == 2500){
		target = 0x09;
	}else if(mvolt == 2700){
		target = 0x0A;
	}else if(mvolt == 2800){
		target = 0x0B;
	}else if(mvolt >= 3000 || mvolt <= 3300){
		target = axp152_mvolt_to_target(mvolt, 3000, 3300, 100);
		target += 0x0C;
	}

	i2c.read_nbyte(AXP152_I2C_ADDR,AXP152_ALDO1_ALD02_VOLTAGE,&data,1);

	data &= 0xF0;

	target = target & 0x0F;

	target = data | target;

	return i2c.write_nbyte(AXP152_I2C_ADDR,AXP152_ALDO1_ALD02_VOLTAGE,&target,1);
}
/*
 * Name								: initialize
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 11/12/2019
 * 
 * create.
 * ----------------------
 */
int initialize(void)
{
	unsigned char ver;
	int rc;
	
	rc = i2c.read_nbyte(AXP152_I2C_ADDR,AXP152_CHIP_VERSION,&ver,1);

	if (rc){
		return rc;
	}

	if (ver != 0x05){
		return -1;
	}

	return 0;
}
