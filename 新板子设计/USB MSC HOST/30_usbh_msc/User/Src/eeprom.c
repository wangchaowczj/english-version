/*
 * FILE								: eeprom.c
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
//-----------------Include files-------------------------//
#include "i2c.h"
#include "eeprom.h"

//---------------- Function Prototype -------------------//
static int initialize(void);
static unsigned char  read(int addr);
static int write(int addr, unsigned char dat);
static int write_page(int addr, unsigned char * buffer);

//---------------- Variable -----------------------------//
EEPROM_T eeprom = {
	.initialize = initialize,
	.read = read,
	.write = write,
	.write_page = write_page
};
//-----------------Function------------------------------//
/*
 * Name               : initialize
 * Description        : ---
 * Author             : zh.
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 06/12/2017
 *
 * create.
 * -------------------
 */
static int initialize(void)
{
	i2c.initialize();

	return 0;
}

/*
 * Name               : write
 * Description        : ---
 * Author             : zh.
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 06/12/2017
 *
 * create.
 * -------------------
 */
static int
write(int addr, unsigned char dat)
{
	int i;
	unsigned char madd;

	if (addr > 255) { //select block 0~255 / 256 ~ 511
		madd = _24LC04B | 0X02;
		addr -= 256;
	}else {
		madd = _24LC04B;
	}
	i2c.write_nbyte(madd, addr, &dat, 1);

	for (i = 0; i < 300000; i++) ;

	return 0;
}
/*
 * Name               : write_page
 * Description        : ---
 * Author             : zh.
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 06/12/2017
 *
 * create.
 * -------------------
 */
static int
write_page(int addr, unsigned char * buffer)
{
	int i;
	unsigned char madd;

	if (addr > 255) { //select block 0~255 / 256 ~ 511
		madd = _24LC04B | 0X02;
		addr -= 256;
	}else {
		madd = _24LC04B;
	}
	i2c.write_nbyte(madd, addr, buffer, 16);

	for (i = 0; i < 300000; i++) ;

	return 0;
}
/*
 * Name               : read
 * Description        : ---
 * Author             : zh.
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 06/12/2017
 *
 * create.
 * -------------------
 */
static unsigned char
read(int addr)
{
	unsigned char madd;
	unsigned char dat;

	if (addr > 255) { //select block 0~255 / 256 ~ 511
		madd = _24LC04B | 0X02;
		addr -= 256;
	}else {
		madd = _24LC04B;
	}

	i2c.read_nbyte(madd, addr, &dat, 1);

	return dat;
}
