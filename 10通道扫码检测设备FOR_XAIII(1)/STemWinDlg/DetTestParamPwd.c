#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "密码:",       GUI_ID_TEXT0,     70, 75, 60,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     130, 75-2, 120,  28,  0,          0 },
  { TEXT_CreateIndirect,   "方向键回删",       GUI_ID_TEXT1,     0, 105, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u32 Pwd;
	
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

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);		
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 8);
			MULTIEDIT_SetPasswordMode(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 1);
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);	
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
					{
						Pwd = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
						if(Pwd != 12345678UL)
						{
							if(RESULT_NULL == ShowCustomMessageBox("密码错误!", OpData.DlgTitle, MB_OK))
							{
								return;
							}
							MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "");
							break;
						}
				
						ShowDetTestParamStageMenuDlg();					
					}
					else
					{
						ShowCustomMessageBox("请输入密码!", OpData.DlgTitle, MB_OK);
					}
					break;
				case GUI_CUSTOM_KEY_BACK:
					ShowSystemSetDlg();					
					break;
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F3:
				case GUI_CUSTOM_KEY_F4:
					if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
					{						
						MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GUI_KEY_BACKSPACE);
					}					
					break;					
				case GUI_CUSTOM_KEY_0:
				case GUI_CUSTOM_KEY_1:
				case GUI_CUSTOM_KEY_2:
				case GUI_CUSTOM_KEY_3:
				case GUI_CUSTOM_KEY_4:
				case GUI_CUSTOM_KEY_5:
				case GUI_CUSTOM_KEY_6:
				case GUI_CUSTOM_KEY_7:
				case GUI_CUSTOM_KEY_8:
				case GUI_CUSTOM_KEY_9:
					ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
					MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), ((WM_KEY_INFO*)(pMsg->Data.p))->Key);
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);

					if(WM_IsWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{						
						ShowCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
					}					
					break;
				default:
					break;
			}		
			break;			
		default:
			WM_DefaultProc(pMsg);			
	}
}

void ShowDetTestParamPwdDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

