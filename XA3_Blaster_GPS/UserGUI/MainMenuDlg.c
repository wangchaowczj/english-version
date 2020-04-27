#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Blast",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }, //起爆 
  { BUTTON_CreateIndirect,   "Detail",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//详情
};

void DeskCallback(WM_MESSAGE * pMsg)
{
//	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
//	    GUI_SetColor(GUI_WHITE); //设置前景颜色，及字体和绘图的颜色
//		GUI_SetBkColor(COLOR_DLG_BK);
		GUI_Clear();
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 直接显示界面内容
  *         @arg @b 1 在显示界面内容前先对网路发送复位指令   
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
            ShowInfoTitle("Main Menu");//主菜单

            GUI_SetFont(&NORMAL_FONT);
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                GUI_DispStringHCenterAt((const char*)"1.Net Test", 120,60);//1.网络测试
                GUI_DispStringHCenterAt((const char*)"2.Info MGT", 120,95);//2.信息管理
                GUI_DispStringHCenterAt((const char*)"3.Sys Set ", 120,130);//3.系统设置
                GUI_DispStringHCenterAt((const char*)"4.Tool    ", 120,165);//4.辅助工具
                sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器
                sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:	错误雷管:
                ShowInfoHelp(&temp[0], &temp[40]);
            }
            else
            {                
                if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Op_Data.DelaySampleCfg))
                {
                    GUI_DispStringHCenterAt((const char*)"0.Sampling INSP", 120,40);//0.雷管抽检
                    GUI_DispStringHCenterAt((const char*)"1.Logging      ", 120,70);//1.雷管注册
                    GUI_DispStringHCenterAt((const char*)"2.Fast Test    ", 120,100);//2.组网测试
                    GUI_DispStringHCenterAt((const char*)"3.DLY Set      ", 120,130);//3.延期设置
                    GUI_DispStringHCenterAt((const char*)"4.Info MGT     ", 120,160);//4.信息管理
                    GUI_DispStringHCenterAt((const char*)"5.More...      ", 120,190);//5.更多功能  				
                }
                else
                {
                    GUI_DispStringHCenterAt((const char*)"1.Logging  ", 120,45);//1.雷管注册
                    GUI_DispStringHCenterAt((const char*)"2.Fast Test", 120,80);//2.组网测试
                    GUI_DispStringHCenterAt((const char*)"3.DLT Set  ", 120,115);//3.延期设置
                    GUI_DispStringHCenterAt((const char*)"4.Info MGT ", 120,150);//4.信息管理
                    GUI_DispStringHCenterAt((const char*)"5.More...  ", 120,185);//5.更多功能                
                }
                sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管: 错误雷管:
                GetDetInfoString(&temp[40], Det.Type);	
                ShowInfoHelp(&temp[0], &temp[40]);			
            }
            break;
        case WM_INIT_DIALOG://初始化界面
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }
            else
            {
                WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);            
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    if(DEVICE_USAGES_MASTER != device_info.DeviceUsages)
                    {
                        if(!AllDetFactoryCodeIsRulePass())
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Blast Progress");//起爆流程
                            SetMsgBoxMessage("Part of DETs have ERR\n\nChip Factory Codes!");//有部分雷管工厂代码错误!
                            SetMsgBoxMessageColor(GUI_RED);
                            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;
                        }                    
                    }

                    MsgBoxInit();
                    SetMsgBoxCaption("Blast Progress");//起爆流程
                    SetMsgBoxMessage("Pls test in safety area!");//请在安全区域测试!
                    SetMsgBoxMessageColor(GUI_RED);
                    SetMsgBoxLeft("Back");//返回
                    SetMsgBoxRight("OK");//确定
                    if(ShowMsgBox() != GUI_KEY_RIGHT)
                    {
                        return;
                    }
                    Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_H_GOAL;
                    if(DEVICE_USAGES_MASTER != device_info.DeviceUsages)
                    {
                        if(CheckDetDelayIsSame() != 0xFFFF)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Blast Progress");//起爆流程
                            SetMsgBoxMessage("All DETs have same DLY!");//所有雷管延期都相同!
                            SetMsgBoxLeft("Back");//返回
                            SetMsgBoxRight("OK");//确定

                            if(ShowMsgBox() != GUI_KEY_RIGHT)
                            {
                                return;
                            }
                        }
                    }
                    if ((DEVICE_USAGES_SLAVE != device_info.DeviceUsages) &&
                        (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType))
                    {//丹灵离线起爆模式,独立起爆方式时很判断是否授权
                        if(!HasAccreditInfoFileForDL())
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Blast Progress");//起爆流程
                            SetMsgBoxMessage("Not downloading \n\n authorization files!");//没有下载授权文件!
                            SetMsgBoxMessageColor(GUI_RED);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;                         
                        }
//                        if(GPSIsWorking())
//                        {
//                            ReceiveGPSInfoStop();
//                        }
//                        if ((ReadOldGPSInfo(&Op_Data.Gps) == ERROR) || (GPSInfoTimeIsPass(&Op_Data.Gps, &RTC_Type) == 0))
//                        {
//                            MsgBoxInit();
//                            SetMsgBoxCaption("起爆流程");
//                            SetMsgBoxMessage("没有定位数据!");
//                            SetMsgBoxMessageColor(GUI_RED);
//                            SetMsgBoxLeft("返回");
//                            ShowMsgBox();
//                            return; 
//                        }
//                        if(!AccreditAreaVerifyForDL())
//                        {
//                            MsgBoxInit();
//                            SetMsgBoxCaption("起爆流程");
//                            SetMsgBoxMessage("不在准爆区域!");
//                            SetMsgBoxMessageColor(GUI_RED);
//                            SetMsgBoxLeft("返回");
//                            ShowMsgBox();
//                            return;
//                        }
                    }
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        if(DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType)
                        {
                            Op_Data.BlueSlaveStatus = BLAST_INFO_VERIFY;
                            ShowBlastInfoVerifyDlg();                        
                        }
                        else
                        {
                            ShowBlueHostStatusDlg(0);//搜索蓝牙密钥
                        }
                        return;
                    }
                    if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
                    {
                        if(BlastSingleSelfTest(1) == ERROR)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Device Abnormal");//设备异常
                            SetMsgBoxMessage("Circuit Abnormal!");//起爆信号发送电路异常!
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox(); 
                            return;
                        }
                    }

                    ShowWaitLinCurrentReadyDlg(3);										
                    break;
                case GUI_KEY_RIGHT:
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        if(GetSlaveStateCount(SLAVE_VOLATILE) == 0)
                        {
                            SetSlaveInfoListViewDlgListViewSelIndex(0);
                            ShowSlaveInfoListViewDlg(0);					
                        }
                        else
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Decoder Info");//译码器信息
                            SetMsgBoxMessage("No Decoder Info!");//没有译码器信息!
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();                     
                        }
                    }
                    else
                    {
                        if(Det.RegCount > 0)
                        {
                            UTF8StringToGBK("进入延期设置界面", (char*)Op_Data.Temp);
                            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                            
                            SetNetInfoDlgListViewSelIndex(0);//从第1发开始显示
                            ShowNetInfoDlg(0);					
                        }
                        else
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("DET Info");//雷管信息
                            SetMsgBoxMessage("No Logged DETs!");//没有注册雷管!
                            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();				
                        }				
                    }
                    break;
                case GUI_KEY_1:
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        SetSlaveInfoListViewDlgListViewSelIndex(0);
                        ShowSlaveInfoListViewDlg(2);
                    }
                    else
                    {
                        if ((DEVICE_USAGES_ALONE == device_info.DeviceUsages) &&
                            (DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType))
                        {
                            if(!HasAccreditInfoFileForDL())
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Logging DETs");//雷管注册
                                SetMsgBoxMessage("Not downloading \n\n authorization files!");//没有下载授权文件!
                                SetMsgBoxMessageColor(GUI_RED);
                                SetMsgBoxLeft("Back");//返回
                                ShowMsgBox();
                                return;
                            }                        
                        }

                        UTF8StringToGBK("进入注册功能", (char*)Op_Data.Temp);
                        WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                        
                        Op_Data.BusMonitorCfg = 1;//提示异常，但可以自动恢复
                        Op_Data.BusCurrentOffAutoSave = 0;
                        if(Op_Data.LinABCurrent < 15)
                        {
                            SetWaitRegListViewDlgSelIndex(RegHoleInit());//初始化待注册孔
                            
                            if(HasDivideRegHole())
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Logging DETs");//雷管注册
                                SetMsgBoxMessage("Part of DETs registration \n\naren't in order!");//有部分雷管跳孔注册!
                                SetMsgBoxMessageColor(GUI_RED);
                                sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("LOC");//定位
                                SetMsgBoxRight("CONT");//继续
                                switch(ShowMsgBox())
                                {
                                    case GUI_KEY_LEFT:
                                        {
                                            u16 i;
                                            for(i=0;i<DET_MAX_SIZE;i++)
                                            {
                                                if(GetDetRegStatus(i) == ERROR)
                                                {
                                                    SetWaitRegListViewDlgSelIndex(i);
                                                    break;
                                                }
                                            }                                      
                                        }
                                        ShowWaitRegListViewDlg();
                                        break;
                                    case GUI_KEY_RIGHT:
                                        ShowWaitRegListViewDlg();
                                        break;                                        
                                }
                            }
                            else
                            {
                                ShowWaitRegListViewDlg();
                            }
                        }
                        else
                        {
                            ShowProgbarMaskRegDetDlg();
                        }				
                    }
                    break;
                case GUI_KEY_2:
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        ShowInfoManageDlg();
                    }
                    else
                    {
                        if(!AllDetFactoryCodeIsRulePass())
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Fast Test");//组网测试
                            SetMsgBoxMessage("Part of DETs have ERR\n\nChip Factory Codes!");//有部分雷管工厂代码错误!
                            SetMsgBoxMessageColor(GUI_RED);
                            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();
                            return;
                        }
                        
                        UTF8StringToGBK("进入组网测试", (char*)Op_Data.Temp);
                        WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);	
                        
                        ShowWaitLinCurrentReadyDlg(0);					
                    }				
                    break;	
                case GUI_KEY_3:
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        ShowSystemSetMenuDlg();
                    }
                    else
                    {
                        if(Det.RegCount > 0)
                        {
                            UTF8StringToGBK("进入延期设置界面", (char*)Op_Data.Temp);
                            WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
                            
                            SetNetInfoDlgListViewSelIndex(0);//从第1发开始显示
                            ShowNetInfoDlg(1);//启用右功能键和确认键修改延期					
                        }
                        else
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("DET Info");//雷管信息
                            SetMsgBoxMessage("No Logged DETs!");//没有注册雷管!
                            SetMsgBoxMessageColor(GUI_RED);
                            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            ShowMsgBox();					
                        }				
                    }			
                    break;	
                case GUI_KEY_4:
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        ShowHelpToolMenuDlg();
                    }
                    else
                    {
                        ShowInfoManageDlg();	
                    }			
                    break;	
                case GUI_KEY_5:
                    if(DEVICE_USAGES_MASTER != device_info.DeviceUsages)
                    {
                        ShowMainMenuMoreDlg();			
                    }			
                    break;              
                case GUI_KEY_0:
                    if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Op_Data.DelaySampleCfg))
                    {
                        if(Det.RegCount > 0)
                        {
                            MsgBoxInit();
                            SetMsgBoxCaption("Sampling INSP");//雷管抽检
                            SetMsgBoxMessage("This operation will del \n\n all logged DETs!");//将删除所有已注册雷管!
                            SetMsgBoxMessageColor(GUI_RED);
                            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            SetMsgBoxRight("OK");//确定
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                DelaySampleProcessInit();
                            }						
                        }
                        else
                        {
                            DelaySampleProcessInit();
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
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && Op_Data.DelaySampleCfg && Op_Data.DelaySampleFlag)
                    {
                        RS485Config();//抽检时会修改发送引脚的配置
                    }
                    Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_L_GOAL;
                    SetNormalHighVoltageGoal();		
                    Op_Data.ForcePowerOffSwitch = 1;//允许电池电量低时自动关机
                    Op_Data.ChargeCurrentVaryFlag = 0;//不记录总线峰值变化
                    Op_Data.DoubleKeySwitch = 0;//不允许按下双键
                    Op_Data.DelaySampleFlag = 0;
                    Op_Data.BusMonitorCfg = 0;
                    Op_Data.BusMonitorDisableShow = 0;
                    Op_Data.WaitDetVerify = 0;
                    Op_Data.MainMenuNow = 1;
                    Op_Data.OfflineMonitorBlastProcess = 0;
                    OpenDetCmdDelayInterval();
                    CLOSE_SCAN();
                    
                    OPEN_DET_RECEIVE_RES();//取消雷管检测任务时有用
                    
//                    if(!GetLastGPSInfo(NULL))
//                    {//还没有定位成功过
//                        if(!GPSIsWorking())
//                        {
//                            ReceiveGPSInfoStart();
//                        }                    
//                    }

                    if(!TaskIsExist(APP_WIFI_TASK_PRIO))
                    {
                        TaskCreate(APP_WIFI_TASK_PRIO, (void*)3);
                    }
                    
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        SetRS485Master();
                        CLOSE_BUS_VOLTAGE();
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                        Op_Data.BusCurrentOffAutoSave = 0;
                    }
                    else
                    {                       
                        SetRS485Slave();
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                        OPEN_BUS_6_7V();
                        Op_Data.BusCurrentOffAutoSave = 1;
                    }
                    Op_Data.BlastVerifyResult = 0;
                    Op_Data.BlastStatus = BLAST_DISABLE;
                    
                    CLOSE_HV_ENABLE();
                    OPEN_BLUE_POWER();
                    if(GetBlueStatus() == BLUE_MASTER)
                    {
                        TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)0);
                    }
                    AllRegDetRankBaseOnHoleInfo();
                    
                    if (GetWindowParam() == 1)
                    {
                        AllDisCharge();
                    }                   
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

                    UTF8StringToGBK("进入主界面", (char*)Op_Data.Temp);
                    WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);                    
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    WM_InvalidateWindow(hWin);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    
                    if(!MsgBoxIsShow())
                    {
                        if(Op_Data.AutoUpdateStatus == 2)
                        {
                            Op_Data.AutoUpdateStatus = 0xFF;
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Firmware Update");//固件升级       
                            if(HexLoader.NewVersion[0] != 0)
                            {
                                sprintf((char*)Op_Data.Temp, "Find New Firmware,update?\n\n%s", HexLoader.NewVersion);//发现新固件, 是否升级?
                                SetMsgBoxMessage((char*)Op_Data.Temp);
                            }
                            else
                            {
                                SetMsgBoxMessage("Find New Firmware,update?");//发现新固定, 是否升级?
                            }
                            SetMsgBoxLeft("Cancel");//取消
                            SetMsgBoxRight("Update");//升级
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
                            {
                                ShowSoftwareUpdateDlg();
                            }					
                        }                    
                    }				
                    break;
                default:
                    break;
            }		
            break;	
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowMainMenuDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}

