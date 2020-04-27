#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "返回",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     10, 50, 220,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     10, 70, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     10, 90, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     10, 110, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT4,     10, 130, 220,  20,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	GPS_INFO GPSInfo;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("定位信息");	
			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "在线译码器:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);
				sprintf(&temp[40], "注册雷管:%u 错误雷管:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);					
			}
			else
			{
				sprintf(&temp[0], "注册雷管:%d 错误雷管:%d",Det.RegCount, Det.ErrorCount);
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);					
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            
            if(GetGPSPassCount() > 5)
            {
                SetGPSPassCount(5);//避免后台自动关掉定位
            }
            if(!GPSIsWorking())
            {
                ReceiveGPSInfoStart();
            }
            
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"定位中...");
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1),"");
            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2),"");

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1000MS_ID, GUI_TIMER_10MS, 0);
            
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
                    if(GetLastGPSInfo(&GPSInfo))
                    {//已经定位成功
                        if(GPSIsWorking()) 
                        {
                            SaveGPSInfo(&GPSInfo);
                            
                            ReceiveGPSInfoStop();
                        }
                    }
					ShowHelpToolMenuDlg();
					break;
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
                    UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
					break;
				case GUI_TIMER_1000MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_1000MS);

                    if(GetGPSPassCount() > 5)
                    {
                        SetGPSPassCount(5);//避免后台自动关掉定位
                    }
                    sprintf(temp, "卫星数量:%d", GetSatelliteCount());
                    TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), temp);
                    
                    if(GetCurrentGPSInfo(&GPSInfo))
                    {                            
                        sprintf(temp, "经度:%.5f", GPSFormatConvert(GPSInfo.Longitude, sizeof(GPSInfo.Longitude)));
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), temp);
                        
                        sprintf(temp, "纬度:%.5f", GPSFormatConvert(GPSInfo.Latitude, sizeof(GPSInfo.Latitude)));
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), temp);
                        
                        sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d",
                            GPSInfo.Year,GPSInfo.Month,GPSInfo.Day,
                            GPSInfo.Hour,GPSInfo.Minute,GPSInfo.Second);
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4), temp);
                    }
                    else
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), "经度:");
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "纬度:");
                        TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4), "");
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

void ShowGpsRealtimeDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
