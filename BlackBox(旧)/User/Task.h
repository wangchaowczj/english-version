#ifndef _TASK_H_
#define _TASK_H_

typedef struct
{		
	u16 overlaped;	
}OP_DATA;

typedef struct
{
	OS_EVENT* usb_rcv;
	OS_EVENT* MsgQeue;//定义消息队列指针  
}MSG_SEM;



extern STR_COMM  HidData ;
extern OP_DATA Op_Data;
extern MSG_SEM MsgSem;

extern void AppStartUpTask(void* p_arg);
extern void AppUSBTask(void* p_arg);
extern void AppLedTask(void* p_arg);
extern void AppCmdSaveTask(void* p_arg);

#endif
