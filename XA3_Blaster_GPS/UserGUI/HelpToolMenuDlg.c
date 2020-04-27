#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
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
			ShowInfoTitle("Help Tool");//辅助工具
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				GUI_DispStringHCenterAt("1.Device Self-test", 120, 80);//1.设备自检120, 100
				GUI_DispStringHCenterAt("2.WIFI            ", 120, 100);//2.WIFI网络 120, 130
				GUI_DispStringHCenterAt("3.Firmware Update ", 120, 120);// 3.固件升级
				GUI_DispStringHCenterAt("4.Log Upload      ", 120, 140);// 4.日志上传
				
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);	//注册雷管:%u 错误雷管:
				ShowInfoHelp(&temp[0], &temp[40]);				
			}
			else
			{
				//GUI_DispStringHCenterAt("1.DET Test 5.Firmware Upgrade", 120, 80);//1.雷管测试 5.固件升级
				//GUI_DispStringHCenterAt("2.DET Analysis 6.Dat Operation", 120, 105);//2.雷管分析 6.数据操作
				//GUI_DispStringHCenterAt("3.Device Self-test 7.Re-write Inner Code", 120, 130);//3.设备自检 7.内码重写
                //GUI_DispStringHCenterAt("4.WIFI 8.Log Upload", 120, 155);	//4.WIFI网络 8.日志上传			
				GUI_DispStringAt("1.DET Test", 48, 50);            //1.雷管测试
				GUI_DispStringAt("2.DET Analysis", 48, 70);        //2.雷管分析
				GUI_DispStringAt("3.Device Self-test", 48, 90);    //3.设备自检
        GUI_DispStringAt("4.WIFI", 48, 110);	             //4.WIFI网络	
				GUI_DispStringAt("5.Firmware Update", 48, 130);   //5.固件升级
				GUI_DispStringAt("6.Data Operation", 48, 150);      //6.数据操作
				GUI_DispStringAt("7.Rewrite Chip ID", 48, 170);//7.内码重写
        GUI_DispStringAt("8.Log Upload", 48, 190);	       //8.日志上传	

				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
				GetDetInfoString(&temp[40], Det.Type);	
				ShowInfoHelp(&temp[0], &temp[40]);				
			}			
			break;
		case WM_INIT_DIALOG://初始化界面
            CLOSE_SCAN();
            Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_L_GOAL;
            DacOutLVVoltage(Op_Data.LinNormalLowVoltageGoal);
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				CLOSE_BUS_VOLTAGE();
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}		
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
				{
					case GUI_KEY_LEFT:
						ShowMainMenuDlg(0);
						break;
					case GUI_KEY_1://设备自检
						OPEN_BUS_6_7V();						
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Device Self-test");//设备自检
                            SetMsgBoxMessage("Device Self-testing \n\nDon't connect DET to \nthe bus terminal!");//设备自检过程中 \n\n总线端口禁止连接雷管!
                            sprintf(temp, "Log DET:%d, 	ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            SetMsgBoxRight("OK");//确定
                        
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                ShowDeviceSelfTestDlg();
                            }
                        }
						break;				
					case GUI_KEY_2:
						ShowWifiNetSetDlg();//WIFI 
						break;
					case GUI_KEY_3:
						ShowSoftwareUpdateDlg();//固件升级				
						break;
					case GUI_KEY_4:
						LogRecorder.Select = 0;
						ShowLogRecorderListViewDlg();				
						break;						
					default:
						break;
				}			
			}
			else
			{
				switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
				{
					case GUI_KEY_LEFT:
						ShowMainMenuMoreDlg();
						break;
					case GUI_KEY_1://定位信息
                        if(Op_Data.LinABCurrent > 30)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("DET Test");//雷管测试
                            SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管! Permit one DET on bus
                            sprintf(temp, "Log DET:%d, ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;
                        }
						ShowHelpToolDetTestDlg(0);
						break;				
					case GUI_KEY_2://雷管分析
                        if(Op_Data.LinABCurrent > 30)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("DET Analysis");//雷管分析
                            SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管!
                            sprintf(temp, "Log DET:%d, ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;
                        }

						ShowDetAnalystDlg();
						break;
					case GUI_KEY_3://设备自检
                        MsgBoxInit();
                        SetMsgBoxCaption("Device Self-test");//设备自检
                        SetMsgBoxMessage("Device Self-testing... \n\nDon't connect DET to the bus!");//设备自检过程中 \n\n总线端口禁止连接雷管!
                        sprintf(temp, "Log DET:%d, ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确定
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ShowDeviceSelfTestDlg();
                        }
						break;					
					case GUI_KEY_4:
						ShowWifiNetSetDlg();//WIFI
						break;
					case GUI_KEY_5:
						ShowSoftwareUpdateDlg();//固件升级				
						break;				
					case GUI_KEY_6:				
						ShowDetBackupOperateTypeSelDlg();														
						break;
					case GUI_KEY_7://内码重写
                        if(Op_Data.LinABCurrent > 30)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Rewrite Chip ID");//内码重写
                            SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管!
                            sprintf(temp, "Log DET:%d, ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;
                        }
						ShowDetCodeWriteMenuDlg(1);			
						break;
					case GUI_KEY_8://日志上报
						LogRecorder.Select = 0;
						ShowLogRecorderListViewDlg();			
						break;						
					default:
						break;
				}			
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

void ShowHelpToolMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
