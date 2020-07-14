#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   " BIT测试:",       GUI_ID_TEXT0,     60, 40, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "静态电流:",       GUI_ID_TEXT1,     60, 70, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "电容充电:",       GUI_ID_TEXT2,     60, 100, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "充电电流:",       GUI_ID_TEXT3,     60, 130, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     168, 40-2, 92,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     168, 70-2, 92,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT2,     168, 100-2, 92,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT3,     168, 130-2, 92,  28,  0,          0 },
  { TEXT_CreateIndirect,   "通过方向键设置参数",       GUI_ID_TEXT4,     0, 160, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u32 test_ctrl;
	
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
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3), &NORMAL_FONT);

			if(OpData.ParamCfgSel == ENTER_FACTORY)
			{
				test_ctrl = test_param_cfg[0].test_ctrl;
			}
			else if(OpData.ParamCfgSel == SEMIFINISHED)
			{//半成品检
				test_ctrl = test_param_cfg[1].test_ctrl;
			}
			else
			{
				test_ctrl = test_param_cfg[2].test_ctrl;
			}			
			if(test_ctrl&DET_BIT_TEST_MASK_BIT)
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "检");			
			}
			else
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "不检");			
			}

			if(test_ctrl&DET_LV_CURRENT_TEST_MASK_BIT)
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "检");			
			}
			else
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "不检");			
			}
			
			if(test_ctrl&DET_CHARGE_TEST_MASK_BIT)
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "检");			
			}
			else
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "不检");			
			}
			
			if(test_ctrl&DET_CHARGE_HV_CURRENT_TEST_MASK_BIT)
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT3), "检");			
			}
			else
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT3), "不检");			
			}
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT4),GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(RESULT_YES != ShowCustomMessageBox("确定修改?", OpData.DlgTitle, MB_YESNO))
					{
						return;
					}
					if(OpData.ParamCfgSel == ENTER_FACTORY)
					{
						test_ctrl = test_param_cfg[0].test_ctrl;
					}
					else if(OpData.ParamCfgSel == SEMIFINISHED)
					{//半成品检
						test_ctrl = test_param_cfg[1].test_ctrl;
					}
					else
					{
						test_ctrl = test_param_cfg[2].test_ctrl;
					}					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));					
					if(strcmp((char*)OpData.GuiTemp, "检") == 0)
					{
						test_ctrl |= DET_BIT_TEST_MASK_BIT;
					}
					else
					{
						test_ctrl &= ~DET_BIT_TEST_MASK_BIT;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));					
					if(strcmp((char*)OpData.GuiTemp, "检") == 0)
					{
						test_ctrl |= DET_LV_CURRENT_TEST_MASK_BIT;
					}
					else
					{
						test_ctrl &= ~DET_LV_CURRENT_TEST_MASK_BIT;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));					
					if(strcmp((char*)OpData.GuiTemp, "检") == 0)
					{
						test_ctrl |= DET_CHARGE_TEST_MASK_BIT;
					}
					else
					{
						test_ctrl &= ~DET_CHARGE_TEST_MASK_BIT;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT3), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));					
					if(strcmp((char*)OpData.GuiTemp, "检") == 0)
					{
						test_ctrl |= DET_CHARGE_HV_CURRENT_TEST_MASK_BIT;
					}
					else
					{
						test_ctrl &= ~DET_CHARGE_HV_CURRENT_TEST_MASK_BIT;
					}
					if(OpData.ParamCfgSel == ENTER_FACTORY)
					{
						test_param_cfg[0].test_ctrl = test_ctrl;
					}
					else if(OpData.ParamCfgSel == SEMIFINISHED)
					{//半成品检
						test_param_cfg[1].test_ctrl = test_ctrl;
					}
					else
					{
						test_param_cfg[2].test_ctrl = test_ctrl;
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
					break;
				case GUI_CUSTOM_KEY_BACK:
					ShowDetTestParamMenuDlg();
					break;
				case GUI_CUSTOM_KEY_F1:	
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					}					
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
					}										
					ShowInputCursor(OpData.CurrentInput, 1);					
					break;					
				case GUI_CUSTOM_KEY_F3:	
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
					}					
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					}
					ShowInputCursor(OpData.CurrentInput, 1);					
					break;
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F4:
					ShowInputCursor(OpData.CurrentInput, 0);
					MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
					if(strcmp((char*)OpData.GuiTemp, "检") == 0)
					{
						MULTIEDIT_SetText(OpData.CurrentInput, "不检");
					}
					else if(strcmp((char*)OpData.GuiTemp, "不检") == 0)
					{
						MULTIEDIT_SetText(OpData.CurrentInput, "检");
					}
					ShowInputCursor(OpData.CurrentInput, 1);
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

void ShowDetTestMaskBitSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

