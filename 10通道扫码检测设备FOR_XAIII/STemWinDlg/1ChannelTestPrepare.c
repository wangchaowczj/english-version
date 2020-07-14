#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "等待接入雷管...",       GUI_ID_TEXT0,     0, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     0, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "方向键快速设置",       GUI_ID_TEXT2,     0, 110, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT4,     0, 170, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
    ErrorStatus Result;
    
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
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
            
            if(NeedScan())
            {
                if(Channel.BarCode[0].Status == BARCODE_NEW)
                {
                    sprintf((char*)OpData.GuiTemp, "管码:%s", Channel.BarCode[0].CodeExternal);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);	
                }
                else
                {
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "等待扫码...");
                }
                sprintf((char*)OpData.GuiTemp, "按5键下一盒 %d/%d", DetOutsideCodeBuffer.Count, DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX);
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);	
            }
            
            ConnectChannel(0);
			
			switch(UserParam.DetType)
			{
				case DET_TYPE_XA_02:
					sprintf((char*)OpData.GuiTemp, "类型:XA-02 统计:%ld", OpData.DetDetectorPassCount);
					break;
				case DET_TYPE_XA_06:
					sprintf((char*)OpData.GuiTemp, "类型:XA-06 统计:%ld", OpData.DetDetectorPassCount);
					break;
				case DET_TYPE_XA_21:
					sprintf((char*)OpData.GuiTemp, "类型:XA-21 统计:%ld", OpData.DetDetectorPassCount);
					break;
				default:
					sprintf((char*)OpData.GuiTemp, "类型:未知 统计:%ld", OpData.DetDetectorPassCount);
					break;
			}            
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS,0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS,0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
				case GUI_CUSTOM_KEY_FOOT:					
                    if(UserParam.NetMode == 0)
                    {
						
						if(UserParam.DetCodeWrite == 1)
						{//写码重复写入检验非联网模式
							if(BarCode.Status == BARCODE_IDLE)
							{
								break;
							}
							OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);//不删掉任务发指令会死机
							if(DetWriteTestRecorderCheck(0) == DET_TEST_RECORDER_REPEAT_ERR)
							{
								sprintf((char*)OpData.GuiTemp, "确认重复写入:%s", BarCode.CodeExternal);									
								if(RESULT_YES != ShowCustomMessageBox((char*)OpData.GuiTemp, OpData.DlgTitle, MB_YESNO))
								{
									WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_150MS_ID, GUI_TIMER_150MS_ID,0);
									break;
								}
							}		
						}
                        DetTestStart();                   
                    }
                    break;
				case GUI_CUSTOM_KEY_BACK:
					OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
					ShowMainMenuDlg();
					break;
				case GUI_CUSTOM_KEY_5:
					if(NeedScan() && (UserParam.NetMode == 0))
					{
						if(DetOutsideCodeBuffer.Count < DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX)
						{
							if(RESULT_YES == ShowCustomMessageBox("确认下一盒?", OpData.DlgTitle, MB_YESNO))
							{
								DetOutsideCodeBuffer.Count = 0;
								
								sprintf((char*)OpData.GuiTemp, "按5键下一盒 %d/%d", DetOutsideCodeBuffer.Count, DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);	
							}
						}
						else
						{
							DetOutsideCodeBuffer.Count = 0;
							
							sprintf((char*)OpData.GuiTemp, "按5键下一盒 %d/%d", DetOutsideCodeBuffer.Count, DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);
						}

				
					}						
					break;
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F3:					
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F4:
                    OSTaskDel(APP_FACTORY_API_TASK_PRIO); 
					if(DET_CODE_VERIFY == UserParam.Stage)
					{
						break;
					}
					else
					{
						if(ENTER_FACTORY == UserParam.Stage)
						{
							ShowDetTestContentEnterFactorySetDlg(1);					
						}
						else if((SEMIFINISHED == UserParam.Stage)||(FINISHED == UserParam.Stage))
						{
							ShowTestItemAndCodingSetDlg(1);
						}					
					}                    

					break;				
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:						
					if(NeedScan() && (UserParam.NetMode == 0))
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);

						if(ScanKeyBoard.Status == SCAN_KEYBOARD_END)
						{//已经扫到数据
							BarCodeDataAnalyst();//分析数据
							ClearScanKeyBoard();
						}					
						if(BarCode.Status == BARCODE_ERROR)
						{
							BarCode.Status = BARCODE_IDLE;
							
							TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "等待扫码...");
							
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						}
						else if(BarCode.Status == BARCODE_NEW)
						{
                            if(((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED)) &&(UserParam.DetCodeWrite || UserParam.CheckDetCode))
							{
								if(!DetOutsideCodeQueueIsSameStartWith(&BarCode.CodeExternal[0]))
								{
									BarCode.Result = BARCODE_PACK_ERR;
								}							
							}
							if(DetInfoQueueItemIsExist(&DetInfoQueue, &BarCode.DetCode[0]))
							{
								BarCode.Result = BARCODE_REPEAT_ERR;
							}
//							if(VerifyDetCode())
//							{//只是为了验证雷管内码与条码是否一致
//								BarCode.Result = BARCODE_PASS;
//							}
							switch(BarCode.Result)
							{
								case BARCODE_PASS:
                                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                                    
									TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_BLUE);
									sprintf((char*)OpData.GuiTemp, "管码:%s", BarCode.CodeExternal);					
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
										
									memcpy(&Channel.BarCode[0], &BarCode, sizeof(BARCODE));	
									break;
								default:
									TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
									GetBarcodeCompareStatusErrorDesString((char*)OpData.GuiTemp);
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));								
									break;
							}
							BarCode.Status = BARCODE_IDLE;
						}						
					}
					break;
				case GUI_TIMER_120MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                    if(NeedScan())
                    {
                        if(Channel.BarCode[0].Status != BARCODE_NEW)
                        {
                            break;
                        }
                    }
                    Result = XA2_DetCheckIn();
                    if(Result != ERROR)
                    {
						if((UserParam.DetCodeWrite == 1) && (UserParam.NetMode == 0))
						{//写码重复写入检验非联网模式
							if(DetWriteTestRecorderCheck(0) == DET_TEST_RECORDER_REPEAT_ERR)
							{
								WM_DeleteTimer(pMsg->Data.v);
								sprintf((char*)OpData.GuiTemp, "确认重复写入:%s", BarCode.CodeExternal);	
								if(RESULT_YES != ShowCustomMessageBox((char*)OpData.GuiTemp, OpData.DlgTitle, MB_YESNO))
								{
									WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_150MS_ID, GUI_TIMER_150MS_ID,0);
									break;
								}
							}		
						}
						DetTestStart();
                        
                    }					
					break;
				case GUI_TIMER_150MS_ID://重复检测等待离线
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_150MS);
					Result = XA2_DetCheckIn();
					TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "等待取下重复雷管");
                    if(Result == ERROR)
					{
						Show1ChannelTestPrepareDlg();
					}
					break;
				case GUI_TIMER_500MS_ID:				
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);			

					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT1));                    

                    Voltage = GetLinABVoltage(0, NULL);
                    Current = GetCurrentValue(10, NULL);
                    
                    sprintf((char*)OpData.GuiTemp, "%0.1fV/%0.2fuA", Voltage/1000.0, Current/100.0);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp); 
				
					break;                    
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void Show1ChannelTestPrepareDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

