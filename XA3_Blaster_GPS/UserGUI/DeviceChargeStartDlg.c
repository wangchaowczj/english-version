#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 320, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON0,     1, 1, 10, 10,  0,          0 },
};
static u8 ChargeNumber = 0;
	
static void ShowBatteryState(u16 BatAccumulated)
{
	//const u16 BatVoltage[5] = {3580, 3710, 3760, 3840, 3970};
//	const u16 BatVoltage[5] = {3500, 3600, 3700, 3800, 3900};
	u8 i, Number = 0;
    char Buffer[5];
    
	//根据输入的电量转换成电量格数
	if(BatAccumulated > 75)
	{
		Number = 5;
	}
	else if(BatAccumulated > 60)
	{
		Number = 4;
	} 
	else if(BatAccumulated > 40)
	{
		Number = 3;
	}
	else if(BatAccumulated > 20)
	{
		Number = 2;
	}
	else if(BatAccumulated > 5)
	{
		Number = 1;
	}
	else
	{
		Number = 0;
	}

	if(Number > 1)
	{
		GUI_SetColor(GUI_GREEN); 
	}
	else
	{
		GUI_SetColor(GUI_RED); 
	}	


	GUI_FillRect(35,125,195,185);
	GUI_FillRect(195,145,205,165);//电池前边的正极标志


	GUI_SetColor(GUI_WHITE); 
	GUI_FillRect(40,130,190,180);	

	GUI_SetColor(GUI_BLUE); 
	if((ChargeNumber > 5) || (ChargeNumber < Number))
	{
		ChargeNumber = Number;
	}
	Number = ChargeNumber;


	for(i=0;i<Number;i++)
	{
		GUI_FillRect(49+28*i,135,69+28*i,175);	
	}
    
	GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&NORMAL_FONT);
    Number = BatAccumulated*1.3158+0.5; //76%的电量，显示100% ，换算比例是100/76
    if(Number > 100)
    {
        Number = 100;
    }
    sprintf(Buffer, "%d%%", Number);
    GUI_DispStringHCenterAt(Buffer, 120,95);
}

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	static u8 BatAccumulated = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();
			ShowBatteryState(BatAccumulated);	
			break;
		case WM_INIT_DIALOG://初始化界面
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_BUTTON0));
			
			//为了减小设备自身功耗，关掉一些非必要的电路
			CLOSE_BUS_VOLTAGE();
			
			//关机状态下，常灭
			device_info.LcdBrightTime = 0;		
			LcdBrightRefreshProcess(1);		//屏幕亮度设到最暗
			
			ChargeNumber = 0;
			BatAccumulated = LTC2942BatAccumulated();
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			DlghWin.Timer = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10000MS_ID, GUI_TIMER_10000MS, 0);
			break;
		case WM_KEY:
			WM_RestartTimer(DlghWin.Timer, GUI_TIMER_10000MS);
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_RIGHT:
					break;										
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					
					if(GET_KEY_PW_STATE() == KEY_DOWN)
					{
						WM_RestartTimer(DlghWin.Timer, GUI_TIMER_10000MS);
						LcdBrightRefreshProcess(1);		//屏幕亮度设到最暗
						
						Op_Data.SystemIdleTime = OSTimeGet();
					}		

					BatAccumulated = LTC2942BatAccumulated();	
					if((BatAccumulated > 3) || (LTC2942Init() == ERROR))//电池容量太小时禁止开机
					{
						POWER_ON();
						if(GET_PW_STATE() == PW_STATE_ON)
						{//
							ShowDeviceStartDlg();
						}					
					}
					else
					{
						POWER_OFF();
					}					
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					ChargeNumber++;//充电的时候如果还没充满，形成进度条形式
					
					BatAccumulated = LTC2942BatAccumulated();
				
					WM_InvalidateWindow(pMsg->hWin);				
					break;
				case GUI_TIMER_10000MS_ID:
					LCD_BRIGHT_L();
					break;
				default:
					break;
			}		
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDeviceChargeStartDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
