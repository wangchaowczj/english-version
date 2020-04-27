#include "includes.h"
#include "GuiIncludes.h"

/*界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     145, 118, 70, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     145, 143, 70, 20,  0,          0 }
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b  
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hitem;
	u32 Value;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();

            ShowInfoTitle("Net PARM");//孔网参数
            ShowInfoHelp("Input:0...9","OK: ↙ ");		//输入:0...9键","确认:↙键

            GUI_SetFont(&SMALL_FONT);	
            sprintf((char*)temp," In-hole DET:%d", NetParam[GetWindowParam()&0xFF].Count);//孔内雷管:
            GUI_DispStringAt(temp,25,95);
            GUI_DispStringAt("Outer-hole DLY:",25,120);//孔间延期:"
            if(NetParam[GetWindowParam()&0xFF].Count > 1)
            {
                GUI_DispStringAt("In-hole DLY   :",25,145);		//孔内延期:
            }

            break;
        case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://取消修改				
                    ShowNetParamListViewDlg();														
                    break;
                case GUI_KEY_RIGHT://右键回删
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                        {						
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {//当前正在输入孔内延期
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
                            ShowInputCursor(DlghWin.CurrentInput, 1);
                        }					
                    }				
                    break;					
                case GUI_KEY_ENTER://回车键确认
                    if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                    {
                        ShowInputCursor(DlghWin.CurrentInput, 0);
                        if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                        {//正在输入孔间延期
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);						
                            if(NetParam[GetWindowParam()&0xFF].Count != 1)//孔内雷管数为1时不需要设置孔内延期
                            {
                                ShowInputCursor(DlghWin.CurrentInput, 1);
                                break;
                            }
                        }
                        
                        if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {//孔内延期输入完成
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                            if(Value > Det.DlyMax)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Net PARM");//孔网参数
                                SetMsgBoxMessage("Outer-hole DLY ERR!");//孔间延期错误!
                                SetMsgBoxLeft("Back");//返回
                                
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowNetParamModifyDlg(GetWindowParam());
                                }							
                                break;
                            }
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
                            if(Value> Det.DlyMax)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Net PARM");//孔网参数
                                SetMsgBoxMessage("In-hole DLY ERR!");//孔内延期错误!
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowNetParamModifyDlg(GetWindowParam());
                                }					

                                break;
                            }						

                            NetParam[GetWindowParam()&0xFF].SpacesDly = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                            NetParam[GetWindowParam()&0xFF].InsideSpacesDly = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));						
                            
                            if(SaveNetParam() != ERROR)
                            {
                                ShowNetParamListViewDlg();
                            }
                            else
                            {	
                                ShowSDCardError();
                            }							
                        }
                    }
                    break;
                case GUI_KEY_0://数字
                case GUI_KEY_1://数字			
                case GUI_KEY_2://数字
                case GUI_KEY_3://数字
                case GUI_KEY_4://数字
                case GUI_KEY_5://数字
                case GUI_KEY_6://数字
                case GUI_KEY_7://数字
                case GUI_KEY_8://数字
                case GUI_KEY_9://数字
                    ShowInputCursor(DlghWin.CurrentInput, 0);
                    MULTIEDIT_AddKey(DlghWin.CurrentInput, ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
                    break;				
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(NetParam[GetWindowParam()&0xFF].Count == 1)
                    {//孔内雷管数为1时不需要设置孔内延期
                        NetParam[GetWindowParam()&0xFF].InsideSpacesDly = 0;
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
                    }
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
                    
                    hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
                    MULTIEDIT_SetMaxNumChars(hitem, 6);
                    InitUserDataToMULTIEDIT(hitem, NetParam[GetWindowParam()&0xFF].SpacesDly);

                    hitem = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                    MULTIEDIT_SetMaxNumChars(hitem, 6);
                    InitUserDataToMULTIEDIT(hitem, NetParam[GetWindowParam()&0xFF].InsideSpacesDly);

                    DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);	
                    ShowInputCursor(DlghWin.CurrentInput, 1);
                    
                    if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                    {
                        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
                    }
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);	                    				
                    break;
                case GUI_TIMER_500MS_ID:
                    WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
                    
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
                    
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        ShowCursor(DlghWin.CurrentInput);
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

void ShowNetParamModifyDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
