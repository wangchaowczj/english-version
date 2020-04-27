#include "includes.h"
#include "GuiIncludes.h"

/*界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     162, 113, 40, 20,  0,          0 }
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

		ShowInfoTitle("Logging PARM");//注册参数

		ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键		
		GUI_SetFont(&SMALL_FONT);	

		GUI_DispStringAt("Loading QTY:",40,115);//带载数量:
		break;
	case WM_INIT_DIALOG://初始化界面
		MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 2);
		
		MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
		
		InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), SlaveInfo.Number);
				
		ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 1);	

		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);		
		break;
		//这个是接收键盘上的按键的相应
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT://取消修改				
				ShowSystemSetMenuDlg();				
				break;
			case GUI_KEY_RIGHT://右键回删
				if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
				{						
					MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GUI_KEY_BACKSPACE);
				}									
				break;					
			case GUI_KEY_ENTER://回车键确认
				if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
				{                 
					ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
					Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
					if(Value == 0)
					{                       
                        MsgBoxInit();
                        SetMsgBoxCaption("Loading QTY");//帯载数量
                        SetMsgBoxMessage("Loading QTY can't be 0");//带载数量不能为0
                        SetMsgBoxLeft("Back");

                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowSlaveNumberSetDlg();
                        }
						break;
					}
					if(Value > SLAVE_NUMBER_MAX)					
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("Loading QTY");//带载数量
                        sprintf((char*)temp,"Loading QTY <= %d",SLAVE_NUMBER_MAX);//带载数量不能大于
                        SetMsgBoxMessage(temp);
                        SetMsgBoxLeft("Back");

                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowSlaveNumberSetDlg();
                        }								
						break;
					}
					SlaveInfo.Number = (u32)Value;
					if(SaveUserConfigParam() == ERROR)
					{
						ShowSDCardError();
						break;
					}
                    MsgBoxInit();
                        
					sprintf((char*)temp,"Loading QTY:%d,ADD:A...%c",//带载数量:%d,地址范围:
						SlaveInfo.Number, 
						(SlaveInfo.Number<=26)?'A'+(SlaveInfo.Number-1):'a'+(SlaveInfo.Number-27));
                        
                    SetMsgBoxCaption("Loading QTY");//带载数量
                    SetMsgBoxMessage(temp);
                    SetMsgBoxLeft("Back");
                    if(ShowMsgBox() == GUI_KEY_LEFT)
                    {
                        ShowSystemSetMenuDlg();
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
				ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
				MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
				break;				
		}
		break;
	case WM_TIMER:
		switch(WM_GetTimerId(pMsg->Data.v))
		{
			case GUI_TIMER_500MS_ID:
				WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
				
				ShowCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0));			
				break;
			default:
				break;
		}		
		break;		
	default:
		WM_DefaultProc(pMsg);
	}
}

void ShowSlaveNumberSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
