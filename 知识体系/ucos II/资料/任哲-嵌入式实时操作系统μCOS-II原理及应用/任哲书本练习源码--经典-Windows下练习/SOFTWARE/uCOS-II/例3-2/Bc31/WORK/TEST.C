/******************************Test*******************************/
#include "includes.h"
#define  TASK_STK_SIZE   512				//�����ջ����
OS_STK   MyTaskStk[TASK_STK_SIZE];			//���������ջ��
OS_STK   YouTaskStk[TASK_STK_SIZE];			//���������ջ��
OS_STK   InterTaskStk[TASK_STK_SIZE];			//���������ջ��
INT16S   key;						//�����˳�uCOS_II�ļ�
INT8U	 x=0,y=0;					//�ַ���ʾλ��
BOOLEAN  InterKey=FALSE;
void  MyTask(void *data);				//��������
void  YouTask(void *data);				//��������
void  InterTask(void *data);				//��������
/************************������*********************************/
void  main (void)
{
    char* s_M="M";					//����Ҫ��ʾ���ַ�
    OSInit( );						//��ʼ��uCOS_II
    PC_DOSSaveReturn( );				//����Dos����
    PC_VectSet(uCOS, OSCtxSw);				//��װuCOS_II�ж�
    OSTaskCreate(
		MyTask,					//��������MyTask
		s_M,					//�����񴫵ݲ���
		&MyTaskStk[TASK_STK_SIZE - 1],		//���������ջջ��ָ��
		0					//��������ȼ���Ϊ0
		);				
    OSStart( );						//�������������
}

/*******************����MyTask**********************************/

void  MyTask (void *pdata)
{
    char* s_Y="Y";					//����Ҫ��ʾ���ַ�
	char* s_H="H";
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);			//��װʱ���ж�����
    PC_SetTickRate(OS_TICKS_PER_SEC);			//����ʱ��Ƶ��
    OS_EXIT_CRITICAL( );
    OSStatInit( );					//��ʼ��ͳ������
    OSTaskCreate(
		YouTask,				//��������MyTask
		s_Y,					//�����񴫵ݲ���
		&YouTaskStk[TASK_STK_SIZE - 1],		//���������ջջ��ָ��
		1					// MyTask�����ȼ���Ϊ1
		);	
	OSTaskCreate(
			InterTask,			//��������MyTask
			s_H,				//�����񴫵ݲ���
			&InterTaskStk[TASK_STK_SIZE - 1],//���������ջջ��ָ��
			2				// MyTask�����ȼ���Ϊ2
			);
    for (;;) 
    {
        if (x>50) 
		{
	   		x=0;
	   		y+=2; 
        }                                                 
        PC_DispChar(x, y,				//�ַ�����ʾλ��
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE );
       		x += 1;   
		//�������Esc�����˳�uCOS_II
		if (PC_GetKey(&key) == TRUE) 
		{
        	if (key == 0x1B) 
	    	{
            	PC_DOSReturn( );			//�ָ�Dos����
        	}
    	}
    	OSTimeDlyHMSM(0, 0, 3, 0);			//�ȴ�3��
}
}

/************************����YouTask******************************/

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
		x, y,				//�ַ�����ʾλ��
		*(char*)pdata, 
		DISP_BGND_BLACK+DISP_FGND_WHITE 
		);
       	x += 1;	
        OSTimeDlyHMSM(0, 0, 1, 0);		//�ȴ�1��
    }
}
/***********************InterTask*******************************/
char*s="�������ж���Ҫ�����е�����InterTask��";
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
			x, y,			//�ַ�����ʾλ��
			*(char*)pdata, 
			DISP_BGND_BLACK+DISP_FGND_WHITE 
			);
		PC_DispStr(5,6,s,DISP_BGND_BLACK+DISP_FGND_WHITE);
       		x += 1;
	}
	InterKey=FALSE;
	OSIntNesting--;
        OSTimeDlyHMSM(0, 0, 1, 0);		//�ȴ�1��
    }
}
/************************End************************************/
