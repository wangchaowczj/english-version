#include "includes.h"
#include "GuiIncludes.h"

/*界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "6.65V 15000uA", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     124, 100, 80, 20,  0,          0 },//114, 100, 80, 20,  0,          0 
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     124, 125, 80, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT2,     124, 150, 80, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT3,     124, 175, 80, 20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	u32 Value;
	char temp[50];
	u8 Result, State;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Modify PWD");	//密码修改
			ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键
		
			GUI_SetFont(&SMALL_FONT);
			if(Op_Data.DKeyVoltage != 0xFFFF)
			{
				sprintf(temp, "Dongle Battery:%d%%", BatVoltageToAccumulated(Op_Data.DKeyVoltage));	//密钥电量:
				GUI_DispStringAt((const char*)temp,19 , 47);			
			}
			
			sprintf(temp, "Dongle Name:%s",BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name);	//密钥名
			GUI_DispStringAt((const char*)temp,19 , 72);			
			GUI_DispStringAt("User Name   :",19,102);//用户名:
			GUI_DispStringAt("Original PWD:",19,127);//旧密码:
			GUI_DispStringAt("New PWD     :",19,152);//新密码:
			GUI_DispStringAt("Comfirm     :",19,177);//确认码:
			break;
		case WM_INIT_DIALOG://初始化界面
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&SMALL_FONT);
			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3),&SMALL_FONT);
			
			hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			MULTIEDIT_SetMaxNumChars(hitem, 8);

			hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
			MULTIEDIT_SetMaxNumChars(hitem, 8);
			MULTIEDIT_SetPasswordMode(hitem, 1);
			
			hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
			MULTIEDIT_SetMaxNumChars(hitem, 8);
			MULTIEDIT_SetPasswordMode(hitem, 1);
			
			hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
			MULTIEDIT_SetMaxNumChars(hitem, 8);
			MULTIEDIT_SetPasswordMode(hitem, 1);
            
            Result = strlen((char*)BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name);
			if((Result >= 11) && (Result <= 12))
			{
				InitUserStringToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 
                    (char*)&BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[Result-8], 8);
			}
			
			DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);	
			ShowInputCursor(DlghWin.CurrentInput, 1);
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
			break;
			//这个是接收键盘上的按键的相应
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://取消				
					ShowSystemSetMenuDlg();														
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsWindow(DlghWin.CurrentInput))
					{
						if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
						{//当前正在输入旧密码
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
						{//当前正在输入新密码
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT3) == DlghWin.CurrentInput)
						{//当前正在输入确认码
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}					
					}				
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
					{
						if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
						{//正在输入用户名						
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
						{//正在输入新密码
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
						{//正在输入旧密码
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}						
						else
						{//确认码输入完成
							ShowInputCursor(DlghWin.CurrentInput, 0);
							Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
							
							if(Value != GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3)))
							{
                                MsgBoxInit();
                                SetMsgBoxCaption("Modify PWD");//密码修改
                                SetMsgBoxMessage("Passwords do not match!");//新密码不一致
                                SetMsgBoxLeft("Back");
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDkeyPasswdModifyDlg();
                                }							
								break;
							}
							memset(temp, 0, sizeof(temp));
							
							MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &temp[0], 10);						
							MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &temp[10], 10);						
							MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT2), &temp[20], 10);
										
							
							Result = BlueDkeyModifyUserInfo((u8*)&temp[0], (u8*)&temp[10], (u8*)&temp[0], (u8*)&temp[20], &State);
							if(Result == '1')							
							{
								OSTaskDel(APP_TEMPORARY_TASK_PRIO);
								CLOSE_BLUE_POWER();
                                
                                MsgBoxInit();
                                SetMsgBoxCaption("Modify PWD");//密码修改
                                SetMsgBoxMessage("Modification Completed!");//密码修改成功!
                                SetMsgBoxLeft("Back");
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowSystemSetMenuDlg();
                                }
							}
							else
							{
								if(ShowDkeyUserInfoError(Result, State) == GUI_KEY_LEFT)
                                {
                                    ShowDkeyPasswdModifyDlg();
                                }
							}	
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

void ShowDkeyPasswdModifyDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
