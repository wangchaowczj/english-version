#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "STRT DET No.:", GUI_ID_TEXT0,     0, 20, 104,  20,  0,          0 },//起始管码
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     106, 20, 120,  20,  0,          0 },
	{ TEXT_CreateIndirect, "DET QTY:", GUI_ID_TEXT1,     24, 45, 100,  20,  0,          0 },//雷管数量
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT1,     106, 45, 120,  20,  0,          0 },
	{ TEXT_CreateIndirect, "Sum In Box:", GUI_ID_TEXT2,     16, 70, 88,  20,  0,          0 },//盒内数量
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT2,     106, 70, 120,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT3,     10, 100, 220,  35,  0,          0 },
    { BUTTON_CreateIndirect, "1ABCDEFGHI", GUI_ID_USER+1,     10, 140, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "2JKLMNOPQR", GUI_ID_USER+2,     10, 165, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "3STUVWXYZ", GUI_ID_USER+3,     10, 190, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "4abcdefghi", GUI_ID_USER+4,     125, 140, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "5jklmnopqr", GUI_ID_USER+5,     125, 165, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "6stuvwxyz", GUI_ID_USER+6,     125, 190, 105,  20,  0,          0 }
};
static void KeyboardStatusUpdate(WM_HWIN hWin)
{
    if(DlghWin.KeyboardStatus)
    {
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+1));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+2));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+3));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+4));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+5));
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_USER+6));
        
        TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), device_info.TextColor);
        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), device_info.BackgroundColor); 
    }
    else
    {
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+1));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+2));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+3));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+4));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+5));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+6));

        
        TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), device_info.BackgroundColor);
        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), GUI_RED);      
    }
    if(DlghWin.KeyboardStatus == 1)
    {
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
    }
    else
    {
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
    }
}
static ErrorStatus CheckInput(WM_HWIN hWin, u8 Save)
{
    U32_UNION Serl, Pwd;
    u32 U32Value;
    u16 Box, Index, DetCount, InBox, RegIndex;
    u8 Outside[14];
    
    InBox = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
    if((InBox == 0) || (InBox > 100))
    {
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Sum In Box ERR!");//盒内数量错误!
        return ERROR;
    }   
    memset(Outside, 0, sizeof(Outside));
    
    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)Outside, sizeof(Outside));
    sscanf((char*)&Outside[8],"%d", &U32Value);
    Box = U32Value/100;
    Index = U32Value%100;
    if(Index >= InBox)
    {
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Sum In Boc ERR!");////盒内数量错误!
        return ERROR;
    }

    DetCount = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
    RegIndex = GetWaitRegListViewDlgSelIndex();
    while(DetCount--)
    {
        if(Box > 999)
        {
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Input ERR!");//输入错误!
            return ERROR;
        }
        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)Outside, sizeof(Outside));
        sprintf((char*)&Outside[8], "%03d%02d",Box, Index);
        
        if(ERROR == XA3_DetOutside2Inside(Det.Type, (u8*)Outside, &Serl.U32Value, &Pwd.U32Value))
        {
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "DET ID ERR!");//管码错误!
            return ERROR;
        }
        if(DetFactoryCodeIsRulePass(Serl.U32Value, Pwd.U32Value) == 0)
        {
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Diff Chip Factory Code!");//工厂代码不一致!
            return ERROR;
        }
        if(CheckOverlap(&Serl.U8Buffer[0]) == ERROR)
        {
            if(XA3_DetInside2Outside(Det.Info[Op_Data.overlaped].Serl.U32Value, Det.Info[Op_Data.overlaped].Pwd.U32Value, Outside) == ERROR)
            {
                sprintf((char*)Op_Data.Temp, "DET has been logged:%d-%d",//雷管已注册: 
                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);            
            }
            else
            {
                sprintf((char*)Op_Data.Temp, "DET has been logged:%d-%d\nDET ID: %s", //雷管已注册:%d-%d\n管码:%s
                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside, Outside);
            }
            
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)Op_Data.Temp);
            return ERROR;
        }
        if ((DEVICE_USAGES_ALONE == device_info.DeviceUsages) &&
            (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType) && (Save == 0))
        {
            u8 ErrorCode;
            
            if(!DetIsAccreditPassForDL(&Serl, &Pwd, &ErrorCode))
            {
                XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, &Op_Data.Temp[0]);
                GetAccreditErrorString2ForDL((char*)&Op_Data.Temp[13], ErrorCode);
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)Op_Data.Temp);
                return ERROR;
            }        
        }
        Index++;
        if(Index >= InBox)
        {
            Index = 0;
            Box++;
        }
        if(Save)
        {
            Det.Info[RegIndex].Serl.U32Value = Serl.U32Value;
            Det.Info[RegIndex].Pwd.U32Value = Pwd.U32Value;
            Det.Info[RegIndex].Freq.U32Value = 0;
            RegIndex++;
        }
    }
    if(Save)
    {
        if(SaveAllDetInfoNotChangeRank(0) == ERROR)
        {
            ShowSDCardError();
            return ERROR;
        }
        if(RegIndex >= DET_MAX_SIZE)
        {
            RegIndex = DET_MAX_SIZE - 1;
        }
        SetWaitRegListViewDlgSelIndex(RegIndex);
    }
    return SUCCESS;
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
//            ShowInfoTitle("注册雷管");
            
            GUI_SetFont(&SMALL_FONT);
            
            if ((DlghWin.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0)) && 
                (MULTIEDIT_GetTextSize(DlghWin.CurrentInput) == 8))
            {
                ShowInfoHelp("Input:0...9","Number/Letter switch: ↙ "); //输入:0...9键","数字/字母切换:↙键                      
            }
            else
            {
                ShowInfoHelp("Input:0...9","OK: ↙ ");  //输入:0...9键","确认:↙键         
            }
			break;
		case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 13);

            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 3);

            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&SMALL_FONT);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT2), 3);
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
            ShowInputCursor(DlghWin.CurrentInput, 1);

            DlghWin.KeyboardStatus = 0;
            memset(DlghWin.KeyboardText, 0, sizeof(DlghWin.KeyboardText));
            KeyboardStatusUpdate(hWin);
            
            UTF8StringToGBK("进入批量注册", (char*)Op_Data.Temp);
            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);	
                        
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);            
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://左键返回
					ShowWaitRegListViewDlg();														
					break;
				case GUI_KEY_RIGHT://右键回删
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(DlghWin.KeyboardStatus != 2)
                        {
                            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                            {						
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                            }
                            else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
                            {
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);	
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                            }
                            else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                            {
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);	
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                            }                        
                        }
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "");
                        DlghWin.KeyboardStatus = 0;
                        KeyboardStatusUpdate(hWin);
                        
                        WM_InvalidateWindow(hWin);                        
                    }			
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)//没有输入时返回1
					{
                        if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                        {//起始雷管
                            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 13)
                            {
                                U32_UNION Serl, Pwd;
                                
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                MULTIEDIT_GetText(DlghWin.CurrentInput, temp, sizeof(temp));
                                
                                if(ERROR == XA3_DetOutside2Inside(Det.Type, (u8*)temp, &Serl.U32Value, &Pwd.U32Value))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Start DET ID ERR!");//起始管码错误!
                                    return;
                                }
                                if(CheckOverlap(&Serl.U8Buffer[0]) == ERROR)
                                {
                                    sprintf((char*)Op_Data.Temp, "This DET has been logged:%d-%d\nDET ID: %s", //该雷管已注册:%d-%d\n管码:%s
                                        Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside, temp);
                                    
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)Op_Data.Temp);
                                    return;
                                }
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);	
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                            }
                            else if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) == 8)
                            {
                                DlghWin.KeyboardStatus = !DlghWin.KeyboardStatus;
                                KeyboardStatusUpdate(hWin);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "");
                            }
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {//雷管数量
                            u16 Index, DetCount;
                            
                            DetCount = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
                            Index = GetWaitRegListViewDlgSelIndex();
                            if((DetCount == 0) || (DetCount > DET_MAX_SIZE) || ((Index+DetCount) > DET_MAX_SIZE))
                            {
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "DET QTY ERR!");//雷管数量错误!
                                return;
                            }
                            Index = GetRegDetIndexFirst(GetWaitRegListViewDlgSelIndex());
                            if(Index < (GetWaitRegListViewDlgSelIndex()+DetCount))
                            {
                                sprintf(temp, "Hole %d-%d has been logged", Det.Info[Index].Hole.Name, Det.Info[Index].Hole.Inside);//孔号%d-%d已注册
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), temp);
                                return;
                            }

                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);	
                            ShowInputCursor(DlghWin.CurrentInput, 1);
                        }
                        else
                        {//盒内数量
                            if(ERROR != CheckInput(hWin, 0))
                            {
                                if(ERROR != CheckInput(hWin, 1))
                                {
                                    UTF8StringToGBK("批量注册,起始管码:", (char*)Op_Data.Temp);
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)Op_Data.Temp, sizeof(Op_Data.Temp));
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);
                                    
                                    UTF8StringToGBK("雷管数量:", (char*)Op_Data.Temp);
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);
                                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)Op_Data.Temp, sizeof(Op_Data.Temp));
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);

                                    UTF8StringToGBK("盒内数量:", (char*)Op_Data.Temp);
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);
                                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)Op_Data.Temp, sizeof(Op_Data.Temp));
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);
                                    
                                    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)temp, sizeof(temp));
                                    SaveDetFeature(temp[7]);//保存特征号
                                    ShowWaitRegListViewDlg();
                                }
                            }
                        }
					}
					break;
				case GUI_KEY_0://数字
				case GUI_KEY_1://数字			
				case GUI_KEY_2://数字
				case GUI_KEY_3://数字
				case GUI_KEY_4://数字
				case GUI_KEY_5://数字
				case GUI_KEY_6://数字
				case GUI_KEY_7://数字
				case GUI_KEY_8://数字
				case GUI_KEY_9://数字
					if(WM_IsVisible(DlghWin.CurrentInput))
					{
                        if(DlghWin.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
                        {
                            temp[0] = ((WM_KEY_INFO*)(pMsg->Data.p))->Key-'0';
                            if(DlghWin.KeyboardStatus == 0)
                            {
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);                        
                            }
                            else if(DlghWin.KeyboardStatus == 1)
                            {
                                if((temp[0] >= 1) && (temp[0] <= 6))
                                {
                                    DlghWin.KeyboardStatus = 2;
                                    KeyboardStatusUpdate(hWin);
                                    ShowKeyboardCharSelectList(hWin, WM_GetDialogItem(hWin, GUI_ID_USER+temp[0]), WM_GetDialogItem(hWin,GUI_ID_TEXT3));                                
                                }
                            }
                            else 
                            {
                                if(temp[0] == 0)
                                {
                                    temp[0] = 10;
                                }
                                if (temp[0] <= strlen((const char*)DlghWin.KeyboardText))
                                {
                                    ShowInputCursor(DlghWin.CurrentInput, 0);
                                    MULTIEDIT_AddKey(DlghWin.CurrentInput, DlghWin.KeyboardText[temp[0]-1]);
                                    DlghWin.KeyboardStatus = 0;
                                    KeyboardStatusUpdate(hWin);
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "");                                    
                                }
                            }                        
                        }
                        else
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);                         
                        }

                        WM_InvalidateWindow(hWin);
					}				
					break;				
			}
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
					
					if(WM_IsVisible(DlghWin.CurrentInput))
					{
						ShowCursor(DlghWin.CurrentInput);
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

void ShowRegDetOutsideInputBatchDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


