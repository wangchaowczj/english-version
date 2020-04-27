#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  20,  0,          0 },//20, 100, 200,  20,  0,          0
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     20, 120, 220,  20,  0,          0 }
};

static void ScanBar(WM_HWIN hWin)
{
    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
	  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
	  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Scanning...");//扫描中...
    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
    TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)11);/*扫码*/
}
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 扫码写入
  *         @arg @b 1 通过手动输入管码方式进行内码修改
  *         @arg @b 2 利用备份数据恢复内码
  *         @arg @b 3 组网检测时，通过错误列表重写内码  
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Rewrite Chip ID");//内码重写
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:	
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{
                        OSTaskDelReq(APP_BACKGROUND_TASK_PRIO);
                        CLOSE_SCAN();
                        if((GetWindowParam()&0xFFFF) == 3)
                        {//组网检测时，通过错误列表重写内码
                            ShowNetTestErrorListViewDlg(GetWindowParam()>>16);
                        }
                        else
                        {
                            ShowDetCodeWriteMenuDlg(GetWindowParam()>>16);                        
                        }
					}										
					break;						
			}
			break;
		case WM_UPDATE_PROGBAR:
			WM_Paint(DlghWin.Status);
			
			sprintf((char*)Op_Data.Temp, "Completed %.1f%%", (0xFFFF-pMsg->Data.v)/655.36);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp);
		
			WM_Paint(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
			break;			
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                           
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);                    
                    
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }			
                    if((GetWindowParam()&0xFFFF) == 0)
                    {//扫码写入                       
                        ScanBar(hWin);
                    }
                    else
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls join a DET...");//请接入1发雷管...
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
                    }
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);			
                                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);                    
                    break;
                case GUI_TIMER_2MS_ID:                                                
                    WM_DeleteTimer(pMsg->Data.v);

                    if(WindowHasChild())
                    {
                        break;
                    }
                    
                    MsgBoxInit();
                    if(0 == ScanData.Bytes)
                    {                        
                        SetMsgBoxCaption("Rewrite Chip ID");//内码重写
                        SetMsgBoxMessage("Not getting the Dat!");//没有扫描到数据!
                        SetMsgBoxLeft("Cancel");
                        SetMsgBoxRight("Scan");//扫码
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_LEFT:
                                ShowDetCodeWriteMenuDlg(GetWindowParam()>>16);
                                break;
                            case GUI_KEY_RIGHT:
                                ScanBar(hWin);
                                break;
                        }
                        break;
                    }
                    
                    BarCodeDataAnalyst();			
                    if(BARCODE_NEW == BarCode.Status)
                    {
                        if(BarCode.Result == BARCODE_PASS)
                        {
                            SoundOutput(0);
                            
                            Op_Data.RegDetInfo.Serl.U32Value = BarCode.DetSerl.U32Value;
                            Op_Data.RegDetInfo.Pwd.U32Value = BarCode.DetPwd.U32Value;

                            Op_Data.RegDetInfo.Freq.U32Value = 0;
                            
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls join a DET...");//请接入1发雷管...
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
                            
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
                        SetMsgBoxCaption("Rewrite Chip ID");//内码重写
                        SetMsgBoxLeft("Cancel");//取消
                        SetMsgBoxRight("Scan");//扫码 
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_LEFT:
                                ShowDetCodeWriteMenuDlg(GetWindowParam()>>16);
                                break;
                            case GUI_KEY_RIGHT:
                                ScanBar(hWin);
                                break;
                        }
                    }
                    break;
                case GUI_TIMER_10MS_ID:
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    WM_DeleteTimer(pMsg->Data.v);
                    switch((GetWindowParam()&0xFFFF))
                    {
                        case 0://扫码写入
                            if(AppTaskResult.DetOperate == 0)
                            {
                                SoundOutput(1);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Communication Abnormal");//通信异常
                            }
                            else
                            {
                                SoundOutput(0);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Scanning Input Completed");//扫码写入成功
                                memset(temp, 0, sizeof(temp));
                                if(ERROR == XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)temp))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DET ID:unknown");//管码:未知//作用是显示管码
                                }
                                else
                                {
                                    sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码:
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);//显示管码
                                }
                            }
                            break;
                        case 1://通过手动输入管码方式进行内码修改
                        case 3://组网检测时，通过错误列表重写内码 
                            if(AppTaskResult.DetOperate == 0)
                            {
                                SoundOutput(1);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Communication Abnormal");//通信异常
                            }
                            else
                            {
                                SoundOutput(0);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip ID modify completed");//内码修改成功
                                memset(temp, 0, sizeof(temp));
                                if(ERROR == XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)temp))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DET ID:unknown");//管码:未知//作用是显示管码
                                }
                                else
                                {
                                    sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);//显示管码
                                }
                            }
                            break;
                        case 2://利用备份数据恢复内码
                            if(AppTaskResult.DetOperate == 1)
                            {
                                SoundOutput(0);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip ID recovery completed");//内码恢复成功
                                memset(temp, 0, sizeof(temp));
                                if(ERROR == XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)temp))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DET ID:unknown");//管码:未知//作用是显示管码
                                }
                                else
                                {
                                    sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);//显示管码
                                }
                                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
                            }
                            else if(AppTaskResult.DetOperate == 2)
                            {
                                SoundOutput(1);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't recover Chip ID"); //无法恢复内码
                                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
                            }
                            else if(AppTaskResult.DetOperate == 3)
                            {
                                SoundOutput(1);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Not Need Recovery");//不需要恢复
                                memset(temp, 0, sizeof(temp));
                                if(ERROR == XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)temp))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DET ID:unknown");//管码:未知//作用是显示管码
                                }
                                else
                                {
                                    sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);//显示管码
                                }
                                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
                            }
                            else
                            {
                                SoundOutput(1);
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Communication Abnormal");//通信异常"
                            }
                            break;
                    }
                   
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));                    
                    break;
                case GUI_TIMER_100MS_ID:
                    if(XA3_DetCheckIn() == ERROR)
					{//雷管已经下线
						WM_DeleteTimer(pMsg->Data.v);
                        
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls connect a DET...");//请接入1发雷管...
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
					}
                    else
                    {//接有雷管,雷管还没下线 
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
                    }
                    break;
                case GUI_TIMER_120MS_ID:                    
                    if(XA3_DetCheckIn() == ERROR)
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);	
                        break;
					}
                                         
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Rewriting the Dat...");//正在重写数据...
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                    
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                    switch(GetWindowParam()&0xFFFF)
                    {
                        case 2:
                            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)26); 
                            break;
                        default:
                            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)25); 
                            break;
                    }                     

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

void ShowDetCodeWriteStatusDlg(u16 ParentParam, u16 DlgParam)
{
    u32 Param;
    
    Param = (ParentParam<<16) + DlgParam;
    
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
