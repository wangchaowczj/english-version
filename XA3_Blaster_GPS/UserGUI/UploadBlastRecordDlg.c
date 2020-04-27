#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect,   "Cancel",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "Uploading Blast Info...", GUI_ID_TEXT0,     20, 110, 200,  25,  0,          0 }//正在上传起爆信息...
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u32 timeout = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Blast Info");//起爆信息
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:%u 错误雷管:					
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%u 错误雷管:	
				GetDetInfoString(&temp[40], Det.Type);							
			}				
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
			FileSystemLock();
			WifiLock();				
			OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
			WifiUnlock();
			FileSystemUnlock();
			
			OSTaskDel(APP_WIFI_TASK_PRIO);
			
			timeout = OSTimeGet()+500;
			Op_Data.UpdateGuiFlag = 1;
							
			TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)1);/*上传指定起爆信息*/								

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}		
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					Op_Data.UpdateGuiFlag = 0;
					OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
					FileSystemLock();
					WifiLock();				
					OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
					WifiUnlock();
					FileSystemUnlock();
					
					ExitSafeMonitor();					
					ShowBlastRecorderListViewDlg();
					break;					
			}
			break;
		case WM_SAFE_MONITOR_TASK_END:
			Op_Data.UpdateGuiFlag = 0;
			if(((ErrorStatus)pMsg->Data.v) != ERROR)
			{							
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Upload Completed!");	//起爆信息上传成功!				
			}
			else
			{							
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Upload Failed!");//起爆信息上传失败!	
			}
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "Back");
			break;
		case WM_UPDATE_PROGBAR:
			WM_Paint(DlghWin.Status);
			if(OSTimeGet() > timeout)
			{
				timeout = OSTimeGet()+300;
				TEXT_GetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp, 50);
				if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info   ") == 0)//正在上传起爆信息   
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info.  ");
				}
				else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info.  ") == 0)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info.. ");
				}
				else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info.. ") == 0)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info...");
				}
				else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info...") == 0)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info   ");
				}
				WM_Paint(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	

					if(OSTimeGet() > timeout)
					{
						timeout = OSTimeGet()+300;
						TEXT_GetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp, 50);
						if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info   ") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info.  ");
						}
						else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info.  ") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info.. ");
						}
						else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info.. ") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info...");
						}
						else if(strcmp((char*)Op_Data.Temp, "Uploading Blast Info...") == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Uploading Blast Info   ");
						}
						WM_Paint(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
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

void ShowUploadBlastRecordDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
