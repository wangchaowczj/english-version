#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "检测中...",       GUI_ID_TEXT0,     0, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     0, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     0, 110, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
    
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
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            OSTaskDel(APP_DET_DETECTOR_TASK_PRIO); 
            TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)1);//开始检测雷管
            SetSystemSleepCount(OSTimeGet());
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
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID://检测结束				
					WM_DeleteTimer(pMsg->Data.v);
                    
                    
                    WriteDetTestRecord();//保存测试结果
                               
                    if(GetChannelOldStatusErrorCount() == 0)
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
                    else
                    {
                        TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
                    }
                    
                    GetDetErrCodeDescribe(Channel.OldStatus[0], (char*)OpData.GuiTemp);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
                    
                    if((GetChannelOldStatusErrorCount() == 0) || (!DetCodeIsAllZero(&Channel.Info[0])))
                    {
                        sprintf((char*)OpData.GuiTemp, "内码:%02X%02X%02X", 
                            Channel.Info[0].ser1, Channel.Info[0].ser2, Channel.Info[0].ser3);
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
                    }  
                    
                    sprintf((char*)OpData.GuiTemp, "管码:%s", Channel.BarCode[0].CodeExternal);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);
                    
                    if((GetChannelOldStatusErrorCount() == 0) || UserParam.AutoBack)
                    {
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS,0);
                    }
					break;
				case GUI_TIMER_120MS_ID:					
                    if(BJ_DetCheckIn() != ERROR)
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                    }
                    else
                    {
                        OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                        ShowChannelTestPrepareDlg();
                    }							
					break;
				case GUI_TIMER_500MS_ID:					
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);			

					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));                  
                    
                    if(OpData.TestStatus == TEST_WAIT_OFFLINE)
                    {
                        Voltage = GetLinABVoltage(0, NULL);
                        Current = GetCurrentValue(10, NULL);
                        
                        sprintf((char*)OpData.GuiTemp, "%0.1fV/%0.2fuA", Voltage/1000.0, Current/100.0);
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);                     
                    } 							
					break;                    
			}
			break;
		case WM_DET_TASK://检测结束
            OpData.TestStatus = TEST_WAIT_OFFLINE;
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);            						
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void Show1ChannelTestResultForBJDataBindingDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

