#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },	
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ BUTTON_CreateIndirect, "Detail", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//详情
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 80, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 105, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 130, 240,  25,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     0, 155, 240,  25,  0,          0 }
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 显示主机控制从机快速测试结果
  *         @arg @b 1 显示主机控制从机起爆流程测试完成后结果
  *         @arg @b 2 显示主机控制从机网络充电完成后结果    
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

			ShowInfoTitle("Blast Progress");//起爆流程					
			GUI_SetFont(&SMALL_FONT);
			
			if (GetWindowParam() == 0)
			{//显示网络测试流程测试总表
				if(GetSlaveErrorCount() != 0)
				{
					ShowInfoHelp("Decoder Abnormal", NULL);//译码器异常	
				}			
			}
			else if (GetWindowParam() == 1)
			{//显示起爆流程测试总表
				if(GetSlaveErrorCount() == 0)
				{
                    if(Op_Data.WaitDetVerify == 0)
                    {
                        if(((DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType) || (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)) && BlasterIs04() && 
                            (SlaveInfo.OnlineCount != SlaveInfo.RegCount))
                        {
                            ShowInfoHelp("Have unconnected DETs", "No Charging");//有未连接雷管 禁止充电
                        }
                        else
                        {
                            ShowInfoHelp("↙ Beginning DETs Charging", NULL); //↙键开始网路充电                        
                        }                     
                    }
                    else
                    {
                        GUI_SetColor(GUI_RED);
                        GUI_SetFont(&NORMAL_FONT);
                        GUI_DispStringHCenterAt((const char*)"Waiting for the validation", 120,165);//等待手持机三码验证
											  GUI_DispStringHCenterAt((const char*)"of handheld device", 120,185);//等待手持机三码验证
                        GUI_SetFont(&SMALL_FONT);
                        GUI_SetColor(device_info.TextColor);
                        ShowInfoHelp(" ", NULL);
                    }
				}
				else
				{
					ShowInfoHelp("Decoder Abnormal", NULL);	//译码器异常
				}
			} 
			else if (GetWindowParam() == 2)
			{//等待双键起爆界面
				if(GetSlaveErrorCount() == 0)
				{
					ShowInfoHelp("Press 1 and 3 together", "Release To Blast");	//请同时按下数字键1和3 		抬起按键后将触发起爆	
				}
				else
				{
					ShowInfoHelp("Decoder Abnormal", NULL);//译码器异常	
				}				
			}
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if(GetWindowParam() == 0)
					{
						ShowMainMenuDlg(0);
					}
					else
					{
						Op_Data.DoubleKeySwitch = 0;//禁止打开双键
                        
                        switch(ShowBlastCannelConfirm())
                        {
                            case GUI_KEY_LEFT:
                                ShowBlastMasterProcessDlg(GetWindowParam());
                                break;
                            case GUI_KEY_RIGHT:
                                ShowBlastCannelDlg();
                                break;
                        }                         
					}
					break;
				case GUI_KEY_RIGHT:
					if ((GetWindowParam() <= 1) || ((2 == GetWindowParam())&&(GetSlaveErrorCount() != 0)))
					{
						SetSlaveInfoListViewDlgListViewSelIndex(0);
                        if(GetWindowParam() == 2)
                        {
                            ShowSlaveInfoListViewDlg(5);
                        }
                        else
                        {
                            ShowSlaveInfoListViewDlg(GetWindowParam());
                        }
					}					
					break;
				case GUI_KEY_ENTER:
					if (GetWindowParam() == 1)
					{
						if(GetSlaveErrorCount() == 0)
						{
                            if(Op_Data.WaitDetVerify == 0)
                            {
                                if(!(((DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType) || (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)) &&
                                    BlasterIs04() && (SlaveInfo.OnlineCount != SlaveInfo.RegCount)))
                                {
                                    SetSlaveInfoListViewDlgListViewSelIndex(0);						
                                    ShowSlaveInfoListViewDlg(4);//充电                                 
                                }                          
                            }
						}
					}
					break;
				case GUI_KEY_DOUBLE:
					if ((2 == GetWindowParam())&&(GetSlaveErrorCount() == 0))
					{//起爆双键					
						if(device_info.BlastDelay <= 1)
						{//起爆延时时间为0或者5秒
							ShowBlastDetDlg();
						}
						else
						{
							ShowBlastDelayDlg();						
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

                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);				
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);				
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER); 
                    if (GetWindowParam() <= 1)
                    {//显示快速测试、起爆流程测试总表
											sprintf(temp, "Decoder QTY:%-5u", GetSlaveStateCount(SLAVE_TRUE));//译码器数:
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), temp);
                        
                        sprintf(temp, "Log DET:%-5u", SlaveInfo.RegCount);//注册雷管:
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), temp);
                        
                        sprintf(temp, "ERR DET:%-5u", SlaveInfo.ErrorCount);//错误雷管:
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), temp);	
                        
                        if(SlaveInfo.ErrorCount != 0)
                        {
                            TEXT_SetTextColor(WM_GetDialogItem(hWin , GUI_ID_TEXT2), GUI_RED);
                        }
                        sprintf(temp, "Online DET:%-5u", SlaveInfo.OnlineCount);//在线雷管:
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), temp);
                    } 
                    else if (GetWindowParam() == 2)
                    {//等待双键起爆界面
                        if(GetSlaveErrorCount() == 0)
                        {
                            Op_Data.DoubleKeySwitch = 1;//允许打开双键
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"DETs Charge Completed");//网路充电完成
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Press 1 and 3 to blast");//请按1和3双键起爆
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2),"Make sure the area is safe!");//请确保爆区安全
                            TEXT_SetTextColor(WM_GetDialogItem(hWin , GUI_ID_TEXT2), GUI_RED);
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));				
                        }
                        else
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"DETs Charge Failed");//网路充电失败
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Decoder Abnormal");//译码器异常					
                        }
                    }				
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_10MS, 0);
                    break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    WM_InvalidateWindow(hWin);
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
						BlastTerminationFromAppDlg();
					}					
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowBlastMasterProcessDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
