#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 30, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 60, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT2,     0, 90, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT3,     0, 120, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT4,     0, 150, LCD_WIDTH_PIXEL,  30,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT5,     0, 0, LCD_WIDTH_PIXEL,  30,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
	u8 showcode[14];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("雷管分析");
			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_HCENTER|TEXT_CF_VCENTER);		
			
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "雷管分析中...");					
		
			OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
			TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)3);//雷管分析任务
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:
					if(!TaskIsExist(APP_DET_DETECTOR_TASK_PRIO))
					{
						ShowSystemToolDlg();
					}
					break;
				case GUI_CUSTOM_KEY_OK:
				case GUI_CUSTOM_KEY_FOOT:
					if(!TaskIsExist(APP_DET_DETECTOR_TASK_PRIO))
					{
						ShowDetAnalystResultDlg();
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
					
					if(OpData.DetDetectorTaskResult&0x8000)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), " ");				
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "雷管短路!");
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), " ");
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), " ");					
					}
					else if(OpData.DetDetectorTaskResult&0x4000)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), " ");				
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "雷管断路!");
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), " ");
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), " ");					
					}					
					else
					{
						if(OpData.DetDetectorTaskResult&0x0001)
						{//序列码回读正常						
							sprintf((char*)OpData.GuiTemp," 雷管内码:%02X%02X%02X%02X%02X%02X%02X%02X",
								Channel.XA2_Info[0].Serl.U8Buffer[0],Channel.XA2_Info[0].Serl.U8Buffer[1],Channel.XA2_Info[0].Serl.U8Buffer[2],
								Channel.XA2_Info[0].Serl.U8Buffer[3],Channel.XA2_Info[0].Pwd.U8Buffer[0],Channel.XA2_Info[0].Pwd.U8Buffer[1],
								Channel.XA2_Info[0].Pwd.U8Buffer[2],Channel.XA2_Info[0].Pwd.U8Buffer[3]);	
								
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);
							if(XA2_DetInside2Outside(Channel.XA2_Info[0].Serl.U32Value,Channel.XA2_Info[0].Pwd.U32Value,&showcode[0]) == ERROR)
							{
								sprintf((char*)OpData.GuiTemp," 雷管外码:外码转换失败!");	
							}
							else
							{
								sprintf((char*)OpData.GuiTemp," 雷管外码:%s",&showcode[0]);	
							}
							
								
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_LEFT|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), (char*)OpData.GuiTemp);
							
							if(OpData.DetDetectorTaskResult&0x0002)
							{//测频正常
								sprintf((char*)OpData.GuiTemp," 频率:%ldHz 电流:%.2fuA",Channel.XA2_Info[0].freq, Channel.XA2_Info[0].lv_current/100.0);
							}
							else
							{
								sprintf((char*)OpData.GuiTemp," 频率:测量失败 电流:%.2fuA" ,Channel.XA2_Info[0].lv_current/100.0);							
							}
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)OpData.GuiTemp);
							
							if(OpData.DetDetectorTaskResult&0x0004)
							{//BIT测试正常
								strcpy((char*)&OpData.GuiTemp[0], " BIT测试:成功 ");
							}
							else
							{
								strcpy((char*)&OpData.GuiTemp[0], " BIT测试:失败 ");
							}					
							
							if(OpData.DetDetectorTaskResult&0x0010)
							{//密码
								strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "密码:正确 ");
							}
							else
							{
								strcpy((char*)&OpData.GuiTemp[strlen((char*)OpData.GuiTemp)], "密码:错误 ");
							}
							
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);
							
							if(OpData.DetDetectorTaskResult&0x0020)
							{//充电正常
								sprintf((char*)OpData.GuiTemp," 充电:成功    电流:%.2fuA", Channel.XA2_Info[0].hv_current/100.0);
							}
							else
							{
								sprintf((char*)OpData.GuiTemp," 充电:失败    电流:%.2fuA", Channel.XA2_Info[0].hv_current/100.0);
							}
							
							TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);
						}
						else
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), " ");				
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "雷管内码回读失败!");
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), " ");
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), " ");
						}					
					}
					break;			
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					if(TaskIsExist(APP_DET_DETECTOR_TASK_PRIO))
					{
						UpdateTextPointCount(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
					}
					else
					{
                        ErrorStatus Result;
                        
						ConnectChannel(0);
						Voltage = GetLinABVoltage(0, NULL);
						Current = GetCurrentValue(10,NULL);				
						sprintf((char*)OpData.GuiTemp, "电压:%0.1fV  电流:%0.2fuA", Voltage/1000.0, Current/100.0);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);							
						
                        Result = XA2_DetCheckIn();
						if(Result != ERROR)
						{
							ShowDetAnalystResultDlg();
						}						
					}
					break;				
				default:
					break;
			}		
			break;
		case WM_DET_TASK:			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);			
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetAnalystResultDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}
