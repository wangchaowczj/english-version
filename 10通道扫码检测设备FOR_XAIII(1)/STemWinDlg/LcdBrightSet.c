#include "includes.h"
#include "GuiIncludes.h"


/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON0,     1, 1, 10, 10,  0,          0 },
	{ SLIDER_CreateIndirect, "", GUI_ID_SLIDER0,     20, 90, 280, 20,  0,          0 },
	{ TEXT_CreateIndirect, "方向键改变亮度", GUI_ID_TEXT0,     00, 120, 320, 30,  0,          0 }
};

#define LCD_BRIGHT_RANK 32

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
			WindowTitleUpdate("亮度设置");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
				
			if(UserParam.LcdBright == 0)
			{
				UserParam.LcdBright = 1;
			}
			OpData.DlgCount = UserParam.LcdBright;
			
			LcdSetBright(UserParam.LcdBright);
			SLIDER_SetRange(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 1, LCD_BRIGHT_RANK);
			SLIDER_SetNumTicks(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 2);
			SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), LCD_BRIGHT_RANK-OpData.DlgCount+1);							
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(RESULT_YES == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
					{
						UserParam.LcdBright = OpData.DlgCount;
						LcdSetBright(UserParam.LcdBright);

						UTF8StringToGBK("屏幕亮度", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, (u8*)OpData.GuiTemp, UserParam.LcdBright) == ERROR)
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
					if(UserParam.LcdBright != OpData.DlgCount)
					{
						if(RESULT_YES != ShowCustomMessageBox("确认取消修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}					
					}
					LcdSetBright(UserParam.LcdBright);
					ShowSystemSetDlg();						
					break;
				case GUI_CUSTOM_KEY_F1:
					if(OpData.DlgCount < LCD_BRIGHT_RANK)
					{
						OpData.DlgCount++;
					}

					SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), LCD_BRIGHT_RANK-OpData.DlgCount+1);
					LcdSetBright(OpData.DlgCount);
					
					WM_SetFocus(WM_GetClientWindow(hWin));
					break;
				case GUI_CUSTOM_KEY_F4:
					OpData.DlgCount += 2;
					if(OpData.DlgCount > LCD_BRIGHT_RANK)
					{
						OpData.DlgCount = LCD_BRIGHT_RANK;
					}

					SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), LCD_BRIGHT_RANK-OpData.DlgCount+1);
					LcdSetBright(OpData.DlgCount);
					
					WM_SetFocus(WM_GetClientWindow(hWin));				
					break;					
				case GUI_CUSTOM_KEY_F3:
					if(OpData.DlgCount > 1)
					{
						OpData.DlgCount--;
					}

					SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), LCD_BRIGHT_RANK-OpData.DlgCount+1);				
					LcdSetBright(OpData.DlgCount);
					
					WM_SetFocus(WM_GetClientWindow(hWin));
					break;
				case GUI_CUSTOM_KEY_F2:
					if(OpData.DlgCount > 2)
					{
						OpData.DlgCount -= 2;
					}
					else
					{
						OpData.DlgCount = 1;
					}
					SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), LCD_BRIGHT_RANK-OpData.DlgCount+1);	
					LcdSetBright(OpData.DlgCount);
					
					WM_SetFocus(WM_GetClientWindow(hWin));
					break;
				default:
					break;
			}
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowLcdBrightSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

