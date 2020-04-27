#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回	
	{ BUTTON_CreateIndirect, "Detail", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//详情
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};

const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;


static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	WM_HWIN hItem;	
	u16 i, Index, Number;
	char GBK[50];
	u8* FilePath;
	u8 FileName[17];
	
	FilePath = (u8*)GetBlasterPath();
	memcpy(FileName, &BlastRecorder.FileName[BlastRecorder.Select][0],12);
	strcpy((char*)&FileName[12],".txt");
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < Op_Data.BlastRecorderSlaveNumber)
		{
			LISTVIEW_AddRow(hItem, NULL);
			
			sprintf((char*)&Op_Data.Temp[0], "%c", 'A'+Index);
			LISTVIEW_SetItemText(hItem, 0,i, (char*)Op_Data.Temp);
			
			UTF8StringToGBK("注册雷管数", (char*)&GBK[1]);
			GBK[0] = 'A'+Index;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Op_Data.Temp[0],5, FilePath, FileName))
			{
				Op_Data.Temp[0] = '0';
				Op_Data.Temp[1] = 0x00;
			}
			LISTVIEW_SetItemText(hItem, 1,i, (char*)Op_Data.Temp);
			
			UTF8StringToGBK("错误雷管数", (char*)&GBK[1]);
			GBK[0] = 'A'+Index;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Op_Data.Temp[0],5, FilePath, FileName))
			{
				Op_Data.Temp[0] = '0';
				Op_Data.Temp[1] = 0x00;
			}
			LISTVIEW_SetItemText(hItem, 2,i, (char*)Op_Data.Temp);
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
	char temp[80];
	u32 U32Value = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Decoder Info");//译码器信息
			sprintf(&temp[0], "Decoder:%d", Op_Data.BlastRecorderSlaveNumber);//译码器:
			ShowInfoHelp(&temp[0], NULL);
			break;
		case WM_INIT_DIALOG://初始化界面
			if(ListViewSelIndex >= Op_Data.BlastRecorderSlaveNumber)
			{
				ListViewSelIndex = 0;			
			}

			Pageindex = ListViewSelIndex/PageSize;//当前所在页
			
			//滚动条设置
			SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
	//		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
			hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
			SCROLLBAR_SetValue(hScrollbar, Pageindex);
			
			SCROLLBAR_SetNumItems(hScrollbar, Op_Data.BlastRecorderSlaveNumber);
			SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行
			if(Op_Data.BlastRecorderSlaveNumber <= PageSize)
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
			
			LISTVIEW_AddColumn(hItem, 8*5, "No.", GUI_TA_CENTER);//表号
			LISTVIEW_AddColumn(hItem, 8*11, "Log DET", GUI_TA_CENTER);//注册雷管
			LISTVIEW_AddColumn(hItem, 8*10+4, "ERR DET", GUI_TA_CENTER);//错误雷管
			
			HEADER_SetTextAlign(hHeader,0,GUI_TA_HCENTER|GUI_TA_VCENTER);
			HEADER_SetTextAlign(hHeader,1,GUI_TA_HCENTER|GUI_TA_VCENTER);
			HEADER_SetTextAlign(hHeader,2,GUI_TA_HCENTER|GUI_TA_VCENTER);
			
			ShowListViewData(hWin, Pageindex);
		
			
			LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
			
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
					ShowBlastRecorderListViewDlg();
					break;
				case GUI_KEY_RIGHT:
					Op_Data.BlastRecorderDeviceName = 'A'+ListViewSelIndex;
					LISTVIEW_GetItemText(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),1,ListViewSelIndex, (char*)Op_Data.Temp, 10);
					sscanf((char*)Op_Data.Temp, "%d", (int*)&U32Value);
					Op_Data.BlastRecorderDeviceRegNumber = U32Value;
					if(Op_Data.BlastRecorderDeviceRegNumber != 0)
					{
						ShowBlastRecorderDetailedDlg();				
					}
					break;				
				case GUI_KEY_2://向上翻1行
					Pageindex = ListViewSelIndex/PageSize;//当前所在页
					ListViewSelIndex--;
					if(ListViewSelIndex >= Op_Data.BlastRecorderSlaveNumber)
					{
						ListViewSelIndex = Op_Data.BlastRecorderSlaveNumber-1;
					}
					if((ListViewSelIndex/PageSize) != Pageindex)
					{//跨页
						Pageindex = ListViewSelIndex/PageSize;//当前所在页
						SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);
						
						ShowListViewData(hWin, Pageindex);
					}
					LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
					break;
				case GUI_KEY_8://向下翻1行
					Pageindex = ListViewSelIndex/PageSize;//当前所在页
					ListViewSelIndex++;
					if(ListViewSelIndex >= Op_Data.BlastRecorderSlaveNumber)
					{
						ListViewSelIndex = 0;
					}
					if((ListViewSelIndex/PageSize) != Pageindex)
					{//跨页
						Pageindex = ListViewSelIndex/PageSize;//当前所在页
						SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);
						
						ShowListViewData(hWin, Pageindex);
					}				
					LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);		
					break;
				case GUI_KEY_4://向后翻1页
					if(Op_Data.BlastRecorderSlaveNumber > PageSize)
					{
						Pageindex = ListViewSelIndex/PageSize;//当前所在页
						Pageindex--;
						if((PageSize*Pageindex)>= Op_Data.BlastRecorderSlaveNumber)
						{
							Pageindex = (Op_Data.BlastRecorderSlaveNumber-1)/PageSize;
						}
						ListViewSelIndex = PageSize*Pageindex;
						
						ShowListViewData(hWin, Pageindex);
						
						SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);				
						LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);		
					}
					break;
				case GUI_KEY_6://向前翻1页
					if(Op_Data.BlastRecorderSlaveNumber > PageSize)
					{
						Pageindex = ListViewSelIndex/PageSize;//当前所在页
						Pageindex++;
						if(Pageindex > ((Op_Data.BlastRecorderSlaveNumber-1)/PageSize))
						{
							Pageindex = 0;
						}
						ListViewSelIndex = PageSize*Pageindex;
						
						ShowListViewData(hWin, Pageindex);
						
						SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);				
						LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);					
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

void ShowBlastRecorderSlaveInfoListDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
