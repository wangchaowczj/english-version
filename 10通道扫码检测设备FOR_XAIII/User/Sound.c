/**
  *************************************************************************************
  * @file    Sound.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   声音模块源文件
  *************************************************************************************
  */
#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Sound
  *@brief 声音模块
  *@{
  */

/**@defgroup Sound_Functions
  *@brief 函数定义
  *@{
  */ 
/**
  *************************************************************************************
  * @brief  声音配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
void SoundConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SOUND_PORT_RCC , ENABLE);	//GPIO时钟配置

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = SOUND_PIN;
	GPIO_Init(SOUND_PORT, &GPIO_InitStructure);
	
	CLOSE_SOUND_SW();	
}

/**
  *************************************************************************************
  * @brief  发出声音
  * @param  num: 响声次数
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   响声间隔500ms
  *************************************************************************************  
  */
void Sound(u8 num)
{
	u8 i;
	
	for(i=0; i<num; i++)
	{
		OPEN_SOUND_SW();
		OSTimeDlyHMSM(0,0,0,300);
		CLOSE_SOUND_SW();
		OSTimeDlyHMSM(0,0,0,100);		
	}
}
/**
 *@} Sound_Functions
 */
/**
 *@} Sound
 */
/**
 *@} PowerBoard
 */
