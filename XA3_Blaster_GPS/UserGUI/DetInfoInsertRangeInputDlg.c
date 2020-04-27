#include "includes.h"
#include "GuiIncludes.h"

/*界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     100, 90, 80, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     100, 115, 80, 20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 150, 240, 20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u32 Value;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Dat Merge");	//数据合并
			ShowInfoHelp("Input:0...9","OK: ↙ ");		//输入:0...9键","确认:↙键
			
			GUI_SetFont(&SMALL_FONT);
			if(GetWindowParam() == 0)
			{
				GUI_DispStringAt("Dat Merge:M<-S",60,57);//数据合并:主机<-从机
			}
			else
			{
				GUI_DispStringAt("Dat Merge:M->S",60,57);	//数据合并:主机->从机
			}				
			
			GUI_DispStringAt("From:",60,92);//起始孔号:
			GUI_DispStringAt("To  :",60,117);//截止孔号:
			break;
		case WM_INIT_DIALOG://初始化界面
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
			
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 3);
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 3);

			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);	
			ShowInputCursor(DlghWin.CurrentInput, 1);
			
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
				case GUI_KEY_LEFT://取消				
					if((0 == GET_BLUE_POWER_STATUS()) && (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS()))
					{
						ShowDetBlueInsertMenuDlg();
					}
					else
					{
						ShowDetBackupOperateBlueTypeSelDlg();
					}														
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsWindow(DlghWin.CurrentInput))
					{
						if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
						{//当前正在输入截止孔
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "");
					}				
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
					{
						Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
						if((Value == 0) || (Value > DET_MAX_SIZE))
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Hole No. beyond the range!");//孔号超出范围!
							break;
						}
						if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
						{//正在输入起始孔						
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}						
						else
						{//输入完成
							ShowInputCursor(DlghWin.CurrentInput, 0);
							Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
							
							if(Value > GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Start Hole No.>Stop Hole No.!");		//起始孔号大于截止孔号!					
								break;
							}
							Op_Data.InputValue1 = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
							Op_Data.InputValue2 = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
							
							ShowDetInfoInsertStatusDlg(GetWindowParam());							
						}
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
					ShowInputCursor(DlghWin.CurrentInput, 0);
					MULTIEDIT_AddKey(DlghWin.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
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

					if(WM_IsWindow(DlghWin.CurrentInput))
					{
						ShowCursor(DlghWin.CurrentInput);
					}					
					break;
				default:
					break;
			}		
			break;	
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetInsertRangeInputDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
