#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { LISTBOX_CreateIndirect,   "",       GUI_ID_LISTBOX0,     15, INFO_TITLE_LINE_Y+20, 200, 148, 0, 0 }
};

u8 Pageindex = 0;
const static u8 PageSize = 6;

static void ShowListBoxData(LISTBOX_Handle hObj, u8 Pageindex)
{
	u8 i;
	u8 Index;
	while(LISTBOX_GetNumItems(hObj) != 0)
	{
		LISTBOX_DeleteItem(hObj, 0);
	}
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < 26)
		{
			sprintf((char*)Op_Data.Temp,"%c",'A'+Index);
			LISTBOX_AddString(hObj, (char*)Op_Data.Temp);				
		}
		else
		{
			break;
		}
	}	
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	SCROLLBAR_Handle hScrollbar;
	u8 Index = 0;
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("Device No.");//设备序号
		sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
		GetDetInfoString(&temp[40], Det.Type);
		ShowInfoHelp(&temp[0], &temp[40]);
		break;
	case WM_INIT_DIALOG://初始化界面
		Index = device_info.name-'A';
		Pageindex = Index/PageSize;
	
		hitem = WM_GetDialogItem(hWin, GUI_ID_LISTBOX0);
		LISTBOX_SetAutoScrollV(hitem, 0);		
		LISTBOX_SetFont(hitem, &NORMAL_FONT);
		LISTBOX_SetScrollbarColor(hitem, SCROLLBAR_CI_THUMB, GUI_BLUE);//缩略图区域的颜色。

		LISTBOX_SetBkColor(hitem, LISTBOX_CI_UNSEL, GUI_WHITE);//没有选中的行为为白底
		LISTBOX_SetBkColor(hitem, LISTBOX_CI_SEL, GUI_BLUE);//选中的行为蓝底

		//滚动条设置
		SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色		
		hScrollbar = SCROLLBAR_Create(215, INFO_TITLE_LINE_Y+20, 10, 148, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
		
		SCROLLBAR_SetNumItems(hScrollbar, 26);
		SCROLLBAR_SetValue(hScrollbar, Index);	
		SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行
		
		
		ShowListBoxData(hitem, Pageindex);
		LISTBOX_SetSel(hitem, Index%PageSize);

		if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
		}
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:
				Index = Pageindex*PageSize + LISTBOX_GetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0));
				device_info.name = 'A'+ Index;
				if(ERROR != SaveUserConfigParam())
				{							
					ShowSystemSetMenuDlg();					
				}				
				else
				{
					ShowSDCardError();
				}					
				break;
			case GUI_KEY_2:
				Index = Pageindex*PageSize + LISTBOX_GetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0));
				Index--;
				if(Index >= 26)
				{
					Index = 25;
				}				
				if((Index/PageSize) != Pageindex)
				{//跨页
					Pageindex = Index/PageSize;//当前所在页
					SCROLLBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0), Pageindex*PageSize);
					
					ShowListBoxData(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Pageindex);
				}
				LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Index%PageSize);	
				break;				
			case GUI_KEY_4:
				Pageindex--;
				if((PageSize*Pageindex) >= 26)
				{
					Pageindex = 25/PageSize;
				}
				Index = PageSize*Pageindex;
				ShowListBoxData(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Pageindex);	
				LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Index%PageSize);
				SCROLLBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0), Pageindex*PageSize);
				break;
			case GUI_KEY_6:
				Pageindex++;
				if((PageSize*Pageindex) >= 26)
				{
					Pageindex = 0;
				}
				Index = PageSize*Pageindex;
				ShowListBoxData(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Pageindex);	
				LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Index%PageSize);
				SCROLLBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0), Pageindex*PageSize);
				break;
			case GUI_KEY_8:
				Index = Pageindex*PageSize + LISTBOX_GetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0));
				Index++;
				if(Index >= 26)
				{
					Index = 0;
				}				
				if((Index/PageSize) != Pageindex)
				{//跨页
					Pageindex = Index/PageSize;//当前所在页
					SCROLLBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0), Pageindex*PageSize);
					
					ShowListBoxData(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Pageindex);
				}
				LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0), Index%PageSize);
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

void ShowDeviceNameSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
