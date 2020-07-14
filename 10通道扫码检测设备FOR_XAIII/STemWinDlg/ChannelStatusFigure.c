#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0, 1 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "F1键查看错误信息", GUI_ID_TEXT0,     0, 155, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect, "检测完成,请取下所有雷管", GUI_ID_TEXT1,     0, 180, LCD_WIDTH_PIXEL,  25,  0,          0 }
};

static void ShowChannelStatusFigure(WM_HWIN hWin)
{
	u8 i;
	u16 x, y;
	u8 space = 0;
	u8 r = 20;
	GUI_COLOR  Color, OldBkColor, OldColor;
	
	WM_SelectWindow(hWin);	
	
	OldColor = GUI_GetColor();
	OldBkColor = GUI_GetBkColor();
	
	GUI_SetFont(&NORMAL_FONT);
	if (Channel.Count <= 5)
	{
		space = (LCD_WIDTH_PIXEL - Channel.Count*r*2)/(Channel.Count+1);
		y = LCD_HEIGHT_PIXEL/2 - STATUSBAR_HEIGHT;
		for(i = 0; i < Channel.Count;i++)
		{
			x = space+r+(space+2*r)*i;
			
			if(DET_TESTING == Channel.XA2_Info[i].Status)
			{//正在检测
				Color = GUI_BLUE;
			}
			else if (DET_PASS == Channel.XA2_Info[i].Status)
			{//雷管正常
				Color = GUI_GREEN;
			}
			else
			{//雷管异常
				Color = GUI_RED;
			}
			GUI_SetColor(Color);
			GUI_FillCircle(x, y, r);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(Color);

			sprintf((char*)OpData.GuiTemp,"%d",i+1);
			GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);
//			GUI_SetColor(GUI_BLACK);
//			GUI_SetBkColor(OldBkColor);	
			if(i%2 == 0)
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTYELLOW);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTGREEN);
			}			
			if ((DET_PASS == Channel.XA2_Info[i].Status) && ((UserParam.DetCodeWrite == 1) || (UserParam.Stage == FINISHED)))
			{
				sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[i].CodeExternal[8]);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的外码后五位	
			}
			else
			{
				GUI_DispStringHCenterAt("     ", x, y+r+2);
			}			
		}		
	}
	else
	{
		space = (LCD_WIDTH_PIXEL - 5*r*2)/6;
		y = (LCD_HEIGHT_PIXEL-30-space*2-4*r)/2+r - STATUSBAR_HEIGHT;
		
		for(i = 0; i < 5; i++)
		{
			x = space+r+(space+2*r)*i;
			
			if(DET_TESTING == Channel.XA2_Info[i].Status)
			{//正在检测
				Color = GUI_BLUE;
			}
			else if (DET_PASS == Channel.XA2_Info[i].Status)
			{//雷管正常
				Color = GUI_GREEN;
			}
			else
			{//雷管异常
				Color = GUI_RED;
			}
			GUI_SetColor(Color);
			GUI_FillCircle(x, y, r);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(Color);
			
			sprintf((char*)OpData.GuiTemp,"%d",i+1);
			GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);
//			GUI_SetColor(GUI_BLACK);
//			GUI_SetBkColor(OldBkColor);	
			if(i%2 == 0)
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTYELLOW);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTGREEN);
			}			
			if ((DET_PASS == Channel.XA2_Info[i].Status)&& ((UserParam.DetCodeWrite == 1) || (UserParam.Stage == FINISHED)))
			{
				sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[i].CodeExternal[8]);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的外码后五位	
			}
			else
			{
				GUI_DispStringHCenterAt("     ", x, y+r+2);
			}			
		}
		y += space*2+2*r-10;		
		for(i = 0; i < Channel.Count-5;i++)
		{
			x = space+r+(space+2*r)*i;
			
			if(DET_TESTING == Channel.XA2_Info[5+i].Status)
			{//正在检测
				Color = GUI_BLUE;
			}
			else if (DET_PASS == Channel.XA2_Info[5+i].Status)
			{//雷管正常
				Color = GUI_GREEN;
			}
			else
			{//雷管异常
				Color = GUI_RED;
			}
			GUI_SetColor(Color);
			GUI_FillCircle(x, y, r);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(Color);
			
			sprintf((char*)OpData.GuiTemp,"%d",i+6);
			GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y-GUI_GetFontSizeY()/2);		
//			GUI_SetColor(GUI_BLACK);
//			GUI_SetBkColor(OldBkColor);	
			if(i%2 == 0)
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTYELLOW);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				GUI_SetBkColor(GUI_LIGHTGREEN);
			}			
			if ((DET_PASS == Channel.XA2_Info[i+5].Status) && ((UserParam.DetCodeWrite == 1) || (UserParam.Stage == FINISHED)))//写码或者成品检才会显示管码
			{
				sprintf((char*)OpData.GuiTemp, "%s", &Channel.BarCode[i+5].CodeExternal[8]);
				GUI_DispStringHCenterAt((char*)OpData.GuiTemp, x, y+r+2);//显示扫码得到的外码后五位	
			}
			else
			{
				GUI_DispStringHCenterAt("     ", x, y+r+2);
			}
		}
	}
	GUI_SetBkColor(OldBkColor);
	GUI_SetColor(OldColor);
}

/**********************************************************
对话框参数:
0 各通道正在测试雷管，显示测试状态
1 从错误列表中返回  
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
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
            
            OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
            
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS,0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{	
				case GUI_CUSTOM_KEY_BACK:
					if (OpData.TestStatus == TEST_WAIT_OFFLINE)
					{//已经检测完，正在等待所有雷管下线
						OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
						ShowChannelTestPrepareDlg();
					}
					break;
				case GUI_CUSTOM_KEY_F1:
					if (OpData.TestStatus == TEST_WAIT_OFFLINE)
					{
						if(GetChannelOldStatusErrorCount() != 0)
						{//有错误雷管时可查看错误雷管详细信息
							OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
							ShowDetTestErrorListDlg();
						}					
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
                    GetStageDesString((char*)OpData.GuiTemp);
                    WindowTitleUpdate((char*)OpData.GuiTemp);

					WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
					if(GetWindowParam() == 0)
					{
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
                        
                        OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);                        
                        TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)1);//开始检测雷管 
					}					
					else
					{//从错误列表中返回
						WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
						WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
                        
						OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
						TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)2);//等待所有雷管下线							
					}
																									
					WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_200MS_ID, GUI_TIMER_5MS,0);
					CloseScanKeyBoard();//不允许扫码枪正常扫码
					break;
				case GUI_TIMER_2MS_ID://检测结束				
					WM_DeleteTimer(pMsg->Data.v);
                    WriteDetTestRecord();//保存测试结果
                    
                    OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
                    
                    TaskCreate(APP_DET_DETECTOR_TASK_PRIO, (void*)2);//等待所有雷管下线	
                    if(GetChannelOldStatusErrorCount() != 0)
                    {
                        WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
                    }
                    WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
					break;
				case GUI_TIMER_3MS_ID:	//所有雷管都已经下线			
					WM_DeleteTimer(pMsg->Data.v);
					OSTaskDel(APP_DET_DETECTOR_TASK_PRIO);
					ShowChannelTestPrepareDlg();
					break;                    
				case GUI_TIMER_200MS_ID:
					ShowChannelStatusFigure(hWin);
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_200MS);					
					break;					
			}
			break;
		case WM_DET_TASK://检测结束
            if(OpData.TestStatus != TEST_WAIT_OFFLINE)
            {//检测结束
                OpData.TestStatus = TEST_WAIT_OFFLINE;
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS,0);            
            }
            else
            {//雷管已经下线
                WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3MS_ID, GUI_TIMER_3MS,0); 
            } 			
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowChannelStatusFigureDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

