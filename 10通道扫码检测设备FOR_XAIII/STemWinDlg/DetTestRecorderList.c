#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { LISTVIEW_CreateIndirect,   "",       GUI_ID_LISTVIEW0,     5, 5, LCD_WIDTH_PIXEL-20,  165,  0,          0 },
  { TEXT_CreateIndirect,   "1键删除数据 2键导出数据",       GUI_ID_TEXT0,     0, 175, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

const static u8 PageSize = 5;

static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u8 i;
	u16 Number,Index;
	WM_HWIN hItem;
	u32 TotilNum, ErrNum;
	u8 FileName[7];
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < DetTestRecorder.Number)
		{
			LISTVIEW_AddRow(hItem, NULL);
			
			sprintf((char*)OpData.GuiTemp,"%d", Index+1);
			LISTVIEW_SetItemText(hItem, 0,i, (char*)OpData.GuiTemp);
			
			memset(FileName, 0, sizeof(FileName));
			memcpy(FileName, DetTestRecorder.FileName[Index], sizeof(DetTestRecorder.FileName[0]));
			if(ERROR == ReadDetTestRecordTestCount(FileName, &TotilNum, &ErrNum))
			{
				TotilNum = 0;
				ErrNum = 0;
			}
			OpData.GuiTemp[0] = FileName[0];
			OpData.GuiTemp[1] = FileName[1];
			OpData.GuiTemp[2] = '-';
			OpData.GuiTemp[3] = FileName[2];
			OpData.GuiTemp[4] = FileName[3];
			OpData.GuiTemp[5] = '-';
			OpData.GuiTemp[6] = FileName[4];
			OpData.GuiTemp[7] = FileName[5];
			OpData.GuiTemp[8] = 0x00;
			LISTVIEW_SetItemText(hItem, 1,i, (char*)OpData.GuiTemp);

			sprintf((char*)OpData.GuiTemp,"%d", ErrNum);
			LISTVIEW_SetItemText(hItem, 2,i, (char*)OpData.GuiTemp);

			sprintf((char*)OpData.GuiTemp,"%d", TotilNum);
			LISTVIEW_SetItemText(hItem, 3,i, (char*)OpData.GuiTemp);			
		}
	}
}
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	SCROLLBAR_Handle hScrollbar;
	WM_HWIN hItem;
	HEADER_Handle hHeader ;
	u16 PageIndex;
	u16 U16Value;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("检测记录");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			if(DetTestRecorder.Select >= DetTestRecorder.Number)
			{
				DetTestRecorder.Select = 0;
			}			
			//滚动条设置
			SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
	//		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
			hScrollbar = SCROLLBAR_Create(305, 5, 10, 165, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
						
			//总页数
			U16Value = DetTestRecorder.Number/PageSize;
			if(DetTestRecorder.Number%PageSize)
			{
				U16Value++;
			}
			SCROLLBAR_SetValue(hScrollbar, DetTestRecorder.Select/PageSize);
			SCROLLBAR_SetNumItems(hScrollbar, U16Value);
			SCROLLBAR_SetPageSize(hScrollbar, 1);
			
			if(DetTestRecorder.Number <= PageSize)
			{//不到一页也创建滚动条而采用隐藏显示的方式主要是为了方便处理
				WM_HideWindow(hScrollbar);
				WM_MoveWindow(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0),5, 0);
			}		
						
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
			
			LISTVIEW_AddColumn(hItem, 12*4+4, "序号", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*8+4, "检测时间", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*6+4, "错误数", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*4+24, "总数", GUI_TA_CENTER);			

			ShowListViewData(hWin, DetTestRecorder.Select/PageSize);
			LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), DetTestRecorder.Select%PageSize);			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					ShowDeviceInfoMenuDlg();
					break;
				case GUI_CUSTOM_KEY_1:	
					sprintf((char*)OpData.GuiTemp, "确定删除序号%d?", DetTestRecorder.Select+1);
					if(RESULT_YES != ShowCustomMessageBox((char*)OpData.GuiTemp, OpData.DlgTitle, MB_YESNO))
					{
						return;
					}
					U16Value = DetTestRecorder.Select;
					if(ERROR == DeleteDetTestRecordFile(DetTestRecorder.Select))
					{
						if(RESULT_NULL == ShowCustomMessageBox("删除失败!", OpData.DlgTitle,MB_OK))
						{
							return;
						}
					}
					else
					{
						if(RESULT_NULL == ShowCustomMessageBox("删除成功!", OpData.DlgTitle,MB_OK))
						{
							return;
						}						
					}
					DetTestRecorder.Select = U16Value;
					if(DetTestRecorder.Number == 0)
					{
						ShowDeviceInfoMenuDlg();
					}
					else
					{
						ShowDetTestRecorderListDlg();
					}					
					break;
				case GUI_CUSTOM_KEY_2:
					ShowWaitDiskInsertDlg(1);
					break;
				case GUI_CUSTOM_KEY_F1:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					if(DetTestRecorder.Number > 1)
					{
						U16Value = DetTestRecorder.Select%PageSize;
						DetTestRecorder.Select--;
						if(DetTestRecorder.Select >= DetTestRecorder.Number)
						{
							DetTestRecorder.Select = DetTestRecorder.Number-1;
						}
						PageIndex = DetTestRecorder.Select/PageSize;
						if((U16Value == 0) && (DetTestRecorder.Number > PageSize))
						{							
							ShowListViewData(hWin, PageIndex);
						}
						U16Value = DetTestRecorder.Select%PageSize;
						LISTVIEW_SetSel(hItem, U16Value);
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);										
					}
					break;
				case GUI_CUSTOM_KEY_F2:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					if(DetTestRecorder.Number > PageSize)
					{
						PageIndex = DetTestRecorder.Select/PageSize;
						PageIndex++;
						if(PageIndex*PageSize >= DetTestRecorder.Number)
						{
							PageIndex = 0;
						}
						
						DetTestRecorder.Select = PageIndex*PageSize;
						PageIndex = DetTestRecorder.Select/PageSize;
						ShowListViewData(hWin, PageIndex);
						LISTVIEW_SetSel(hItem, 0);	
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);					
					}
					break;					
				case GUI_CUSTOM_KEY_F3:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					if(DetTestRecorder.Number > 1)
					{
						DetTestRecorder.Select++;
						if(DetTestRecorder.Select >= DetTestRecorder.Number)
						{
							DetTestRecorder.Select = 0;
						}
						PageIndex = DetTestRecorder.Select/PageSize;
						U16Value = DetTestRecorder.Select%PageSize;
						if((U16Value == 0) && (DetTestRecorder.Number > PageSize))
						{							
							ShowListViewData(hWin, PageIndex);
						}
						LISTVIEW_SetSel(hItem, U16Value);
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);										
					}
					break;
				case GUI_CUSTOM_KEY_F4:	
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					if(DetTestRecorder.Number > PageSize)
					{
						PageIndex = DetTestRecorder.Select/PageSize;
						PageIndex--;
						if(PageIndex*PageSize >= DetTestRecorder.Number)
						{
							PageIndex = (DetTestRecorder.Number-1)/PageSize;
						}
						
						DetTestRecorder.Select = PageIndex*PageSize;
						PageIndex = DetTestRecorder.Select/PageSize;
						ShowListViewData(hWin, PageIndex);
						LISTVIEW_SetSel(hItem, 0);	
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);					
					}			
					break;					
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTestRecorderListDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

