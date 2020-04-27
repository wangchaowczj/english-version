#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 启动高压提示界面
  *         @arg @b 1 等待双键起爆界面
  *         @arg @b 2 启动高压失败提示界面    
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
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);
			GUI_Clear();

			ShowInfoTitle("Blast Progress");//起爆流程					
			GUI_SetFont(&SMALL_FONT);
            switch(GetWindowParam())
            {
                case 0://检测高压芯片产生的电压是否符合要求
                    GUI_DispStringHCenterAt("Starting High V...",120,100);//高压启动中...
                    break;
                case 1://等待双键起爆界面	
                    GUI_DispStringHCenterAt("DETs Charge Completed",120,100);//网路充电完成
                    GUI_DispStringHCenterAt("Press 1 and 3 to blast",120,120);//请按1和3双键起爆
                    GUI_SetColor(GUI_RED);
                    GUI_DispStringHCenterAt("Make sure the area is safe!",120,160);//请确保爆区安全
                    GUI_SetColor(device_info.TextColor);
                    ShowInfoHelp("Press 1 and 3 to blast", "Release To Blast");//请同时按下数字键1和3  抬起按键后将触发起爆
                    break;
                case 2://启动高压失败提示界面
                    GUI_DispStringHCenterAt("Start Failed!",120,100);//启动失败!
                    GUI_DispStringHCenterAt("High V Abnormal!",120,120);//设备高压异常!
                    ShowInfoHelp("Pls exit the process and retry", "Invalid Retry,bcak the FTY");	//请退出起爆流程后再重试   重试无效则需返厂维修
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
						;
					}
					else
					{
						Op_Data.DoubleKeySwitch = 0;//不允许按下双键

                        switch(ShowBlastCannelConfirm())
                        {
                            case GUI_KEY_LEFT:
                                Op_Data.DoubleKeySwitch = 1;
                                break;
                            case GUI_KEY_RIGHT:
                                ShowBlastCannelDlg();
                                break;
                        } 						
					}
					break;
				case GUI_KEY_DOUBLE:
					if (1 == GetWindowParam())
					{//起爆双键
                        u8 U8Value;
                        
						UTF8StringToGBK("已按下起爆双键", (char*)Op_Data.Temp);
						WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
						
                        U8Value = DetOfflinePossible(Op_Data.LinABCurrent);
                        if(U8Value != 0)
                        {
                            sprintf((char*)temp,"Part of DETs lost connection:%d", U8Value);//部分雷管可能掉线:
                            UTF8StringToGBK((char*)temp, (char*)Op_Data.Temp);
                
                            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Blast Progress");//起爆流程
                            SetMsgBoxMessage("Part of DETs lost connection!");//部分雷管可能掉线!
                            SetMsgBoxMessageColor(GUI_RED);
                            SetMsgBoxLeft("Cancel");//取消
                            SetMsgBoxRight("CONT");//继续

                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowBlastCannelDlg();
                                return;
                            }
                        }
						if(Op_Data.DelaySampleFlag)
						{
							ShowDelaySampleProcessBlastDetDlg();
						}
						else
						{
							if(device_info.BlastDelay <= 1)
							{//起爆延时时间为0或者5秒
                                Op_Data.OfflineMonitorBlastProcess = 0;
								ShowBlastDetDlg();
							}
							else
							{
                                ErrorStatus Result;
                                
                                CLOSE_HV_SW();
                                OSTimeDly(10);
                                Result = BlastSingleSelfTest(0);
                                OPEN_HV_SW();
                                if(Result == ERROR)
                                {
                                    Op_Data.DoubleKeySwitch = 0;//不允许按下双键
                                    CLOSE_DOUBLE_KEY_SW();//就是想让这个引脚电平切换一下
                                    
                                    MsgBoxInit();
                                    SetMsgBoxCaption("Device Abnormal");//设备异常
                                    SetMsgBoxMessage("Circuit Abnormal!");//起爆信号发送电路异常!
                                    SetMsgBoxRight("Retry");//重试

                                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                                    {
                                        ShowBlastProcessDlg(GetWindowParam());
                                    }                   
                                }
                                else
                                {
                                    sprintf((char*)temp,"Device Delaytime:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);//起爆延时: 时 分 秒
                                    UTF8StringToGBK((char*)temp, (char*)Op_Data.Temp);
                                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                                    
                                    Op_Data.OfflineMonitorBlastProcess = 0;
                                    ShowBlastDelayDlg();	                                
                                }					
							}						
						}
					}
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
                        case 0://检测高压芯片产生的电压是否符合要求
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                            TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)0);//在后台检测	                            
                            break;
                        case 1://等待双键起爆界面
                            Op_Data.DoubleKeySwitch = 1;//允许打开双键
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	
                            break;
                        case 2://启动高压失败提示界面
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                            break;                            
                    }
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
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
                        if(!MsgBoxIsShow())
                        {
                            BlastTerminationFromAppDlg();
                            break;                        
                        }
					}				
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
					break;
				default:
					break;
			}		
			break;
		case WM_BACKGROUND_TASK_END:
			if (GetWindowParam() == 0)
			{//检测高压芯片产生的电压是否符合要求
				if(((ErrorStatus)pMsg->Data.v) == ERROR)
				{
					ShowBlastProcessDlg(2);
				}
				else
				{
					ShowProgbarChargeDlg();
				}			
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowBlastProcessDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
