#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  50,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 100, 240,  25,  0,          0 },//不知道为什么这一行不显示（字体排列声明了两次，起冲突了）
  { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 125, 240,  25,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u32 DelayFinishTime = 0;//延时结束时对话的系统时间
	static u8 BlastState = 0;
	u32 CurrentTime = 0;
	u32 Value;
	OS_STK_DATA StkData;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);         
			GUI_Clear();

			ShowInfoTitle("Blast Progress");	//起爆流程

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);		//注册雷管:		错误雷管:	
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount); //注册雷管:		错误雷管:
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);			
			break;
		case WM_INIT_DIALOG://初始化界面
			Op_Data.BlastStatus = DET_BLASTING;
			
			BlastState = 0;//等待倒数计数结束
			
			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &BIG_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);



			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
			
			CurrentTime = OSTimeGet();
			if(0 == device_info.BlastDelay)
			{
				DelayFinishTime = CurrentTime;
			}
			else
			{
				DelayFinishTime = CurrentTime + 5000/(1000/OS_TICKS_PER_SEC);//5秒倒数计数+3秒显示"起爆"				
			}
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}		
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
                    
                    if(MsgBoxIsShow())
                    {
                        break;
                    }
					CurrentTime = OSTimeGet();
					switch(BlastState)
					{
						case 0://当前为起爆倒数计数状态
							if(CurrentTime < DelayFinishTime)
							{
								Value = (DelayFinishTime - CurrentTime)*(1000/OS_TICKS_PER_SEC);
								if(Value >= 5000)
								{
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"5");	
								}
								else
								{
									sprintf(temp, "%d", 1+(Value/1000));						
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);							
								}				
							}
							else
							{//延时时间到
								BlastState++;//等待起爆状态
								//保存发送起爆信号之前的总线电压电流值
								BlastInfo.V_BeforeFire = Op_Data.LinABVoltage;
								BlastInfo.I_BeforeFire = Op_Data.LinABCurrent;
								
								BlastSingleMonitor.Count = 0;
								
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Blast");//起爆  Fire不显示
							  WM_Paint(WM_GetDialogItem(hWin , GUI_ID_TEXT0));//立即更新界面
								
								if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
								{
                                    ErrorStatus Result;

                                    UTF8StringToGBK("准备发送起爆信号!", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),CURRENT_STATUS);
                                    
									OPEN_BUS_6_7V();//把总线切到低压下再发送起爆信号
														
									System168MDelay1ms(20);
									Result = CreateStartBlastTask();
									OPEN_HV_SW();//起爆完成，总线电压恢复到高压状态
																		
									WriteSystemLog(NULL,0,BLAST_SINGLE_DATA);//保存起爆信号监测数据	
									                                   
                                    if (Result == ERROR)
                                    {
										UTF8StringToGBK("起爆信号发送前电压异常", (char*)Op_Data.Temp);
										WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO); 
                                       
                                        MsgBoxInit();
                                        SetMsgBoxCaption("Blast Progress");//起爆流程
                                        SetMsgBoxMessage("Bus Short Circuit!\n\nNo Blasting!");//总线短路! 禁止发送起爆信号!
                                        sprintf(temp, "DET QTY:%d", Det.RegCount);//注册雷管数:
                                        SetMsgBoxHelp1(temp);

                                        ShowMsgBox();
                                        return;
                                    }
                                    else if (BlastSingleSendedResult() == ERROR)
                                    {//
										UTF8StringToGBK("起爆信号发送失败", (char*)Op_Data.Temp);
										WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO); 
                                       
                                        Op_Data.DoubleKeySwitch = 0;//不允许按下双键
                                        CLOSE_DOUBLE_KEY_SW();//就是想让这个引脚电平切换一下
                                    
                                        MsgBoxInit();
                                        SetMsgBoxCaption("Blast Progress");//起爆流程
                                        SetMsgBoxMessage("Failure Sending!");//起爆信号发送失败!
                                        sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                        SetMsgBoxHelp1(temp);
                                        SetMsgBoxRight("Retry");//重试

                                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                                        {
                                            ShowBlastProcessDlg(1);
                                            return;
                                        }                                      
                                    }
                                    else
                                    {
                                        Op_Data.BusMonitorDisableShow = 1;//总线异常时不显示
                                        
                                        SaveBlasteFireTime();//保存起爆时间
                                        if(Det.RegCount != 0)
                                        {
                                            WriteBlasterRecord();//保存起爆记录
                                            WriteSystemLog(NULL,0,BLAST_RECORDER_COPY);//把起爆记录保存到日志中
                                            UTF8StringToGBK("高压调压电路DA最小值:", (char*)Op_Data.Temp);
                                            WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO);
                                            WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
                                        }                                    
                                        Op_Data.BlastStatus = DET_BLAST_END;
                                        Op_Data.DoubleKeySwitch = 0;                                    
                                    }
								}
								else
								{
									TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)7);
										
									OSTimeDly(5);
								}
								
								DelayFinishTime = CurrentTime + 3000/(1000/OS_TICKS_PER_SEC);//3秒显示"起爆"	
								
							}
							break;
						case 1://起爆信号发送完成，正在显示"起爆"
							if(CurrentTime >= DelayFinishTime)
							{//等待时间结束
								if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
								{//等待从机的起爆结果
									if(OS_ERR_NONE == OSTaskStkChk(APP_DET_OPERATE_TASK_PRIO, &StkData))
									{
										break;
									}
								}
								WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
								TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &SMALL_FONT);
								WM_ResizeWindow(WM_GetDialogItem(hWin , GUI_ID_TEXT0),0, 25 - WM_GetWindowSizeY(WM_GetDialogItem(hWin , GUI_ID_TEXT0)));//改为控件大小，主要是为了行间距更好看
								WM_MoveTo(WM_GetDialogItem(hWin , GUI_ID_TEXT0),0,160);						
								
								TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1), &SMALL_FONT);
								TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
								TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
								WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
								

								if(DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType)
								{//当前设备类型为安管设备
									if (((DEVICE_USAGES_MASTER != device_info.DeviceUsages) && (0 == Det.RegCount)) ||
										((DEVICE_USAGES_MASTER == device_info.DeviceUsages) && (0 == SlaveInfo.RegCount))) 
									{//雷管数为0时不需要上传数据，直接显示关机
										BlastState = 3;
										WM_MoveTo(WM_GetDialogItem(hWin , GUI_ID_TEXT0),0,120);
										WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));							
									}
									else
									{			
										BlastState = 2;//等待数据上传、下载完成
										DelayFinishTime = CurrentTime + 600000/(1000/OS_TICKS_PER_SEC);//超时时间设置为10分钟
										
										TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Sending...");//起爆信息上传中...
										TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0)," ");								
																					
										UTF8StringToGBK("保存当前起爆信息!", (char*)Op_Data.Temp);
										WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO);//日志
									}
								}
								else
								{//非安管设备，直接显示关机
									BlastState = 3;
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"");
									WM_MoveTo(WM_GetDialogItem(hWin , GUI_ID_TEXT0),0,120);
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));							
								}
							}
							break;
						case 2://等待起爆记录写入密钥和上传服务器完成
							if(Op_Data.BlastInfoUpdateResult)
							{
								BlastState++;
								
								if(Op_Data.BlastInfoUpdateResult == '1')
								{
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Upload Completed!");//起爆信息上传成功!
										
									UTF8StringToGBK("起爆信息上传成功!", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO);//日志
									
									UpdateBlasterRecordUploadState(1,0xFFFF);//更新上传标志								
								}
								else
								{
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Upload Failed!");//起爆信息上传失败!
										
									UTF8StringToGBK("起爆信息上传失败!", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO);//日志								
								}
							}
							else
							{
								if(CurrentTime >= DelayFinishTime)
								{//已经超时
									BlastState++;
																		
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Time out,failed!");//起爆信息上传失败(超时)!
									
									UTF8StringToGBK("起爆信息上传失败(超时)!", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp,strlen((char*)Op_Data.Temp),NEW_STR_INFO);//日志												
								}									
							}
							break;
						case 3://等待关闭设备
							WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
						  TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"The Blast Over!");	//起爆完毕,请关闭设备!
              TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2),"Pls Shut Down!");	//起爆完毕,请关闭设备!
							BlastState++;
							if((DEVICE_TYPE_SAFE_MONITOR != device_info.DeviceType) && (DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (device_info.BlastDelay >= 11))
							{//非安管设备长延时10分钟以上，起爆完自动关机
								ShowPowerOffDlg(0);
							}
							break;
						default:
							break;
					}				
					break;
				case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
						BlastTerminationFromAppDlg();
						break;
					}
                    
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

void ShowBlastDetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
