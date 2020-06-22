/************************Test**************************************/
#include "includes.h"

#define  TASK_STK_SIZE   512			//任务堆栈长度

OS_STK   MyTaskStk[TASK_STK_SIZE];	//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];	//定义任务堆栈区
INT16S   key;							//用于退出uCOS_II的键	
INT8U	 y1=0,y2=0;						//字符显示位置
BOOLEAN  ac_key;						//定义信号量
char* s="原始数据";						//定义要显示的字符
void  MyTask(void *data);					//声明任务
void  YouTask(void *data);					//声明任务
/************************主函数*********************************/
void  main (void)
{
    OSInit( );								//初始化uCOS_II
    ac_key=1;							//设置信号量初值
    PC_DOSSaveReturn( );					//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);			//安装uCOS_II中断
    OSTaskCreate(MyTask,					//创建任务MyTask
		(void*)0,							//给任务传递参数
		&MyTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶指针
		0);								//使任务的优先级别为0

    OSStart( );							//启动uCOS_II的多任务管理
}

/***********************任务MyTask********************************/

void  MyTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
INT8U err;
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);			//安装uCOS_II时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);	//设置uCOS_II时钟频率
    OS_EXIT_CRITICAL( );
    OSStatInit( );							//初始化uCOS_II的统计任务
    OSTaskCreate(YouTask,					//创建任务MyTask
		(void*)0,							//给任务传递参数
		&YouTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶指针
		2);								//使任务的优先级别为2
    for (;;) 
    {  	
       	if(ac_key)
		{ 
			ac_key=FALSE;				//使信号量无效
        	s="MyTask访问共享数据s";         
			PC_DispStr(5, ++y1,			//显示字符的位置
				s, 
				DISP_BGND_BLACK+DISP_FGND_WHITE);
				ac_key=TRUE; 			//发信号
		}       
	//如果按下Esc键则退出uCOS_II
		if (PC_GetKey(&key) == TRUE) 
		{
            if (key == 0x1B) 
	    	{
                PC_DOSReturn( );
            }
        }	
        OSTimeDly(20);					//等待20个时钟节拍
    }
}
/************************任务YouTask****************************/
void  YouTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    INT8U err;       
    pdata = pdata; 
    for (;;) 
    { 
        if(ac_key)
		{
			ac_key=FALSE;				//使信号量为无信号                
        	s="YouTask访问共享数据s";         
			PC_DispStr(28, ++y2,			//显示字符串
				s, 
				DISP_BGND_BLACK+DISP_FGND_WHITE ); 
			OSTimeSet(0);				//置OSTime为0
			while(OSTime<500)
			{
				PC_DispStr(55, y2,		//显示字符串
					s, 
					DISP_BGND_BLACK+DISP_FGND_WHITE ); 
			}				
			ac_key=TRUE;				//发信号
		}
        OSTimeDly(10);					//等待10个时钟节拍
    }
}
/************************End**************************************/
