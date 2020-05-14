/*
 * FILE								: i2c_b.c
 * DESCRIPTION				: This file is i2c_b driver.
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
//---------------- Include files ------------------------//

#include "i2c_b.h"
#include "stm32h7xx_hal.h"


//---------------- Function Prototype -------------------//

static int initialize(void);
static unsigned char read(void);
static int write(unsigned char data);
static int start(void);
static int stop(void);
static int ack(char);
static int read_nbyte(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
static int write_nbyte(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);

//---------------- Variable -----------------------------//
MY_I2C_B_T i2c_b = {
	.initialize = initialize,
	.read = read,
	.write = write,
	.start = start,
	.stop = stop,

	.ack = ack,
	.read_nbyte = read_nbyte,
	.write_nbyte = write_nbyte
};

//-----------------Function------------------------------//
/*
 * Name								: delay
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static void delay(void)
{
	int i;

	for (i = 0; i < 200; i++) ;
}
/*
 * Name								: initialize
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int initialize(void)
{	
	SDA_OUTPUT;
	SCL_OUTPUT;
	
	SDA_ON;
	SCL_ON;

	return 0;
}
/*
 * Name								: start
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int start(void)
{
	SDA_ON;
	delay();

	SCL_ON;
	delay();

	SDA_OFF;
	delay();

	SCL_OFF;
	delay();

	return 0;
}
/*
 * Name								: stop
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int stop(void)
{
//	SDA_OUTPUT;

	SDA_OFF;
	delay();

	SCL_ON;
	delay();

	SDA_ON;
	delay();

	return 0;
}
/*
 * Name								: ack
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int ack(char a)
{
//	SDA_OUTPUT;

	if (a) {
		SDA_OFF;
	}else {
		SDA_ON;
	}
	delay();
	SCL_ON;
	delay();
	SCL_OFF;

	delay();

	return 0;
};
/*
 * Name								: write
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int write(unsigned char data)
{
	unsigned char i = 8;
//	SDA_OUTPUT;

	for (i = 0; i < 8; i++) {
		if ((data << i) & 0x80) SDA_ON;
		else SDA_OFF;
		delay();
		SCL_ON;
		delay();
		SCL_OFF;
	}
	SDA_INPUT;
	delay();

	delay();
	SCL_ON;

	delay();
	if (SDA_DATA) i2c_b.ack_flag = 0;
	else i2c_b.ack_flag = 1;
	SCL_OFF;
	SDA_OUTPUT;
	delay();

	return 0;
}
/*
 * Name								: read
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static unsigned char read(void)
{
	unsigned char retc;
	unsigned char BitCnt;

	retc = 0;

	SDA_ON; 
	SDA_INPUT;

	for (BitCnt = 0; BitCnt < 8; BitCnt++) {
		delay();
		SCL_OFF; 
		delay();
		SCL_ON; 
		delay();
		retc = retc << 1;
		if ((SDA_DATA) == 1) retc = retc + 1; 
		delay();
	}
	SCL_OFF;
	delay();

	SDA_OUTPUT;

	return(retc);
}
/*
 * Name								: read_nbyte
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int read_nbyte(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
{
	unsigned char i;

	i2c_b.start();
	i2c_b.write(sla);
	if(i2c_b.ack_flag==0)return(0);
	
	i2c_b.write(suba);
	if(i2c_b.ack_flag==0)return(0);
	
	i2c_b.start();
	i2c_b.write(sla + 1);
	if(i2c_b.ack_flag==0)return(0);
	
	for (i = 0; i < no - 1; i++) {
		*s = i2c_b.read();
		i2c_b.ack(1);
		s++;
	}
	*s = i2c_b.read();
	i2c_b.ack(0);
	i2c_b.stop();

	return(1);
}
/*
 * Name								: write_nbyte
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 07/11/2019
 * 
 * create.
 * ----------------------
 */
static int write_nbyte(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
{
	unsigned char i;
	
	i2c_b.start();
	i2c_b.write(sla);
 	if(i2c_b.ack_flag==0) return(0);
	
	i2c_b.write(suba);
	if(i2c_b.ack_flag==0) return(0);

	for(i=0;i<no;i++){
		i2c_b.write(*s);
		if(i2c_b.ack_flag==0) return(0);
		s++;
	}
	i2c_b.stop();

	return(1);
}
