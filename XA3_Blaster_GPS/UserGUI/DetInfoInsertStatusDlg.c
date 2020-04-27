#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 110, 240,  20,  0,          0 }
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
			ShowInfoTitle("Dat Merge");//数据合并			
			
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);						
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{						
						if((0 == GET_BLUE_POWER_STATUS()) && (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS()))
						{
							ShowDetBlueInsertMenuDlg();
						}
						else
						{
							ShowDetBackupOperateBlueTypeSelDlg();
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
                    
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }	
                    
                    WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    
                    if(GetWindowParam() == 0)
                    {//合并数据(主机<-从机)
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Importing Dat...");//正在导入数据...
                        TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)12);//
                    }
                    else
                    {//合并数据(主机->从机)
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Exporting Dat...");//正在导出数据...
                        TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)13);//
                    }
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
					break;
				case GUI_TIMER_2MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
					ReadAllDetInfo(0);
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
					WM_InvalidateWindow(hWin);
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
		case WM_BACKGROUND_TASK_END:
		    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
		if(GetWindowParam() == 0)
			{//合并数据(读入)
				if((pMsg->Data.v) == 1)
				{							
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Merge Completed(M<-S)!");	//合并数据(主机<-从机)成功!									
				}
				else if((pMsg->Data.v) == 2)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Save Failed!");//保存数据失败!
				}
				else if((pMsg->Data.v) == 3)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Chip TypeCode ERR!");	//雷管类型错误!				
				}
				else if((pMsg->Data.v) == 4)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"No Dat in the slave!");		//从机没有数据!			
				}
				else if((pMsg->Data.v) == 5)
				{					
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Have Repeated DET Dat!");		//有重码雷管数据!			
				}				
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");	//无法连接从机!				
				}
			}
			else
			{
				if((pMsg->Data.v) == 0)
				{							
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "No Dat in the master!");//主机没有数据!
				}
				else if((pMsg->Data.v) == 2)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dat Merge Completed(M->S)!");	//合并数据(主机->从机)成功!
				}
				else if((pMsg->Data.v) == 3)
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Have Repeated DET Dat!");	//有重码雷管数据!
				}					
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Can't connect to the slave!");	//无法连接从机!				
				}				
			}
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);			
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetInfoInsertStatusDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
