#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 155, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 180, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

static u8 ScanChannelIndex = 0;

void GetStageDesString(char* Buffer)
{
    switch(UserParam.Stage)
    {
        case ENTER_FACTORY:
            strcpy(Buffer, "入厂检测");
            break;
        case SEMIFINISHED:
            strcpy(Buffer, "半成品检测");
            break;
        case FINISHED:
            strcpy(Buffer, "成品检测");
            break;
        case DET_CODE_VERIFY:
            strcpy(Buffer, "内码验证");
            break;
        default:
            strcpy(Buffer, "未知");
            break;
    }
}
void GetBarcodeCompareStatusErrorDesString(char* Buffer)
{
    switch(BarCode.Result)
    {
        case BARCODE_PASS:
            strcpy(Buffer, "条码:正常");
            break;
        case BARCODE_DET_TYPE_ERR:
            strcpy(Buffer, "条码:芯片类型错误!");								
            break;
        case BARCODE_FACTORY_ERR:
            strcpy(Buffer, "条码:工厂代码错误!");								
            break;	
        case BARCODE_PRODUCT_TIME_ERR:
            strcpy(Buffer, "条码:生产时间错误!");													
            break;
        case BARCODE_DELAY_ERR:
            strcpy(Buffer, "条码:延期时间错误!");								
            break;	
        case BARCODE_REPEAT_ERR:
            strcpy(Buffer, "条码:该发雷管已经检测!");
            break;
        case BARCODE_PACK_ERR:
            strcpy(Buffer, "条码:不是同一盒!");								
            break;           
        default:
            sprintf((char*)Buffer, "条码错误(%d)", (u8)BarCode.Result);								
            break;
    }
}
static void ShowOldErrorChannle(WM_HWIN TextWin)
{
	u8 i;
	
	if(GetChannelOldStatusErrorCount())
	{
		strcpy((char*)OpData.GuiTemp, "上轮错:");
		for(i=0; i < Channel.Count; i++)
		{
			if ((Channel.OldStatus[i] != DET_PASS) && (Channel.OldStatus[i] != DET_TESTING))
			{
				if(i == 0)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "①");
				}
				else if(i == 1)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "②");
				}
				else if(i == 2)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "③");
				}
				else if(i == 3)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "④");
				}
				else if(i == 4)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑤");
				}
				else if(i == 5)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑥");
				}
				else if(i == 6)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑦");
				}
				else if(i == 7)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑧");
				}
				else if(i == 8)
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑨");
				}
				else
				{
					strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑩");
				}			
			}
		}
		TEXT_SetText(TextWin, (char*)OpData.GuiTemp);
	}
	else
	{
		TEXT_SetText(TextWin, " ");
	}
}
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
static void ShowChannelStatusFigure(WM_HWIN hWin, u8 Refresh)
{
	u8 i;
	u16 x, y;
	u8 space = 0;
	u8 r = 20;
	GUI_COLOR  Color;
	
	WM_SelectWindow(hWin);	
	
	GUI_SetColor(GUI_BLACK);
	GUI_SetBkColor(COLOR_DLG_BK);	
	
	GUI_SetFont(&NORMAL_FONT);

	if (Channel.Count <= 5)
	{
		space = (LCD_WIDTH_PIXEL - Channel.Count*r*2)/(Channel.Count+1);
		y = LCD_HEIGHT_PIXEL/2 - STATUSBAR_HEIGHT;
		for(i = 0; i < Channel.Count;i++)
		{
			x = space+r+(space+2*r)*i;
			if(Refresh&0x01)
			{
				Color = GetChannelFigureColor(Channel.NewStatus[i]);

				GUI_SetColor(Color);
				GUI_FillCircle(x, y, r);
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(Color);
				if(NeedScan())
				{
					if(ScanChannelIndex == i)
					{
						GUI_SetColor(GUI_RED);
					}				
				}

				sprintf((char*)OpData.GuiTemp,"%d",i+1);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);				
			}
		
			if(Refresh&0x02)
			{
				if(i%2 == 0)
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTYELLOW);
				}
				else
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTGREEN);
				}

				if ((Channel.BarCode[i].Status == BARCODE_NEW) && (Channel.BarCode[i].Result == BARCODE_PASS))
				{
					sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[i].CodeExternal[8]);
					GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的内码后5位	
				}
				else
				{
					GUI_DispStringHCenterAt("     ", x, y+r+2);
				}			
			}							
		}		
	}
	else
	{
		space = (LCD_WIDTH_PIXEL - 5*r*2)/6;
		y = (LCD_HEIGHT_PIXEL-30-space*2-4*r)/2+r - STATUSBAR_HEIGHT;
		
		for(i = 0; i < 5; i++)
		{
			x = space+r+(space+2*r)*i;
			if(Refresh&0x01)
			{
				Color = GetChannelFigureColor(Channel.NewStatus[i]);
				GUI_SetColor(Color);
				GUI_FillCircle(x, y, r);
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(Color);
				if(NeedScan())
				{
					if(ScanChannelIndex == i)
					{
						GUI_SetColor(GUI_RED);
					}				
				}
				
				sprintf((char*)OpData.GuiTemp,"%d",i+1);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);				
			}
		
			if(Refresh&0x02)
			{
				if(i%2 == 0)
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTYELLOW);
				}
				else
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTGREEN);
				}
				if ((Channel.BarCode[i].Status == BARCODE_NEW) && (Channel.BarCode[i].Result == BARCODE_PASS))
				{
					sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[i].CodeExternal[8]);
					GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的内码前3位	
				}
				else
				{
					GUI_DispStringHCenterAt("     ", x, y+r+2);
				}			
			}
		}
		y += space*2+2*r-10;		
		for(i = 0; i < Channel.Count-5;i++)
		{
			x = space+r+(space+2*r)*i;
			if(Refresh&0x01)
			{
				Color = GetChannelFigureColor(Channel.NewStatus[5+i]);
				GUI_SetColor(Color);
				GUI_FillCircle(x, y, r);
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(Color);
				if(NeedScan())
				{
					if(ScanChannelIndex == (5+i))
					{
						GUI_SetColor(GUI_RED);
					}				
				}
				
				sprintf((char*)OpData.GuiTemp,"%d",i+6);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);			
			}
				
			if(Refresh&0x02)
			{
				if(i%2 == 0)
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTYELLOW);
				}
				else
				{
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_LIGHTGREEN);
				}
				if ((Channel.BarCode[5+i].Status == BARCODE_NEW) && (Channel.BarCode[5+i].Result == BARCODE_PASS))
				{
					sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[5+i].CodeExternal[8]);
					GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的内码前3位	
				}
				else
				{
					GUI_DispStringHCenterAt("     ", x, y+r+2);
				}			
			}			
		}
	}
	GUI_SetColor(GUI_BLACK);
	GUI_SetBkColor(COLOR_DLG_BK);	
}

void DetTestStart(void)
{
    OpData.NetModeNext = 0;
    if(UserParam.NetMode)
    {
        OPEN_IO_RELAY1();    
    }

    OpData.TestStatus = TEST_DOING;
    SetSystemSleepCount(OSTimeGet());

	if(Channel.Count == 1)
	{
		Show1ChannelTestResultDlg();
	}
	else
	{
		ShowChannelStatusFigureDlg(0);        
	} 
}
void ShowChannelTestPrepareDlg(void)
{
	if(Channel.Count == 1)
	{
		Show1ChannelTestPrepareDlg();
	}
	else
	{
		ShowChannelTestPrepareFigureDlg();       
	}         
}
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 i;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			ScanChannelIndex = 0;
            
			GetStageDesString((char*)OpData.GuiTemp);
            WindowTitleUpdate((char*)OpData.GuiTemp);

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
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);			
			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			if((GetChannelOldStatusErrorCount() == 0) && (OpData.DetDetectorPassCount == 0))
			{
				
				if(DET_CODE_VERIFY == UserParam.Stage)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "内码验证环节不需要设置");	
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "按方向键快速设置");				
				}
			}
			else
			{
				TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
				
				ShowOldErrorChannle(WM_GetDialogItem(hWin, GUI_ID_TEXT0));	
			}			
					

			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS,0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS,0);			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS,0);
			OpenScanKeyBoard();//允许扫码枪正常扫码
			SetAllChannelBarCodeIdle();
			
			OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
			TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//检测雷管是否已经连接	

            if(UserParam.NetMode)
            {
                OpData.TestStatus = TEST_WAIT_START;
                TaskCreate(APP_FACTORY_API_TASK_PRIO, (void*)0);                
            }
            else
            {
                OpData.TestStatus = TEST_UNALLOWEED;
            }
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
//					if(NeedScan() && (UserParam.NetMode == 0))
//					{
//						if(AllChannelBarCodeHasData() == 0)
//						{
//							TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
//							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "所有通道都需要扫码");						
//							break;
//						}					
//					}
                    if(UserParam.NetMode == 0)
                    {                       
                        if(OpData.TestStatus == TEST_UNALLOWEED)//如果所有通道都没电流，不允许检测
                        {
                            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "所有通道都是开路状态");	
                            TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//继续检测通道状态                            
                            break;
                        }
						if(UserParam.DetCodeWrite == 1)
						{
							OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);//不删掉任务发指令会死机
							memset((char*)&OpData.GuiTemp,0,strlen((char*)OpData.GuiTemp));
							for(i=0;i<Channel.Count;i++)
							{							
								if(DetWriteTestRecorderCheck(i) == DET_TEST_RECORDER_REPEAT_ERR)
								{
									if(i == 0)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "①");
									}
									else if(i == 1)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "②");
									}
									else if(i == 2)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "③");
									}
									else if(i == 3)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "④");
									}
									else if(i == 4)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑤");
									}
									else if(i == 5)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑥");
									}
									else if(i == 6)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑦");
									}
									else if(i == 7)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑧");
									}
									else if(i == 8)
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑨");
									}
									else
									{
										strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "⑩");
									}
								}
							}

						
	//						if(DetWriteTestRecorderCheck(0) == DET_TEST_RECORDER_REPEAT_ERR)
							if(strlen((char*)OpData.GuiTemp) > 0)//有重复检测出现
							{
								if(RESULT_YES != ShowCustomMessageBox((char*)OpData.GuiTemp, "重复写入通道号", MB_YESNO))
								{
									ShowChannelTestPrepareFigureDlg(); 
									break;
								}
							}
						}
						DetTestStart();                   
                    }
					else
					{
						if(RESULT_NULL == ShowCustomMessageBox("当前为联网模式!", OpData.DlgTitle, MB_OK))
						{						
//							return;
							break;
						}
						ShowChannelTestPrepareFigureDlg();
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
				case GUI_CUSTOM_KEY_1:
				case GUI_CUSTOM_KEY_2:
				case GUI_CUSTOM_KEY_3:
				case GUI_CUSTOM_KEY_4:
				case GUI_CUSTOM_KEY_5:
				case GUI_CUSTOM_KEY_6:
				case GUI_CUSTOM_KEY_7:
				case GUI_CUSTOM_KEY_8:
				case GUI_CUSTOM_KEY_9:
					if(NeedScan() && (UserParam.NetMode == 0))
					{
						if(((((WM_KEY_INFO*)(pMsg->Data.p))->Key) - GUI_CUSTOM_KEY_1) < Channel.Count)
						{
							ScanChannelIndex = (((WM_KEY_INFO*)(pMsg->Data.p))->Key) - GUI_CUSTOM_KEY_1;
						}
						sprintf((char*)OpData.GuiTemp, "通道%d,等待扫码...", ScanChannelIndex+1);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
					}
					break;
				case GUI_CUSTOM_KEY_0:
					if(NeedScan() && (UserParam.NetMode == 0))
					{
						if(Channel.Count >= 10)
						{
							ScanChannelIndex = 9;
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "通道10,等待扫码...");						
						}
					}
					break;					
				default:
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
						}
						else if(BarCode.Status == BARCODE_NEW)
						{
							u8 SameIndex;
//							if(DetInfoQueueItemIsExist(&DetInfoQueue, &BarCode.DetCode[0]))
//							{
//								BarCode.Result = BARCODE_REPEAT_ERR;
//							}
							if(VerifyDetCode())
							{//只是为了验证雷管内码与条码是否一致
								BarCode.Result = BARCODE_PASS;
							}
							switch(BarCode.Result)
							{
								case BARCODE_PASS:				
									SameIndex = GetBarCodeSameIndex(&BarCode);
									if (SameIndex < Channel.Count)
									{
										TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
										sprintf((char*)OpData.GuiTemp, "条码:通道(%d)相同", SameIndex+1);
										TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);	
										break;
									}									
									if(ScanChannelIndex >= Channel.Count)
									{
										ScanChannelIndex = Channel.Count -1;
									}
									
									TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_BLUE);
									sprintf((char*)OpData.GuiTemp, "外码:%s", BarCode.CodeExternal);					
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
										
									memcpy(&Channel.BarCode[ScanChannelIndex], &BarCode, sizeof(BARCODE));	
									
									ShowChannelStatusFigure(hWin, 0x02);
									
									ScanChannelIndex = GetChannelBarCodeEmptyIndex();//下一个通道
									if(ScanChannelIndex >= Channel.Count)
									{
										ScanChannelIndex = Channel.Count -1;
									}
									break;
								default:
                                    GetBarcodeCompareStatusErrorDesString((char*)OpData.GuiTemp);
									TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);									
									break;
							}
							BarCode.Status = BARCODE_IDLE;
						}						
					}
					break;			
				case GUI_TIMER_1MS_ID:				
					WM_DeleteTimer(pMsg->Data.v);			
					ShowChannelStatusFigure(hWin, 0x01);
					if(Channel.Count != 1)
					{
						TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)0);//继续检测通道状态
					}					
					break;
				case GUI_TIMER_500MS_ID:				
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);		
					UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
					break;	
				case GUI_TIMER_120MS_ID:					
					if((TaskIsExist(APP_DET_DETECTOR_TASK_PRIO) == ERROR) && (Channel.Count == 1))
					{
                        ErrorStatus Result;
                        Result = XA2_DetCheckIn();
						if(Result != ERROR)
						{
							DetTestStart();
						}					
					}
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);		
					break;				
			}
			break;
		case WM_DET_TASK:
            if(UserParam.NetMode == 0)
            {
                if(GetChannelNewStatusCount(DET_OPEN_CIRCUIT_ERR) == Channel.Count)
                {
                    OpData.TestStatus = TEST_UNALLOWEED;
                }
                else
                {
                    OpData.TestStatus = TEST_WAIT_START;
                }            
            }
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);			
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowChannelTestPrepareFigureDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

