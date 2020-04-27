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
		ShowInfoTitle("System Settings");//系统设置

		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{			
			GUI_DispStringHCenterAt("1.Loading QTY      ",120, 50);//1.带载数量
			GUI_DispStringHCenterAt("2.Device Type      ",120, 70);//2.使用方式
			GUI_DispStringHCenterAt("3.Screen Light     ",120, 90);//3.屏幕亮度
      GUI_DispStringHCenterAt("4.Backlighting Time",120, 110);//4.背光时间   
			GUI_DispStringHCenterAt("5.Modify PWD       ",120, 130);//5.密码修改
			GUI_DispStringHCenterAt("6.Device Delaytime ",120, 150);//6.起爆延时
			GUI_DispStringHCenterAt("7.Screen Color     ",120, 170);//7.屏幕颜色"
      GUI_DispStringHCenterAt("8.Detection Method ",120, 190);//8.检测方式  			
		}
		else if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
		{
			GUI_DispStringHCenterAt("1.Net PARM         ",120,60);//1.孔网参数
			GUI_DispStringHCenterAt("2.Chip TypeCode    ",120,80);//2.雷管类型
			GUI_DispStringHCenterAt("3.Device Type      ",120,100);//3.使用方式
      GUI_DispStringHCenterAt("4.Device No.       ",120,120);//4.设备序号   
			GUI_DispStringHCenterAt("5.Backlighting Time",120,140);//5.背光时间
			GUI_DispStringHCenterAt("6.Screen Light     ",120,160);//6.屏幕亮度
			GUI_DispStringHCenterAt("7.Screen Color     ",120,180);//7.屏幕颜色
		
		}
		else
		{
			GUI_DispStringHCenterAt("1.Net PARM         ",120,50);//1.孔网参数
			GUI_DispStringHCenterAt("2.Chip TypeCode    ",120,70);//2.雷管类型
			GUI_DispStringHCenterAt("3.Device Type      ",120,90);//3.使用方式
      GUI_DispStringHCenterAt("4.Modify PWD       ",120,110);//4.密码修改
		  GUI_DispStringHCenterAt("5.Screen Light     ",120,130);//5.屏幕亮度
			GUI_DispStringHCenterAt("6.Backlighting Time",120,150);//6.背光时间
			GUI_DispStringHCenterAt("7.Device Delaytime ",120,170);//7.起爆延时
      GUI_DispStringHCenterAt("8.Screen Color     ",120,190);//8.屏幕颜色
		}		
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
			sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:%u 错误雷管:					
		}
		else
		{
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);							
		}
		ShowInfoHelp(&temp[0], &temp[40]);
		break;
	case WM_INIT_DIALOG://初始化界面
		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
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
				case GUI_KEY_1:
					ShowSlaveNumberSetDlg();					
					break;				
				case GUI_KEY_2:
					ShowDeviceUsagesSetDlg();				
					break;
				case GUI_KEY_3:
					ShowSliderDlg(0);				
					break;					
				case GUI_KEY_4:
					ShowSliderDlg(1);				
					break;
				case GUI_KEY_5:					
					ShowBlueHostStatusDlg(1);//搜索蓝牙密钥	
					break;
				case GUI_KEY_6:
//					ShowMasterBlastDelaySetDlg();
					ShowSliderDlg(2);
					break;
				case GUI_KEY_7:
					ShowColorSetDlg();
					break;
				case GUI_KEY_8:
					ShowMasterTimeShareSetDlg();
					break;					
			}		
		}
		else if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					ShowMainMenuMoreDlg();
					break;			
				case GUI_KEY_1:
					SetNetParamListViewDlgSelIndex(0);
					ShowNetParamListViewDlg();//设置网孔参数				
					break;
				case GUI_KEY_2:
					ShowDetTypeSetDlg();
					break;
				case GUI_KEY_3:
					ShowDeviceUsagesSetDlg();
					break;					
				case GUI_KEY_4:
					ShowDeviceNameSetDlg();	
					break;
				case GUI_KEY_5:
					ShowSliderDlg(1);	
					break;				
				case GUI_KEY_6:
					ShowSliderDlg(0);
					break;
				case GUI_KEY_7:
					ShowColorSetDlg();
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
				case GUI_KEY_1:
					SetNetParamListViewDlgSelIndex(0);
					ShowNetParamListViewDlg();//设置网孔参数				
					break;
				case GUI_KEY_2:
					ShowDetTypeSetDlg();
					break;
				case GUI_KEY_3:
					ShowDeviceUsagesSetDlg();
					break;						
				case GUI_KEY_4:					
					ShowBlueHostStatusDlg(1);//搜索蓝牙密钥
					break;				
				case GUI_KEY_5:
					ShowSliderDlg(0);	
					break;
				case GUI_KEY_6:
					ShowSliderDlg(1);					
					break;
				case GUI_KEY_7://设置使用模式
					ShowSliderDlg(2);								
					break;	
				case GUI_KEY_8:
					ShowColorSetDlg();
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

void ShowSystemSetMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
