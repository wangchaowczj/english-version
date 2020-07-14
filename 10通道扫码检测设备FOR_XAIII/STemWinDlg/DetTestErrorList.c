#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { LISTVIEW_CreateIndirect,   "",       GUI_ID_LISTVIEW0,     5, 5, LCD_WIDTH_PIXEL-20,  191,  0,          0 }
};

const static u8 PageSize = 6;

void GetDetErrCodeDescribe(u8 DetErrCode, char* Buffer)
{
    switch((DET_ERR_CODE)DetErrCode)
    {
        case DET_PASS:
            strcpy(Buffer, "雷管正常");
            break;                    
        case DET_CODE_REPEAT_TEST:
            strcpy(Buffer, "该发雷管已经检测");
            break;
        case LV_CURRENT_LESS:
            strcpy(Buffer, "静态电流小");
            break;
        case LV_CURRENT_MORE:
            strcpy(Buffer, "静态电流大");
            break;
        case LV_REVERSE_CURRENT_LESS:
            strcpy(Buffer, "静态电流(反向)小");
            break;
        case LV_REVERSE_CURRENT_MORE:
            strcpy(Buffer, "静态电流(反向)大");
            break;
        case DET_INFO_READ_ERR:
            strcpy(Buffer, "序列码读取失败");
            break;
        case DET_FREQ_MIN:
            strcpy(Buffer, "频率小");
            break;
        case DET_FREQ_MAX:
            strcpy(Buffer, "频率大");
            break;
        case DET_TYPE_ERR:
            strcpy(Buffer, "芯片类型错误");
            break;
        case DET_BRIDGE_SHORT:
            strcpy(Buffer, "桥头短路");
            break;
        case DET_BIT_ERR:
            strcpy(Buffer, "BIT错误");
            break;					
        case DET_NO_WORK_CAP:
            strcpy(Buffer, "没有工作电容");
            break;
        case DET_PASSWORD_ERR:
            strcpy(Buffer, "密码错误");
            break;
        case DET_CHARGE_ERR:
            strcpy(Buffer, "充电错误");
            break;
        case DET_EXPEN_ERR:
            strcpy(Buffer, "准备起爆错误");
            break;
        case HV_CURRENT_LESS:
            strcpy(Buffer, "充电后电流小");
            break;
        case HV_CURRENT_MORE:
            strcpy(Buffer, "充电后电流大");
            break;
        case HV_REVERSE_CURRENT_LESS:
            strcpy(Buffer, "充电后电流(反向)小");
            break;
        case HV_REVERSE_CURRENT_MORE:
            strcpy(Buffer, "充电后电流(反向)大");
            break;
        case LV_VOLTAGE_MIN:
            strcpy(Buffer, "总线低压时电压过低");
            break;
        case LV_VOLTAGE_MAX:
            strcpy(Buffer, "总线低压时电压过高");
            break;
        case HV_VOLTAGE_MIN:
            strcpy(Buffer, "总线高压时电压过低");
            break;
        case HV_VOLTAGE_MAX:
            strcpy(Buffer, "总线高压时电压过高");
            break;
        case DET_WR_CODE_UNLOCK_PWD_ERR:
            strcpy(Buffer, "密码错误");
            break;
        case DET_WR_CODE_ERR:
        case DET_WR_CODE_VERIFY_ERR:
            strcpy(Buffer, "写内码错误");
            break;
        case DET_FIX_DELAY_NOT_EQUAL:
            strcpy(Buffer, "雷管延期错误");
            break;
        case DET_SHORT_ERR:
            strcpy(Buffer, "雷管短路");
            break;
        case DET_OPEN_CIRCUIT_ERR:
            strcpy(Buffer, "雷管断路");
            break;
        case DET_WR_CODE_NULL_ERR:
            strcpy(Buffer, "没有扫码");
            break;
        case DET_CODE_ALL_ZERO:
            strcpy(Buffer, "没有写入内码");
            break;
        case DET_CODE_NOT_SAME:
            strcpy(Buffer, "内码不相同");
            break;	
//XAII新增错误工厂代码错误
		case DET_BIT_NO_RESPONSE:
			strcpy(Buffer, "BIT命令没有响应");
            break;
		case DET_CHARGE_NO_RESPONSE:
			strcpy(Buffer, "充电命令没有响应");
            break;
		case DET_PASSWORD_READ_ERR:
			strcpy(Buffer, "PWD读取错误	");
            break;
		case DET_PASSWORD_NOT_ACK:
			strcpy(Buffer, "发送解密指令无响应");
            break;
		case DET_CODE_RULE_ERR:
			strcpy(Buffer, "内码不符合规则");
            break;	
		case DET_READ_EEPROM_ERR:
			strcpy(Buffer, "读EEPROM出错");
            break;	
		case DET_CODE_BACKUP_ERR1:
			strcpy(Buffer, "序列码备份数据错误");
            break;	
		case DET_CODE_BACKUP_ERR2:
			strcpy(Buffer, "密码备份数据错误");
            break;	
		case DET_TEST_RECORDER_DATA_ERR2:
			strcpy(Buffer, "数据错误");
            break;	
		case DET_TEST_RECORDER_ERR1:
			strcpy(Buffer, "没有经过I型检测");
            break;	
		case DET_TEST_RECORDER_ERR2:
			strcpy(Buffer, "没有经过II型检测");
            break;
		case DET_TEST_RECORDER_ERR3:
			strcpy(Buffer, "没有经过半成品检测");
            break;
		case DET_TEST_RECORDER_CRC_ERR:
			strcpy(Buffer, "检测记录校验错误");
            break;
		case DET_TEST_RECORDER_CRC_DIFFERENT_ERR:
			strcpy(Buffer, "检测记录两次不同");
            break;
		case DET_TEST_RECORDER_REPEAT_ERR:
			strcpy(Buffer, "重复写码检测");
			break;
//		case DET_TEST_RECORDER_REPEAT_ERR2:
//			strcpy(Buffer, "成品已检测");
//			break;
		
        default:
            sprintf(Buffer,"错误代码:%02X", DetErrCode);
            break;
    }
}
static void ShowListViewData(WM_HWIN hWin, u16 Pageindex)
{
	u8 i;
	u8 Number,Index, ChannelIndex;
	WM_HWIN hItem;
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	Number = GetChannelOldStatusErrorCount();
	for(i=0; i < PageSize; i++)
	{
		Index = Pageindex*PageSize+i;
		if(Index < Number)
		{
			LISTVIEW_AddRow(hItem, NULL);
			
			ChannelIndex = GetChannelOldStatusErrorIndex(Index);
			
			sprintf((char*)OpData.GuiTemp,"%d", ChannelIndex+1);
			LISTVIEW_SetItemText(hItem, 0,i, (char*)OpData.GuiTemp);
			
            GetDetErrCodeDescribe(Channel.OldStatus[ChannelIndex], (char*)OpData.GuiTemp);
			
			LISTVIEW_SetItemText(hItem, 1,i, (char*)OpData.GuiTemp);		
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
	u8 PageIndex;
	u16 U8Value;
	u8 Number;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("错误信息");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			Number = GetChannelOldStatusErrorCount();
		
			//滚动条设置
			SCROLLBAR_SetDefaultColor(GUI_BLUE, 0);//滚动条颜色
	//		SCROLLBAR_SetDefaultBkColor(GREEN,0);//滚动条底色		
			hScrollbar = SCROLLBAR_Create(305, 5, 10, 191, hWin, GUI_ID_SCROLLBAR0, WM_CF_SHOW, SCROLLBAR_CF_VERTICAL);
						
			//总页数
			U8Value = Number/PageSize;
			if(Number%PageSize)
			{
				U8Value++;
			}
			SCROLLBAR_SetValue(hScrollbar, 0);
			SCROLLBAR_SetNumItems(hScrollbar, U8Value);
			SCROLLBAR_SetPageSize(hScrollbar, 1);
			
			if(Number <= PageSize)
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
			
			LISTVIEW_AddColumn(hItem, 12*4+8, "通道", GUI_TA_CENTER);
			LISTVIEW_AddColumn(hItem, 12*18+28, "错误信息", GUI_TA_CENTER);	
			
			OpData.Select = 0;
			ShowListViewData(hWin, OpData.Select/PageSize);
			LISTVIEW_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0), OpData.Select%PageSize);			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
                    ShowChannelStatusFigureDlg(1);				
					break;
				case GUI_CUSTOM_KEY_F1:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					Number = GetChannelOldStatusErrorCount();
					if(Number > 1)
					{
						U8Value = OpData.Select%PageSize;
						OpData.Select--;
						if(OpData.Select >= Number)
						{
							OpData.Select = Number-1;
						}
						PageIndex = OpData.Select/PageSize;
						
						if((U8Value == 0) && (Number > PageSize))
						{							
							ShowListViewData(hWin, PageIndex);
						}
						U8Value = OpData.Select%PageSize;
						LISTVIEW_SetSel(hItem, U8Value);
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);										
					}
					break;
				case GUI_CUSTOM_KEY_F2:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					Number = GetChannelOldStatusErrorCount();
					if(Number > PageSize)
					{
						PageIndex = OpData.Select/PageSize;
						PageIndex++;
						if(PageIndex*PageSize >= Number)
						{
							PageIndex = 0;
						}
						
						OpData.Select = PageIndex*PageSize;
						PageIndex = OpData.Select/PageSize;
						ShowListViewData(hWin, PageIndex);
						LISTVIEW_SetSel(hItem, 0);	
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);					
					}
					break;					
				case GUI_CUSTOM_KEY_F3:
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					Number = GetChannelOldStatusErrorCount();
					if(Number > 1)
					{
						OpData.Select++;
						if(OpData.Select >= Number)
						{
							OpData.Select = 0;
						}
						PageIndex = OpData.Select/PageSize;
						U8Value = OpData.Select%PageSize;
						if((U8Value == 0) && (Number > PageSize))
						{							
							ShowListViewData(hWin, PageIndex);
						}
						LISTVIEW_SetSel(hItem, U8Value);
						SCROLLBAR_SetValue(hScrollbar, PageIndex);
						WM_SetFocus(hItem);										
					}
					break;
				case GUI_CUSTOM_KEY_F4:	
					hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
					hScrollbar = WM_GetDialogItem(hWin, GUI_ID_SCROLLBAR0);
					Number = GetChannelOldStatusErrorCount();
					if(Number > PageSize)
					{
						PageIndex = OpData.Select/PageSize;
						PageIndex--;
						if(PageIndex*PageSize >= Number)
						{
							PageIndex = (Number-1)/PageSize;
						}
						
						OpData.Select = PageIndex*PageSize;
						PageIndex = OpData.Select/PageSize;
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

void ShowDetTestErrorListDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

