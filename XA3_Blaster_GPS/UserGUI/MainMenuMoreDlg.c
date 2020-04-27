#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
};


static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("More");

            GUI_SetFont(&NORMAL_FONT);
            GUI_DispStringHCenterAt((const char*)"1.Net Test", 120,60);//1.网路测试
            GUI_DispStringHCenterAt((const char*)"2.Del DET ", 120,95);//2.雷管删除
            GUI_DispStringHCenterAt((const char*)"3.Sys Set ", 120,130);//3.系统设置
            GUI_DispStringHCenterAt((const char*)"4.Tool    ", 120,165);//4.辅助工具

            sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
            GetDetInfoString(&temp[40], Det.Type);	
            ShowInfoHelp(&temp[0], &temp[40]);	
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);            
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    ShowMainMenuDlg(0);										
                    break;
                case GUI_KEY_1:
                    if(!AllDetFactoryCodeIsRulePass())
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Network Test");//网路测试
                        SetMsgBoxMessage("Part of DETs have ERR\n\nChip Factory Codes!");//有部分雷管工厂代码错误!
                        SetMsgBoxMessageColor(GUI_RED);
                        sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        ShowMsgBox();
                        return;
                    }
                    MsgBoxInit();
                    SetMsgBoxCaption("Network Test");//网路测试
                    SetMsgBoxMessage("Pls test in safety area!");//请在安全区域测试!
                    SetMsgBoxMessageColor(GUI_RED);
                    sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                    SetMsgBoxHelp1(temp);
                    SetMsgBoxLeft("Back");
                    SetMsgBoxRight("OK");
                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                    {
                        UTF8StringToGBK("进入网路测试", (char*)Op_Data.Temp);
                        WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                        
                        ShowWaitLinCurrentReadyDlg(1);                        
                    }			
                    break;		
                case GUI_KEY_2:
                    UTF8StringToGBK("进入删除雷管界面", (char*)Op_Data.Temp);
                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                    
                    ShowDeleteDetMenuDlg();			
                    break;		
                case GUI_KEY_3:
                    ShowSystemSetMenuDlg();	
                    break;	
                case GUI_KEY_4:
                    ShowHelpToolMenuDlg();			
                    break;  				
                default:
                    break;
            }
            break; 
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);	
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        ShowMainMenuDlg(0);
                    }
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
                    break;
                default:
                    break;
            }		
            break;	
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowMainMenuMoreDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

