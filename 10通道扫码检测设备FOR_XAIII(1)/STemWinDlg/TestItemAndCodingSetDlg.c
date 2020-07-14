#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "内码设置:",       GUI_ID_TEXT0,     35, 30, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "芯片类型:",       GUI_ID_TEXT1,     35, 60, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "延期设置:",       GUI_ID_TEXT2,     35, 90, 108,  25,  0,          0 },
  { TEXT_CreateIndirect,   "延期(ms):",       GUI_ID_TEXT3,     35, 120, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     143, 30-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     143, 60-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT2,     143, 90-2, 142,  28,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT3,     143, 120-2, 142,  28,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT5,     0, 150, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
static const char* DetCodeWriteArray[] = {"写", "不写检验", "不写不检"};
static const char* DetTypeArray[] = {"XA-02", "XA-06", "XA-21"};
static const char* DelaySetArray[] = {"写", "不写检验", "不写不检"};

static void ShowOrHideDelayInput(WM_HWIN hWin)
{
	u8 DetType = 0;
	
	MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
	if((strcmp((char*)OpData.GuiTemp, "写") == 0) && (UserParam.NetMode == 0))
	{//不联网模式下，需要写内码时，通过扫码进行设置
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));	
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
        
        return; 
	}    

    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
    
    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
    DetType = GetDetTypeForXA((char*)OpData.GuiTemp);

    if(XA2_DetIsFixDelay(DetType))
    {//固定延期
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));

        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
        if(strcmp((char*)OpData.GuiTemp, "不写不检") == 0)
        {
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));            				
        }
        else
        {
            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
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
static void ShowHint(WM_HWIN hWin)
{
    if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
    {
        MULTIEDIT_GetText(OpData.CurrentInput, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
        if ((strcmp((char*)OpData.GuiTemp, "写") == 0) && (UserParam.NetMode == 0))
        {
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), "通过扫码方式设置其余参数");	
            return;          							
        }				
    }				
    else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
    {
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), "数字键录入,左右键回删");
        return;
    }

    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), "通过方向键设置参数");
}	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 DetType = 0;
	u8 DetCodeWrite;
	u8 DelayWrite;	
	u8 CheckDelay;
	u8 CheckDetCode;
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
            
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 5);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 6);
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT5), GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
			if((0 == UserParam.DetCodeWrite)&&(UserParam.CheckDetCode!=0))
			{//允许手动设置雷管类型和延期值
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "不写检验");																				
			}
			else if((0 == UserParam.DetCodeWrite)&&(UserParam.CheckDetCode==0))
			{
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "不写不检");	
			}
			else if(UserParam.DetCodeWrite!=0)
			{//通过扫码方式设置
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "写");	
			}	

			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			
			MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), GetDetTypeStringForXA(UserParam.DetType));
			
			if(0 == UserParam.DelayWrite)
			{//是否写延期值
                if(UserParam.CheckDelay)
                {
                    MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "不写检验");
                }
                else
                {
                    MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "不写不检");
                }								
			}
			else
			{//
				MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), "写");	
			}
            
            InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3), UserParam.DetDelay);           										
					
			ShowOrHideDelayInput(hWin);
			ShowHint(hWin);
            
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);			
            
			ScanKeyBoard.Status = SCAN_KEYBOARD_IDLE;
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:										
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
					if(strcmp((char*)OpData.GuiTemp, "写") == 0)
					{
						DetCodeWrite = 1;
                        CheckDetCode = 0;
                        if(UserParam.NetMode == 0)
                        {
                            break;//不联网时，强制要求扫码设置
                        }						
					}
					else if(strcmp((char*)OpData.GuiTemp, "不写不检") == 0)
					{
						DetCodeWrite = 0;
						CheckDetCode = 0;
					}
					else if(strcmp((char*)OpData.GuiTemp, "不写检验") == 0)
					{
						DetCodeWrite = 0;
						CheckDetCode = 1;
					}
                    
                    DelayWrite = 0;
                    CheckDelay = 0;
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
                    DetType = GetDetTypeForXA((char*)OpData.GuiTemp);
					if(XA2_DetIsFixDelay(DetType))
					{//固定延期
                        Delay = UserParam.DetDelay;
                        
						MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
						if(strcmp((char*)OpData.GuiTemp, "写") == 0)
						{
							DelayWrite = 1;
						}
						else if(strcmp((char*)OpData.GuiTemp, "不写检验") == 0)
						{								
                            CheckDelay = 1;
						}
                        else if(strcmp((char*)OpData.GuiTemp, "不写不检") == 0)
                        {
                            ;
                        }
                        if(DelayWrite || CheckDelay)
                        {
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
					}
					else
					{//双电容芯片不需要设置延期值
						Delay = 0;				
					}					
					
					if((UserParam.DetType != DetType) || (UserParam.DetCodeWrite != DetCodeWrite) || 
						(UserParam.DelayWrite != DelayWrite) || (UserParam.DetDelay != Delay)||
					(UserParam.CheckDelay != CheckDelay)||(UserParam.CheckDetCode!=CheckDetCode))
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
						UserParam.CheckDelay = CheckDelay;
						UTF8StringToGBK("延期检测", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.CheckDelay) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
							break;
						}
						UserParam.CheckDetCode = CheckDetCode;
						UTF8StringToGBK("内码检验", (char*)OpData.GuiTemp);						
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.CheckDelay) == ERROR)
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
					ShowInputCursor(OpData.CurrentInput, 0);
				    if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{//延期
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT2)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);							
						}				
					}						
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);							
						}
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
						{
							OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);							
						}
					}																
					ShowInputCursor(OpData.CurrentInput, 1);
					break;
				case GUI_CUSTOM_KEY_F2:
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
                        ShowEditArrayNext(OpData.CurrentInput, DetCodeWriteArray, GET_ARRAY_LENGTH(DetCodeWriteArray)); 						
					}				
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						ShowEditArrayNext(OpData.CurrentInput, DetTypeArray, GET_ARRAY_LENGTH(DetTypeArray)); 
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						ShowEditArrayNext(OpData.CurrentInput, DelaySetArray, GET_ARRAY_LENGTH(DelaySetArray)); 			
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}					
					}
                    ShowOrHideDelayInput(hWin);
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
						ShowEditArrayPrevious(OpData.CurrentInput, DetCodeWriteArray, GET_ARRAY_LENGTH(DetCodeWriteArray));						
					}				
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						ShowEditArrayPrevious(OpData.CurrentInput, DetTypeArray, GET_ARRAY_LENGTH(DetTypeArray));
					}	
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						ShowEditArrayPrevious(OpData.CurrentInput, DelaySetArray, GET_ARRAY_LENGTH(DelaySetArray)); 					
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}					
					}
                    ShowOrHideDelayInput(hWin);
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
						if ((strcmp((char*)OpData.GuiTemp, "写") == 0) && (UserParam.NetMode == 0))
						{
							ClearDetInfoQueue(&DetInfoQueue);
//							OpData.DetDetectorPassCount = 0;

							ClearChannelOldStatus();
							DetOutsideCodeBuffer.Count = 0;
							
							BarCodeDataAnalyst();//分析数据
							ClearScanKeyBoard();
							
							ShowDetTestContentModifyDlg(GetWindowParam());
							break;						
						}										
						ClearScanKeyBoard();
					}
					
					ShowHint(hWin);//根据当前输入状态进行提示
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

void ShowTestItemAndCodingSetDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}
