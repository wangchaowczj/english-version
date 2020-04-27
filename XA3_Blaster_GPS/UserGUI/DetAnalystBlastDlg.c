#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ TEXT_CreateIndirect, "Press 1 and 3 to Blast", GUI_ID_TEXT0,     0, 100, 240,  50,  0,          0 }//请按1和3双键起爆
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];

	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("DET Analysis");//雷管分析
			ShowInfoHelp(" ", NULL);					
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			Op_Data.DoubleKeySwitch = 1;//允许打开双键
			
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://左键返回
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{
						CLOSE_HV_SW_INSIDE();
						OPEN_BUS_6_7V();
                        SetNormalHighVoltageGoal();
						OSTimeDly(10);
						AllDisCharge();
						ShowDetAnalystDlg();
					}															
					break;
				case GUI_KEY_DOUBLE:		
					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
					TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &BIG_FONT);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Blast");//起爆
					
					OPEN_BUS_6_7V();//把总线切到低压下再发送起爆信号
										
					CreateStartBlastTask();
					CLOSE_BUS_VOLTAGE();//起爆完成，关闭总线电压
					SaveBlasteFireTime();//保存起爆时间
					WriteSystemLog(NULL,0,BLAST_SINGLE_DATA);//保存起爆信号监测数据	

					Op_Data.DoubleKeySwitch = 0;	
							
					WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3000MS_ID, GUI_TIMER_3000MS, 0);					
					break;								
			}
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_3000MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &SMALL_FONT);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Blast Completed");	//起爆完毕
					WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
					break;
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

void ShowDetAnalystBlastDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


