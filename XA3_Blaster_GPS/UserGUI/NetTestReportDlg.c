#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Detail", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 快速测试
  *         @arg @b 1 完全测试 
  *         @arg @b 2 进入起爆流程前的快速测试 
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
	u8 i;
	u16 DlgParam;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
            DlgParam = GetWindowParam();
            switch(DlgParam)
            {
                case 0:
                    ShowInfoTitle("Fast Test");//组网测试
                    break;
                case 1:
                    ShowInfoTitle("Network Test");//网路测试
                    break;
                case 2:
                    ShowInfoTitle("Blast Progress");//起爆流程
                    break;                    
            }
            if(DlgParam == 2)
            {
                sprintf(&temp[0], "Online DET:%d", GetDetOnlineCount());//在线雷管:
                ShowInfoHelp(&temp[0], " ↙ Continue"); //↙键继续               
            }
            else
            {
                sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                sprintf(&temp[40], "Online DET:%d", GetDetOnlineCount());//在线雷管:
                ShowInfoHelp(&temp[0], &temp[40]);            
            }
			
			GUI_SetFont(&SMALL_FONT);
						
			if (Det.ErrorCount == 0)
			{
				GUI_DispStringAt("Log DET Sum:",30,70);//注册雷管数:
				GUI_DispDecSpace(Det.RegCount,0);
				GUI_DispStringAt("ERR DET Sum:",30,90);//错误雷管数:
				GUI_DispDecSpace(Det.ErrorCount,0);                
				GUI_DispStringAt("BusCurrent(AV):",30,110);//总线平均电流:
				GUI_DispDecSpace(BlastInfo.I_BeforeTest,0);
				GUI_DispString("uA");
				i=0;
				if (Det.RegCount != 0)
				{
					GUI_DispStringAt("DETCurrent(AV):",30,130);//单发平均电流:
					GUI_DispDecSpace(BlastInfo.I_BeforeTest/Det.RegCount,0);
					GUI_DispString("uA");	
					i++;
				}
				if (((DlgParam == 0) ||(DlgParam == 2))&&(BlastInfo.I_Leak > 500))
				{//只有快速测试时检测总线漏电流
					GUI_SetColor(GUI_RED);
					GUI_DispStringAt("Leakage I Large!",30,130+20*i);//总线漏电流大!
					GUI_SetColor(device_info.TextColor);
					i++;
				}
				if(((DlgParam == 0) ||(DlgParam == 2)) && ((Op_Data.DetTestResult&0x01) == 0x01))
				{//只有快速测试时检测注册雷管
					GUI_SetColor(GUI_RED);
					GUI_DispStringAt("Have Unlogged DETs!",30,130+20*i);//有未注册雷管!
                    i++;
                    if(Op_Data.FindUnRegDetInfo)
                    {
                        memset(temp, 0,sizeof(temp));
                        if(ERROR == XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, Op_Data.UnRegDetInfo.Pwd.U32Value, (u8*)temp))
                        {
                            sprintf((char*)Op_Data.Temp, "Chip ID:%02X%02X%02X%02X%02X%02X%02X%02X",//内码: 
                                Op_Data.UnRegDetInfo.Serl.U8Buffer[0], Op_Data.UnRegDetInfo.Serl.U8Buffer[1], 
                                Op_Data.UnRegDetInfo.Serl.U8Buffer[2], Op_Data.UnRegDetInfo.Serl.U8Buffer[3], 
                                Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], Op_Data.UnRegDetInfo.Pwd.U8Buffer[1], 
                                Op_Data.UnRegDetInfo.Pwd.U8Buffer[2], Op_Data.UnRegDetInfo.Pwd.U8Buffer[3]);                            
                        }
                        else
                        {
                            sprintf((char*)Op_Data.Temp, "Chip ID:%s", temp); //内码:
                        }

                        GUI_DispStringAt((char*)Op_Data.Temp,45,130+20*i);
                        i++;                        
                    }                     
					GUI_SetColor(device_info.TextColor);					
				}
			} 
			else
			{
				GUI_DispStringAt("Log DET Sum:",30,40);//注册雷管数:
				GUI_DispDecSpace(Det.RegCount,0);
                GUI_SetColor(GUI_RED);
				GUI_DispStringAt("ERR DET Sum:",30,60);//错误雷管数:
				GUI_DispDecSpace(Det.ErrorCount,0);
                GUI_SetColor(device_info.TextColor);
				GUI_DispStringAt("BusCurrent(AV):",30,80);//总线平均电流:
				GUI_DispDecSpace(BlastInfo.I_BeforeTest,0);
				GUI_DispString("uA");
				GUI_DispStringAt("DETCurrent(AV):",30,100);//单发平均电流:
				GUI_GotoXY(30,120);
				GUI_DispDecSpace(BlastInfo.I_BeforeTest/Det.RegCount,0);
				GUI_DispString("uA(Include ERR DETs)");//含错误雷管
				i=0;
				if((Det.RegCount-Det.ErrorCount) != 0)
				{
					GUI_GotoXY(30,140);
					GUI_DispDecSpace(BlastInfo.I_BeforeTest/(Det.RegCount-Det.ErrorCount),0);
					GUI_DispString("uA(Uninclude ERR DETs)");	//不含错误雷管
					i++;
				}
				if (((DlgParam == 0) ||(DlgParam == 2))&&(BlastInfo.I_Leak > 500))
				{//只有快速测试时检测总线漏电流
					GUI_SetColor(GUI_RED);
					GUI_DispStringAt("Leakage Current Large!",30,140+20*i);//总线漏电流大!
					GUI_SetColor(device_info.TextColor);
					i++;
				}
				if(((DlgParam == 0) ||(DlgParam == 2)) && ((Op_Data.DetTestResult&0x01) == 0x01))
				{//只有快速测试时检测注册雷管
					GUI_SetColor(GUI_RED);
					GUI_DispStringAt("Have unlogged DETs!",30,140+20*i);//有未注册雷管!
                    i++;
                    if(Op_Data.FindUnRegDetInfo)
                    {
                        memset(temp, 0,sizeof(temp));
                        if(ERROR == XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, Op_Data.UnRegDetInfo.Pwd.U32Value, (u8*)temp))
                        {
                            sprintf((char*)Op_Data.Temp, "Chip ID:%02X%02X%02X%02X%02X%02X%02X%02X", //内码:
                                Op_Data.UnRegDetInfo.Serl.U8Buffer[0], Op_Data.UnRegDetInfo.Serl.U8Buffer[1], 
                                Op_Data.UnRegDetInfo.Serl.U8Buffer[2], Op_Data.UnRegDetInfo.Serl.U8Buffer[3],
                                Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], Op_Data.UnRegDetInfo.Pwd.U8Buffer[1], 
                                Op_Data.UnRegDetInfo.Pwd.U8Buffer[2], Op_Data.UnRegDetInfo.Pwd.U8Buffer[3]);                        
                        }
                        else
                        {
                            sprintf((char*)Op_Data.Temp, "DER ID:%s", temp);//管码:
                        }

                        GUI_DispStringAt((char*)Op_Data.Temp,30,140+20*i);
                        i++;                        
                    }                    
					GUI_SetColor(device_info.TextColor);					
				}			
			}
			break;
		case WM_INIT_DIALOG://初始化界面
			if (Det.ErrorCount == 0)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));		
			}
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			if ((DEVICE_USAGES_SLAVE == device_info.DeviceUsages) && 
				(GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500))
			{//目前是由主机控制
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			}			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
                case GUI_KEY_LEFT:
                    if (GetWindowParam() == 2)
                    {
                        switch(ShowBlastCannelConfirm())
                        {
                            case GUI_KEY_RIGHT:
                                ShowBlastCannelDlg();
                                break;
                        }
                    }
                    else
                    {
                        if (!((DEVICE_USAGES_SLAVE == device_info.DeviceUsages) && 
                            (GetChannelxVoltage(ADC_CH_11_3V, 10) >= 2500)))
                        {
                            ShowMainMenuDlg(1);					
                        }                    
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if (Det.ErrorCount != 0)
                    {
                        SetNetTestErrorListViewDlgListViewSelIndex(0);
                        if (GetWindowParam() == 2)
                        {
                            ShowNetTestErrorListViewDlg(6);
                        }
                        else
                        {
                            ShowNetTestErrorListViewDlg(GetWindowParam());
                        }
                    }	
                    break;
                case GUI_KEY_ENTER:
                    if (GetWindowParam() == 2)
                    {//起爆流程前的组网测试, 只有从机和控制盒会进入这里
                        if(DEVICE_USAGES_SLAVE == device_info.DeviceUsages)
                        {
                            ShowRemoteBlastSlaveStateDlg(0); 
                        }
                        else if(DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType)
                        {
                            Op_Data.BlueSlaveStatus = BLAST_INFO_VERIFY;
                            ShowBlastInfoVerifyDlg();
                        }
                        else
                        {
                            ShowBlueHostStatusDlg(0);
                        }
                    }	
                    break;                      
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
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

void ShowNetTestReportDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
