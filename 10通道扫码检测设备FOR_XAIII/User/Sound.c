/**
  *************************************************************************************
  * @file    Sound.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ����ģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Sound
  *@brief ����ģ��
  *@{
  */

/**@defgroup Sound_Functions
  *@brief ��������
  *@{
  */ 
/**
  *************************************************************************************
  * @brief  ��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void SoundConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SOUND_PORT_RCC , ENABLE);	//GPIOʱ������

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
  * @brief  ��������
  * @param  num: ��������
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   �������500ms
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
