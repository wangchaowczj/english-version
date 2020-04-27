#include "includes.h"
#include "GuiIncludes.h"

/*界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	//返回
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//回删
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT0,     148, 70, 48, 20,  0,          0 },//114, 70, 80, 20,  0,          0
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT1,     148, 95, 48, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT2,     148, 120, 48, 20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, "", GUI_ID_EDIT3,     148, 145, 48, 20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 165, 240, 20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 185, 240, 20,  0,          0 }//翻译之前没有这几句，自己后来加的
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	u32 Value;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
			ShowInfoTitle("Date Shift");//数据移动	
			ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键		
			
			GUI_SetFont(&SMALL_FONT);
							
			GUI_DispStringAt("From:",108,72);//起始孔号:
			GUI_DispStringAt("To  :",108,97);//截止孔号:
			if(GetWindowParam() == 0)
			{
				GUI_DispStringAt("Hole No. Inc:",44,122);//递增孔数:
				GUI_DispStringAt("     DLY Inc:",44,147);//延期增加:
			}
			else
			{
				GUI_DispStringAt("Hole No. Dec:",44,122);	//递减孔数:
				GUI_DispStringAt("     DLY Dec:",44,147);//延期减小:
			}			
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);	
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://取消				
					ShowNetInfoModifyMenuDlg();														
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsWindow(DlghWin.CurrentInput))
					{
						if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
						{						
							MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
						{//当前正在输入截止孔
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
						{//当前正在输入截止孔
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT3) == DlghWin.CurrentInput)
						{//当前正在输入截止孔
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}						
						TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "");
					}				
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
					{	
						u16 Start, End;
						s16 Pos = 0;
						s16 DelaySpace = 0;
						
						if(WM_GetDialogItem(hWin, GUI_ID_EDIT0) == DlghWin.CurrentInput)
						{//正在输入起始孔
							Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
							if((Value == 0) || (Value > DET_MAX_SIZE))
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Hole No. beyond the range!");//孔号超出范围!
								break;
							}						
							if(GetCurrentHoleNameDetCount(Value) == 0)
							{
								
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "No Logged DET in this hole!");//该孔没有注册雷管!

								break;						
							}						
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
						{//正在输入截止孔
							Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
							if((Value == 0) || (Value > DET_MAX_SIZE))
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Hole No. beyond the range!");//孔号超出范围!
								break;
							}												
							if(GetCurrentHoleNameDetCount(Value) == 0)
							{
								
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "This hole hasn't logged DET!");//该孔没有注册雷管!

								break;						
							}
							Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
							
							if(Value > GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1)))
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Start Hole No.> Stop Hole No.!");//起始孔号大于截止孔号!							
								break;
							}							
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}
						else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
						{//正在输入孔数
//							Value = GetUserValueFromMULTIEDIT(CurrentInput);							
//							if(Value == 0)//允许为0，只为了加减延期
//							{
//								if(Op_Data.DlgParam == 0)
//								{
//									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "递增孔数不能为0!");	
//								}
//								else
//								{
//									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "递减孔数不能为0!");	
//								}														
//								break;
//							}						
							ShowInputCursor(DlghWin.CurrentInput, 0);
							DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);
							ShowInputCursor(DlghWin.CurrentInput, 1);
						}						
						else
						{//延期输入完成
							ShowInputCursor(DlghWin.CurrentInput, 0);
							Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));							
							if(Value > Det.DlyMax)
							{
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DLY beyond the range!");//延期超出范围!															
								break;
							}

							Start = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
							End = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
							Pos = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
							DelaySpace = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3));							
							if(GetWindowParam() == 0)
							{//孔号递增															
								Value = HoleMove(Start, End,Pos, DelaySpace);
							}
							else
							{//孔号递减	
								Value = HoleMove(Start, End,-Pos, -DelaySpace);
							}
							ReadAllDetInfo(0);
							if(Value == 0)
							{
                                MsgBoxInit();
                                SetMsgBoxCaption("Date Shift");//数据移动
                                SetMsgBoxMessage("Dat Shift Completed!");//数据移动成功!
                                sprintf(temp, "Log DET:%d ERR DET:%d", Det.RegCount, Det.ErrorCount);//注册雷管:%d 错误雷管:
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回
                                SetMsgBoxRight("OK");//确定
                                switch(ShowMsgBox())
                                {
                                    case GUI_KEY_LEFT:
                                        ShowNetInfoModifyMenuDlg();
                                        break;
                                    case GUI_KEY_RIGHT:
                                        ShowNetInfoDlg(GetWindowParam());
                                        break;                                        
                                }
							}
							else
							{
								if(Value == 1)
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Hole No. beyond the range!");//孔号超出范围!	
								}
								else if(Value == 2)
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Repeated Hole No.!");	//孔号有重叠!	
								}
								else if(Value == 3)
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "DLY beyond the range!");//延期超出范围!	
								}							
								else
								{
									TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "Dat Shift Failed!");//数据移动失败!
								}
								WM_InvalidateWindow(hWin);							
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
					
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&SMALL_FONT);
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3),&SMALL_FONT);
                    
                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 3);
                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 3);
                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT2), 3);
                    MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 5);

                    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT0), GUI_RED);
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    
                    DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);	
                    ShowInputCursor(DlghWin.CurrentInput, 1);
                    if(Det.RegCount > 0)
                    {
                        Value = GetRegDetHoleMax();
                        if(GetWindowParam() == 0)
                        {//递增
                            if(Det.Info[Value].Hole.Name < DET_MAX_SIZE)
                            {
                                InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), Det.Info[Value].Hole.Name);
                            }                
                        }
                        else
                        {
                            InitUserDataToMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1), Det.Info[Value].Hole.Name);
                        }
                    }
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

void ShowNetInfoModifyRangeInputDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
