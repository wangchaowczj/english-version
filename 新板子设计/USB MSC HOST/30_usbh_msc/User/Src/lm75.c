/*
 * FILE               : lm75.c
 * DESCRIPTION        : This file is lm75 driver.
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
//--------------------------- Include ---------------------------//

#include "i2c_b.h"
#include "lm75.h"

//--------------------- Function Prototype ----------------------//

static int initialize(void);
static float read(void);

//--------------------------- Variable --------------------------//

LM75_T lm75={
	.initialize=initialize,
	.read=read
};

//--------------------------- Function --------------------------//
/*
 * Name                : initialize
 * Description         : ---
 * Author              : zh.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 07/30/2019
 * 
 * create.
 * --------------------
 */
static int initialize(void)
{
 	i2c_b.initialize();

	return 0;
} 

/*
 * Name                : read
 * Description         : ---
 * Author              : zh.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 07/30/2019
 * 
 * create.
 * --------------------
 */
static float read(void)
{
	union{
		unsigned char buf[2];
		short int value;
	}temp;
	float f;
	unsigned char c;

	i2c_b.read_nbyte(LM75,0,temp.buf,2);

	c = temp.buf[0];
	temp.buf[0] = temp.buf[1];
	temp.buf[1] = c;

	f = temp.value;
	f /= (float)32.0;
	f *= (float)0.125;

	lm75.temperature_value = f;
	return f;
}
