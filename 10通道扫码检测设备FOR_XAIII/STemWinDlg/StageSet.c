#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.入厂检测",       GUI_ID_TEXT0,     75, 40, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.半成品检测",       GUI_ID_TEXT1,     75, 70, LCD_WIDTH_PIXEL,  25,  0,          0 },  
  { TEXT_CreateIndirect,   "3.成品检测",       GUI_ID_TEXT2,     75, 100, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "4.内码验证",       GUI_ID_TEXT3,     75, 130, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT4,     75, 160, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("检测环节");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	
			RADIO_CreateEx(55,45,200,150,hWin,WM_CF_SHOW,0,GUI_ID_RADIO0,4,30);	
		
            switch(UserParam.Stage)
            {
                case ENTER_FACTORY:
                    RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
                    break;
                case SEMIFINISHED:
                    RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
                    break;
                case FINISHED:
                    RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
                    break;
                case DET_CODE_VERIFY:
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
								UserParam.Stage = ENTER_FACTORY;
								break;
							case 1:
								UserParam.Stage = SEMIFINISHED;
								break;
							case 2:
								UserParam.Stage = FINISHED;
								break;
							case 3:
                                UserParam.Stage = DET_CODE_VERIFY;						
								break;
						}
						
						DetTestParamInit();
						
						UTF8StringToGBK("检测环节", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.Stage) == ERROR)
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
					if(UserParam.Stage != (USE_STAGE_CFG)RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
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

void ShowStageSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

