#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP

#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)//读取按键s1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键s2
#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键s3 
#define KEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键s4 


#define KEY_S4 	4
#define KEY_S3	3
#define KEY_S2	2
#define KEY_S1	1

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数	//typedef uint8_t u8				    
#endif
