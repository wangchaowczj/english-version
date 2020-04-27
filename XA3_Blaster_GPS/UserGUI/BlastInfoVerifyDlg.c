#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Cancel",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//取消
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 105, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     20, 130, 200,  20,  0,          0 }
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
			switch(Op_Data.BlueSlaveStatus)
			{
				case BLAST_INFO_VERIFY:
					ShowInfoTitle("Blast Progress");//起爆流程							
					break;
				case DET_INFO_IN_OR_OUT:
					ShowInfoTitle("Dat Backup");//数据备份							
					break;
				case DET_INFO_INSERT:
					ShowInfoTitle("Dat Merge");//数据合并	
					break;
				default:
					break;
			}			
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:	错误雷管:				
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:	错误雷管:	
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
			Op_Data.BlastVerifyResult = 0;
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			if(BLAST_INFO_VERIFY == Op_Data.BlueSlaveStatus)
			{
				if(device_info.BlastDelay == 0)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Current DLY:0s");//当前起爆延时:0秒
				}
				else if(device_info.BlastDelay == 1)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Current DLY:5s");//当前起爆延时:5秒
				}
				else 
				{
					sprintf((char*)Op_Data.Temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);//当前起爆延时: 时 分 秒						
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
				}			
			}
            Op_Data.AppTemporaryTaskResult = SUCCESS;
            OSTaskDel(APP_TEMPORARY_TASK_PRIO);
            if(GetBlueStatus() != BLUE_SLAVE)
            {
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Initializing The Bluetooth...");//蓝牙初始化...
							  
                //蓝牙模块从机初始化
                TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)0);             
            }
            else
            {
                OPEN_BLUE_POWER();
            }
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_1MS, 0);//查询方式判断蓝牙是否完成初始化  
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					switch(Op_Data.BlueSlaveStatus)
					{
						case BLAST_INFO_VERIFY:
							CLOSE_BLUE_POWER();
							ShowMainMenuDlg(0);							
							break;
						case DET_INFO_IN_OR_OUT:
							CLOSE_BLUE_POWER();
							ReadAllDetInfo(0);
							ShowDetBackupBlueTypeSelDlg();							
							break;
						case DET_INFO_INSERT:
							CLOSE_BLUE_POWER();
							ReadAllDetInfo(0);
							ShowDetBackupOperateBlueTypeSelDlg();							
							break;							
						default:
							break;
					}										
					break;
			}
			break;
		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_10MS_ID:
					if(!TaskIsExist(APP_TEMPORARY_TASK_PRIO))
					{//已经初始完成
						if((ErrorStatus)Op_Data.AppTemporaryTaskResult != ERROR)
						{//初始化成功
							switch(Op_Data.BlueSlaveStatus)
							{
								case BLAST_INFO_VERIFY:
                                    Op_Data.BlastStatus = NOT_START;
                                    
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Waiting for the validation");//等待手持设备验证...	
								  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "of handheld device...");//等待手持设备验证...	
								
									UTF8StringToGBK("等待手持设备验证", (char*)Op_Data.Temp);									
									WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);									
									break;
								case DET_INFO_IN_OR_OUT:
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Waiting for the Dat backup...");//等待主机备份数据...	

									UTF8StringToGBK("等待主机备份数据", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);										
									break;
								case DET_INFO_INSERT:
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Waiting for the Dat merge...");//等待主机合并数据...

									UTF8StringToGBK("等待主机合并数据", (char*)Op_Data.Temp);
									WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);										
									break;									
								default:
									break;
							}													
						}
						else
						{
                            MsgBoxInit();
                            SetMsgBoxCaption("Device Abnormal");//设备异常
                            SetMsgBoxMessage("Bluetooth Abnormal!");//蓝牙模块异常!
                            SetMsgBoxLeft("Back");//返回

                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowMainMenuDlg(0);
                            }							
						}
					}
					else
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
					}
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
					if(BLAST_INFO_VERIFY == Op_Data.BlueSlaveStatus)
					{
						if('1' == Op_Data.BlastVerifyResult)
						{
							UTF8StringToGBK("APP授权验证通过", (char*)Op_Data.Temp);
							WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), CURRENT_STATUS);
							
							if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
							{
								SetSlaveInfoListViewDlgListViewSelIndex(0);
								ShowSlaveInfoListViewDlg(3);							
							}
							else
							{
								ShowWaitLinCurrentReadyDlg(2);					
							}							
						}					
					}
					WM_Paint(hWin);
					break;
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowBlastInfoVerifyDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
