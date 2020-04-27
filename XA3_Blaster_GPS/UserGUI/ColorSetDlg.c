#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
    { BUTTON_CreateIndirect,   "OK",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//确定
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO0,     30, 90, 17,  100,  0,          5 },
  { RADIO_CreateIndirect,   "",       GUI_ID_RADIO1,     145, 90, 17,  100,  0,          5 },//160,70
  { TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 190, 240,  20,  0,          0 }
};


/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	char temp[80];
	u32 BackgroundColor, TextColor;
	static RADIO_Handle CurrentRadio;
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);        
            GUI_Clear();
            ShowInfoTitle("Screen Color");//屏幕颜色

            if(CurrentRadio == WM_GetDialogItem(hWin, GUI_ID_RADIO0))
            {
                ShowInfoHelp("Setting The BG Color", " ↙ Switch Setting Item");//正在设置背景颜色 ↙键切换设置项
            }
            else
            {
                ShowInfoHelp("Setting The Text Color", " ↙ Switch Setting Item");//正在设置内容颜色  ↙键切换设置项
            }
            
            //RADIO字体设置有错位现象，采用变通方式，文字自绘
            //背景颜色
            //GUI_DispStringAt("B", 14,80);//背
            //GUI_DispStringAt("B", 14,100);//景
            //GUI_DispStringAt("G", 14,120);//颜
            //GUI_DispStringAt("色", 14,140);//色
						GUI_DispStringAt("Background", 30,65);
            GUI_DispStringAt("1.Default", 47,90);//1.默认
            GUI_DispStringAt("2.Black", 47,110);//2.黑色
            GUI_DispStringAt("3.White", 47,130);//3.白色
            GUI_DispStringAt("4.Blue", 47,150);//4.蓝色
            GUI_DispStringAt("5.Green", 47,170);//5.绿色
            
            //内容颜色
            //GUI_DispStringAt("内", 144,80);
            //GUI_DispStringAt("容", 144,100);
            //GUI_DispStringAt("颜", 144,120);
            //GUI_DispStringAt("色", 144,140);
						GUI_DispStringAt("Text", 145,65);
            GUI_DispStringAt("1.Red", 162,90);//1.红色  
            GUI_DispStringAt("2.Black", 162,110);//2.黑色
            GUI_DispStringAt("3.White", 162,130);//3.白色
            GUI_DispStringAt("4.Blue", 162,150);//4.蓝色
            GUI_DispStringAt("5.Green", 162,170);//5.绿色             
            break;
        case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);          
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO0);                   
            if(COLOR_DLG_BK == device_info.BackgroundColor)
            {
                RADIO_SetValue(hitem, 0);
            }
            else if(GUI_BLACK == device_info.BackgroundColor)
            {
                RADIO_SetValue(hitem, 1);
            }
            else if(GUI_WHITE == device_info.BackgroundColor)
            {
                RADIO_SetValue(hitem, 2);
            }
            else if(GUI_BLUE == device_info.BackgroundColor)
            {
                RADIO_SetValue(hitem, 3);
            }        
            else if(GUI_GREEN == device_info.BackgroundColor)
            {
                RADIO_SetValue(hitem, 4);
            }

            hitem = WM_GetDialogItem(hWin, GUI_ID_RADIO1);
                   
            if(GUI_RED == device_info.TextColor)
            {
                RADIO_SetValue(hitem, 0);
            }
            else if(GUI_BLACK == device_info.TextColor)
            {
                RADIO_SetValue(hitem, 1);
            }
            else if(GUI_WHITE == device_info.TextColor)
            {
                RADIO_SetValue(hitem, 2);
            }
            else if(GUI_BLUE == device_info.TextColor)
            {
                RADIO_SetValue(hitem, 3);
            }        
            else if(GUI_GREEN == device_info.TextColor)
            {
                RADIO_SetValue(hitem, 4);
            }
            
            CurrentRadio = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
            
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }		
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
            break;
        case WM_KEY:
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "");
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    ShowSystemSetMenuDlg();
                    break;
                case GUI_KEY_RIGHT:                    
                    BackgroundColor = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
                    if(BackgroundColor == 0)
                    {
                        BackgroundColor = COLOR_DLG_BK;
                    }
                    else if(BackgroundColor == 1)
                    {
                        BackgroundColor = GUI_BLACK;
                    }
                    else if(BackgroundColor == 2)
                    {
                        BackgroundColor = GUI_WHITE;
                    }
                    else if(BackgroundColor == 3)
                    {
                        BackgroundColor = GUI_BLUE;
                    }
                    else if(BackgroundColor == 4)
                    {
                        BackgroundColor = GUI_GREEN;
                    }
                    else
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls set background color");//请设置背景颜色
                        break;
                    }
                    TextColor = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO1));
                    if(TextColor == 0)
                    {
                        TextColor = GUI_RED;
                    }
                    else if(TextColor == 1)
                    {
                        TextColor = GUI_BLACK;
                    }
                    else if(TextColor == 2)
                    {
                        TextColor = GUI_WHITE;
                    }
                    else if(TextColor == 3)
                    {
                        TextColor = GUI_BLUE;
                    }
                    else if(TextColor == 4)
                    {
                        TextColor = GUI_GREEN;
                    }
                    else
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Pls set text color");//请设置内容颜色
                        break;
                    }
                    if(TextColor == BackgroundColor)
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "BG and text CLR can't be same");//背景与内容颜色不能相同
                        break;                       
                    }
                                      
                    device_info.BackgroundColor = BackgroundColor;
                    device_info.TextColor = TextColor;
                    SaveUserConfigParam();
                    
                    SetGuiDefaultColor(device_info.BackgroundColor, device_info.TextColor);
                    WM_Paint(hWin);                    
                    break; 
                case GUI_KEY_ENTER:
                    if (CurrentRadio == WM_GetDialogItem(hWin, GUI_ID_RADIO0))
                    {
                        CurrentRadio = WM_GetDialogItem(hWin, GUI_ID_RADIO1);
                    }
                    else
                    {
                        CurrentRadio = WM_GetDialogItem(hWin, GUI_ID_RADIO0);
                    }
                    WM_Paint(hWin); 
                    break;                
                case GUI_KEY_1:
                    RADIO_SetValue(CurrentRadio, 0);
                    break;				
                case GUI_KEY_2:
                    RADIO_SetValue(CurrentRadio, 1);
                    break;
                case GUI_KEY_3:
                    RADIO_SetValue(CurrentRadio, 2);				
                    break;
                case GUI_KEY_4:
                    RADIO_SetValue(CurrentRadio, 3);				
                    break;
                case GUI_KEY_5:
                    RADIO_SetValue(CurrentRadio, 4);				
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

void ShowColorSetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
