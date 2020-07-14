#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "等待扫码...",       GUI_ID_TEXT0,     0, 0, LCD_WIDTH_PIXEL,  LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HTIMER Htimer;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("扫码测试");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			TEXT_SetWrapMode(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_WRAPMODE_CHAR);
			
			BarCode.Status = BARCODE_IDLE;
			ClearScanKeyBoard();			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS,0);
			Htimer = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS,0);
			
			ClearTimerHandleBuffer();
			SaveTimerHandleToBuffer(Htimer);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:	
					ShowMainMenuDlg();
					break;		
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					if(ScanKeyBoard.Status == SCAN_KEYBOARD_END)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)ScanKeyBoard.Data);
						BarCode.Status = BARCODE_IDLE;
						ClearScanKeyBoard();
						
						RemoveTimerHandleAndDeleteById(GUI_TIMER_500MS_ID);						
					}				
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					break;
				case GUI_TIMER_500MS_ID:										
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));	
					break;					
			}			
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowScanTestDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

