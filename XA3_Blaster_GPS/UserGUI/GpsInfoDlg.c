#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "返回",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect,   "定位",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     10, 70, 220,  20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     10, 95, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     10, 120, 220,  20,  0,          0 },
    { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     10, 145, 220,  20,  0,          0 }
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
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

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
					ShowHelpToolMenuDlg();
					break;
				case GUI_KEY_RIGHT:
					ShowGpsRealtimeDlg();
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
				case GUI_TIMER_1000MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_1000MS);
                    
                    if ((DEVICE_USAGES_SLAVE == device_info.DeviceUsages) || 
                        (DEVICE_TYPE_SAFE_MONITOR_OFFLINE != device_info.DeviceType))
                    {
                        if ((ReadOldGPSInfo(&Op_Data.Gps) != ERROR) && GPSInfoTimeIsPass(&Op_Data.Gps))
                        {                            
                            sprintf(temp, "经度:%.5f", GPSFormatConvert(Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude)));
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), temp);
                            
                            sprintf(temp, "纬度:%.5f", GPSFormatConvert(Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude)));
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), temp);
                            
                            sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d",
                                Op_Data.Gps.Year,Op_Data.Gps.Month,Op_Data.Gps.Day,
                                Op_Data.Gps.Hour,Op_Data.Gps.Minute,Op_Data.Gps.Second);
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), temp);
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "");
                        }
                        else
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "没有定位数据!");
                        }
                    }
                    else
                    {
                        if ((ReadOldGPSInfo(&Op_Data.Gps) != ERROR) && GPSInfoTimeIsPass(&Op_Data.Gps))
                        {                            
                            sprintf(temp, "经度:%.5f", GPSFormatConvert(Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude)));
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), temp);
                            
                            sprintf(temp, "纬度:%.5f", GPSFormatConvert(Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude)));
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), temp);
                            
                            sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d",
                                Op_Data.Gps.Year,Op_Data.Gps.Month,Op_Data.Gps.Day,
                                Op_Data.Gps.Hour,Op_Data.Gps.Minute,Op_Data.Gps.Second);
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), temp);
                            

                            if(HasAccreditInfoFileForDL())
                            {
                                if(AccreditAreaVerifyForDL())
                                {
                                    TEXT_SetTextColor(WM_GetDialogItem(hWin , GUI_ID_TEXT3), GUI_BLUE);
                                    TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "在准爆区内!");
                                }
                                else
                                {
                                    TEXT_SetTextColor(WM_GetDialogItem(hWin , GUI_ID_TEXT3), GUI_RED);
                                    TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "不在准爆区内!");
                                }
                            }
                            else
                            {
                                TEXT_SetTextColor(WM_GetDialogItem(hWin , GUI_ID_TEXT3), GUI_RED);
                                TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "没有授权文件!");
                            }
                        }
                        else
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), "");
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), "没有定位数据!");
                        }
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

void ShowGpsInfoDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
