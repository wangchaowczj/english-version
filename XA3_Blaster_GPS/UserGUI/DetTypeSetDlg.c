#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     10, 112, 17,  80,  0,          0X1E02 }
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
		ShowInfoTitle("DET Type");//雷管类型
		sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);
		GetDetInfoString(&temp[40], Det.Type);
		ShowInfoHelp(&temp[0], &temp[40]);
		
		//RADIO字体设置有错位现象，采用变通方式，文字自绘
	  
		GUI_SetFont(&NORMAL_FONT);
	  GUI_DispStringAt("1.XA06(0-6000ms)", 30,105);
    GUI_DispStringAt("2.XA21(0-21000ms)", 30,135);
		break;
	case WM_INIT_DIALOG://初始化界面
		hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
		
		RADIO_SetTextColor(hitem,device_info.TextColor);
		RADIO_SetBkColor(hitem, device_info.BackgroundColor);
		if(Det.Type == DET_TYPE_XA06)
		{
			RADIO_SetValue(hitem, 0);
		}
		else if(Det.Type == DET_TYPE_XA21)
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
			case GUI_KEY_1:
				hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
				if(0 != RADIO_GetValue(hitem))
				{
					WM_InvalidateWindow(hWin);
					if(0 == Det.RegCount)
					{
						Det.Type = DET_TYPE_XA06;
						if(ERROR != SaveUserConfigParam())
						{							
							Det.DlyMax = GetDetMaxDelay(Det.Type);
							RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);							
						}
						else
						{
                            ShowSDCardError();					
						}
					}
					else
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("DET Type");//雷管类型
                        SetMsgBoxMessage("Before modifying the Chip\n\nTypeCode must del all DETs!");//修改雷管类型前 \n\n必需删除所有雷管!
                        sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        GetDetInfoString(&temp[0], Det.Type);
                        SetMsgBoxHelp2(temp);
                        SetMsgBoxLeft("Back");
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowDetTypeSetDlg();
                        }
					}
				}
				break;				
			case GUI_KEY_2:
				hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
				if(1 != RADIO_GetValue(hitem))
				{
					WM_InvalidateWindow(hWin);
					if(0 == Det.RegCount)
					{
						Det.Type = DET_TYPE_XA21;
						if(ERROR != SaveUserConfigParam())
						{							
							Det.DlyMax = GetDetMaxDelay(Det.Type);
							RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);							
						}
						else
						{
                            ShowSDCardError();					
						}
					}
					else
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("DET Type");//雷管类型
                        SetMsgBoxMessage("Before modifying the Chip\n\nTypeCode must del all DETs!");//修改雷管类型前 \n\n必需删除所有雷管!
                        sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        GetDetInfoString(&temp[0], Det.Type);
                        SetMsgBoxHelp2(temp);
                        SetMsgBoxLeft("Back");
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowDetTypeSetDlg();
                        }
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
				break;
			default:
				break;
		}		
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowDetTypeSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
