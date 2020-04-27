#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect,   "Check",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//查看
    { LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 4, INFO_TITLE_LINE_Y+4, 222, 179, 0, 0 }
};

const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;


static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u8 Count, SlaveIndex;
    
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
    Count = GetAccreditErrorFileCount();
    if(Count == 0)
    {
        return;
    }
     
	for(i=0; i < PageSize; i++)
	{
        Index = Pageindex*PageSize+i;
		if(Index < Count)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Index+1);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
            
            SlaveIndex = GetAccreditErrorFileIndex(Index);
            
            sprintf((char*)&Op_Data.Temp[0], "%c", 0x41+SlaveIndex);//表号	            
            LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
            
            sprintf((char*)&Op_Data.Temp[0], "%d", GetAccreditErrorCount(SlaveIndex));
			LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);         
		}		
	}
}

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数高8位
  *         @arg @b 0 高8位，组网测试
  *         @arg @b 1 高8位，起爆流程测试
  *         @arg @b 0 低8位 动态显示授权中
  *         @arg @b 1 低8位 静态显示有授权异常的从机列表   
  * @retval 无
  * @author ZH
  * @date   2019年7月4日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;	
	static SCROLLBAR_Handle hScrollbar;
	static u16 ListViewPageindex = 0;
	char temp[80];
    u16 U16Value;
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Abnormal Info");//异常信息
            sprintf(&temp[0], "Log DET Sum:%d", SlaveInfo.RegCount);//注册雷管:
            ShowInfoHelp(&temp[0], NULL);
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
            ListViewSelIndex = 0;
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    if(WindowHasChild())
                    {
                        return;
                    }
                    MsgBoxInit();
                    SetMsgBoxCaption("Decoder Info");//译码器信息
                    SetMsgBoxMessage("Exit?");//确定退出?
                    SetMsgBoxLeft("Cancel");//取消
                    SetMsgBoxRight("OK");//确定
                     
                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                    {
                        SlaveEnterMainMenuDlg();
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if(WindowHasChild())
                    {
                        return;
                    }
                    U16Value = GetAccreditErrorFileIndex(ListViewSelIndex); 
                    if(ERROR != ReadSlaveDetInfo(U16Value))
                    {
                        Op_Data.DislaySlaveIndex = U16Value;
                        ShowDetAccreditErrorForDLDlg(GetWindowParam()&0xFF00+U16Value);
                    }                    
                    break;
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    U16Value = GetAccreditErrorFileCount();
                    if(ListViewSelIndex >= U16Value)
                    {
                        ListViewSelIndex = U16Value-1;
                    }
                    if((ListViewSelIndex/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_KEY_8://向下翻1行
                    ListViewSelIndex++;
                    if(ListViewSelIndex >= GetAccreditErrorFileCount())
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
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_KEY_4://向后翻1页
                    U16Value = GetAccreditErrorFileCount();
                    if(U16Value > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex) >= U16Value)
                        {
                            ListViewPageindex = (U16Value-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        WM_InvalidateWindow(hWin);    
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    U16Value = GetAccreditErrorFileCount();
                    if(U16Value > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((U16Value-1)/PageSize))
                        {
                            ListViewPageindex = 0;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;		
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        WM_InvalidateWindow(hWin); 
                    }
                    break;                   
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if((GetWindowParam()&0xFF) == 0)
                    {
                        ShowLoadingDlg("Blast Progress", "Validating the AUZ...");//起爆流程", "授权验证中...
                        TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)16);
                    }
                    else
                    {
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_1MS, 0);
                    }
                    break;
                case GUI_TIMER_2MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    ListViewSelIndex = 0;

                    ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                    
                    //滚动条设置
                    SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
            //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
                    hScrollbar = SCROLLBAR_Create(226, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
                    
                    SCROLLBAR_SetNumItems(hScrollbar, Det.RegCount);
                    SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行
                    if(GetAccreditErrorFileCount() <= PageSize)
                    {//不到一页也创建滚动条而采用隐藏显示的方式主要是为了方便处理
                        WM_HideWindow(hScrollbar);
                        WM_MoveWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),5, 0);
                    }		
                                
                    //表内容设置	
                    hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
                    LISTVIEW_SetRowHeight(hItem, 16+1);	
                    LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_UNSEL, GUI_WHITE);//没有选中的行为为白底
                    LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_BLUE);//选中的行为蓝底
                    LISTVIEW_SetGridVis(hItem, 1);	//带分隔	

                    //表头设置
                    hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
                    HEADER_SetHeight(hHeader, 16+6);//设置表头高度;		
                    HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
                    HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色
                    
                    LISTVIEW_AddColumn(hItem, 8*4+4, "SN", GUI_TA_CENTER);//序号
                    LISTVIEW_AddColumn(hItem, 8*7, "No.", GUI_TA_CENTER);//表号
                    LISTVIEW_AddColumn(hItem, 8*15+10, "Abnormal QTY", GUI_TA_CENTER);//授权异常数量

                    HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                        
                    ShowListViewData(hWin, ListViewPageindex);
                    
                    LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize); 
                    
                    WM_InvalidateWindow(hWin);
                    break;                    
                case GUI_TIMER_3MS_ID:                                    
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    CloseLoadingDlg();
                    
                    MsgBoxInit();
                    SetMsgBoxCaption("Blast Progress");//起爆流程
								    sprintf((char*)Op_Data.Temp, "Unauthorized DET QTY:%d!", GetAllAccreditErrorCount());//有未授权雷管%d发
                    SetMsgBoxMessage((char*)Op_Data.Temp);
                    SetMsgBoxMessageColor(GUI_RED);
                    sprintf(temp, "Log DET:%d", SlaveInfo.RegCount);//注册雷管数:
                    SetMsgBoxHelp1(temp);
                    SetMsgBoxRight("OK");
                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                    {
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_1MS, 0);
                    }
                    break;
                case GUI_TIMER_4MS_ID:                                    
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    WriteSystemLog("UID码授权通过", strlen("UID码授权通过"), CURRENT_STATUS);
                    
                    if(((GetWindowParam() >> 8) == 1)&&(SlaveInfo.ErrorCount > 0))
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Blast Progress");//起爆流程
                        SetMsgBoxMessage("Slave device test completed\n\nHave Abnormal DETs!");//从机测试结束,有异常雷管!
                        SetMsgBoxRight("OK");

                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ShowBlastMasterProcessDlg(1);
                        }         
                    }
                    else
                    {
                        ShowBlastMasterProcessDlg(0);            
                    } 
                    break;
                default:
                    break;
            }		
            break;
		case WM_BACKGROUND_TASK_END: 
            CloseLoadingDlg();
            if(pMsg->Data.v > 0)
            { 
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3MS_ID, GUI_TIMER_1MS, 0); 
            }
            else
            {
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_4MS_ID, GUI_TIMER_1MS, 0);
            }
			break;            
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowSlaveAccreditErrorForDLDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
