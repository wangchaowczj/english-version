/************************Test*************************************/
#include "includes.h"
#define  TASK_STK_SIZE   512					//任务堆栈长度
OS_STK   StartTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];				//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];				//定义任务堆栈区
INT16S   key;	
char*s1="MyTask";
char*s2="YouTask"; 
INT8U err; 
INT8U y=0;
INT8U YouTaskRun=0;
OS_EVENT *Fun_Semp; 
void  Fun(INT8U x,INT8U y);
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
    Fun_Semp = OSSemCreate (0);
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
        OSSemPend(Fun_Semp,0,&err);	//请求信号量
	PC_DispStr(0,++y,
		s1,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
	Fun(7,y);			//调用函数Fun( )
	
        OSTimeDlyHMSM(0, 0, 1, 0);				//等待1秒
    }
}
/************************任务YouTask******************************/
void  YouTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;     
     
    for (;;) 
    {           
	PC_DispStr(0,++y,
		s2,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
	if(YouTaskRun==5)
	      OSSemPost(Fun_Semp);		//发送信号量
	YouTaskRun++;          		
        OSTimeDlyHMSM(0, 0, 2, 0);				//等待2秒
    }
}
/************************Fun()************************************/
void  Fun(INT8U x,INT8U y)
{
	PC_DispStr(x,y,
		"调用了Fun( )函数",
		DISP_BGND_BLACK+DISP_FGND_WHITE );
}
/************************End**************************************/
