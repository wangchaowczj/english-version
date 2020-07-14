#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     10, 5, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     10, 30, LCD_WIDTH_PIXEL,  25,  0,          0 },
//  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     10, 55, LCD_WIDTH_PIXEL,  25,  0,          0 },
//  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     10, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT4,     10, 55, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT5,     10, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },  
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT6,     10, 105, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT7,     10, 130, LCD_WIDTH_PIXEL,  25,  0,          0 } 
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 FacCode = 0;
	WM_HTIMER Htimer;
	MSG_BOX_RESULT MsgValue;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();

			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("测试项目");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			if(BarCode.Status == BARCODE_ERROR)
			{
				TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT4),GUI_RED);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "条码错误,请重新扫码!");
			}
			else if(BarCode.Status == BARCODE_NEW)
			{
				switch(BarCode.DetType)
				{
					case DET_TYPE_XA_02:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "芯片类型:XA-02");
						break;
					case DET_TYPE_XA_06:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "芯片类型:XA-06");
						break;						
					case DET_TYPE_XA_21:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "芯片类型:XA-21");
					default:
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "芯片类型:未知");
						break;
				}
				
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码写入:写");	
				
				if(FactoryCodeInsideToOutside(BarCode.FactoryCode, &FacCode) == ERROR)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "工厂代码:未知");										
				}
				else
				{
					sprintf((char*)OpData.GuiTemp, "工厂代码:%d", FacCode);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);				
				}				


				sprintf((char*)OpData.GuiTemp, "生产时间:%u", BarCode.ProductTime);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), (char*)OpData.GuiTemp);		

				memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
				strcpy((char*)OpData.GuiTemp, "特征号码:");
				OpData.GuiTemp[strlen((char*)OpData.GuiTemp)] = BarCode.Feature;
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6), (char*)OpData.GuiTemp);	
					
				if(XA2_DetIsFixDelay(BarCode.DetType))
				{//固定延期芯片显示延期才有意义
					sprintf((char*)OpData.GuiTemp, "雷管延期:%ums", BarCode.Delay);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7), (char*)OpData.GuiTemp);					
				}				
			}
		
			Htimer = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			ClearTimerHandleBuffer();
			SaveTimerHandleToBuffer(Htimer);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(BarCode.Status != BARCODE_NEW)
					{
						break;
					}
					
					RemoveTimerHandleAndDeleteById(GUI_TIMER_100MS_ID);
					MsgValue = ShowCustomMessageBox("确定修改?", OpData.DlgTitle, MB_YESNO);
					if(RESULT_NO == MsgValue)
					{
						ShowDetTestContentModifyDlg(GetWindowParam());
						break;
					}
					else if(RESULT_NULL == MsgValue)
					{
						return;
					}
					UserParam.DetType = BarCode.DetType;
					UTF8StringToGBK("雷管类型", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetType) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					
					
					UserParam.FactoryCode = BarCode.FactoryCode;
					UTF8StringToGBK("工厂代码", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.FactoryCode) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}									
					UserParam.ProductTime = BarCode.ProductTime;
					UTF8StringToGBK("生产时间", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.ProductTime) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					UserParam.Feature = BarCode.Feature;
					UTF8StringToGBK("特征号", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.Feature) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					
					UserParam.DetCodeWrite = 1;
					UTF8StringToGBK("内码写入", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetCodeWrite) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					
					UserParam.CheckDetCode = 0;
					UTF8StringToGBK("内码验证", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.CheckDetCode) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					UserParam.DelayWrite = 1;
					UTF8StringToGBK("延期写入", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DelayWrite) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					
					if(XA2_DetIsFixDelay(UserParam.DetType))
					{
						UserParam.DetDelay = BarCode.Delay;
					}
					else
					{
						UserParam.DetDelay = 0;//在线可编程默认为0
					}
					
					UTF8StringToGBK("雷管延期", (char*)OpData.GuiTemp);						
					if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetDelay) == ERROR)
					{
						if(RESULT_NULL == ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK))
						{
							return;
						}
						ShowDetTestContentMenuDlg(GetWindowParam());
						break;
					}
					if(GetWindowParam() == 0)
					{
						ShowDetTestContentMenuDlg(GetWindowParam());
					}
					else
					{
						BarCode.Status = BARCODE_IDLE;
						ClearScanKeyBoard();
						ShowChannelTestPrepareDlg();
					}									
					break;
				case GUI_CUSTOM_KEY_BACK:
					RemoveTimerHandleAndDeleteById(GUI_TIMER_100MS_ID);
					if(BarCode.Status == BARCODE_NEW)
					{
						MSG_BOX_RESULT MsgValue;
						
						MsgValue = ShowCustomMessageBox("确定取消修改?", OpData.DlgTitle, MB_YESNO);
						if(RESULT_YES == MsgValue)
						{
							ShowDetTestContentMenuDlg(GetWindowParam());
							break;
						}
						else if(RESULT_NULL == MsgValue)
						{
							return;
						}
					}					
					ShowDetTestContentModifyDlg(GetWindowParam());
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:								
				if(ScanKeyBoard.Status == SCAN_KEYBOARD_END)
					{//已经扫到数据
						BarCodeDataAnalyst();//分析数据
						ClearScanKeyBoard();
						ShowDetTestContentModifyDlg(GetWindowParam());
						break;
					}						

					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);					
					break;
				default:
					break;
			}		
			break;				
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTestContentModifyDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);	
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

