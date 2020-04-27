#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Modify", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//修改
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 4, INFO_TITLE_LINE_Y+4, 222, 179, 0, 0 }
};

const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;

void SetNetInfoDlgListViewSelIndex(u16 Index)
{
	ListViewSelIndex = Index;
}


static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 i;
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
		if(Index < Det.RegCount)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Index+1);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
            
            sprintf((char*)&Op_Data.Temp[0], "%d-%d", Det.Info[Index].Hole.Name, Det.Info[Index].Hole.Inside);//孔号	            
            LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Det.Info[Index].Delay);
			LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);
            
            memset(&Op_Data.Temp[0], 0, sizeof(Op_Data.Temp));
            if(XA3_DetInside2Outside(Det.Info[Index].Serl.U32Value, Det.Info[Index].Pwd.U32Value, &Op_Data.Temp[0]) == ERROR)
            {
                sprintf((char*)&Op_Data.Temp[0], "%02X%02X%02X%02X", 
                    Det.Info[Index].Serl.U8Buffer[0], Det.Info[Index].Serl.U8Buffer[1],
                    Det.Info[Index].Serl.U8Buffer[2], Det.Info[Index].Serl.U8Buffer[3]);
                LISTVIEW_SetItemText(hItem, 3, i, (char*)&Op_Data.Temp[0]);            
            }
            else
            {
                LISTVIEW_SetItemText(hItem, 3, i, (char*)&Op_Data.Temp[7]);
            }


            if(0x00 != Det.Info[Index].Err)
            {//错误雷管显示红色
                LISTVIEW_SetItemTextColor(hItem,0,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,1,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,2,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,3,i,0,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,0,i,1,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,1,i,1,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,2,i,1,GUI_RED);
                LISTVIEW_SetItemTextColor(hItem,3,i,1,GUI_RED);			
            }
            else
            {//非错误雷管，没选中时显黑色，选中后显白色
                LISTVIEW_SetItemTextColor(hItem,0,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,1,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,2,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,3,i,0,GUI_BLACK);
                LISTVIEW_SetItemTextColor(hItem,0,i,1,GUI_WHITE);
                LISTVIEW_SetItemTextColor(hItem,1,i,1,GUI_WHITE);
                LISTVIEW_SetItemTextColor(hItem,2,i,1,GUI_WHITE);
                LISTVIEW_SetItemTextColor(hItem,3,i,1,GUI_WHITE);			
            }            
		}		
	}	
}
static void ScanBar(WM_HWIN hWin)
{
    TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)11);/*扫码*/
}
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 主界面下按右功能键显示网路信息
  *         @arg @b 1 延期修改显示网路列表  
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
            
            memset(temp, 0, sizeof(temp));
            strcpy(temp, "DET ID:");//管码:
            if(ERROR == XA3_DetInside2Outside(Det.Info[ListViewSelIndex].Serl.U32Value, 
                Det.Info[ListViewSelIndex].Pwd.U32Value, (u8*)&temp[strlen(temp)]))
            {
                strcpy(temp, "DET ID:unknown");//管码:未知
            }
            
            switch(GetWindowParam())
            {
                case 0:
                    ShowInfoTitle("Logged DETs Info");//网路信息
                    ShowInfoHelp(&temp[0], " ↙ More Items");	//↙键更多选项
                    break;
                case 1:
                    ShowInfoTitle("Delaytime Settings");//设置延期
                    ShowInfoHelp(&temp[0], " ↙ Set single DET delaytime");//↙键设置单发雷管延期
                    break;                   
            }
            break;
        case WM_INIT_DIALOG://初始化界面
            if(ListViewSelIndex >= Det.RegCount)
            {
                ListViewSelIndex = 0;			
            }
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                    CLOSE_SCAN();
                    ShowMainMenuDlg(0);				
                    break;
                case GUI_KEY_RIGHT:				
                    if(0 == GetWindowParam())
                    {//详情状态下右键为删除单发雷管
                        MsgBoxInit();
                        
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("Del the selected DET?");//确定删除指定的雷管?
                        SetMsgBoxHelp1("Pls do this with caution");//请谨慎执行此操作
                        SetMsgBoxHelp2("Advise to backup the Dat");//建议提前备份数据
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确定

                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            if(DeleteDetInfo(&Det.Info[ListViewSelIndex].Hole, &Det.Info[ListViewSelIndex].Hole) == ERROR)
                            {
                                ShowSDCardError();
                            }
                            else
                            {
                                ShowNetInfoDlg(GetWindowParam());
                            }
                        }						
                    }
                    else if(1 == GetWindowParam())
                    {//延期列表状态下为批量修改延期
                        ShowDelayModifyBatchDlg();	//批量修改延期	
                    }                   
                    break;
                case GUI_KEY_ENTER:
                    if(0 == GetWindowParam())
                    {
                        ShowNetInfoModifyMenuDlg();
                    }
                    else if(1 == GetWindowParam())
                    {
                        ShowDelayModifySingleDlg(1, ListViewSelIndex);					
                    }
                    break;				
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= Det.RegCount)
                    {
                        ListViewSelIndex = Det.RegCount-1;
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
                    if(ListViewSelIndex >= Det.RegCount)
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
                    if(Det.RegCount > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex)>= Det.RegCount)
                        {
                            ListViewPageindex = (Det.RegCount-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        WM_InvalidateWindow(hWin);    
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(Det.RegCount > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((Det.RegCount-1)/PageSize))
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
                case GUI_KEY_5:
                    if((device_info.HasScan) && (GetWindowParam() <=1))
                    {
                        if(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
                        {
                            OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                        }
                        else
                        {
                            ScanBar(hWin);                             
                        }						
                    }
                    break;                    
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if((0 == GetWindowParam()) && (device_info.NetInfoSort == 1))
                    {
                        AllRegDetRankBaseOnDelay();//把已经注册的雷管按延期排序
                    }
                    else
                    {
                       AllRegDetRankBaseOnHoleInfo();//把已经注册的雷管重新排序 
                    }
                    
                    if(ListViewSelIndex >= Det.RegCount)
                    {
                        ListViewSelIndex = 0;			
                    }

                    ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                    
                    //滚动条设置
                    SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
            //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
                    hScrollbar = SCROLLBAR_Create(226, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
                    
                    SCROLLBAR_SetNumItems(hScrollbar, Det.RegCount);
                    SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行
                    if(Det.RegCount <= PageSize)
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
                    LISTVIEW_AddColumn(hItem, 8*7, "Delay", GUI_TA_CENTER);//延期
                    LISTVIEW_AddColumn(hItem, 8*8+10, "DET", GUI_TA_CENTER);//雷管

                    HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,3, GUI_TA_HCENTER|GUI_TA_VCENTER);
                    
                    ShowListViewData(hWin, ListViewPageindex);
                    
                    LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize);
                    
                    if(0 == GetWindowParam())
                    {//详情状态下右键功能为删除单发雷管
                        BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2), "Del");//删除
                    }
                    else if(1 == GetWindowParam())
                    {//延期修改显示网路列表 
                        BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2), "Modify");//修改
                    }
                    if(device_info.HasScan && Op_Data.ScanIsZEBRA && (GetWindowParam() <=1))
                    {
                        OPEN_SCAN();
                    }      
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }
                    
                    WM_InvalidateWindow(hWin);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    break;
                case GUI_TIMER_2MS_ID:                                    
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(WindowHasChild())
                    {
                        break;
                    }
                    
                    MsgBoxInit();
                    if(0 == ScanData.Bytes)
                    {
                        SetMsgBoxCaption("Logging DETs");//雷管注册
                        SetMsgBoxMessage("Not getting the Dat!");//没有扫描到数据!
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("Scan");//扫码

                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ScanBar(hWin);
                        }
                        break;
                    }
                    
                    BarCodeDataAnalyst();			
                    if(BARCODE_NEW == BarCode.Status)
                    {
                        if(BarCode.Result == BARCODE_PASS)
                        {
                            if(ERROR == CheckOverlap(BarCode.DetSerl.U8Buffer))
                            {
                                SoundOutput(0);
                                ListViewSelIndex = Op_Data.overlaped;
                                if(ListViewSelIndex >= DET_MAX_SIZE)
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
                            }	
                            else
                            {
                                SetMsgBoxMessage("The DET is unlogged!");//雷管未注册!
                            }
                        }
                        else if(BarCode.Result == BARCODE_DET_TYPE_ERR)
                        {
                            SetMsgBoxMessage("Chip TypeCode ERR!");//雷管类型错误!
                        }
                        else if(BarCode.Result == BARCODE_FACTORY_ERR)
                        {
                            SetMsgBoxMessage("Chip Factory Code ERR!");//工厂代码错误!
                        }                    
                        else
                        {
                            SetMsgBoxMessage("No meet Encoding ruler!");//雷管不符合要求!
                        }					
                    }
                    else
                    {                        
                        if(ScanData.Bytes > 0)
                        {
                            if(ScanData.Bytes >= SCAN_DATA_MAX_LEN)
                            {                            
                                ScanData.Bytes = SCAN_DATA_MAX_LEN -1;
                            }
                            ScanData.Buffer[ScanData.Bytes] = 0x00;
                            
                            sprintf((char*)Op_Data.Temp, "Bar Code ERR!\n\n%s", (char*)&ScanData.Buffer[0]);//条码错误!
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                        }
                        else
                        {
                            SetMsgBoxMessage("Bar Code ERR!");//条码错误!
                        }                        
                    }
                    if(MsgBoxData.Message[0] != NULL)
                    {
                        SoundOutput(1);	
                        SetMsgBoxCaption("DET Info");//雷管信息
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("Scan");//扫码
                        
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ScanBar(hWin);   
                        }
                    }
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
			if(pMsg->Data.v != 2)
            {
                if(!(device_info.HasScan && Op_Data.ScanIsZEBRA))
                {
                    CLOSE_SCAN();
                }
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
            }
			break;            
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowNetInfoDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
