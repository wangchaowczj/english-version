#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 0,  LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL-STATUSBAR_HEIGHT, 0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     0, 0, LCD_WIDTH_PIXEL,  25,  0,          0 },
  { TEXT_CreateIndirect,   "联网检测:",       GUI_ID_TEXT0,     35, 50, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT0,     143, 50-2, 142,  28,  0,          0 },
  { TEXT_CreateIndirect,   "厂家名称:",       GUI_ID_TEXT1,     35, 80, 108,  25,  0,          0 },
  { MULTIEDIT_CreateIndirect,   "",       GUI_ID_EDIT1,     143, 80-2, 142,  28,  0,          0 },  
  { TEXT_CreateIndirect,   "通过方向键设置参数",       GUI_ID_TEXT4,     0, 150, LCD_WIDTH_PIXEL,  25,  0,          0 }
};


const FACTORY_NAME_CFG FactoryNameArray[] = {{"江西国泰", 34}, {"内蒙红旗", 10},{"广东韶化", 52}};
static const char* NetModeArray[] = {"不联网", "联网"};

u8 FactoryNameArrayLength(void)
{
    return sizeof(FactoryNameArray)/sizeof(FACTORY_NAME_CFG);
}
const char* FactoryCode2String(u8 InterfaceCode)
{
    u16 i, Size;
    Size = FactoryNameArrayLength();
    
    for(i=0;i<Size;i++)
    {
        if(InterfaceCode == FactoryNameArray[i].Code)
        {
            return FactoryNameArray[i].Name;
        }        
    }
    return NULL;
}
static u8 FactoryString2Code(const char* Content)
{
    u16 i, Size;
    
    Size = FactoryNameArrayLength();
    for(i=0;i<Size;i++)
    {
        if(strcmp(FactoryNameArray[i].Name, Content) == 0)
        {
            return FactoryNameArray[i].Code;
        }        
    }
    return 0;
}

static u8 GetFactoryStringIndex(const char* Content)
{
    u16 i, Size;
    
    Size = FactoryNameArrayLength();
    for(i=0;i<Size;i++)
    {
        if(strcmp(FactoryNameArray[i].Name, Content) == 0)
        {
            return i;
        }        
    }
    return 0;    
}

static void ShowFactoryStringPrevious(MULTIEDIT_HANDLE hObj)
{
    u8 index;
    
    MULTIEDIT_GetText(hObj, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
    
    index = GetFactoryStringIndex((char*)OpData.GuiTemp);
    if(index == 0)
    {
        index = FactoryNameArrayLength()-1;
    }
    else
    {
        index--;
    }
    MULTIEDIT_SetText(hObj, FactoryNameArray[index].Name);
}
static void ShowFactoryStringNext(MULTIEDIT_HANDLE hObj)
{
    u8 index;
    
    MULTIEDIT_GetText(hObj, (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
    
    index = GetFactoryStringIndex((char*)OpData.GuiTemp);
    index++;
    if(index >= FactoryNameArrayLength())
    {
        index = 0;
    }
    MULTIEDIT_SetText(hObj, FactoryNameArray[index].Name);
}
static void ShowOrHideDelayInput(WM_HWIN hWin)
{	
	MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));

	if ((strcmp((char*)OpData.GuiTemp, "联网") == 0) && (FactoryNameArrayLength() > 1))
	{
		WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
		WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));			
	}
	else
	{
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));	
	}						
}
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
			WindowTitleUpdate("联网设置");			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));	

			MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0), &NORMAL_FONT);	
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1), &NORMAL_FONT);
            
			MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 20);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 20);
			
            MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), NetModeArray[UserParam.NetMode]);
            
            MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), FactoryCode2String(UserParam.NetModeFactoryCode));
			
			OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
            
			TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT4), GUI_RED);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			
			ShowOrHideDelayInput(hWin);
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
			WM_SetFocus(hWin);
			
			ScanKeyBoard.Status = SCAN_KEYBOARD_IDLE;
			break;
		case WM_KEY:		
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
                    {
                        u8 NetMode;
                        u8 InterfaceCode;
                        
                        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
                        NetMode = GetStringArrayIndex(NetModeArray, GET_ARRAY_LENGTH(NetModeArray), (char*)OpData.GuiTemp);
                        
                        MULTIEDIT_GetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), (char*)OpData.GuiTemp, sizeof(OpData.GuiTemp));
                        InterfaceCode = FactoryString2Code((char*)OpData.GuiTemp);					
                        if ((UserParam.NetMode != NetMode) || (UserParam.NetModeFactoryCode != InterfaceCode))
                        {
                            if(RESULT_NO == ShowCustomMessageBox("确认修改?", OpData.DlgTitle, MB_YESNO))
                            {
                                break;
                            }
                            OpData.DetDetectorPassCount = 0;
							UserParam.DetDetectorPassCount = OpData.DetDetectorPassCount;
							UTF8StringToGBK("历史检测数量", (char*)OpData.GuiTemp);						
							if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.DetDetectorPassCount) == ERROR)
							{
								ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
								break;
							}
                            ClearChannelOldStatus();
                            
                            UserParam.NetMode = NetMode;
                            UTF8StringToGBK("联网检测", (char*)OpData.GuiTemp);						
                            if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.NetMode) == ERROR)
                            {
                                ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
                                break;
                            }
                            
                            UserParam.NetModeFactoryCode = InterfaceCode;
                            UTF8StringToGBK("联网管厂", (char*)OpData.GuiTemp);						
                            if(WriteKeyU32Value(CONFIG_PARAM_FILE_COMPLETE_PATH, OpData.GuiTemp, UserParam.NetModeFactoryCode) == ERROR)
                            {
                                ShowCustomMessageBox("保存失败!", OpData.DlgTitle, MB_OK);
                                break;
                            }        
                        }                        
                    }
                    ShowSystemSetDlg();					
					break;
				case GUI_CUSTOM_KEY_BACK:
					ShowSystemSetDlg();					
					break;
				case GUI_CUSTOM_KEY_F4:
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
                        ShowEditArrayPrevious(OpData.CurrentInput, NetModeArray, GET_ARRAY_LENGTH(NetModeArray));                        
					}
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
                        ShowFactoryStringPrevious(OpData.CurrentInput);				
					}                    
					ShowOrHideDelayInput(hWin);
					break;
				case GUI_CUSTOM_KEY_F2:
					if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
					{
                        ShowEditArrayNext(OpData.CurrentInput, NetModeArray, GET_ARRAY_LENGTH(NetModeArray)); 					
					}				
					else if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
					{
                        ShowFactoryStringNext(OpData.CurrentInput);						
					}
					ShowOrHideDelayInput(hWin);
					break;
                case GUI_CUSTOM_KEY_F1:
                    if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT1))
                    {
                        ShowInputCursor(OpData.CurrentInput, 0);
                        OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
                    }
                    break;
                case GUI_CUSTOM_KEY_F3:
                    if(OpData.CurrentInput == WM_GetDialogItem(hWin, GUI_ID_EDIT0))
                    {
                        if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
                        {
                            ShowInputCursor(OpData.CurrentInput, 0);
                            OpData.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);                        
                        }
                    }
                    break;                   					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:	
					break;			
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);

					if(WM_IsWindow(OpData.CurrentInput))
					{						
						ShowCursor(OpData.CurrentInput);
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

void ShowNetModeSetDlg(void)
{
	CloseWindowAndParent(DlghWin.Dlg);
	
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, STATUSBAR_HEIGHT);
}



