#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     0, 100, 240,  96,  0,          0 }
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

			ShowInfoTitle("Device Abnormal");//设备异常	
			
			switch(GetWindowParam())
			{
                case 0://总线短路
                    ShowInfoHelp("Bus Short Circuit?", NULL);//请检查总线是否短路
                    break;
                case 1://总线电压异常
                    ShowInfoHelp("Bus Current Large?", NULL);//请检查总线电流是否过大
                    break;
                case 2://起爆流程雷管掉线
                    ShowInfoHelp("Shut Down", "Pls Check DET Network");//请关机检查雷管网络
                    break;                    
            }           
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_TOP);
            TEXT_SetWrapMode(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_WRAPMODE_CHAR);			
            
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);    
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					break;		
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					
                    switch(GetWindowParam())
                    {
                        case 0://总线短路
                            Op_Data.BlastStatus = BLAST_ERROR;
                            Op_Data.OfflineMonitorBlastProcess = 0;
                            
                            sprintf(temp, "Bus Short Circuit,Shut Down!\n\nV:%.1fV I:%duA",//总线短路,请关闭设备!\n\n电压:%.1fV 电流:%duA 
                                            Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                            
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)temp);
                            
                            UTF8StringToGBK(temp, (char*)Op_Data.Temp);
                            WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), CURRENT_STATUS);
                            break;
                        case 1://总线电压异常
                            Op_Data.BlastStatus = BLAST_ERROR;
                            Op_Data.OfflineMonitorBlastProcess = 0;
                            
                            sprintf(temp, "Bus V Abnormal,Shut Down!\n\nV:%.1fV I:%duA", //总线电压异常,请关闭设备!\n\n电压:%.1fV 电流:%duA
                                            Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)temp);
                            
                            UTF8StringToGBK(temp, (char*)Op_Data.Temp);
                            WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), CURRENT_STATUS);
                            break;
                        case 2://起爆流程雷管掉线
                            Op_Data.BlastStatus = BLAST_ERROR;
                            Op_Data.OfflineMonitorBlastProcess = 0;
                
                            sprintf(temp, "Bus I Small,DET Maybe Drop!\n\nV:%.1fV I:%duA",//总线电流小,可能有雷管掉线!\n\n电压:%.1fV 电流:%duA 
                                            Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)temp);
                            
                            UTF8StringToGBK(temp, (char*)Op_Data.Temp);
                            WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), CURRENT_STATUS);
                            
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                            if (0 != GET_LINAB_SHORT_SW_CTRL_STATE())
                            {//当前总线电压没有关闭
                                Op_Data.DoubleKeySwitch = 0;
                                CLOSE_DOUBLE_KEY_SW(); 
                                if(GET_HV_SW_CTRL_STATE() == 0)
                                {//当前为端口电压为低压状态
                                    OPEN_BUS_6_7V();
                                    AllDisCharge();
                                }
                                else
                                {//高压
                                    OPEN_BUS_6_7V();
                                    OSTimeDly(10);
                                    AllDisCharge();
                                    OPEN_HV_SW();
                                }
                            }
                            break;
                        default:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls Shut Down!");//请关闭设备!
                            break;
                    }
                    WM_InvalidateWindow(hWin);
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
void ShowWaitPowerOffDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
