#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "芯片类型:",       GUI_ID_TEXT0,     35, 50, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     143, 50-2, 142,  28,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     0, 100, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
static const char* DetTypeArray[] = {"XA-02", "XA-06", "XA-21"};
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 DetType = 0;
	
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

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2), &NORMAL_FONT);

			MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GetDetTypeStringForXA(UserParam.DetType));
														
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "通过方向键修改");			
							
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);					
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);
			
			ScanKeyBoard.Status = SCAN_KEYBOARD_IDLE;
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:															
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
					DetType = GetDetTypeForXA((char*)OpData.GuiTemp);				
					
					if(UserParam.DetType != DetType)
					{
						if(RESULT_NO == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}
						ClearDetInfoQueue(&DetInfoQueue);
						OpData.DetDetectorPassCount = 0;
						UserParam.DetDetectorPassCount = OpData.DetDetectorPassCount;
						UTF8StringToGBK("历史检测数量", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetDetectorPassCount) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}
						ClearChannelOldStatus();
                        DetOutsideCodeBuffer.Count = 0;
						
						UserParam.DetType = DetType;
						UTF8StringToGBK("雷管类型", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetType) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}											
					}
					if(GetWindowParam() == 0)
					{
						ShowDetTestContentMenuDlg(0);
					}
					else
					{
						ShowChannelTestPrepareDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_BACK:
					if(GetWindowParam() == 0)
					{
						ShowDetTestContentMenuDlg(0);
					}
					else
					{
						ShowChannelTestPrepareDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_F1:	
				case GUI_CUSTOM_KEY_F4:
					ShowEditArrayPrevious(OpData.CurrentInput, DetTypeArray, GET_ARRAY_LENGTH(DetTypeArray));
					break;
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F3:
					ShowEditArrayNext(OpData.CurrentInput, DetTypeArray, GET_ARRAY_LENGTH(DetTypeArray));
					break;
				default:
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

void ShowDetTestContentEnterFactorySetDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);	
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

