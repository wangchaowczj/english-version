#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
  { WINDOW_CreateIndirect, "",    0,               0, 00,  240, 300, 0,          0 },
  { BUTTON_CreateIndirect,   "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
  { BUTTON_CreateIndirect,   "Back",       GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 }//返回
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
            ShowInfoTitle("Del DET");//雷管删除
            sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管:
            sprintf(&temp[40], "ERR DET:%d", Det.ErrorCount);//错误雷管:
            ShowInfoHelp(&temp[0], &temp[40]);
            GUI_DispStringAt("1.Del All DETs", 28, 85);//1.删除全部雷管//40
            GUI_DispStringAt("2.Del Selected DETs",28, 110);//2.删除指定雷管
            GUI_DispStringAt("3.Del All ERR DETs",28, 135);//3.删除全部错误雷管
            GUI_DispStringAt("4.Del Selected ERR DETs",28, 160);//4.删除指定错误雷管
            break;
        case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);	
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:
                    ShowMainMenuMoreDlg();
                    break;
                case GUI_KEY_1://删除全部雷管
                    MsgBoxInit();
                    SetMsgBoxCaption("Del DET");//雷管删除
                    SetMsgBoxMessage("Del All DETs?");//确定删除所有雷管?
                    sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管: 错误雷管:
                    SetMsgBoxHelp1(temp);
                    SetMsgBoxLeft("Cancel");//取消
                    SetMsgBoxRight("OK");//确认

                    if(ShowMsgBox() == GUI_KEY_RIGHT)
                    {
                        if(DeleteAllDetInfo() == ERROR)
                        {
                            ShowSDCardError();
                        }
                        else
                        {
                            ShowMainMenuDlg(0);
                        }
                    }
                    break;
                case GUI_KEY_2://删除指定的雷管			
                    ShowDeleteDetInputHoleInfoDlg();
                    break;				
                case GUI_KEY_3://删除全部错误雷管
                    if(0 == Det.ErrorCount)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("No ERR DETs!");//没有错误雷管!
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:  错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回

                        ShowMsgBox();                    
                    }
                    else
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("Del All ERR DETs?");//确定删除所有错误雷管?
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确认
                        
                        if(ShowMsgBox() == GUI_KEY_RIGHT)
                        {
                            if(DeleteAllErrorDetInfo() == ERROR)
                            {
                                ShowSDCardError();
                            }
                            else
                            {
                                ShowDeleteDetMenuDlg();
                            }
                        }                    
                    }			
                    break;	
                case GUI_KEY_4://删除指定的错误雷管
                    if(0 == Det.ErrorCount)
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("No ERR DETs!");//没有错误雷管!
                        sprintf(temp, "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回

                        if(ShowMsgBox() == GUI_KEY_LEFT)
                        {
                            ShowDeleteDetMenuDlg();
                        }
                    }
                    else
                    {
    					SetNetTestErrorListViewDlgListViewSelIndex(0);	
    					ShowNetTestErrorListViewDlg(3);					
                    }				
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

void ShowDeleteDetMenuDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
