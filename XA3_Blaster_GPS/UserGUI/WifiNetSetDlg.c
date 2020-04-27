#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Cancel",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect, "Scan", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     38, 115, 30, 50,  0,          0X1E02 }//50,112,17,40,0,3
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[60];
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("WIFI");//无线局域网
		if(GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP)
		{
			sprintf(&temp[0], "WIFI Connect:%s", MyWifi.Ssid);//Wifi已连接到:%s
			ShowInfoHelp(&temp[0], NULL);			
		}
		else
		{
			ShowInfoHelp("WIFI Connect Failed", NULL);//Wifi没有连接	
		}
		
		//RADIO字体设置有错位现象，采用变通方式，文字自绘
		GUI_SetFont(&NORMAL_FONT);//后增加的语句
		GUI_DispStringAt("1.Close WIFI", 58,110);//1.关闭WIFI网络
		GUI_DispStringAt("2.Open WIFI", 58,140);//2.打开WIFI网络
		break;
	case WM_INIT_DIALOG://初始化界面
		hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
		
		RADIO_SetTextColor(hitem,device_info.TextColor);
		RADIO_SetBkColor(hitem, device_info.BackgroundColor);
		if(GET_WIFI_POWER_STATE() != 0)
		{//WIFI电源为关
			RADIO_SetValue(hitem, 0);
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
		}
		else
		{
			RADIO_SetValue(hitem, 1);
		}
		
		
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
		}			
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:
				ShowHelpToolMenuDlg();
				break;
			case GUI_KEY_1:
				if(0 != RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
				{
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
					if(GET_WIFI_POWER_STATE() == 0)
					{//当前WIFI处于打开状态
						BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2),"OK");//确定
						WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
					}
					else
					{
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
					}
				}
				break;				
			case GUI_KEY_2:
				if(1 != RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
				{
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
					if(GET_WIFI_POWER_STATE() == 0)
					{//当前WIFI处于关闭状态
						BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2),"Scan");//扫描
					}
					else
					{
						BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2),"OK");//确定
					}					
				}
				break;	
			case GUI_KEY_RIGHT:
				temp[0] = 0;
				if(0 == RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
				{
					if(GET_WIFI_POWER_STATE() == 0)
					{
						CLOSE_WIFI_POWER();
						temp[0] = 1;
					}					
				}
				else
				{
					if(GET_WIFI_POWER_STATE() != 0)
					{
						OPEN_WIFI_POWER();
						temp[0] = 1;
					}					
				}
				if(temp[0] == 0)
				{//不需要保存设置
					if(0 != RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
					{//扫描当前WIFI网络
						ShowWifiNetInfoDlg();
					}
				}
				else
				{
					if(ERROR == SaveUserConfigParam())
					{
						ShowSDCardError();
						break;
					}
					if(0 == RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
					{
						ShowHelpToolMenuDlg();	
					}
					else
					{
						ShowWifiNetInitDlg();	
					}					
				}							
				break;
		}
		break;
	case WM_TIMER:
		switch(WM_GetTimerId(pMsg->Data.v))
		{
			case GUI_TIMER_500MS_ID:
				WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
				
				sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

				WM_InvalidateWindow(hWin);
				break;
			default:
				break;
		}		
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowWifiNetSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
