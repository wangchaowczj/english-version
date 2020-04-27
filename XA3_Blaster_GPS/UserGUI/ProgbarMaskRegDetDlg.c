#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR0,     20, 120, 200, 20,  0,          0 }
};

static void UpdateBar(WM_HWIN hWin)
{
	GUI_RECT Rect;
	
	PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);
	
	//仅更新帮助区内容，即更新错误雷管数
	Rect.x0 = 0;
	Rect.y0 = INFO_HELP_LINE_Y;
	Rect.x1 = LCDP_MAX_X - 1;
	Rect.y1 = BUTTON_SMALL_Y - 1;
	WM_InvalidateRect(hWin,&Rect);		
}
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

            ShowInfoTitle("Logging DETs");//雷管注册
            GUI_SetFont(&SMALL_FONT);	
            GUI_DispStringAt("Initializing...",60,100);//信息初始化中...			

            sprintf(&temp[0],"Log DET:%d",Det.RegCount);//注册雷管:		

            ShowInfoHelp(&temp[0], NULL);
            break;
        case WM_INIT_DIALOG://初始化界面
            PROGBAR_SetFont(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0),&SMALL_FONT);
            
            WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));//不允许手动取消
            
            hitem = WM_GetDialogItem(hWin, GUI_ID_PROGBAR0);
            PROGBAR_SetBarColor(hitem, 0, device_info.TextColor);//进度条左侧颜色
            PROGBAR_SetBarColor(hitem, 1, GetDifferentColor());//进度条右侧颜色
            PROGBAR_SetTextColor(hitem, 0, device_info.BackgroundColor);
            PROGBAR_SetTextColor(hitem, 1, device_info.TextColor);
            
            //雷管检测任务
            OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
            TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)24);
            ClearProgbarData();
            
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_10MS_ID, GUI_TIMER_10MS, 0);	
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://取消雷管测试							
                    break;			
            }
            break;
        case WM_UPDATE_PROGBAR://更新进度条
            UpdateBar(hWin);	
            break;
        case WM_SET_PROGBAR_PARAM:
            PROGBAR_SetMinMax(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), 0, Op_Data.Progbar.Bar[0].Max);
            PROGBAR_SetValue(WM_GetDialogItem(hWin, GUI_ID_PROGBAR0), Op_Data.Progbar.Bar[0].Value);	
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_10MS_ID:                 
                    UpdateBar(hWin);
                    if(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
                    {
                        WM_RestartTimer(pMsg->Data.v, GUI_TIMER_10MS);
                        break;
                    }
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    SetWaitRegListViewDlgSelIndex(RegHoleInit());//初始化待注册孔
	
                    if((TEST_RESULT)AppTaskResult.DetOperate == UNREG_DET)
                    {//有未注册雷管
                        int Value;
                        
                        MsgBoxInit();
                        SetMsgBoxCaption("Logging DETs");//雷管注册

                        if(Op_Data.FindUnRegDetInfo)
                        {
                            memset(temp, 0,sizeof(temp));
                            if(ERROR == XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, Op_Data.UnRegDetInfo.Pwd.U32Value, (u8*)temp))
                            {
                                sprintf((char*)Op_Data.Temp, "Have unLogged DETs!\n\nChip ID:%02X%02X%02X%02X%02X%02X%02X%02X", //网络中有未注册雷管!\n\n内码:
                                    Op_Data.UnRegDetInfo.Serl.U8Buffer[0], Op_Data.UnRegDetInfo.Serl.U8Buffer[1], 
                                    Op_Data.UnRegDetInfo.Serl.U8Buffer[2], Op_Data.UnRegDetInfo.Serl.U8Buffer[3], 
                                    Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], Op_Data.UnRegDetInfo.Pwd.U8Buffer[1], 
                                    Op_Data.UnRegDetInfo.Pwd.U8Buffer[2], Op_Data.UnRegDetInfo.Pwd.U8Buffer[3]);                            
                            }
                            else
                            {
                                sprintf((char*)Op_Data.Temp, "Have unlogged DETs!\n\nChip ID:%s", temp);//网络中有未注册雷管!\n\n管码:
                            }

                            SetMsgBoxMessage((char*)Op_Data.Temp);
                        }
                        else
                        {
                            SetMsgBoxMessage("网络中有未注册雷管!");
                        }
                        SetMsgBoxMessageColor(GUI_RED);
                        sprintf(temp, "Log DET:%d ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确定
                        
                        Value = ShowMsgBox();
                        if(Value == GUI_KEY_LEFT)
                        {
                            ShowMainMenuDlg(0);
                            return;
                        }
                        else if(Value != GUI_KEY_RIGHT)
                        {
                            return;
                        }	
                    }
                    if(HasDivideRegHole())
                    {
                        MsgBoxInit();
                        SetMsgBoxCaption("DET Info");//雷管信息
                        SetMsgBoxMessage("Part of DETs registration \n\naren't in order!");//有部分雷管跳孔注册!
                        SetMsgBoxMessageColor(GUI_RED);
                        sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                        SetMsgBoxHelp1(temp);
                        SetMsgBoxLeft("Back");//返回
                        SetMsgBoxRight("OK");//确定
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_LEFT:
                                {
                                    u16 i;
                                    for(i=0;i<DET_MAX_SIZE;i++)
                                    {
                                        if(GetDetRegStatus(i) == ERROR)
                                        {
                                            SetWaitRegListViewDlgSelIndex(i);
                                            break;
                                        }
                                    }                                      
                                }
                                ShowWaitRegListViewDlg();
                                break;
                            case GUI_KEY_RIGHT:
                                ShowWaitRegListViewDlg();
                                break;                                        
                        }
                    }
                    else
                    {
                        ShowWaitRegListViewDlg();
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

void ShowProgbarMaskRegDetDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}

