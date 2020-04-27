#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     10, 45, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT1,     10, 70, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT2,     10, 95, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT3,     10, 120, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT4,     10, 145, 220,  25,  0,          0 },
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT5,     10, 170, 220,  25,  0,          0 },
    { TEXT_CreateIndirect, " ", GUI_ID_TEXT6,     10, 195, 220,  25,  0,          0 }	
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	char UFT8[100];
	u8 len;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Device Info");	//设备信息
			GUI_SetFont(&SMALL_FONT);
			
			//传给中爆网的ID，红色显示
			GUI_SetColor(device_info.TextColor);
			len = strlen((char*)&device_info.mac[0]);
			if((len == 11) && ((device_info.mac[0] == 'F') || (device_info.mac[0] == 'f')))
			{//如果是F4417102301 那么中爆网是44171001
				memset((char*)&temp[0],0, sizeof(temp));
				temp[0] = device_info.mac[0];
				GUI_DispStringAt((char*)&temp[0],82,74);

				GUI_SetColor(GUI_RED);
				memset((char*)&temp[0],0, sizeof(temp));
				memcpy((char*)&temp[0], (char*)&device_info.mac[1], 6);		
				GUI_DispString((char*)&temp[0]);

				GUI_SetColor(device_info.TextColor);
				memset((char*)&temp[0],0, sizeof(temp));
				memcpy((char*)&temp[0], (char*)&device_info.mac[7], 2);		
				GUI_DispString((char*)&temp[0]);

				GUI_SetColor(GUI_RED);
				memset((char*)&temp[0],0, sizeof(temp));
				memcpy((char*)&temp[0], (char*)&device_info.mac[9], 2);		
				GUI_DispString((char*)&temp[0]);				
			}
			else if(len > 8)
			{//后8位显示红色
				memset((char*)&temp[0],0, sizeof(temp));
				memcpy((char*)&temp[0], (char*)&device_info.mac[0], len-8);
				GUI_DispStringAt((char*)&temp[0],82,74);
				GUI_SetColor(GUI_RED);
				GUI_DispString((char*)&device_info.mac[len-8]);
			}
			else
			{
				GUI_DispStringAt((char*)&device_info.mac[0],82,74);				
			}
			GUI_SetColor(device_info.TextColor);
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);		//注册雷管:%u 错误雷管:			
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%u 错误雷管:	
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);					
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT4), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT5), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin , GUI_ID_TEXT6), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Dev Name:Xin An Control Box");//设备名称:信安控制盒
			//GBKStringToUTF8((char*)&device_info.mac[0],(char*)UFT8);
			//sprintf(temp, "设备编号:%s", UFT8);
			//TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),temp);
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"DEV ID  :");//设备编号:
			
			
			GBKStringToUTF8((char*)&device_info.hw_version[0],(char*)UFT8);
			sprintf(temp, "Firmware Ver:%s", UFT8);//硬件版本:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2),temp);
			
			sprintf(temp, "SW Ver:%s", VERSION_STRING);//软件版本:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3),temp);
			
			if(device_info.BlueVersion[0] == 0x00)
			{
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4),"Bluetooth Ver:Abnormal");//蓝牙版本:蓝牙异常
			}
			else
			{
				sprintf(temp, "Bluetooth Ver:V%s", (char*)&device_info.BlueVersion[0]);//蓝牙版本:V
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4),temp);						
			}
			
			if ((adc_adjust.bat.k == 0) && (adc_adjust.linab.k == 0) && (adc_adjust.low1current.k == 0) &&
				(adc_adjust.low2current.k == 0) && (adc_adjust.high1current.k == 0) &&
				(adc_adjust.high2current.k == 0))
			{
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT5),"Cali Date:uncalibrated");//标定时间:未标定						
			}
			else
			{
				memset(temp,0,sizeof(temp));
				strcpy((char*)&temp[0], "Cali Date:");//标定时间:
				if(ERROR == ReadSpecifiedFileData(ADJUST_FILE_PATH, ADJUST_TIME_FILE_NAME, (u8*)&temp[strlen((const char*)&temp[0])], 17))
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT5),"Cali Date:uncalibrated");//标定时间:未标定
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT5), temp);
				}							
			}
            memset((char*)&Op_Data.Temp[0], 0, sizeof(Op_Data.Temp));
            strcpy((char*)&Op_Data.Temp[0], "IP:");//IP地址:
            IPAddrU32ToString(device_info.Server.Ip, (u8*)&Op_Data.Temp[strlen((char*)&Op_Data.Temp)]);
            sprintf((char*)&Op_Data.Temp[strlen((char*)&Op_Data.Temp)],":%d",device_info.Server.Port);            
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT6), (char*)&Op_Data.Temp[0]);

			CLOSE_BLUE_POWER();

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
					ShowInfoManageDlg();
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

void ShowDeviceInfoDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
