#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     20, 20, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     20, 45, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "Feature Code:", GUI_ID_TEXT2,     20, 70, 56,  20,  0,          0 },//特征码:
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     124, 70, 100,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     40, 100, 160,  35,  0,          0 },
    { BUTTON_CreateIndirect, "1ABCDE", GUI_ID_USER+1,     10, 140, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "2FGHIJ", GUI_ID_USER+2,     10, 165, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "3KLMNO", GUI_ID_USER+3,     10, 190, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "4PQRST", GUI_ID_USER+4,     10, 215, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "5UVWXYZ", GUI_ID_USER+5,     10, 240, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "6abcde", GUI_ID_USER+6,     125, 140, 105,  20,  0,          0 },
    { BUTTON_CreateIndirect, "7fghij", GUI_ID_USER+7,     125, 165, 105,  20,  0,          0 },
    { BUTTON_CreateIndirect, "8klmno", GUI_ID_USER+8,     125, 190, 105,  20,  0,          0 },
    { BUTTON_CreateIndirect, "9pqrst", GUI_ID_USER+9,     125, 215, 105,  20,  0,          0 },
    { BUTTON_CreateIndirect, "0uvwxyz", GUI_ID_USER+0,     125, 240, 105,  20,  0,          0 }
};

static ErrorStatus CheckInput(WM_HWIN hWin, u8 Save)
{
    U32_UNION Serl, Pwd;
    u8 Buffer[14];
    BOX_BIG_INFO BoxBigInfo;
    BOX_SMALL_INFO BoxSmallInfo;
    u16 Box, Index, DetCount, InBox, RegIndex;
    
    memset(Buffer, 0, sizeof(Buffer));
    if(BarCodeAnalyst17ForBoxBig(&BoxBigInfo) != ERROR)
    {//箱条码
        DetCount = BoxBigInfo.InBoxBig*BoxBigInfo.InBoxSmall;
        sprintf((char*)Buffer, "%02d%01d%02d%02d", BoxBigInfo.Factory, BoxBigInfo.Year%10, BoxBigInfo.Month,BoxBigInfo.Day);
        
        Box = BoxBigInfo.Box*BoxBigInfo.InBoxBig;//起始盒号 = 箱号*每箱盒数量
        InBox = BoxBigInfo.InBoxSmall;//每盒数量
    }
    else if(BarCodeAnalyst18ForBoxSmall(&BoxSmallInfo) != ERROR)
    {//盒条码
        DetCount = BoxSmallInfo.InBoxSmall;
        sprintf((char*)Buffer, "%02d%01d%02d%02d", BoxSmallInfo.Factory, BoxSmallInfo.Year%10, BoxSmallInfo.Month,BoxSmallInfo.Day);
        Box = BoxSmallInfo.Box;
        InBox = BoxSmallInfo.InBoxSmall;//每盒数量
    }
    MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)&Buffer[strlen((char*)Buffer)], 3);
    
    if(strlen((char*)Buffer) != 8)
    {
        MsgBoxInit();
        SetMsgBoxCaption("Logging DETs");//雷管注册
        SetMsgBoxMessage("PARM ERR!");//参数错误!
        SetMsgBoxLeft("Back");
        ShowMsgBox();
        return ERROR;
    }
    Index = 0;
    RegIndex = GetWaitRegListViewDlgSelIndex();
    while(DetCount--)
    {
        if(Box > 999)
        {
            MsgBoxInit();
            SetMsgBoxCaption("Logging DETs");//雷管注册
            SetMsgBoxMessage("Box No. Beyond 999!");//号超出999!
            SetMsgBoxLeft("Back");//返回
            ShowMsgBox();
            return ERROR;
        }
        sprintf((char*)&Buffer[8], "%03d%02d", Box, Index);
        
        if(ERROR == XA3_DetOutside2Inside(Det.Type, (u8*)Buffer, &Serl.U32Value, &Pwd.U32Value))
        {
            MsgBoxInit();
            SetMsgBoxCaption("Logging DETs");//雷管注册
            SetMsgBoxMessage("DET ID ERR!");//管码错误
            SetMsgBoxLeft("Back");//返回
            ShowMsgBox();
            return ERROR;
        }
        if(DetFactoryCodeIsRulePass(Serl.U32Value, Pwd.U32Value) == 0)
        {
            MsgBoxInit();
            SetMsgBoxCaption("Logging DETs");//雷管注册
            SetMsgBoxMessage("Diff Chip Factory Code!");//工厂代码不一致!
            SetMsgBoxLeft("Back");//返回
            ShowMsgBox();
            return ERROR;
        }
        if(CheckOverlap(&Serl.U8Buffer[0]) == ERROR)
        {
            u8 Outside[14];
            
            memset(Outside, 0, sizeof(Outside));
            if(XA3_DetInside2Outside(Det.Info[Op_Data.overlaped].Serl.U32Value, Det.Info[Op_Data.overlaped].Pwd.U32Value, Outside) == ERROR)
            {
                sprintf((char*)Op_Data.Temp, "DET has been logged:%d-%d", //雷管已注册:
                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);            
            }
            else
            {
                sprintf((char*)Op_Data.Temp, "DET has been logged:%d-%d\nDET ID:%s", //雷管已注册:%d-%d\n管码:
                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside, Outside);            
            }
            
            MsgBoxInit();
            SetMsgBoxCaption("Logging DETs");//雷管注册
            SetMsgBoxMessage((char*)Op_Data.Temp);
            SetMsgBoxLeft("Back");//返回
            ShowMsgBox();
            return ERROR;
        }
        if ((DEVICE_USAGES_ALONE == device_info.DeviceUsages) &&
            (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType) && (Save == 0))
        {
            u8 ErrorCode;
            
            if(!DetIsAccreditPassForDL(&Serl, &Pwd, &ErrorCode))
            {
                MsgBoxInit();
                
                SetMsgBoxCaption("Logging DETs");//雷管注册
                XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, &Op_Data.Temp[0]);
                GetAccreditErrorString2ForDL((char*)&Op_Data.Temp[13], ErrorCode);
                
                SetMsgBoxMessage((char*)Op_Data.Temp);
                SetMsgBoxMessageColor(GUI_RED);
                SetMsgBoxLeft("Cancel");//取消
                ShowMsgBox();
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
static KeyEnter(WM_HWIN hWin)
{
    char temp[50];
    
    if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)//没有输入时返回1
    {
        if(ERROR != CheckInput(hWin, 0))
        {
            if(ERROR != CheckInput(hWin, 1))
            {
                MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)&temp[0], 3);
                SaveDetFeature(temp[0]);
                
                //记日志
                TEXT_GetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)&temp[0], sizeof(temp));
                UTF8StringToGBK((char*)&temp[0], (char*)Op_Data.Temp);
                WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                
                memset(temp, 0, sizeof(temp));
                strcpy(temp, "Feature Code:");//特征号:
                MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)&temp[strlen((char*)temp)], 3);
                UTF8StringToGBK((char*)&temp[0], (char*)Op_Data.Temp);
                WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), STR_INFO);
            }
        }
        ShowWaitRegListViewDlg();
    }
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
    BOX_BIG_INFO BoxBigInfo;
    BOX_SMALL_INFO BoxSmallInfo;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);          
            GUI_Clear();
            GUI_SetFont(&SMALL_FONT);
			break;
		case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            
            TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), device_info.TextColor);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), device_info.BackgroundColor);
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
            
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 1);
            
            temp[0] = GetDetFeatureDefult();
            if(temp[0] != 0)
            {
                temp[1] = 0;
                InitUserStringToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), temp, 1);
            }
            DlghWin.KeyboardStatus = 0;
            memset(DlghWin.KeyboardText, 0, sizeof(DlghWin.KeyboardText));             
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
            ShowInputCursor(DlghWin.CurrentInput, 1);
            
            ScanData.Bytes = GetScanDataBytes();
            ScanData.Buffer[ScanData.Bytes] = 0x00;
            
            if(BarCodeAnalyst17ForBoxBig(&BoxBigInfo) != ERROR)
            {//箱条码
                sprintf((char*)Op_Data.Temp, "TBC:%s", &ScanData.Buffer[0]);//箱条码:
                if(ScanData.Bytes == 18)
                {
                    Op_Data.Temp[strlen((char*)Op_Data.Temp)-1] = 0;
                }
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp);
                
                sprintf((char*)Op_Data.Temp, "DET QTY:%d", BoxBigInfo.InBoxBig*BoxBigInfo.InBoxSmall);//雷管数量:
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
            }
            else if(BarCodeAnalyst18ForBoxSmall(&BoxSmallInfo) != ERROR)
            {//盒条码
                sprintf((char*)Op_Data.Temp, "BBC:%s", &ScanData.Buffer[0]);//盒条码
                if(ScanData.Bytes == 19)
                {
                    Op_Data.Temp[strlen((char*)Op_Data.Temp)-1] = 0;
                }
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp);
                
                sprintf((char*)Op_Data.Temp, "DET QTY:%d", BoxSmallInfo.InBoxSmall);//雷管数量:
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
                
                //机台号就是特征号
                temp[0] = ScanData.Buffer[14];
                temp[1] = 0;
                
                ClearUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                InitUserStringToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), temp, 1);
                
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_1MS, 0);
            }
            
            SoundOutputRight();
            
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
                        if(DlghWin.KeyboardStatus != 0)
                        {
                            DlghWin.KeyboardStatus = 0;
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));					
                        }
                        else
                        {
                            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                            {						
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                            }                        
                        }                 
                    }			
					break;					
				case GUI_KEY_ENTER://回车键确认
                    KeyEnter(hWin);
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
                        temp[0] = ((WM_KEY_INFO*)(pMsg->Data.p))->Key-'0';
                        if(DlghWin.KeyboardStatus == 0)
                        {//
                            DlghWin.KeyboardStatus = 1;
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
                            ShowKeyboardCharSelectList(hWin, WM_GetDialogItem(hWin, GUI_ID_USER+temp[0]), WM_GetDialogItem(hWin,GUI_ID_TEXT3));
                        }
                        else
                        {//当前为等待第二次选择
                            if(temp[0] == 0)
                            {
                                temp[0] = 10;
                            }
                            if (temp[0] <= strlen((const char*)DlghWin.KeyboardText))
                            {
                                DlghWin.KeyboardStatus = 0;
                                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, DlghWin.KeyboardText[temp[0]-1]);
                            }
                        }			
					}				
					break;				
			}
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_2MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					
					KeyEnter(hWin);			
					break;
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

void ShowRegDetFeatureInputForBarcodeDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


