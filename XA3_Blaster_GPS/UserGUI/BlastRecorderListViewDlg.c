#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ BUTTON_CreateIndirect, "Upload", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//上传
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 9, INFO_TITLE_LINE_Y+7, 212, 173, 0, 0 }
};

const static u8 PageSize = 4;
static u8 UpdateFlag[PageSize];



static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u16 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u8 FileName[17];
	u8 Buffer[50], j, Len;
	u8* FilePath;
	char GBK[50];
	
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
		if(Index < BlastRecorder.Number)
		{
            LISTVIEW_AddRow(hItem, NULL);
            
			memcpy(FileName, &BlastRecorder.FileName[Index][0],12);
			strcpy((char*)&FileName[12],".txt");
			
			sprintf((char*)&Op_Data.Temp[0], "%d", Index+1);
            LISTVIEW_SetItemText(hItem, 0, i, (char*)&Op_Data.Temp[0]);
			
			if(ConfigFileVerify(FilePath, (u8*)FileName, 0) == ERROR)
			{
				UpdateFlag[i] = 1;
                LISTVIEW_SetItemText(hItem, 1, i, "----");
                LISTVIEW_SetItemText(hItem, 2, i, "----");
                LISTVIEW_SetItemText(hItem, 3, i, "----");		
			}
			else
			{
				UTF8StringToGBK("起爆时间", (char*)GBK);
				if(ERROR == GetConfigFileString((u8*)GBK, &Buffer[0],50, FilePath, FileName))
				{
					LISTVIEW_SetItemText(hItem, 1, i, " ");
				}
				else
				{
                    memset((char*)&Op_Data.Temp[0], 0, sizeof(Op_Data.Temp));
					Len = 2;
					for(j=0; j<17;j++)
					{
						if((j == 2) || (j == 5))
						{
							Op_Data.Temp[j] = '-';
						}
						else if(j == 8)
						{
							Op_Data.Temp[j] = '\n';
						}
						else if((j == 11) || (j == 14))
						{
							Op_Data.Temp[j] = ':';
						}
						else
						{
							Op_Data.Temp[j] = Buffer[Len++];					
						}
					}
					Op_Data.Temp[j] = 0x00;
                    LISTVIEW_SetItemText(hItem, 1, i, (char*)&Op_Data.Temp[0]);
				}
				UTF8StringToGBK("注册雷管总数", (char*)GBK);
				if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Op_Data.Temp[0],6, FilePath, FileName))
				{
					LISTVIEW_SetItemText(hItem, 2, i, " ");
				}
                else
                {
                    LISTVIEW_SetItemText(hItem, 2, i, (char*)&Op_Data.Temp[0]);
                }
				if(DEVICE_USAGES_SLAVE==device_info.DeviceUsages)
				{//译码器中保存的起爆记录信息不完整，不能作为一起爆记录上传
                    LISTVIEW_SetItemText(hItem, 3, i, "----");                    
					UpdateFlag[i] = 1;			
				}
				else
				{
					UTF8StringToGBK("信息上传标志", (char*)GBK);
					if(ERROR == GetConfigFileString((u8*)GBK, &Buffer[0],50, FilePath, FileName))
					{
						LISTVIEW_SetItemText(hItem, 3, i, " ");
						UpdateFlag[i] = 1;
					}
					else
					{
						UTF8StringToGBK("未上传", (char*)GBK);
						if(memcmp(Buffer, GBK, 6) == 0)
						{
                            LISTVIEW_SetItemText(hItem, 3, i, "N");//"没有 上传" 改为 N  
							UpdateFlag[i] = 0;
						}
						else
						{
                            LISTVIEW_SetItemText(hItem, 3, i, "Y");//"已经 上传" "Has Been\nUpload"  改为 Y
							UpdateFlag[i] = 1;
						}
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
	char temp[60];
	u8 i;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("Blast Info");//起爆信息
				sprintf(&temp[0], "Blast Info QTY:%d",BlastRecorder.Number);//起爆信息数量:	
            ShowInfoHelp(&temp[0], "Press ↙ ,browse details");//按↙键查看详情			
            GUI_SetFont(&SMALL_FONT);
            break;
        case WM_INIT_DIALOG://初始化界面
            if(BlastRecorder.Select >= BlastRecorder.Number)
            {
                BlastRecorder.Select = 0;			
            }

            ListViewPageindex = BlastRecorder.Select/PageSize;//当前所在页
            
            //滚动条设置
            SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
    //		SCROLLBAR_SetDefaultBkColor(GUI_GREEN,0);//滚动条底色		
            hScrollbar = SCROLLBAR_Create(221, INFO_TITLE_LINE_Y+7, 10, 173, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
            SCROLLBAR_SetValue(hScrollbar, ListViewPageindex);
            
            SCROLLBAR_SetNumItems(hScrollbar, BlastRecorder.Number);
            SCROLLBAR_SetPageSize(hScrollbar, PageSize);//每页4行
            if(BlastRecorder.Number <= PageSize)
            {//不到一页也创建滚动条而采用隐藏显示的方式主要是为了方便处理
                WM_HideWindow(hScrollbar);
                WM_MoveWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),5, 0);
            }		
                        
            //表内容设置	
            hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
            LISTVIEW_SetRowHeight(hItem, 16*2+4);	
            LISTVIEW_SetBkColor(hItem, 0, GUI_WHITE);//没有选中的行为为白底
            LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
            LISTVIEW_SetGridVis(hItem, 1);	//带分隔
            

            //表头设置
            hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
            HEADER_SetHeight(hHeader, 16+8);//设置表头高度;
            HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
            HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色

            LISTVIEW_AddColumn(hItem, 8*5+4, "SN", GUI_TA_CENTER);//序号
            LISTVIEW_AddColumn(hItem, 8*9+4, "DATE", GUI_TA_CENTER);//起爆时间
            LISTVIEW_AddColumn(hItem, 8*5+4, "QTY", GUI_TA_CENTER);//数量
            LISTVIEW_AddColumn(hItem, 8*5+4, "ST", GUI_TA_CENTER);//状态		

            for(i=0; i<4; i++)
            {
                HEADER_SetTextAlign(hHeader,i,GUI_TA_HCENTER|GUI_TA_VCENTER);
            }
            
            ShowListViewData(hWin, ListViewPageindex);	
            
            for(i=0;i<PageSize;i++)
            {
                LISTVIEW_SetTextAlign(hItem, i, GUI_TA_HCENTER|GUI_TA_VCENTER);	
            }
            
            LISTVIEW_SetSel(hItem, BlastRecorder.Select%PageSize);

            if((UpdateFlag[BlastRecorder.Select%PageSize] == 0) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
            {//该条记录没有上传
                WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
            }
            else
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
            }
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
                    ShowInfoManageDlg();				
                    break;
                case GUI_KEY_RIGHT:	
                    if(DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType)
                    {
                        if(UpdateFlag[BlastRecorder.Select%PageSize] == 0) 
                        {//如果当前选择的起爆记录还没有上传，则允许上传	
                            if ((GET_WIFI_POWER_STATE() != 0) || (GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP))
                            {//WIFI电源没有打开 没有连接上
                                MsgBoxInit();
                                SetMsgBoxCaption("Blast Info");//起爆信息
                                SetMsgBoxMessage("Pls Connect WIFI!");//请连接WIFI网络!
                                SetMsgBoxLeft("Back");//返回

                                ShowMsgBox();
                                break;
                            }
                            ShowUploadBlastRecordDlg();
                        }                    
                    }			
                    break;
                case GUI_KEY_ENTER:
                    Op_Data.BlastRecorderDeviceUsages = GetBlastRecorderDeviceUsages(BlastRecorder.Select);
                    if(DEVICE_USAGES_MASTER == Op_Data.BlastRecorderDeviceUsages)
                    {
                        ShowBlastRecorderMasterBriefDlg();
                    }
                    else
                    {
                        Op_Data.BlastRecorderDeviceName = GetBlastRecorderDeviceName(BlastRecorder.Select);

                        ShowBlastRecorderDetailedDlg();
                    }
                            
                    break;
                case GUI_KEY_2://向上翻1行
                    BlastRecorder.Select--;
                    if(BlastRecorder.Select >= BlastRecorder.Number)
                    {
                        BlastRecorder.Select = BlastRecorder.Number-1;
                    }
                    if((BlastRecorder.Select/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = BlastRecorder.Select/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), BlastRecorder.Select%PageSize);
                    if((UpdateFlag[BlastRecorder.Select%PageSize] == 0) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
                    {//该条记录没有上传
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    break;
                case GUI_KEY_8://向下翻1行
                    BlastRecorder.Select++;
                    if(BlastRecorder.Select >= BlastRecorder.Number)
                    {
                        BlastRecorder.Select = 0;
                    }
                    if((BlastRecorder.Select/PageSize) != ListViewPageindex)
                    {//跨页
                        ListViewPageindex = BlastRecorder.Select/PageSize;//当前所在页
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);
                        
                        ShowListViewData(hWin, ListViewPageindex);
                    }				
                    LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), BlastRecorder.Select%PageSize);
                    if((UpdateFlag[BlastRecorder.Select%PageSize] == 0) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
                    {//该条记录没有上传
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    else
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }				
                    break;
                case GUI_KEY_4://向后翻1页
                    if(BlastRecorder.Number > PageSize)
                    {
                        ListViewPageindex--;
                        if((PageSize*ListViewPageindex) >= BlastRecorder.Number)
                        {
                            ListViewPageindex = (BlastRecorder.Number-1)/PageSize;
                        }
                        BlastRecorder.Select = PageSize*ListViewPageindex;
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), BlastRecorder.Select%PageSize);
                        if((UpdateFlag[BlastRecorder.Select%PageSize] == 0) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
                        {//该条记录没有上传
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }
                        else
                        {
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }					
                    }
                    break;
                case GUI_KEY_6://向前翻1页
                    if(BlastRecorder.Number > PageSize)
                    {
                        ListViewPageindex++;
                        if(ListViewPageindex > ((BlastRecorder.Number-1)/PageSize))
                        {
                            ListViewPageindex = 0;
                        }
                        BlastRecorder.Select = PageSize*ListViewPageindex;		
                        ShowListViewData(hWin, ListViewPageindex);
                        SCROLLBAR_SetValue(hScrollbar, ListViewPageindex*PageSize);				
                        LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), BlastRecorder.Select%PageSize);	
                        if((UpdateFlag[BlastRecorder.Select%PageSize] == 0) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
                        {//该条记录没有上传
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }
                        else
                        {
                            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }						
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


void ShowBlastRecorderListViewDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
