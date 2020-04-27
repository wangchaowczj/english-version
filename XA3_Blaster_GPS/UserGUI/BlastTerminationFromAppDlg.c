#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//返回
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
			ShowInfoTitle("Blast Progress");//起爆流程
			
            GUI_DispStringHCenterAt("Authorization Terminated!", 120, 100);//起爆流程终止授权!			

            sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:  错误雷管:
            GetDetInfoString(&temp[40], Det.Type);	
            ShowInfoHelp(&temp[0], &temp[40]);			
			break;
		case WM_INIT_DIALOG://初始化界面
            Op_Data.OfflineMonitorBlastProcess = 0;
            Op_Data.DoubleKeySwitch = 0;
            CLOSE_DOUBLE_KEY_SW();
            
            UTF8StringToGBK("起爆流程终止授权", (char*)temp);
            WriteSystemLog((char*)temp, strlen((char*)temp), CURRENT_STATUS);
            
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)8);		
            }
            else
            {
                OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                TestInfoInit();	
                OPEN_BUS_6_7V();
                AllDisCharge();
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_200MS_ID, GUI_TIMER_200MS, 0);                
            }            
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
                    ShowMainMenuDlg(1);
                    break;						
                default:
                    break;
            }			
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_200MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					
					AllDisCharge();			
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

void BlastTerminationFromAppDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
