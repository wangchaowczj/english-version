#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.检测环节 2.测试项目",       GUI_ID_TEXT0,     0, 30, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.测试参数 4.声音设置",       GUI_ID_TEXT1,     0, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },  
  { TEXT_CreateIndirect,   "5.屏幕亮度 6.时钟设置",       GUI_ID_TEXT2,     0, 90, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "7.通道设置 8.联网设置",       GUI_ID_TEXT3,     0, 120, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "9.异常处理           ",       GUI_ID_TEXT4,     0, 150, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("系统设置");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					ShowMainMenuDlg();
					break;
				case GUI_CUSTOM_KEY_1:	
					ShowStageSetDlg();
					break;
				case GUI_CUSTOM_KEY_2:
					if(DET_CODE_VERIFY == UserParam.Stage)
					{
						ShowCustomMessageBox("内码验证环节不需要设置!", OpData.DlgTitle, MB_OK);
					}
					else
					{
						ShowDetTestContentMenuDlg(0);				
					}                    
					break;						
				case GUI_CUSTOM_KEY_3:	
					ShowDetTestParamPwdDlg();
					break;
				case GUI_CUSTOM_KEY_4:	
					ShowSoundSetDlg();
					break;
				case GUI_CUSTOM_KEY_5:	
					ShowLcdBrightSetDlg();
					break;
				case GUI_CUSTOM_KEY_6:	
					ShowRtcSetDlg();
					break;	
				case GUI_CUSTOM_KEY_7:	
					ShowChannelCountSetDlg();
					break;
				case GUI_CUSTOM_KEY_8:	
                    ShowNetModeSetDlg();					
					break;
				case GUI_CUSTOM_KEY_9:	
					ShowAutoBackSetDlg();
					break;                    
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowSystemSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

