#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     0, 20, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "电压允许偏差:±100mV",       GUI_ID_TEXT1,     0, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "电流允许偏差:±1uA  ",       GUI_ID_TEXT2,     0, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     0, 110, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "方向键切换电压状态",       GUI_ID_TEXT4,     0, 140, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "数字键切换通道",       GUI_ID_TEXT5,     0, 170, LCD_WIDTH_PIXEL,  25,  0,          0 }
};


/**********************************************************
对话框参数:
高4位，0-9  表示 要检验的通道号
低4位，0表示检验电压，1表示检测频率
***********************************************************/
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u16 Voltage, Current;
	static u8 Channel = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("设备检验");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			OPEN_PW_5V();
			SET_LIN_OPEN();
			CLOSE_HV_SW();				
			OPEN_PW_LV();
			OPEN_PW_HV();
			
			Channel = 0;//默认1通道
			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			WM_SetFocus(hWin);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);	
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:					
					OpData.OutputWareFlag = 0;
					OSTimeDly(5);
					CLOSE_HV_SW();
					OPEN_PW_LV();
					ShowDeviceDemarcateMenuDlg();
					break;	
				case GUI_CUSTOM_KEY_F1:
				case GUI_CUSTOM_KEY_F2:
				case GUI_CUSTOM_KEY_F3:
				case GUI_CUSTOM_KEY_F4:
					if((GetWindowParam()&0x0F) == 0)
					{
						if(GET_HV_SW_STATUS() == 0)
						{//当前为低压状态
							OPEN_HV_SW();
						}
						else
						{
							CLOSE_HV_SW();
						}									
					}
					break;
				case GUI_CUSTOM_KEY_0:
					Channel = 9;//10通道
					WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
					break;
				case GUI_CUSTOM_KEY_1:
				case GUI_CUSTOM_KEY_2:
				case GUI_CUSTOM_KEY_3:
				case GUI_CUSTOM_KEY_4:
				case GUI_CUSTOM_KEY_5:
				case GUI_CUSTOM_KEY_6:
				case GUI_CUSTOM_KEY_7:
				case GUI_CUSTOM_KEY_8:
				case GUI_CUSTOM_KEY_9:
					Channel = ((WM_KEY_INFO*)(pMsg->Data.p))->Key - GUI_CUSTOM_KEY_1;
					WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					sprintf((char*)OpData.GuiTemp, "正在检验通道%d", 1+Channel);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)OpData.GuiTemp);	
					ConnectChannel(Channel);
					if(GetWindowParam() == 1)
					{						
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "正在输出方波(500Hz)");
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "频率允许偏差:±0.5Hz ");
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT4));
						OpData.OutputWareFlag = 1;
						SET_LINA_L();//该管脚保持低电平，另一管脚，跳变。可用示波器直接测
					}				
					break;
				case GUI_TIMER_500MS_ID:
					if(GetWindowParam() == 0)
					{
						Voltage = GetLinABVoltage(Channel,NULL);
						Current = GetCurrentValue(10,NULL);				
						sprintf((char*)OpData.GuiTemp, "电压:%0.2fV  电流:%0.2fuA", Voltage/1000.0, Current/100.0);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);		
						
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);					
					}
					else
					{
						WM_DeleteTimer(pMsg->Data.v);
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

void ShowDeviceDemarcateDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

