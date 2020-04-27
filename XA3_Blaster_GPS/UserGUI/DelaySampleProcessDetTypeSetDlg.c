#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "返回",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "确定",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     40, 112, 17,  80,  0,          2 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
	case WM_PAINT://绘制界面
		GUI_Clear();
		ShowInfoTitle("雷管类型");
		sprintf(&temp[0], "抽检数量:%d",Op_Data.DelaySampleDetCount);
		ShowInfoHelp(&temp[0], "  键修改抽检数量");
				
		ShowEnterIcon((LCDP_MAX_X-GetUtf8StrCharsCount("  键修改抽检数量")*8)/2+8*0, INFO_HELP_Y2);
			
		//RADIO字体设置有错位现象，采用变通方式，文字自绘
		GUI_DispStringAt("1.XA-PD1(0-6000ms)", 57,110);
		GUI_DispStringAt("2.XA-PD2(0-21000ms)", 57,130);
		break;
	case WM_INIT_DIALOG://初始化界面
		hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);

		if((Op_Data.DelaySampleDetCount == 0) || (Op_Data.DelaySampleDetCount > 30))
		{
			Op_Data.DelaySampleDetCount = 1;
		}
		
		if(Det.RegCount > 0)
		{
			DeleteAllDetInfo();
		}
		
		Op_Data.DelaySampleDelay = 0;//默认瞬发
		
		RegHoleInit(&Op_Data.RegDetInfo.Hole);//初始化待注册孔
		//根据待注册孔选择孔网参数	
		RegDetNetParam.StartHole.Name = Op_Data.RegDetInfo.Hole.Name;
		RegDetNetParam.StartHole.Inside = 1;
		RegDetNetParam.StartDly = Op_Data.DelaySampleDelay;	
		
		RADIO_SetTextColor(hitem,device_info.TextColor);
		RADIO_SetBkColor(hitem, device_info.BackgroundColor);
		if(Det.Type == DET_TYPE_XA_PD1)
		{
			RADIO_SetValue(hitem, 0);
		}
		else if(Det.Type == DET_TYPE_XA_PD2)
		{
			RADIO_SetValue(hitem, 1);
		}
			
		WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
	
		sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
		BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{
			case GUI_KEY_LEFT:
				SetMainMenuDlgParam(0);
				ShowMainMenuDlgWithParam();	
				break;
			case GUI_KEY_RIGHT:
				switch(RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
				{
					case 0:
						Det.Type = DET_TYPE_XA_PD1;
						break;
					case 1:
						Det.Type = DET_TYPE_XA_PD2;
						break;
					default:
						return;
				}
				if(ERROR != SaveUserConfigParam())
				{							
					Det.DlyMax = GetDetMaxDelay(Det.Type);
										
					Op_Data.DelaySampleFlag = 1;					

					if(Op_Data.LinABCurrent < 10)
					{
						SetMessageBoxDlgTitle("雷管抽检");
						SetMessageBoxDlgMsg("请确认抽检设备是否准备完毕!");
						SetMessageBoxDlgHelp1(" ");
						SetMessageBoxDlgBtLeftText("返回");
						SetMessageBoxDlgBtLeftFun(&ShowMainMenuDlgWithParam);
						SetMessageBoxDlgBtRightText("确定");
						SetMessageBoxDlgBtRightFun(&ShowDelaySampleProcessWaitRegDetDlg);						
						ShowMessageBoxDlg();					
					}
					else
					{
						ShowDelaySampleProcessWaitRegDetDlg();	
					}																		
				}
				else
				{
					SetMessageBoxDlgTitle("设备异常");
					SetMessageBoxDlgMsg("数据保存失败!");
					SetMessageBoxDlgHelp1("请检查SD卡是否安装可靠");
					ShowMessageBoxDlg();						
				}	
				break;				
			case GUI_KEY_1:
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
				break;				
			case GUI_KEY_2:
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
				break;
			case GUI_KEY_ENTER:
				ShowDelaySampleProcessDetCountInputDlg();
				break;
			default:
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

void ShowDelaySampleProcessDetTypeSetDlg(void)
{
	if(WM_IsWindow(Op_Data.hWin))
	{
		GUI_EndDialog(Op_Data.hWin, 0);
	}	
	Op_Data.hWin = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
