#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },  //返回
  { BUTTON_CreateIndirect,   "CONT",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//继续
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 120, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 140, 240,  20,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	CHECK_RESULT Result;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("DET Test");//雷管测试
			GetDetInfoString(&temp[0], Det.Type);
			ShowInfoHelp("Permit a DET on bus", &temp[0]);//总线只能接1发雷管
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			if(Op_Data.LinABCurrent < 10)
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Waiting for ");//等待雷管上线...
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "the DET logging...");//等待雷管上线...
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls re-logging...");//请雷管重新上线...
				
			}
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

            Op_Data.LinNormalLowVoltageGoal = 5500;
            DacOutLVVoltage(Op_Data.LinNormalLowVoltageGoal);
                        
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));	
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
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{//返回
                        Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_L_GOAL;
                        DacOutLVVoltage(Op_Data.LinNormalLowVoltageGoal);
                        switch(GetWindowParam())
                        {
                            case 0:
                                ShowHelpToolMenuDlg();
                                break;
                            case 1:
                                ShowWaitRegListViewDlg();
                                break;                                
                        }											
					}										
					break;
				case GUI_KEY_RIGHT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
					{//继续
						ShowHelpToolDetTestDlg(GetWindowParam());					
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

                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "");

                    Result = (CHECK_RESULT)AppTaskResult.DetOperate;
                    if ((DET_PASS == Result) || (OVERLAPED == Result))
                    {
                        if(Op_Data.LinABCurrent < device_info.single_current_min)
                        {
                            Result = CURRENT_MIN;
                        }
                        else if(Op_Data.LinABCurrent > device_info.single_current_max)
                        {
                            Result = CURRENT_MAX;
                        }
                    }
                    if(DET_PASS  == Result)
                    {//正常时，自动检测下一发
                        SoundOutputRight();
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                        WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_120MS_ID, GUI_TIMER_120MS, 0);
                    }
                    else
                    {
                        SoundOutputError(); 
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    }
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));    
                    switch(Result)//雷管检测结果
                    {
                        case DET_PASS:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DET Normal!");//雷管正常!
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "DETs are not logged!");//雷管还没有注册!
                            memset(temp, 0, sizeof(temp));
                            if(XA3_DetInside2Outside(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value, (u8*)temp) != ERROR)
                            {
                                sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码:
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)Op_Data.Temp);
                            }
                            else
                            {
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "DET ID:unknown");//管码:未知
                            }
                            break;
                        case OVERLAPED:			  //上线过程中这个序列号已经注册
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DET Normal!");//雷管正常!
                            sprintf(temp,"DET has been logged:%d-%d", Det.Info[Op_Data.overlaped].Hole.Name, Det.Info[Op_Data.overlaped].Hole.Inside);//雷管已注册:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);
                            memset(temp, 0, sizeof(temp));
                            if(XA3_DetInside2Outside(Det.Info[Op_Data.overlaped].Serl.U32Value, Det.Info[Op_Data.overlaped].Pwd.U32Value, (u8*)temp) != ERROR)
                            {
                                sprintf((char*)Op_Data.Temp, "DET ID:%s", temp);//管码:
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)Op_Data.Temp);
                            }
                            else
                            {
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "DET ID:unknown");//管码:未知
                            }
                            break;
                        case TEST_ERR:			  //BIT错误 
                        case FREQ_MIN:			  //频率过小
                        case FREQ_MAX:			  //频率过大
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DET ERR!");		//雷管错误!		
                            break;
                        case TYPE_ERR:			  //雷管类型错误
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip TypeCode ERR!");//雷管类型错误!
                            GetDetTypeName((char*)Op_Data.Temp ,GetDetType(Op_Data.RegDetInfo.Serl.U32Value, Op_Data.RegDetInfo.Pwd.U32Value));
                            sprintf(temp,"Chip TypeCode:%s", (char*)Op_Data.Temp);//当前雷管类型:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), temp);
                            break;
                        case CURRENT_MIN:		  //电流小
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DET Current Small!");//雷管电流小!
                            break;
                        case CURRENT_MAX:		  //电流大		
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DET Current Large!");//雷管电流大!
                            break;
                        case SECU_CODE_ERR:		  //密码校验错误		
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "ChipPWD ERR!");//密码错误!
                            break;
                        case SERL_NULL_ERR:       //内码还没有写入
                        case DET_CODE_ERR:		  //内码不符合规则		
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip ID ERR!");//雷管内码错误!
                            break;
                        case DET_FAC_CODE_ERR:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip Factory Code ERR!");//工厂代码错误!
                            break;                            
                        default:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Communication Abnormal!");//通信异常!
                            break;
                    }
                    break;
				case GUI_TIMER_120MS_ID:
                    if((abs(Op_Data.LinABVoltage-LIN_NORMAL_LOW_VOLTAGE_DET_ANALYS_GOAL) > 100) || (Op_Data.LinABCurrent < 10) || (XA3_DetCheckIn() == ERROR))
                    {//刚进来时由7V降到5.5V，如果之前已经连接有雷管，会导致没有电流
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_120MS);
                        break;
                    }

                    WM_DeleteTimer(pMsg->Data.v);
                    
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Testing...");//测试中...
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "");
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
                    //雷管操作任务
                    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
                    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)27);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);                   							
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
					break;
				default:
					break;
			}		
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowHelpToolDetTestDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
