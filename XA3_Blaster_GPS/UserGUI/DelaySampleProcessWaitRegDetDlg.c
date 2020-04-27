#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 110, 240,  20,  0,          0 },  
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 130, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 150, 240,  20,  0,          0 }
};
static void SaveConfim(WM_HWIN hWin, DET_INFO *RegDetInfo)
{        
    SoundOutput(0);
    
    if(Det.RegCount > DET_MAX_SIZE)
    {
        Det.RegCount = DET_MAX_SIZE-1;
    }
    Det.Info[Det.RegCount].Serl.U32Value = RegDetInfo->Serl.U32Value;
    Det.Info[Det.RegCount].Pwd.U32Value = RegDetInfo->Pwd.U32Value;
    Det.Info[Det.RegCount].Freq.U32Value = RegDetInfo->Freq.U32Value;
    Det.Info[Det.RegCount].Delay = Op_Data.DelaySampleDelay;
    
    GetDetNum();//重新统计雷管数据

    if(ERROR == SaveAllDetInfo(0))//保存雷管信息
    {
        ShowSDCardError();
    }
    else
    {                                                    
        ShowWaitLinCurrentReadyDlg(2);	        
    }
}
static void ScanBar(WM_HWIN hWin)
{
    TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)11);/*扫码*/
//    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[60];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Logging DETs");//雷管注册
			if(device_info.HasScan)
			{
				ShowInfoHelp("Press ↙ Modify the delaytime", "Press 5 scan and log");	//按↙键修改延期  			 按5键扫码注册				
			}	
			else
			{
				ShowInfoHelp("Press ↙ Modify the delaytime", NULL);//按↙键修改延期 				
			}
						
			GUI_SetFont(&SMALL_FONT);
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_TA_HCENTER|TEXT_CF_VCENTER);
		  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_TA_HCENTER|TEXT_CF_VCENTER);
	   
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls join a DET...");//请接入雷管...
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_TA_HCENTER|TEXT_CF_VCENTER);			
			sprintf(temp,"Delaytime:%dms", Op_Data.DelaySampleDelay);//雷管延期:
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), GUI_TA_HCENTER|TEXT_CF_VCENTER);		
			GetDetInfoString(&temp[0], Det.Type);			
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), temp);									

            if(device_info.HasScan && Op_Data.ScanIsZEBRA)
            {
                OPEN_SCAN();
            }
//            SetRS485Slave();//通过发送、接收状态控制外置LED灯亮灭，作为其他抽检设备的一把信号
            SetRS485Master();
            RS485TxPinExitUartConfig();

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
                    OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                    CLOSE_SCAN();
					ShowMainMenuDlg(0);
					break;
				case GUI_KEY_ENTER:			
					ShowDelaySampleProcessDelaySelDlg();
					break;
				case GUI_KEY_5: 
                    if(device_info.HasScan)
                    {
                        if(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
                        {
                            OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                        }
                        else
                        {
                            ScanBar(hWin);                             
                        }                       						
                    }			
					break;
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
                case GUI_TIMER_2MS_ID:                                  
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(WindowHasChild())
                    {
                        break;
                    }
                    MsgBoxInit();
                    if(0 == ScanData.Bytes)
                    {
                        SetMsgBoxCaption("Logging DETs");//雷管注册
                        SetMsgBoxMessage("Not getting the Dat!");//没有扫描到数据!
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("Scan");//扫码

                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ScanBar(hWin);
                        }
                        break;
                    }
                    
                    BarCodeDataAnalyst();			
                    if(BARCODE_NEW == BarCode.Status)
                    {
                        if(BarCode.Result == BARCODE_PASS)
                        {
                            if(ERROR == CheckOverlap(BarCode.DetSerl.U8Buffer))
                            {
                                sprintf((char*)Op_Data.Temp, "DET has been logged!\n\nHole No.:%d-%d", //雷管已注册!\n\n孔号:%d-%d
                                    Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);
                                SetMsgBoxMessage((char*)Op_Data.Temp);
                            }	
                            else
                            {
                                DET_INFO RegDetInfo;
                                
                                RegDetInfo.Serl.U32Value = BarCode.DetSerl.U32Value;
                                RegDetInfo.Pwd.U32Value = BarCode.DetPwd.U32Value;
                                RegDetInfo.Freq.U32Value = 0;
                                
                                SaveConfim(hWin, &RegDetInfo);
                                MsgBoxInit();                                
                            }
                        }
                        else if(BarCode.Result == BARCODE_DET_TYPE_ERR)
                        {
                            SetMsgBoxMessage("Chip TypeCode ERR!");//雷管类型错误!
                        }
                        else if(BarCode.Result == BARCODE_FACTORY_ERR)
                        {
                            SetMsgBoxMessage("Chip Factory Code ERR!");//工厂代码错误!
                        }                        
                        else
                        {
                            SetMsgBoxMessage("No meet Encoding ruler!");//雷管不符合要求!
                        }					
                    }
                    else
                    {                        
                        if(ScanData.Bytes > 0)
                        {
                            if(ScanData.Bytes >= SCAN_DATA_MAX_LEN)
                            {                            
                                ScanData.Bytes = SCAN_DATA_MAX_LEN -1;
                            }
                            ScanData.Buffer[ScanData.Bytes] = 0x00;
                            sprintf((char*)Op_Data.Temp, "Bar Code ERR!\n\n%s", (char*)&ScanData.Buffer[0]);//条码错误!
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                        }
                        else
                        {
                            SetMsgBoxMessage("Bar Code ERR!");//条码错误!
                        }                        
                    }
                    if(MsgBoxData.Message[0] != NULL)
                    {
                        SoundOutput(1);	
                        SetMsgBoxCaption("Logging DETs");//注册雷管
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("Scan");//扫码
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            ScanBar(hWin);
                        }
                    }
                    break;
                case GUI_TIMER_10MS_ID:
                    if(WindowHasChild())
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls join a DET...");//请接入雷管...
                    
                    MsgBoxInit();
                    switch((CHECK_RESULT)AppTaskResult.DetOperate)//雷管检测结果
                    {
                        case DET_PASS:			  //检测到雷管                           
                            SaveConfim(hWin, &Op_Data.RegDetInfo);
                            MsgBoxInit();
                            break;		
                        case TEST_ERR:			  //BIT错误 
                        case FREQ_MIN:			  //频率过小
                        case FREQ_MAX:			  //频率过大
                            SetMsgBoxMessage("Chip TypeCode ERR!");//雷管错误!
                            break;				
                        case OVERLAPED:			  //上线过程中这个序列号已经注册
                            sprintf((char*)Op_Data.Temp, "DET has been logged!\n\nHole No.:%d-%d",//雷管已注册!\n\n孔号:%d-%d 
                                Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            break;
                        case TYPE_ERR:			  //雷管类型错误
                            strcpy((char*)Op_Data.Temp, "Chip TypeCode ERR!\n\nDET Type:");//雷管类型错误!\n\n当前雷管类型:
                            GetDetTypeName((char*)&Op_Data.Temp[strlen((char*)Op_Data.Temp)] ,GetDetType(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value));
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            break;
                        case CURRENT_MIN:		  //电流小
                            SetMsgBoxMessage("DET Current Small!");//雷管电流小!
                            break;
                        case CURRENT_MAX:		  //电流大
                            SetMsgBoxMessage("DET Current Large!");//雷管电流大!
                            break;
                        case SECU_CODE_ERR:		  //密码校验错误
                            SetMsgBoxMessage("ChipPWD ERR!");//密码错误!
                            break;
                        case SERL_NULL_ERR:       //内码还没有写入                    
                        case DET_CODE_ERR:		  //内码不符合规则
                            SetMsgBoxMessage("Chip ID ERR!");//雷管内码错误!
                            break;
                        case DET_FAC_CODE_ERR:
                            SetMsgBoxMessage("Chip Factory Code ERR!");//工厂代码错误!
                            break;                            
                        default:
                            break;
                    }
                    if(MsgBoxData.Message[0] != NULL)
                    {
                        SetMsgBoxCaption("Logging DETs");//雷管注册
                        SetMsgBoxLeft("Back");//返回
                        ShowMsgBox();
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0); 
                    break;
				case GUI_TIMER_120MS_ID:
                    if(WindowHasChild())
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(XA3_DetCheckIn() == ERROR)
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }
                    if(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
                    {
                        OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                        CLOSE_SCAN();
                    }
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Detecting The DET...");//正在检测雷管...
                    WM_DeleteTimer(pMsg->Data.v);
                    //雷管操作任务
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)21);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);  				
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
					break;
				default:
					break;
			}		
			break;
		case WM_BACKGROUND_TASK_END:
			if(pMsg->Data.v != 2)
            {
                if(!(device_info.HasScan && Op_Data.ScanIsZEBRA))
                {
                    CLOSE_SCAN();
                }
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
            }
			break;             
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDelaySampleProcessWaitRegDetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
