#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//回删
	{ TEXT_CreateIndirect, "Charge V(mV):", GUI_ID_TEXT0,     40, 100, 104,  20,  0,          0 },//充电电压(mV):
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     144, 100, 56,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 130, 240,  20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;

	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("DET Analysis");//雷管分析
            if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
            {
                ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键
            }
            else
            {
                ShowInfoHelp("", NULL);
            }
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 5);	
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://左键返回
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{
						CLOSE_HV_SW_INSIDE();
						OPEN_BUS_6_7V();
						SetNormalHighVoltageGoal();
						OSTimeDly(10);
						AllDisCharge();
						ShowDetAnalystDlg();
					}															
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
						{						
							MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GUI_KEY_BACKSPACE);
						}				
					}			
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)//没有输入时返回1
					{
						ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
						Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));	
						if((Value < 5000) || (Value > 12000))
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Voltage Range:5~12V");//电压范围:5~12V
							break;
						}
						Op_Data.LinNormalHighVoltageGoal = Value;
						setDacHvValueMinFromVoltage(LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX);
                        
						UTF8StringToGBK("雷管分析输入高压值:", (char*)Op_Data.Temp);
						WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);
						WriteSystemLog(&Op_Data.LinNormalHighVoltageGoal,sizeof(Op_Data.LinNormalHighVoltageGoal), U16_DATA);
						
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT0));						
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
						
						TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), device_info.TextColor);	
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Please open the High V!");	//请打开高压键
						DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
						OPEN_HV_ENABLE();//允许用户打开高压
						
						WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
					}
					break;
				case GUI_KEY_0://数字
				case GUI_KEY_1://数字			
				case GUI_KEY_2://数字
				case GUI_KEY_3://数字
				case GUI_KEY_4://数字
				case GUI_KEY_5://数字
				case GUI_KEY_6://数字
				case GUI_KEY_7://数字
				case GUI_KEY_8://数字
				case GUI_KEY_9://数字
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
						MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
					}				
					break;				
			}
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:					
					if(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)
					{//大于2V	 检测到安全锁按键按下
						WM_DeleteTimer(pMsg->Data.v);
						
						UTF8StringToGBK("检测到高压键按下", (char*)Op_Data.Temp);
						WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);
						
						OPEN_11_3V_SW();//打开芯片高压使能开关
						OSTimeDlyHMSM(0,0,0,50);
								
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Starting High V...");	//高压启动中...
						
						TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)0);	
					}
					else
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					}
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
					
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						ShowCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
					}				
					break;	
				case GUI_TIMER_10000MS_ID://充电完毕
					WM_DeleteTimer(pMsg->Data.v);
					ShowDetAnalystBlastDlg();
					break;
				default:
					break;
			}		
			break;
		case WM_BACKGROUND_TASK_END://检测高压芯片产生的电压是否符合要求
			if(((ErrorStatus)pMsg->Data.v) == ERROR)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "High V Abnormal!");//设备高压异常
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			}
			else
			{
				Op_Data.LinAdjustTime = OSTimeGet() + 5000;//刚输出高压,电流太大，如果马上调整有可能会出现电压太高，硬件保护
				OPEN_HV_SW();//高压芯片产生的高压正常，输出到总线
				
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Charging...");//充电中...
				WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10000MS_ID, GUI_TIMER_10000MS, 0);
			}
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetAnalystHighVoltageInputDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


