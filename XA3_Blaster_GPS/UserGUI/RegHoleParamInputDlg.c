#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 100, 240,  20,  0,          0 },//56, 100, 72,  20,  0,          0
	{ MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     152, 100, 56,  20,  0,          0 },//128, 100, 56,  20,  0,          0
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     0, 130, 240,  20,  0,          0 }//3, 130, 240,  20,  0,          0
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;
    u8 U8Value;
    HOLE SelectHole;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
            switch(GetWindowParam())
            {
                case 0:
                    ShowInfoTitle("In-hole DLY");//孔内延期
                    break;
                case 1:
                    ShowInfoTitle("Outer-hole DLY");//孔间延期
                    break;
                case 2:
                    ShowInfoTitle("In-hole DET");//孔内雷管
                    break;
            }
            
            GUI_SetFont(&SMALL_FONT);
            
            SelectHole = Det.Info[GetWaitRegListViewDlgSelIndex()].Hole;
            sprintf((char*)Op_Data.Temp, "Current Hole No.:%d-%d", SelectHole.Name,SelectHole.Inside);//当前孔号:
			      GUI_DispStringAt((const char*)Op_Data.Temp, 32, 75);
            
			      ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键
			break;
		case WM_INIT_DIALOG://初始化界面
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);		
            
		        TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			      TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);		
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT://左键返回
					ShowWaitRegListViewDlg();															
					break;
				case GUI_KEY_RIGHT://右键回删
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)
						{						
							MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), GUI_KEY_BACKSPACE);
						}				
					}			
					break;					
				case GUI_KEY_ENTER://回车键确认
					if(MULTIEDIT_GetTextSize(WM_GetDialogItem(hWin, GUI_ID_EDIT0)) > 1)//没有输入时返回1
					{                        
						ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
						Value = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                        
                        SelectHole = Det.Info[GetWaitRegListViewDlgSelIndex()].Hole;
                        switch(GetWindowParam())
                        {
                            case 0://孔内延期
                                if(Value > Det.DlyMax)
                                {
                                    sprintf((char*)Op_Data.Temp, "In-hole DLY:0~%dms", Det.DlyMax);//孔内延期:
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
                                    return;
                                }
                                
                                NetParam[Det.HoleNumber[SelectHole.Name-1]-1].InsideSpacesDly = Value;
                                if(SaveNetParam() != ERROR)
                                {
                                    RegHoleInit();
                                    ShowWaitRegListViewDlg();
                                }
                                else
                                {	
                                    ShowSDCardError();
                                }                                
                                break;
                            case 1://孔间延期
                                if(Value > Det.DlyMax)
                                {
                                    sprintf((char*)Op_Data.Temp, "Outer-hole DLY:0~%dms", Det.DlyMax);//孔间延期:
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
                                    return;
                                }

                                NetParam[Det.HoleNumber[SelectHole.Name-1]-1].SpacesDly = Value;
                                if(SaveNetParam() != ERROR)
                                {
                                    RegHoleInit();
                                    ShowWaitRegListViewDlg();
                                }
                                else
                                {	
                                    ShowSDCardError();
                                }                                
                                break;
                            case 2://孔内雷管
                                if((Value < 1) || (Value > NET_PARAM_MAX))
                                {
                                    sprintf((char*)Op_Data.Temp, "In-hole DET:1~%d", NET_PARAM_MAX);//孔内雷管:
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), (char*)Op_Data.Temp);
                                    return;
                                }

                                if(ERROR != GetCurrentHoleMaxMinRegInside(SelectHole.Name, &U8Value, NULL))
                                {
                                    if(Value < U8Value)
                                    {
                                        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "In-hole DET QTY < ");//孔内雷管小于已注册孔
																		  	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), "Logged Hole QTY   ");//孔内雷管小于已注册孔
                                        return;
                                    }						
                                }                               
                                
                                device_info.RegNetParamDefault = Value-1;
                                if(SaveUserConfigParam() != ERROR)
                                {
                                    u16 i;
                                    U32 U32Value1, U32Value2;
                                    
                                    Det.HoleNumber[SelectHole.Name-1] = Value;
                                    //大于当前选择孔号的所有未注册孔的孔内雷管数都修改成当前值
                                    for(i=0; i < DET_MAX_SIZE; i++)
                                    {
                                        if((i+1) > SelectHole.Name)
                                        {
                                            if(GetCurrentHoleNameDetCount(i+1) == 0)
                                            {
                                                Det.HoleNumber[i] = Value;
                                            }                                        
                                        }
                                    }
                                    if(ERROR == SaveAllDetInfo(0))//保存雷管信息
                                    {
                                        ShowSDCardError();
                                    }
                                    else
                                    {
                                        RegHoleInit();
                                        
                                        //之前的待注册孔有可能不存在了，重新选择待注册孔
                                        U32Value2 = (SelectHole.Name<<16) + SelectHole.Inside;
                                        for(i=0; i < DET_MAX_SIZE; i++)
                                        {
                                            U32Value1 = (Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
                                            if(U32Value1 >= U32Value2)
                                            {
                                                SetWaitRegListViewDlgSelIndex(i);
                                                break;
                                            }
                                        }
                                        ShowWaitRegListViewDlg();
                                    }
                                }
                                else
                                {	
                                    ShowSDCardError();
                                }
                                break;
                            default:
                                break;
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
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						ShowInputCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 0);
						MULTIEDIT_AddKey(WM_GetDialogItem(hWin, GUI_ID_EDIT0), ((WM_KEY_INFO*)(pMsg->Data.p))->Key);				
					}				
					break;				
			}
			break;		
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					WM_DeleteTimer(pMsg->Data.v);
					
                    TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);
                    //TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
                    
                    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT1), GUI_RED);
                    
                    switch(GetWindowParam())
                    {
                        case 0:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "       In-hole DLY:");//孔内延期:
                            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 5);
                            break;
                        case 1:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "    Outer-hole DLY:");//孔间延期:
                            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 5);
                            break;
                        case 2:
                            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), "       In-hole DET:");//孔内雷管:
                            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 2);
                            break;
                    }
                    	
                    MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
                
                    sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
                    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);								
					break;
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
					
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_EDIT0)))
					{
						ShowCursor(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
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

void ShowRegHoleParamInputDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}


