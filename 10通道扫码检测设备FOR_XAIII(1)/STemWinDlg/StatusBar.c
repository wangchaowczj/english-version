#include "includes.h"
#include "GuiIncludes.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmDiskIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmUSBIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmUSBErrorIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmUSBNonSupportIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmScan;


static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, STATUSBAR_HEIGHT, 0,          0 },
	{ BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 }
};
void WindowTitleUpdate(char* title)
{	
	memset(OpData.DlgTitle, 0, sizeof(OpData.DlgTitle));
	if(strlen(title) < sizeof(OpData.DlgTitle))
	{
		strcpy(OpData.DlgTitle, title);	
	}

	if(WM_IsWindow(DlghWin.Status))
	{
		WM_Paint(DlghWin.Status);
	}		
}

static void ShowBatteryState(u8 BatCapacity)
{
	u8 i, Number = 0;
		
	//根据电量转换成电量格数
	if(BatCapacity > 80)
	{
		Number = 5;
	}
	else if(BatCapacity > 60)
	{
		Number = 4;
	} 
	else if(BatCapacity > 40)
	{
		Number = 3;
	}
	else if(BatCapacity > 20)
	{
		Number = 2;
	}
	else if(BatCapacity > 5)
	{
		Number = 1;
	}
	else
	{
		Number = 0;
	}
	
	if(Number > 1)
	{
		GUI_SetColor(GUI_GREEN); 
	}
	else
	{
		GUI_SetColor(GUI_RED); 
	}	
	
	GUI_FillRect(4,8,34,21);
	GUI_FillRect(5,9,33,20);
	GUI_FillRect(35,12,36,17);//电池前边的正极标志


	GUI_SetColor(GUI_BLACK); 
	GUI_FillRect(6,10,32,19);

	if(Number > 1)
	{
		GUI_SetColor(GUI_GREEN); 
	}
	else
	{
		GUI_SetColor(GUI_RED); 
	}	
	
	for(i=0;i<Number;i++)
	{
		GUI_FillRect(8+5*i,11,10+5*i,18);	
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbStatus
*	功能说明: 窗口hStatusWin的回调函数
*	形    参：pMsg  指针地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void Callback(WM_MESSAGE * pMsg) 
{
	int xSize;
	static uint8_t TempStr[50];
	static WM_HTIMER hTimerTime;
	WM_HWIN hWin = pMsg->hWin;
	static u8 ForcePowerOff = 0;
    u8 IconX = 40;
    
	switch (pMsg->MsgId) 
	{			
		/* 窗口被删除的时候同时删除定时器 */
		case WM_DELETE:	
			WM_DeleteTimer(hTimerTime);
			break;
		case WM_TIMER:
			WM_InvalidateWindow(hWin);
			WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);	

			OpData.BatCapacity = GetBatteryCapacityByVoltage(GetBatteryDisplayValue(GetBatVoltage(NULL)));
			if(SystemSleepIsEnable())
			{
				if(SystemSleepCountIsTimeout(OSTimeGet()) )
				{
					SystemEnterSleepMode();
					SetSystemSleepCount(OSTimeGet());
					
					WM_HideWindow(DlghWin.Dlg);
					WM_ShowWindow(DlghWin.Dlg);				
				}
			}					
			else
			{
				SetSystemSleepCount(OSTimeGet());
			}
			if(WM_IsVisible(pMsg->hWin))
			{
				if((OpData.BatCapacity <= 3)&&(GET_AC_IN_STATE() == 0))
				{//电池电量太低时强制关机
					if(ForcePowerOff <= 10)
					{
						ForcePowerOff++;
						if(ForcePowerOff > 10)
						{
							ShowPowerOffDlg(1);	
						}					
					}					
				}
				else
				{
					ForcePowerOff = 0;
				}				
			}							
			break;
		case WM_INIT_DIALOG://初始化界面
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			WM_HideWindow(pMsg->hWin);
			
			OpData.BatCapacity = GetBatteryCapacityByVoltage(GetBatteryDisplayValue(InitBatteryDisplayValue(GetBatVoltage(NULL))));

			hTimerTime = WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);			
			break;
		case WM_PAINT:
			xSize = WM_GetWindowSizeX(hWin);
		
			GUI_SetColor(GUI_WHITE);
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
					
			GUI_SetFont(&NORMAL_FONT);			
			
			if(GET_AC_IN_STATE() == NOT_AC)
			{
				ShowBatteryState(OpData.BatCapacity);//显示电池电量              
			}
            else
            {
                IconX = 5;
            }
			
			GUI_SetColor(GUI_WHITE);
			GUI_SetBkColor(GUI_BLUE);		
			
			if(HostInfo.SupportDeviceType)
			{
				switch(GetHostEnumState())
				{
					case BUS_FREE:
					case WAITING_INSERT:
						break;
					case ENUM_ERROR_WAIING_LEAVE:
						GUI_DrawBitmap(&bmUSBErrorIcon,IconX,3);//枚举失败
						break;
					case DEVICE_TYPE_ERR:
						GUI_DrawBitmap(&bmUSBNonSupportIcon,IconX,3);//不支持的设备类型
						break;				
					case ENUM_SUCCESS:
						if(SUPPORT_DEVICE_MSC == HostInfo.DeviceType)
						{
							GUI_DrawBitmap(&bmDiskIcon,IconX,3);
						}
						else if(SUPPORT_DEVICE_KEYBOARD == HostInfo.DeviceType)
						{
							GUI_DrawBitmap(&bmScan,IconX,3);
						}						
						break;
					default:
						GUI_DrawBitmap(&bmUSBIcon,IconX,3);//正在识别插入的USB设备
						break;
				}			
			}
			else if(READ_USB_GET_IN_PIN() != USB_CONNET_DEVICE)
			{
				GUI_DrawBitmap(&bmUSBIcon,IconX,3);
			}			
			GUI_DispStringAt((char *)OpData.DlgTitle, xSize - GUI_GetStringDistX((const char *)OpData.DlgTitle) -110, 4);
			
			RTC_Read_Time();

			sprintf((char *)TempStr, "%02d:%02d:%02d", RTC_Type.hour, RTC_Type.minute, RTC_Type.second);
			GUI_DispStringAt((char *)TempStr, xSize - GUI_GetStringDistX((const char *)TempStr)-5, 4);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_POWER:	
					ShowPowerOffDlg(0);
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 显示状态栏
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ShowStatus(void)
{
	/* 创建一个状态窗口 */
	if(WM_IsWindow(DlghWin.Status))
	{
		GUI_EndDialog(DlghWin.Status, 0);
	}	
	DlghWin.Status = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
}
