#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     10, 95, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT1,     10, 125, 220,  25,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Info Management");//信息管理	
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);			//注册雷管:%u 错误雷管:		
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%u 错误雷管:		
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);					
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
      TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &NORMAL_FONT);	
		  TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1), &NORMAL_FONT);		
            
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"1.Device Info");//1.设备信息
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"2.Blast Info ");//2.起爆信息

			CLOSE_BLUE_POWER();

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
					ShowMainMenuDlg(0);
					break;
				case GUI_KEY_1:
					ShowDeviceInfoDlg();
					break;
				case GUI_KEY_2:
					if(BlastRecorder.Number == 0)
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Info");//起爆信息
                        SetMsgBoxMessage("No Blast Info!");//没有起爆信息!
                        SetMsgBoxLeft("Back");

                        ShowMsgBox();
					}
					else
					{
						//蓝牙模块从机初始化
						OSTaskDel(APP_TEMPORARY_TASK_PRIO);
						TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)0);//手机可以导入起爆记录
						
						BlastRecorder.Select = 0;
						ShowBlastRecorderListViewDlg();						
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
					break;
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowInfoManageDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
