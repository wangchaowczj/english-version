#ifndef _TASKCREATE_H_
#define _TASKCREATE_H_

typedef struct
{
	OS_STK Startup[APP_STARTUP_TASK_STK_SIZE];
	OS_STK Usb[APP_USB_TASK_STK_SIZE];
	OS_STK UserGUI[APP_USER_GUI_TASK_STK_SIZE];	
	OS_STK KeyScan[APP_KEY_SCAN_TASK_STK_SIZE];
	OS_STK SafeMonitor[APP_SAFE_MONITOR_TASK_STK_SIZE];
	OS_STK Wifi[APP_WIFI_TASK_STK_SIZE];	
}APP_PRIVATE_STK;


typedef struct
{	
	OS_STK Stk[APP_PUBLIC_TASK_STK_SIZE];
}APP_PUBLIC_STK;

extern u8 TaskIsExist(u8 prio);
extern void RequestTaskDelAndWait(u8 prio);
extern INT8U TaskCreate(INT8U prio, void    *p_arg);

#endif
