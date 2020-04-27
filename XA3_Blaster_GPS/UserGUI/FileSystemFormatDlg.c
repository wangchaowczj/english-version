#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 320, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, "TF Card File System Abnormal!", GUI_ID_TEXT0,     0, 90, 240,  20,  0,          0 },//TF卡文件系统异常
  { TEXT_CreateIndirect, "Formatting causes Dat lost!", GUI_ID_TEXT1,     0, 120, 240,  20,  0,          0 },//格式化将丢失数据,请慎重
	{ TEXT_CreateIndirect, "Pls Be Careful!", GUI_ID_TEXT1,     0, 150, 240,  20,  0,          0 }//格式化将丢失数据,请慎重
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);
            GUI_SetFont(&SMALL_FONT);        
			GUI_Clear();
                
            if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_TEXT1)))
            {
                GUI_DispStringHCenterAt(" ↙ Start Initialization", 120, 150);   //↙键开始格式化         
            }
			break;
		case WM_INIT_DIALOG://初始化界面
      TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
						
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));								
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_ENTER:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_TEXT1)))
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));                       
                        TaskCreate(APP_LOW_TASK_PRIO, (void*)0);
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Initilizating(15-20m)");//格式化中(15-20分钟)   
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                        WM_Paint(hWin);
                    }												
					break;
			}
			break;
		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					if(!TaskIsExist(APP_LOW_TASK_PRIO))
					{//已经初始完成
                        WM_DeleteTimer(pMsg->Data.v);
						if((ErrorStatus)Op_Data.AppLowTaskResult == ERROR)
						{//初始化成功
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Formatting Failed!");	//格式化失败,请重启设备	
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls Re-start Device!");	//格式化失败,请重启设备
						}
						else
						{
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Formatting Completed!");	//格式化成功,请重启设备	
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Pls Re-start Device!");	//格式化成功,请重启设备
						}
					}
					else
					{
            UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
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

void ShowFileSystemFormatDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
