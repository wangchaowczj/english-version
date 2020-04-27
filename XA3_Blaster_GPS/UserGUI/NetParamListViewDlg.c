#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
	{ BUTTON_CreateIndirect, "Modify", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//修改
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};
const static u8 PageSize = 8;
static u16 ListViewSelIndex = 0;
void SetNetParamListViewDlgSelIndex(u16 Index)
{
	ListViewSelIndex = Index;
}
static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u8 i;
	u16 Number,Index;
	WM_HWIN hItem;
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < NET_PARAM_MAX)
		{
			LISTVIEW_AddRow(hItem, NULL);
			
			sprintf((char*)Op_Data.Temp,"%d", NetParam[Index].Count);//孔内雷管数
			LISTVIEW_SetItemText(hItem, 0,i, (char*)Op_Data.Temp);
            
			sprintf((char*)Op_Data.Temp,"%d", NetParam[Index].SpacesDly);//孔间延期
			LISTVIEW_SetItemText(hItem, 1,i, (char*)Op_Data.Temp);
            
			sprintf((char*)Op_Data.Temp,"%d", NetParam[Index].InsideSpacesDly);//孔内延期
			LISTVIEW_SetItemText(hItem, 2,i, (char*)Op_Data.Temp);	
            
            if(Index == device_info.RegNetParamDefault)
            {//显示红色
                LISTVIEW_SetItemTextColor(hItem,0,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,1,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,2,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,0,i,1,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,1,i,1,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,2,i,1,GUI_RED);		
            }
            else
            {//没选中时显黑色，选中后显白色
                LISTVIEW_SetItemTextColor(hItem,0,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,1,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,2,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,0,i,1,GUI_WHITE);
                LISTVIEW_SetItemTextColor(hItem,1,i,1,GUI_WHITE);
                LISTVIEW_SetItemTextColor(hItem,2,i,1,GUI_WHITE);			
            }
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
	char temp[80];
    u16 Pageindex = 0;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("Net PARM");	//孔网参数
            sprintf(&temp[0], "Default DET QTY:%d",device_info.RegNetParamDefault+1);//默认孔内雷管:
            ShowInfoHelp(&temp[0], " ↙ Set Default PARM");//↙键设置默认参数
            break;
        case WM_INIT_DIALOG://初始化界面
            AllRegDetRankBaseOnHoleInfo();//把已经注册的雷管重新排序
            if(ListViewSelIndex >= NET_PARAM_MAX)
            {
                ListViewSelIndex = 0;
            }		 		
            Pageindex = ListViewSelIndex/PageSize;

            //滚动条设置
            SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
    //		SCROLLBAR_SetDefaultBkColor(GUI_GREEN,0);//滚动条底色		
            hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
            SCROLLBAR_SetValue(hScrollbar, Pageindex);
            
            SCROLLBAR_SetNumItems(hScrollbar, NET_PARAM_MAX);
            SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页4行
            if(NET_PARAM_MAX <= PageSize)
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
            HEADER_SetHeight(hHeader, 16*2+6);//设置表头高度;
            HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
            HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色
                    
            LISTVIEW_AddColumn(hItem, 8*6, "DET\nQTY", GUI_TA_CENTER);//孔内\n雷管
            LISTVIEW_AddColumn(hItem, 8*10, "Outer\nDLY", GUI_TA_CENTER);//孔间\n延期
            LISTVIEW_AddColumn(hItem, 8*10, "DLY\nIn-hole", GUI_TA_CENTER);//孔内\n延期
            
            HEADER_SetTextAlign(hHeader, 0, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader, 1, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader, 2, GUI_TA_HCENTER|GUI_TA_VCENTER);
            
            ShowListViewData(hWin, Pageindex);
            
            LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    ShowSystemSetMenuDlg();				
                    break;
                case GUI_KEY_RIGHT:
                    ShowNetParamModifyDlg(ListViewSelIndex%NET_PARAM_MAX);                          
                    break;
                case GUI_KEY_ENTER:
                    if(ListViewSelIndex < NET_PARAM_MAX)
                    {
                        device_info.RegNetParamDefault = ListViewSelIndex;
                        SaveUserConfigParam();
                        Pageindex = ListViewSelIndex/PageSize;//当前所在页
                        ShowListViewData(hWin, Pageindex);
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        WM_Paint(hWin);
                    }					
                    break;	                    
                case GUI_KEY_2://向上翻1行
                    Pageindex = ListViewSelIndex/PageSize;//当前所在页
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= NET_PARAM_MAX)
                    {
                        ListViewSelIndex = NET_PARAM_MAX-1;
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
                    if(ListViewSelIndex >= NET_PARAM_MAX)
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
                    if(NET_PARAM_MAX > PageSize)
                    {
                        Pageindex = ListViewSelIndex/PageSize;//当前所在页
                        Pageindex--;
                        if((PageSize*Pageindex) >= NET_PARAM_MAX)
                        {
                            Pageindex = (NET_PARAM_MAX-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*Pageindex;
                        ShowListViewData(hWin, Pageindex);
                        SCROLLBAR_SetValue(hScrollbar, Pageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);					
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(NET_PARAM_MAX > PageSize)
                    {
                        Pageindex = ListViewSelIndex/PageSize;//当前所在页
                        Pageindex++;
                        if(Pageindex > ((NET_PARAM_MAX-1)/PageSize))
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

void ShowNetParamListViewDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


