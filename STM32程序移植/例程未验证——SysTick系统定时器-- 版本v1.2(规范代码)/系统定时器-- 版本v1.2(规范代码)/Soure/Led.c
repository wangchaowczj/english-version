/*****************************************************************
*��Ȩ���� 2012��̩̹�Ƽ��ɷ����޹�˾
*
*�ļ����� led.c
*����ժҪ����ɶ�LED��ӦIO�ĳ�ʼ������;
*����˵������
*           
*��ǰ�汾��v1.0
*��    �ߣ�������
*������ڣ�2012��8��3��
******************************************************************/

#include "Include.h"

/*****************************************************************
*�������ƣ�	LedInit
*����������	��ɶ�LED��ӦIO�ĳ�ʼ������;
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
void LedInit(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ������ʱ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_SetBits(GPIOC, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9);	      
       
}
