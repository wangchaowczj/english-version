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
	RS485DirCountDecrement();
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
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); //���жϱ�־λ
		switch(DetProtocol.SendBuffer.State)
		{
			case DET_SEND://���ڷ���״̬
				if((DET_CMD == DetProtocol.Mode)||(DET_FIXED_PULSE == DetProtocol.Mode))
				{
					DET_COM_SEND_TOGGLE();
				}
				else
				{
					BLAST_SIGNAL_TOGGLE();	
				}
				if(DET_FIXED_PULSE == DetProtocol.Mode)
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index&0x01]);
						DetProtocol.SendBuffer.Index++;
						if((DetProtocol.SendBuffer.Index&0xFF) == 0xFF)
						{
							Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
						}						
					}				
					else
					{
						DET_COM_SEND_PIN_INIT();						

						DetProtocol.SendBuffer.State = DET_END;//����������
						TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��																
					}					
				}
				else
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);
                        if((DetProtocol.SendBuffer.Index == 6) &&(DET_BLAST == DetProtocol.Mode))
                        {
                            if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && Op_Data.DelaySampleCfg && Op_Data.DelaySampleFlag)
                            {
                                RS485_COMM_TX_PIN_TOGGLE();//����ӵ�LED��
                            }
                        }
					}				
					else
					{
						if(DET_CMD == DetProtocol.Mode)
						{
							DET_COM_SEND_PIN_INIT();						
						}
						else
						{
							BLAST_SIGNAL_PIN_INIT();	
						}				
						if(DetProtocol.RecData.ExpentCount != 0)
						{//��Ҫ��������					
							TIM_SetAutoreload(TIM2, 0xFFFFFFFFUL);
							DetProtocol.SendBuffer.State = DET_DELAY_REC;
							if((DET_CMD == DetProtocol.Mode)||(DET_FIXED_PULSE == DetProtocol.Mode))
							{//��5V״̬�½�������
								SET_CMD_RECEIVE_STATE();//����Ϊ����״̬
								ADJUST_G_RESET();//�ñȽ�����λһ��
							}
						}
						else
						{
							DetProtocol.SendBuffer.State = DET_END;//����������
							TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��						
						}										
					}				
				}
				break;
			case DET_DELAY_REC://������ʱ�ȴ�����״̬
				DetProtocol.SendBuffer.State = DET_WAITING_REC;//��ʼ����
				
//				TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
				TIM_SetCounter(TIM2, 0);
				//���ⲿ�ж�
				ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, ENABLE);
				ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, ENABLE);			
				break;
			case DET_WAITING_REC:
			default:
				DetProtocol.SendBuffer.State = DET_TIMEOUT;
				
				TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
				TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
				TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
				NVIC_DisableIRQ(TIM2_IRQn);
				
				//ʧ���ⲿ�ж�
				ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, DISABLE);
				ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, DISABLE);			
				break;
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
	
	temp = TIM2->CNT;
	
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{//FAIL�½���
		EXTI_ClearITPendingBit(EXTI_Line6);

		if(DetProtocol.Fall.Count > 0) 			
		{//����˲�
			if((temp-DetProtocol.Fall.Timer[DetProtocol.Fall.Count-1])>22000)//�����½���֮�����ٶ�Ӧ2λ���ݣ���Ҫ��С��4/5(�����Ƶ��120K����),��ʱ������ֵ��Ӧ11.9ns
			{
				if((DetProtocol.Rise.Count > 0) && 
					(temp > DetProtocol.Rise.Timer[DetProtocol.Rise.Count-1]) && 
					((temp-DetProtocol.Rise.Timer[DetProtocol.Rise.Count-1])>9000))//�����غ��½���֮��,���ٶ�Ӧ1λ���ݣ���Ҫ��С��4/5(�����Ƶ��120K����)
				{
					if(DetProtocol.Rise.Count == (DetProtocol.Fall.Count+1))//�����غ��½���Ӧ���ǽ������
					{
						DetProtocol.Fall.Timer[DetProtocol.Fall.Count++] = temp;					
					}
				}
			}
		}
		else
		{
			//�����غ��½���Ӧ���ǽ������
			if((1 == DetProtocol.Rise.Count) && ((temp-DetProtocol.Rise.Timer[0])>9000))
			{
				DetProtocol.Fall.Timer[DetProtocol.Fall.Count++] = temp;				
			}
		}
		
		if(DetProtocol.Fall.Count >= DET_REC_BUFFER_MAX)
		{
			DetProtocol.SendBuffer.State = DET_END;
			
			TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
			TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
			TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
			NVIC_DisableIRQ(TIM2_IRQn);
			
			//ʧ���ⲿ�ж�
			ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, DISABLE);
			ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, DISABLE);
		}
        if(DetProtocol.RealFall.Count < DET_REC_BUFFER_MAX)
        {
            DetProtocol.RealFall.Timer[DetProtocol.RealFall.Count++] = temp;
        }		
	}
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{ //RISE�½���
		EXTI_ClearITPendingBit(EXTI_Line7);		

		if(DetProtocol.Rise.Count > 0) 
		{//����˲�
            if (DetProtocol.Fall.Count == 0)
            {//�ڻ�û�н��յ�FALL�ź�ǰ������������յ����RISE�źţ�ֻ�������һ��
                DetProtocol.Rise.Count = 0;
                DetProtocol.Rise.Timer[DetProtocol.Rise.Count++] = temp;
            }
			else if((temp-DetProtocol.Rise.Timer[DetProtocol.Rise.Count-1])>22000)//����������֮�����ٶ�Ӧ2λ���ݣ���Ҫ��С��4/5(�����Ƶ��120K����),��ʱ������ֵ��Ӧ11.9ns
			{
				if((temp > DetProtocol.Fall.Timer[DetProtocol.Fall.Count-1]) && 
					((temp-DetProtocol.Fall.Timer[DetProtocol.Fall.Count-1])>9000))//�����غ��½���֮��,���ٶ�Ӧ1λ���ݣ���Ҫ��С��4/5(�����Ƶ��120K����)
				{
					if(DetProtocol.Rise.Count == DetProtocol.Fall.Count)//�����غ��½���Ӧ���ǽ������
					{
						DetProtocol.Rise.Timer[DetProtocol.Rise.Count++] = temp;												
					}
				}
			}			
		}
		else
		{
			DetProtocol.Rise.Timer[DetProtocol.Rise.Count++] = temp;
		}
	
		if(DetProtocol.Rise.Count >= DET_REC_BUFFER_MAX)
		{
			DetProtocol.SendBuffer.State = DET_END;
			
			TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
			TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
			TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
			NVIC_DisableIRQ(TIM2_IRQn);
			
			//ʧ���ⲿ�ж�
			ExtiConfig(EXTI_Line6, EXTI_Trigger_Falling, DISABLE);
			ExtiConfig(EXTI_Line7, EXTI_Trigger_Falling, DISABLE);
		}
        if(DetProtocol.RealRise.Count < DET_REC_BUFFER_MAX)
        {
            DetProtocol.RealRise.Timer[DetProtocol.RealRise.Count++] = temp;
        }
	}
//	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line8);
//		ResetRS485DirCount();
//	}	
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
		
		for(temp=0; temp<100; temp++)
		{
			if(GET_PW_INT_STATE() != 0)//�ų���������ĸ��ţ���������ػ�
			{//�͵�ƽ��ʾ���ж��ź���Ч
				break;
			}
			System168MDelay100us(1);		
		}
		if(temp>=100)
		{
			SetPowerOffKeyState();		
		}	
	}
	
	OSIntExit();
}

void EXTI15_10_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line15);		
	}
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */			   
void TIM5_IRQHandler(void)
{	
    OSIntEnter();
    if(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update)==SET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        if(SoundData.Index < SoundData.Count)
        {
            if(SoundData.Data[SoundData.Index].Type == SOUND_SPECIFIC)
            {
                TIM_SetCompare2(TIM4, SoundData.Data[SoundData.Index].Ptr[0]);
            }
            else
            {
                TIM_SetCompare2(TIM4, SoundData.Data[SoundData.Index].Ptr[SoundData.Data[SoundData.Index].Index]); 
            }
            SoundData.Data[SoundData.Index].Index++;
            if(SoundData.Data[SoundData.Index].Index >= SoundData.Data[SoundData.Index].Size)
            {
                SoundData.Index++;
            }
        }
    }

    if(SoundData.Index >= SoundData.Count)
    {  
        TIM_Cmd(TIM4, DISABLE); 
        TIM_Cmd(TIM5, DISABLE); 
        TIM_CtrlPWMOutputs(TIM4, DISABLE);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        CLOSE_SOUND_SW();
    }
    OSIntExit();
}

/**
  * @brief  485ͨ��
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	u8 temp;
	
	OSIntEnter();
	if(USART_GetITStatus(RS485_COMM, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(RS485_COMM);
		if(RS485Data.status != RECIEVED)//����Ѿ����յ����ݰ�ͷ��β��ȴ�������ɺ��ټ�������
		{
        	if((OSTimeGet() - RS485DataReceiveTime) > 200)//��ʱ��û���յ����ݣ������֮ǰ�յ�������,����������Գɹ�Ч��
			{
				RS485Data.bytenum = 0;
			}
			
			RS485DataReceiveTime = OSTimeGet();
            
			if(RS485Data.bytenum >= BUF_MAX)
			{
				RS485Data.bytenum = 0;
			}
			
			if((RS485Data.bytenum != 0) && (0xC0 != RS485Data.uch_buf[0]))
			{
				RS485Data.bytenum = 0;
			}
			if(0xC0 == temp)
			{//����ͷ����β					
				if(RS485Data.bytenum >= 0x04)//����1�ֽ�����+2�ֽ�CRC16У��+2�ֽ�ͷβ
				{
					RS485Data.status = RECIEVED;//���յ���ͷ��β
					
					if(RS485_SLAVE == GetRS485MasterSlaveState())
					{//RS485Ϊ�ӻ�
						if(OS_ERR_NONE != OSSemPost(MsgSem.RS485))
						{
							RS485Data.bytenum = 0;
							RS485Data.status = RECIEVING;
						}						
					}
				}
				else
				{
					RS485Data.bytenum = 0;
				}
				RS485Data.uch_buf[RS485Data.bytenum++] = temp;
			}
			else if(0x00 != RS485Data.bytenum)
			{//�����������Ѿ����յ�����
				RS485Data.uch_buf[RS485Data.bytenum++] = temp;
			}
			else
			{//�������ݣ���û�н��յ���ͷ
				;
			}
		}
	}
	OSIntExit();
}
/**
  * @brief  ��WIFIͨ��
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{	
	u8 temp;

	OSIntEnter();	
	if(USART_GetITStatus(WIFI_COM, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(WIFI_COM);

		if(MyWifi.RecData.Bytes >= WIFI_BUF_SIZE)
		{
			MyWifi.RecData.Bytes= 0;
		}
		MyWifi.RecData.Buffer[MyWifi.RecData.Bytes++] = temp;
	}
	OSIntExit();	
}
/**
  * @brief  ��ɨ��ģ��ͨ��
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	u8 temp;

	OSIntEnter();	
	if(USART_GetITStatus(SCAN_COM, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(SCAN_COM);

		if(ScanData.Bytes >= SCAN_DATA_MAX_LEN)
		{
			ScanData.Bytes = 0;
		}
		ScanData.Buffer[ScanData.Bytes++] = temp;
	}
	OSIntExit();
}
/**
  * @brief  ��Blueͨ��
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{	
	u8 temp;

	OSIntEnter();	
	if(USART_GetITStatus(BLUE_COM, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(BLUE_COM);

		if(BlueData.bytenum >= BUF_MAX)
		{
			BlueData.bytenum= 0;
		}
		if((EXIT_AT_CMD_MODE_STATUS != GET_BLUE_AT_CMD_MODE_STATUS()) ||
			(BLUE_NOT_CONNECT == GET_BLUE_CONNECT_STATUS()))
		{//AT����
			BlueData.uch_buf[BlueData.bytenum++] = temp;
		}
		else
		{//����͸��
		
			if((OSTimeGet() - BlueDataReceiveTime) > 200)//��ʱ��û���յ����ݣ������֮ǰ�յ�������,����������Գɹ�Ч��
			{
				BlueData.bytenum = 0;
			}
			
			BlueDataReceiveTime = OSTimeGet();
			
			if((BlueData.bytenum != 0) && (0xC0 != BlueData.uch_buf[0]))
			{
				BlueData.bytenum = 0;
			}		
			if(0xC0 == temp)
			{//����ͷ����β					
				if(BlueData.bytenum >= 0x04)//����1�ֽ�����+2�ֽ�CRC16У��+2�ֽ�ͷβ
				{
					BlueData.status = RECIEVED;//���յ���ͷ��β
					
					if(0 == GetBlueCurrentHostSlaveStatus())
					{//��ǰΪ�ӻ�
						if(OS_ERR_NONE != OSSemPost(MsgSem.usb_rcv))
						{
							BlueData.bytenum = 0;
							BlueData.status = RECIEVING;
						}						
					}
				}
				else
				{
					BlueData.bytenum = 0;
				}
				BlueData.uch_buf[BlueData.bytenum++] = temp;
			}
			else if(0x00 != BlueData.bytenum)
			{//�����������Ѿ����յ�����
				BlueData.uch_buf[BlueData.bytenum++] = temp;
			}
			else
			{//�������ݣ���û�н��յ���ͷ
				;
			}			
		}
	}
	OSIntExit();	
}
/**
  * @brief  ��GPSģ��ͨ��
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void)
{
	OSIntEnter();	
	GPSUartCallback();
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
