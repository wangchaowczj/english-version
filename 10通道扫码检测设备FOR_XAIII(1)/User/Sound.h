/**
  *************************************************************************************
  * @file    Sound.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为Sound.c的头文件
  *************************************************************************************
  */
#ifndef _SOUND_H_
#define _SOUND_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Sound
  *@brief 声音模块
  *@{
  */
/**
 *@defgroup Sound_PinDefines
 *@{ 
 */
//声音控制
#define SOUND_PORT       				 GPIOB
#define SOUND_PORT_RCC    				 RCC_AHB1Periph_GPIOB
#define SOUND_PIN         				 GPIO_Pin_9
#define OPEN_SOUND_SW()                  GPIO_SetBits(SOUND_PORT, SOUND_PIN)
#define CLOSE_SOUND_SW()                 GPIO_ResetBits(SOUND_PORT, SOUND_PIN)

/**
 *@} Sound_PinDefines
 */
/**
 *@} Sound
 */
/**
 *@} PowerBoard
 */
extern void SoundConfig(void);
extern void Sound(u8 num);

#endif
