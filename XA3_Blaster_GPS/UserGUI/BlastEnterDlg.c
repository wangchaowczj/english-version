#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//回删
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     113, 130, 90,  20,  0,          0 },//105是X轴的位置，130是Y轴的位置，90是对话框的长度，20是对话框的高度
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT1,     113, 160, 90,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     50, 42, 190,  20,  0,          0 },
};

static void BackKey(void)
{
    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
    {
        ShowMainMenuDlg(0);
    }
    else
    {
        ShowNetTestReportDlg(2);
    }
} 
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u8 Result, State;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Blast Progress");//起爆流程
            if(device_info.BlastDelay == 0)
            {
                ShowInfoHelp("OK: ↙ ", "Current DLY:0s");//确认:↙键 当前起爆延时:0秒
            }
            else if(device_info.BlastDelay == 1)
            {
                ShowInfoHelp("OK: ↙ ", "Current DLY:5s");//确认:↙键 当前起爆延时:5秒
            }
            else 
            {
                sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);//当前起爆延时 时 分 秒						
                ShowInfoHelp("OK: ↙ ", temp);//确认:↙键
            }
            
            GUI_SetFont(&SMALL_FONT);
            if(Op_Data.DKeyVoltage != 0xFFFF)
            {
                sprintf(temp, "Dongle Battery:%d%%", BatVoltageToAccumulated(Op_Data.DKeyVoltage));	//密钥电量
                GUI_DispStringAt((const char*)temp,28 , 72);//50,72			
            }		
            sprintf(temp, "Dongle Name:%s",BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name);	//密钥名
            GUI_DispStringAt((const char*)temp,28 , 102);//50,102
            GUI_DispStringAt((const char*)"User Name:",28 , 132);//用户名: 50,132
            GUI_DispStringAt((const char*)"Password :",28 ,162);//密  码: 50,162
            break;
        case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 8);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 8);
            MULTIEDIT_SetPasswordMode(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 1);
            
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);

            Result = strlen((char*)BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name);
            if((Result >= 11) && (Result <= 12))
            {
                InitUserStringToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 
                    (char*)&BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[Result-8], 8);
            }
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
            ShowInputCursor(DlghWin.CurrentInput, 1);
            
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }
            
            if(DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)
            {
                if(memcmp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name, "BKG_", 4) == 0)
                {
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
                } 
            }
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://左键返回
                    BackKey();										
                    break;
                case GUI_KEY_RIGHT://右键回删
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
                    break;					
                case GUI_KEY_ENTER://回车键确认
                    if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                    {
                        if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                        {//正在输入用户名,切换到密码
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);	
                            ShowInputCursor(DlghWin.CurrentInput, 1);
                        }
                        else
                        {//密码输入完成						
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            
                            //用户名密码验证						
                            memset(temp,0,sizeof(temp));
                            MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)temp, 10);
                            MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)&temp[10], 10);
                            if(ERROR != GetBlueDkeyInfo())
                            {
                                Result = BlueDkeyUserInfoVerify((u8*)&temp[0], (u8*)&temp[10],&State);
                                if(Result == '1')
                                {//用户名密码正确
                                    UTF8StringToGBK("用户名密码正确", (char*)Op_Data.Temp);									
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                                    
                                    OSTaskDel(APP_TEMPORARY_TASK_PRIO);
                                    CLOSE_BLUE_POWER();	 
                                    
                                    memset(BlastInfo.DkeyInfo.Operator,0,sizeof(BlastInfo.DkeyInfo.Operator));
                                    memcpy(BlastInfo.DkeyInfo.Operator, &temp[0], sizeof(BlastInfo.DkeyInfo.Operator));

                                    if ((DEVICE_USAGES_ALONE == device_info.DeviceUsages) &&
                                        (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType))
                                    {
                                        ShowDetAccreditErrorForDLDlg(0);//雷管授权验证
                                    }
                                    else
                                    {
                                        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
                                        {
                                            ShowWaitLinCurrentReadyDlg(2);
                                        }
                                        else
                                        {
                                            SetSlaveInfoListViewDlgListViewSelIndex(0);
                                            ShowSlaveInfoListViewDlg(3);
                                        }                                    
                                    }							
                                }
                                else if((0 == GET_BLUE_POWER_STATUS()) && (BLUE_NOT_CONNECT == GET_BLUE_CONNECT_STATUS()))
                                {
                                    if(ShowDkeyUserInfoError(Result, State) == GUI_KEY_LEFT)
                                    {
                                        ShowBlueHostStatusDlg(0);
                                    }                                
                                }
                                else
                                {
                                    if(ShowDkeyUserInfoError(Result, State) == GUI_KEY_LEFT)
                                    {
                                        ShowBlastEnterDlg();
                                    }
                                }						
                            }
                            else if((0 == GET_BLUE_POWER_STATUS()) && (BLUE_NOT_CONNECT == GET_BLUE_CONNECT_STATUS()))
                            {
                                if(ShowDkeyUserInfoError(Result, State) == GUI_KEY_LEFT)
                                {
                                    ShowBlastEnterDlg();
                                }
                            }						
                            else
                            {
                                if(ShowDkeyUserInfoError(0x01, 0) == GUI_KEY_LEFT)
                                {
                                    ShowBlastEnterDlg();
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
                    ShowInputCursor(DlghWin.CurrentInput, 0);
                    MULTIEDIT_AddKey(DlghWin.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
                    break;			
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_2MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    Result = BlueDkeyGpsIsPass(&Op_Data.Gps);
                    if(Result == 0x01)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Location Not Obtained!");//密钥未获取位置信息!
                        SetMsgBoxHelp1("Get location by dongle");//通过密钥获取定位信息
                        SetMsgBoxMessageColor(GUI_RED);
                    }
                    else if(Result == 0x02)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Retry!");//请重新获取位置信息!
                        SetMsgBoxHelp1("Valid for 24 hours");//位置信息24小时内有效
                        SetMsgBoxMessageColor(GUI_RED); 
                    }
                    else if(Result != 0x00)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Can't connect to the dongle!");//无法连接数字密钥!
                    }
                    if(Result != 0x00)
                    {
                        SetMsgBoxLeft("Back");//返回
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            BackKey();
                        }
                        return;
                    }
                    if(!AccreditAreaVerifyForDL())
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Location ERR!");//不在准爆区域!
                        sprintf(temp,"Location:%.4f, %.4f", //位置:
                            GPSFormatConvert(Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude)), 
                            GPSFormatConvert(Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude)));
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxMessageColor(GUI_RED);
                        SetMsgBoxLeft("Back");//返回
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            BackKey();
                        }
                        return;
                    }
                    sprintf(temp,"Location:%.4f, %.4f", //位置
                        GPSFormatConvert(Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude)), 
                        GPSFormatConvert(Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude)));
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

                    if(WM_IsWindow(DlghWin.CurrentInput))
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

void ShowBlastEnterDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


