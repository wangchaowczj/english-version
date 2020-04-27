#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Detial", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }
};

const static u8 PageSize = 8;
static u16 ListViewSelIndex = 0;


void SetSlaveInfoListViewDlgListViewSelIndex(u16 Index)
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
		if(Index < SlaveInfo.Number)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)&Op_Data.Temp[0], "%c", 'A'+Index);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
            
			if(SlaveInfo.Info[Index].State != SLAVE_TRUE)
			{
				if((SlaveInfo.Info[Index].Flags == 0) && (GetWindowParam() <= 3))
				{//还没有读到数据
					strcpy((char*)&Op_Data.Temp[0],"----");
				}
				else
				{
					sprintf((char*)&Op_Data.Temp[0], "%d", SlaveInfo.Info[Index].RegCount);
				}
                LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
                
				if (((SlaveInfo.Info[Index].Flags == 0)||(SlaveInfo.Info[Index].Flags == 1)) && (GetWindowParam() <= 3))
				{
					strcpy((char*)&Op_Data.Temp[0],"----");
				}
				else
				{
					sprintf((char*)&Op_Data.Temp[0], "%d", SlaveInfo.Info[Index].ErrorCount);
				}
				LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);
                
				if(SlaveInfo.Info[Index].State == SLAVE_VOLATILE)
				{
					if(GetWindowParam() <= 3)
					{
						if((SlaveInfo.Info[Index].Flags == 0) || (SlaveInfo.Info[Index].Flags == 1))
						{
							strcpy((char*)&Op_Data.Temp[0],"Prepare");//准备中
						}
						else if(SlaveInfo.Info[Index].Flags == 2)
						{
							strcpy((char*)&Op_Data.Temp[0],"Test");//测试中
						}
						else if(SlaveInfo.Info[Index].Flags == 3)
						{
							strcpy((char*)&Op_Data.Temp[0],"Read");//回读中
						}
						else
						{
							strcpy((char*)&Op_Data.Temp[0],"Test");//测试中
						}						
					}
					else
					{
						strcpy((char*)&Op_Data.Temp[0],"Charge");//充电中
					}					
				}
				else
				{
					strcpy((char*)&Op_Data.Temp[0],"×");
				}
                LISTVIEW_SetItemText(hItem, 3, i, (char*)&Op_Data.Temp[0]);
			}
			else
			{
				sprintf((char*)&Op_Data.Temp[0], "%d", SlaveInfo.Info[Index].RegCount);
                LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
                
				sprintf((char*)&Op_Data.Temp[0], "%d", SlaveInfo.Info[Index].ErrorCount);
                LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);
                
				if(SlaveInfo.Info[Index].ErrorBit != 0x00)
				{
					strcpy((char*)&Op_Data.Temp[0],"×");
				}
				else if(SlaveInfo.Info[Index].WarnBit != 0x00)
				{
					strcpy((char*)&Op_Data.Temp[0],"！");
				}
				else
				{
					strcpy((char*)&Op_Data.Temp[0],"√");
				}
                LISTVIEW_SetItemText(hItem, 3, i, (char*)&Op_Data.Temp[0]);
			}
            if ((SlaveInfo.Info[Index].State == SLAVE_FALSE) ||
                (SlaveInfo.Info[Index].ErrorBit != 0x00))
            {//异常显示红色
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
            {//没选中时显黑色，选中后显白色
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
void SlaveEnterMainMenuDlg(void)
{
	CLOSE_BLUE_POWER();
	OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
	TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)8);
	SlaveInfo.RegCount = 0;
	SlaveInfo.ErrorCount = 0;

	ShowMainMenuDlg(0);
}
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数,bit7 为1时表示恢复上次界面内容，bit3-bit0含义如下
  *         @arg @b 0 显示组网测试结果(允许查找错误雷管内码信息,返回主界面)  
  *         @arg @b 1 显示起爆雷管测试结果(允许查找错误雷管内码信息,返回报表界面)  
  *         @arg @b 2 动态显示组网测试状态(主机控制从机进行快速测试)  
  *         @arg @b 3 动态显示起爆流程测试状态 
  *         @arg @b 4 动态显示起爆流程网路充电状态
  *         @arg @b 5 网路充电结果(允许查找错误雷管内码信息,返回报表界面)
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
    u32 Param;
	u8 i;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("Decoder Info");//译码器信息
            sprintf(&temp[0], "Loading QTY:%d", SlaveInfo.Number);//带载数量:
            ShowInfoHelp(&temp[0], NULL);
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);            
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    Param = GetWindowParam();
                    if(Param == 0)
                    {
                        OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                        ShowMainMenuDlg(0);					
                    }
                    else if(Param == 1)
                    {
                        ShowBlastMasterProcessDlg(1);
                    }
                    else if(Param == 2)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Decoder Info");//译码器信息
                        SetMsgBoxMessage("Exit?");//确定退出?
                        SetMsgBoxLeft("Cancel");
                        SetMsgBoxRight("OK");
                        
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            SlaveEnterMainMenuDlg();
                        }
                    }
                    else if(Param == 5)
                    {
                        ShowBlastMasterProcessDlg(2);
                    }                    
                    else
                    {
                        if(ShowBlastCannelConfirm() == GUI_KEY_RIGHT)
                        {
                            ShowBlastCannelDlg();
                        }			
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if (SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Decoder Info");//译码器信息
                        SetMsgBoxMessage("This decoder can't connect!");//该译码器无法连接!
                        SetMsgBoxLeft("Back");

                        ShowMsgBox();
                        break;
                    }
                    else if ((SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                        (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                    {                        
                        MsgBoxInit();
                        
                        memset(Op_Data.Temp, 0, sizeof(Op_Data.Temp));
                        //异常
                        if((SlaveInfo.Info[ListViewSelIndex].ErrorBit & SLAVE_ERROR_BIT_DET_TYPE) != 0)
                        {
                            strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Chip TypeCode ERR!\n\n");//雷管类型错误!
                        }
                        if((SlaveInfo.Info[ListViewSelIndex].ErrorBit & SLAVE_ERROR_BIT_BUS_VOLTAGE) != 0)
                        {
                            strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Bus Abnormal!\n\n");//总线异常!
                        }
                        
                        //警告
                        if((SlaveInfo.Info[ListViewSelIndex].WarnBit & SLAVE_ERROR_BIT_BAT_LOW) != 0)
                        {
                            strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Low Battery!\n\n");//电池电量低!
                        }					
                        if((SlaveInfo.Info[ListViewSelIndex].WarnBit & SLAVE_WARN_BIT_DET_ERROR) != 0)
                        {
                            strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Have ERR DETs!\n\n");//有错误雷管!
                            if((GetWindowParam() == 0) || (GetWindowParam() == 1))
                            {//可查看错误雷管列表
                                if(ERROR != ReadSlaveDetInfo(ListViewSelIndex))
                                {
                                    Op_Data.DislaySlaveIndex = ListViewSelIndex;
                                    
                                    SetMsgBoxRight("Detail");
                                }
                            }
                        }
                        if((SlaveInfo.Info[ListViewSelIndex].WarnBit & SLAVE_WARN_BIT_BUS_LEAK) != 0)
                        {
                            sprintf((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Bus leakage is large(%uuA)!", SlaveInfo.Info[ListViewSelIndex].I_Leak);//总线漏电大
                        }
                        SetMsgBoxCaption("Decoder Info");//译码器信息
                        SetMsgBoxMessage((char*)Op_Data.Temp);
                        SetMsgBoxLeft("Back");

                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            SetNetTestErrorListViewDlgListViewSelIndex(0);
                            if(GetWindowParam() == 5)
                            {
                                ShowNetTestErrorListViewDlg(5);
                            }
                            else if(GetWindowParam() == 0)
                            {//组网测试
                                ShowNetTestErrorListViewDlg(7);
                            }
                            else if(GetWindowParam() == 1)
                            {//起爆流程
                                ShowNetTestErrorListViewDlg(4);
                            }
                        }
                    }			
                    break;				
                case GUI_KEY_2://向上翻1行
                    ListViewSelIndex--;
                    if(ListViewSelIndex >= SlaveInfo.Number)
                    {
                        ListViewSelIndex = SlaveInfo.Number-1;
                    }
                    if((ListViewSelIndex/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                    if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                        (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                        (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                    {
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {//该表没有异常或者警告
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }				
                    break;
                case GUI_KEY_8://向下翻1行
                    ListViewSelIndex++;
                    if(ListViewSelIndex >= SlaveInfo.Number)
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
                    if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                        (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                        (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                    {
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {//该表没有异常或者警告
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }				
                    break;
                case GUI_KEY_4://向后翻1页
                    if(SlaveInfo.Number > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex)>= SlaveInfo.Number)
                        {
                            ListViewPageindex = (SlaveInfo.Number-1)/PageSize;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        
                        if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                            (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                            (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                        {
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }
                        else
                        {//该表没有异常或者警告
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }					
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(SlaveInfo.Number > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((SlaveInfo.Number-1)/PageSize))
                        {
                            ListViewPageindex = 0;
                        }
                        ListViewSelIndex = PageSize*ListViewPageindex;		
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        
                        if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                            (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                            (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                        {
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }
                        else
                        {//该表没有异常或者警告
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
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
                    
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                    
                    Param = GetWindowParam();
                    switch(Param)
                    {
                        case 2://主机控制从机进行快速测试,并动态显示从机测试状态
                            SlaveInitState();
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)4);
                            break;
                        case 3://主机控制从机测试雷管,并动态显示从机起爆流程雷管测试状态
                            SlaveInitState();
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)5);  
                            break;
                        case 4://主机控制从机网络充电,并动态显示从机状态
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)6);
                            break;
                    }
                    if((Param >= 2) && (Param <= 4))
                    {
                        OSTimeDly(100);
                    }                    
//                    if ((DlgParam &0x80) == 0x80)
//                    {//取消退出
//                        DlgParam &= 0x7F;
//                        if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
//                        {
//                            if ((DlgParam >= 2) && (DlgParam <= 4))
//                            {
//                                ShowBlastMasterProcessDlg(DlgParam-2);
//                            }			
//                        }
//                    }
                    if(ListViewSelIndex >= SlaveInfo.Number)
                    {
                        ListViewSelIndex = 0;			
                    }

                    ListViewPageindex = ListViewSelIndex/PageSize;//当前所在页
                    
                    //滚动条设置
                    SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
            //		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
                    hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+4, 10, 179, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
                    SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
                    
                    SCROLLBAR_SetNumItems(hScrollbar, SlaveInfo.Number);
                    SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页9行
                    if(SlaveInfo.Number <= PageSize)
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
                    
                    LISTVIEW_AddColumn(hItem, 8*5, "No.", GUI_TA_CENTER);//表号
                    LISTVIEW_AddColumn(hItem, 8*6+4, "Log\nDET", GUI_TA_CENTER);//注册\n雷管
                    LISTVIEW_AddColumn(hItem, 8*7, "ERR\nDET", GUI_TA_CENTER);//错误\n雷管
                    LISTVIEW_AddColumn(hItem, 8*8, "Status", GUI_TA_CENTER);//状态
                    
                    HEADER_SetTextAlign(hHeader,0,GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,1,GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,2,GUI_TA_HCENTER|GUI_TA_VCENTER);
                    HEADER_SetTextAlign(hHeader,3,GUI_TA_HCENTER|GUI_TA_VCENTER);
                    
                    ShowListViewData(hWin, ListViewPageindex);
                    
                    LISTVIEW_SetSel(hItem, ListViewSelIndex%PageSize);	
                    if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                        (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                        (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                    {
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {//该表没有异常或者警告
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    ClearTimerHandleBuffer();
                    if((Param >= 2) && (Param <= 4))
                    {
                        SaveTimerHandleToBuffer(WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0));
                        SaveTimerHandleToBuffer(WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_2000MS, 0));//初始2秒                     
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_50MS_ID, GUI_TIMER_50MS, 0);
                    break;
                case GUI_TIMER_50MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_50MS);
                    if (Op_Data.BlastVerifyResult == '0')
                    {//终止起爆
                        BlastTerminationFromAppDlg();
                        break;
                    }                    
                    break;
                case GUI_TIMER_100MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
                    Param = GetWindowParam();
                    if((Param >= 2) && (Param <= 4))
                    {
                        if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                        {
                            break;
                        }
                        if(MsgBoxIsShow())
                        {
                            break;
                        }
                        RemoveTimerHandleAndDeleteById(GUI_TIMER_100MS_ID);
                        RemoveTimerHandleAndDeleteById(GUI_TIMER_500MS_ID);
                        if ((Param == 2) || (Param == 3))
                        {//组网测试和起爆流程
                            if(DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)
                            {
                                ShowSlaveAccreditErrorForDLDlg((Param-2)<<8);
                                return;
                            }
                        }
                        //所有从机雷管已经测试完成
                        if((Param == 3)&&(SlaveInfo.ErrorCount > 0))
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Blast Progress");//起爆流程
                            SetMsgBoxMessage("Slave tests completed!\n\n Have Abnormal DETs!");//从机测试结束,有异常雷管!
                            SetMsgBoxRight("OK");

                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                ShowBlastMasterProcessDlg(Param-2);
                            }         
                        }
                        else
                        {
                            ShowBlastMasterProcessDlg(Param-2);            
                        }                     
                    }  
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);

                    if(MsgBoxIsShow())
                    {
                        break;
                    }                                        
                    if((SlaveInfo.Info[ListViewSelIndex].Flags <= 1) && (GetWindowParam() <= 3))
                    {//还没有开始测试
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {
                        ShowListViewData(hWin, ListViewPageindex);
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
                        
                        if ((SlaveInfo.Info[ListViewSelIndex].State == SLAVE_FALSE) ||
                            (SlaveInfo.Info[ListViewSelIndex].ErrorBit != 0) ||
                            (SlaveInfo.Info[ListViewSelIndex].WarnBit != 0))
                        {
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }
                        else
                        {//该表没有异常或者警告
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }					
                    }
                    for(i=0; i<SlaveInfo.Number; i++)
                    {
                        if (SlaveInfo.Info[i].State == SLAVE_FALSE)
                        {                    
                            RemoveTimerHandleAndDeleteById(GUI_TIMER_100MS_ID);
                            RemoveTimerHandleAndDeleteById(GUI_TIMER_500MS_ID);
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);                           
                            
                            MsgBoxInit();
                        
                            SetMsgBoxCaption("Decoder Info");//译码器信息
                            sprintf((char*)Op_Data.Temp, "Decoder %c unconnected!", i+'A');//译码器%c无法连接!
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                SlaveEnterMainMenuDlg();
                            }
                            return;					
                        }
                        else if (SlaveInfo.Info[i].ErrorBit != 0)
                        {
                            RemoveTimerHandleAndDeleteById(GUI_TIMER_100MS_ID);
                            RemoveTimerHandleAndDeleteById(GUI_TIMER_500MS_ID);
                            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);                           
                            
                            MsgBoxInit();

                            Op_Data.Temp[0] = 0;
                            sprintf((char*)Op_Data.Temp, "Decoder %c:\n\n", i+'A');//译码器
                            if((SlaveInfo.Info[ListViewSelIndex].ErrorBit & SLAVE_ERROR_BIT_DET_TYPE) != 0)
                            {
                                strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Chip TypeCode ERR!\n\n");//雷管类型错误!
                            }
                            if((SlaveInfo.Info[ListViewSelIndex].ErrorBit & SLAVE_ERROR_BIT_BUS_VOLTAGE) != 0)
                            {
                                strcpy((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)], "Bus Abnormal!\n\n");//总线异常!
                            }
                            
                            SetMsgBoxCaption("Decoder Info");//译码器信息
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                SlaveEnterMainMenuDlg();
                            }
                            return;						
                        }
                    }				
                    WM_InvalidateWindow(hWin);//动态刷新从机状态
                    break;
                default:
                    break;
            }		
            break;	
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowSlaveInfoListViewDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
