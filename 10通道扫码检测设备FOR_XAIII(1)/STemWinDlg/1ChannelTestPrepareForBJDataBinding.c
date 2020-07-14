#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "请接入雷管...",       GUI_ID_TEXT0,     0, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     50, 80, 156,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     206, 80, 60,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     0, 110, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "F1键起始管码 F2键盒流水",       GUI_ID_TEXT4,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 }
//  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT5,     50, 80, 170,  25,  0,          0 },
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			GetStageDesString((char*)OpData.GuiTemp);
            WindowTitleUpdate((char*)OpData.GuiTemp);
            
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
            
            ConnectChannel(0);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
				case GUI_CUSTOM_KEY_FOOT:					
                    DetTestStart();  
                    break;
				case GUI_CUSTOM_KEY_BACK:
					OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
					ShowMainMenuDlg();
					break;
				case GUI_CUSTOM_KEY_F1:
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    ShowDetOutsideForBJDlg(1);
					break;
				case GUI_CUSTOM_KEY_F2:
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    ShowDetOutsideBoxIndexForBJDlg(1);
					break;				
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);

                    if((UserParam.DetOutsideStartBoxIndex/100) > 999)
                    {
                        ShowCustomMessageBox("请重新设置起始管码", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }
                    if(GetCurrentDetOutsideStart(OpData.GuiTemp) == ERROR)
                    {
                        ShowCustomMessageBox("起始管码错误!", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }

                    if((UserParam.DetOutsideStartBoxIndex%100) >= UserParam.InBoxNumber)
                    {
                        ShowCustomMessageBox("盒内数量错误", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }
                    CreateChannelDetOutside();
                    
                    sprintf((char*)OpData.GuiTemp, "管码:%s", Channel.BarCode[0].CodeExternal);
                    OpData.GuiTemp[strlen((char*)OpData.GuiTemp)-5] = 0x00;
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)&Channel.BarCode[0].CodeExternal[8]);
                    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2), GUI_RED);
                    
					WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS,0);
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS,0);                    
					break;
				case GUI_TIMER_120MS_ID:					
                    if(BJ_DetCheckIn() != ERROR)
                    {
                        DetTestStart();
                    }
                    else
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                    }							
					break;
				case GUI_TIMER_500MS_ID:				
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);			

					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    
                    Voltage = GetLinABVoltage(0, NULL);
                    Current = GetCurrentValue(10, NULL);
                    
                    sprintf((char*)OpData.GuiTemp, "%0.1fV/%0.2fuA", Voltage/1000.0, Current/100.0);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);
					break;                    
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void Show1ChannelTestPrepareForBJDataBindingDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

