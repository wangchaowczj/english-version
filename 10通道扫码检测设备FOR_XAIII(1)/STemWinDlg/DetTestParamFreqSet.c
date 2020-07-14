#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "频率最小值(Hz):",       GUI_ID_TEXT0,     20, 60, 180,  25,  0,          0 },
  { TEXT_CreateIndirect,   "频率最大值(Hz):",       GUI_ID_TEXT1,     20, 90, 180,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     200, 60-2, 100,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     200, 90-2, 100,  28,  0,          0 },
  { TEXT_CreateIndirect,   "上下键切换,左右键回删",       GUI_ID_TEXT2,     0, 120, LCD_WIDTH_PIXEL,  25,  0,          0 }
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
			WindowTitleUpdate("芯片频率");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &NORMAL_FONT);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 6);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 6);
			if(OpData.ParamCfgSel == ENTER_FACTORY)
			{
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[0].det_freq_min);
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[0].det_freq_max);				
			}
			else if(OpData.ParamCfgSel == SEMIFINISHED)
			{//半成品检
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[1].det_freq_min);
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[1].det_freq_max);				
			}
			else
			{
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[2].det_freq_min);
				InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[2].det_freq_max);									
			}
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2),GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
					{
						u32 Min, Max;
						ShowInputCursor(OpData.CurrentInput, 0);
						if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) <= 1)
						{
							ShowCustomMessageBox("请输入最小值!", OpData.DlgTitle, MB_OK);
							return;							
						}
						if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT1)) <= 1)
						{
							ShowCustomMessageBox("请输入最大值!", OpData.DlgTitle, MB_OK);
							return;							
						}						
														
						Min = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
						Max = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
						if (Min > Max)
						{
							ShowCustomMessageBox("最小值不能大于最大值!", OpData.DlgTitle, MB_OK);
							return;
						}
						if(Min < 50000UL)
						{
							ShowCustomMessageBox("最小值不能小于50KHz!", OpData.DlgTitle, MB_OK);
							return;							
						}
						else if(Max > 120000UL)
						{
							ShowCustomMessageBox("最大值不能大于120KHz!", OpData.DlgTitle, MB_OK);
							return;							
						}
						if(RESULT_YES != ShowCustomMessageBox("确定修改?", OpData.DlgTitle, MB_YESNO))
						{
							return;
						}
						if(OpData.ParamCfgSel == ENTER_FACTORY)
						{
							test_param_cfg[0].det_freq_min = Min;
							test_param_cfg[0].det_freq_max = Max;								
						}						
						else if(OpData.ParamCfgSel == SEMIFINISHED)
						{//半成品检
							test_param_cfg[1].det_freq_min = Min;
							test_param_cfg[1].det_freq_max = Max;								
						}
						else
						{
							test_param_cfg[2].det_freq_min = Min;
							test_param_cfg[2].det_freq_max = Max;																	
						}								
						if(UserParam.Stage == ENTER_FACTORY)
						{
							memcpy((char*)&test_param, (char*)&test_param_cfg[0], sizeof(TEST_PARAM));
						}
						else if(UserParam.Stage == SEMIFINISHED)
						{
							memcpy((char*)&test_param, (char*)&test_param_cfg[1], sizeof(TEST_PARAM));
						}
						else
						{
							memcpy((char*)&test_param, (char*)&test_param_cfg[2], sizeof(TEST_PARAM));
						}
						if(ERROR == SaveDetTestParamCfg())
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							return;
						}
						ShowDetTestParamMenuDlg();												
					}					
					break;
				case GUI_CUSTOM_KEY_BACK:
					ShowDetTestParamMenuDlg();
					break;
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F3:
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					}					
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
					ShowInputCursor(OpData.CurrentInput, 1);
					break;
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F4:					
					if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
					{
						ShowInputCursor(OpData.CurrentInput, 0);
						MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						ShowInputCursor(OpData.CurrentInput, 1);
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

void ShowDetTestParamFreqSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

