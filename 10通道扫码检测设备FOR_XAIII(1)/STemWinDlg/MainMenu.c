#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1},
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.雷管检测 2.设备工具",       GUI_ID_TEXT0,     0, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.系统设置 4.设备信息",       GUI_ID_TEXT1,     0, 100, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     0, 130, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("主菜单");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
            ClearDetInfoQueue(&DetInfoQueue);
            DetOutsideCodeBuffer.Count = 0;		
			
			ClearChannelOldStatus();
            OpData.TestStatus = TEST_UNALLOWEED;
			
			BarCode.Status = BARCODE_IDLE;
			ClearScanKeyBoard();
			OpenScanKeyBoard();
            SetAllChannelBarCodeIdle();
            OSTaskDel(APP_FACTORY_API_TASK_PRIO);           
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_1:
                    ShowChannelTestPrepareDlg();					
					break;				
				case GUI_CUSTOM_KEY_2:	
					ShowSystemToolDlg();
					break; 
				case GUI_CUSTOM_KEY_3:
					ShowSystemSetDlg();
					break;
				case GUI_CUSTOM_KEY_4:
					ShowDeviceInfoMenuDlg();
					break;						
			}
			break;
		case WM_TIMER:
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowMainMenuDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

