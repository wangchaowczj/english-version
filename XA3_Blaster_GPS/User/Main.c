#include "includes.h"

int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000); //�������ļ����Ӧ
	
	OSInit();
	TaskCreate(APP_STARTUP_TASK_PRIO, (void*)0);	
	OSStart();
	return 0;
}
