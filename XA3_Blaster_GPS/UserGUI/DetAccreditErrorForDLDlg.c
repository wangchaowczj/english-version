#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 4, INFO_TITLE_LINE_Y+4, 222, 179, 0, 0 }
};

const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;

static u16 ListBuffer[9];

static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u16 Count, DetIndex;
    u8 Buffer[3];
    
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
    Count = GetAccreditErrorCount(GetWindowParam()&0xFF);
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
            
            ReadAccreditErrorFile(GetWindowParam()&0xFF, Index, 1, Buffer);
            
            DetIndex = (Buffer[0]<<8) + Buffer[1];
            ListBuffer[i] = DetIndex;
            
            sprintf((char*)&Op_Data.Temp[0], "%d-%d", Det.Info[DetIndex].Hole.Name, Det.Info[DetIndex].Hole.Inside);//孔号	            
            LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
            
			GetAccreditErrorString2ForDL((char*)&Op_Data.Temp[0], Buffer[2]);
			LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);         
		}		
	}
}

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 低8位表示从机索引，独立起爆模式时为0
  *         @arg @b  高8位表示主机的状态，0表示组网测试，1表示起爆流程  
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
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
            if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0)))
            {
                memset(temp, 0, sizeof(temp));
                strcpy(temp, "DET ID:");//管码:
                U16Value = ListBuffer[ListViewSelIndex%PageSize];
                if(ERROR == XA3_DetInside2Outside(Det.Info[U16Value].Serl.U32Value, 
                    Det.Info[U16Value].Pwd.U32Value, (u8*)&temp[strlen(temp)]))
                {
                    strcpy(temp, "DET ID:unknown"); //管码:未知
                }
                memset(Op_Data.Temp, 0, sizeof(Op_Data.Temp));
                strcpy((char*)Op_Data.Temp, "UID:");//UID码:
                
                DetCode2UIDStr(&Det.Info[U16Value].Serl, &Det.Info[U16Value].Pwd, (u8*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)]);
                ShowInfoHelp(temp, (char*)Op_Data.Temp);
            }
            else
            {
                ShowInfoHelp(" ", NULL);
            }

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
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        ShowSlaveAccreditErrorForDLDlg((GetWindowParam()&0xFF00)+1);
                    }
                    else
                    {
                        switch(ShowBlastCannelConfirm())
                        {
                            case GUI_KEY_RIGHT:
                                ShowBlastCannelDlg();
                                break;
                        } 
                    }			
                    break;				
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    U16Value = GetAccreditErrorCount(GetWindowParam()&0xFF);
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
                    if(ListViewSelIndex >= GetAccreditErrorCount(GetWindowParam()&0xFF))
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
                    U16Value = GetAccreditErrorCount(GetWindowParam()&0xFF);
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
                    U16Value = GetAccreditErrorCount(GetWindowParam()&0xFF);
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
                    
                    if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
                    {
                        ShowLoadingDlg("Blast Progress", "Validating the authorization...");//起爆流程", "授权验证中...
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
                    if(GetAccreditErrorCount(GetWindowParam()&0xFF) <= PageSize)
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
                    LISTVIEW_AddColumn(hItem, 8*7, "Hole", GUI_TA_CENTER);//孔号
                    LISTVIEW_AddColumn(hItem, 8*15+10, "AUZ ST", GUI_TA_CENTER);//授权状态

                    HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                        
                    ShowListViewData(hWin, ListViewPageindex);
                    
                    LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize); 
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }
                    
                    WM_InvalidateWindow(hWin);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    break;                    
                case GUI_TIMER_3MS_ID:                                    
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    CloseLoadingDlg();
                    
                    MsgBoxInit();
                    SetMsgBoxCaption("Blast Progress");//起爆流程
                    sprintf((char*)Op_Data.Temp, "Have %d unauthorized DET(s)!", GetAllAccreditErrorCount());//有未授权雷管%d发!
                    SetMsgBoxMessage((char*)Op_Data.Temp);
                    SetMsgBoxMessageColor(GUI_RED);
                    sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                    SetMsgBoxHelp1(temp);
                    SetMsgBoxRight("OK");//确定
                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                    {
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_1MS, 0);
                    }
                    break;
                case GUI_TIMER_4MS_ID:                                    
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    UTF8StringToGBK("UID Authorization Approved", (char*)Op_Data.Temp);//UID码授权通过
                    WriteSystemLog((char*)Op_Data.Temp, 0, CURRENT_STATUS);
                    ShowWaitLinCurrentReadyDlg(2);
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
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

void ShowDetAccreditErrorForDLDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
