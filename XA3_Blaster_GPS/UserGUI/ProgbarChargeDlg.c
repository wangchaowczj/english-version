#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR0,     20, 120, 200, 20,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];

	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();

		ShowInfoTitle("Blast Progress");//起爆流程
		GUI_SetFont(&SMALL_FONT);	
		GUI_DispStringAt("Charging Online DETs...",28,100);		//网路充电中...

		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			sprintf(&temp[0],"Log DET:%d ERR DET:%d",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:%d 错误雷管:
			sprintf(&temp[40],"Online DET:%d",SlaveInfo.OnlineCount);	//在线雷管:		
		}
		else
		{
			sprintf(&temp[0],"Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			sprintf(&temp[40],"Online DET:%d",GetDetOnlineCount());			//在线雷管:
		}
		ShowInfoHelp(&temp[0], &temp[40]);
		break;
	case WM_INIT_DIALOG://初始化界面
		PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0),&SMALL_FONT);
				
		hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR0);
		PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
		PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
		PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
        PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
		
		TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)3);
		ClearProgbarData();

		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_200MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
		break;
		//这个是接收键盘上的按键的相应
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:		
				break;			
		}
		break;
	case WM_UPDATE_PROGBAR://更新进度条
		PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);
		break;
	case WM_SET_PROGBAR_PARAM:
		PROGBAR_SetMinMax(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), 0, Op_Data.Progbar.Bar[0].Max);
		PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);	
		break;		
	case WM_TIMER:
		switch(WM_GetTimerId(pMsg->Data.v))
		{
			case GUI_TIMER_10MS_ID:				
				PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);
				
				if (Op_Data.BlastVerifyResult == '0')
				{//终止起爆
					BlastTerminationFromAppDlg();
					break;
				}
                if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                {//没有充电完成
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                    break;
                }
                WM_DeleteTimer(pMsg->Data.v);
                
                if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
                {
                    ShowBlastProcessDlg(1);			
                }
                else if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
                {
                    ShowRemoteBlastSlaveStateDlg(5);			
                }                
				break;			
			case GUI_TIMER_500MS_ID:
				WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
								
				sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
				break;
			default:
				break;
		}		
		break;		
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowProgbarChargeDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

