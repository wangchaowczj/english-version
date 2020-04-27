#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Detail", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//详情
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	static u8 Count = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Blast Progress");//起爆流程
			if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
			{
				if(Op_Data.DelaySampleFlag)
				{
					sprintf(&temp[0], "Delaytime:%dms", Op_Data.DelaySampleDelay);//延期:
					ShowInfoHelp(&temp[0], "Pls press the High V Key!");//请按高压键启动高压!				
				}
				else
				{
                    if(((DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType) || (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)) && 
                        BlasterIs04() && (GetDetOnlineCount() != Det.RegCount))
                    {//卫星厂,有未连接雷管
                        ShowInfoHelp("Have unconnected DET!", "Don't open the High V!");//有未连接雷管!  禁止启动高压!
                    }
                    else
                    {
                        sprintf(&temp[0], "Online DET:%d", GetDetOnlineCount());//在线雷管:
                        
                        ShowInfoHelp(temp, "Pls press the High V Key!");//请按高压键启动高压!
                    }
				}						
			}			

			GUI_SetFont(&SMALL_FONT);
			
			if (Det.ErrorCount == 0)
			{
				GUI_DispStringAt("Log DET:",30,90);//注册雷管数:
				GUI_DispDecSpace(Det.RegCount,0);
				GUI_DispStringAt("ERR DET Sum:",30,110);//错误雷管数:
				GUI_DispDecSpace(Det.ErrorCount,0);               
				GUI_DispStringAt("BusCurrent(AV):",30,130);//总线平均电流:
				GUI_DispDecSpace(BlastInfo.I_BeforeTest,0);
				GUI_DispString("uA");
				if (Det.RegCount != 0)
				{
					GUI_DispStringAt("DETCurrent(AV):",30,150);//单发平均电流:
					GUI_DispDecSpace(BlastInfo.I_BeforeTest/Det.RegCount,0);
					GUI_DispString("uA");			
				}
			} 
			else
			{
				GUI_DispStringAt("Log DET:",30,70);//注册雷管数:
				GUI_DispDecSpace(Det.RegCount,0);
                
                GUI_SetColor(GUI_RED);
				GUI_DispStringAt("ERR DET Sum:",30,90);//错误雷管数:
				GUI_DispDecSpace(Det.ErrorCount,0);
                GUI_SetColor(device_info.TextColor);
                
				GUI_DispStringAt("BusCurrent(AV):",30,110);//总线平均电流:
				GUI_DispDecSpace(BlastInfo.I_BeforeTest,0);
				GUI_DispString("uA");
				GUI_DispStringAt("DETCurrent(AV):",30,130);//单发平均电流:
				GUI_GotoXY(30,150);
				GUI_DispDecSpace(BlastInfo.I_BeforeTest/Det.RegCount,0);
				GUI_DispString("uA(Include ERR DETs)");//含错误雷管
				
				if((Det.RegCount-Det.ErrorCount) != 0)
				{
					GUI_GotoXY(30,170);
					GUI_DispDecSpace(BlastInfo.I_BeforeTest/(Det.RegCount-Det.ErrorCount),0);
					GUI_DispString("uA(Uninclude ERR DETs)");//不含错误雷管				
				}
			}
			break;
		case WM_INIT_DIALOG://初始化界面
			if (Det.ErrorCount == 0)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));		
			}
				
			if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			}
			else if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
			{
                if(!(((DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType) || (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)) && BlasterIs04() && 
                    (GetDetOnlineCount() != Det.RegCount)))
                {
                    SetNormalHighVoltageGoal();
                    DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
                    OPEN_HV_ENABLE();//允许用户打开高压
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);                    
                }
			}
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
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
                    if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
                    {
                        CLOSE_HV_ENABLE();//禁止用户打开高压
                        switch(ShowBlastCannelConfirm())
                        {
                            case GUI_KEY_LEFT:
                                OPEN_HV_ENABLE();//允许用户打开高压
                                break;
                            case GUI_KEY_RIGHT:
                                ShowBlastCannelDlg();
                                break;
                        }                        
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if (Det.ErrorCount != 0)
                    {
                        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
                        {
                            CLOSE_HV_ENABLE();//禁止用户打开高压
                        }
                        
                        SetNetTestErrorListViewDlgListViewSelIndex(0);
                        ShowNetTestErrorListViewDlg(2);	
                    }	
                    break;			
                }
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);

					if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
					{//大于2V			
						Count++;
						if(Count > 5)
						{//检测到安全锁按键按下
							UTF8StringToGBK("检测到安全锁按键按下", (char*)Op_Data.Temp);
							WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);
							
							OPEN_11_3V_SW();//打开芯片高压使能开关
							OSTimeDlyHMSM(0,0,0,50);
							
							WM_DeleteTimer(pMsg->Data.v);
							
							ShowBlastProcessDlg(0);//检测高压芯片及相应电路是否正常				
						}
					}							
					break;
				case GUI_TIMER_500MS_ID:
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
						BlastTerminationFromAppDlg();
						break;
					}				
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

void ShowBlastTestReportDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
