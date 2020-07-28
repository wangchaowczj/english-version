#include "systemInit.h"
extern vu32 TimingDelay;

/******************************************************************************* SysTick   ��ʼ������
* Function Name  : SysTick_Config 
* Description    : Configures SysTick 
* Input          : None 
* Output         : None 
* Return         : None 
*******************************************************************************/  
//SysTick����   
/*void SysTick_Init(void)  
{  
    /* Disable SysTick Counter */  
//    SysTick_CounterCmd(SysTick_Counter_Disable);  
    
    /* Disable the SysTick Interrupt */  
  //  SysTick_ITConfig(DISABLE);  
    
    /* Configure HCLK clock as SysTick clock source */  
  //  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  
    /* SysTick interrupt each 1000 Hz with HCLK equal to 72MHz */  
 //   SysTick_SetReload(9);  
    /* Enable the SysTick Interrupt */  
 //   SysTick_ITConfig(ENABLE);  
//}  
  
/******************************************************************************* SysTickDelayMs  �ӳ�һ΢�뺯��
* Function Name  : SysTickDelayUs 
* Description    : Inserts a delay time. 
* Input          : nTime: specifies the delay time length, in milliseconds. 
* Output         : None 
* Return         : None 
*******************************************************************************/  
void SysTickDelayUs(u32 nTime)  
{  
  /* Enable the SysTick Counter */  
  SysTick_CounterCmd(SysTick_Counter_Enable);  
    
  TimingDelay = nTime;  
  while(TimingDelay != 0);  
  /* Disable SysTick Counter */  
  SysTick_CounterCmd(SysTick_Counter_Disable);  
  /* Clear SysTick Counter */  
  SysTick_CounterCmd(SysTick_Counter_Clear);  
}  
  
/******************************************************************************* TimingDelayMs_Decrement    �жϵ��ú���
* Function Name  : TimingDelayMs_Decrement 
* Description    : Decrements the TimingDelay variable. 
* Input          : None 
* Output         : TimingDelay 
* Return         : None 
*******************************************************************************/  
extern void TimingDelay_Decrement(void)  
{  
  if (TimingDelay != 0x00)  
  {   
    TimingDelay--;  
  }  
}   
