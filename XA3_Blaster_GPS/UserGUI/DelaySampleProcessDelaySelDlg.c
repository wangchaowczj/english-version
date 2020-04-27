#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { BUTTON_CreateIndirect, "Others", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//其它
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
		ShowInfoTitle("DET Delaytime");//雷管延期
		sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:   错误雷管:
		GetDetInfoString(&temp[40], Det.Type);
		ShowInfoHelp(&temp[0], &temp[40]);
			
		GUI_DispStringAt("1.0ms", 90,100);
		GUI_DispStringAt("2.150ms", 90,120);
		sprintf(&temp[0], "3.%dms",Det.DlyMax);
		GUI_DispStringAt(temp, 90,140);
		break;
	case WM_INIT_DIALOG://初始化界面
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:
				ShowDelaySampleProcessWaitRegDetDlg();
				break;
			case GUI_KEY_RIGHT://右键
				ShowDelaySampleProcessDelayInputDlg();			
				break;
			case GUI_KEY_1:
				Op_Data.DelaySampleDelay = 0;
				SetRegDetDelay(Op_Data.DelaySampleDelay);
				if(ERROR == SaveAllDetInfo(0))
				{
					ShowSDCardError();
					break;
				}				
				ShowDelaySampleProcessWaitRegDetDlg();
				break;				
			case GUI_KEY_2:
				Op_Data.DelaySampleDelay = 150;
				SetRegDetDelay(Op_Data.DelaySampleDelay);
				if(ERROR == SaveAllDetInfo(0))
				{
					ShowSDCardError();
					break;
				}				
				ShowDelaySampleProcessWaitRegDetDlg();
				break;		
			case GUI_KEY_3:
				Op_Data.DelaySampleDelay = Det.DlyMax;
				SetRegDetDelay(Op_Data.DelaySampleDelay);
				if(ERROR == SaveAllDetInfo(0))
				{
					ShowSDCardError();
					break;
				}				
				ShowDelaySampleProcessWaitRegDetDlg();
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

void ShowDelaySampleProcessDelaySelDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
