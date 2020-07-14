#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "自检蜂鸣器...",       GUI_ID_TEXT0,     0, 70, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			if(OpData.DlgCount == 1)
			{
				GUI_SetBkColor(GUI_RED);
				GUI_Clear();
			}
			else if(OpData.DlgCount == 2)
			{
				GUI_SetBkColor(GUI_GREEN);
				GUI_Clear();
			}
			else if(OpData.DlgCount == 3)
			{
				GUI_SetBkColor(GUI_BLUE);
				GUI_Clear();
			}
			else
			{				
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(COLOR_DLG_BK);
				GUI_Clear();
			}		
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("设备自检");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			OpData.DlgCount = 0;
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			TaskCreate(APP_SOUND_TASK_PRIO, (void*)5);												
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_5MS_ID, GUI_TIMER_5MS, 0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					if(OpData.DlgCount >= 4)
					{
						if(!TaskIsExist(APP_BACKGROUND_TASK_PRIO))
						{
							ShowSystemToolDlg();					
						}					
					}
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_5MS_ID:
					if(TaskIsExist(APP_SOUND_TASK_PRIO))
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_5MS);
					}
					else
					{
						WM_DeleteTimer(pMsg->Data.v);					
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "自检LCD:红->绿->蓝");
						WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1500MS_ID, GUI_TIMER_1500MS, 0);					
					}
					break;
				case GUI_TIMER_1500MS_ID:
					OpData.DlgCount++;
					if(OpData.DlgCount == 1)
					{
						WM_ResizeWindow(hWin,0,STATUSBAR_HEIGHT);
						WM_MoveTo(hWin, 0, 0);
						
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_1500MS);
					}
					else if ((OpData.DlgCount == 2) || (OpData.DlgCount == 3))
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_1500MS);
					}
					else if (OpData.DlgCount == 4)
					{
						WM_ResizeWindow(hWin,0,STATUSBAR_HEIGHT);
						WM_MoveTo(hWin, 0, STATUSBAR_HEIGHT);
						
						WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						
						WM_DeleteTimer(pMsg->Data.v);
						
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "自检电气性能...");
						TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)1);	
						WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);						
					}
					WM_InvalidateWindow(hWin);
					break;
				case GUI_TIMER_500MS_ID:					
					if(!TaskIsExist(APP_BACKGROUND_TASK_PRIO))
					{//已经自检完
						TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0),GUI_RED);
						switch(OpData.AppBackgroundTaskResult&0x0F)
						{
							case 0:
								TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0),GUI_BLUE);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "设备自检正常!");
								break;
							case 1:								
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "LCD总线异常!");
								break;
							case 2:
								sprintf((char*)OpData.GuiTemp, "%d通道静态电流大!", 1+((OpData.AppBackgroundTaskResult>>4)&0x0F));
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
								break;								
							case 3:
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "5V电路异常!");
								break;
							case 4:
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "6_7V电路异常!");
								break;	
							case 5:
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "11_3V电路异常!");
								break;	
							case 6:
								sprintf((char*)OpData.GuiTemp, "%d通道端口电压异常!", 1+((OpData.AppBackgroundTaskResult>>4)&0x0F));
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
								break;	
							case 7:
								sprintf((char*)OpData.GuiTemp, "%d通道反向电路异常!", 1+((OpData.AppBackgroundTaskResult>>4)&0x0F));
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
								break;								
							default:
								sprintf((char*)OpData.GuiTemp, "设备异常(%d)", OpData.AppBackgroundTaskResult);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
								break;								
						}
					}
					else
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
						UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));	
					}
					break;				
			}
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowSelfTestDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

