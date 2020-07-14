#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1},
	{ BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
	{ TEXT_CreateIndirect, "请插入U盘...", GUI_ID_TEXT0,     0, 70, LCD_WIDTH_PIXEL, 30,  0,          0 }	
};
/**********************************************************
对话框参数:
0 拷入HEX文件
1 输出记录文件到U盘  
***********************************************************/
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
			WindowTitleUpdate("检测U盘");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0) ,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_1MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:			
					if(0 == GetWindowParam())
					{//拷入HEX文件
						ShowSystemToolDlg();
					}
					else if(1 == GetWindowParam())
					{//输出记录文件到U盘
						ShowDetTestRecorderListDlg();
					}                  
					else
					{
						ShowMainMenuDlg();//返回	
					}
					break;				
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:										
					if ((GetHostEnumState() == ENUM_SUCCESS) && (SUPPORT_DEVICE_MSC == HostInfo.DeviceType))
					{
						WM_DeleteTimer(pMsg->Data.v);
						
						if(0 == GetWindowParam())
						{//拷入HEX文件
							ShowHexFileImportDlg();
						}
						else if(1 == GetWindowParam())
						{//输出记录文件到U盘
							ShowFileExportToMscDlg(0);
						}
						else if(2 == GetWindowParam())
						{//输出三码绑定文件到U盘
							ShowFileExportToMscDlg(1);
						}
						else
						{
							ShowMainMenuDlg();
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


void ShowWaitDiskInsertDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}


