#include "includes.h"
#include "GuiIncludes.h"


/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//取消
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     20, 110, 200,  20,  0,          0 },
	{ TEXT_CreateIndirect, "Initializing WIFI...", GUI_ID_TEXT1,     20, 135, 200,  20,  0,          0 }//无线网络初始化中...
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	u8 LogDate[11];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			
			ShowInfoTitle("Log Upload");//日志上传"
			GUI_SetFont(&SMALL_FONT);		

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:%u 错误雷管:%	
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);////注册雷管:%u 错误雷管:%	
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);   
					  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);//这条语句是新加的
		        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);//这条语句是新加的		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://取消固件升级
					//下载固件过程中需要申请资源，必需保证资源处于释放状态才能删除任务
					OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
					FileSystemLock();
					WifiLock();
					
					OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
					
					WifiUnlock();
					FileSystemUnlock();
					
					ExitSafeMonitor();
					ShowLogRecorderListViewDlg();					
					break;			
			}
			break;
		case WM_UPDATE_PROGBAR:
			sprintf((char*)temp, "Upload %.2f%%", pMsg->Data.v/100.0);//已上传
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);
			break;
		case WM_SAFE_MONITOR_TASK_END:
			if(((ErrorStatus)pMsg->Data.v) != ERROR)
			{							
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Upload Completed!");//上传成功!
				UTF8StringToGBK("Log Upload Completed!", (char*)Op_Data.Temp);//上传日志成功!					
			}
			else
			{							
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Upload Failed!");//上传失败!
				UTF8StringToGBK("Log Upload Failed!", (char*)Op_Data.Temp);//上传日志失败!		
			}
			WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);	
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "Back");//返回
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
                            
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    
                    if ((GET_WIFI_POWER_STATE() != 0) || (GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP))
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Log Upload");//日志上传
                        SetMsgBoxMessage("Pls Connect To WIFI!");//请连接WIFI网络!
                        SetMsgBoxLeft("Back");//返回

                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowHelpToolMenuDlg();
                        }
                        break;		
                    } 
                    else if(TaskIsExist(APP_SAFE_MONITOR_TASK_PRIO))
                    {
                        OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
                        FileSystemLock();
                        WifiLock();
                        
                        OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
                        
                        WifiUnlock();
                        FileSystemUnlock();
                    }
                    OSTaskDel(APP_WIFI_TASK_PRIO);
                    
                    LogDate[0] = '2';
                    LogDate[1] = '0';
                    LogDate[2] = LogRecorder.FileName[LogRecorder.Select][0];
                    LogDate[3] = LogRecorder.FileName[LogRecorder.Select][1];
                    LogDate[4] = '-';
                    LogDate[5] = LogRecorder.FileName[LogRecorder.Select][2];
                    LogDate[6] = LogRecorder.FileName[LogRecorder.Select][3];
                    LogDate[7] = '-';
                    LogDate[8] = LogRecorder.FileName[LogRecorder.Select][4];
                    LogDate[9] = LogRecorder.FileName[LogRecorder.Select][5];
                    LogDate[10] = 0x00;
                    
                    sprintf(temp, "Log Upload:%s", &LogDate[0]);//上传日志:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
                    
                    UTF8StringToGBK(temp, (char*)Op_Data.Temp);
                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);	
                                    
                    TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)5);/*上传日志文件*/		

                    
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
					break;
				default:
					break;
			}		
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowLogUploadDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

