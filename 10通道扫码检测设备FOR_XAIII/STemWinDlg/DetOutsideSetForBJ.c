#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "起始管码:",       GUI_ID_TEXT0,     20, 60, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     128, 60-2, 170,  28,  0,          0 },
  { TEXT_CreateIndirect,   "盒内数量:",       GUI_ID_TEXT1,     20, 95, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     128, 95-2, 170,  28,  0,          0 },
  { TEXT_CreateIndirect,   "上下键切换,左键回删",       GUI_ID_TEXT2,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "右键选择字母(仅特征号位)",       GUI_ID_TEXT3,     0, 170, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
	
static 	u8 LetterIndex = 0;
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
    u8 U8Value;
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
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &NORMAL_FONT);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 13);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 3);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            if(GetCurrentDetOutsideStart(OpData.GuiTemp) != ERROR)
            {
                InitUserStringToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, 13);         
            }
            if ((UserParam.InBoxNumber >= 1) && (UserParam.InBoxNumber <= 100)  && (((UserParam.InBoxNumber%5) == 0) || (UserParam.InBoxNumber == 1)))
            {
                InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), UserParam.InBoxNumber);
            }		
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);	
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
                    if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) != 14)
                    {
                        ShowCustomMessageBox("请输入13位起始管码!", OpData.DlgTitle, MB_OK);
                        break;
                    }
                    if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT1)) <= 1)
                    {
                        ShowCustomMessageBox("请输入盒内数量!", OpData.DlgTitle, MB_OK);
                        break;                        
                    }
                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
                    //判断输入的管码是否非法
                    if(DetOutsideVerify(OpData.GuiTemp) == ERROR)
                    {
                        ShowCustomMessageBox("请输入正确的起始管码!", OpData.DlgTitle, MB_OK);
                        break;
                    }
                    U8Value = GetInBoxIndexFromDetOutside(OpData.GuiTemp);//盒内流水号
                    if ((Channel.Count == 10) && ((U8Value%10) != 0))
                    {
                        ShowCustomMessageBox("10通道模式,最后1位必需是0", OpData.DlgTitle, MB_OK);
                        break;  
                    }
					
					U32Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));//输入的盒内数量
					if ((Channel.Count == 5) && ((U8Value%5) != 0) && (U32Value > 5))
                    {
                        ShowCustomMessageBox("5通道模式,最后1位必需是0或5", OpData.DlgTitle, MB_OK);
                        break;  
                    }
					else if ((Channel.Count == 5) && ((U8Value%5) != 0) && (U32Value == 5))
                    {
                        ShowCustomMessageBox("5通道模式,最后1位必需是0", OpData.DlgTitle, MB_OK);
                        break;  
                    }
                    
//                    U32Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));//输入的盒内数量
                    if((U32Value == 0) || (U32Value > 100) || (U8Value >= U32Value))
                    {
                        ShowCustomMessageBox("盒内数量或起始管码错误!", OpData.DlgTitle, MB_OK);
                        break; 
                    }
                    if(((U32Value%5) != 0) && (U32Value !=1))
                    {
                        ShowCustomMessageBox("要求盒内数量是5的倍数或为1!", OpData.DlgTitle, MB_OK);
                        break; 
                    }

                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
                    UserParam.DetOutsideStartBoxIndex = AsciiToU32(&OpData.GuiTemp[8]);
                    OpData.GuiTemp[8] = 0;
//                    UserParam.DetOutsideStartHead = AsciiToU32(&OpData.GuiTemp[0]);
					memcpy((char*)&UserParam.DetOutsideStartHead, &OpData.GuiTemp[0], 8);
                    
                    UTF8StringToGBK("起始管码前8位", (char*)OpData.GuiTemp);
                    if(WriteKeyString(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetOutsideStartHead) == ERROR)
                    {
                        if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
                        {
                            return;
                        }
                    }

                    UTF8StringToGBK("起始管码后5位", (char*)OpData.GuiTemp);
                    if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetOutsideStartBoxIndex) == ERROR)
                    {
                        if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
                        {
                            return;
                        }
                    }
                    
                    UserParam.InBoxNumber = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
                    UTF8StringToGBK("盒内数量", (char*)OpData.GuiTemp);
                    if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.InBoxNumber) == ERROR)
                    {
                        if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
                        {
                            return;
                        }
                    }                   
					if(((U32Value%10) != 0) && (Channel.Count == 10) && (U32Value != 1))
					{
						Channel.Count = 5;
						UTF8StringToGBK("通道数量", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, Channel.Count) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
						}
						else
						{
							ShowCustomMessageBox("通道数量设置为5，保存成功!", OpData.DlgTitle, MB_OK);
						}							
					}
					else if((U32Value == 1) &&(Channel.Count != 1))
					{
						Channel.Count = 1;
						UTF8StringToGBK("通道数量", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, Channel.Count) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
						}
						else
						{
							ShowCustomMessageBox("通道数量设置为1，保存成功!", OpData.DlgTitle, MB_OK);
						}							
					}
					else
					{
						ShowCustomMessageBox("保存成功!", OpData.DlgTitle, MB_OK);
					}
						
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
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) == 9)//第8位位特征号允许输入字母
						{
							if(LetterIndex > 25)
							{
								LetterIndex = 0;
							}
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
							MULTIEDIT_AddKey(OpData.CurrentInput, LETTER[LetterIndex++]);
						}						
					}
					break;
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

void ShowDetOutsideForBJDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

