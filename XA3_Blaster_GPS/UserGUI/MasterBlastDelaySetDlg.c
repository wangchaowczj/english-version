#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
  { BUTTON_CreateIndirect, "Save", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }, //保存
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     40, 112, 17,  80,  0,          2 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("Device Delaytime");//起爆延时
		sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:%
		GetDetInfoString(&temp[40], Det.Type);
		ShowInfoHelp(&temp[0], &temp[40]);
		
		//RADIO字体设置有错位现象，采用变通方式，文字自绘
		GUI_DispStringAt("1.Delaytime 0s", 61,110);//1.起爆延时0秒
		GUI_DispStringAt("2.Delaytime 5s", 61,130);//2.起爆延时5秒
		break;
	case WM_INIT_DIALOG://初始化界面
		hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
		
		RADIO_SetTextColor(hitem,device_info.TextColor);
		RADIO_SetBkColor(hitem, device_info.BackgroundColor);
		if(device_info.BlastDelay == 0)
		{
			RADIO_SetValue(hitem, 0);
		}
		else
		{
			RADIO_SetValue(hitem, 1);
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
				device_info.BlastDelay = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
				if(ERROR != SaveUserConfigParam())
				{							
					ShowSystemSetMenuDlg();						
				}
				else
				{
					ShowSDCardError();						
				}
				break;				
			case GUI_KEY_1:
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
				break;				
			case GUI_KEY_2:
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
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

void ShowMasterBlastDelaySetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
