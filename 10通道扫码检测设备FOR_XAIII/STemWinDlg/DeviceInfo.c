#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     20, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     10, 30, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     10, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     10, 90, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     10, 120, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("设备信息");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_VCENTER);
			
			sprintf((char*)OpData.GuiTemp, "软件版本:%s", VERSION_STRING);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
						
			memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
			strcpy((char*)OpData.GuiTemp, "硬件版本:");
			memcpy((char*)&OpData.GuiTemp[strlen((const char*)OpData.GuiTemp)], DeviceInfo.HW, sizeof(DeviceInfo.HW));
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);	
			
			memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
			strcpy((char*)OpData.GuiTemp, "设备编号:");
			memcpy((char*)&OpData.GuiTemp[strlen((const char*)OpData.GuiTemp)], DeviceInfo.ID, sizeof(DeviceInfo.ID));
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);	

			if(AdcAdjust.Time[0] == 0)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "校准时间:未校准");	
			}
			else
			{
				memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
				strcpy((char*)OpData.GuiTemp, "校准时间:");
				memcpy((char*)&OpData.GuiTemp[strlen((const char*)OpData.GuiTemp)], AdcAdjust.Time, sizeof(AdcAdjust.Time));
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);				
			}
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:	
					ShowDeviceInfoMenuDlg();
					break;		
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDeviceInfoDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

