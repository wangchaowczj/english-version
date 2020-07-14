#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON0,     4, 270, 200, 50,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     40, 65, 70, 30,  0,          0 },
	{ TEXT_CreateIndirect, "年", GUI_ID_TEXT0,     110, 65, 24, 30,  0,          0 },	
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     134, 65, 50, 30,  0,          0 },	
	{ TEXT_CreateIndirect, "月", GUI_ID_TEXT1,     184, 65, 24, 30,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT2,     208, 65, 50, 30,  0,          0 },
	{ TEXT_CreateIndirect, "日", GUI_ID_TEXT2,     258, 65, 24, 30,  0,          0 },	
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT3,     40, 105, 70, 30,  0,          0 },
	{ TEXT_CreateIndirect, "时", GUI_ID_TEXT3,     110, 105, 24, 30,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT4,     134, 105, 50, 30,  0,          0 },
	{ TEXT_CreateIndirect, "分", GUI_ID_TEXT4,     184, 105, 24, 30,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT5,     208, 105, 50, 30,  0,          0 },
	{ TEXT_CreateIndirect, "秒", GUI_ID_TEXT5,     258, 105, 24, 30,  0,          0 },
	{ TEXT_CreateIndirect, "左右键切换,上下键回删", GUI_ID_TEXT8,     0, 140, 320, 30,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	int U16Value;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();		
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("时钟设置");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT8), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3),&NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT4),&NORMAL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT5),&NORMAL_FONT);

			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 4);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 2);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT2), 2);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 2);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT4), 2);			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT5), 2);
			
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT0), RTC_Type.year);
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT1), RTC_Type.month);
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT2), RTC_Type.day);
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT3), RTC_Type.hour);
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT4), RTC_Type.minute);
			InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin,GUI_ID_EDIT5), RTC_Type.second);			
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			WM_SetFocus(WM_GetClientWindow(hWin));
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					if(RESULT_YES == ShowCustomMessageBox("确认取消修改?", "时钟设置", MB_YESNO))
					{
						ShowSystemSetDlg();					
					}
					break;
				case GUI_CUSTOM_KEY_OK:
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) <= 2)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);							
					RTC_Type_Temp.year = U16Value;
					if((RTC_Type_Temp.year < 2000) || (RTC_Type_Temp.year > 2099))
					{
						ShowCustomMessageBox("项目(年)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) == 0)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);						
					RTC_Type_Temp.month = U16Value;
					if((RTC_Type_Temp.month < 1) || (RTC_Type_Temp.month > 12))
					{
						ShowCustomMessageBox("项目(月)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) == 0)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);				
					RTC_Type_Temp.day = U16Value;					
					if((RTC_Type_Temp.day == 0) || (RTC_Type_Temp.day > RTC_Get_MaxDay(RTC_Type_Temp.year, RTC_Type_Temp.month)))
					{
						ShowCustomMessageBox("项目(日)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT3), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) == 0)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);				
					RTC_Type_Temp.hour = U16Value;
					if(RTC_Type_Temp.hour > 23)
					{
						ShowCustomMessageBox("项目(时)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT4), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) == 0)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);				
					RTC_Type_Temp.minute = U16Value;
					if(RTC_Type_Temp.minute > 59)
					{
						ShowCustomMessageBox("项目(分)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT5), (char*)OpData.GuiTemp, 10);
					if(strlen((char*)OpData.GuiTemp) == 0)
					{
						ShowCustomMessageBox("请输入所有项目!", "时钟设置", MB_OK);
						return;						
					}					
					sscanf((char*)OpData.GuiTemp, "%d", &U16Value);				
					RTC_Type_Temp.second = U16Value;
					if(RTC_Type_Temp.second > 59)
					{
						ShowCustomMessageBox("项目(秒)输入错误!", "时钟设置", MB_OK);
						return;
					}
					
					if(RESULT_YES != ShowCustomMessageBox("确认修改时间?", "时钟设置", MB_YESNO))
					{
						return;
					}					
					RTC_Write_Time();
					if(RESULT_OK == ShowCustomMessageBox("设置成功!", "时钟设置", MB_OK))
					{
						ShowSystemSetDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F3:				
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{//年
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 3)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}
					}
					else
					{
						if(MULTIEDIT_GetTextSize(OpData.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(OpData.CurrentInput, GUI_KEY_BACKSPACE);
						}
					}							
					break;
				case GUI_CUSTOM_KEY_F4:	//左
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT5);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT4))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
					}
					else
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT4);
					}
					ShowInputCursor(OpData.CurrentInput, 1);
					WM_SetFocus(WM_GetClientWindow(hWin));
					break;
				case GUI_CUSTOM_KEY_F2:	//右
					ShowInputCursor(OpData.CurrentInput, 0);
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT2))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT3))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT4);
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT4))
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT5);
					}
					else
					{
						 OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
					}
					ShowInputCursor(OpData.CurrentInput, 1);
					WM_SetFocus(WM_GetClientWindow(hWin));
					break;
				case GUI_CUSTOM_KEY_0:
				case GUI_CUSTOM_KEY_1:
				case GUI_CUSTOM_KEY_2:
				case GUI_CUSTOM_KEY_3:
				case GUI_CUSTOM_KEY_4:
				case GUI_CUSTOM_KEY_5:
				case GUI_CUSTOM_KEY_6:
				case GUI_CUSTOM_KEY_7:
				case GUI_CUSTOM_KEY_8:
				case GUI_CUSTOM_KEY_9:
					if(WM_IsWindow(OpData.CurrentInput))
					{
						ShowInputCursor(OpData.CurrentInput, 0);
						MULTIEDIT_AddKey(OpData.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);					
					}
					break;					
			}
			break;			
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					if(WM_IsWindow(OpData.CurrentInput))
					{
						ShowCursor(OpData.CurrentInput);
					}
					break;					
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}


void ShowRtcSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}


