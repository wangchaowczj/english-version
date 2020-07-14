#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     0, 10, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.静态电流 2.充电电流",       GUI_ID_TEXT1,     0, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.芯片频率 4.测试项目",       GUI_ID_TEXT2,     0, 90, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("测试参数");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			if(OpData.ParamCfgSel == ENTER_FACTORY)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "入厂检测");
			}			
			else if(OpData.ParamCfgSel == SEMIFINISHED)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "半成品检测");
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "成品检测");
			}
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0),GUI_RED);
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
				case GUI_CUSTOM_KEY_BACK:
					ShowDetTestParamStageMenuDlg();
					break;			
				case GUI_CUSTOM_KEY_1:	
					ShowDetTestParamCurrentSetDlg(0);
					break;
				case GUI_CUSTOM_KEY_2:		
					ShowDetTestParamCurrentSetDlg(1);
					break;
				case GUI_CUSTOM_KEY_3:	
					ShowDetTestParamFreqSetDlg();
					break;
				case GUI_CUSTOM_KEY_4:
					ShowDetTestMaskBitSetDlg();
					break;						
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTestParamMenuDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

