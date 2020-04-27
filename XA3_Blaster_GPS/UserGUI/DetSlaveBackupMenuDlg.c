#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { BUTTON_CreateIndirect,   "OK",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 125, 240,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 105, 240,  25,  0,          0 },//1.主机<-从机
  { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     0, 135, 240,  25,  0,          0 }//2.主机->从机
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u8 DlgState = 0;
	
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
			
			SetRS485Master();
			
			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &SMALL_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1), &SMALL_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		  
		  TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT2), &NORMAL_FONT);
		  TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT3), &NORMAL_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		  
		  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "1.M<-S");
		  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "2.M->S");
		
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
						SetRS485Slave();
						
						ReadAllDetInfo(0);
						if(DlgState == 0x00)
						{								
							ShowDetBackupBlueTypeSelDlg();
						}
						else
						{
							ShowDetSlaveBackupMenuDlg();						
						}					
					}					
					break;		
				case GUI_KEY_1://把从机中的数据导入主机
					if(DlgState == 0x00)
					{
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
				    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
						DlgState = 0x01;
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), " ");						

						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Importing Dat...");//正在导入数据...
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
						
						while(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
						{
							OSTimeDly(1);
						}
						TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)14);/*把从机中的数据导入主机*/		
					}
					break;
				case GUI_KEY_2://数据备份到密钥
					  WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
				    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
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
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Exporting Dat...");//正在导出数据...
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
						  WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							
							while(TaskIsExist(APP_BACKGROUND_TASK_PRIO))
							{
								OSTimeDly(1);
							}
							TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)15);/*把主机中的数据导出到从机*/							
						}						
					}					
					break;						
			}
			break;
		case WM_BACKGROUND_TASK_END:
		  	WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
			  WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT3));
			if(DlgState == 0x01)//正在把从机中人数据导入主机
			{ 
				  
				if((pMsg->Data.v) == 0)
				{							
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Export Completed(M<-S)!");	//导入数据(主机<-从机)成功!									
				}
				else if((pMsg->Data.v) == 1)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Not finding the slave!");//没有找到从机!
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Permit connect to a slave!");//只能接一台从机!
				}				
				else if((pMsg->Data.v) == 2)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Diff Device Type!");//设备类型不相同!"
				}	
				else if((pMsg->Data.v) == 3)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"No Log DETs in the slave!");//从机没有注册雷管!
					
				}
				else if((pMsg->Data.v) == 4)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Chip TypeCode ERR!");		//雷管类型错误!			
				}				
				else if((pMsg->Data.v) == 5)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Save Failed!");//保存数据失败!
				}			
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");	//无法连接从机!				
				}
				
				ReadAllDetInfo(0);//重新读数据
				
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));				
			}
			else if(DlgState == 0x02)//正在把主机中人数据导入从机
			{
				if((pMsg->Data.v) == 0)
				{							
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Export Completed(M->S)!");	//导出数据(主机->从机)成功!									
				}
				else if((pMsg->Data.v) == 1)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Not finding the slave!");//没有找到从机!"
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"Permit connect to a slave!");//只能接一台从机!
				}				
				else if((pMsg->Data.v) == 2)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Diff Device Type!");//设备类型不相同!
				}	
				else if((pMsg->Data.v) == 3)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Chip TypeCode ERR!");	//雷管类型错误!				
				}				
				else if((pMsg->Data.v) == 4)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Del slave Dat failed!");//从机删除数据失败!
				}			
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");	//无法连接从机!				
				}

				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));				
			}
			WM_InvalidateWindow(hWin);
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

void ShowDetSlaveBackupMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
