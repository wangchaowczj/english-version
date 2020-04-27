#include "includes.h"
#include "GuiIncludes.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmLogo00;
extern GUI_CONST_STORAGE GUI_BITMAP bmLogo34;

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 320, 0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 60, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 90, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 120, 240,  25,  0,          0 }
};
void ShowLogo(void)
{

    if((device_info.mac[0] == 'f') || (device_info.mac[0] == 'F'))
    {
        if((device_info.mac[1] == '3') && (device_info.mac[2] == '4'))
        {
            GUI_DrawBitmap(&bmLogo34, (240-bmLogo34.XSize)/2,160+(45-bmLogo34.YSize)/2);
            GUI_DispStringHCenterAt((const char*)"Jiangxi Xinyu Guotai", 120,210);//鲲程电子//江西新余国泰
            GUI_DispStringHCenterAt((const char*)"Chemical Industry Co.,Ltd", 120,240);//鲲程电子//特种化工有限责任公司
            return;
        }
    }
    GUI_DrawBitmap(&bmLogo00, (240-bmLogo00.XSize)/2,160+(45-bmLogo00.YSize)/2);
    GUI_DispStringHCenterAt((const char*)"SHKC Electronic", 120,210);//鲲程电子//上海鲲程
    GUI_DispStringHCenterAt((const char*)"Technology Co.,Ltd", 120,240);//鲲程电子//电子科技有限公司
	
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 Result = 0;	
	char temp[50];
	static u8 PaintLogo = 0;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面  
            GUI_SetColor(GUI_BLUE);
            GUI_SetBkColor(COLOR_DLG_BK); 
			GUI_Clear();
            if(PaintLogo)
            {
                GUI_SetFont(&NORMAL_FONT);
                ShowLogo();           
            }            
			break;
		case WM_INIT_DIALOG://初始化界面 
            PaintLogo = 0;
            
            TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0),&NORMAL_FONT);   
            TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), COLOR_DLG_BK);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_BLUE);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Welcome");//欢迎使用信安控制盒
            
            TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1),&NORMAL_FONT);
            TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), COLOR_DLG_BK);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_BLUE);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
            TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT2),&NORMAL_FONT);
            TEXT_SetBkColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2), COLOR_DLG_BK);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2), GUI_BLUE);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            WM_Paint(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
            WM_Paint(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
            WM_Paint(WM_GetDialogItem(hWin, GUI_ID_TEXT2));
			
			WM_Paint(hWin);
			
			LcdSetBright(LCD_BRIGHT_RANK);		//屏幕亮度设到最暗
			Result = SystemHardwareInit();
			if(0 == Result)
			{//初始化正常
				if(DEVICE_TYPE_SAFE_MONITOR == device_info.DeviceType)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");//安管版——不显示
				}
				else if(DEVICE_TYPE_SAFE_MONITOR_OFFLINE == device_info.DeviceType)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");//离线起爆版——不显示
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "");//密钥版——不显示
				}
				sprintf(temp,"%04d-%02d-%02d", RTC_Type.year,RTC_Type.month,RTC_Type.day);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), temp);
				                
				WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);
				
				//HID从机通信任务
				TaskCreate(APP_USB_TASK_PRIO, (void*)0);	
				TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)4);	
                
                PaintLogo = 1;
                WM_Paint(hWin);
			}
			else 
			{
				TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0),&SMALL_FONT);
				TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1),&SMALL_FONT);
				if(1 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Clock Abnormal!");//RTC时钟异常!
				}
				else if(2 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls reformat the memory chip!");//请重新格式化存储芯片!			
				}				
				else if(3 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls insert the TF card!");	//请插入TF卡!		
				}
				else if(4 == Result)
				{
//					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "请重新格式化TF卡!");
                    ShowFileSystemFormatDlg();
				}
				else if(5 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "PMIC Abnormal!");//电源管理芯片异常!
					
					UTF8StringToGBK("开机:电源管理芯片异常!", (char*)Op_Data.Temp);
					WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);					
				}
				else if(6 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "TF card");	//TF卡读写异常!		
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Read and write Abnormal!");	//TF卡读写异常!	
				}
				else if(7 == Result)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Memory card");	//存储芯片读写异常!		
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Read and write Abnormal!");	//存储芯片读写异常!
				}
                else if(8 == Result)
				{
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Software Version ERR!");//软件版本错误!
				}
				else
				{
					sprintf(temp,"Device Abnormal(%d)", Result);//设备异常
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), temp);
					
					UTF8StringToGBK(temp, (char*)Op_Data.Temp);
					WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);					
				}
			}
            SetGuiDefaultColor(device_info.BackgroundColor, device_info.TextColor);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_RIGHT:
					break;										
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_10MS_ID:								
					if(!TaskIsExist(APP_TEMPORARY_TASK_PRIO))
					{
						if(WM_IsWindow(DlghWin.Status))
						{
							WM_ShowWindow(DlghWin.Status);
						}					
						Result = Op_Data.AppTemporaryTaskResult;
						if(Result == 0)
						{
							WriteSystemLog(NULL,0, DEV_START);
							
							ShowMainMenuDlg(0);	
						}
						else
						{
                            WM_MoveWindow(hWin, 0, TOP_TITLE_HEIGHT);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Device Abnormal");//设备异常
                            
							if(1 == Result)
							{
								strcpy((char*)temp, "Bluetooth Abnormal!");//蓝牙模块异常!
							}
							else if(2 == Result)
							{
								strcpy((char*)temp, "WIFI Abnormal!");//WIFI模块异常!
							}
							else if(3 == Result)
							{
								strcpy((char*)temp, "Scanning Abnormal!");//扫码模块异常!
							}							
							else
							{
								sprintf((char*)temp,"Abnormal Code:%d", Result);//异常代码:
							}
                            
							SetMsgBoxMessage(temp);
							SetMsgBoxRight("OK");//确定
                            if(ShowMsgBox() == GUI_KEY_RIGHT)
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
				default:
					break;
			}		
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDeviceStartDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
