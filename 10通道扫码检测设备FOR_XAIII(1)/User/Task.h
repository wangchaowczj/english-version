#ifndef _TASK_H_
#define _TASK_H_
#include "GUI.h"
#include "WM.h"
extern DEVICE_INFO DeviceInfo;

typedef struct
{
	OS_EVENT* usb_rcv;
}MSG_SEM;
extern MSG_SEM MsgSem;

extern USER_PARAM UserParam;

typedef enum
{
    TEST_UNALLOWEED,
    TEST_WAIT_START,
    TEST_DOING,
    TEST_WAIT_OFFLINE
}TEST_STATUS;

typedef struct
{
	u8 UsbState;
	char DlgTitle[31];
	u8 GuiTemp[200];
	u8 GuiTemp1[20];
	u8 DlgState;
	u16 DlgCount;
	WM_HTIMER htimer;
	WM_HWIN CurrentInput;
	u8 Result;
	u8 BatCapacity;
	u8 Select;//当前节点列表选中
	u8 AppBackgroundTaskResult;
	u16 DetDetectorTaskResult;
	u8 OutputWareFlag;//标定时输出方波
	u32 DetDetectorPassCount;
	USE_STAGE_CFG ParamCfgSel;
    TEST_STATUS TestStatus;
    u32 NetModeNext;
}OP_DATA;
extern OP_DATA OpData;




extern STR_COMM  HidData ;
extern STR_COMM  RS485Data ;
extern STR_COMM  RS232Data ;
extern volatile u32 RS485RecTimeLatest; 
extern volatile u32 RS232RecTimeLatest; 
extern volatile u8 MCU_IO_IN1_Flag; 

extern void AppStartUpTask(void* p_arg);
extern void AppUsbTask(void* p_arg);
extern void AppCH374Task(void* p_arg);
extern void AppKeyScanTask(void* p_arg);
extern void AppUserGUITask(void* p_arg);
extern void DetDetectorTask(void* p_arg);
extern void AppBackgroundTask(void* p_arg);
extern void AppSoundTask(void* p_arg);
extern void AppFactoryApiTask(void* p_arg);

#endif
