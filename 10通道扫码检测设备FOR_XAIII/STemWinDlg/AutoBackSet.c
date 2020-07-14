#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.手动返回",       GUI_ID_TEXT0,     105, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.自动返回",       GUI_ID_TEXT1,     105, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "设置雷管异常时的处理方式",       GUI_ID_TEXT2,     0, 120, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "仅在单通道检测时有效",       GUI_ID_TEXT3,     0, 150, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("异常处理");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), GUI_RED);
            
			RADIO_CreateEx(80,55,200,100,hWin,WM_CF_SHOW,0,GUI_ID_RADIO0,2,30);

			if(UserParam.AutoBack == 0)
			{
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);				
			}
            else
            {
                RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
            }
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(RESULT_YES == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
					{
                        UserParam.AutoBack = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
												
						UTF8StringToGBK("自动返回", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.AutoBack) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
						}						
						ShowSystemSetDlg();					
					}
					break;
				case GUI_CUSTOM_KEY_BACK:
					if(UserParam.AutoBack != (USE_STAGE_CFG)RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
					{
						if(RESULT_YES != ShowCustomMessageBox("确认取消修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}					
					}

					ShowSystemSetDlg();
					break;
				case GUI_CUSTOM_KEY_1:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
					break;
				case GUI_CUSTOM_KEY_2:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
					break;							
			}
			WM_SetFocus(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowAutoBackSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

