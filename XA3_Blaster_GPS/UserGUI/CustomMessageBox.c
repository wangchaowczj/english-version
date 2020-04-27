#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },  
  { BUTTON_CreateIndirect,   "",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     0, 100, 240,  96,  0,          0 }
};
MSG_BOX_DATA MsgBoxData;

static u8 IsShowMsgFlag = 0;
u8 MsgBoxIsShow(void)
{
	return IsShowMsgFlag;
}
void CloseMsgBoxForce(void)
{
	if(IsShowMsgFlag)
	{
		IsShowMsgFlag = 2;
	}	
}
void CloseMsgBoxAndParentForce(void)
{
	if(IsShowMsgFlag)
	{
		IsShowMsgFlag = 3;
	}	
}
void MsgBoxInit(void)
{
    MsgBoxData.Caption[0] = 0;
    MsgBoxData.Message[0] = 0;
    MsgBoxData.Help1[0] = 0;
    MsgBoxData.Help2[0] = 0;
    MsgBoxData.Left[0] = 0;
    MsgBoxData.Right[0] = 0;
    MsgBoxData.MessageColor = device_info.TextColor;
}
void SetMsgBoxCaption(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Caption))
    {
        strcpy(MsgBoxData.Caption, str);
    }
}
void SetMsgBoxMessage(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Message))
    {
        strcpy(MsgBoxData.Message, str);
    }
    else
    {
        memcpy(MsgBoxData.Message, str, sizeof(MsgBoxData.Message)-1);
        MsgBoxData.Message[sizeof(MsgBoxData.Message)-1] = 0;
    }
}
void SetMsgBoxHelp1(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Help1))
    {
        strcpy(MsgBoxData.Help1, str);
    }
}
void SetMsgBoxHelp2(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Help2))
    {
        strcpy(MsgBoxData.Help2, str);
    }
}
void SetMsgBoxLeft(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Left))
    {
        strcpy(MsgBoxData.Left, str);
    }
}
void SetMsgBoxRight(char* str)
{
    if(strlen(str) < sizeof(MsgBoxData.Right))
    {
        strcpy(MsgBoxData.Right, str);
    }
}
void SetMsgBoxMessageColor(u32 Color)
{
    MsgBoxData.MessageColor = Color;
}
u8 HelpSupportEnter(void)
{
    u8 i, len;
    
    len = strlen(MsgBoxData.Help1);
    if(len > 0)
    {
        for(i=0; i<len;)
        {
            if((u8)MsgBoxData.Help1[i] >= 0xe0)
            {
                if(memcmp(&MsgBoxData.Help1[i], "↙", 3) == 0)
                {
                    return 1;
                }
                i += 3;
            }
            else
            {
                i++;
            }
        }
    }
    len = strlen(MsgBoxData.Help2);
    if(len > 0)
    {
        for(i=0; i<len; )
        {
            if((u8)MsgBoxData.Help2[i] >= 0xe0)
            {
                if(memcmp(&MsgBoxData.Help2[i], "↙", 3) == 0)
                {
                    return 2;
                }
                i += 3;
            }
            else
            {
                i++;
            }
        }
    }
    return 0;
}
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
    char temp[80];
    char* Line1;
    char* Line2;
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle((char*)MsgBoxData.Caption);
            
            GUI_SetFont(&SMALL_FONT);
            if(MsgBoxData.Help1[0] == 0)
            {
                Line1 = NULL;
            }
            else
            {
                Line1 = (char*)MsgBoxData.Help1;
            }
            if(MsgBoxData.Help2[0] == 0)
            {
                Line2 = NULL;
            }
            else
            {
                Line2 = (char*)MsgBoxData.Help2;
            }
            ShowInfoHelp(Line1, Line2);
            break;
        case WM_INIT_DIALOG://初始化界面
            IsShowMsgFlag = 1;
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_TOP);
            TEXT_SetWrapMode(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_WRAPMODE_CHAR);
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), MsgBoxData.MessageColor);
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)MsgBoxData.Message);
            
            if(MsgBoxData.Left[0] == NULL)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
            }
            else
            {
                BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), MsgBoxData.Left);
            }
            if(MsgBoxData.Right[0] == NULL)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
            }
            else
            {
                BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON2), MsgBoxData.Right);
            }
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }

            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_2MS_ID, GUI_TIMER_2MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
                    {
                        DlghWin.Dlg = WM_GetParent(hWin);
                        GUI_EndDialog(hWin, GUI_KEY_LEFT);
                        IsShowMsgFlag = 0;
                    }
                    break;
                case GUI_KEY_RIGHT:
                    if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
                    {
                        DlghWin.Dlg = WM_GetParent(hWin);
                        GUI_EndDialog(hWin, GUI_KEY_RIGHT); 
                        IsShowMsgFlag = 0;
                    }
                    break;
                case GUI_KEY_ENTER:
                    if(HelpSupportEnter())
                    {
                        DlghWin.Dlg = WM_GetParent(hWin);
                        GUI_EndDialog(hWin, GUI_KEY_ENTER); 
                        IsShowMsgFlag = 0;
                    }
                    break;	                
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_2MS_ID:
                    if(IsShowMsgFlag == 2)
                    {
                        IsShowMsgFlag = 0;	
                        GUI_EndDialog(hWin, 0);
                    }
                    else if(IsShowMsgFlag == 3)
                    {
                        WM_HWIN Parent ;
                        
                        IsShowMsgFlag = 0;	
                        Parent = WM_GetParent(hWin);
                        GUI_EndDialog(hWin, 0);
                        CloseWindowAndParent(Parent);
                    }				
                    if(IsShowMsgFlag != 0)
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_2MS);
                    }
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

int ShowMsgBox(void)
{
    WM_HWIN hWin;
    
	hWin = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, DlghWin.Dlg, 0, 0);
    
    WM_MakeModal(hWin);
	DlghWin.Dlg = hWin;

	/* Exec dialog */
	return GUI_ExecCreatedDialog(hWin);
}
