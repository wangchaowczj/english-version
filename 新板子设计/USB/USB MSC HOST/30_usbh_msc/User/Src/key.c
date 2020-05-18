/*
 * FILE                : key.c
 * DESCRIPTION         : This file is key driver.
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
//--------------------------- Include ---------------------------//

#include "key.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"

//--------------------- Function Prototype ----------------------//

static unsigned char read(void);
static void process(void);

//--------------------------- Variable --------------------------//

KEY_T key = {
	.read = read,
	.process = process,
	.value = 0,
	.bak_value = 0,
};

//--------------------------- Function --------------------------//
/*
 * Name                : process
 * Description         : ---
 * Author              : zh.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 02/27/2020
 * 
 * create.
 * --------------------
 */
static unsigned char read(void)
{
	static unsigned char key_state = 0;
	static unsigned int key_value;
	static unsigned char key_return = 0;
	
	switch(key_state){
		case 0://按键被按下
			if(!(ARM_KEY_VALUE)){
				key_state ++;
				key_value = ARM_KEY_VALUE;
			}
			break;
		case 1://消抖处理
			if(key_value != ARM_KEY_VALUE){
				key_state --;
			}else{
				switch(key_value){
					case 0x00:
						key_return = ARM_KEY;
						break;
				}
				key_state ++;
			}
			break;
		case 2://按键释放
			if(ARM_KEY_VALUE == 0x01){
				key_state = 0;
				key_return = KEY_NONE;
			}
			break;
	}

	return key_return;
}
/*
 * Name                : process
 * Description         : ---
 * Author              : zh.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 02/27/2020
 * 
 * create.
 * --------------------
 */
static void process(void)
{
	static int cnt = 0;
	
	//按键处理
	if(key.value != key.bak_value){
		switch(key.value){
			case ARM_KEY:
				if(cnt ++ % 2){
					LED_ON;
				}else{
					LED_OFF;
				}
				break;
		}
		key.bak_value = key.value;
	}
}
