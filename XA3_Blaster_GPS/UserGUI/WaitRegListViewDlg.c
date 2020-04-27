#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "PARM", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//参数
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 4, INFO_TITLE_LINE_Y+4, 222, 179, 0, 0 }
};

const static u8 PageSize = 9;
static u16 ListViewSelIndex = 0;
static u16 ListViewPageindex = 0;
static SCROLLBAR_Handle hScrollbar;

void SetWaitRegListViewDlgSelIndex(u16 Index)
{
	ListViewSelIndex = Index;
}
u16 GetWaitRegListViewDlgSelIndex(void)
{
	return ListViewSelIndex;;
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
		if(Index < DET_MAX_SIZE)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Index+1);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
            
            sprintf((char*)&Op_Data.Temp[0], "%d-%d", Det.Info[Index].Hole.Name, Det.Info[Index].Hole.Inside);//孔号	            
            LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);

            if(GetDetRegStatus(Index) != ERROR)
            {
                memset(&Op_Data.Temp[0], 0, sizeof(Op_Data.Temp));
                if(XA3_DetInside2Outside(Det.Info[Index].Serl.U32Value, Det.Info[Index].Pwd.U32Value, &Op_Data.Temp[0]) == ERROR)
                {
                    sprintf((char*)&Op_Data.Temp[0], "%02X%02X%02X%02X", 
                        Det.Info[Index].Serl.U8Buffer[0], Det.Info[Index].Serl.U8Buffer[1], 
                        Det.Info[Index].Serl.U8Buffer[2], Det.Info[Index].Serl.U8Buffer[3]); 
                    LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);                  
                }
                else
                {
                    LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[7]);//显示管码的后6位
                }
            }
            
			sprintf((char*)&Op_Data.Temp[0], "%d", Det.Info[Index].Delay);
			LISTVIEW_SetItemText(hItem, 3, i, (char*)&Op_Data.Temp[0]);
          
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
static void SaveConfim(WM_HWIN hWin, DET_INFO *RegDetInfo)
{
    u8 Outside[14];
    
    if ((DEVICE_USAGES_ALONE == device_info.DeviceUsages) &&
        (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType))
    {
        u8 ErrorCode;
        
        if(!DetIsAccreditPassForDL(&RegDetInfo->Serl, &RegDetInfo->Pwd, &ErrorCode))
        {
            SoundOutputError();
            
            MsgBoxInit();
            
            SetMsgBoxCaption("Logging DETs");//雷管注册
            GetAccreditErrorString1ForDL((char*)&Op_Data.Temp[0], ErrorCode);
            SetMsgBoxMessage((char*)Op_Data.Temp);
            SetMsgBoxMessageColor(GUI_RED);
            SetMsgBoxLeft("Cancel");
            ShowMsgBox();
            return;
        }        
    }
    if(GetDetRegStatus(ListViewSelIndex) != ERROR)
    {
        MsgBoxInit();
        
        SetMsgBoxCaption("Logging DETs");//雷管注册
        sprintf((char*)&Op_Data.Temp[0], "Replace Hole %d-%d?", //确认替换孔
            Det.Info[ListViewSelIndex].Hole.Name, Det.Info[ListViewSelIndex].Hole.Inside);
        SetMsgBoxMessage((char*)Op_Data.Temp);
        SetMsgBoxMessageColor(GUI_RED);
        SetMsgBoxLeft("Cancel");
        SetMsgBoxRight("OK");
        if(ShowMsgBox() != GUI_KEY_RIGHT)
        {
            return;
        }
    }
    else if(ListViewSelIndex > 0)
    {
        if(GetDetRegStatus(ListViewSelIndex-1) == ERROR)
        {
            MsgBoxInit();
            
            SetMsgBoxCaption("Logging DETs");////雷管注册
            sprintf((char*)&Op_Data.Temp[0], "Log Hole To %d-%d?",//确认跳孔注册到 
                Det.Info[ListViewSelIndex].Hole.Name, Det.Info[ListViewSelIndex].Hole.Inside);
            SetMsgBoxMessage((char*)Op_Data.Temp);
            SetMsgBoxMessageColor(GUI_RED);
            SetMsgBoxLeft("Cancel");
            SetMsgBoxRight("OK");
            if(ShowMsgBox() != GUI_KEY_RIGHT)
            {
                return;
            }           
        }
    }
    
    SoundOutputRight();
    
    Det.Info[ListViewSelIndex].Serl.U32Value = RegDetInfo->Serl.U32Value;
    Det.Info[ListViewSelIndex].Pwd.U32Value = RegDetInfo->Pwd.U32Value;
    Det.Info[ListViewSelIndex].Freq.U32Value = RegDetInfo->Freq.U32Value;
    Det.Info[ListViewSelIndex].Err = 0;
    GetDetNum();//重新统计雷管数据
    
    memset((char*)Op_Data.Temp, 0, sizeof((char*)Op_Data.Temp));
    
    sprintf((char*)Op_Data.Temp, 
        "Reg: %d-%d %02X%02X%02X%02X%02X%02X%02X%02X",
        Det.Info[ListViewSelIndex].Hole.Name, Det.Info[ListViewSelIndex].Hole.Inside,
        Det.Info[ListViewSelIndex].Serl.U8Buffer[0], Det.Info[ListViewSelIndex].Serl.U8Buffer[1],
        Det.Info[ListViewSelIndex].Serl.U8Buffer[2], Det.Info[ListViewSelIndex].Serl.U8Buffer[3],
        Det.Info[ListViewSelIndex].Pwd.U8Buffer[0], Det.Info[ListViewSelIndex].Pwd.U8Buffer[1],
        Det.Info[ListViewSelIndex].Pwd.U8Buffer[2], Det.Info[ListViewSelIndex].Pwd.U8Buffer[3]);
    
    
    memset((char*)Outside, 0, sizeof(Outside));
    if(XA3_DetInside2Outside(Det.Info[ListViewSelIndex].Serl.U32Value, Det.Info[ListViewSelIndex].Pwd.U32Value,Outside) != ERROR)
    {
        sprintf((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], " %s", Outside);
    }
    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);
    
    if(ERROR == SaveAllDetInfoNotChangeRank(0))//保存雷管信息
    {
        ShowSDCardError();
    }
    else
    {                                                    
        ListViewSelIndex++;
        if(ListViewSelIndex >= DET_MAX_SIZE)
        {
            ListViewSelIndex = DET_MAX_SIZE - 1;
        }
        ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
        
        ShowListViewData(hWin, ListViewPageindex);
        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
        WM_InvalidateWindow(hWin);
        
        if(Det.RegCount >= DET_MAX_SIZE)
        {
            MsgBoxInit();
            
            SetMsgBoxCaption("Logging DETs");//雷管注册
            SetMsgBoxMessage("Logged DETs reach MAX!");//注册雷管数已达最大值!
            sprintf((char*)&Op_Data.Temp[0], "Log DET:%d",Det.RegCount);//注册雷管:
            SetMsgBoxHelp1((char*)Op_Data.Temp);
            SetMsgBoxLeft("Back");
            SetMsgBoxRight("CONT");
            switch(ShowMsgBox())
            {
                case GUI_KEY_LEFT:
                    ShowMainMenuDlg(0);
                    break;
                case GUI_KEY_RIGHT:
                    break;                
            }
        }
    }
}
static void ScanBar(WM_HWIN hWin)
{
    TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)11);/*扫码*/
}
static u8 AllUnRegFollow(u16 Count)
{
    u16 Index;
                        
    MsgBoxInit();

    if((Count==0) || ((ListViewSelIndex+Count) > DET_MAX_SIZE))
    {
        SetMsgBoxCaption("Logging DETs");//雷管注册
        SetMsgBoxMessage("QTY Exceeded!");//数量超出范围!
        SetMsgBoxLeft("Back");
        ShowMsgBox();
        return 0;
    }
    Index = GetRegDetIndexFirst(ListViewSelIndex);
    if(Index < (ListViewSelIndex+Count))
    {
        sprintf((char*)Op_Data.Temp, "Hole %d-%d has been logged!", Det.Info[Index].Hole.Name, Det.Info[Index].Hole.Inside);//孔号%d-%d已注册
        SetMsgBoxCaption("Logging DETs");
        SetMsgBoxMessage((char*)Op_Data.Temp);
        SetMsgBoxLeft("Back");
        ShowMsgBox();
        
        return 0;        
    }
    return 1;
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;		
	char temp[200];
    BOX_BIG_INFO BoxBigInfo;
    BOX_SMALL_INFO BoxSmallInfo;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Logging DETs");
            sprintf(&temp[0], "Tout:%d Tin:%d Log:%d",//孔间:%d 孔内:%d 注册:%d 
                NetParam[Det.HoleNumber[Det.Info[ListViewSelIndex].Hole.Name-1]-1].SpacesDly, 
                NetParam[Det.HoleNumber[Det.Info[ListViewSelIndex].Hole.Name-1]-1].InsideSpacesDly,
                Det.RegCount);
            if(device_info.HasScan)
            {                
                ShowInfoHelp(&temp[0], "5 Scan ↙ Tool");//5键扫码 ↙键工具
            }
            else
            {                
                ShowInfoHelp(&temp[0], "↙ Tool");//↙键工具                
            }
            break;
        case WM_INIT_DIALOG://初始化界面
            Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_L_GOAL;
            
            if(ListViewSelIndex >= DET_MAX_SIZE)
            {
                ListViewSelIndex = 0;			
            }
            
            ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
            
            //滚动条设置
            SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
    //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
            hScrollbar = SCROLLBAR_Create(226, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
            SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
            
            SCROLLBAR_SetNumItems(hScrollbar, DET_MAX_SIZE);
            SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行	
                        
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
            LISTVIEW_AddColumn(hItem, 8*8+10, "DET", GUI_TA_CENTER);//雷管
            LISTVIEW_AddColumn(hItem, 8*7, "DLY", GUI_TA_CENTER);//延期

            HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,2, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,3, GUI_TA_HCENTER|GUI_TA_VCENTER);
            
            ShowListViewData(hWin, ListViewPageindex);
            
            LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize);
            
            if(device_info.HasScan && Op_Data.ScanIsZEBRA)
            {
                OPEN_SCAN();
            }
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

            UTF8StringToGBK("进入注册列表界面", (char*)Op_Data.Temp);
            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);	            
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                    CLOSE_SCAN();
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                    ShowMainMenuDlg(0);       
                    break;
                case GUI_KEY_RIGHT:	             
                    ShowRegParamMenuDlg(0);                    
                    break;
                case GUI_KEY_ENTER:
                    ShowRegParamMenuDlg(1);
                    break;				
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= DET_MAX_SIZE)
                    {
                        ListViewSelIndex = DET_MAX_SIZE-1;
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
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_KEY_4://向后翻1页
                    ListViewPageindex--;
                    if((PageSize*ListViewPageindex)>= DET_MAX_SIZE)
                    {
                        ListViewPageindex = (DET_MAX_SIZE-1)/PageSize;
                    }
                    ListViewSelIndex = PageSize*ListViewPageindex;
                    ShowListViewData(hWin, ListViewPageindex);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_KEY_6://向前翻1页
                    ListViewPageindex++;
                    if(ListViewPageindex > ((DET_MAX_SIZE-1)/PageSize))
                    {
                        ListViewPageindex = 0;
                    }
                    ListViewSelIndex = PageSize*ListViewPageindex;		
                    ShowListViewData(hWin, ListViewPageindex);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_KEY_5:
                    if(device_info.HasScan)
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
                case GUI_TIMER_2MS_ID:                              
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(WindowHasChild())
                    {
                        break;
                    }
                    
                    MsgBoxInit();
                    if(0 == ScanData.Bytes)
                    {
                        SoundOutputError();
                        
                        SetMsgBoxCaption("Logging DETs");
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
                                u8 Outside[14];
                                char Buffer[8];
                                
                                UTF8StringToGBK("扫码注册重号错误:", (char*)Op_Data.Temp);
                                WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);

                                memcpy(&Buffer[0], &BarCode.DetSerl.U8Buffer[0], 4);
                                memcpy(&Buffer[4], &BarCode.DetPwd.U8Buffer[0], 4);
                                WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);

                                memcpy(&Buffer[0], &Det.Info[Op_Data.overlaped].Serl.U8Buffer[0], 4);
                                memcpy(&Buffer[4], &Det.Info[Op_Data.overlaped].Pwd.U8Buffer[0], 4);
                                WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
        
                                memset(Outside, 0, sizeof(Outside));
                                if(XA3_DetInside2Outside(Det.Info[Op_Data.overlaped].Serl.U32Value, Det.Info[Op_Data.overlaped].Pwd.U32Value, Outside) == ERROR)
                                {
                                    sprintf((char*)temp, "DET has been logged!\n\nHole No.:%d-%d", //雷管已注册!\n\n孔号:%d-%d
                                        Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);                                
                                }
                                else
                                {
                                    sprintf((char*)temp, "DET has been logged!\n\nHole No.:%d-%d\n\nDET ID: %s", //雷管已注册!\n\n孔号:%d-%d\n\n管码:%s
                                        Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside, Outside);  
                                }

                                SetMsgBoxMessage((char*)temp);
                                SetMsgBoxRight("LOC");//定位
                            }	
                            else
                            {
                                DET_INFO RegDetInfo;
                                
                                RegDetInfo.Serl.U32Value = BarCode.DetSerl.U32Value;
                                RegDetInfo.Pwd.U32Value = BarCode.DetPwd.U32Value;

                                RegDetInfo.Freq.U32Value = 0;
                                
                                SaveConfim(hWin, &RegDetInfo);
                                MsgBoxInit();
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
                    else if(BarCodeAnalyst17ForBoxBig(&BoxBigInfo) != ERROR)
                    {//箱条码
                        if(AllUnRegFollow(BoxBigInfo.InBoxBig*BoxBigInfo.InBoxSmall))
                        {
                            ShowRegDetFeatureInputForBarcodeDlg();
                        }
                        return;                        
                    }
                    else if(BarCodeAnalyst18ForBoxSmall(&BoxSmallInfo) != ERROR)
                    {//盒条码
                        if(AllUnRegFollow(BoxSmallInfo.InBoxSmall))
                        {
                            ShowRegDetFeatureInputForBarcodeDlg();
                        }
                        return;
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
                            sprintf((char*)temp, "Bar Code ERR!\n\n%s", (char*)&ScanData.Buffer[0]);//条码错误!
                            SetMsgBoxMessage((char*)temp);
                        }
                        else
                        {
                            SetMsgBoxMessage("Bar Code ERR!");//条码错误!
                        }                        
                    }
                    if(MsgBoxData.Message[0] != NULL)
                    {
                        SoundOutputError();	
                        SetMsgBoxCaption("Logging DETs");
                        SetMsgBoxLeft("Back");
                        if(MsgBoxData.Right[0] == NULL)
                        {
                            SetMsgBoxRight("Scan");//扫码
                        }
                        
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            if(strcmp((char*)&MsgBoxData.Right[0], "Scan") == 0)//扫码
                            {
                                ScanBar(hWin);
                            }
                            else if(strcmp((char*)&MsgBoxData.Right[0], "Location") == 0)//定位
                            {
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
                                WM_InvalidateWindow(hWin);
                            }    
                        }
                    }
                    break;
                case GUI_TIMER_10MS_ID:
                    if(MsgBoxIsShow())
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    CloseLoadingDlg();
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    MsgBoxInit();
                    switch((CHECK_RESULT)AppTaskResult.DetOperate)//雷管检测结果
                    {
                        case DET_PASS:			  //检测到雷管                           
                            SaveConfim(hWin, &Op_Data.RegDetInfo);
                            MsgBoxInit();
                            break;		
                        case TEST_ERR:			  //BIT错误 
                        case FREQ_MIN:			  //频率过小
                        case FREQ_MAX:			  //频率过大
                            SetMsgBoxMessage("DET ERR!");//雷管错误!
                            break;				
                        case OVERLAPED:			  //上线过程中这个序列号已经注册
                            {
                                u8 Outside[14];
                                
                                memset(Outside, 0, sizeof(Outside));
                                if(XA3_DetInside2Outside(Det.Info[Op_Data.overlaped].Serl.U32Value, Det.Info[Op_Data.overlaped].Pwd.U32Value, Outside) == ERROR)
                                {
                                    sprintf((char*)Op_Data.Temp, "DET has been logged!\n\nHole No.:%d-%d", //雷管已注册!\n\n孔号:%d-%d
                                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);
                                }
                                else
                                {
                                    sprintf((char*)Op_Data.Temp, "DET has been logged!\n\nHole No.:%d-%d\n\nDET ID: %s", //雷管已注册!\n\n孔号:%d-%d\n\n管码:
                                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside, Outside);
                                }
                            }

                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            SetMsgBoxRight("LOC");//定位
                            break;
                        case TYPE_ERR:			  //雷管类型错误
                            strcpy((char*)Op_Data.Temp, "Chip TypeCode ERR!\n\nDET Type:");//雷管类型错误!\n\n当前雷管类型:
                            GetDetTypeName((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)] ,GetDetType(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value));
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            break;
                        case CURRENT_MIN:		  //电流小
                            SetMsgBoxMessage("DET Current Small!");//雷管电流小!
                            break;
                        case CURRENT_MAX:		  //电流大
                            SetMsgBoxMessage("DET Current Large!");//雷管电流大!
                            break;
                        case SECU_CODE_ERR:		  //密码校验错误
                            SetMsgBoxMessage("ChipPWD ERR!\n\nPls Rewrite ChipPWD!");//密码错误!\n\n请重写密码!
                            SetMsgBoxRight("Rewrite");
                            break;
                        case SERL_NULL_ERR:       //内码还没有写入                    
                        case DET_CODE_ERR:		  //内码不符合规则
                            SetMsgBoxMessage("Chip ID ERR!\n\nPls Rewrite Chip ID!");//雷管内码错误 请重写内码!
                            SetMsgBoxRight("Rewrite");
                            break;
                        case DET_FAC_CODE_ERR:
                            SetMsgBoxMessage("Chip Factory Code ERR!");//工厂代码错误
                            break;                            
                        default:
                            break;
                    }
                    if(MsgBoxData.Message[0] != NULL)
                    {
                        SoundOutputError();
                        SetMsgBoxCaption("Logging DETs");//雷管注册
                        SetMsgBoxLeft("Back");
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_RIGHT:
                                if(strcmp(MsgBoxData.Right, "Re-write") == 0)//重写
                                {
                                    ShowDetCodeWriteMenuDlg(0);
                                }
                                else if(strcmp(MsgBoxData.Right, "Location") == 0)//定位
                                {
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
                                    WM_InvalidateWindow(hWin);
                                }
                                break;
                        }
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0); 
                    break;
                case GUI_TIMER_120MS_ID:
                    if(WindowHasChild())
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(XA3_DetCheckIn() == ERROR)
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
                    {
                        OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                        CLOSE_SCAN();
                    }
                    ShowLoadingDlg("Logging DETs", "Recognising...");//雷管注册 识别中
                    WM_DeleteTimer(pMsg->Data.v);
                    //雷管操作任务
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)21);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);   
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

void ShowWaitRegListViewDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
