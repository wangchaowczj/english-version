#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect,   "Del",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};
const static u8 PageSize = 9;

static u16 ListViewSelIndex = 0;

void SetNetTestErrorListViewDlgListViewSelIndex(u16 Index)
{
	ListViewSelIndex = Index;
}

static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u16 ErrorDetIndex;
	u8 ErrType;

	if(Op_Data.CurrentSlaveIndex != Op_Data.DislaySlaveIndex)
	{//如果内存中的雷管数据不是指定表号的数据则需要重新读取
		ReadSlaveDetInfo(Op_Data.DislaySlaveIndex);
	}
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
    
    for(i=0; i < PageSize; i++)
    {
        Index = Pageindex*PageSize+i;
        if(Index < Det.ErrorCount)
        {
            LISTVIEW_AddRow(hItem, NULL);
            
            ErrorDetIndex = GetErrorDetIndex(Index);
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Index+1);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
            
            sprintf((char*)&Op_Data.Temp[0], "%d-%d", Det.Info[ErrorDetIndex].Hole.Name, Det.Info[ErrorDetIndex].Hole.Inside);//孔号
            LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
            
            ErrType = Det.Info[ErrorDetIndex].Err&0x0F;
            if(DET_TEST_CONNECT_ERR == ErrType)
            {
                LISTVIEW_SetItemText(hItem, 2, i, "Unconnected");//未连接
            }
            else if(DET_TEST_COMM_ERR == ErrType)
            {
                LISTVIEW_SetItemText(hItem, 2, i, "COMM Abnormal");  //通信异常              
            }				
            else if(DET_TEST_PWD_ERR == ErrType)
            {
                LISTVIEW_SetItemText(hItem, 2, i, "ChipPWD ERR"); //密码错误               
            }
            else if(DET_TEST_TYPE_ERR == ErrType)
            {
                LISTVIEW_SetItemText(hItem, 2, i, "DET Type ERR"); //类型错误               
            }
            else
            {
                LISTVIEW_SetItemText(hItem, 2, i, "DET ERR");//雷管错误
            }
        }
    }
}
static void BackKey(u16 Param)
{
    if((Param == 0) || (Param == 1))
    {//快速测试和完全测试，返回主界面
        ShowNetTestReportDlg(Param);					
    }
    else if(Param == 2)
    {//起爆测试，返回报表界面
        ShowBlastTestReportDlg();							
    }
    else if (Param == 3)
    {//删除错误雷管，返回删除雷管界面					
        ShowDeleteDetMenuDlg();				
    }
    else if (Param == 4)
    {//主从机模式下查找从机错误雷管列表(起爆流程)
        ShowSlaveInfoListViewDlg(1);
    }
    else if (Param == 5)
    {//主从机模式下充电结束后查找从机错误雷管列表
        ShowSlaveInfoListViewDlg(5);
    }
    else if (Param == 6)
    {//起爆流程前的组网测试
        ShowNetTestReportDlg(2);
    }
    else if (Param == 7)
    {//主从机模式下查找从机错误雷管列表(组网测试)
        ShowSlaveInfoListViewDlg(0);
    }
}
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 快速测试
  *         @arg @b 1 完全测试
  *         @arg @b 2 起爆测试 
  *         @arg @b 3 删除错误雷管
  *         @arg @b 4 从主机模式下查看从机错误雷管列表(起爆流程)
  *         @arg @b 5 主从机模式下充电结束后查找从机错误雷管列表
  *         @arg @b 6 起爆流程前的组网测试
  *         @arg @b 7 主从机模式下查找从机错误雷管列表(组网测试)  
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
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("ERR List");//错误列表
            if(Op_Data.DelaySampleFlag)
            {
                sprintf(&temp[0], "Log DET:%d ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:%
                ShowInfoHelp(&temp[0], "Shut down,and check the DET");		//请关闭设备后检查雷管		
            }
            else
            {
                u16 ErrorDetIndex;
                
                ErrorDetIndex = GetErrorDetIndex(ListViewSelIndex);
                memset(temp, 0, sizeof(temp));
                strcpy(temp, "DET ID:");//管码:
                if(ERROR == XA3_DetInside2Outside(Det.Info[ErrorDetIndex].Serl.U32Value, 
                    Det.Info[ErrorDetIndex].Pwd.U32Value, (u8*)&temp[strlen(temp)]))
                {
                    strcpy(temp, "DET ID:unknown");//管码:未知
                }
                
                if(GetWindowParam() == 0)
                {
                    u8 Err;
                    
                    Err = Det.Info[ErrorDetIndex].Err&0x0F;
                    
                    if ((Err == DET_TEST_PWD_ERR) || (Err == DET_TEST_CONNECT_ERR))
                    {
                        ShowInfoHelp(&temp[0], " ↙ Rewrite Chip ID"); //↙键重写内码                  
                    }
                }
                else if(GetWindowParam() == 6)
                {
                    ShowInfoHelp(&temp[0], " ↙ Del all unconnected DETs");//↙键删除所有未连接雷管
                }
                else
                {
                    sprintf(&temp[40], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                    ShowInfoHelp(&temp[0], &temp[40]);
                }
            }
            break;
        case WM_INIT_DIALOG://初始化界面           
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);	
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
                    {
                        BackKey(GetWindowParam());                       
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
                    {
                        u32 DlgParam;
                        
                        DlgParam = GetWindowParam();
                        if((DlgParam == 0) || (DlgParam == 1) || (DlgParam == 3))
                        {//删除错误雷管
                            MsgBoxInit();
                            SetMsgBoxCaption("Del DET");//雷管删除
                            SetMsgBoxMessage("Del this ERR DET?");//确定删除该发错误雷管?
                            SetMsgBoxHelp1("Pls do this with caution");//请谨慎执行此操作
                            SetMsgBoxHelp2("Advise to backup the Dat");//建议提前备份数据
                            SetMsgBoxLeft("Back");//返回
                            SetMsgBoxRight("OK");//确定
                            
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                sprintf((char*)Op_Data.Temp, "Del ERR Hole:%d-%d", //删除错误孔:
                                    Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole.Name, 
                                    Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole.Inside);

                                UTF8StringToGBK((char*)Op_Data.Temp, temp);
                                WriteSystemLog((char*)temp, 0, CURRENT_STATUS);
                            
                                if(DeleteDetInfo(&Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole, &Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole) == ERROR)
                                {
                                    ShowSDCardError();
                                }
                                else
                                {
                                    if(Det.ErrorCount)
                                    {
                                        ShowNetTestErrorListViewDlg(GetWindowParam());
                                    }
                                    else
                                    {
                                        BackKey(GetWindowParam());
                                    }
                                }
                            }
                        }
                        else if(DlgParam == 6)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Del DET");//雷管删除
                            SetMsgBoxMessage("Del this ERR DET?");//确定删除该发错误雷管?
                            SetMsgBoxHelp1("Pls do this with caution");//请谨慎执行此操作
                            SetMsgBoxLeft("Back");//返回
                            SetMsgBoxRight("OK");//确定
                            
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                sprintf((char*)Op_Data.Temp, "Del ERR Hole:%d-%d", //删除错误孔:
                                    Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole.Name, 
                                    Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole.Inside);

                                UTF8StringToGBK((char*)Op_Data.Temp, temp);
                                WriteSystemLog((char*)temp, 0, CURRENT_STATUS);
                                
                                Op_Data.BlastDeleteDet = 1;
                                DeleteDetInfoNotSave(&Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole, &Det.Info[GetErrorDetIndex(ListViewSelIndex)].Hole);
                                if(Det.ErrorCount)
                                {
                                    ShowNetTestErrorListViewDlg(GetWindowParam());
                                }
                                else
                                {
                                    BackKey(GetWindowParam());
                                }
                            }
                        }
                    }
                    break;			
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= Det.ErrorCount)
                    {
                        ListViewSelIndex = Det.ErrorCount-1;
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
                    if(ListViewSelIndex >= Det.ErrorCount)
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
                    if(Det.ErrorCount > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex)>= Det.ErrorCount)
                        {
                            ListViewPageindex = (Det.ErrorCount-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        WM_InvalidateWindow(hWin);                          
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(Det.ErrorCount > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((Det.ErrorCount-1)/PageSize))
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
                case GUI_KEY_ENTER:
                    if(GetWindowParam() == 0)
                    {//组网测试
                        u16 ErrorDetIndex;
                        u8 Err;
                        
                        ErrorDetIndex = GetErrorDetIndex(ListViewSelIndex);
                        Err = Det.Info[ErrorDetIndex].Err&0x0F;
                        
                        if ((Err == DET_TEST_PWD_ERR) || (Err == DET_TEST_CONNECT_ERR))
                        {
                            if(Op_Data.LinABCurrent > 30)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Rewrite Chip ID");//内码重写
                                SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管!
                                sprintf(temp, "Log DET:%d, ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d, 错误雷管:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回
                                ShowMsgBox();
                                return;
                            }
                            sprintf(temp, "Rewrite:%d-%d", Det.Info[ErrorDetIndex].Hole.Name, Det.Info[ErrorDetIndex].Hole.Inside);//重写:
                            UTF8StringToGBK(temp, (char*)Op_Data.Temp);
                            WriteSystemLog((char*)Op_Data.Temp, 0, CURRENT_STATUS);
                            
                            Op_Data.RegDetInfo.Serl = Det.Info[ErrorDetIndex].Serl;
                            Op_Data.RegDetInfo.Pwd = Det.Info[ErrorDetIndex].Pwd;
                            ShowDetCodeWriteStatusDlg(GetWindowParam(),3);
                        }
                    }
                    else if(GetWindowParam() == 6)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("Del all unconnected DETs?");//确定删除所有未连接雷管?
                        SetMsgBoxHelp1("Pls do this with caution");//请谨慎执行此操作
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确定
                        
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            u16 i;
                            
                            for(i=0; i<DET_MAX_SIZE; i++)
                            {
                                if (GetDetRegStatus(i) != ERROR)
                                {
                                    if ((Det.Info[i].Err&0x0F) == DET_TEST_CONNECT_ERR)
                                    {
                                        Op_Data.BlastDeleteDet = 1;
                                        
                                        Det.Info[i].Serl.U32Value = 0x00;
                                        Det.Info[i].Pwd.U32Value = 0x00;
                                        Det.Info[i].Freq.U32Value = 0x00;
                                        Det.Info[i].Err = 0x00;                
                                    }		
                                }
                            }
                            GetDetNum();
                            AllRegDetRankBaseOnHoleInfo();
                            
                            UTF8StringToGBK("删除所有未连接雷管", (char*)Op_Data.Temp);
                            WriteSystemLog((char*)Op_Data.Temp, 0, CURRENT_STATUS);
                            
                            if(Det.ErrorCount)
                            {
                                ShowNetTestErrorListViewDlg(GetWindowParam());
                            }
                            else
                            {
                                BackKey(GetWindowParam());
                            }
                        }
                    }
                    break;					
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    switch(GetWindowParam())
                    {
                        case 0://快速
                        case 1://完全
                        case 3://删除错误雷管
                        case 6://起爆流程前的组网测试  
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                            break;
                        default:
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                            break;
                    }

                    if(ListViewSelIndex >= Det.ErrorCount)
                    {
                        ListViewSelIndex = 0;		
                    }

                    ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                    
                    hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
                    hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
                    
                    //滚动条设置
                    SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
            //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
                    hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
                    
                    SCROLLBAR_SetNumItems(hScrollbar, Det.ErrorCount);
                    SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行			
                    if(Det.ErrorCount <= PageSize)
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
                    LISTVIEW_AddColumn(hItem, 8*6+4, "Hole", GUI_TA_CENTER);//孔号
                    LISTVIEW_AddColumn(hItem, 8*15, "Status", GUI_TA_CENTER);//状态

                    HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    
                    ShowListViewData(hWin, ListViewPageindex);
                            
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);		

                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }
                    if(Op_Data.DelaySampleFlag)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_TIMER_500MS_ID:
                    if (Op_Data.BlastVerifyResult == '0')
                    {//终止起爆
                        BlastTerminationFromAppDlg();
                        break;
                    }			
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

void ShowNetTestErrorListViewDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
