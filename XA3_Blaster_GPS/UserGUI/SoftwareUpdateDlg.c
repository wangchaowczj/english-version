#include "includes.h"
#include "GuiIncludes.h"


/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Cancel",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "Initializing WIFI...", GUI_ID_TEXT0,     0, 110, 240,  20,  0,          0 },//无线网络初始化中...   20, 110, 220,  20,  0,          0 
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT1,     0, 135, 240,  20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	OS_STK_DATA StkData;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            
            ShowInfoTitle("Firmware Update");//固件升级
            GUI_SetFont(&SMALL_FONT);		

            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
                sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);	//注册雷管:%u 错误雷管:				
            }
            else
            {
                sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                GetDetInfoString(&temp[40], Det.Type);							
            }
            ShowInfoHelp(&temp[0], &temp[40]);
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://取消固件升级
                    //下载固件过程中需要申请资源，必需保证资源处于释放状态才能删除任务
                    OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
                    FileSystemLock();
                    WifiLock();
                    
                    OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
                    
                    WifiUnlock();
                    FileSystemUnlock();
                    
                    ExitSafeMonitor();
                    ShowHelpToolMenuDlg();
                    
                    break;			
            }
            break;
        case WM_SOFTWARE_UPDATE://固件更新
            switch(pMsg->Data.v)
            {
                case 0:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Initializing WIFI...");//无线网络初始化中...
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"");
                    break;		
                case 1:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Checking the update...");//正在检查更新...
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1)," ");
                    break;
                case 2://保存从服务器中获取到的数据
                    if(HexLoader.NewFileSize != 0)
                    {
                        sprintf(temp, "Ver:%s", &HexLoader.NewVersion[0]);//版本
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
                        if(HexLoader.CurrentFileSize >= HexLoader.NewFileSize)
                        {						
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Firmware Downloaded!");//固件已下载完成!
                        }
                        else
                        {
                            sprintf((char*)temp, "Download %.2f%%", (double)HexLoader.CurrentFileSize/HexLoader.NewFileSize*100);//固件已下载
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);
                        }
                    }
                    break;
                case 3://计算文件校验值
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Validating the firmware...");//固件数据验证中				
                    break;
                default:
                    break;
            }
            break;
        case WM_SAFE_MONITOR_TASK_END:
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "Back");
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), " ");
            switch(pMsg->Data.v)
            {
                case 0:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"WIFI Abnormal!");//无线网络异常!
                    break;
                case 1:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Download completed,restart!");//固件下载成功,请重启设备!
                    if(device_info.DeviceUsages == DEVICE_USAGES_SLAVE)
                    {
                        CLOSE_WIFI_POWER();				
                    }
                    break;
                case 2:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Not needing the update!");//固件不需要升级!							
                    break;	
                case 3:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Download ERR!");//下载错误,请重新更新!
								    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"Pls re-update!");//下载错误,请重新更新!
                    break;
                case 4:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"Firmware Save Failed!");//固件保存失败,请重新开机!
								    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"Pls restart!");//固件保存失败,请重新开机!
                    break;				
                case 0xFF:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"WIFI initialization failed!");	//无线网络初始化失败!	                  									
                    break;
                default:
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"WIFI Abnormal!");//无线网络异常!
                    break;
            }	
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);		
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    
                    if ((GET_WIFI_POWER_STATE() != 0) || (GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP))
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Firmware Update");//固件升级
                        SetMsgBoxMessage("Pls Connect To WIFI!");//请连接WIFI网络!
                        SetMsgBoxLeft("Back");

                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowHelpToolMenuDlg();
                        }
                        break;		
                    } 
                    else if(OS_ERR_TASK_NOT_EXIST != OSTaskStkChk(APP_SAFE_MONITOR_TASK_PRIO, &StkData))
                    {
                        OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
                        FileSystemLock();
                        WifiLock();
                        
                        OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
                        
                        WifiUnlock();
                        FileSystemUnlock();
                    }
                    OSTaskDel(APP_WIFI_TASK_PRIO);
                    
                    TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)2);/*固件更新*/		

                    
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }				
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
                    break;
                default:
                    break;
            }		
            break;		
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowSoftwareUpdateDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

