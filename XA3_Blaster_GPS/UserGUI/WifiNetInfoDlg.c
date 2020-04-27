#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Connect", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 18, INFO_TITLE_LINE_Y+10, 208, 170, 0, 0 }
};

const static u8 PageSize = 7;
static u16 ListViewSelIndex = 0;

void ClearSameWifiScanResult(AP_SCAN_RESULT* Result)
{
	u8 i,j,k;
	
	if(Result->ScanNumber > 1)
	{
		for(i=0;i<Result->ScanNumber-1;i++)
		{
			for(j=i+1;j<Result->ScanNumber;j++)
			{
				if(strcmp((const char*)Result->ScanSum[i].ssid, (const char*)Result->ScanSum[j].ssid) == 0)
				{
					for(k=j+1;k<Result->ScanNumber;k++)
					{
						Result->ScanSum[k-1] = Result->ScanSum[k];
					}
					Result->ScanNumber--;
					j--;
				}
			}			
		}	
	}
}

static void ShowListViewData(WM_HWIN hWin)
{
	u8 i;
	u16 Number;
    WM_HWIN hItem;
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}

	for(i=0; i < PageSize; i++)
	{
		if(i<MyWifi.ScanResult.ScanNumber)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			sprintf((char*)Op_Data.Temp,"%d", i+1);
			LISTVIEW_SetItemText(hItem, 0,i, (char*)Op_Data.Temp);

            LISTVIEW_SetItemText(hItem, 1,i, (char*)MyWifi.ScanResult.ScanSum[i].ssid);
		}
	}	
}
static void SetListViewSelcet(WM_HWIN hWin)
{
    u16 Number;
    
    Number = LISTVIEW_GetNumRows(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
    if(Number > 0)
    {
        if(ListViewSelIndex >= Number)
        {
            ListViewSelIndex = 0;
        }
        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), ListViewSelIndex%PageSize);
    }
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;	
	char temp[80];
    u8 Number;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("WIFI");//无线局域网
            if(GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP)
            {
                sprintf(&temp[0], "WIFI Connect:%s", MyWifi.Ssid);//WIFI已连接到
                ShowInfoHelp(&temp[0], "Update the list every 5s");//设备每5秒自动刷新列表			
            }
            else
            {
                ShowInfoHelp("WIFI Unconnected", "Update the list every 5s");	//设备每5秒自动刷新列表	
            }		
            break;
        case WM_INIT_DIALOG://初始化界面            
            //表内容设置	
            hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
            LISTVIEW_SetRowHeight(hItem, 16+4);	
            LISTVIEW_SetBkColor(hItem, 0,GUI_WHITE);//没有选中的行为为白底
            LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
            LISTVIEW_SetGridVis(hItem, 1);	//带分隔
            
            //表头设置
            hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
            HEADER_SetHeight(hHeader, 16+10);//设置表头高度;
            HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
            HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色
            
            LISTVIEW_AddColumn(hItem, 8*5, "SN", GUI_TA_CENTER);//序号
            LISTVIEW_AddColumn(hItem, 164, "WIFI Name", GUI_TA_CENTER);//无线网络名称
            
            HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
            HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
            
            MyWifi.ScanResult.ScanNumber = 0;

            ShowListViewData(hWin); 
            SetListViewSelcet(hWin);

            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_5000MS_ID, GUI_TIMER_5MS, 0);
            
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
                    OSTaskDelReq(APP_WIFI_TASK_PRIO);
                    ShowHelpToolMenuDlg();								
                    break;
                case GUI_KEY_RIGHT:
                    OSTaskDelReq(APP_WIFI_TASK_PRIO);
                    ShowWifiPasswordDlg(ListViewSelIndex);
                    break;			
                case GUI_KEY_2://向上翻1行
                    Number = LISTVIEW_GetNumRows(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                    if(Number > 0)
                    {
                        ListViewSelIndex--;
                        if(ListViewSelIndex >= Number)
                        {
                            ListViewSelIndex = Number-1;
                        }
                        SetListViewSelcet(hWin);                
                    }	
                    break;
                case GUI_KEY_8://向下翻1行
                    Number = LISTVIEW_GetNumRows(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                    if(Number > 0)
                    {
                        ListViewSelIndex++;
                        if(ListViewSelIndex >= Number)
                        {
                            ListViewSelIndex = 0;
                        }
                        SetListViewSelcet(hWin);              
                    }
                    break;		
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_2MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    ShowListViewData(hWin);
                    SetListViewSelcet(hWin);
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_5000MS_ID, GUI_TIMER_5000MS, 0);
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	

                    WM_InvalidateWindow(hWin);
                    break;
                case GUI_TIMER_5000MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    OSTaskDel(APP_WIFI_TASK_PRIO);
                    TaskCreate(APP_WIFI_TASK_PRIO, (void*)2);	/*扫描WIFI网络*/									
                    break;
                default:
                    break;
            }		
            break;
        case WM_WIFI_TASK_END:
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);			
            break;
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowWifiNetInfoDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
