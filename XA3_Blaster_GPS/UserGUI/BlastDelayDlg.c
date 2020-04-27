#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//取消
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  50,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	static u32 DelayFinishTime = 0;//延时结束时对话的系统时间
	u32 Value, CurrentTime;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Blast Progress");	//起爆流程
			ShowInfoHelp("DET will blast", "when the countdown is over");//起爆延时结束后将起爆雷管
			GUI_SetFont(&SMALL_FONT);
			break;
		case WM_INIT_DIALOG://初始化界面
			//device_info.BlastDelay大于1时才会调用本对话框
			DelayFinishTime = OSTimeGet() + (device_info.BlastDelay-1)*60000/(1000/OS_TICKS_PER_SEC);
            
			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &NORMAL_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			DlghWin.Timer = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}			 
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
                    WM_DeleteTimer(DlghWin.Timer);
                    
					CurrentTime = OSTimeGet();//保存退出时的系统时间
                    switch(ShowBlastCannelConfirm())
                    {
                        case GUI_KEY_LEFT:
                            DelayFinishTime += (OSTimeGet() - CurrentTime);
                            DlghWin.Timer = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
                            break;
                        case GUI_KEY_RIGHT:
                            ShowBlastCannelDlg();
                            break;
                    }                   
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					
					CurrentTime = OSTimeGet();
					//减去5秒的目的因为，最后5秒倒数计数不在本对话框中显示
					if(CurrentTime < DelayFinishTime-(5000/(1000/OS_TICKS_PER_SEC)))
					{
						//DelayFinishTime - CurrentTime得到的值为离延时结束对应的滴答定时器个数
						Value = (DelayFinishTime - CurrentTime)*(1000/OS_TICKS_PER_SEC)/100;
						sprintf(temp, "%02d:%02d.%01d", Value/600, Value/10%60,Value%10);						
						TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
					}
					else
					{
						WM_DeleteTimer(pMsg->Data.v);
						
						ShowBlastDetDlg();
					}				
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
						BlastTerminationFromAppDlg();
						break;
					}					
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

void ShowBlastDelayDlg()
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
