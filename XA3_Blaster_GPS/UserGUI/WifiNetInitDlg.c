#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 }
};



/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[60];
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();

		ShowInfoTitle("WIFI");//无线局域网
		GUI_SetFont(&SMALL_FONT);
		GUI_DispStringHCenterAt((const char*)"WIFI Initializing...", 120,110);	//WIFI网络初始化中
		break;
	case WM_INIT_DIALOG://初始化界面	

		//在后台进行设备自检
		OSTaskDel(APP_WIFI_TASK_PRIO);
		TaskCreate(APP_WIFI_TASK_PRIO, (void*)0);/*打开WIFI网络*/
			
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
		}		
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
		break;
	case WM_TIMER:
		switch(WM_GetTimerId(pMsg->Data.v))
		{
			case GUI_TIMER_2MS_ID:
				WM_DeleteTimer(pMsg->Data.v);
				
				ShowWifiNetInfoDlg();			
				break;
            case GUI_TIMER_10MS_ID:
                WM_DeleteTimer(pMsg->Data.v);
                {
                    MsgBoxInit();
                    SetMsgBoxCaption("Device Abnormal");//设备异常
                    SetMsgBoxMessage("WIFI Abnormal!");//WIFI模块异常!
                    SetMsgBoxLeft("Back");
                    
                    if(ShowMsgBox() == GUI_KEY_LEFT)
                    {
                        ShowHelpToolMenuDlg();
                    }
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
	case WM_WIFI_TASK_END:
		if((ErrorStatus)pMsg->Data.v == ERROR)
		{
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);			
		}
		else
		{
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
		}		 		
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowWifiNetInitDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
