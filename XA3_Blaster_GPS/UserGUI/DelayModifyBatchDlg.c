#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//回删
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     128, 60, 105,  20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT1,     128, 90, 105,  20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT2,     128, 120, 105,  20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT3,     128, 150, 105,  20,  0,          0 },
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT4,     128, 180, 105,  20,  0,          0 }	
};
	
/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;
	HOLE StartHole, EndHole;
	u16 StartDly, SpacesDly, InsideSpacesDly;
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Delaytime Settings");//延期设置
				ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键	确认:↙键

            GUI_SetFont(&SMALL_FONT);	
            GUI_DispStringAt((const char*)"Hole No. From :",7 , 62);//起始孔号:
            GUI_DispStringAt((const char*)"Hole No. To   :",7 , 92);//截止孔号:
            GUI_DispStringAt((const char*)"Base DLY      :",7 , 122);//起始延期:
            GUI_DispStringAt((const char*)"Outer-hole DLY:",7 , 152);//孔间延期:
            GUI_DispStringAt((const char*)"In-hole DLY   :",7 , 182);//孔内延期:
            break;
        case WM_INIT_DIALOG://初始化界面	
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 3);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 3);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT2), 5);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT3), 5);
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT4), 5);		
            
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT2),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT3),&SMALL_FONT);
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT4),&SMALL_FONT);
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);

            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);					
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://左键返回
                    ShowNetInfoDlg(1);//启用右功能键和确认键修改延期										
                    break;
                case GUI_KEY_RIGHT://右键回删
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                        {						
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT4) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);						
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
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);											
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT2);						
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT2) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT3);						
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT3) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT4);						
                        }				
                        else
                        {//输入完成
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));	
                            if((Value == 0) || (Value > DET_MAX_SIZE))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");//延期设置
                                SetMsgBoxMessage("Start Hole No. ERR!\n\nPls Re-enter!");//起始孔号错误! 请重新输入!
                                sprintf(temp,"Range of start hole No.:1~%d",DET_MAX_SIZE);//起始孔号范围:1~
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }						
                                break;
                            }
                            StartHole.Name = Value;
                  
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));	
                            if((Value == 0) || (Value > DET_MAX_SIZE))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");//延期设置
                                SetMsgBoxMessage("Stop Hole No. ERR!\n\nPls Re-enter!");//截止孔号错误! 请重新输入!
                                sprintf(temp,"Range of stop hole No.:1~%d",DET_MAX_SIZE);//截止孔号范围:1~
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");//返回

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }						
                                break;
                            }
                            EndHole.Name = Value;
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT2));	
                            if(Value > Det.DlyMax)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");//延期设置
                                SetMsgBoxMessage("Base Delaytime ERR!\n\nPls Re-enter!");//起始延期错误!\n\n请重新输入!
                                sprintf(temp, "Range of base DLY:0~%dms", Det.DlyMax);//起始延期范围:0~%dms
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }							
                                break;
                            }
                            StartDly = Value;
                            
                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT3));	
                            if(Value > Det.DlyMax)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");
                                SetMsgBoxMessage("Outer-hole DLY ERR!\n\nPls Re-enter!");//孔间延期错误!\n\n请重新输入!
                                sprintf(temp, "Outer-hole DLY:0~%dms", Det.DlyMax);//孔间延期范围:0~%dms
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");
                                
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }					
                                break;
                            }
                            SpacesDly = Value;

                            Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT4));	
                            if(Value > Det.DlyMax)
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");
                                SetMsgBoxMessage("In-hole DLY ERR!\n\nPls Re-enter!");//孔内延期错误!\n\n请重新输入!
                                sprintf(temp, "Range of In-hole DLY:0~%dms", Det.DlyMax);//孔内延期范围:0~%dms
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");

                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }						
                                break;
                            }
                            InsideSpacesDly = Value;
                            if(StartHole.Name <= EndHole.Name)
                            {
                                StartHole.Inside = 1;
                                EndHole.Inside = NET_PARAM_MAX;
                            }
                            else
                            {
                                StartHole.Inside = NET_PARAM_MAX;
                                EndHole.Inside = 1;                                
                            }
                            //根据输入的延期参数计算已注册的雷管延期否超限
                            if(ERROR == DelayParamVerify(&StartHole, &EndHole, StartDly,SpacesDly,InsideSpacesDly))
                            {
                                MsgBoxInit();
                                SetMsgBoxCaption("Delaytime Settings");
                                SetMsgBoxMessage("DLY beyond the range!\n\nPls Re-enter!");//延期超出范围!\n\n请重新输入!
                                sprintf(temp, "Settable Range:0~%dms", Det.DlyMax);//延期可设置范围:0~%dms
                                SetMsgBoxHelp1(temp);
                                SetMsgBoxLeft("Back");
                                
                                if(ShowMsgBox() == GUI_KEY_LEFT)
                                {
                                    ShowDelayModifyBatchDlg();
                                }							
                                break;							
                            }
                            else
                            {
                                if(ERROR == DelayModifyBaseOnParam(&StartHole, &EndHole, StartDly,SpacesDly,InsideSpacesDly))
                                {
                                    ShowSDCardError();
                                }
                                else
                                {
                                    Value = GetRegHoleMinIndexFromHoleRange(&StartHole, &EndHole);
                                    if(Value < Det.RegCount)
                                    {//输入的孔范围中有已注册孔
                                        SetNetInfoDlgListViewSelIndex(Value);//从修改的第1发开始显示
                                    }
                                    ShowNetInfoDlg(1);	//启用右功能键和确认键修改延期								
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

void ShowDelayModifyBatchDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


