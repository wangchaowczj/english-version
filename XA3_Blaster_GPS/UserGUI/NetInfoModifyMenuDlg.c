#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { TEXT_CreateIndirect, "1.Hole No. Inc  ", GUI_ID_TEXT0,     20, 85, 200,  20,  0,          0 },//1.孔号递增
  { TEXT_CreateIndirect, "2.Hole No. Dec  ", GUI_ID_TEXT1,     20, 110, 200,  20,  0,          0 },//2.孔号递减
  { TEXT_CreateIndirect, "3.Sort Hole No. ", GUI_ID_TEXT2,     20, 135, 200,  20,  0,          0 },//3.孔号排序
  { TEXT_CreateIndirect, "4.Sort Delaytime", GUI_ID_TEXT3,     20, 160, 200,  20,  0,          0 }//4.延期排序
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
			ShowInfoTitle("Date Shift");	//数据移动		
									
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);						
			break;
		case WM_INIT_DIALOG://初始化界面
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
      TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					ShowNetInfoDlg(0);
					break;	
				case GUI_KEY_1:
					ShowNetInfoModifyRangeInputDlg(0);
					break;
				case GUI_KEY_2:
					ShowNetInfoModifyRangeInputDlg(1);
					break;
				case GUI_KEY_3:
					device_info.NetInfoSort = 0;
                    SaveUserConfigParam();
					ShowNetInfoDlg(GetWindowParam());
					break;
				case GUI_KEY_4:
					device_info.NetInfoSort = 1;
                    SaveUserConfigParam();
					ShowNetInfoDlg(GetWindowParam());
					break;						
				default:
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

void ShowNetInfoModifyMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}