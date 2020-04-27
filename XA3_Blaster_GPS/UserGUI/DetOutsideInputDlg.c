#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "DET ID:", GUI_ID_TEXT0,     30, 70, 60,  20,  0,          0 },//管码 40, 70, 40,  20,  0,          0
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     90, 70, 120,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     40, 100, 160,  35,  0,          0 },
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
        
        TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), device_info.TextColor);
        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), device_info.BackgroundColor); 
    }
    else
    {
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+1));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+2));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+3));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+4));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+5));
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_USER+6));

        
        TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), device_info.BackgroundColor);
        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);      
    }
    if(DlghWin.KeyboardStatus == 1)
    {
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
    }
    else
    {
        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
    }
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
            ShowInfoTitle("Rewrite Chip ID");//内码重写
            
            GUI_SetFont(&SMALL_FONT);
            
            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) == 8)
            {
                ShowInfoHelp("Input:0...9","Number/Letter switch: ↙ ");    //输入:0...9键","数字/字母切换:↙键                    
            }
            else
            {
                ShowInfoHelp("Input:0...9","OK: ↙ "); //输入:0...9键","确认:↙键          
            }
			break;
		case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
            DlghWin.KeyboardStatus = 0;
            memset(DlghWin.KeyboardText, 0, sizeof(DlghWin.KeyboardText));
            KeyboardStatusUpdate(hWin);
            
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 13);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);            
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://左键返回
					ShowDetCodeWriteMenuDlg(GetWindowParam());															
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsVisible(DlghWin.CurrentInput))
					{
                        if(DlghWin.KeyboardStatus != 2)
                        {
                            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                            {						
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                                
                            }                        
                        }
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                        DlghWin.KeyboardStatus = 0;
                        KeyboardStatusUpdate(hWin);
                        
                        WM_InvalidateWindow(hWin);
					}			
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 13)//没有输入时返回1
					{                        
						ShowInputCursor(DlghWin.CurrentInput, 0);
                        MULTIEDIT_GetText(DlghWin.CurrentInput, temp, sizeof(temp));
                        
                        if(ERROR == XA3_DetOutside2Inside(Det.Type, (u8*)temp, &Op_Data.RegDetInfo.Serl.U32Value, &Op_Data.RegDetInfo.Pwd.U32Value))
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DET ID ERR!");//管码错误!
                            return;
                        }
                        SaveDetFeature(temp[7]);//保存特征号
                        ShowDetCodeWriteStatusDlg(GetWindowParam(), 1);
					}
                    else if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) == 8)
                    {
                        DlghWin.KeyboardStatus = !DlghWin.KeyboardStatus;
                        KeyboardStatusUpdate(hWin);
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
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
                                ShowKeyboardCharSelectList(hWin, WM_GetDialogItem(hWin, GUI_ID_USER+temp[0]), WM_GetDialogItem(hWin,GUI_ID_TEXT1));                            
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
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");                                
                            }
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
					
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						ShowCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
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

void ShowDetOutsideInputDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}


