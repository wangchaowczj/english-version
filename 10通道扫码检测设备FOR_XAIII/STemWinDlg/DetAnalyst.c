#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 70, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 100, LCD_WIDTH_PIXEL,  30,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
	ErrorStatus Result;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("雷管分析");
			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);		
			
			ConnectChannel(0);
			
			if(GetCurrentValue(10,NULL) < 1000)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "等待雷管上线...");
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "请雷管重新上线...");
			}
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_150MS_ID, GUI_TIMER_150MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					ShowSystemToolDlg();
					break;
				case GUI_CUSTOM_KEY_OK:
				case GUI_CUSTOM_KEY_FOOT:
					ShowDetAnalystResultDlg();			
					break;									
			}	
			break;	
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{							
				case GUI_TIMER_150MS_ID:					
                    Result = XA2_DetCheckIn();
                    if(Result == ERROR)
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_150MS);
					}
					else
					{
						ShowDetAnalystResultDlg();
					}						
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					Voltage = GetLinABVoltage(0, NULL);
					Current = GetCurrentValue(10,NULL);				
					sprintf((char*)OpData.GuiTemp, "电压:%0.1fV  电流:%0.2fuA", Voltage/1000.0, Current/100.0);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
					
					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
					break;				
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetAnalystDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}
