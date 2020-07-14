#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     28, 40, 290,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     28, 70, 290,  25,  0,          0 },
  { TEXT_CreateIndirect,   "新管码后5位:",       GUI_ID_TEXT2,     28, 100, 144,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     172, 100-2, 122,  28,  0,          0 },
  { TEXT_CreateIndirect,   "方向键回删",       GUI_ID_TEXT3,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u32 U32Value;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("管码设置");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 5);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

            if(GetCurrentDetOutsideStart(OpData.GuiTemp) != ERROR)
            {
                memset(OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
                strcpy((char*)OpData.GuiTemp, "起始管码:");
                GetCurrentDetOutsideStart(&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)]);
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
                

                sprintf((char*)OpData.GuiTemp, "盒内数量:%d", UserParam.InBoxNumber);
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
            }
				
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);	
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
                    if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) != 6)
                    {
                        ShowCustomMessageBox("请输入5位数字!", OpData.DlgTitle, MB_OK);
                        break;
                    }
                    U32Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));//输入的盒内流水号
                    
                    if((U32Value%100) >= UserParam.InBoxNumber)
                    {
                        ShowCustomMessageBox("超出盒内数量!", OpData.DlgTitle, MB_OK);
                        break; 
                    }
                    if ((Channel.Count == 10) && ((U32Value%10) != 0))
                    {
                        ShowCustomMessageBox("10通道模式,最后1位必需是0", OpData.DlgTitle, MB_OK);
                        break;  
                    }
                    UserParam.DetOutsideStartBoxIndex = U32Value;
                    
                    UTF8StringToGBK("起始管码后5位", (char*)OpData.GuiTemp);
                    if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetOutsideStartBoxIndex) == ERROR)
                    {
                        if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
                        {
                            return;
                        }
                    }
                    ShowCustomMessageBox("保存成功!", OpData.DlgTitle, MB_OK);
//					break;//和BACK按键相同
				case GUI_CUSTOM_KEY_BACK:
                    if(GetWindowParam() == 1)
                    {
                        ShowChannelTestPrepareDlg();
                    }
                    else
                    {
                        ShowSystemSetDlg();
                    }										
					break;
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F4:
					if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
					{						
						MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
					}					
					break;
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F3:
                    ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{						
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
                    else
                    {
                        OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
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
					ShowInputCursor(OpData.CurrentInput, 0);
					MULTIEDIT_AddKey(OpData.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);

					if(WM_IsWindow(OpData.CurrentInput))
					{						
						ShowCursor(OpData.CurrentInput);
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

void ShowDetOutsideBoxIndexForBJDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

