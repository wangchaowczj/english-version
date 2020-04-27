#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Cancel",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Save",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     40, 100, 17,  80,  0,          0X1E03 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	u16 Value;
	u8 U8Value;
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("Device Type");//使用模式
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			sprintf(&temp[0], "Decode:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
			sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);		//注册雷管:%u 错误雷管:			
		}
		else
		{
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);////注册雷管:%u 错误雷管:	
			GetDetInfoString(&temp[40], Det.Type);							
		}
		ShowInfoHelp(&temp[0], &temp[40]);
		
		//RADIO字体设置有错位现象，采用变通方式，文字自绘
		GUI_SetFont(&NORMAL_FONT);
		GUI_DispStringAt("1.Controller", 57,95);//1.控制器
		GUI_DispStringAt("2.Decoder(S)", 57,125);//2.译码器(从机)
		GUI_DispStringAt("3.Encoder(M)", 57,155);//3.编码器(主机)
		break;
	case WM_INIT_DIALOG://初始化界面
		hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
		
		RADIO_SetTextColor(hitem,device_info.TextColor);
		RADIO_SetBkColor(hitem, device_info.BackgroundColor);
		if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
		{
			RADIO_SetValue(hitem, 0);
		}
		else if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
		{
			RADIO_SetValue(hitem, 1);
		}
		else if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			RADIO_SetValue(hitem, 2);
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
		}
		else
		{
			RADIO_SetValue(hitem, 0);
		}
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));

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
				ShowSystemSetMenuDlg();
				break;
			case GUI_KEY_RIGHT:
				Value = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
				if (((0 == Value) && (DEVICE_USAGES_ALONE != device_info.DeviceUsages)) ||
					((1 == Value) && (DEVICE_USAGES_SLAVE != device_info.DeviceUsages)) ||
					((2 == Value) && (DEVICE_USAGES_MASTER != device_info.DeviceUsages)))
				{
					U8Value = device_info.DeviceUsages;
					if(0 == Value)
					{
						device_info.DeviceUsages = DEVICE_USAGES_ALONE;
					}
					else if(1 == Value)
					{
						device_info.DeviceUsages = DEVICE_USAGES_SLAVE;
					}
					else
					{
						device_info.DeviceUsages = DEVICE_USAGES_MASTER;
					}
                    
					if(ERROR != SaveUserConfigParam())
					{
                        device_info.DeviceUsages = U8Value;//避免屏幕改变表号的显示状态
                        MsgBoxInit();
                        SetMsgBoxCaption("Device Type");//使用模式
                        SetMsgBoxMessage("Setting Completed\n\nTake effect after restart!");//设置成功\n\n重启设备后生效!

                        ShowMsgBox();
					}
					else
					{                        
						ShowSDCardError();												
					}
				}
				break;
			case GUI_KEY_1://独立设备
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
				if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
				{
					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				}
				else
				{
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				}
				break;				
			case GUI_KEY_2://从机
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
				if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
				{
					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				}
				else
				{
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				}
				break;
			case GUI_KEY_3://主机
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
				if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
				{
					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				}
				else
				{
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
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
				break;
			default:
				break;
		}		
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowDeviceUsagesSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
