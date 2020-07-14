/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "includes.h"

static u8 Timer2StartFlag = 0;


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */ 
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	if(OpData.OutputWareFlag)//标定，输出500Hz方波
	{
		DET_TX1_PORT->ODR ^=DET_TX1_PIN;
	}
	OSIntExit();	
}
void RTCAlarm_IRQHandler(void)
{
	OSIntEnter();
//	RTC_WaitForSynchro();
//	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
//	{
////		 EXTI_ClearITPendingBit(EXTI_Line17);
//////		 RTC_WaitForLastTask();
////		 if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
////			{
////				// 清除唤醒标志
////				PWR_ClearFlag(PWR_FLAG_WU);
//////				RTC_WaitForLastTask();
////			}
////		RTC_ClearITPendingBit(RTC_IT_ALR);
////		RTC_WaitForLastTask();
////		RTC_Enter_StandbyMode(standbytime);//standbytime秒后唤醒
//	}
	OSIntExit();
}
void RTC_WKUP_IRQHandler(void)
{
	OSIntEnter();
	while(RTC_GetFlagStatus(RTC_FLAG_WUTF)!=RESET)
    {
        RTC_ClearFlag(RTC_FLAG_WUTF);
    }
    EXTI_ClearITPendingBit(EXTI_Line22);
	OpenAllPortClock();
	POWER_OFF(); 
	OSIntExit();
}

/**
  * @brief  This function handles TIM2_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	OSIntEnter();
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )  //TIMx->DIER	TIMx->SR
	{		
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); //清中断标志位
		switch(DetProtocol.SendBuffer.State)
		{
			case DET_SEND://处于发送命令状态
				SET_LIN_REVERSE();
				if(DET_FIXED_PULSE == DetProtocol.Mode)
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index&0x01]);
						DetProtocol.SendBuffer.Index++;
					}				
					else
					{
						DetProtocol.SendBuffer.State = DET_END;//不接收数据
						TIM_Cmd(TIM2, DISABLE);  //关闭时钟
					}				
				}
				else
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);
					}				
					else if(DetProtocol.RecData.ExpentCount != 0)
					{//需要接收数据,将在400us后再产生一次中断
						TIM_SetAutoreload(TIM2, 0xFFFFFFFFUL);
						DetProtocol.SendBuffer.State = DET_DELAY_REC;
					}
					else
					{
						DetProtocol.SendBuffer.State = DET_END;//不接收数据
						TIM_Cmd(TIM2, DISABLE);  //关闭时钟
					}				
				}
				break;
			case DET_DELAY_REC://处于延时等待接收状态
				DetProtocol.SendBuffer.State = DET_WAITING_REC;//开始接收
				
				TIM_Cmd(TIM2, DISABLE);  //关闭时钟
				TIM_SetCounter(TIM2, 0);
				Timer2StartFlag = 0;
				//打开外部中断
				ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, ENABLE);				
				break;
			case DET_WAITING_REC:
			default:
				DetProtocol.SendBuffer.State = DET_TIMEOUT;
				TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
				TIM_Cmd(TIM2, DISABLE);  //关闭时钟
				TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清除溢出中断标志
				NVIC_DisableIRQ(TIM2_IRQn);//失能外部中断

				ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, DISABLE);				
				break;
		}
	}
	OSIntExit();
}
/**
  * @brief  This function handles TIM3_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{	
	OSIntEnter();
		
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);						
	}
	OSIntExit();
}
/**
  * @brief  This function handles TIM4_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{	
	OSIntEnter();

	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
		TIM_Cmd(TIM4, DISABLE);	//需要时再启动
			
	}
	OSIntExit();
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None 用于系统标定
  */			   
void TIM5_IRQHandler(void)
{	
	OSIntEnter();
	
	if(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);	
	}

	OSIntExit();
}

void EXTI0_IRQHandler(void)
{
//	u8 i;
	
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		CH374InterruptCallBack();
	}	
	
//	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
//	{
//		EXTI_ClearFlag(EXTI_Line0);
//		for(i=0; i<100; i++)
//		{
//			if(GET_PW_INT_STATE() != 0)//排除静电产生的干扰，避免意外关机
//			{//低电平表示，中断信号有效
//				break;
//			}
//			SystemDelay100us(1);		
//		}
//		if(i>=100)
//		{
//			SetPowerOffKeyState();		
//		}		
//	}	
	OSIntExit();	
}

void EXTI1_IRQHandler(void)
{	
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
        MCU_IO_IN1_Flag = 1;
	}	
	
	OSIntExit();	
}
void EXTI2_IRQHandler(void)
{	
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
	}	
	
	OSIntExit();	
}
void EXTI9_5_IRQHandler(void)
{
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{//睡眠唤醒中断PC5
        EXTI_ClearFlag(EXTI_Line5);
	}	
	OSIntExit();
}
/**
  * @brief  This function handles EXTI9_5 interrupt request.
  * @param  None
  * @retval None  
  */
void EXTI15_10_IRQHandler(void)
{	
	OSIntEnter();	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
        EXTI_ClearFlag(EXTI_Line15);
		
		TIM_Cmd(TIM2, ENABLE);
		if(Timer2StartFlag == 0)
		{//启动定时器
			TIM_Cmd(TIM2, ENABLE);
			Timer2StartFlag = 1;
		}
		else
		{
			if(DetProtocol.RecBuffer.Count < DET_REC_BUFFER_MAX)
			{
				DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = TIM_GetCounter(TIM2);
				TIM_SetCounter(TIM2, 0);
			}		
		}
	}

	OSIntExit();	
}


/**
  * @brief  485通信
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	u8 temp;
	
	OSIntEnter();
    if (USART_GetITStatus(RS485_COMM, USART_IT_RXNE) != RESET)
    {				
        temp = USART_ReceiveData(RS485_COMM);
		if(RS485Data.status == RECIEVING)
		{
			if((OSTimeGet() - RS485RecTimeLatest > 5) || (RS485Data.bytenum >= BUF_MAX))
			{
				RS485Data.bytenum = 0;
			}
            RS485Data.uch_buf[RS485Data.bytenum++] = temp;						
		}
		RS485RecTimeLatest = OSTimeGet();
    }
	OSIntExit();	
}
/**
  * @brief  232通信
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
	u8 temp;
	
	OSIntEnter();
    if (USART_GetITStatus(RS232_COMM, USART_IT_RXNE) != RESET)
    {				
        temp = USART_ReceiveData(RS232_COMM);
		if(RS232Data.status == RECIEVING)
		{
			if((OSTimeGet() - RS232RecTimeLatest > 5) || (RS232Data.bytenum >= BUF_MAX))
			{
				RS232Data.bytenum = 0;
			}
            RS232Data.uch_buf[RS232Data.bytenum++] = temp;						
		}
		RS232RecTimeLatest = OSTimeGet();
    }
	OSIntExit();	
}
/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler (void)
{
  /* Process All SDIO Interrupt Sources */
//	OSIntEnter();  
//	SD_ProcessIRQSrc();
//	OSIntExit();  
}

/**
  * @brief  This function handles DMA2 Stream3 or DMA2 Stream6 global interrupts
  *         requests.
  * @param  None
  * @retval None
  */
void SD_SDIO_DMA_IRQHANDLER (void)
{
  /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
//	OSIntEnter();  
//	SD_ProcessDMAIRQ();
//	OSIntExit();    
}
void OTG_FS_WKUP_IRQHandler(void)
{
	OSIntEnter(); 
//	if(USB_OTG_Core_dev.cfg.low_power)
//	{
//		/* Reset SLEEPDEEP and SLEEPONEXIT bits */
//		SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

//		/* After wake-up from sleep mode, reconfigure the system clock */
//		SystemInit();
//		USB_OTG_UngateClock(&USB_OTG_Core_dev);
//	}
//	EXTI_ClearITPendingBit(EXTI_Line18);
  	OSIntExit();
}

void OTG_FS_IRQHandler(void)
{
	OSIntEnter(); 
//	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) /* ensure that we are in device mode */
//	{
//		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
//	}
//	else
//	{
//		USBD_OTG_ISR_Handler (&USB_OTG_Core_dev);
//	}
  	OSIntExit();  
}
void OTG_HS_WKUP_IRQHandler(void)
{
	OSIntEnter();
//	if(USB_OTG_Core_dev.cfg.low_power)
//	{
//		/* Reset SLEEPDEEP and SLEEPONEXIT bits */
//		SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

//		/* After wake-up from sleep mode, reconfigure the system clock */
//		SystemInit();
//		USB_OTG_UngateClock(&USB_OTG_Core_dev);
//	}
//	EXTI_ClearITPendingBit(EXTI_Line18);
	OSIntExit(); 
}

void OTG_HS_IRQHandler(void)
{  
	OSIntEnter(); 
//	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) /* ensure that we are in device mode */
//	{
//		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
//	}
//	else
//	{
//		USBD_OTG_ISR_Handler (&USB_OTG_Core_dev);
//	}
	OSIntExit();
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
