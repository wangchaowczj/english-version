#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR0,     20, 120, 200, 20,  0,          0 }
};

static void UpdateBar(WM_HWIN hWin)
{
	GUI_RECT Rect;
	
	PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);
	
	//仅更新帮助区内容，即更新错误雷管数
	Rect.x0 = 0;
	Rect.y0 = INFO_HELP_LINE_Y;
	Rect.x1 = LCDP_MAX_X - 1;
	Rect.y1 = BUTTON_SMALL_Y - 1;
	WM_InvalidateRect(hWin,&Rect);		
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("Blast Progress");//起爆流程
            GUI_SetFont(&SMALL_FONT);	
            GUI_DispStringAt("Fast Testing...",48,100);	//组网测试中...		

            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                sprintf(&temp[0],"Log DET:%d",SlaveInfo.RegCount);//注册雷管:
                sprintf(&temp[40],"ERR DET:%d",SlaveInfo.ErrorCount);	//错误雷管:		
            }
            else
            {
                sprintf(&temp[0],"Log DET:%d",Det.RegCount);//注册雷管:
                sprintf(&temp[40],"ERR DET:%d",Det.ErrorCount);	//错误雷管:		
            }
            ShowInfoHelp(&temp[0], &temp[40]);
            break;
        case WM_INIT_DIALOG://初始化界面
            PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0),&SMALL_FONT);
            
            //做从机时，起爆流程测试不允许手动取消
            if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
            }
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR0);
            PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
            PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
            PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
            PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
            
            //雷管检测任务
            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)0);
            ClearProgbarData();
                        
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);	
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://取消雷管测试
                    if(OSTaskSuspend(APP_DET_OPERATE_TASK_PRIO) != OS_ERR_TASK_NOT_EXIST)//挂起任务
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Exit the fast test?");//确定退出组网测试?
                        SetMsgBoxLeft("Back");
                        SetMsgBoxRight("OK");
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_LEFT:
                                OSTaskResume(APP_DET_OPERATE_TASK_PRIO);
                                break;
                            case GUI_KEY_RIGHT:
                                if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                                {//
                                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                                    AllDisCharge();
                                }
                                TestInfoInit();
                                ShowMainMenuDlg(1);
                                break;                                
                        }                        
                    }			
                    				
                    break;			
            }
            break;
        case WM_UPDATE_PROGBAR://更新进度条
            UpdateBar(hWin);	
            break;
        case WM_SET_PROGBAR_PARAM:
            PROGBAR_SetMinMax(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), 0, Op_Data.Progbar.Bar[0].Max);
            PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);	
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_10MS_ID:
                    UpdateBar(hWin);
                    if (Op_Data.BlastVerifyResult == '0')
                    {//终止起爆
                        BlastTerminationFromAppDlg();
                        break;
                    }                    
                    
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {//没有测试完成
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    if((TEST_RESULT)AppTaskResult.DetOperate == UNREG_DET)
                    {
                        Op_Data.DetTestResult |= 0x01;//有未注册雷管
                    }
                    else
                    {
                        Op_Data.DetTestResult &= 0xFE;
                    }			
                            
                    UTF8StringToGBK("起爆流程组网测试", (char*)Op_Data.Temp);
                    WriteSystemLog(Op_Data.Temp,strlen((char*)Op_Data.Temp), TEST_INFO);
                    ShowNetTestReportDlg(2);
                    SoundOutput(0);                    
                    break;
                default:
                    break;
            }		
            break;		
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowProgbarQuickBeforeBlastDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

