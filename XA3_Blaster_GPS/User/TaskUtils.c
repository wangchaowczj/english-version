#include "includes.h"


#define APP_PUBLIC_TASK_MAX   4 //�������ͬʱʹ�ù�����ջ��������

__align(8) static APP_PRIVATE_STK PrivateStk;//˽��ջ,ÿ�������������ָ����СRAM��Դ
__align(8) static APP_PUBLIC_STK PublicStk[APP_PUBLIC_TASK_MAX];//����ջ,��ʡRAM��Դ
static u8 PublicStkPrio[APP_PUBLIC_TASK_MAX];//���湲��ջ���Ѿ������ȥ������

static  OS_EVENT   *PublicStkMutex = 0;

/**
  *************************************************************************************
  * @brief  ����ջ��Դ
  * @param[in]  p_arg����
  * @retval  ��
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
  * @brief  �ͷ�ջ��Դ
  * @param[in]  p_arg����
  * @retval  ��
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
  * @brief  �ж������Ƿ����
  * @param[in]  p_arg���������
  * @retval  1 ������ڣ� 0������
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
  * @brief  ����ɾ�����񲢵ȴ����ɾ��
  * @param[in]  p_arg���������
  * @retval  ��
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
  * @brief  ��ѯ�Ƿ��пռ�Ĺ���ջ
  * @param[in] ��
  * @retval  0 û�У� 1��
  * @author ZH
  * @date   2016��12��21��
  * @note   ��
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
  * @brief  ��ȡ���еĹ���ջ����
  * @param[in] Ҫ��������񼶱�
  * @retval  ���еĹ���ջ����
  * @author ZH
  * @date   2016��12��21��
  * @note   ��
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
  * @brief  ��������
  * @param[in]  prio���������ȼ� 
  * @param[in]  p_arg���������
  * @retval ��� 
  * @author ZH
  * @date   2016��7��5��
  * @note   ��
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
		
		StkIndex = GetIdlePublicStk(prio);//����һ����������ջ,���û�п���ջʱ��һֱ�ȵ���Ϊֹ
		Result = OSTaskCreateExt(PublicTask, (void*)p_arg,
					&PublicStk[StkIndex].Stk[APP_PUBLIC_TASK_STK_SIZE-1],prio,
					prio,&PublicStk[StkIndex].Stk[0], APP_PUBLIC_TASK_STK_SIZE,
					(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
		if (OS_ERR_NONE != Result)
		{//���񴴽�ʧ�ܣ������ͷ�������Ĺ�������ջ
			PublicStkPrio[StkIndex] = 0;
		}	
		PublicStkUnlock();			
	}	
	return Result;
}
