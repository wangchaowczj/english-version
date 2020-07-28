#include "includes.h"
__align(8) APP_PRIVATE_STK PrivateStk; //汇编字节对齐

OS_EVENT* SW_USB;

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
	INT8U Result = OS_ERR_PRIO_INVALID;
	
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
        case APP_LED1_TASK_PRIO:
			Result = OSTaskCreateExt(AppLED1Task, (void*)p_arg,
						&PrivateStk.LED1[APP_LED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED1[0], APP_LED1_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
        case APP_LED2_TASK_PRIO:
			Result = OSTaskCreateExt(AppLED2Task, (void*)p_arg,
						&PrivateStk.LED2[APP_LED2_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED2[0], APP_LED2_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
        case APP_GetUSB_TASK_PRIO:
			Result = OSTaskCreateExt(AppGetUSBTask, (void*)p_arg,
						&PrivateStk.GetUSB[APP_GetUSB_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.GetUSB[0], APP_GetUSB_TASK_STK_SIZE,
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
    SW_USB = OSSemCreate(0);
    TaskCreate(APP_GetUSB_TASK_PRIO,(void*)0);
    TaskCreate(APP_LED2_TASK_PRIO,(void*)0);
    TaskCreate(APP_LED1_TASK_PRIO,(void*)0);
    TaskCreate(APP_LED_TASK_PRIO,(void*)0);


	OSTaskDel(OS_PRIO_SELF);
  
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
	u8 err;
    while(1)
	{
		OSSemPend(SW_USB,0,&err);
        LED1_ON();
//		OSTimeDlyHMSM(0,0,0,100);
//		LED1_OFF();
//		OSTimeDlyHMSM(0,0,0,400);	
	}
}

void AppLED1Task(void* p_arg)
{	
//    u8 err;
	while(1)
	{
//		OSSemPend(SW_USB,0,&err);
        LED3_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED3_OFF();
		OSTimeDlyHMSM(0,0,0,400);	
	}
}
void AppLED2Task(void* p_arg)
{	
//    u8 err;
	while(1)
	{
//		OSSemPend(SW_USB,0,&err);
        LED4_ON();
		OSTimeDlyHMSM(0,0,0,100);
		LED4_OFF();
		OSTimeDlyHMSM(0,0,0,400);	
	}
}

void AppGetUSBTask(void* p_arg)
{	
    u8 t=0;
    GET_USB_CONFIG();  
	while(1)
	{
		if(USB_ST==1)
        {
            t++;
            if(t>5)
            {
                t = 0; 
                OSSemPost(SW_USB);
            }   
            
        }            
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
        LcdShowStr2Center("鲲程电子 ","V1.0");      
    }
}











