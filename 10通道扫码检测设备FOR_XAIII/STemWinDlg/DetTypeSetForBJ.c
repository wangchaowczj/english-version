#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.VA-M",       GUI_ID_TEXT0,     100, 75, 210,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.VA-T",       GUI_ID_TEXT1,     100, 105, 210,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
    u8 value;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();

			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("雷管类型");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));

            RADIO_CreateEx(80,80,15,60,hWin,WM_CF_SHOW,0,GUI_ID_RADIO0,2,30);

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_TOP);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_TOP);
            
            switch(UserParam.DetType)
            {
                case DET_TYPE_BJ_M:
                    RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
                    break;            
                case DET_TYPE_BJ_T:
                    RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
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
								UserParam.DetType = DET_TYPE_BJ_M;
								break;
							case 1:
								UserParam.DetType = DET_TYPE_BJ_T;
								break;
						}
						UTF8StringToGBK("雷管类型", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetType) == ERROR)
						{
							if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
							{
								return;
							}
						}
                        
						OpData.DetDetectorPassCount = 0;
						ClearChannelOldStatus();
                        if(GetWindowParam() == 0)
                        {
                            ShowSystemSetDlg();
                        }
                        else
                        {
                            BarCode.Status = BARCODE_IDLE;
                            ClearScanKeyBoard();
                            ShowChannelTestPrepareDlg();
                        }														                        
                    }
                    break;
				case GUI_CUSTOM_KEY_BACK:
                    value = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
                    if(value == 0)
                    {
                        value = DET_TYPE_BJ_M;
                    }
                    else if(value == 1)
                    {
                        value = DET_TYPE_BJ_T;
                    }
                    else
                    {
                        break;
                    }
					if(UserParam.DetType != value)
					{
						if(RESULT_YES != ShowCustomMessageBox("确认取消修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}					
					}

                    if(GetWindowParam() == 0)
                    {
                        ShowSystemSetDlg();
                    }
                    else
                    {
                        BarCode.Status = BARCODE_IDLE;
                        ClearScanKeyBoard();
                        ShowChannelTestPrepareDlg();
                    }
					break;	
				case GUI_CUSTOM_KEY_1:			
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
					break;					
				case GUI_CUSTOM_KEY_2:				
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
					break;			
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTypeSetForBJDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

