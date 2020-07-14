#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { LISTVIEW_CreateIndirect,   "",       GUI_ID_LISTVIEW0,     22, 5, LCD_WIDTH_PIXEL-44,  165,  0,          0 },
  { TEXT_CreateIndirect,   "F1键起始管码 F2键盒流水",       GUI_ID_TEXT0,     0, 175, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

static GUI_COLOR GetChannelFigureColor(DET_ERR_CODE Status)
{
	if (DET_PASS == Status)
	{
		return 0x66FFFF;//黄色
	}
	else if (DET_OPEN_CIRCUIT_ERR == Status)
	{
		return GUI_GRAY;//灰色
	}
	else
	{
		return GUI_BROWN;//棕色
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
        
        Color = GetChannelFigureColor(Channel.NewStatus[i]);
        LISTVIEW_SetItemBkColor(hItem,0,i,0,Color);
        LISTVIEW_SetItemBkColor(hItem,0,i,1,Color); 
            
        sprintf((char*)OpData.GuiTemp,"%d", i+1);
        LISTVIEW_SetItemText(hItem, 0,i, (char*)OpData.GuiTemp);
        
        LISTVIEW_SetItemText(hItem, 1,i, (char*)&Channel.BarCode[i].CodeExternal[8]);

        Color = GetChannelFigureColor(Channel.NewStatus[5+i]);
        LISTVIEW_SetItemBkColor(hItem,2,i,0,Color);
        LISTVIEW_SetItemBkColor(hItem,2,i,1,Color);
        
        sprintf((char*)OpData.GuiTemp,"%d", 6+i);
        LISTVIEW_SetItemText(hItem, 2,i, (char*)OpData.GuiTemp);

        LISTVIEW_SetItemText(hItem, 3,i, (char*)&Channel.BarCode[5+i].CodeExternal[8]);			
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
			LISTVIEW_AddColumn(hItem, 12*5+20, "管码", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*4+10, "通道", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*5+20, "管码", GUI_TA_CENTER);			
                        
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
					ShowMainMenuDlg();
					break;
				case GUI_CUSTOM_KEY_OK:
                case GUI_CUSTOM_KEY_FOOT:
                    if(OpData.TestStatus == TEST_UNALLOWEED)
                    {
                        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "所有通道都必需接入雷管");	
                        TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//继续检测通道状态                            
                        break;
                    }
                    DetTestStart();   
					break;
				case GUI_CUSTOM_KEY_F1:
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    ShowDetOutsideForBJDlg(1);
					break;
				case GUI_CUSTOM_KEY_F2:
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    ShowDetOutsideBoxIndexForBJDlg(1);
					break;
            }
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);

                    if((UserParam.DetOutsideStartBoxIndex/100) > 999)
                    {
                        ShowCustomMessageBox("请重新设置起始管码", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }
                    if(GetCurrentDetOutsideStart(OpData.GuiTemp) == ERROR)
                    {
                        ShowCustomMessageBox("起始管码错误!", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }
                    if((UserParam.DetOutsideStartBoxIndex%10) != 0)
                    {
                        ShowCustomMessageBox("管码最后1位必需是0", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }

                    if((((UserParam.DetOutsideStartBoxIndex+10)%100) > UserParam.InBoxNumber) || (((UserParam.InBoxNumber%10) != 0) && ((UserParam.InBoxNumber%5) == 0)))
                    {
                        ShowCustomMessageBox("盒内数量错误", OpData.DlgTitle, MB_OK);
                        ShowDetOutsideForBJDlg(1);
                        return;
                    }
                    CreateChannelDetOutside();
                    
					OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//检测雷管是否已经连接				
					break;            
				case GUI_TIMER_2MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);			
					ShowListViewData(hWin);
					TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//继续检测通道状态				
					break;					
			}
			break;
		case WM_DET_TASK:
            if(GetChannelNewStatusCount(DET_PASS) == 0)//Channel.Count
            {
                OpData.TestStatus = TEST_UNALLOWEED;
            }
            else
            {
                OpData.TestStatus = TEST_WAIT_START;
            }   
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS,0);			
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void Show10ChannelTestPrepareListForBJDataBindingDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

