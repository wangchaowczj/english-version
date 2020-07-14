#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.完成响",       GUI_ID_TEXT0,     105, 45, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.不响",       GUI_ID_TEXT1,     105, 75, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.全部合格响",       GUI_ID_TEXT2,     105, 105, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "4.有不合格响",       GUI_ID_TEXT3,     105, 135, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("声音设置");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	
			
			RADIO_CreateEx(85,50,15,150,hWin,WM_CF_SHOW,0,GUI_ID_RADIO0,4,30);
			
			switch(UserParam.Sound)
			{
				case FINISHED_RING:
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
					break;
				case NOT_RING:
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
					break;
				case RIGHT_RING:
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
					break;
				case ERROR_RING:
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 3);
					break;								
			}					
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(RESULT_YES == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
					{
						switch(RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
						{
							case 0:
								UserParam.Sound = FINISHED_RING;
								break;
							case 1:
								UserParam.Sound = NOT_RING;
								break;
							case 2:
								UserParam.Sound = RIGHT_RING;
								break;
							case 3:
								UserParam.Sound = ERROR_RING;
								break;								
						}

						UTF8StringToGBK("声音", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.Sound) == ERROR)
						{
							if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
							{
								return;
							}
						}						
						ShowSystemSetDlg();					
					}
					break;
				case GUI_CUSTOM_KEY_BACK:					
					if(UserParam.Sound != (SOUND_CFG)RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
					{
						if(RESULT_YES != ShowCustomMessageBox("确认取消修改?", OpData.DlgTitle, MB_YESNO))
						{
							return;
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
				case GUI_CUSTOM_KEY_3:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
					break;
				case GUI_CUSTOM_KEY_4:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 3);
					break;					
			}
			WM_SetFocus(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowSoundSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

