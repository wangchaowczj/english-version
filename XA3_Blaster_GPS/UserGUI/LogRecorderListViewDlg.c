#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回	
	{ BUTTON_CreateIndirect, "Upload", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//上传
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};

const static u8 PageSize = 9;

static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u8 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u8 LogDate[11];
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < LogRecorder.Number)
		{
			LISTVIEW_AddRow(hItem, NULL);
			
			sprintf((char*)Op_Data.Temp,"%d", Index+1);
			LISTVIEW_SetItemText(hItem, 0,i, (char*)Op_Data.Temp);
			
			LogDate[0] = '2';
			LogDate[1] = '0';
			LogDate[2] = LogRecorder.FileName[Index][0];
			LogDate[3] = LogRecorder.FileName[Index][1];
			LogDate[4] = '-';
			LogDate[5] = LogRecorder.FileName[Index][2];
			LogDate[6] = LogRecorder.FileName[Index][3];
			LogDate[7] = '-';
			LogDate[8] = LogRecorder.FileName[Index][4];
			LogDate[9] = LogRecorder.FileName[Index][5];
			LogDate[10] = 0x00;
			
			LISTVIEW_SetItemText(hItem, 1,i, (char*)LogDate);	
		}
	}
}

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;	
	static SCROLLBAR_Handle hScrollbar;
	u16 Pageindex = 0;
	char temp[60];
	u8 i;
	u32 fileSize;
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();

		ShowInfoTitle("Log File");//日志文件
		sprintf(&temp[0], "Log File QTY:%d",LogRecorder.Number);	//日志文件数量:
		ShowInfoHelp(&temp[0], NULL);		
		break;
	case WM_INIT_DIALOG://初始化界面
		if(LogRecorder.Select >= LogRecorder.Number)
		{
			LogRecorder.Select = 0;			
		}

		Pageindex = LogRecorder.Select/PageSize;//当前所在页
		
		//滚动条设置
		SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
//		SCROLLBAR_SetDefaultBkColor(GUI_GREEN,0);//滚动条底色		
		hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
		SCROLLBAR_SetValue(hScrollbar, Pageindex);
		
		SCROLLBAR_SetNumItems(hScrollbar, BlastRecorder.Number);
		SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页4行
		if(LogRecorder.Number <= PageSize)
		{//不到一页也创建滚动条而采用隐藏显示的方式主要是为了方便处理
			WM_HideWindow(hScrollbar);
			WM_MoveWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),5, 0);
		}		
					
		//表内容设置	
		hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
		LISTVIEW_SetRowHeight(hItem, 16+1);	
		LISTVIEW_SetBkColor(hItem, 0, GUI_WHITE);//没有选中的行为为白底
		LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
		LISTVIEW_SetGridVis(hItem, 1);	//带分隔
		

		//表头设置
		hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
		HEADER_SetHeight(hHeader, 16+6);//设置表头高度;
		HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
		HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色

		LISTVIEW_AddColumn(hItem, 8*5+4, "SN", GUI_TA_CENTER);//序号
		LISTVIEW_AddColumn(hItem, 8*21, "Date", GUI_TA_CENTER);//日期

		for(i=0; i<2; i++)
		{
			HEADER_SetTextAlign(hHeader,i,GUI_TA_HCENTER|GUI_TA_VCENTER);
		}
		
		ShowListViewData(hWin, Pageindex);	
		
		for(i=0;i<PageSize;i++)
		{
			LISTVIEW_SetTextAlign(hItem, i, GUI_TA_HCENTER|GUI_TA_VCENTER);	
		}
		
		LISTVIEW_SetSel(hItem, LogRecorder.Select%PageSize);

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
				ShowHelpToolMenuDlg();				
				break;
			case GUI_KEY_RIGHT:				
				if ((GET_WIFI_POWER_STATE() != 0) || (GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP))
				{//WIFI电源没有打开 没有连接上
                    MsgBoxInit();
                    SetMsgBoxCaption("Log Upload");//日志上传
                    SetMsgBoxMessage("Pls Connect To WIFI!");//请连接WIFI网络!
                    SetMsgBoxLeft("Back");//返回

                    ShowMsgBox(); 
					break;
				}
				fileSize = GetLogFileSize(LogRecorder.Select);
				if(fileSize == 0)
				{
                    MsgBoxInit();
                    SetMsgBoxCaption("Log Upload");//日志上传
                    SetMsgBoxMessage("Blank File!");//文件为空
                    SetMsgBoxLeft("Back");	//返回		
					break;
				}
				else if(fileSize > 0x1400000)//根据协议，如果每包传400字节，最多只能传24M,限制20M
				{
                    MsgBoxInit();
                    SetMsgBoxCaption("Log Upload");//日志上传
                    SetMsgBoxMessage("File is too large!");//文件太大
                    SetMsgBoxLeft("Back");//返回

                    ShowMsgBox();                    
					break;
				}
				
				ShowLogUploadDlg();			
				break;
			case GUI_KEY_2://向上翻1行
				Pageindex = LogRecorder.Select/PageSize;//当前所在页
				LogRecorder.Select--;
				if(LogRecorder.Select >= LogRecorder.Number)
				{
					LogRecorder.Select = LogRecorder.Number-1;
				}
				
				if((LogRecorder.Select/PageSize) != Pageindex)
				{//跨页
					Pageindex = LogRecorder.Select/PageSize;//当前所在页
					SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);
					
					ShowListViewData(hWin, Pageindex);
				}
				LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), LogRecorder.Select%PageSize);
				break;
			case GUI_KEY_8://向下翻1行
				Pageindex = LogRecorder.Select/PageSize;//当前所在页
				LogRecorder.Select++;
				if(LogRecorder.Select >= LogRecorder.Number)
				{
					LogRecorder.Select = 0;
				}
				if((LogRecorder.Select/PageSize) != Pageindex)
				{//跨页
					Pageindex = LogRecorder.Select/PageSize;//当前所在页
					SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);
					
					ShowListViewData(hWin, Pageindex);
				}				
				LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), LogRecorder.Select%PageSize);				
				break;
			case GUI_KEY_4://向后翻1页
				if(BlastRecorder.Number > PageSize)
				{
					Pageindex = LogRecorder.Select/PageSize;//当前所在页
					Pageindex--;
					if((PageSize*Pageindex) >= LogRecorder.Number)
					{
						Pageindex = (LogRecorder.Number-1)/PageSize;
					}
					LogRecorder.Select = PageSize*Pageindex;
					ShowListViewData(hWin, Pageindex);
					SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);				
					LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), LogRecorder.Select%PageSize);					
				}
				break;
			case GUI_KEY_6://向前翻1页
				if(BlastRecorder.Number > PageSize)
				{
					Pageindex = LogRecorder.Select/PageSize;//当前所在页
					Pageindex++;
					if(Pageindex > ((LogRecorder.Number-1)/PageSize))
					{
						Pageindex = 0;
					}
					LogRecorder.Select = PageSize*Pageindex;		
					ShowListViewData(hWin, Pageindex);
					SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);				
					LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), LogRecorder.Select%PageSize);						
				}
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


void ShowLogRecorderListViewDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
