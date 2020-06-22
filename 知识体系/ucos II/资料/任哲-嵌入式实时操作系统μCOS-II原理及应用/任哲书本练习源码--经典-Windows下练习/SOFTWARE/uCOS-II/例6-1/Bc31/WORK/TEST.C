/************************Test*************************************/
#include "includes.h"
#define  TASK_STK_SIZE   512				//任务堆栈长度
OS_STK   StartTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];			//定义任务堆栈区
OS_MEM *IntBuffer;
INT8U    IntPart[50][64];
INT8U   *IntBlkPtr;
INT16S   key;						//用于退出的键
char*s1="MyTask正在运行";
INT8U err;  						
INT8U y=0;						//字符显示位置					
void  StartTask(void *data);				//声明起始任务
void  MyTask(void *data);				//声明任务
/************************主函数*********************************/
void  main (void)
{
    OSInit( );						//初始化uCOS_II
    PC_DOSSaveReturn( );				//保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);				//安装uCOS_II中断
    IntBuffer = OSMemCreate(IntPart,50,64,&err);
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
	IntBlkPtr=OSMemGet (
				IntBuffer, 	//内存分区的指针
				&err			//错误信息
				);
    	
        *IntBlkPtr=1;                                       
	PC_DispStr(*IntBlkPtr*10,++y,
		s1,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
	*++IntBlkPtr=2;
	PC_DispStr(*IntBlkPtr*10,++y,
		s1,
		DISP_BGND_BLACK+DISP_FGND_WHITE );
	IntBlkPtr--;
	OSMemPut (
		IntBuffer, 			//内存块所属内存分区的指针
		IntBlkPtr			//待释放内存块的指针
		);
          		
        OSTimeDlyHMSM(0, 0, 1, 0);		//等待1秒
    }
}
/************************End**************************************/
