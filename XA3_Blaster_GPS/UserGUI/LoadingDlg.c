#include "includes.h"
#include "GuiIncludes.h"


static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     10, 100, 220,  20,  0,          0 }
};
static LOADING_PARAM LoadingParam;

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle((char*)LoadingParam.sCaption);          
            
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
                sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);	//注册雷管:%u 错误雷管:
                ShowInfoHelp(&temp[0], &temp[40]);            
            }
            else
            {
                sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%u 错误雷管:
                GetDetInfoString(&temp[40], Det.Type);	
                ShowInfoHelp(&temp[0], &temp[40]);            
            }	
            break;
        case WM_INIT_DIALOG://初始化界面
            if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
            {
                WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
            }
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    break;				                  
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);

                    if(LoadingParam.sMessage != 0)
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), LoadingParam.sMessage);
                    }			
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);

                    UpdateTextPointCount(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
                    break;
                default:
                    break;
            }		
            break;
        default:
            WM_DefaultProc(pMsg);
	}
}
u8 LoadingDlgIsShow(void)
{
    if((DlghWin.Loading != 0) && WM_IsWindow(DlghWin.Loading))
    {
        return 1;
    }
    return 0;
}
void CloseLoadingDlg(void)
{
	if(WM_IsWindow(DlghWin.Loading))
	{
		GUI_EndDialog(DlghWin.Loading, 0);
        DlghWin.Loading = 0;
	}
}
void ShowLoadingDlg(const char* sCaption, const char* sMessage)
{ 
	if(WM_IsWindow(DlghWin.Loading))
	{
		GUI_EndDialog(DlghWin.Loading, 0);
	}
    LoadingParam.sCaption = sCaption;
    LoadingParam.sMessage = sMessage;
    
	DlghWin.Loading = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, DlghWin.Dlg, 0, 0);
}
