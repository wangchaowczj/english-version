#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },  
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
};

static MSG_DLG MsgDlg;

void InitMessageBoxDlg(void)
{
	memset(&MsgDlg, 0, sizeof(MsgDlg));
	MsgDlg.Color = device_info.TextColor;
}
void SetMessageBoxDlgTitle(char* Title)
{
	if((Title != NULL) && ((strlen(Title)+1) <= sizeof(MsgDlg.TitleBuffer)))
	{
		strcpy(MsgDlg.TitleBuffer, Title);
	}
}
void SetMessageBoxDlgMsgColor(int color)
{
	MsgDlg.Color = color;
}
void SetMessageBoxDlgHelp1(char* Help1)
{
	if((Help1 != NULL) && ((strlen(Help1)+1) <= sizeof(MsgDlg.Help1Buffer)))
	{
		strcpy(MsgDlg.Help1Buffer, Help1);
	}
}
void SetMessageBoxDlgHelp2(char* Help2)
{
	if((Help2 != NULL) && ((strlen(Help2)+1) <= sizeof(MsgDlg.Help2Buffer)))
	{
		strcpy(MsgDlg.Help2Buffer, Help2);
	}
}
void SetMessageBoxDlgMsg(char* Msg)
{
	if((Msg != NULL) && ((strlen(Msg)+1) <= sizeof(MsgDlg.MsgBuffer)))
	{
		strcpy(MsgDlg.MsgBuffer, Msg);
	}
}
void SetMessageBoxDlgBtLeftText(char* Text)
{
	if((Text != NULL) && ((strlen(Text)+1) <= sizeof(MsgDlg.BtLeftText)))
	{
		strcpy(MsgDlg.BtLeftText, Text);
	}
}
void SetMessageBoxDlgBtRightText(char* Text)
{
	if((Text != NULL) && ((strlen(Text)+1) <= sizeof(MsgDlg.BtRightText)))
	{
		strcpy(MsgDlg.BtRightText, Text);
	}
}
void SetMessageBoxDlgBtLeftFun(DlgFun FunLeft)
{	
	MsgDlg.FunLeft = FunLeft;
}
void SetMessageBoxDlgBtRightFun(DlgFun FunRight)
{
	MsgDlg.FunRight = FunRight;
}
void SetMessageBoxDlgBtEnterFun(DlgFun FunEnter)
{
	MsgDlg.FunEnter = FunEnter;
}
void SetMessageBoxDlgEnterNextFun(DlgFun FunNext)
{
	MsgDlg.FunNext = FunNext;
}
void SetMessageBoxDlgBackgroundFun(DlgFun Background)
{
	MsgDlg.FunBackground = Background;
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			if (0x00 != MsgDlg.TitleBuffer[0])
			{
				ShowInfoTitle(MsgDlg.TitleBuffer);		
			}

			if ((0x00 != MsgDlg.Help1Buffer[0]) || (0x00 != MsgDlg.Help2Buffer[0]))
			{
				ShowInfoHelp((MsgDlg.Help1Buffer[0] == 0x00)?NULL:MsgDlg.Help1Buffer,
					(MsgDlg.Help2Buffer[0] == 0x00)?NULL:MsgDlg.Help2Buffer);		
			}

			GUI_SetFont(&SMALL_FONT);
			GUI_SetColor(MsgDlg.Color);
			GUI_DispStringHCenterAt(MsgDlg.MsgBuffer,120,100);
			break;
		case WM_INIT_DIALOG://初始化界面
			if(MsgDlg.FunLeft == NULL)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			}
			else
			{
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), MsgDlg.BtLeftText);
			}
			
			if(MsgDlg.FunRight == NULL)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
			}
			else
			{
				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2), MsgDlg.BtRightText);
			}
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
				case GUI_KEY_LEFT:
					if(MsgDlg.FunLeft != NULL)
					{										
						MsgDlg.FunLeft();
						InitMessageBoxDlg();
					}
					break;
				case GUI_KEY_RIGHT:
					if(MsgDlg.FunRight != NULL)
					{
						MsgDlg.FunRight();	
						InitMessageBoxDlg();
					}
					break;
				case GUI_KEY_ENTER:
					if(MsgDlg.FunEnter != NULL)
					{
						MsgDlg.FunEnter();	
						InitMessageBoxDlg();
					}
					break;					
			}
			break;
		case WM_BACKGROUND_TASK_END:
			if(MsgDlg.FunBackground != NULL)
			{			
				MsgDlg.FunBackground();
				InitMessageBoxDlg();			
			}
			break;		
		case WM_ENTER_NEXT_DLG:
			if(MsgDlg.FunNext != NULL)
			{			
				MsgDlg.FunNext();
				InitMessageBoxDlg();			
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

void ShowMessageBoxDlg(void)
{
	if(WM_IsWindow(Op_Data.hWin))
	{
		GUI_EndDialog(Op_Data.hWin, 0);
	}
	Op_Data.hWin = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
