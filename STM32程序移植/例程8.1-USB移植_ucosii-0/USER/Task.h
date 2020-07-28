#ifndef _TASK_H_
#define _TASK_H_

typedef struct
{
	OS_STK Startup[APP_STARTUP_TASK_STK_SIZE];
	OS_STK RS485[APP_RS485_TASK_STK_SIZE];
	OS_STK LED[APP_LED_TASK_STK_SIZE];
	OS_STK RS485Dir[APP_RS485_DIR_TASK_STK_SIZE];
	
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
extern void AppRS485Task(void* p_arg);
extern void AppLEDTask(void* p_arg);
extern void AppRS485DirTask(void* p_arg);

#endif
