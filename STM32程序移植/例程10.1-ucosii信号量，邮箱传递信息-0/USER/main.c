#include <stm32f10x.h>
#include "includes.h"




int main(void) 
{
	OSInit();    
	TaskCreate(APP_STARTUP_TASK_PRIO, (void*)0);
	OSStart();
	return 0;
	
}



