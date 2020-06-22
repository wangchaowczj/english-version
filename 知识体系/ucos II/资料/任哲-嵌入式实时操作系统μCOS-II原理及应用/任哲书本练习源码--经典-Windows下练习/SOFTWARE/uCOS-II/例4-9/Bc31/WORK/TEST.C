/************************Test*************************************/
#include "includes.h"
#define  TASK_STK_SIZE   512				//任务堆栈长度
#define  N_MESSAGES      128				//定义消息队列长度
OS_STK   StartTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
INT16S   key;						//用于退出的键

char*ss;
char*s100;
char*s;
char*s500;
void *MsgGrp[N_MESSAGES];				//定义消息指针数组
INT8U err;  						
INT8U y=0;						//字符显示位置
				
OS_EVENT *Str_Q;					//定义事件控制块
void  StartTask(void *data);				//声明起始任务
void  MyTask(void *data);				//声明任务
void  YouTask(void *data);				//声明任务
/************************主函数*********************************/
void  main (void)
{
    OSInit( );						//初始化uCOS_II
    PC_DOSSaveReturn( );				//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);				//安装uCOS_II中断
    Str_Q = OSQCreate (&MsgGrp[0],N_MESSAGES);		//创建消息队列
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
    s="这个串能收到几次？";
    OSQPostFront(Str_Q,s);			//发送消息

    for (;;) 
    {
	if(OSTimeGet()>100 && OSTimeGet()<500)
	{
		s100="现在OSTime的值在100到500之间";
		OSQPostFront(Str_Q,s100);	//发送消息
		s="这个串是哪个任务收到的？";
		OSQPostFront(Str_Q,s);		//发送消息
	
	}
	if(OSTimeGet()>5000 && OSTimeGet()<5500)
	{
		s500="现在OSTime的值在5000到5500之间";
		OSQPostFront(Str_Q,s500);	//发送消息
	}
	//如果按下Esc键则退出uCOS_II
	if (PC_GetKey(&key) == TRUE) 
	{
            if (key == 0x1B) 
	    {
                PC_DOSReturn();
            }
        }
        OSTimeDlyHMSM(0, 0, 1, 0);	//等待1秒
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
	ss=OSQPend(Str_Q,0,&err);  	//请求消息队列                                         
        PC_DispStr(10,++y,
		ss,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
        OSTimeDlyHMSM(0, 0, 1, 0);	//等待1秒
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
	ss=OSQPend(Str_Q,0,&err); 	//请求消息队列                                          
        PC_DispStr(10,++y,
		ss,
		DISP_BGND_BLACK+DISP_FGND_WHITE );	          		
        OSTimeDlyHMSM(0, 0, 1, 0);		//等待1秒
    }
}

/************************End**************************************/
