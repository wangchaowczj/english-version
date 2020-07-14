#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { LISTVIEW_CreateIndirect,   "",       GUI_ID_LISTVIEW0,     22, 5, LCD_WIDTH_PIXEL-44,  165,  0,          0 },
  { TEXT_CreateIndirect,   "检测中...",       GUI_ID_TEXT0,     0, 90, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "F1键查看错误信息",       GUI_ID_TEXT1,     0, 175, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

static GUI_COLOR GetChannelFigureColor(DET_ERR_CODE Status)
{
	if (DET_PASS == Status)
	{
		return GUI_GREEN;
	}
	else
	{
		return GUI_RED;
	}
}

static void ShowListViewData(WM_HWIN hWin)
{
	u8 i;
	u16 Number;
	WM_HWIN hItem;
	GUI_COLOR  Color;

	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);

	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < 5; i++)
	{
        LISTVIEW_AddRow(hItem, NULL);
        
        Color = GetChannelFigureColor(Channel.Info[i].Status);
        LISTVIEW_SetItemBkColor(hItem,0,i,0,Color);
        LISTVIEW_SetItemBkColor(hItem,0,i,1,Color); 
            
        sprintf((char*)OpData.GuiTemp,"%d", i+1);
        LISTVIEW_SetItemText(hItem, 0,i, (char*)OpData.GuiTemp);
        
        LISTVIEW_SetItemText(hItem, 1,i, (char*)&Channel.BarCode[i].CodeExternal[0]);

        Color = GetChannelFigureColor(Channel.Info[5+i].Status);
        LISTVIEW_SetItemBkColor(hItem,2,i,0,Color);
        LISTVIEW_SetItemBkColor(hItem,2,i,1,Color);
        
        sprintf((char*)OpData.GuiTemp,"%d", 6+i);
        LISTVIEW_SetItemText(hItem, 2,i, (char*)OpData.GuiTemp);

        LISTVIEW_SetItemText(hItem, 3,i, (char*)&Channel.BarCode[5+i].CodeExternal[0]);			
	}
}
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
            GetStageDesString((char*)OpData.GuiTemp);
            WindowTitleUpdate((char*)OpData.GuiTemp);
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);            
						
			//表内容设置	
			hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
			LISTVIEW_SetRowHeight(hItem, 24+2);	
			LISTVIEW_SetBkColor(hItem, 0, GUI_WHITE);//没有选中的行为为白底
			LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
			LISTVIEW_SetGridVis(hItem, 1);	//带分隔	

			//表头设置
			hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
			HEADER_SetHeight(hHeader, 30);//设置表头高度;
			HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
			HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色
			
			LISTVIEW_AddColumn(hItem, 12*4+10, "通道", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*14+50, "管码", GUI_TA_CENTER);		
            
            WM_HideWindow(hItem);
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
            
            OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);             
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
                    if(OpData.TestStatus == TEST_WAIT_OFFLINE)
                    {
                        OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                        ShowChannelTestPrepareDlg();                  
                    }
					break;
				case GUI_CUSTOM_KEY_F1:
                    if(OpData.TestStatus == TEST_WAIT_OFFLINE)
                    {
						if(GetChannelOldStatusErrorCount() != 0)
						{//有错误雷管时可查看错误雷管详细信息
							OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
							ShowDetTestErrorListDlg();
						}
                    }                   
					break;
            }
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_3MS_ID://所有雷管都已经下线				
					WM_DeleteTimer(pMsg->Data.v);
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);//所有雷管都已经下线
                    ShowChannelTestPrepareDlg();							
					break;
				case GUI_TIMER_2MS_ID://检测结束				
					WM_DeleteTimer(pMsg->Data.v);
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
                    ShowListViewData(hWin);
                    
                    WriteDetTestRecord();//保存测试结果
                    if(GetChannelOldStatusErrorCount() == 0)//所有雷管都正确流水号才增加
                    {
                        WriteDetDataBindingRecord();//保存三码绑定数据
                        
                        UserParam.DetOutsideStartBoxIndex += Channel.Count;
                        
                        if ((UserParam.DetOutsideStartBoxIndex%100) >= UserParam.InBoxNumber)
                        {
                            UserParam.DetOutsideStartBoxIndex = UserParam.DetOutsideStartBoxIndex - (UserParam.DetOutsideStartBoxIndex%100);
                            UserParam.DetOutsideStartBoxIndex += 100;
                        }
                        UTF8StringToGBK("起始管码后5位", (char*)OpData.GuiTemp);
                        if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetOutsideStartBoxIndex) == ERROR)
                        {
                            if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
                            {
                                return;
                            }
                        }                    
                    }

                    
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);                  
                    TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)2);//等待所有雷管下线
                    
                    if(GetChannelOldStatusErrorCount() == 0)
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "请取下所有雷管");
                    }
					break;            
				case GUI_TIMER_1MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);
                    if(GetWindowParam() == 0)
                    {
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                        
                        OSTaskDel(APP_DET_DETECTOR_TASK_PRIO); 
                        TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)1);//开始检测雷管
                        SetSystemSleepCount(OSTimeGet());
                    }
                    else
                    {//从查看错误信息界面返回
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
                        ShowListViewData(hWin);                    

						OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                        
						TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)2);//等待所有雷管下线	
                    }							
					break;					
			}
			break;
		case WM_DET_TASK:
            if(OpData.TestStatus != TEST_WAIT_OFFLINE)
            {//检测结束
                OpData.TestStatus = TEST_WAIT_OFFLINE;
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS,0);            
            }
            else
            {//雷管已经下线
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3MS_ID, GUI_TIMER_3MS,0); 
            }            						
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void Show5ChannelTestResultListForBJDataBindingDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

