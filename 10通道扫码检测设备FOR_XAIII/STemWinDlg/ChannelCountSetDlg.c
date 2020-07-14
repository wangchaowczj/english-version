#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "1.单通道检测",       GUI_ID_TEXT0,     105, 50, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "2.五通道检测",        GUI_ID_TEXT1,     105, 80, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "3.十通道检测",       GUI_ID_TEXT2,     105, 110, LCD_WIDTH_PIXEL,  25,  0,          0 }
};



	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 U8Value;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();

			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("通道设置");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			
			RADIO_CreateEx(80,55,200,100,hWin,WM_CF_SHOW,0,GUI_ID_RADIO0,3,30);
			if(Channel.Count == 1)
			{
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);					
			}
			else if(Channel.Count == 5)
			{
				RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);					
			}            
            else
            {
                Channel.Count = 10;
                RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
            }			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(RESULT_YES == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
					{
                        u8 Count = 0;
                        
						switch(RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0)))
						{
							case 0:
								Count = 1;
								break;
							case 1:
								Count = 5;
								break;
							case 2:
								Count = 10;
								break;                                
                            default:
                                break;
						}
                        if(Count == 0)
                        {
                            break;
                        }
						Channel.Count = Count;					
						UTF8StringToGBK("通道数量", (char*)OpData.GuiTemp);
						if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, Channel.Count) == ERROR)
						{
							ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
						}						
						ShowSystemSetDlg();					
					}
					break;
				case GUI_CUSTOM_KEY_BACK:
                    U8Value = RADIO_GetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0));
                    if(U8Value == 0)
                    {
                        U8Value = 1;
                    }
                    else if(U8Value == 1)
                    {
                        U8Value = 5;
                    }                    
                    else
                    {
                        U8Value = 10;
                    }
					if(Channel.Count != U8Value)
					{
						if(RESULT_YES != ShowCustomMessageBox("确认取消修改?", OpData.DlgTitle, MB_YESNO))
						{
							break;
						}					
					}

					ShowSystemSetDlg();
					break;
				case GUI_CUSTOM_KEY_1:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 0);
					break;
				case GUI_CUSTOM_KEY_2:
					//允许VA三码绑定使用5通道
//                    if(UserParam.Stage == DATA_BINDING)
//                    {
//                        ShowCustomMessageBox("三码绑定不能是五通道", OpData.DlgTitle, MB_OK);
//                        break;
//                    }
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 1);
					break;
				case GUI_CUSTOM_KEY_3:	
					RADIO_SetValue(WM_GetDialogItem(hWin, GUI_ID_RADIO0), 2);
					break;                    
			}
			WM_SetFocus(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowChannelCountSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}

