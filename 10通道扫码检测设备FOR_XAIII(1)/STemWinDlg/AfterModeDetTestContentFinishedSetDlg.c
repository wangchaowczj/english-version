#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "内码写入:",       GUI_ID_TEXT0,     35, 30, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "芯片类型:",       GUI_ID_TEXT1,     35, 60, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "延期写入:",       GUI_ID_TEXT2,     35, 90, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "延期(ms):",       GUI_ID_TEXT3,     35, 120, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     143, 30-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     143, 60-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT2,     143, 90-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT3,     143, 120-2, 142,  28,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT4,     0, 150, LCD_WIDTH_PIXEL,  25,  0,          0 }
};


static void ShowOrHideDelayInput(WM_HWIN hWin)
{
	u8 DetType = 0;
	
	MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
	if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
	{
		WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
		WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
		
		MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
		if(strcmp((char*)OpData.GuiTemp, "XA-BD1") == 0)
		{
			DetType = DET_TYPE_XA_BD1;
		}
		else if(strcmp((char*)OpData.GuiTemp, "XA-BD2") == 0)
		{
			DetType = DET_TYPE_XA_BD2;
		}		
		else if(strcmp((char*)OpData.GuiTemp, "XA-PD1") == 0)
		{
			DetType = DET_TYPE_XA_PD1;
		}
		else if(strcmp((char*)OpData.GuiTemp, "XA-PD2") == 0)
		{
			DetType = DET_TYPE_XA_PD2;
		}
		else if(strcmp((char*)OpData.GuiTemp, "XA-PD3") == 0)
		{
			DetType = DET_TYPE_XA_PD3;
		}

		if(XA_DetIsFixDelay(DetType))
		{//固定延期
			WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
			WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));

			MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
			if(strcmp((char*)OpData.GuiTemp, "写") == 0)
			{
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
			}
			else
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
			}			
		}
		else
		{//在线可编程
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));				
		}			
	}
	else
	{
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));	
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
	}		
}	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 DetType = 0;
	u8 DetCodeWrite;
	u8 DelayWrite;
	u32 Delay;
	
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
					
			if(0 == UserParam.DetCodeWrite)
			{//允许手动设置雷管类型和延期值
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "不写");
				
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过方向键设置参数");																				
			}
			else
			{//通过扫码方式设置
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "写");	
				
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过扫码方式设置其余参数");
			}	

			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			
			switch(UserParam.DetType)
			{
				case DET_TYPE_XA_BD1:
					MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "XA-BD1");					
					break;
				case DET_TYPE_XA_BD2:
					MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "XA-BD2");					
					break;					
				case DET_TYPE_XA_PD1:
					MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "XA-PD1");					
					break;
				case DET_TYPE_XA_PD2:
					MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "XA-PD2");
					break;
				case DET_TYPE_XA_PD3:
					MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "XA-PD3");
					break;					
			}
			
			if(0 == UserParam.DelayWrite)
			{//是否写延期值
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "不写");																			
			}
			else
			{//
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "写");	
			}	
			
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3), UserParam.DetDelay);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 5);
			
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT4), GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
					
			ShowOrHideDelayInput(hWin);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);
			
			ScanKeyBoard.Status = SCAN_KEYBOARD_IDLE;
			break;

			//这个是接收键盘上的按键的相应
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:										
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
					if(strcmp((char*)OpData.GuiTemp, "写") == 0)
					{
//						DetCodeWrite = 1;
						break;//强制要求扫码设置
					}
					else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
					{
						DetCodeWrite = 0;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
					if(strcmp((char*)OpData.GuiTemp, "XA-BD1") == 0)
					{
						DetType = DET_TYPE_XA_BD1;
					}
					else if(strcmp((char*)OpData.GuiTemp, "XA-BD2") == 0)
					{
						DetType = DET_TYPE_XA_BD2;
					}					
					else if(strcmp((char*)OpData.GuiTemp, "XA-PD1") == 0)
					{
						DetType = DET_TYPE_XA_PD1;
					}
					else if(strcmp((char*)OpData.GuiTemp, "XA-PD2") == 0)
					{
						DetType = DET_TYPE_XA_PD2;
					}
					else if(strcmp((char*)OpData.GuiTemp, "XA-PD3") == 0)
					{
						DetType = DET_TYPE_XA_PD3;
					}
					if(XA_DetIsFixDelay(DetType))
					{//固定延期
						MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							DelayWrite = 1;
							
							MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT3), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));	
							if(strlen((char*)OpData.GuiTemp) == 0)
							{
								ShowCustomMessageBox("请输入延期!", OpData.DlgTitle, MB_OK);
								break;
							}
							Delay = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3));					
							if (Delay > GetMaxDelay(DetType))
							{
								ShowCustomMessageBox("延期值超出范围!", OpData.DlgTitle, MB_OK);
								break;
							}	
						}
						else
						{
							DelayWrite = 0;
							Delay = UserParam.DetDelay;
						}						
					}
					else
					{//双电容芯片不需要设置延期值
						DelayWrite = 0;
						Delay = 0;				
					}					
					
					if((UserParam.DetType != DetType) || (UserParam.DetCodeWrite != DetCodeWrite) || 
						(UserParam.DelayWrite != DelayWrite) || (UserParam.DetDelay != Delay))
					{
						if(RESULT_NO == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}
//						ClearDetInfoQueue(&DetInfoQueue);
						OpData.DetDetectorPassCount = 0;
						ClearChannelOldStatus();
//						DetOutsideCodeBuffer.Count = 0;
						
						UserParam.DetType = DetType;
						UTF8StringToGBK("雷管类型", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetType) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}						
						
						UserParam.DetCodeWrite = DetCodeWrite;
						UTF8StringToGBK("内码写入", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetCodeWrite) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}
						
						UserParam.DelayWrite = DelayWrite;
						UTF8StringToGBK("延期写入", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DelayWrite) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}
						
						UserParam.DetDelay = Delay;
						UTF8StringToGBK("雷管延期", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetDelay) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}						
					}
					if(GetWindowParam() == 0)
					{
						ShowDetTestContentMenuDlg();
						SetWindowParam(0);
					}
					else
					{
						ShowChannelTestPrepareFigureDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_BACK:
					if(GetWindowParam() == 0)
					{
						ShowDetTestContentMenuDlg();
						SetWindowParam(0);
					}
					else
					{
						ShowChannelTestPrepareFigureDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_F1:	
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);							
						}
					}					
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);							
						}
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT2)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);							
						}
					}																
					ShowInputCursor(OpData.CurrentInput, 1);
					break;					
				case GUI_CUSTOM_KEY_F3:	
					ShowInputCursor(OpData.CurrentInput, 0);
						
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);							
						}
					}						
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT2)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);	
						}
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT3)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);	
							ClearUserDataToMULTIEDIT(OpData.CurrentInput);
						}
					}															
					ShowInputCursor(OpData.CurrentInput, 1);
					break;
				
				case GUI_CUSTOM_KEY_F4:
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "不写");

							ShowOrHideDelayInput(hWin);								
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "写");
							
							ShowOrHideDelayInput(hWin);	
						}					
					}				
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "XA-BD1") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD3");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-BD2") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-BD1");
						}						
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD1") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-BD2");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD2") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD1");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD3") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD2");
						}
						ShowOrHideDelayInput(hWin);
					}	
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "不写");

							ShowOrHideDelayInput(hWin);								
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "写");
							
							ShowOrHideDelayInput(hWin);	
						}					
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}					
					}					
					break;
				case GUI_CUSTOM_KEY_F2:
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "不写");

							ShowOrHideDelayInput(hWin);							
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "写");

							ShowOrHideDelayInput(hWin);							
						}					
					}				
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "XA-BD1") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-BD2");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-BD2") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD1");
						}						
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD1") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD2");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD2") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-PD3");
						}
						else if(strcmp((char*)OpData.GuiTemp, "XA-PD3") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "XA-BD1");
						}
						ShowOrHideDelayInput(hWin);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "不写");

							ShowOrHideDelayInput(hWin);								
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
						{
							MULTIEDIT_SetText(OpData.CurrentInput, "写");
							
							ShowOrHideDelayInput(hWin);	
						}					
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}					
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
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{//延期
						ShowInputCursor(OpData.CurrentInput, 0);
						MULTIEDIT_AddKey(OpData.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);					
					}
					break;						
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);	
					if(ScanKeyBoard.Status == SCAN_KEYBOARD_END)
					{//已经扫到数据
						MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
//							ClearDetInfoQueue(&DetInfoQueue);
							OpData.DetDetectorPassCount = 0;
//							DetOutsideCodeBuffer.Count = 0;
							ClearChannelOldStatus();
							
							BarCodeDataAnalyst();//分析数据
							ClearScanKeyBoard();
							
							ShowDetTestContentModifyDlg();
							break;						
						}										
						ClearScanKeyBoard();
					}
					//根据当前输入状态进行提示
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过扫码方式设置其余参数");							
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过方向键设置参数");							
						}					
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过方向键设置参数");
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "通过方向键设置参数");
					}					
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "数字键录入,	左右键回删");
					}
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

void ShowAfterModeDetTestContentFinishedSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

