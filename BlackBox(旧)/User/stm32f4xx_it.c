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

#include "usb_bsp.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbd_hid_core.h"
#include "usb_dcd_int.h"
#include "usb_hcd_int.h"
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;
extern __ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;

u32 BlueDataReceiveTime = 0;
u32 RS485DataReceiveTime = 0;

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
	OSIntExit();
}

/**
  * @brief  This function handles DMA2_Stream7_IRQHandler interrupt request.
  * @param  None
  * @retval None  
  */
void TIM8_UP_TIM13_IRQHandler(void)
{

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
		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update); //清中断标志位
        switch(DetProtocol.Mode)
        {
            case DET_UNKNOW:
                TIM_SetCounter(TIM2, 0);
                TIM_SetAutoreload(TIM2, 0xFFFFFFFF);
                break;
            case DET_CMD:
                TIM_SetCounter(TIM2, 0);
                TIM_SetAutoreload(TIM2, 0xFFFFFFFF);
                DetProtocol.Mode = DET_UNKNOW;
                if ((DetProtocol.Cmd.Bit == 0) && (DetProtocol.Cmd.Count > 0))
                {//已经收到指令
                    DET_DATA_STRUCT *DetDataStruct;
                    
                    DetDataStruct = GetFreeDetDataStruct();//申请一个内存块
                    if(DetDataStruct != NULL)
                    {
                        DetDataStruct->Type = DET_CMD_DATA;
                        DetDataStruct->Data.Cmd.Count = DetProtocol.Cmd.Count;
                        memcpy(&DetDataStruct->Data.Cmd.Data[0], &DetProtocol.Cmd.Data[0], DetProtocol.Cmd.Count);
                        
                        OSQPost(MsgSem.MsgQeue, (void*)DetDataStruct);//首地址放入消息队列
                    }

                    if (DetProtocol.Cmd.Data[0] == 0x42)
                    {
                        StartDetAck();
                    }
                }
                break;
            case DET_BLAST:
                TIM_SetCounter(TIM2, 0);
                TIM_SetAutoreload(TIM2, 0xFFFFFFFF);
                DetProtocol.Mode = DET_UNKNOW;
                break;
            default:
                break;
        }
	}
	OSIntExit();
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */			   
void TIM5_IRQHandler(void)
{	
	OSIntEnter();
	if ( TIM_GetITStatus(TIM5 , TIM_IT_Update) != RESET )  //TIMx->DIER	TIMx->SR
	{		
		TIM_ClearITPendingBit(TIM5 , TIM_IT_Update); //清中断标志位

        SET_LED1_RED_PIN_TOGGLE();
        
        TIM_SetCounter(TIM5, 0xFFFFFFFFUL-TNUS(200));
        if(DetProtocol.Ack.Index < DetProtocol.Ack.Count)
        {
            u8 Bit = 0;

            if(DetProtocol.Ack.Bit <= 7)
            {
                if(READ_BIT(DetProtocol.Ack.Data[DetProtocol.Ack.Index], 1<<DetProtocol.Ack.Bit))
                {
                    Bit = 1;
                }
                DetProtocol.Ack.Bit++;
            }
            else if(DetProtocol.Ack.Bit == 8)
            {//奇校验
                Bit = !GetEventBit(DetProtocol.Ack.Data[DetProtocol.Ack.Index]);
                DetProtocol.Ack.Bit++;
            }
            else if(DetProtocol.Ack.Bit >= 9)
            {//偶校验
                Bit = GetEventBit(DetProtocol.Ack.Data[DetProtocol.Ack.Index]);
                DetProtocol.Ack.Bit = 0;
                DetProtocol.Ack.Index++;
            }

            if(Bit)
            {
                DET_ACK_HIGH();
            }
            else
            {
                DET_ACK_LOW();
            }
        }				
        else
        {
            DET_ACK_INIT();				
            TIM_Cmd(TIM5, DISABLE);  //关闭时钟											
        }
	}
	OSIntExit();
}

/**
  * @brief  This function handles EXTI9_5 interrupt request.
  * @param  None
  * @retval None  
  */
void EXTI9_5_IRQHandler(void)
{
	u32 temp;

	OSIntEnter();

	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{//雷管命令接收
		EXTI_ClearITPendingBit(EXTI_Line9);
        
        temp = TIM2->CNT;
        
        TIM_SetCounter(TIM2, 0);
        if(DetProtocol.Mode == DET_UNKNOW)
        {
            if ((temp > TNUS(250)) && (temp < TNUS(350)))
            {
                DetProtocol.Mode = DET_CMD;
                TIM_SetCounter(TIM2, 0);
                TIM_SetAutoreload(TIM2, TNUS(200));//设置定时器超时时间为200us
            }
            else if ((temp > TNUS(27500)) && (temp < TNUS(28500)))
            {
                DetProtocol.Mode = DET_BLAST;
                DetProtocol.Rec.Timer[DetProtocol.Rec.Count++] = temp;
            }
            else
            {
                DetRecStatusInit();
            }
        }
        else if(DetProtocol.Mode == DET_CMD)
        {
            DetProtocol.Rec.Timer[DetProtocol.Rec.Count++] = temp;
            if(DetProtocol.Rec.Count >= 2)
            {
                u32 cycle = DetProtocol.Rec.Timer[0]+DetProtocol.Rec.Timer[1];
                
                DetProtocol.Rec.Count = 0;
                if ((cycle > TNUS(120)) && (cycle < TNUS(160)))//140us
                {
                    u8 bit = 0xFF;

                    if ((DetProtocol.Rec.Timer[0] > TNUS(20)) && (DetProtocol.Rec.Timer[0] < TNUS(60)))
                    {//40us+100us对应0
                        bit = 0;
                    }
                    else if ((DetProtocol.Rec.Timer[0] > TNUS(80)) && (DetProtocol.Rec.Timer[0] < TNUS(120)))
                    {//100us+40us对应1
                        bit = 1;
                    }
                    if ((bit == 0) || (bit == 1))
                    {
                        if(DetProtocol.Cmd.Bit <= 7)
                        {
                            if(bit)
                            {
                                SET_BIT(DetProtocol.Cmd.Data[DetProtocol.Cmd.Count], 1<<DetProtocol.Cmd.Bit);
                            }
                            else
                            {
                                CLEAR_BIT(DetProtocol.Cmd.Data[DetProtocol.Cmd.Count], 1<<DetProtocol.Cmd.Bit);
                            }
                            DetProtocol.Cmd.Bit++; 
                        }
                        else if(GetEventBit(DetProtocol.Cmd.Data[DetProtocol.Cmd.Count]) == bit)
                        {//偶校验正常
                            DetProtocol.Cmd.Bit = 0;
                            DetProtocol.Cmd.Count++;                           
                        }
                        else
                        {
                            DetRecStatusInit();
                        }
                    }
                    else
                    {
                        DetRecStatusInit();
                    }
                }
                else
                {
                    DetRecStatusInit();
                }
            }
        }
        else if(DetProtocol.Mode == DET_BLAST)
        {
            if ((DetProtocol.Rec.Count == 1) && ((temp > TNUS(13500)) && (temp < TNUS(14500))))
            {
                DetProtocol.Rec.Timer[DetProtocol.Rec.Count++] = temp;
            }
            else if ((DetProtocol.Rec.Count == 2) && ((temp > TNUS(27500)) && (temp < TNUS(28500))))
            {
                DetProtocol.Rec.Timer[DetProtocol.Rec.Count++] = temp;
            }
            else if (DetProtocol.Rec.Count == 3)
            {//已经收到一个完整的起爆信号
                DET_DATA_STRUCT *DetDataStruct;
                
                DetProtocol.Rec.Timer[DetProtocol.Rec.Count++] = temp;
                
                DetDataStruct = GetFreeDetDataStruct();//申请一个内存块
                if(DetDataStruct != NULL)
                {
                    u8 i;
                    
                    DetDataStruct->Type = DET_BLAST_CMD_DATA;
                    DetDataStruct->Data.BlastCmd.Count = DetProtocol.Rec.Count;
                    
                    for(i=0;i<DetDataStruct->Data.BlastCmd.Count;i++)
                    {
                        DetDataStruct->Data.BlastCmd.Data[i] = DetProtocol.Rec.Timer[i];
                    }
                    
                    OSQPost(MsgSem.MsgQeue, (void*)DetDataStruct);//首地址放入消息队列
                }
                DetRecStatusInit();                
            }
            else
            {
                DetRecStatusInit();
            }
        }        
	}
	OSIntExit();
}

void EXTI15_10_IRQHandler(void)
{
    OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line15);		
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
	if(USB_OTG_Core_dev.cfg.low_power)
	{
		/* Reset SLEEPDEEP and SLEEPONEXIT bits */
		SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

		/* After wake-up from sleep mode, reconfigure the system clock */
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_Core_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
  	OSIntExit();
}

void OTG_FS_IRQHandler(void)
{
	OSIntEnter(); 
	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) /* ensure that we are in device mode */
	{
		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
	}
	else
	{
		USBD_OTG_ISR_Handler (&USB_OTG_Core_dev);
	}
  	OSIntExit();  
}
void OTG_HS_WKUP_IRQHandler(void)
{
	OSIntEnter();
	if(USB_OTG_Core_dev.cfg.low_power)
	{
		/* Reset SLEEPDEEP and SLEEPONEXIT bits */
		SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

		/* After wake-up from sleep mode, reconfigure the system clock */
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_Core_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
	OSIntExit(); 
}

void OTG_HS_IRQHandler(void)
{  
	OSIntEnter(); 
	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) /* ensure that we are in device mode */
	{
		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
	}
	else
	{
		USBD_OTG_ISR_Handler (&USB_OTG_Core_dev);
	}
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
