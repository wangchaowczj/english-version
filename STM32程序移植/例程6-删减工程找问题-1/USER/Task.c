#include "includes.h"
__align(8) APP_PRIVATE_STK PrivateStk;

OP_DATA Op_Data;
bool EnableMaster = 0; 			// Master/Slave selection
u8 StautSwFlag = 1;//û���������
//extern u32 RS485_S_timecount;
//extern u32 RS4852_S_timecount;

//extern TRANSCEIVER UartTransceiver1;
//extern TRANSCEIVER UartTransceiver2;
/**
  *************************************************************************************
  * @brief  ��������
  * @param[in]  prio���������ȼ� 
  * @param[in]  p_arg���������
  * @retval ��� 
  * @author 
  * @date   2016��7��5��
  * @note   ��
  *************************************************************************************  
  */
INT8U TaskCreate(INT8U prio, void* p_arg)
{
	INT8U Result = OS_ERR_PRIO_INVALID;
	
	switch(prio)
	{
		case APP_STARTUP_TASK_PRIO:
			Result = OSTaskCreateExt(AppStartUpTask, (void*)p_arg,
						&PrivateStk.Startup[APP_STARTUP_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.Startup[0], APP_STARTUP_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;	
//		case APP_RS485_TASK_PRIO:
//			Result = OSTaskCreateExt(AppRS485Task, (void*)p_arg,
//						&PrivateStk.RS485[APP_RS485_TASK_STK_SIZE-1],prio,
//						prio,&PrivateStk.RS485[0], APP_RS485_TASK_STK_SIZE,
//						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
//			break;
		case APP_OLED_TASK_PRIO:
			Result = OSTaskCreateExt(AppOLEDTask, (void*)p_arg,
						&PrivateStk.OLED[APP_OLED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.OLED[0], APP_OLED_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;
        case APP_LED_TASK_PRIO:
			Result = OSTaskCreateExt(AppLEDTask, (void*)p_arg,
						&PrivateStk.LED[APP_LED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED[0], APP_LED_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;	
//		case APP_RS485_DIR_TASK_PRIO:
//			Result = OSTaskCreateExt(AppRS485DirTask, (void*)p_arg,
//						&PrivateStk.RS485Dir[APP_RS485_DIR_TASK_STK_SIZE-1],prio,
//						prio,&PrivateStk.RS485Dir[0], APP_RS485_DIR_TASK_STK_SIZE,
//						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
//			break;	
				
		default:
			break;
	}
	return Result;	
}

/**
  *************************************************************************************
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2018��3��1��
  * @note   ��
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{   
	SystemGpioConfig();
//	TaskCreate(APP_RS485_TASK_PRIO,(void*)0); 
	TaskCreate(APP_LED_TASK_PRIO,(void*)0); 
//	TaskCreate(APP_RS485_DIR_TASK_PRIO,(void*)0);
    TaskCreate(APP_OLED_TASK_PRIO,(void*)0);    
	OSTaskDel(OS_PRIO_SELF);
}
/**
  *************************************************************************************
  * @brief  RS485ͨ������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
//void AppRS485Task(void* p_arg)
//{	
//	u16 Bytes, Index,Bytes2, Index2;
//	u8 Buffer[512],Buffer2[512];
//	while(1)
//	{
//		//����1���治Ϊ��ͨ���ӻ����ͳ�ȥ				
//		while(GetTransceiverBytesCount(&UartTransceiver1) != 0)
//		{//
//			LED1_ON();
//			Bytes = GetTransceiverBytesCount(&UartTransceiver1);
//			Index = 0;
//			while(Index < Bytes)
//			{
//				GetTransceiverByte(&UartTransceiver1, &Buffer[Index++]);
//			}
//			Uart2SendData(Buffer, Bytes);
//			LED1_OFF();
//		}		
//		//�ӻ�2���治Ϊ��ͨ���������ͳ�ȥ	
//		while((GetTransceiverBytesCount(&UartTransceiver2) != 0))	
//		{//
//			LED1_ON();
//			Bytes2 = GetTransceiverBytesCount(&UartTransceiver2);				
//			Index2 = 0;
//			while(Index2 < Bytes2)
//			{
//				GetTransceiverByte(&UartTransceiver2, &Buffer2[Index2++]);
//			}					
//			UartSendData(Buffer2, Bytes2);
//			LED1_OFF();
//		}
//		OSTimeDlyHMSM(0,0,0,2);
//	}
//}
/**
  *************************************************************************************
  * @brief  LED����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void AppLEDTask(void* p_arg)
{	
	while(1)
	{
		LED1_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED1_OFF();
		OSTimeDlyHMSM(0,0,0,400);	
	}
}

/**
  *************************************************************************************
  * @brief  LED����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
//void AppRS485DirTask(void* p_arg)
//{	
//	while(1)
//	{
//		if(RS4852_S_timecount>5000)//�ӻ��жϷ���
//		{
//			SET_RS4852_UART_R();
//			if(READ_RS4852_UART_S()!=0)
//			{
//				OSTimeDlyHMSM(0,0,0,20);
//				if(READ_RS4852_UART_S()!=0)//ȷ����ƽ
//				{
//					RS4852_S_timecount=0;
//					SET_RS4852_SW_H();//�ӻ����Ž���

//				}
//			}
//			else
//			{
//				OSTimeDlyHMSM(0,0,0,20);
//				if(READ_RS4852_UART_S()==0)//ȷ����ƽ
//				{
//					RS4852_S_timecount=0;
//					SET_RS4852_SW_H();//�ӻ����Ž���
//				}
//			}

//		}
////		if((RS485_S_timecount>1000) && (GPIO_ReadOutputDataBit(RS485_UART_DIR_PORT,RS485_UART_DIR_PIN) == 0))//��������ӻ��жϷ���
//		if(RS485_S_timecount>1000)
//		{
//			SET_RS485_UART_R();
////			RS485_S_timecount=1001;
//		}
//		else if(RS485_S_timecount==0)
//		{
//			SET_RS485_UART_R();
////			SET_RS4852_UART_R();
//		}
////		else if((GPIO_ReadOutputDataBit(RS485_UART_DIR_PORT,RS485_UART_DIR_PIN) != 0) && (RS485_S_timecount<1))
////		{
////			SET_RS485_UART_R();
////		}			
//		OSTimeDlyHMSM(0,0,0,100);
//	}
//}


void AppOLEDTask(void* p_arg)
{
    I2C_GPIO_Config();    
    LcdInit();  
    while(1)
    {
     LcdShowStr2Center("��̵��� ","V1.0");
    }
}
