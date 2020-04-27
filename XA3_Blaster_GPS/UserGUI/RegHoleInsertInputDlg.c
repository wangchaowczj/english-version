#include "includes.h"
#include "GuiIncludes.h"

static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Del", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     29, 90, 150,  20,  0,          0 },
	{ TEXT_CreateIndirect, "DET QTY     :", GUI_ID_TEXT1,     29, 115, 130,  20,  0,          0 },//孔内雷管
	{ TEXT_CreateIndirect, "DLY INC(AFT):", GUI_ID_TEXT2,     29, 140, 130,  20,  0,          0 },//延期递增
  { MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT0,     133, 115, 70,  20,  0,          0 },
  { MULTIEDIT_CreateIndirect, NULL, GUI_ID_EDIT1,     133, 140, 70,  20,  0,          0 },
  { TEXT_CreateIndirect, "", GUI_ID_TEXT9,     0, 170, 240,  20,  0,          0 },
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 无
  * @retval 无
  * @author ZH
  * @date   2018年11月15日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	u32 Value;
    HOLE SelectHole;
    
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            ShowInfoTitle("Insert Hole No.");//插入孔号
            ShowInfoHelp("Input:0...9","OK: ↙ ");//输入:0...9键","确认:↙键		
            break;
        case WM_INIT_DIALOG://初始化界面
            TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT9), GUI_RED);
            TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT9), TEXT_CF_HCENTER|TEXT_CF_VCENTER);
            
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT0), 3);	
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT0),&SMALL_FONT);
            
            MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin, GUI_ID_EDIT1), 5);	
            MULTIEDIT_SetFont(WM_GetDialogItem(hWin, GUI_ID_EDIT1),&SMALL_FONT);
            
            sprintf((char*)Op_Data.Temp, "Insert Hole No.:%d", Det.Info[GetWaitRegListViewDlgSelIndex()].Hole.Name);//插入孔号:
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), (char*)Op_Data.Temp);
            
            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
            ShowInputCursor(DlghWin.CurrentInput, 1); 
                
            WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
        
            sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
            BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);
            break;
        case WM_KEY:
            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT9), "");
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT://左键返回
                    ShowWaitRegListViewDlg();									
                    break;
                case GUI_KEY_RIGHT://右键回删
                    if(WM_IsWindow(DlghWin.CurrentInput))
                    {
                        if(MULTIEDIT_GetTextSize(DlghWin.CurrentInput) > 1)
                        {						
                            MULTIEDIT_AddKey(DlghWin.CurrentInput, GUI_KEY_BACKSPACE);
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT0);	
                            ShowInputCursor(DlghWin.CurrentInput, 1);
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
                            if((Value == 0) || (Value > NET_PARAM_MAX))
                            {
                                sprintf((char*)Op_Data.Temp, "In-hole DET:1~%d", NET_PARAM_MAX);//孔内雷管:1~%d
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT9), (char*)Op_Data.Temp);
                                break;
                            } 
                            DlghWin.CurrentInput = WM_GetDialogItem(hWin, GUI_ID_EDIT1);
                        }
                        else if(WM_GetDialogItem(hWin, GUI_ID_EDIT1) == DlghWin.CurrentInput)
                        {
                            u16 Count, AddDelay;
                            int i;
                            
                            SelectHole = Det.Info[GetWaitRegListViewDlgSelIndex()].Hole;
                            
                            ShowInputCursor(DlghWin.CurrentInput, 0);
                            Value = GetDecFromMULTIEDIT(DlghWin.CurrentInput);
                            if(Value > Det.DlyMax)
                            {
                                sprintf((char*)Op_Data.Temp, "Delaytime:0~%d", Det.DlyMax);//延期:0~%d
                                TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT9), (char*)Op_Data.Temp);
                                break;
                            }
                            if(Det.RegCount > 0)
                            {
                                Count = GetRegDetHoleMax();
                                if((Count >= DET_MAX_SIZE) || (SelectHole.Name >= Count))
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT9), "Can't Insert DETs!");//无法插入
                                    return; 
                                }                            
                            }
                            
                            Count = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
                            AddDelay = GetDecFromMULTIEDIT(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
                            
                            for(i=DET_MAX_SIZE-Count; i<DET_MAX_SIZE;i++)
                            {
                                if(GetDetRegStatus(i) != ERROR)
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT9), "Insert Failed");//插入失败
                                    return;
                                }
                            }
                            for(i=0; i<DET_MAX_SIZE;i++)
                            {
                                if(Det.Info[i].Hole.Name > SelectHole.Name)
                                {
                                    Value = i;
                                    break;
                                }
                            }
                                                        
                            for(i=DET_MAX_SIZE-1-Count; i >= Value; i--)
                            {
                                Det.Info[i].Hole.Name++;
                                Det.Info[i].Delay += AddDelay;
                                if(Det.Info[i].Delay > Det.DlyMax)
                                {
                                    Det.Info[i].Delay = Det.DlyMax;
                                }
                                Det.Info[i+Count] = Det.Info[i];                                
                            }
                            //孔内注册数也要相应修改
                            
                            for(i=DET_MAX_SIZE-1; i > SelectHole.Name; i--)
                            {
                                Det.HoleNumber[i] = Det.HoleNumber[i-1];
                            }
                            Det.HoleNumber[SelectHole.Name] = Count;
                            
                            i=0;
                            while(i<Count)
                            {
                                Det.Info[Value+i].Serl.U32Value = 0;
                                Det.Info[Value+i].Pwd.U32Value = 0;
                                                                
                                Det.Info[Value+i].Hole.Name = SelectHole.Name+1;
                                Det.Info[Value+i].Hole.Inside = i+1;
                                
                                Det.Info[Value+i].Delay = CalcRegHoleDelayFromNetParam(Value+i);
                                if(Det.Info[Value+i].Delay > Det.DlyMax)
                                {
                                    Det.Info[Value+i].Delay = Det.DlyMax;
                                }
                                
                                i++;
                            }
                            SetWaitRegListViewDlgSelIndex(Value);
                            ShowWaitRegListViewDlg();
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

                    ShowCursor(DlghWin.CurrentInput);				
                    break;
                default:
                    break;
            }		
            break;	
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowRegHoleInsertInputDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}


