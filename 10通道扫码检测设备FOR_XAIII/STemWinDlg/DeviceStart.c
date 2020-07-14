#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, 0,          0, 1},
	{ TEXT_CreateIndirect, "欢迎使用扫码设备", GUI_ID_TEXT0,     0, 60, LCD_WIDTH_PIXEL,  30,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 90, LCD_WIDTH_PIXEL,  30,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 120, LCD_WIDTH_PIXEL,  30,  0,          0 },
	{ BUTTON_CreateIndirect,   "确定",       GUI_ID_BUTTON0,     120, 180,80 ,  30,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 Result = 0;	
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);				
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
            
//			WM_Paint(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
			WM_PaintWindowAndDescs(hWin);
			
			LcdSetBright(UserParam.LcdBright);
			Result = SystemHardwareInit();
			InitBatteryDisplayValue(GetBatVoltage(NULL));//初始化完成后测到的电压比较准
			if(0 == Result)
			{//初始化正常
				CLOSE_LED1();
				CLOSE_LED2();
				switch(UserParam.Stage)
				{
					case FINISHED:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "成品检");	
						break;
					case SEMIFINISHED:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "半成品检");	
						break;
					case DET_CODE_VERIFY:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码验证");	
						break;
					case ENTER_FACTORY:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "入厂检");	
						break;                       
				}
				
				RTC_Read_Time();
				sprintf((char*)OpData.GuiTemp,"%04d-%02d-%02d", RTC_Type.year,RTC_Type.month,RTC_Type.day);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);
				
                WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
//				WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2000MS_ID, GUI_TIMER_2000MS, 0);									
			}
			else 
			{
				if(1 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "RTC异常!");
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
				}
				else if(2 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "请插入TF卡!");			
				}				
				else if(3 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "TF卡文件系统异常!");			
				}
				else if(4 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "USB异常!");
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
				}
				else if(5 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "电源管理系统异常!");
				}
				else if(6 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "充电管理系统异常!");
				}					
				else
				{
					sprintf((char*)OpData.GuiTemp,"设备异常(代码:%d)", Result);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
				}				
			}		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
//				case GUI_KEY_RIGHT:
//					break;	
				case GUI_CUSTOM_KEY_OK:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
                    {
                        CLOSE_LED1();				
                        if(WM_IsWindow(DlghWin.Status))
                        {
                            WM_ShowWindow(DlghWin.Status);
                        }
                        SetSystemSleepCount(OSTimeGet());
						OpData.DetDetectorPassCount = UserParam.DetDetectorPassCount;
                        ShowChannelTestPrepareDlg();//ShowMainMenuDlg();	
												
                    }
                    break; 

			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_2000MS_ID:
		
					break;
				default:
					break;
			}		
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDeviceStartDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
