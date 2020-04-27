#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 110, 240,  20,  0,          0 },//20, 110, 200,  20,  0,          0
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 130, 240,  20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Bus Abnormal");//总线异常 
						
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管: 错误雷管:
			ShowInfoHelp(&temp[0], NULL);
			break;
		case WM_INIT_DIALOG://初始化界面	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_TA_HCENTER|TEXT_CF_VCENTER);
		  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_TA_HCENTER|TEXT_CF_VCENTER);
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0),GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1),GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			if(BusIsShort())
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Short Circuit!");//总线短路,请排除故障!
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls remove the fault!");//总线短路,请排除故障!
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Voltage Abnormal!");//总线电压异常,请排除故障!
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls remove the fault!");//总线电压异常,请排除故障!
			}
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
			break;
			//这个是接收键盘上的按键的相应
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					break;
				case GUI_KEY_RIGHT:		
					break;			
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_200MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					if(BusIsShort())
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Short Circuit!");//总线短路,请排除故障!
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls remove the fault!");//总线短路,请排除故障!
					}
					else if (abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Voltage Abnormal");//总线电压异常,请排除故障!
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls remove the fault!");//总线电压异常,请排除故障!
					}					
					else
					{//恢复原有界面
						//继续监控总线
						Op_Data.BusMonitorDisable = 0;
						DlghWin.Dlg = WM_GetParent(hWin);
						
						GUI_EndDialog(hWin, 0);
					}
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					if(BusIsShort())
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Short Circuit!");//总线短路,请排除故障!
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls remove the fault!");//总线短路,请排除故障!
					}
					else if (abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Bus Voltage Abnormal!");//总线电压异常,请排除故障!
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls remove the fault!");//总线电压异常,请排除故障!
					}					
					else
					{
						WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_200MS_ID, GUI_TIMER_200MS, 0);
					}
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

void ShowBusErrorRestoreDlg(void)
{
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, DlghWin.Dlg, 0, 0);
}
