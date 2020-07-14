#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.雷管分析 2.设备自检",       GUI_ID_TEXT0,     0, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.设备检验 4.固件升级",       GUI_ID_TEXT1,     0, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "5.扫码测试           ",       GUI_ID_TEXT2,     0, 110, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("设备工具");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					ShowMainMenuDlg();
					break;
				case GUI_CUSTOM_KEY_1:	
					ShowDetAnalystDlg();
					break;
				case GUI_CUSTOM_KEY_2:	
					if(RESULT_YES == ShowCustomMessageBox("自检过程禁止接雷管!", "设备自检", MB_YESNO))
					{
						ShowSelfTestDlg();
					}					
					break;						
				case GUI_CUSTOM_KEY_3:	
					ShowDeviceDemarcateMenuDlg();
					break;
				case GUI_CUSTOM_KEY_4:	
					ShowWaitDiskInsertDlg(0);
					break;
				case GUI_CUSTOM_KEY_5:	
					if ((GetHostEnumState() == ENUM_SUCCESS) && (SUPPORT_DEVICE_KEYBOARD == HostInfo.DeviceType))
					{
						ShowScanTestDlg();
					}
					else
					{
						ShowCustomMessageBox("请插入扫码枪!", OpData.DlgTitle, MB_OK);
					}
					break;					
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowSystemToolDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

