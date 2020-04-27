#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, TOP_TITLE_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect, "", GUI_ID_BUTTON0,     1, 1, 10, 10,  0,          0 }
};
static u8 ChargeNumber = 0;
static void ShowBatteryState(u8 BatAccumulated, u8 ChargeFlag)
{
	//const u16 BatVoltage[5] = {3580, 3710, 3760, 3840, 3970};
	u8 i, Number = 0;

	//根据输入的电量转换成电量格数
	if(BatAccumulated > 75)
	{
		Number = 5;
	}
	else if(BatAccumulated > 60)
	{
		Number = 4;
	} 
	else if(BatAccumulated > 40)
	{
		Number = 3;
	}
	else if(BatAccumulated > 20)
	{
		Number = 2;
	}
	else if(BatAccumulated > 5)
	{
		Number = 1;
	}
	else
	{
		Number = 0;
	}
	
	if(Number > 1)
	{
		GUI_SetColor(GUI_GREEN); 
	}
	else
	{
		GUI_SetColor(GUI_RED); 
	}	
	
	GUI_FillRect(204,3,234,16);
	GUI_FillRect(205,4,233,15);
	GUI_FillRect(235,7,236,12);//电池前边的正极标志


	GUI_SetColor(GUI_BLACK); 
	GUI_FillRect(206,5,232,14);

	if(0x00 == ChargeFlag)
	{
		if(Number > 1)
		{
			GUI_SetColor(GUI_GREEN); 
		}
		else
		{
			GUI_SetColor(GUI_RED); 
		}	
	}
	else
	{//充电是为蓝色
		GUI_SetColor(GUI_BLUE); 
		
		if((ChargeNumber > 5) || (ChargeNumber < Number))
		{
			ChargeNumber = Number;
		}
		Number = ChargeNumber;
	}
	
	for(i=0;i<Number;i++)
	{
		GUI_FillRect(208+5*i,6,210+5*i,13);	
	}
}

void ShowWifiSignalStrength(u8 StateX, u8 Strength)
{
	u8 i;
	
	GUI_SetPenSize(1);
	GUI_AA_SetFactor(4);
	
	for(i=0; i<4; i++)
	{
		if(0xFF == Strength)
		{
			GUI_SetColor(GUI_BLUE);		
		}
		else
		{
			if(i < Strength)
			{
				GUI_SetColor(GUI_WHITE);					
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
			}		
		}

		GUI_AA_DrawArc(StateX+8,16,1+4*i,1+4*i,45,135);
	}
	GUI_SetPenSize(1);
}
void ShowBlueSignal(u8 StateX)
{
	GUI_SetPenSize(1);
	GUI_AA_SetFactor(4);
	
    if(BLUE_NOT_CONNECT == GET_BLUE_CONNECT_STATUS())
    {
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_SetColor(GUI_WHITE);
    }	
	
	GUI_DrawLine(StateX+5,3, StateX+5,17);
	
	GUI_DrawLine(StateX+1,14, StateX+9,7);
	GUI_DrawLine(StateX+5,3, StateX+9,7);
	
	GUI_DrawLine(StateX+1,6, StateX+9,13);
	GUI_DrawLine(StateX+5,17, StateX+9,13);
	
	GUI_SetPenSize(1);
}
void ShowGpsSignal(u8 StateX)
{
    const GUI_POINT aPoints[] = {{5,3}, {1,17}, {5,12}, {9,17}};
	
    if(GetLastGPSInfo(NULL))
    {
        GUI_SetColor(GUI_WHITE);
    }
    else
    {
        GUI_SetColor(GUI_BLACK);
    }	
	GUI_FillPolygon(aPoints, 4, StateX, 0);
}

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char buffer[50];
	u8 Result;
    u16 U16Value;
	static u8 BatAccumulated = 0;
	u8 RightIconNextX = 0;
	OS_STK_DATA StkData;
	static u8 ForcePowerOff = 0;
	static u8 BatAccumulatedAdjust = 0;
	static u16 OldLinABCurrent = 0xFFFF;
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_SetColor(GUI_WHITE); //设置前景颜色，及字体和绘图的颜色
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            
            RightIconNextX = 5;				
            
            if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
            {
                buffer[0] = device_info.name;
                buffer[1] = 0;
                GUI_SetColor(GUI_WHITE);

                GUI_DispStringAt(buffer,RightIconNextX,2);

                RightIconNextX += 10;
            }
            ShowBatteryState(BatAccumulated, (GET_CHARGE_SIGNAL_STATE() == CHARGING)?1:0);//显示电池电量
            
            if(GET_WIFI_POWER_STATE() == 0)
            {//WIFI电源已经打开
                if(GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP)
                {//已经连接成功
                    ShowWifiSignalStrength(RightIconNextX, 4);
                }
                else
                {
                    ShowWifiSignalStrength(RightIconNextX, 0);
                }
                RightIconNextX += 20;
            }
            else
            {
                ShowWifiSignalStrength(RightIconNextX,0xFF);
            }
            
            if(0 == GET_BLUE_POWER_STATUS())
            {
                ShowBlueSignal(RightIconNextX);
                RightIconNextX += 15;		
            }
//            if(0 == GET_GPS_POWER_STATE())
//            {
//                ShowGpsSignal(RightIconNextX);
//                RightIconNextX += 15;		
//            }
            
            GUI_SetColor(GUI_WHITE);
            RTC_Read_Time();
            sprintf(buffer, "%02d:%02d:%02d",RTC_Type.hour,RTC_Type.minute,RTC_Type.second);
            GUI_DispStringHCenterAt(buffer, 120,2);
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_HideWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_BUTTON0));

            BatAccumulated = LTC2942BatAccumulated();		
            
            WM_HideWindow(pMsg->hWin);//默认隐藏
            ChargeNumber = 0;
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            break;	
            //这个是接收键盘上的按键的相应
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_POWER:
                    ShowPowerOffDlg(0);
                    break;					
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_100MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
                    if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (DEVICE_TYPE_CHINESE_GENERAL == device_info.DeviceType))
                    {//在关机充电状态下，因为关了相关电源，WIFI不会处于连接状态
                        if ((0 == Op_Data.IdleUploadBlastRecordFlag) && (DEVICE_USAGES_SLAVE != device_info.DeviceUsages))
                        {//利用设备充电时上传起爆记录												
                            if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
                            {//设备正在充电
                                if(OSTimeGet() > (Op_Data.SystemIdleTime+1800000UL))
                                {//离上次按键至少30分钟，说明当前设置空闲
                                    if(OS_ERR_TASK_NOT_EXIST == OSTaskStkChk(APP_SAFE_MONITOR_TASK_PRIO, &StkData))
                                    {
                                        Op_Data.IdleUploadBlastRecordFlag = 1;
                                        //GPRS任务
                                        TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)3);
                                    }							
                                }	
                            }								
                        }							
                    }

                    if(ERROR == Lcd_PllLockState())
                    {//LCD芯片有可能已经复位
                        System168MDelay1ms(1);
                        if(ERROR == Lcd_PllLockState())
                        {
                            LcdSSD1963_ResetInit();
                            //隐藏->显示整个窗口后，GUI会重新刷整屏
                            WM_HideWindow(DlghWin.Dlg);
                            WM_HideWindow(DlghWin.Status);
                            WM_ShowWindow(DlghWin.Dlg);
                            WM_ShowWindow(DlghWin.Status);		
                        }
                    }	
                    if(Op_Data.ChargeCurrentVaryFlag != 0)
                    {//记录充电后的电流最大、最小值
                        if (GET_LINAB_SHORT_SW_CTRL_STATE() != 0)
                        {//总线没有短路
                            if(GET_HV_SW_CTRL_STATE() != 0)
                            {//还处于高压状态
                                //记录峰值
                                if(Op_Data.LinABCurrent < BlastInfo.I_AfterChargeMin)
                                {
                                    BlastInfo.I_AfterChargeMin = Op_Data.LinABCurrent;
                                }
                                if(Op_Data.LinABCurrent > BlastInfo.I_AfterChargeMax)
                                {
                                    BlastInfo.I_AfterChargeMax = Op_Data.LinABCurrent;
                                }					
                            }
                        }
                    }		
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    BatAccumulated = LTC2942BatAccumulated();
                    Op_Data.BatVoltage = LTC2942BatVoltage();
                    
                    
                    if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
                    {//正在充电
                        ChargeNumber++;//充电的时候如果还没充满，形成进度条形式

                        //电池容量修正
                        if((Op_Data.BatVoltage >= 4080) && (BatAccumulated <= 80))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 4080) && (BatAccumulated >= 90))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage >= 3980) && (BatAccumulated <= 75))//
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 3980) && (BatAccumulated >= 80))
                        {
                            BatAccumulatedAdjust++;				
                        }                     
                        else if((Op_Data.BatVoltage >= 3720) && (BatAccumulated <= 8))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 3720) && (BatAccumulated >= 12))
                        {
                            BatAccumulatedAdjust++;				
                        }
                        else if((Op_Data.BatVoltage <= 3640) && (BatAccumulated != 0))
                        {
                            BatAccumulatedAdjust++;							
                        }
                        else
                        {
                            BatAccumulatedAdjust = 0;
                        }					
                    }
                    else
                    {
                        ChargeNumber = 0;
                        //电池容量修正
                        if((Op_Data.BatVoltage >= 4030) && (BatAccumulated <= 80))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 4030) && (BatAccumulated >= 90))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage >= 3930) && (BatAccumulated <= 75))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 3930) && (BatAccumulated >= 80))
                        {
                            BatAccumulatedAdjust++;				
                        }                    
                        else if((Op_Data.BatVoltage >= 3650) && (BatAccumulated <= 7))
                        {
                            BatAccumulatedAdjust++;
                        }
                        else if((Op_Data.BatVoltage <= 3650) && (BatAccumulated >= 13))
                        {
                            BatAccumulatedAdjust++;				
                        }
                        else if((Op_Data.BatVoltage <= 3570) && (BatAccumulated != 0))
                        {
                            BatAccumulatedAdjust++;											
                        }
                        else
                        {
                            BatAccumulatedAdjust = 0;
                        }					
                    }
                    if(BatAccumulatedAdjust >= 10)
                    {
                        BatAccumulatedAdjust = 0;
                        LTC2942BatAccumulatedAdjust();
                        BatAccumulated = LTC2942BatAccumulated();						
                    }
                    WM_InvalidateWindow(pMsg->hWin);
                    Result = BusStateMonitor(Op_Data.LinABVoltage, Op_Data.LinABCurrent);
                    if(Result != 0)
                    {
                        Op_Data.BusMonitorDisable = 1;
                        if(Op_Data.BusMonitorDisableShow)
                        {
                            if(1 == Result)
                            {
                                sprintf(buffer, "Bus Short Circuit!\n\nV:%.1fV I:%duA", //总线短路!\n\n电压:%.1fV 电流:%duA
                                    Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                            }
                            else
                            {
                                sprintf(buffer, "Bus V Abnormal!\n\nV:%.1fV I:%duA",//总线电压异常!\n\n电压:%.1fV 电流:%duA 
                                    Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);			
                            }                  
                            UTF8StringToGBK(buffer, (char*)Op_Data.Temp);
                            WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), NEW_STR_INFO);	                        
                        }
                        else
                        {
                            if(Op_Data.BusMonitorCfg == 1)
                            {
                                ShowBusErrorRestoreDlg();
                            }
                            else
                            {                 
                                if(1 == Result)
                                {
                                    ShowWaitPowerOffDlg(0);
                                }
                                else
                                {
                                    ShowWaitPowerOffDlg(1);
                                }			
                            }
                            return;
                        }
                    }		
                    if((Op_Data.OfflineMonitorBlastProcess != 0) && (Op_Data.BlastStatus != BLAST_ERROR))
                    {
                        if(Op_Data.LinABCurrent/GetDetOnlineCount() <= 10)
                        {
                            if(Op_Data.OfflineMonitorBlastProcess < 5)
                            {
                                Op_Data.OfflineMonitorBlastProcess++;
                            }
                            else
                            {//连续2秒平均电流小于10uA
                                Op_Data.OfflineMonitorBlastProcess = 0;
                                ShowWaitPowerOffDlg(2);
                                return;
                            }
                        }
                        else
                        {
                            Op_Data.OfflineMonitorBlastProcess = 1;
                        }
                    }
                    if(WM_IsVisible(pMsg->hWin))
                    {					
                        if(Op_Data.ForcePowerOffSwitch)
                        {
                            if((BatAccumulated <= 2) && (ForcePowerOff <= 20))
                            {//电池电量太低时强制关机
                                if(LTC2942Init() != ERROR)
                                {//确认芯片没问题
                                    ForcePowerOff++;
                                    if(ForcePowerOff > 20)
                                    {
                                        ShowPowerOffDlg(1);							
                                    }							
                                }
                            }					
                        }
                        else
                        {
                            ForcePowerOff = 0;
                        }
                    }
                    //记录总线电流变化，用于分析内码变是否与上下电有关系
                    U16Value = Op_Data.LinABCurrent;   
                                    
                    if ((0 != GET_LINAB_SHORT_SW_CTRL_STATE()) && Op_Data.BusCurrentOffAutoSave)
                    {//当前总线电压没有关闭
                        if(OldLinABCurrent != 0xFFFF)
                        {
                            u8 limit = 10;
                            if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
                            {
                                limit = 20;//插着充电器时，设备静态电流会变大10uA左右
                            }
                            
                            if((U16Value < limit) && (OldLinABCurrent > limit))
                            {                      
                                UTF8StringToGBK("总线没电流", (char*)Op_Data.Temp);
                                WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), CURRENT_STATUS);	                        
                            }
                            else if((U16Value > limit) && (OldLinABCurrent < limit))
                            {                      
                                UTF8StringToGBK("总线有电流", (char*)Op_Data.Temp);
                                WriteSystemLog(Op_Data.Temp, strlen((const char*)Op_Data.Temp), CURRENT_STATUS);	                                                
                            }                    
                        }
                    }            
                    
                    OldLinABCurrent = U16Value;
                    break;
                default:
                    break;
            }		
            break;
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowTopTitleDlg(void)
{
	if(WM_IsWindow(DlghWin.Status))
	{
		GUI_EndDialog(DlghWin.Status, 0);
	}	
	DlghWin.Status = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
