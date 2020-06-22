/************************Test*************************************/
#include "includes.h"
#define  TASK_STK_SIZE   512					//任务堆栈长度
OS_STK   StartTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];				//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];				//定义任务堆栈区
INT16S   key;									//用于退出的键
INT8U	 y1=0,y2=0;								//字符显示位置
void *msg_p;									//消息邮箱
void  StartTask(void *data);					//声明起始任务
void  MyTask(void *data);						//声明任务
void  YouTask(void *data);						//声明任务
/************************主函数*********************************/
void  main (void)
{
    OSInit( );									//初始化uCOS_II
    PC_DOSSaveReturn( );						//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);					//安装uCOS_II中断
    OSTaskCreate(StartTask,						//创建任务MyTask
				(void*)0,						//给任务传递参数
				&StartTaskStk[TASK_STK_SIZE - 1],//设置任务堆栈栈顶
				0);								//使任务的优先级别为0
    OSStart( );									//启动多任务管理
}
/***********************任务StartTask*******************************/
void  StartTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);				//安装时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);			//设置uCOS_II时钟频率
    OS_EXIT_CRITICAL( );

    OSStatInit( );								//初始化统计任务
	OSTaskCreate(MyTask,						//创建任务MyTask
				(void*)0,						//给任务传递参数
				&MyTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶
				1);								//使任务的优先级别为1
    OSTaskCreate(YouTask,						//创建任务YouTask
				(void*)0,						//给任务传递参数
				&YouTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶
				2);								//使任务的优先级别为2
    for (;;) 
    {
	//如果按下Esc键则退出uCOS_II
		if (PC_GetKey(&key) == TRUE) 
		{
            if (key == 0x1B) 
	    	{
                PC_DOSReturn( );
            }
        }
        OSTimeDlyHMSM(0, 0, 3, 0);				//等待3秒
    }
}
/************************任务MyTask*******************************/
void  MyTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;     
	for (;;) 
    {                                           
        PC_DispStr(5, ++y1,						//显示字符串
				"MyTask", 
				DISP_BGND_BLACK+DISP_FGND_WHITE );

		if(msg_p!=(void*) 0)					//请求消息
		{
			PC_DispStr(15,y1,
					msg_p, 						//显示收到的消息
					DISP_BGND_BLACK+DISP_FGND_WHITE ); 
			msg_p=(void*) 0;					//使消息邮箱为空
		}
        OSTimeDlyHMSM(0, 0, 1, 0);				//等待1秒
    }
}
/************************任务YouTask******************************/
void  YouTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    char*s="YouTask发送的消息";					//定义消息
    pdata = pdata; 
    for (;;) 
    { 
        PC_DispStr(40, ++y2,					//显示字符串
				"YouTask", 
				DISP_BGND_BLACK+DISP_FGND_WHITE );

        if(OSTimeGet( )>500 && msg_p==(void*) 0)
		{
			msg_p=s; 							//发送消息
			PC_DispStr(50,y2,
				"YouTask发了一个消息", 
				DISP_BGND_BLACK+DISP_FGND_WHITE );
		}       		
        OSTimeDlyHMSM(0, 0, 2, 0);				//等待1秒
    }
}
/************************End**************************************/
