#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
	{ BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
	{ TEXT_CreateIndirect, "文件导出中...", GUI_ID_TEXT0,     0, 90, LCD_WIDTH_PIXEL, 30,  0,          0 }
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
			WindowTitleUpdate("文件导出");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));		
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0) ,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			break;
			//这个是接收键盘上的按键的相应
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					if(!TaskIsExist(APP_BACKGROUND_TASK_PRIO))
					{
						ShowSystemToolDlg();
					}
					break;				
			}
			break;			
		case WM_TIMER:	
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);
                    if(GetWindowParam() == 0)
                    {//输出记录文件到U盘
                        TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)2);

                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    }
                    else if(GetWindowParam() == 1)
                    {//输出三码绑定文件到U盘
                        TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)3);

                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    }							
					break;
				case GUI_TIMER_500MS_ID:
					if(!TaskIsExist(APP_BACKGROUND_TASK_PRIO))
					{
						WM_DeleteTimer(pMsg->Data.v);				
						if((ErrorStatus)OpData.AppBackgroundTaskResult == ERROR)
						{//有其中一个文件导出成功
							if(RESULT_NULL == ShowCustomMessageBox("文件导出失败!", OpData.DlgTitle,MB_OK))
							{
								return;
							}
						}
						else
						{
							if(RESULT_NULL == ShowCustomMessageBox("文件导出成功!", OpData.DlgTitle,MB_OK))
							{
								return;
							}							
						}
                        if(GetWindowParam() == 0)
                        {//输出记录文件到U盘
                            ShowDetTestRecorderListDlg();
                        }
                        else
                        {
                            ShowSystemToolDlg();
                        }
					}
					else
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
						UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));					
					}
					break;
			}
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}


void ShowFileExportToMscDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}


