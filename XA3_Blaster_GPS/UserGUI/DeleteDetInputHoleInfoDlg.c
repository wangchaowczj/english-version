#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },  //回删
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     140, 110, 35,  20,  0,          0 },//105, 110, 50,  20,  0,          0
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT1,     187, 110, 35,  20,  0,          0 },//165, 110, 45,  20,  0,          0 
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT2,     140, 140, 35,  20,  0,          0 },//105, 140, 50,  20,  0,          0
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT3,     187, 140, 35,  20,  0,          0 }	//165, 140, 45,  20,  0,          0 
};

static u8 GetInputInsideSole(u16 HoleName, u8* Inside)
{
	u8 Count = 0;
	u8 Max, Min;
	
	if(Det.HoleNumber[HoleName-1] <= 1)
	{
		*Inside = 1;
		return SUCCESS;
	}
	else
	{
		Count = GetCurrentHoleNameDetCount(HoleName);
		if(Count == 0)
		{
			*Inside = 1;
			return SUCCESS;
		}
		else if(Count == 1)
		{
			if(ERROR != GetCurrentHoleMaxMinRegInside(HoleName, &Max, &Min))
			{
				*Inside = Min;
				return SUCCESS;			
			}
		}
	}
	return ERROR;
}	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;
	HOLE StartHole, EndHole;
	u8 U8Value;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Del DET");//雷管删除
            ShowInfoHelp("Input:0...9","OK: ↙ ");		//输入:0...9键","确认:↙键	

            GUI_SetFont(&SMALL_FONT);	
            GUI_DispStringAt((const char*)" Hole No. From:",18 , 112);//起始孔号: 30 , 112
            GUI_DispStringAt((const char*)"-",176 , 112);//155 , 112
            GUI_DispStringAt((const char*)" Hole No. To  :",18 , 142);//截止孔号:  30 , 142
            GUI_DispStringAt((const char*)"-",176 , 142);//155 , 142
            break;
        case WM_INIT_DIALOG://初始化界面	
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3),&SMALL_FONT);
            
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 3);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 2);
            
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT2), 3);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 2);
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);

            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://左键返回
                    ShowMainMenuDlg(0);											
                    break;
                case GUI_KEY_RIGHT://右键回删
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                        {						
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT3) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);						
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                            if(GetInputInsideSole(Value, &U8Value) != ERROR)
                            {
                                MULTIEDIT_SetText(DlghWin.CurrentInput, "");
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);							
                            }
                        }					
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);						
                        }																	
                    }				
                    break;					
                case GUI_KEY_ENTER://回车键确认
                    if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)//没有输入时返回1
                    {
                        if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
                            if((Value > 0) && (Value <= DET_MAX_SIZE))
                            {
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                                if(GetInputInsideSole(Value, &U8Value) != ERROR)
                                {
                                    InitUserDataToMULTIEDIT(DlghWin.CurrentInput, U8Value);	
                                    DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
                                }
                            }						
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);						
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
                            if((Value > 0) && (Value <= DET_MAX_SIZE))
                            {
                                DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
                                if(GetInputInsideSole(Value, &U8Value) != ERROR)
                                {
                                    InitUserDataToMULTIEDIT(DlghWin.CurrentInput, U8Value);	
                                }
                            }						
                        }					
                        else
                        {//密码输入完成                           
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));	
                            if((Value == 0) || (Value > DET_MAX_SIZE))
                            {                                                              
                                MsgBoxInit();
                                SetMsgBoxCaption("Del DET");//雷管删除
                                SetMsgBoxMessage("Start Hole No. ERR!\nPls Re-enter!");//起始孔号错误!\n请重新输入!
                                sprintf(temp, "DET QTY:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ClearUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                                }						
                                break;
                            }
                            StartHole.Name = Value;
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));	
                            if((Value == 0) || (Value > NET_PARAM_MAX))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Del DET");//雷管删除
                                SetMsgBoxMessage("Start Hole No. ERR!\nPls Re-enter!");//起始孔号错误!\n请重新输入!
                                sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ClearUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
                                }							
                                break;
                            }
                            StartHole.Inside = Value;
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));	
                            if((Value == 0) || (Value > DET_MAX_SIZE))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Del DET");//雷管删除
                                SetMsgBoxMessage("Stop Hole No. ERR!\n\nPls Re-enter!");//截止孔号错误!\n请重新输入!
                                sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ClearUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
                                }						
                                break;
                            }
                            EndHole.Name = Value;
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3));	
                            if((Value == 0) || (Value > NET_PARAM_MAX))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Del DET");//雷管删除
                                SetMsgBoxMessage("Stop Hole No. ERR!\n\nPls Re-enter!");//截止孔号错误!\n请重新输入!
                                sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ClearUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
                                }							
                                break;
                            }

                            EndHole.Inside = Value;		
                            
                            if((((u32)StartHole.Name<<16)+ StartHole.Inside) > (((u32)EndHole.Name<<16)+ EndHole.Inside))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Del DET");//雷管删除
                                SetMsgBoxMessage("Start Hole No.> Stop Hole No.!\nPls Re-enter!");//起始孔号大于截止孔号!\n请重新输入!
                                sprintf(temp, "Log DET:%d", Det.RegCount);//注册雷管数:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回
                                
                                ShowMsgBox();							
                                break;							
                            }
                            MsgBoxInit();
                            SetMsgBoxCaption("Del DET");//雷管删除
                            SetMsgBoxMessage("Del the selected DET?");//确定删除指定的雷管?
                            SetMsgBoxHelp1("Pls do this with caution");//请谨慎执行此操作
                            SetMsgBoxHelp2("Advise to backup the Dat");//建议提前备份数据
                            SetMsgBoxLeft("Cancel");//取消
                            SetMsgBoxRight("OK");//确认
                            
                            Value = ShowMsgBox();
                            if(Value == GUI_KEY_LEFT)
                            {
                                ShowDeleteDetMenuDlg();	
                            }
                            else if(Value == GUI_KEY_RIGHT)
                            {
                                if(DeleteDetInfo(&StartHole, &EndHole) == ERROR)
                                {
                                    ShowSDCardError();
                                }
                                else
                                {
                                    ShowDeleteDetMenuDlg();
                                }
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

void ShowDeleteDetInputHoleInfoDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


