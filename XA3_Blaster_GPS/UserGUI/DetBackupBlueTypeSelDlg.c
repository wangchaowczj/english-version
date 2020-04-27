#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
  { BUTTON_CreateIndirect, "OK", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//确定
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	static u8 DlgStatus = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Dat Backup");//数据备份
			
			if(DlgStatus == 0)
			{
				//GUI_DispStringHCenterAt("1.Master Bluetooth 4.Cascade Slave", 120, 95);//1.蓝牙主机 4.级联从机
				//GUI_DispStringHCenterAt("2.Slave Bluebooth 5.Local Backup",120, 120);//2.蓝牙从机 5.本机备份	
				//GUI_DispStringHCenterAt("3.Cascade Master 6.Local Recover",120, 145);//3.级联主机 6.本机恢复		
				GUI_DispStringAt("1.Master Bluetooth", 48,70); //1.蓝牙主机
				GUI_DispStringAt("2.Slave Bluebooth", 48,90);  //2.蓝牙从机
				GUI_DispStringAt("3.Cascade Master", 48,110);  //3.级联主机
				GUI_DispStringAt("4.Cascade Slave", 48,130);   //4.级联从机
				GUI_DispStringAt("5.Local Backup", 48,150);    //5.本机备份
				GUI_DispStringAt("6.Local Recover", 48,170);   //6.本机恢复
				
			}
			else if(DlgStatus == 1)
			{
				GUI_DispStringHCenterAt("Cover the backup Dat?",120, 100);//确定覆盖已备份数据?	
			}
			else if(DlgStatus == 2)
			{
				GUI_DispStringHCenterAt("Cover the local Dat?",120, 100);//确定覆盖本机数据?	
			}			
			sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
			GetDetInfoString(&temp[40], Det.Type);	
			ShowInfoHelp(&temp[0], &temp[40]);				
		
			break;
		case WM_INIT_DIALOG://初始化界面
			CLOSE_BLUE_POWER();
			DlgStatus = 0;
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}	
			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
			break;
		case WM_KEY:

			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if(DlgStatus == 0)
					{
						ShowHelpToolMenuDlg();					
					}
					else
					{
						ShowDetBackupBlueTypeSelDlg();
					}
					break;
				case GUI_KEY_RIGHT:
					if(DlgStatus == 1)
					{
                        SaveAllDetInfo(1);
                        
                        MsgBoxInit();
                        SetMsgBoxCaption("Dat Backup");//数据备份
                        SetMsgBoxMessage("Dat Backup Completed!");//数据备份成功!
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowHelpToolMenuDlg();
                        }                        
					}
					else if(DlgStatus == 2)
					{
						ReadAllDetInfo(1);
						//验证雷管类型是否一致
						if(ValidateDetType() == ERROR)
						{
							ReadAllDetInfo(0);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Dat Backup");//数据备份
                            SetMsgBoxMessage("Chip TypeCode ERR!");//雷管类型错误!
                            sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                            
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowHelpToolMenuDlg();
                            }                            
						}
						else
						{
							ValidateDetTypeAndDelay();
							SaveAllDetInfo(0);
							
                            MsgBoxInit();
                            SetMsgBoxCaption("Dat Backup");//数据备份
                            SetMsgBoxMessage("Dat Recovery Completed!");//数据恢复成功!
                            sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回

                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowHelpToolMenuDlg();
                            }							
						}					
					}
					break;					
				case GUI_KEY_1://主机模式
					ShowBlueHostStatusDlg(2);
					break;
				case GUI_KEY_2://从机模式
					Op_Data.BlueSlaveStatus = DET_INFO_IN_OR_OUT;
					ShowBlastInfoVerifyDlg();		
					break;
				case GUI_KEY_3://级联主机模式
					ShowDetSlaveBackupMenuDlg();
					break;
				case GUI_KEY_4://级联从机模式
					ShowSlaveStatusDlg();
					break;						
				case GUI_KEY_5://本机备份
					if(Det.RegCount == 0)
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("Dat Backup");//数据备份
                        SetMsgBoxMessage("No Logged DETs!");//没有注册雷管!
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
 
                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowDetBackupBlueTypeSelDlg();
                        }					
					}
					else
					{
						if(FileIsExist(DET_INFO_BACKUP_FILE_NAME) == ERROR)
						{                           
							SaveAllDetInfo(1);
							
                            MsgBoxInit();
                            SetMsgBoxCaption("Dat Backup");//数据备份
                            SetMsgBoxMessage("Dat Backup Completed!");//数据备份成功!
                            sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                            SetMsgBoxHelp1(temp);
                            SetMsgBoxLeft("Back");//返回
                        
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowHelpToolMenuDlg();
                            }						
						}
						else
						{//提示是否覆盖
							DlgStatus = 1; 
							WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							WM_Invalidate(hWin);
						}					
					}

					break;
				case GUI_KEY_6://本机恢复
					if(FileIsExist(DET_INFO_BACKUP_FILE_NAME) != ERROR)
					{
						if(Det.RegCount == 0)
						{
							ReadAllDetInfo(1);
							//验证雷管类型是否一致
							if(ValidateDetType() == ERROR)
							{
								ReadAllDetInfo(0);

                                MsgBoxInit();
                                SetMsgBoxCaption("Dat Backup");//数据备份
                                SetMsgBoxMessage("Chip TypeCode ERR!");//雷管类型错误!
                                sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowHelpToolMenuDlg();
                                }							
							}
							else if(Det.RegCount == 0)
							{                               
								ReadAllDetInfo(0);

                                MsgBoxInit();
                                SetMsgBoxCaption("Dat Backup");//数据备份
                                SetMsgBoxMessage("Backup Dat can't use!");//备份数据无法使用!
                                sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowHelpToolMenuDlg();
                                }								
							}
							else
							{                                
								ValidateDetTypeAndDelay();
								SaveAllDetInfo(0);

                                MsgBoxInit();
                                SetMsgBoxCaption("Dat Backup");//数据备份
                                SetMsgBoxMessage("Dat Recovery Completed!");//数据恢复成功!
                                sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回
                            
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowHelpToolMenuDlg();
                                }						
							}						
						}
						else
						{//提示是否覆盖
							DlgStatus = 2;
							WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							WM_Invalidate(hWin);
						}							
					}
					else
					{
                        MsgBoxInit();
                        SetMsgBoxCaption("Dat Backup");//数据备份
                        SetMsgBoxMessage("No Backup Dat!");//没有备份数据!
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回

                        ShowMsgBox();					
					}	
					break;					
				default:
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

void ShowDetBackupBlueTypeSelDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
