/*****************************************************************
*版权所有 2012，泰坦科技股份有限公司
*
*文件名： led.c
*内容摘要：完成对LED对应IO的初始化设置;
*其他说明：无
*           
*当前版本：v1.0
*作    者：梁尹宣
*完成日期：2012年8月3日
******************************************************************/

#include "Include.h"

/*****************************************************************
*函数名称：	LedInit
*功能描述：	完成对LED对应IO的初始化设置;
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/
void LedInit(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能外设时钟
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_SetBits(GPIOC, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9);	      
       
}
