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
#ifndef __24lc04b_h__
#define __24lc04b_h__

//-----------------Include files-------------------------//

#define _24LC04B 0XA0

//----------------- Typedef------------------------------//

typedef  struct {
	int (* initialize)(void);
	unsigned char (* read)(int addr);
	int (* write)(int addr, unsigned char dat);
	int (* write_page)(int addr, unsigned char * buffer);
}EEPROM_T;

//----------------- Extern ------------------------------//

extern EEPROM_T eeprom;

#endif //__eeprom_h__
