#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "OK",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//确定
  { TEXT_CreateIndirect, "Discharging...", GUI_ID_TEXT0,     0, 110, 240,  20,  0,          0 }//放电中
};

int ShowBlastCannelConfirm(void)
{
    MsgBoxInit();
    SetMsgBoxCaption("Blast Progress");//起爆流程
    SetMsgBoxMessage("Exit The Blast?");//确定退出起爆流程?
    SetMsgBoxLeft("Cancel");//取消
    SetMsgBoxRight("OK");//确定
    
    return ShowMsgBox(); 
}
	
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
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器
				sprintf(&temp[40], "Lod DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管 错误雷管				
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管 错误雷管
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));	

            UTF8StringToGBK("取消起爆流程", (char*)Op_Data.Temp);
            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
            
            Op_Data.OfflineMonitorBlastProcess = 0;
            Op_Data.DoubleKeySwitch = 0;
            CLOSE_DOUBLE_KEY_SW();
            CLOSE_BLUE_POWER();
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                	
                OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)8);
                SlaveInfo.RegCount = 0;
                SlaveInfo.ErrorCount = 0;
			}
			else
            {
                OPEN_BUS_6_7V();
                AllDisCharge();
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_200MS_ID, GUI_TIMER_200MS, 0);
            }
						
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_5000MS_ID, GUI_TIMER_5000MS, 0);
            
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_RIGHT:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
                    {
                        if(Op_Data.BlastDeleteDet)
                        {
                            Op_Data.BlastDeleteDet = 0;
                            ReadAllDetInfo(0);//读雷管信息及延期值
                        }
                        ShowMainMenuDlg(0);	                    
                    }									
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
                    
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
					break;
				case GUI_TIMER_5000MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                   
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Discharge Completed!");//放电完毕!
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    break;                    
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowBlastCannelDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
