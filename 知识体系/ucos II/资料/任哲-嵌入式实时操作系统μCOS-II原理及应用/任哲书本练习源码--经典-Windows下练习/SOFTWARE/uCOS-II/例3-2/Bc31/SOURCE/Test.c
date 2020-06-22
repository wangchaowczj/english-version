/******************************Test*******************************/
#include "includes.h"
#define  TASK_STK_SIZE   512				//任务堆栈长度
OS_STK   MyTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   InterTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
INT16S   key;						//用于退出uCOS_II的键
INT8U	 x=0,y=0;					//字符显示位置
BOOLEAN  InterKey=FALSE;
void  MyTask(void *data);				//声明任务
void  YouTask(void *data);				//声明任务
void  InterTask(void *data);				//声明任务
/************************主函数*********************************/
void  main (void)
{
    char* s_M="M";					//定义要显示的字符
    OSInit( );						//初始化uCOS_II
    PC_DOSSaveReturn( );				//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);				//安装uCOS_II中断
    OSTaskCreate(
		MyTask,					//创建任务MyTask
		s_M,					//给任务传递参数
		&MyTaskStk[TASK_STK_SIZE - 1],		//设置任务堆栈栈顶指针
		0					//任务的优先级别为0
		);				
    OSStart( );						//启动多任务管理
}

/*******************任务MyTask**********************************/

void  MyTask (void *pdata)
{
    char* s_Y="Y";					//定义要显示的字符
	char* s_H="H";
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);			//安装时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);			//设置时钟频率
    OS_EXIT_CRITICAL( );
    OSStatInit( );					//初始化统计任务
    OSTaskCreate(
		YouTask,				//创建任务MyTask
		s_Y,					//给任务传递参数
		&YouTaskStk[TASK_STK_SIZE - 1],		//设置任务堆栈栈顶指针
		1					// MyTask的优先级别为1
		);	
	OSTaskCreate(
			InterTask,			//创建任务MyTask
			s_H,				//给任务传递参数
			&InterTaskStk[TASK_STK_SIZE - 1],//设置任务堆栈栈顶指针
			2				// MyTask的优先级别为2
			);
    for (;;) 
    {
        if (x>50) 
		{
	   		x=0;
	   		y+=2; 
        }                                                 
        PC_DispChar(x, y,				//字符的显示位置
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE );
       		x += 1;   
		//如果按下Esc键则退出uCOS_II
		if (PC_GetKey(&key) == TRUE) 
		{
        	if (key == 0x1B) 
	    	{
            	PC_DOSReturn( );			//恢复Dos环境
        	}
    	}
    	OSTimeDlyHMSM(0, 0, 3, 0);			//等待3秒
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
        if (x>50) 
	{
	   x=0;
	   y+=2; 
	}                                                 
        PC_DispChar(
		x, y,				//字符的显示位置
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE 
		);
       	x += 1;	
        OSTimeDlyHMSM(0, 0, 1, 0);		//等待1秒
    }
}
/***********************InterTask*******************************/
char*s="运行了中断所要求运行的任务InterTask。";
void  InterTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    
    for (;;) 
    {
	if(InterKey)
	{
		if (x>50) 
		{
	   		x=0;
	   		y+=2; 
		}                                                 
		PC_DispChar(
			x, y,			//字符的显示位置
			*(char*)pdata, 
			DISP_BGND_BLACK+DISP_FGND_WHITE 
			);
		PC_DispStr(5,6,s,DISP_BGND_BLACK+DISP_FGND_WHITE);
       		x += 1;
	}
	InterKey=FALSE;
	OSIntNesting--;
        OSTimeDlyHMSM(0, 0, 1, 0);		//等待1秒
    }
}
/************************End************************************/
