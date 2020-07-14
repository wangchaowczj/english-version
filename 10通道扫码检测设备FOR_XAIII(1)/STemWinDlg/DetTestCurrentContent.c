#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT0,     10, 35, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT1,     10, 60, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT2,     10, 85, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT3,     10, 110, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT4,     10, 135, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "",       GUI_ID_TEXT5,     10, 160, LCD_WIDTH_PIXEL,  25,  0,          0 }  
 
};


static const char* GetDetTypeDes(u8 DetType)
{
    switch(DetType)
    {
        case DET_TYPE_XA_02:
            return "芯片类型:XA-02";
        case DET_TYPE_XA_06:
            return "芯片类型:XA-06";					
        case DET_TYPE_XA_21:
            return "芯片类型:XA-21";				
        default:
            return "芯片类型:未知";
    }    
}
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u8 FacCode = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(COLOR_DLG_BK);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG://初始化界面
			WindowTitleUpdate("测试项目");
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
			if(ENTER_FACTORY == UserParam.Stage)
			{
				TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), GetDetTypeDes(UserParam.DetType));			
			}
			else if((FINISHED == UserParam.Stage) || ( SEMIFINISHED== UserParam.Stage))
			{
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GetDetTypeDes(UserParam.DetType));

                if (UserParam.DetCodeWrite)
                {
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码写入:写");
                }
                else
                {
                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码写入:不写");
                }
				if (UserParam.DetCodeWrite && (UserParam.NetMode == 0))
				{
				
					if(FactoryCodeInsideToOutside(UserParam.FactoryCode, &FacCode) == ERROR)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "工厂代码:未知");										
					}
					else
					{
						sprintf((char*)OpData.GuiTemp, "工厂代码:%d", FacCode);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);					
					}


					sprintf((char*)OpData.GuiTemp, "生产时间:%05u", UserParam.ProductTime);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);	
					
					memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
					strcpy((char*)OpData.GuiTemp, "特征号码:");
					OpData.GuiTemp[strlen((char*)OpData.GuiTemp)] = UserParam.Feature;
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);

					if(XA2_DetIsFixDelay(UserParam.DetType))
					{//固定延期芯片显示延期才有意义
						sprintf((char*)OpData.GuiTemp, "雷管延期:%ums", UserParam.DetDelay);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), (char*)OpData.GuiTemp);			
					}					
				}
				else 
				{
					if(UserParam.CheckDetCode)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "内码检验:检");
					}
					else
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "内码检验:不检");
					}
					if(XA2_DetIsFixDelay(UserParam.DetType))
					{//固定延期芯片显示延期才有意义
						if(UserParam.CheckDelay)
						{
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "延期检测:检");
							sprintf((char*)OpData.GuiTemp, "雷管延期:%ums", UserParam.DetDelay);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);							
						}
						else
						{
							if(UserParam.DelayWrite)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "延期写入:写");
								sprintf((char*)OpData.GuiTemp, "雷管延期:%ums", UserParam.DetDelay);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);
							}
							else 
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "延期检测:不检");
							}							
						}		
					}
					else
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "延期检测:不检");
					}
				}													
			}
			else
			{
                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GetDetTypeDes(UserParam.DetType));

				if(0 == UserParam.DetCodeWrite)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码写入:不写");
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "工厂代码:不检");
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), "生产时间:不检");
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), "特征号码:不检");
				}
				else
				{
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "内码写入:写");	

									
					if(FactoryCodeInsideToOutside(UserParam.FactoryCode, &FacCode) == ERROR)
					{
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), "工厂代码:未知");										
					}
					else
					{
						sprintf((char*)OpData.GuiTemp, "工厂代码:%d", FacCode);
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), (char*)OpData.GuiTemp);					
					}


					sprintf((char*)OpData.GuiTemp, "生产时间:%05u", UserParam.ProductTime);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), (char*)OpData.GuiTemp);	
					
					memset((char*)OpData.GuiTemp, 0, sizeof(OpData.GuiTemp));
					strcpy((char*)OpData.GuiTemp, "特征号码:");
					OpData.GuiTemp[strlen((char*)OpData.GuiTemp)] = UserParam.Feature;
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4), (char*)OpData.GuiTemp);													
				}
				if(XA2_DetIsFixDelay(UserParam.DetType))
				{//固定延期芯片显示延期才有意义
					sprintf((char*)OpData.GuiTemp, "雷管延期:%ums", UserParam.DetDelay);
					TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5), (char*)OpData.GuiTemp);			
				}			
			}			
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_BACK:										
					ShowDetTestContentMenuDlg(GetWindowParam());
					break;					
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowDetTestCurrentContentMenuDlg(u8 Param)
{
	CloseWindowAndParent(DlghWin.Dlg);
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
    SetWindowParam(Param);
}

