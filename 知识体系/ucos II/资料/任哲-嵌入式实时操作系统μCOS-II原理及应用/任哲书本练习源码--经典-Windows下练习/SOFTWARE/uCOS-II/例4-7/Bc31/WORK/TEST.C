/************************Test*************************************/
#include "includes.h"
#define  TASK_STK_SIZE   512				//任务堆栈长度
OS_STK   StartTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   HerTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
INT16S   key;						//用于退出的键
char*s1="MyTask正在运行";
char*s2="YouTask正在运行";
char*s3="HerTask正在运行";
char*ss="MyTask请求信号量";
INT8U err;  						
INT8U y=0;						//字符显示位置 
INT32U Times=0;					
OS_EVENT *Mutexp;					//定义事件控制块
void  StartTask(void *data);				//声明起始任务
void  MyTask(void *data);				//声明任务
void  YouTask(void *data);				//声明任务
void  HerTask(void *data);				//声明任务
/************************主函数*********************************/
void  main (void)
{
    OSInit( );						//初始化uCOS_II
    PC_DOSSaveReturn( );				//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);				//安装uCOS_II中断
    Mutexp = OSMutexCreate (1,&err);				//定义信号量
    OSTaskCreate(StartTask,				//创建任务StartTask
	(void*)0,					//给任务传递参数
	&StartTaskStk[TASK_STK_SIZE - 1],		//设置任务堆栈栈顶
	0);						//使任务的优先级别为0
    OSStart( );						//启动多任务管理
}
/***********************任务StartTask*******************************/
void  StartTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);		//安装时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);		//设置uCOS_II时钟频率
    OS_EXIT_CRITICAL( );

    OSStatInit( );				//初始化统计任务
    OSTaskCreate(MyTask,			//创建任务MyTask
		(void*)0,			//给任务传递参数
		&MyTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶
		3);				//使任务的优先级别为3
    OSTaskCreate(YouTask,			//创建任务YouTask
		(void*)0,			//给任务传递参数
		&YouTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶
		4);				//使任务的优先级别为4
    OSTaskCreate(HerTask,			//创建任务HerTask
		(void*)0,			//给任务传递参数
		&HerTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶
		5);				//使任务的优先级别为5
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
        OSTimeDlyHMSM(0, 0, 3, 0);	//等待3秒
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
        OSTimeDlyHMSM(0, 0, 0, 200); 		//等待200毫秒                                          
        {
	    PC_DispStr(10,++y,
		ss,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
            OSMutexPend(Mutexp,0,&err);		//请求信号量
	    PC_DispStr(10,++y,
		s1,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
 
	    OSMutexPost(Mutexp);		//发送信号量
        }
        OSTimeDlyHMSM(0, 0, 0, 200);		//等待200毫秒
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
        PC_DispStr(10,++y,
		s2,
		DISP_BGND_BLACK+DISP_FGND_WHITE );	          		
        OSTimeDlyHMSM(0, 0, 0, 300);		//等待300毫秒
    }
}
/************************任务HerTask******************************/
void  HerTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;   

    for (;;) 
    {                                          
        OSMutexPend(Mutexp,0,&err);	//请求信号量        
	PC_DispStr(10,++y,
		s3,
		DISP_BGND_BLACK+DISP_FGND_WHITE );

        for(Times;Times<20000000;Times++)
	{
		OS_Sched();
	}
        
	OSMutexPost(Mutexp);		//发送信号量          		
        OSTimeDlyHMSM(0, 0, 1, 0);	//等待1秒
    }
}
/************************End**************************************/
