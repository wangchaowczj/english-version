#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.查看设置",       GUI_ID_TEXT0,     0, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.重新设置",       GUI_ID_TEXT1,     0, 90, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("测试项目");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			BarCode.Status = BARCODE_IDLE;
			ClearScanKeyBoard();
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);				
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					if(GetWindowParam() == 0)
					{
						ShowSystemSetDlg();
					}
					else
					{
						ShowChannelTestPrepareDlg();
					}
					break;
				case GUI_CUSTOM_KEY_1:
					ShowDetTestCurrentContentMenuDlg(GetWindowParam());
					break;
				case GUI_CUSTOM_KEY_2:	
					if(ENTER_FACTORY == UserParam.Stage)
					{
						ShowDetTestContentEnterFactorySetDlg(GetWindowParam());					
					}
					else if((SEMIFINISHED == UserParam.Stage)||(FINISHED == UserParam.Stage))
					{
						ShowTestItemAndCodingSetDlg(GetWindowParam());
					}
					break;					
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTestContentMenuDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

