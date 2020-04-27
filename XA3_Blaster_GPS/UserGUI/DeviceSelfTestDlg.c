#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     8, 100, 224,  25,  0,          0 },//20, 100, 200,  25,  0,          0
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT1,     8, 125, 224,  25,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u8 SelfState = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Device Self-test");//设备自检
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);		//注册雷管:%u 错误雷管:		
			}
			else
			{
				sprintf(&temp[0], "Log DET:%u ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
				GetDetInfoString(&temp[40], Det.Type);				
			}

			ShowInfoHelp(&temp[0], &temp[40]);				
			break;
		case WM_INIT_DIALOG://初始化界面			
			SelfState = 0;
					
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			if(Op_Data.LinABCurrent > 10)
			{
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Self-test Abnormal!");//设备自检异常,总线电流大!
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Large current on bus!");//设备自检异常,总线电流大!
			}
			else
			{
				//在后台进行设备自检
				TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)1);/*设备自检低压部分*/
											
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Device Self-testing...");//设备自检中...
				WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_BUTTON0));			
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
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:	
					if(WM_IsVisible(WM_GetDialogItem(hWin , GUI_ID_BUTTON0)))
					{//任务已经结束
						if(DEVICE_USAGES_MASTER != device_info.DeviceUsages)
						{
							OPEN_BUS_6_7V();
						}					
						Op_Data.DoubleKeySwitch = 0;
						CLOSE_HV_SW_INSIDE();
						
						ShowHelpToolMenuDlg();
					}					
					break;
				case GUI_KEY_DOUBLE:
					if ((SelfState == 2) && (Op_Data.DoubleKeySwitch != 0))
					{//低压部分自检完成
						SelfState |= 0x80;
						
						WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_BUTTON0));
						TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Blast Button Self-testing...");//起爆双键自检中...
						//在后台进行设备自检
						TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)7);/*设备自检起爆信号部分*/					
					}
					break;
			}
			break;
		case WM_BACKGROUND_TASK_END:
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				CLOSE_BUS_VOLTAGE();
			}
			else
			{
				OPEN_BUS_6_7V();	
			}		
			if(((ErrorStatus)pMsg->Data.v) == ERROR)
			{							
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Device Self-test Abnormal!");//设备自检异常!
			}
			else
			{				
				if(SelfState == 0)
				{//低压部分自检完成
					SelfState = 1;
					DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
					OPEN_HV_ENABLE();//允许用户打开高压
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Pls open the High V Key!");//请按高压键启动高压!				
				}
				else if((SelfState&0x0F) == 1)
				{//高压部分自检完成
					SelfState = 2;
					Op_Data.DoubleKeySwitch = 1;//允许按双键
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Pls press 1 and 3 together!");//请按1和3双键!					
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Self-test Normal!");	//设备自检正常!			
				}	
			}
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "Back");
			WM_ShowWindow(WM_GetDialogItem(hWin , GUI_ID_BUTTON0));
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					
					if(SelfState == 1)
					{
						if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
						{//大于2V
							OSTimeDlyHMSM(0,0,0,300);
							if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
							{
								SelfState |= 0x80;
								
								OPEN_11_3V_SW();//打开芯片高压使能开关
								OSTimeDlyHMSM(0,0,0,100);
								
								WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_BUTTON0));
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"High V Self-testing...");//高压自检中...
								
								//在后台进行设备自检
								TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)6);/*设备自检高压部分*/				
							}						
						}			
					}				
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
                    UpdateTextPointCount(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
                    
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

void ShowDeviceSelfTestDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
