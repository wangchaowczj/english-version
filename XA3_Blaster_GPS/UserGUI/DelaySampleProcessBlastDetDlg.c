#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "Blast", GUI_ID_TEXT0,     0, 100, 240,  50,  0,          0 },//起爆
	{ TEXT_CreateIndirect, "Blast completed,back!", GUI_ID_TEXT1,     0, 90, 240,  25,  0,          0 }//起爆完毕,返回继续抽检!
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

			ShowInfoTitle("Blast Progress");//起爆流程	

			sprintf(&temp[0], "Delaytime:%dms", Op_Data.DelaySampleDelay);//延期:
			GetDetInfoString(&temp[40], Det.Type);							

			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);
			break;
		case WM_INIT_DIALOG://初始化界面
			Op_Data.BlastStatus = DET_BLASTING;
            Op_Data.OfflineMonitorBlastProcess = 0;
					
			TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &BIG_FONT);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3000MS_ID, GUI_TIMER_3000MS, 0);	
			
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);										
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:					
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{
						OPEN_BUS_6_7V();
						OSTimeDly(50);
						DeleteAllDetInfo();						
						ShowDelaySampleProcessWaitRegDetDlg();				
					}
					break;
			}
			break;			
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					OPEN_BUS_6_7V();//把总线切到低压下再发送起爆信号
										
					System168MDelay1ms(20);
					CreateStartBlastTask();
					CLOSE_BUS_VOLTAGE();//起爆完成，关闭总线电压
					SaveBlasteFireTime();//保存起爆时间
					WriteSystemLog(NULL,0,BLAST_SINGLE_DATA);//保存起爆信号监测数据	
					Op_Data.DoubleKeySwitch = 0;
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
					break;
				case GUI_TIMER_3000MS_ID:
					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
					break;					
				default:
					break;
			}		
			break;	
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDelaySampleProcessBlastDetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
