#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
};


/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 提示打开高压	
  *         @arg @b 1 提示正在检测高压
  *         @arg @b 2 提示等待接入网络 
  *         @arg @b 3 启动高压失败提示界面 
  *         @arg @b 4 提示正在进行网络通信
  *         @arg @b 5 提示充电完成 
  *         @arg @b 6 提示关机  
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Blast Progress");	//起爆流程				
			GUI_SetFont(&SMALL_FONT);

            sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
            sprintf(&temp[40], "Online DET:%d", GetDetOnlineCount());//在线雷管
            ShowInfoHelp(&temp[0], &temp[40]);			
            switch(GetWindowParam())
            {
                case 0://提示打开高压	
                    GUI_DispStringHCenterAt("Pls press the High V Key",120,110);	//Pls press the high V. Key
                    break;			 
                case 1://提示正在检测高压				
                    GUI_DispStringHCenterAt("Starting High V...",120,110);	//高压启动中...			
                    break;
                case 2://启动高压失败提示界面	
                    GUI_DispStringHCenterAt("Failed!",120,100);//启动失败!
                    GUI_DispStringHCenterAt("High V Abnormal!",120,125);//设备高压异常!
                    ShowInfoHelp(NULL, "Invalid retry,back the FTY");	//重试无效则需返厂维修					
                    break;			
                case 3://提示等待接入网络	
                    GUI_DispStringHCenterAt("Waiting for the internet...",120,110);	//等待接入网络...						
                    break;
                case 4:		
                    GUI_DispStringHCenterAt("Communicating...",120,110);	//网络通信中..."			
                    break;			
                case 5: 
                    GUI_DispStringHCenterAt("Charge completed!",120,110);//充电完成,等待起爆!	
								    GUI_DispStringHCenterAt("Waiting for the blast!",120,130);//充电完成,等待起爆!
                    break;
                case 6: 
                    GUI_DispStringHCenterAt("Blast completed!",120,100);//起爆完毕!
                    GUI_DispStringHCenterAt("Pls Shut Down!",120,125);	//请关闭设备!			
                    break;            
            }				
			break;
		case WM_INIT_DIALOG://初始化界面
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);	
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if (GetWindowParam() == 0)
					{
						OSTaskDel(APP_RS485_TASK_PRIO);
						CLOSE_HV_ENABLE();//禁止用户打开高压
						ShowMainMenuDlg(0);
					}
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    if (GetWindowParam() == 0)
                    {
                        DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
                        OPEN_HV_ENABLE();//允许用户打开高压
                    }			
                    else
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    }
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);			
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
                    break;
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					if (GetWindowParam() == 0)
					{
						if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
						{//大于2V			
							OSTimeDlyHMSM(0,0,0,100);
							if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
							{//检测到安全锁按键按下
                                SetWindowParam(1);
								WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
								
								OPEN_11_3V_SW();//打开芯片高压使能开关
								OSTimeDlyHMSM(0,0,0,100);

								//检测高压芯片及相应电路是否正常
								//在后台检测
								TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)0);
								
								WM_InvalidateWindow(hWin);												
							}
						}			
					}					
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
		case WM_BACKGROUND_TASK_END:
			if (GetWindowParam() == 1)
			{//检测高压芯片产生的电压是否符合要求
				if(((ErrorStatus)pMsg->Data.v) == ERROR)
				{
					SetWindowParam(2);
				}
				else
				{
					SetWindowParam(3);
					TaskCreate(APP_RS485_TASK_PRIO, (void*)0);					
				}
				WM_InvalidateWindow(hWin);
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowRemoteBlastSlaveStateDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
