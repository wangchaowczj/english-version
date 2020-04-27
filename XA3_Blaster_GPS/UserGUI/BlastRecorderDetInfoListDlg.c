#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回	
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};
const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;



static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 Number,Index;
	WM_HWIN hItem;
	u16 i;
	u8 KeyBuffer[30];
	u8 FileName[17];
	char GBK[50];
	u8 j, Len, Count;
	u8* FilePath;
	
	if(Op_Data.CurrentSlaveIndex != Op_Data.DislaySlaveIndex)
	{//如果内存中的雷管数据不是指定表号的数据则需要重新读取
		ReadSlaveDetInfo(Op_Data.DislaySlaveIndex);
	}
	
	memcpy(FileName, &BlastRecorder.FileName[BlastRecorder.Select][0],12);
	strcpy((char*)&FileName[12],".txt");	
	
	FilePath = (u8*)GetBlasterPath();
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}

	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < Op_Data.BlastRecorderDeviceRegNumber)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)Op_Data.Temp, "%d", Index+1);//序号	
            LISTVIEW_SetItemText(hItem, 0,i, (char*)Op_Data.Temp);
            
			sprintf((char*)KeyBuffer,"%cDET Info%d", Op_Data.BlastRecorderDeviceName, Index+1);//雷管信息
			memset(&Op_Data.Temp[0], 0, sizeof(Op_Data.Temp));
			UTF8StringToGBK((char*)KeyBuffer, (char*)GBK);
			if(ERROR == GetConfigFileString((u8*)GBK, &Op_Data.Temp[0], sizeof(Op_Data.Temp), FilePath,FileName))
			{
				continue;
			}
			Len = strlen((const char*)&Op_Data.Temp[0]);
			Count = 0;
			for(j=0; j<Len; j++)
			{
				if(Op_Data.Temp[j] == '\t')
				{
					Count++;
					Op_Data.Temp[j] = 0x00;
				}
			}
			if(Count < 5)
			{
				continue;
			}
			Count = 0;
			for(j=0; j<Len; j++)
			{
				if(Op_Data.Temp[j] == 0x00)
				{
					Count++;
					if(Count == 1)//孔号
					{
                        LISTVIEW_SetItemText(hItem, 1,i, (char*)&Op_Data.Temp[j+1]);
					}
					else if(Count == 4)//延期
					{
                        LISTVIEW_SetItemText(hItem, 2,i, (char*)&Op_Data.Temp[j+1]);
					}
					else if(Count == 5)//状态码
					{
                        int DetStatus;
                        
						sscanf((char*)&Op_Data.Temp[j+1], "%02X",(int*)&DetStatus);
                        if(DetStatus == 0)
                        {
                            LISTVIEW_SetItemText(hItem, 3,i, "Normal");//正常
                        }
                        else if(DET_TEST_CONNECT_ERR == (DetStatus&0x0F))
                        {
                            LISTVIEW_SetItemText(hItem, 3,i, "Unconnected");//未连接
                        }
                        else if(DET_TEST_COMM_ERR == (DetStatus&0x0F))
                        {
                            LISTVIEW_SetItemText(hItem, 3,i, "COMM Abnormal");//通信异常
                        }                        
                        else
                        {
                            LISTVIEW_SetItemText(hItem, 3,i, "DET ERR");//雷管错误
                        }
						break;
					}				
				}
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
	static u16 ListViewPageindex = 0;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("DET List");//雷管列表

            sprintf(&temp[0], "Used DET QTY:%d", Op_Data.BlastRecorderDeviceRegNumber);//起爆雷管数:
            ShowInfoHelp(&temp[0], NULL);
            break;
        case WM_INIT_DIALOG://初始化界面
            ListViewSelIndex = 0;		

            ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
            
            hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
            hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
            
            //滚动条设置
            SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
    //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
            hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
            SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
            
            SCROLLBAR_SetNumItems(hScrollbar, Op_Data.BlastRecorderDeviceRegNumber);
            SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行			
            if(Op_Data.BlastRecorderDeviceRegNumber <= PageSize)
            {//不到一页也创建滚动条而采用隐藏显示的方式主要是为了方便处理
                WM_HideWindow(hScrollbar);
                WM_MoveWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),5, 0);
            }
            
            
            HEADER_SetHeight(hHeader, 16+6);//设置表头高度;
            HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
            HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色	

            LISTVIEW_SetRowHeight(hItem, 16+1);	
            LISTVIEW_SetBkColor(hItem, 0, GUI_WHITE);//没有选中的行为为白底
            LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
            LISTVIEW_SetGridVis(hItem, 1);	//带分隔
            
            LISTVIEW_AddColumn(hItem, 8*5, "SN", GUI_TA_CENTER);//序号
            LISTVIEW_AddColumn(hItem, 8*6, "Hole", GUI_TA_CENTER);//孔号
            LISTVIEW_AddColumn(hItem, 8*5+4, "DLY", GUI_TA_CENTER);//延期
            LISTVIEW_AddColumn(hItem, 8*10, "Status", GUI_TA_CENTER);//状态

            HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,3, GUI_TA_HCENTER|GUI_TA_VCENTER);
            
            ShowListViewData(hWin, ListViewPageindex);
                    
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
                    if(Op_Data.BlastRecorderDeviceUsages == DEVICE_USAGES_MASTER)
                    {//显示译码器列表
                        ShowBlastRecorderSlaveInfoListDlg();
                    }
                    else
                    {
                        ShowBlastRecorderListViewDlg();
                    }
                    break;			
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= Op_Data.BlastRecorderDeviceRegNumber)
                    {
                        ListViewSelIndex = Op_Data.BlastRecorderDeviceRegNumber-1;
                    }
                    if((ListViewSelIndex/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);	
                    break;
                case GUI_KEY_8://向下翻1行
                    ListViewSelIndex++;
                    if(ListViewSelIndex >= Op_Data.BlastRecorderDeviceRegNumber)
                    {
                        ListViewSelIndex = 0;
                    }
                    if((ListViewSelIndex/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }				
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                    break;
                case GUI_KEY_4://向后翻1页
                    if(Op_Data.BlastRecorderDeviceRegNumber > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex)>= Det.ErrorCount)
                        {
                            ListViewPageindex = (Op_Data.BlastRecorderDeviceRegNumber-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);					
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(Op_Data.BlastRecorderDeviceRegNumber > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((Op_Data.BlastRecorderDeviceRegNumber-1)/PageSize))
                        {
                            ListViewPageindex = 0;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;		
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
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

void ShowBlastRecorderDetInfoListDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
