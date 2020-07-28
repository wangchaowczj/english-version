#include "includes.h"
__align(8) APP_PRIVATE_STK PrivateStk; //����ֽڶ���

OP_DATA Op_Data;
//bool EnableMaster = 0; 			// Master/Slave selection 
//bool���ֽ��͹ؼ���


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
	INT8U Result = OS_ERR_PRIO_INVALID;//ɶ��˼��
	
	switch(prio)
	{
		case APP_STARTUP_TASK_PRIO:
			Result = OSTaskCreateExt(AppStartUpTask, (void*)p_arg,
						&PrivateStk.Startup[APP_STARTUP_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.Startup[0], APP_STARTUP_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;	
		case APP_LED_TASK_PRIO:
			Result = OSTaskCreateExt(AppLEDTask, (void*)p_arg,
						&PrivateStk.LED[APP_LED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED[0], APP_LED_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;	
        case APP_LED1_TASK_PRIO:
			Result = OSTaskCreateExt(AppLED1Task, (void*)p_arg,
						&PrivateStk.LED1[APP_LED1_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED1[0], APP_LED1_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;	
        case APP_LEDQ_TASK_PRIO:
            Result = OSTaskCreateExt(AppLED2Task, (void*)p_arg,
                        &PrivateStk.LED2[APP_LED2_TASK_STK_SIZE-1],prio,
                        prio,&PrivateStk.LED2[0], APP_LED2_TASK_STK_SIZE,
                        (void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;
		case APP_LCD_TASK_PRIO:
			Result = OSTaskCreateExt(AppLCDTask, (void*)p_arg,
						&PrivateStk.LCD[APP_LCD_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LCD[0], APP_LCD_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;				
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
	TaskCreate(APP_LED_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LED1_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LEDQ_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LCD_TASK_PRIO,(void*)0);
//    OSTaskDel(OS_PRIO_SELF); //���Ҳ���� ������ǰѵ�ǰ������ɾ����Ҳ����ɾ����ʼ���񣬽���������������Ϳ�ʼ�������ȼ�����ѭ������   
    
    OSTaskSuspend(APP_STARTUP_TASK_PRIO);
/*    OS_PRIO_SELF��һ����תվ���趨һ��ֵ0XFF,��Ϊ���ж�
    * �����ǵ������֪���Լ���ǰ��������ȼ���ʱ���ֱ��д���ȼ�������֪����ʱ��������������
    * ��OSTaskDel()�����ڲ���һ���жϳ���
    * if (prio == OS_PRIO_SELF) 
    * {                                  See if requesting to delete self    
    *  prio = OSTCBCur->OSTCBPrio;       Set priority to delete to current   
    * }
    *������ʵ�ֲ���֪��֪����ǰ��������ȼ�������ɾ����ǰ����Ĺ���
*/  
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
void AppLEDTask(void* p_arg)
{	
	while(1)
	{
		LED1_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED1_OFF();
		OSTimeDlyHMSM(0,0,0,400);
//        OSTaskDel(OS_PRIO_SELF);

	}
}

void AppLED1Task(void* p_arg)
{	
	while(1)
	{
		LED3_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED3_OFF();
		OSTimeDlyHMSM(0,0,0,400);	
//        OSTaskDel(OS_PRIO_SELF);
	}
}
void AppLED2Task(void* p_arg)
{	
	while(1)
	{
		LED4_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED4_OFF();
		OSTimeDlyHMSM(0,0,0,400);
//        OSTaskDel(OS_PRIO_SELF);     
	}
}
/**
  *************************************************************************************
  * @brief  LCD����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void AppLCDTask(void* p_arg)
{
    I2C_GPIO_Config();
    LcdInit();

    while(1)
    {

        LcdShowStrCenter("V1.0");   
//        OSTaskDel(OS_PRIO_SELF);        
    }
}












