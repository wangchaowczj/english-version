#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },	
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR0,     20, 70, 200, 20,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR1,     20, 120, 200, 20,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR2,     20, 170, 200, 20,  0,          0 }
};
static void UpdateBar(WM_HWIN hWin)
{
	GUI_RECT Rect;
	
	if(0 == Op_Data.Progbar.Index)
	{
		PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);
	}
	else if(1 == Op_Data.Progbar.Index)
	{
		PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR1), Op_Data.Progbar.Bar[1].Value);
	}
	else if(2 == Op_Data.Progbar.Index)
	{
		PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR2), Op_Data.Progbar.Bar[2].Value);
	}
	
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
            GUI_DispStringAt("Downloading the delaytime...",16,50);//延期下载中...
            GUI_DispStringAt("Preparing for the blast...",16,100);//起爆准备中...
            GUI_DispStringAt("Verifying the Dat...",40,150);//数据验证中...

            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                sprintf(&temp[0],"Log DET:%d",SlaveInfo.RegCount);//注册雷管:
                sprintf(&temp[40],"ERR DET:%d",SlaveInfo.ErrorCount);	//错误雷管:		
            }
            else
            {
                sprintf(&temp[0],"Log DET:%d",Det.RegCount);//注册雷管:
                sprintf(&temp[40],"ERR DET:%d",Det.ErrorCount);//错误雷管:			
            }
            ShowInfoHelp(&temp[0], &temp[40]);
            break;
        case WM_INIT_DIALOG://初始化界面
            PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0),&SMALL_FONT);
            PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR1),&SMALL_FONT);
            PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR2),&SMALL_FONT);
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR0);
            PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
            PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
            PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
            PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR1);
            PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
            PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
            PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
            PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR2);
            PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
            PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
            PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
            PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
            
            //雷管检测任务
            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)2);
            ClearProgbarData();
            
            if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
            {//从机不允许取消
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
            }
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);				
            break;
            //这个是接收键盘上的按键的相应
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://取消雷管测试
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
                    {//做从机时，起爆流程测试不允许手动取消
                        if(OSTaskSuspend(APP_DET_OPERATE_TASK_PRIO) != OS_ERR_TASK_NOT_EXIST)//挂起任务
                        {						
                            switch(ShowBlastCannelConfirm())
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
                                    ShowBlastCannelDlg();
                                    break;                                
                            }
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

            PROGBAR_SetMinMax(WM_GetDialogItem(hWin, GUI_ID_PROGBAR1), 0, Op_Data.Progbar.Bar[1].Max);
            PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR1), Op_Data.Progbar.Bar[1].Value);

            PROGBAR_SetMinMax(WM_GetDialogItem(hWin, GUI_ID_PROGBAR2), 0, Op_Data.Progbar.Bar[2].Max);
            PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR2), Op_Data.Progbar.Bar[2].Value);		
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
                    UTF8StringToGBK("起爆流程测试", (char*)temp);
                    WriteSystemLog(temp,strlen((char*)temp), TEST_INFO);	
                    if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Det.ErrorCount > 0))
                    {
                        SetNetTestErrorListViewDlgListViewSelIndex(0);			
                        ShowNetTestErrorListViewDlg(2);
                    }
                    else
                    {
                        ShowBlastTestReportDlg();				
                    }
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

void ShowProgbarBlastDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

