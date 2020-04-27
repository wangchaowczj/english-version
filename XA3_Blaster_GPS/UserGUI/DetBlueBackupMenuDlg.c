#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { BUTTON_CreateIndirect,   "OK",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//确定
  { TEXT_CreateIndirect, "1.M<-S", GUI_ID_TEXT0,     0, 105, 240,  25,  0,          0 },//1.主机<-从机
  { TEXT_CreateIndirect, "2.M->S", GUI_ID_TEXT1,     0, 130, 240,  25,  0,          0 }//2.主机->从机
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u8 DlgState = 0;
	OS_STK_DATA StkData;
	u16 RegNumber, ErrNumber;
	u8 Type;
	ErrorStatus Result;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Dat Backup");//数据备份
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);
			break;
		case WM_INIT_DIALOG://初始化界面
			DlgState = 0;
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));//暂时隐藏右功能键
			
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
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{//当前左功能键可用
						ReadAllDetInfo(0);
						if(DlgState == 0x00)
						{
							CLOSE_BLUE_POWER();	
							ShowHelpToolMenuDlg();
						}
						else
						{
							
							if(BLUE_NOT_CONNECT == GET_BLUE_CONNECT_STATUS())
							{
								ShowDetBackupBlueTypeSelDlg();
							}
							else
							{
								ShowDetBlueBackupMenuDlg();
							}							
						}					
					}					
					break;
				case GUI_KEY_RIGHT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
					{
						if(DlgState == 0x01)
						{//等待确认从密钥中恢复数据							
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Importing Dat...");//正在导入数据...
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							
							//在后台进行设备自检
							while(OS_ERR_TASK_NOT_EXIST != OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
							{
								OSTimeDly(1);
							}
							TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)3);/*把密钥中的数据导入设备*/					
						}
						else if(DlgState == 0x02)
						{//等待确认数据备份到密钥							
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Exporting Dat...");//正在导出数据...
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							
							//在后台进行设备自检
							while(OS_ERR_TASK_NOT_EXIST != OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
							{
								OSTimeDly(1);
							}
							TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)4);/*把数据导出到密钥*/					
						}					
					}			
					break;			
				case GUI_KEY_1://从密钥中恢复数据
					if(DlgState == 0x00)
					{
						DlgState = 0x01;
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), " ");
						
						Result = GetBlueSlaveVersion(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.Temp,sizeof(Op_Data.Temp));
						if(Result == ERROR)
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");//无法连接从机!
							break;
						}
						else
						{
							if(FormatIsSameForDetInfo(Op_Data.Temp) == 0)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Diff Device Type!");//设备类型不同!
								break;							
							}
						}						
						Result = GetBlueSlaveDetNumberAndType(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4], &RegNumber, &ErrNumber, &Type);
						if(Result != ERROR)
						{
							if(RegNumber == 0)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "No Logged DETs!");	//没有注册雷管!							
							}
							else if (Type != Det.Type)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip TypeCode ERR!");//雷管类型错误!									
							}
							else
							{
								if(Det.RegCount != 0)
								{									
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Cover the Dat of master?");//确定覆盖主机数据?
									WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
								}
								else
								{									
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Importing Dat...");//正在导入数据...
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
									
									//在后台进行设备自检
									while(OS_ERR_TASK_NOT_EXIST != OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
									{
										OSTimeDly(1);
									}
									TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)3);/*把密钥中的数据导入设备*/																	
								}
							}
						}
						else
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");//无法连接从机!
						}
					}
					break;
				case GUI_KEY_2://数据备份到密钥
					if(DlgState == 0x00)
					{
						DlgState = 0x02;
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), " ");						
						if(Det.RegCount == 0)
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "No Logged DETs!");//没有注册雷管!
						}
						else
						{
							Result = GetBlueSlaveVersion(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.Temp,sizeof(Op_Data.Temp));
							if(Result == ERROR)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");//无法连接从机!
								break;
							}
							else
							{
								if(FormatIsSameForDetInfo(Op_Data.Temp) == 0)
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Diff Device Type!");//设备类型不同!
									break;							
								}
							}						
							Result = GetBlueSlaveDetNumberAndType(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4], &RegNumber, &ErrNumber, &Type);
							if(Result != ERROR)
							{
								if (Type != Det.Type)
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Chip TypeCode ERR!");//雷管类型错误!								
								}
								else if(RegNumber == 0)
								{								
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Exporting Dat...");//正在导出数据...
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
									WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
									
									//在后台进行设备自检
									while(OS_ERR_TASK_NOT_EXIST != OSTaskStkChk(APP_BACKGROUND_TASK_PRIO, &StkData))
									{
										OSTimeDly(1);
									}
									TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)4);/*把数据导出到密钥*/								
								}
								else
								{
									
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Cover the Dat of slave?");//确定覆盖从机中的数据?
									WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
								}
							}
							else
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");//无法连接从机!
							}						
						}						
					}					
					break;						
			}
			break;
		case WM_BACKGROUND_TASK_END:
			if(DlgState == 0x01)//正在从密钥中导入数据
			{
				if((pMsg->Data.v) == 1)
				{							
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Export Completed(M<-S)!");//导入数据(主机<-从机)成功!										
				}
				else if((pMsg->Data.v) == 2)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Save Failed!");//保存数据失败!
				}
				else if((pMsg->Data.v) == 3)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Chip TypeCode ERR!");//雷管类型错误!				
				}				
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");//无法连接从机!					
				}
				
				ReadAllDetInfo(0);//重新读数据
				
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
				WM_InvalidateWindow(hWin);
			}
			else if(DlgState == 0x02)//把数据导入到密钥中
			{
				if(((ErrorStatus)pMsg->Data.v) != ERROR)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Export Completed(M->S)!");//导出数据(主机->从机)成功!
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Export Failed(M->S)!");//导出数据(主机->从机)失败!
				}

				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));				
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

void ShowDetBlueBackupMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
