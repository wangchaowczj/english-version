#include "includes.h"


#define APP_PUBLIC_TASK_MAX   4 //最多允许同时使用公共堆栈的任务数

__align(8) static APP_PRIVATE_STK PrivateStk;//私有栈,每个任务独立分配指定大小RAM资源
__align(8) static APP_PUBLIC_STK PublicStk[APP_PUBLIC_TASK_MAX];//共用栈,节省RAM资源
static u8 PublicStkPrio[APP_PUBLIC_TASK_MAX];//保存共用栈中已经分配出去的任务

static  OS_EVENT   *PublicStkMutex = 0;

/**
  *************************************************************************************
  * @brief  锁定栈资源
  * @param[in]  p_arg：无
  * @retval  无
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
static void PublicStkLock(void)
{
	INT8U  err;
	
	if(PublicStkMutex == 0)
	{
		PublicStkMutex = OSMutexCreate(RES_PUBLIC_STK_MUTEX_PRIO, &err);
	}
	
	OSMutexPend(PublicStkMutex,0,&err);
}
/**
  *************************************************************************************
  * @brief  释放栈资源
  * @param[in]  p_arg：无
  * @retval  无
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
static void PublicStkUnlock(void)
{
	if(PublicStkMutex != 0)
	{
		OSMutexPost(PublicStkMutex);	
	}
}

/**
  *************************************************************************************
  * @brief  判断任务是否存在
  * @param[in]  p_arg：任务参数
  * @retval  1 任务存在， 0不存在
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
u8 TaskIsExist(u8 prio)
{
	OS_STK_DATA StkData;

	if(OS_ERR_NONE == OSTaskStkChk(prio, &StkData))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/**
  *************************************************************************************
  * @brief  请求删除任务并等待完成删除
  * @param[in]  p_arg：任务参数
  * @retval  无
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void RequestTaskDelAndWait(u8 prio)
{
	OSTaskDelReq(prio);
	while(TaskIsExist(prio))
	{
		OSTimeDly(1);
	}
}
/**
  *************************************************************************************
  * @brief  查询是否有空间的公共栈
  * @param[in] 无
  * @retval  0 没有， 1有
  * @author ZH
  * @date   2016年12月21日
  * @note   无
  *************************************************************************************  
  */
u8 HasIdlePublicStk(void)
{
	u8 i;
	
	for(i=0; i<APP_PUBLIC_TASK_MAX; i++)
	{
		if(PublicStkPrio[i] == 0)
		{
			return 1;
		}
		else
		{
			if(!TaskIsExist(PublicStkPrio[i]))
			{
				return 1;
			}
		}
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  获取空闲的公共栈索引
  * @param[in] 要申请的任务级别
  * @retval  空闲的公共栈索引
  * @author ZH
  * @date   2016年12月21日
  * @note   无
  *************************************************************************************  
  */
u8 GetIdlePublicStk(INT8U prio)
{
	u8 i;
	
	while(1)
	{
		for(i=0; i<APP_PUBLIC_TASK_MAX; i++)
		{
			if(PublicStkPrio[i] == 0)
			{
				PublicStkPrio[i] = prio;
				return i;
			}
			else
			{
				if(!TaskIsExist(PublicStkPrio[i]))
				{
					PublicStkPrio[i] = prio;
					return i;
				}
			}
		}
		OSTimeDly(1);
	}	
}
/**
  *************************************************************************************
  * @brief  创建任务
  * @param[in]  prio：任务优先级 
  * @param[in]  p_arg：任务参数
  * @retval 结果 
  * @author ZH
  * @date   2016年7月5日
  * @note   无
  *************************************************************************************  
  */
INT8U TaskCreate(INT8U prio, void    *p_arg)
{
	void   (*PublicTask)(void *p_arg);
	u8 StkIndex = 0;
	INT8U Result = OS_ERR_PRIO_INVALID;
	
	PublicTask = NULL;
	
	switch(prio)
	{
		case APP_STARTUP_TASK_PRIO:
			Result = OSTaskCreateExt(AppStartUpTask, (void*)p_arg,
					&PrivateStk.Startup[APP_STARTUP_TASK_STK_SIZE-1],APP_STARTUP_TASK_PRIO,
					APP_STARTUP_TASK_PRIO,&PrivateStk.Startup[0], APP_STARTUP_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;
		case APP_USER_GUI_TASK_PRIO:
			Result = OSTaskCreateExt(AppUserGUITask, (void*)p_arg,
					&PrivateStk.UserGUI[APP_USER_GUI_TASK_STK_SIZE-1], APP_USER_GUI_TASK_PRIO,
					APP_USER_GUI_TASK_PRIO,&PrivateStk.UserGUI[0], APP_USER_GUI_TASK_STK_SIZE,
					(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			break;
		case APP_KEY_SCAN_TASK_PRIO:
			Result = OSTaskCreateExt(AppKeyScanTask, (void*)p_arg,
					&PrivateStk.KeyScan[APP_KEY_SCAN_TASK_STK_SIZE-1], APP_KEY_SCAN_TASK_PRIO,
					APP_KEY_SCAN_TASK_PRIO, &PrivateStk.KeyScan[0], APP_KEY_SCAN_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			break;
		case APP_USB_TASK_PRIO:
			Result = OSTaskCreateExt(AppUSBTask, (void*)p_arg,
					&PrivateStk.Usb[APP_USB_TASK_STK_SIZE-1], APP_USB_TASK_PRIO,
					APP_USB_TASK_PRIO, &PrivateStk.Usb[0], APP_USB_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			break;
		case APP_SAFE_MONITOR_TASK_PRIO:
			Result = OSTaskCreateExt(AppSafeMonitorTask, (void*)p_arg,
					&PrivateStk.SafeMonitor[APP_SAFE_MONITOR_TASK_STK_SIZE-1], APP_SAFE_MONITOR_TASK_PRIO, 
					APP_SAFE_MONITOR_TASK_PRIO, &PrivateStk.SafeMonitor[0], APP_SAFE_MONITOR_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
			break;
		case APP_WIFI_TASK_PRIO:
			Result = OSTaskCreateExt(AppWifiTask, (void*)p_arg,
					&PrivateStk.Wifi[APP_WIFI_TASK_STK_SIZE-1], APP_WIFI_TASK_PRIO, 
					APP_WIFI_TASK_PRIO, &PrivateStk.Wifi[0], APP_WIFI_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			break;				
		case APP_DET_OPERATE_TASK_PRIO:
			PublicTask = AppDetOperateTask;
			break;		
		case APP_RS485_TASK_PRIO:
			PublicTask = AppRS485Task;
			break;	
		case APP_TEMPORARY_TASK_PRIO:
			PublicTask = AppTemporaryTask;
			break;
		case APP_BACKGROUND_TASK_PRIO:
			PublicTask = AppBackgroundTask;
			break;
		case APP_SOUND_TASK_PRIO:
			PublicTask = AppSoundTask;
			break;
		case APP_LOW_TASK_PRIO:
			PublicTask = AppLowPriTask;
			break;				
	}
	if(PublicTask != NULL)
	{
		PublicStkLock();
		
		StkIndex = GetIdlePublicStk(prio);//申请一个公共任务栈,如果没有空闲栈时会一直等到有为止
		Result = OSTaskCreateExt(PublicTask, (void*)p_arg,
					&PublicStk[StkIndex].Stk[APP_PUBLIC_TASK_STK_SIZE-1],prio,
					prio,&PublicStk[StkIndex].Stk[0], APP_PUBLIC_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
		if (OS_ERR_NONE != Result)
		{//任务创建失败，必需释放已申请的公共任务栈
			PublicStkPrio[StkIndex] = 0;
		}	
		PublicStkUnlock();			
	}	
	return Result;
}
