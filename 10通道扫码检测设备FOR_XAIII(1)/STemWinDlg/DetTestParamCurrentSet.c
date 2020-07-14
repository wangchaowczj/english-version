#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "正向电流最小值(uA):",       GUI_ID_TEXT0,     15, 60, 228,  25,  0,          0 },
  { TEXT_CreateIndirect,   "正向电流最大值(uA):",       GUI_ID_TEXT1,     15, 90, 228,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     243, 60-2, 62,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     243, 90-2, 62,  28,  0,          0 },
  { TEXT_CreateIndirect,   "上下键切换,左右键回删",       GUI_ID_TEXT2,     0, 120, LCD_WIDTH_PIXEL,  25,  0,          0 }
};


/**
  *************************************************************************************
  * @param  Param 对话框参数
  *         @arg @b 0 静态电流
  *         @arg @b 1 充电后电流
  *************************************************************************************  
  */

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
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &NORMAL_FONT);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 2);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 2);
			
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2),GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);		
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
					{
						u8 Min, Max;
						
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
						
						if(RESULT_YES != ShowCustomMessageBox("确定修改?", OpData.DlgTitle, MB_YESNO))
						{
							return;
						}
						
						if(GetWindowParam() == 0)
						{
							if(OpData.ParamCfgSel == ENTER_FACTORY)
							{//入厂检
								test_param_cfg[0].det_lv_current_min = Min*100;
								test_param_cfg[0].det_lv_current_max = Max*100;				
							}						
							else if(OpData.ParamCfgSel == SEMIFINISHED)
							{//半成品检
								test_param_cfg[1].det_lv_current_min = Min*100;
								test_param_cfg[1].det_lv_current_max = Max*100;				
							}
							else
							{
								test_param_cfg[2].det_lv_current_min = Min*100;
								test_param_cfg[2].det_lv_current_max = Max*100;								
							}							
						}
						else
						{
							if(OpData.ParamCfgSel == ENTER_FACTORY)
							{//入厂检
								test_param_cfg[0].det_hv_current_min = Min*100;
								test_param_cfg[0].det_hv_current_max = Max*100;				
							}						
							else if(OpData.ParamCfgSel == SEMIFINISHED)
							{//半成品检
								test_param_cfg[1].det_hv_current_min = Min*100;
								test_param_cfg[1].det_hv_current_max = Max*100;				
							}
							else
							{
								test_param_cfg[2].det_hv_current_min = Min*100;
								test_param_cfg[2].det_hv_current_max = Max*100;							
							}								
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
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					if(GetWindowParam() == 0)
					{//静态电流
						WindowTitleUpdate("静态电流");
						if(OpData.ParamCfgSel == ENTER_FACTORY)
						{
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[0].det_lv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[0].det_lv_current_max/100);						
						}
						else if(OpData.ParamCfgSel == SEMIFINISHED)
						{//半成品检
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[1].det_lv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[1].det_lv_current_max/100);				
						}
						else
						{
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[2].det_lv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[2].det_lv_current_max/100);					
						}		
					}
					else
					{//充电电流
						WindowTitleUpdate("充电电流");
						if(OpData.ParamCfgSel == ENTER_FACTORY)
						{
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[0].det_hv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[0].det_hv_current_max/100);
						}
						else if(OpData.ParamCfgSel == SEMIFINISHED)
						{//半成品检
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[1].det_hv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[1].det_hv_current_max/100);					
						}
						else
						{
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), test_param_cfg[2].det_hv_current_min/100);
							InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), test_param_cfg[2].det_hv_current_max/100);					
						}								
					}
					OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					break;
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

void ShowDetTestParamCurrentSetDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

