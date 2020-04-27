#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "返回",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "回删", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     122, 110, 70,  20,  0,          0 }
};

	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;

	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("雷管抽检");
		ShowInfoHelp("输入:0...9键","确认:  键");	
				
		ShowEnterIcon((LCDP_MAX_X-GetUtf8StrCharsCount("确认:  键")*8)/2+8*5, INFO_HELP_Y2);
		
		GUI_SetFont(&SMALL_FONT);	
		GUI_DispStringAt((const char*)"抽检数量:",50 , 112);
		break;
	case WM_INIT_DIALOG://初始化界面	
		MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 5);	
		MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);		
		
		InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0), Op_Data.DelaySampleDetCount);
		
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
		break;

		//这个是接收键盘上的按键的相应
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT://左键返回
				ShowDelaySampleProcessDetTypeSetDlg();			
				break;
			case GUI_KEY_RIGHT://右键回删
				if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
				{						
					MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GUI_KEY_BACKSPACE);
				}			
				break;					
			case GUI_KEY_ENTER://回车键确认
				if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)//没有输入时返回1
				{
					ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
					Value = GetUserValueFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));	
					if((Value == 0) || (Value > 30))
					{
						SetMessageBoxDlgTitle("雷管抽检");
						SetMessageBoxDlgMsg("数量范围:1~30\n\n请重新输入!");
						SetMessageBoxDlgHelp1(" ");
						SetMessageBoxDlgBtLeftText("返回");
						SetMessageBoxDlgBtLeftFun(&ShowDelaySampleProcessDetCountInputDlg);
						ShowMessageBoxDlg();							
						break;
					}
					Op_Data.DelaySampleDetCount = (u16)Value;
				
					ShowDelaySampleProcessDetTypeSetDlg();							
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
				
				sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

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

void ShowDelaySampleProcessDetCountInputDlg(void)
{
	if(WM_IsWindow(Op_Data.hWin))
	{
		GUI_EndDialog(Op_Data.hWin, 0);
	}	
	Op_Data.hWin = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


