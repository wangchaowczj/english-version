#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP

#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)//��ȡ����s1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����s2
#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����s3 
#define KEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//��ȡ����s4 


#define KEY_S4 	4
#define KEY_S3	3
#define KEY_S2	2
#define KEY_S1	1

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��	//typedef uint8_t u8				    
#endif
