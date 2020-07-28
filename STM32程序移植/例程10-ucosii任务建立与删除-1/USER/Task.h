#ifndef _TASK_H_
#define _TASK_H_

typedef struct
{
	OS_STK Startup[APP_STARTUP_TASK_STK_SIZE];
//	OS_STK RS485[APP_RS485_TASK_STK_SIZE];
	OS_STK LED[APP_LED_TASK_STK_SIZE];
    OS_STK LED1[APP_LED1_TASK_STK_SIZE];
    OS_STK LED2[APP_LED2_TASK_STK_SIZE];
    OS_STK LCD[APP_LCD_TASK_STK_SIZE];
//	OS_STK RS485Dir[APP_RS485_DIR_TASK_STK_SIZE];
	
}APP_PRIVATE_STK;

typedef struct
{
	OS_EVENT* RS485;
}MSG_SEM;

typedef struct
{		
	u32 SystemIdleTime; 
}OP_DATA;


extern void AppStartUpTask(void* p_arg);
extern INT8U TaskCreate(INT8U prio, void*  p_arg);
//extern void AppRS485Task(void* p_arg);
extern void AppLEDTask(void* p_arg);
extern void AppLED1Task(void* p_arg);
extern void AppLCDTask(void* p_arg);
extern void AppLED2Task(void* p_arg);
//extern void AppRS485DirTask(void* p_arg);

#endif
