#include "includes.h"
__align(8) APP_PRIVATE_STK PrivateStk; //汇编字节对齐

OP_DATA Op_Data;
//bool EnableMaster = 0; 			// Master/Slave selection 
//bool是字节型关键字


/**
  *************************************************************************************
  * @brief  创建任务
  * @param[in]  prio：任务优先级 
  * @param[in]  p_arg：任务参数
  * @retval 结果 
  * @author 
  * @date   2016年7月5日
  * @note   无
  *************************************************************************************  
  */
INT8U TaskCreate(INT8U prio, void* p_arg)
{
	INT8U Result = OS_ERR_PRIO_INVALID;//啥意思？
	
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
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2018年3月1日
  * @note   无
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{   
	SystemGpioConfig();
	TaskCreate(APP_LED_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LED1_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LEDQ_TASK_PRIO,(void*)0); 
    TaskCreate(APP_LCD_TASK_PRIO,(void*)0);
//    OSTaskDel(OS_PRIO_SELF); //这个也可以 ，这个是把当前的任务删除，也就是删除起始任务，接下来任务调度器就开始根据优先级不断循环查找   
    
    OSTaskSuspend(APP_STARTUP_TASK_PRIO);
/*    OS_PRIO_SELF是一个中转站，设定一个值0XFF,是为了判断
    * 作用是当编程者知道自己当前任务的优先级的时候就直接写优先级，当不知道的时候就用这个来代替
    * 在OSTaskDel()函数内部有一个判断程序
    * if (prio == OS_PRIO_SELF) 
    * {                                  See if requesting to delete self    
    *  prio = OSTCBCur->OSTCBPrio;       Set priority to delete to current   
    * }
    *这样就实现不管知不知道当前任务的优先级都可以删除当前任务的功能
*/  
}

/**
  *************************************************************************************
  * @brief  LED任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2014年9月15日
  * @note   无
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
  * @brief  LCD任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2014年9月15日
  * @note   无
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












