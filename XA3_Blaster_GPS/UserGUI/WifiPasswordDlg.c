#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "6.65V 15000uA", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     40, 102, 160, 35,  0,          0 },	
	{ BUTTON_CreateIndirect, "1ABCDEFGHI", GUI_ID_USER+1,     10, 140, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "2JKLMNOPQR", GUI_ID_USER+2,     10, 165, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "3STUVWXYZ.", GUI_ID_USER+3,     10, 190, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "4abcdefghi", GUI_ID_USER+4,     10, 215, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "5jklmnopqr", GUI_ID_USER+5,     10, 240, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "6stuvwxyz`", GUI_ID_USER+6,     125, 140, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "7~!@#$%^&*", GUI_ID_USER+7,     125, 165, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "8()_+|-=\\{", GUI_ID_USER+8,     125, 190, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "9}[]:\";'<>", GUI_ID_USER+9,     125, 215, 105,  20,  0,          0 },
	{ BUTTON_CreateIndirect, "0?,/ ", GUI_ID_USER+0,     125, 240, 105,  20,  0,          0 },	
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     82, 10, 155, 22,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     82, 55, 155, 22,  0,          0 }
};



/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 i;
	char temp[60];
	static u8 Ssid[35];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);          
            GUI_Clear();
            GUI_SetFont(&SMALL_FONT);
            GUI_DispStringAt("WIFI Name:",2,12);//网络名
            GUI_DispStringAt("Password :",2,57);//密  码		
            break;
        case WM_INIT_DIALOG://初始化界面
            OSTaskDelReq(APP_WIFI_TASK_PRIO);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);					
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://左键返回
                    memcpy((u8*)&MyWifi.Ssid[0], (u8*)&Ssid[0], sizeof(MyWifi.Ssid));
                
                    ShowWifiNetInfoDlg();											
                    break;
                case GUI_KEY_RIGHT://右键回删
                    if(DlghWin.KeyboardStatus != 0)
                    {
                        DlghWin.KeyboardStatus = 0;
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));					
                    }
                    else
                    {
                        if(WM_IsWindow(DlghWin.CurrentInput))
                        {
                            if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                            {						
                                MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                            }
                            else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                            {//当前正在输入密码
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                            }																	
                        }					
                    }			
                    break;					
                case GUI_KEY_ENTER://回车键确认
                    if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                    {
                        if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                        {
                            if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                            {//正在输入网络名,切换到密码
                                ShowInputCursor(DlghWin.CurrentInput, 0);
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                            }					
                        }
                    }
                    else
                    {//正在输入密码
                    
                        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)MyWifi.Ssid, sizeof(MyWifi.Ssid));
                        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)MyWifi.Passwords, sizeof(MyWifi.Passwords));
                        if(strlen((const char*)MyWifi.Passwords) == 0)
                        {
                            strcpy((char*)MyWifi.Auth, "OPEN");
                            strcpy((char*)MyWifi.Encry, "NONE");							
                        }
                        ShowWifiConnectDlg(GetWindowParam());
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
                    temp[0] = ((WM_KEY_INFO*)(pMsg->Data.p))->Key-'0';
                    if(DlghWin.KeyboardStatus == 0)
                    {//
                        DlghWin.KeyboardStatus = 1;
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                        ShowKeyboardCharSelectList(hWin, WM_GetDialogItem(hWin, GUI_ID_USER+temp[0]), WM_GetDialogItem(hWin,GUI_ID_TEXT0));
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
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, DlghWin.KeyboardText[temp[0]-1]);
                        }
                    }			
                    break;			
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    OSTaskDelReq(APP_WIFI_TASK_PRIO);
                    
                    memcpy((u8*)&Ssid[0], (u8*)&MyWifi.Ssid[0], sizeof(MyWifi.Ssid));
                    
                    DlghWin.KeyboardStatus = 0;
                    memset(DlghWin.KeyboardText, 0, sizeof(DlghWin.KeyboardText));
                    
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), device_info.TextColor);
                    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), device_info.BackgroundColor);
                    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &SMALL_FONT);
                    
                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 32);		
                    MULTIEDIT_SetWrapChar(WM_GetDialogItem(hWin, GUI_ID_EDIT0));

                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 32);
                    MULTIEDIT_SetWrapChar(WM_GetDialogItem(hWin, GUI_ID_EDIT1));		
                    //MULTIEDIT_SetPasswordMode(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 1);
                    
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
                    
                    DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
                    
                    if(GetWindowParam() < MyWifi.ScanResult.ScanNumber)
                    {//显示选择的网络名称
                        WM_ResizeWindow(DlghWin.CurrentInput,0, 40 - WM_GetWindowSizeY(DlghWin.CurrentInput));
                        for(i=0;i<strlen((const char*)&MyWifi.ScanResult.ScanSum[GetWindowParam()].ssid[0]);i++)
                        {
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, MyWifi.ScanResult.ScanSum[GetWindowParam()].ssid[i]);
                        }
                        //MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (const char*)&MyWifi.ScanResult.ScanSum[DlgParam].ssid[0]);
                        strcpy((char*)MyWifi.Ssid, (const char*)&MyWifi.ScanResult.ScanSum[GetWindowParam()].ssid[0]);
                        strcpy((char*)MyWifi.Auth, (const char*)&MyWifi.ScanResult.ScanSum[GetWindowParam()].Auth[0]);
                        strcpy((char*)MyWifi.Encry, (const char*)&MyWifi.ScanResult.ScanSum[GetWindowParam()].Encry[0]);
                        
                        if(1 == ShowInputCursor(DlghWin.CurrentInput, 0))
                        {
                            WM_ResizeWindow(DlghWin.CurrentInput,0, 40 - WM_GetWindowSizeY(DlghWin.CurrentInput));
                        }
                        else
                        {
                            WM_ResizeWindow(DlghWin.CurrentInput,0, 22 - WM_GetWindowSizeY(DlghWin.CurrentInput));
                        }			
                        DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                    }
                    else
                    {
                        strcpy((char*)MyWifi.Auth, "WPA2PSK");
                        strcpy((char*)MyWifi.Encry, "AES");			
                    }
                    ShowInputCursor(DlghWin.CurrentInput, 1);

                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }		
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);

                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);                    
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(1 == ShowCursor(DlghWin.CurrentInput))
                        {
                            WM_ResizeWindow(DlghWin.CurrentInput,0, 40 - WM_GetWindowSizeY(DlghWin.CurrentInput));
                        }
                        else
                        {
                            WM_ResizeWindow(DlghWin.CurrentInput,0, 22 - WM_GetWindowSizeY(DlghWin.CurrentInput));
                        }			
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

void ShowWifiPasswordDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}


