
#include "includes.h"

#define  TASK_STK_SIZE   512			//任务堆栈长度

OS_STK   MyTaskStk[TASK_STK_SIZE];		//定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];		//定义任务堆栈区
INT16S   key;					//用于退出uCOS_II的键	
INT8U	 x=0,y=0;				//字符显示位置

void  MyTask(void *data);			//声明任务
void  YouTask(void *data);			//声明任务
/************************主函数*********************************************/
void  main (void)
{
    char* s_M="M";				//定义要显示的字符


    OSInit();					//初始化uCOS_II

    PC_DOSSaveReturn();				//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);			//安装uCOS_II中断

    OSTaskCreate(MyTask,			//创建任务MyTask
		s_M,				//给任务传递参数
		&MyTaskStk[TASK_STK_SIZE - 1],//设置任务堆栈栈顶指针
		0);				//使任务MyTask的优先级别为0

    OSStart();					//启动uCOS_II的多任务管理
}

/***********************任务MyTask*******************************************/

void  MyTask (void *pdata)
{
    char* s_Y="Y";				//定义要显示的字符
    char* s="MyTask:我要求YouTask自己删除自己！";
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif

    pdata = pdata; 

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);		//安装uCOS_II时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);		//设置uCOS_II时钟频率
    OS_EXIT_CRITICAL();

    OSStatInit();				//初始化uCOS_II的统计任务
    OSTaskCreate(YouTask,			//创建任务MyTask
		s_Y,				//给任务传递参数
		&YouTaskStk[TASK_STK_SIZE - 1],	//设置任务堆栈栈顶指针
		2);				//使任务MyTask的优先级别为0
    for (;;) 
    {   
	if (x>30)
	{
		while( OSTaskDelReq( 2 ) != OS_TASK_NOT_EXIST )
		{
		        PC_DispStr(10,8,s,DISP_FGND_YELLOW+DISP_BGND_BLUE);			
			OSTimeDly(1);		//延时一个时钟节拍
		}
	}
     
        if (x>50) 
	{
	   x=0;
	   y+=2; 
        }
                                                 
        PC_DispChar(x, y,			//显示字符的位置
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE );
       		x += 1;                         

	//如果按下Esc键则退出uCOS_II
	if (PC_GetKey(&key) == TRUE) 
	{
            if (key == 0x1B) 
	    {
                PC_DOSReturn();
            }
        }

        OSTimeDlyHMSM(0, 0, 3, 0);		//等待
    }
}
/************************任务YouTask*****************************************/
void  YouTask (void *pdata)
{
    char *s1="YouTask:我必须要删除我自己了！";
    char *s2="YouTask:我已经删除我自己了！";
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif

    pdata = pdata; 

    for (;;) 
    { 
	if( OSTaskDelReq(OS_PRIO_SELF ) == OS_TASK_DEL_REQ )
	{
		PC_DispStr(10,10,s1,DISP_FGND_WHITE+DISP_BGND_BLACK);
		OSTimeDlyHMSM(0, 0, 15, 0);	
		PC_DispStr(10,12,s2,DISP_FGND_WHITE+DISP_BGND_BLACK);	
		OSTaskDel( OS_PRIO_SELF );
	}       
        if (x>50) 
	{
	   x=0;
	   y+=2; 
	}
                                                 
        PC_DispChar(x, y,			//显示字符的位置
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE );
       		x += 1;	
        OSTimeDlyHMSM(0, 0, 1, 0);		//等待
    }
}
/************************End*************************************************/
