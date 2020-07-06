/**
  *************************************************************************************
  * @file    Delay.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为Delay.c的头文件
  *************************************************************************************
  */
#ifndef _DELAY_H_
#define _DELAY_H_


extern void SysTickConfig(void);
extern void System168MDelay1us(void);
void System168MDelay10us(u8 num);
extern void System168MDelay100us(u8 num);
extern void System168MDelay1ms(u8 num);
#endif
