#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { BUTTON_CreateIndirect,   "Charge",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//充电
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 50, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 70, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 90, 240,  20,  0,          0 }, 
  { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     0, 110, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT4,     0, 130, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT5,     0, 150, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT6,     0, 170, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT7,     0, 190, 240,  20,  0,          0 }  
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("DET Analysis");//雷管分析
			
			GetDetInfoString(&temp[0], Det.Type);	
			ShowInfoHelp("Permit a DET on bus", &temp[0]);//总线只能接1发雷管
			break;
		case WM_INIT_DIALOG://初始化界面
			CLOSE_HV_SW_INSIDE();
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
      TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
      TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT6), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT7), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			if(Op_Data.LinABCurrent < 10)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Waiting for");//等待雷管上线...
			  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "the DET logging...");//等待雷管上线...
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "Pls Re-logging...");//请雷管重新上线...
			}
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
			
            Op_Data.LinNormalLowVoltageGoal = 5500;
            DacOutLVVoltage(Op_Data.LinNormalLowVoltageGoal);
                        
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{//等待分析雷管
						DET_COM_SEND_PIN_INIT();//重新初始化电平，雷管分析时，反相测电流
						ShowHelpToolMenuDlg();					
					}
					break;
				case GUI_KEY_RIGHT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
					{//充电，只有管厂专用设备才有这个功能
						DET_COM_SEND_PIN_INIT();//重新初始化电平，雷管分析时，反相测电流
						ShowDetAnalystHighVoltageInputDlg();					
					}
					break;					
			}	
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
                case GUI_TIMER_10MS_ID:
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(AppTaskResult.DetOperate&DET_ANALYST_READ_SN_MASK_BIT)
                    {//序列码回读正常
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT6), TEXT_CF_LEFT|TEXT_CF_VCENTER);
											  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT7), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                        
                        memset(temp, 0, sizeof(temp));
                        strcpy((char*)temp, "DET ID:");//雷管管码:
                        if(ERROR != XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)&temp[strlen(temp)]))
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
                        }
                        else
                        {
                            strcpy((char*)temp, "DET ID:unkonwn");//雷管管码:未知
                        }
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
                        
                        sprintf(temp,"Chip ID:%02X%02X%02X%02X%02X%02X%02X%02X",//雷管内码：
                            Op_Data.RegDetInfo.Serl.U8Buffer[0],Op_Data.RegDetInfo.Serl.U8Buffer[1],
                            Op_Data.RegDetInfo.Serl.U8Buffer[2],Op_Data.RegDetInfo.Serl.U8Buffer[3],
                            Op_Data.RegDetInfo.Pwd.U8Buffer[0],Op_Data.RegDetInfo.Pwd.U8Buffer[1],
                            Op_Data.RegDetInfo.Pwd.U8Buffer[2],Op_Data.RegDetInfo.Pwd.U8Buffer[3]);	
                            
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);

                        sprintf(temp,"I:%duA(P) %duA(N)",BlastInfo.I_BeforeTest, BlastInfo.I_BeforeFire);//电流: 正 反
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), temp);
                        
                        if(AppTaskResult.DetOperate&DET_ANALYST_FREQ_MASK_BIT)
                        {//测频正常
                            sprintf(temp,"F:%uHz ",Op_Data.RegDetInfo.Freq.U32Value);//频率:
                        }
                        else
                        {
                            strcpy(temp,"F:measure failed ");	//频率:测量失败						
                        }
                        
                        if(AppTaskResult.DetOperate&DET_ANALYST_UNLOCK_MASK_BIT)
                        {//解密
                            strcpy(&temp[strlen(temp)], "ChipPWD:CORR");//密码:正确
                        }
                        else
                        {
                            strcpy(&temp[strlen(temp)], "ChipPWD:ERR");//密码:错误
                        }
                        
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), temp);
                        
                        if(AppTaskResult.DetOperate&DET_ANALYST_BIT_RESULT_MASK_BIT)
                        {//BIT测试正常
                            strcpy(&temp[0], "BIT Test: P ");//BIT测试:成功
                        }
                        else
                        {
                            strcpy(&temp[0], "BIT Test: F ");
                        }					
                        
                        if(AppTaskResult.DetOperate&DET_ANALYST_WRITE_DELAY_MASK_BIT)
                        {//写延期正常
                            strcpy(&temp[strlen(temp)], "Write DLY: P ");//写延期:成功
                        }
                        else
                        {
                            strcpy(&temp[strlen(temp)], "Write DLY: F ");//写延期:失败
                        }
                        
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), temp);
                        
                        if(AppTaskResult.DetOperate&DET_ANALYST_CHARGE_MASK_BIT)
                        {//充电正常
                            sprintf(temp,"Charge: P  I:%duA", BlastInfo.I_AfterCharge);//充电:成功    电流:
                        }
                        else
                        {
                            sprintf(temp,"Charge: F  I:%duA", BlastInfo.I_AfterCharge);//充电:失败    电流:
                        }
                        
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), temp);
                        
                          memset(temp, 0, sizeof(temp));
                          strcpy(temp, "DET Type:");//雷管类型:
                          GetDetTypeName((char*)&temp[strlen((char*)temp)] ,GetDetType(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value));
												  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6), temp);
												
                        if(DetCodeRulePass(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value))
                        {
                            strcpy(&temp[0], "Chip ID:normal");//内码:正常
                        }
                        else
                        {
                            strcpy(&temp[0], "Chip ID:abnormal");//内码:异常
                        }
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7), temp);
                        
                        if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Op_Data.DelaySampleCfg))
                        {
                            WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                        }				
                    }
                    else
                    {
                        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
											  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "");				
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "");
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "Chip ID reads back failed!");//雷管内码回读失败!
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "");
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), "");
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6), "");
                    }
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)); 
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
                    break;
				case GUI_TIMER_120MS_ID:
                    if((abs(Op_Data.LinABVoltage-LIN_NORMAL_LOW_VOLTAGE_DET_ANALYS_GOAL) > 100) || (Op_Data.LinABCurrent < 10) || (XA3_DetCheckIn() == ERROR))
                    {//刚进来时由7V降到5.5V，如果之前已经连接有雷管，会导致没有电流
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }

                    WM_DeleteTimer(pMsg->Data.v);
                    
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");						
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "DET Analysing...");//雷管分析中...
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6), "");
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));							

                    //雷管检测任务
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);                       
                    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)22);
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);                                           				
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
                    
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

void ShowDetAnalystDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
