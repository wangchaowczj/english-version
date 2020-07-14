#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, 0,          0, 1 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     0, 100, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



/**********************************************************
对话框参数:
0 正常手动关机
1 电池电量不足   
***********************************************************/
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;

	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();

			break;
		case WM_INIT_DIALOG://初始化界面	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
//			SaveDetTestQueueInfo();
			UserParam.DetDetectorPassCount = OpData.DetDetectorPassCount;
			UTF8StringToGBK("历史检测数量", (char*)OpData.GuiTemp);						
			if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetDetectorPassCount) == ERROR)
			{
				ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
				break;
			}
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3000MS_ID, GUI_TIMER_3000MS, 0);
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					if(GetWindowParam() == 0)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "正在关机,谢谢使用!");
					}
					else
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "电量不足,正在关机!");
					}
					break;
				case GUI_TIMER_3000MS_ID:
					LcdSetBright(0);		   //关显示
					POWER_OFF();
					OSTimeDlyHMSM(0,0,0,100);
					POWER_ON();
					__disable_irq();
					__set_FAULTMASK(1);//关闭所有中断
					NVIC_SystemReset();//软件复位
					while(1);			
//					break;
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowPowerOffDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
    SetWindowParam(Param);
}

