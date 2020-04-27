#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     0, 110, 220,  20,  0,          0 }
};

	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("WIFI");//无线局域网
		
		GUI_SetFont(&SMALL_FONT);
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
			sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);	//注册雷管:%u 错误雷管:
			ShowInfoHelp(&temp[0], &temp[40]);				
		}
		else
		{
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%u 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);			
		}
		break;
	case WM_INIT_DIALOG://初始化界面	
		TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		sprintf(&temp[0], "Connecting %s...", MyWifi.Ssid);//正在连接
		TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
		
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);

		OSTaskDel(APP_WIFI_TASK_PRIO);
        TaskCreate(APP_WIFI_TASK_PRIO, (void*)1);/*连接WIFI网络*/
		
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:	
				if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
				{
					ShowWifiPasswordDlg(GetWindowParam());
				}
				break;			
			default:
				break;
		}
		break;	
	case WM_TIMER:
		switch(WM_GetTimerId(pMsg->Data.v))
		{
			case GUI_TIMER_500MS_ID:
				WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
				
				sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
				break;
			default:
				break;
		}		
		break;
	case WM_WIFI_TASK_END:
		if((ErrorStatus)pMsg->Data.v == ERROR)
		{
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Connect Failed!");//连接失败!
			WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
		}
		else
		{
			ShowHelpToolMenuDlg();
		}
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowWifiConnectDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}

