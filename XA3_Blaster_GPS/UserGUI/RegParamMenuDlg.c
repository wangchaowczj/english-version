#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  160, 175, 0,          0 },// 0,   0, 0,  119, 175, 0,          0
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 }
};

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
//	char temp[50];
	u32 DlgParam;
    HOLE SelectHole;
    
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
            GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
            GUI_SetColor(GUI_BLACK);
            
            switch(GetWindowParam())
            {
                case 0:
                    GUI_SetFont(&NORMAL_FONT);
                    GUI_DispStringHCenterAt((const char*)"PARM", 70, 0);//参数设置
                    GUI_SetFont(&SMALL_FONT);
                    GUI_DispStringHCenterAt((const char*)"1.Current DLY   ", 70, 30);//1.当前延期
                    GUI_DispStringHCenterAt((const char*)"2.In-hole DLY   ", 70, 55);//2.孔内延期
                    GUI_DispStringHCenterAt((const char*)"3.Outer-hole DLY", 70, 80);//3.孔间延期
                    GUI_DispStringHCenterAt((const char*)"4.In-hole DET   ", 70, 105);//4.孔内雷管
                    break;
                case 1:
                    GUI_SetFont(&NORMAL_FONT);
                    GUI_DispStringHCenterAt((const char*)"Tool", 70, 0);//工具
                    GUI_SetFont(&SMALL_FONT);
                    GUI_DispStringHCenterAt((const char*)"1.Del DET    ", 70, 30);//1.删除雷管
                    GUI_DispStringHCenterAt((const char*)"2.Del All    ", 70, 55);//2.删除全部
                    GUI_DispStringHCenterAt((const char*)"3.Insert     ", 70, 80);//3.插    入
                    GUI_DispStringHCenterAt((const char*)"4.Test Single", 70, 105);//4.单发测试
                    GUI_DispStringHCenterAt((const char*)"5.Rewrite    ", 70, 130);//5.内码重写
                    GUI_DispStringHCenterAt((const char*)"6.Input      ", 70, 155);//6.批量注册    
                    break;                    
            }
			break;
		case WM_INIT_DIALOG://初始化界面	
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));			
			break;
		case WM_KEY:
            MsgBoxInit();
            SelectHole = Det.Info[GetWaitRegListViewDlgSelIndex()].Hole;
            DlgParam = GetWindowParam();
            if(DlgParam == 0)
            {//参数设置               
                switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
                {
                    case GUI_KEY_LEFT:
                    case GUI_KEY_RIGHT:                    
                        DlghWin.Dlg = WM_GetParent(hWin);						
                        GUI_EndDialog(hWin, 0);
                        break;
                    case GUI_KEY_ENTER:	
                        SetWindowParam(1);
                        WM_InvalidateWindow(hWin);
                        break;
                    case GUI_KEY_1://当前延期
                        ShowDelayModifySingleDlg(0, GetWaitRegListViewDlgSelIndex());
                        break;
                    case GUI_KEY_2://孔内延期
                        ShowRegHoleParamInputDlg(0);
                        break;
                    case GUI_KEY_3://孔间延期
                        ShowRegHoleParamInputDlg(1);
                        break;
                    case GUI_KEY_4://孔内雷管
                        ShowRegHoleParamInputDlg(2);
                        break;                        
                }            
            }
            else
            {//工具                
                switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
                {
                    case GUI_KEY_LEFT:
                    case GUI_KEY_ENTER:	
                        DlghWin.Dlg = WM_GetParent(hWin);						
                        GUI_EndDialog(hWin, 0);
                        break;
                    case GUI_KEY_RIGHT:	
                        SetWindowParam(0);
                        WM_InvalidateWindow(hWin);
                        break;
                    case GUI_KEY_1://删除雷管	                       
                        DlghWin.Dlg = WM_GetParent(hWin);						
                        GUI_EndDialog(hWin, 0);
                        
                        SetMsgBoxCaption("Del DET");//雷管删除
           
                        if(GetDetRegStatus(GetWaitRegListViewDlgSelIndex()) == ERROR)
                        {
                            sprintf((char*)Op_Data.Temp, "Hole %d-%d is unlogged", SelectHole.Name, SelectHole.Inside);//当前孔号%d-%d未注册
                            SetMsgBoxMessage((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            switch(ShowMsgBox())
                            {
                                case GUI_KEY_LEFT:                               
                                    ShowRegParamMenuDlg(DlgParam);
                                    break;
                                case GUI_KEY_RIGHT:
                                    break;                               
                            } 
                        }
                        else
                        {
                            SetMsgBoxMessage("Del This DET?");//确定删除雷管?
                            sprintf((char*)Op_Data.Temp, "Log DET:%d",Det.RegCount);//注册雷管:
                            SetMsgBoxHelp1((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Cancel");
                            SetMsgBoxRight("OK");
                            
                            switch(ShowMsgBox())
                            {
                                case GUI_KEY_LEFT:                               
                                    ShowRegParamMenuDlg(DlgParam);
                                    break;
                                case GUI_KEY_RIGHT:
                                    if(DeleteDetInfoNotChangeRank(&SelectHole, &SelectHole) == ERROR)
                                    {
                                        ShowSDCardError();
                                    }
                                    else
                                    {
                                        if(Det.RegCount == 0)
                                        {
                                            SetWaitRegListViewDlgSelIndex(RegHoleInit());//初始化待注册孔
                                        }
                                        
                                        ShowWaitRegListViewDlg();
                                    }
                                    break;                               
                            }                        
                        }
                        break;
                    case GUI_KEY_2://删除全部	
                        DlgParam = GetWindowParam();
                        DlghWin.Dlg = WM_GetParent(hWin);						
                        GUI_EndDialog(hWin, 0);
                        
                        MsgBoxInit();
                        SetMsgBoxCaption("Del DET");//雷管删除
                        SetMsgBoxMessage("Del All DETs?");//确定删除所有雷管?
                        sprintf((char*)Op_Data.Temp, "Log DET:%d",Det.RegCount);//注册雷管:
                        SetMsgBoxHelp1((char*)Op_Data.Temp);
                        SetMsgBoxLeft("Cancel");
                        SetMsgBoxRight("OK");
                        
                        switch(ShowMsgBox())
                        {
                            case GUI_KEY_LEFT:                               
                                ShowRegParamMenuDlg(DlgParam);
                                break;
                            case GUI_KEY_RIGHT:
                                if(DeleteAllDetInfo() == ERROR)
                                {
                                    ShowSDCardError();
                                }
                                else
                                {
                                    SetWaitRegListViewDlgSelIndex(RegHoleInit());//初始化待注册孔
                                    ShowWaitRegListViewDlg();
                                }
                                break;                               
                        }                        
                        break;
                    case GUI_KEY_3://插    入
                        if(SelectHole.Name >= Det.Info[DET_MAX_SIZE-1].Hole.Name)
                        {
                            SetMsgBoxMessage("This place can‘t \n\n insert Hole No.!");//该位置无法插入孔号!
                        }
                        else if(Det.RegCount > 0)
                        {                            
                            if((GetRegDetHoleMax() >= DET_MAX_SIZE) || (GetDetRegStatus(DET_MAX_SIZE-1) != ERROR))
                            {    
                                SetMsgBoxMessage("This place can‘t \n\n insert Hole No.!");//无法插入孔号!
                            }
                        }
                        if(MsgBoxData.Message[0] != NULL)
                        {
                            DlghWin.Dlg = WM_GetParent(hWin);						
                            GUI_EndDialog(hWin, 0);
                            
                            SetMsgBoxCaption("Insert Hole No.");//插入孔号
                            SetMsgBoxLeft("Back");  
                            switch(ShowMsgBox())
                            {
                                case GUI_KEY_LEFT:                               
                                    ShowRegParamMenuDlg(DlgParam);
                                    break;                                
                            }
                            return;                        
                        }
                        ShowRegHoleInsertInputDlg();
                        break;
                    case GUI_KEY_4://单发测试
                        if(Op_Data.LinABCurrent > 30)
                        {
                            DlgParam = GetWindowParam();
                            DlghWin.Dlg = WM_GetParent(hWin);						
                            GUI_EndDialog(hWin, 0);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("DET Test");//雷管测试
                            SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管!
                            sprintf((char*)Op_Data.Temp, "Log DET:%d",Det.RegCount);//注册雷管:
                            SetMsgBoxHelp1((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowRegParamMenuDlg(DlgParam);
                            } 
                        }
                        else
                        {
                            ShowHelpToolDetTestDlg(1);
                        }
                        break;
                    case GUI_KEY_5://内码重写
                        if(Op_Data.LinABCurrent > 30)
                        {
                            DlgParam = GetWindowParam();
                            DlghWin.Dlg = WM_GetParent(hWin);						
                            GUI_EndDialog(hWin, 0);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Rewrite Chip ID");//内码重写
                            SetMsgBoxMessage("Permit a DET on bus!");//总线只能接入1发雷管!
                            sprintf((char*)Op_Data.Temp, "Log DET:%d",Det.RegCount);//注册雷管:
                            SetMsgBoxHelp1((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowRegParamMenuDlg(DlgParam);
                            } 
                        }
                        else
                        {
                            ShowDetCodeWriteMenuDlg(0);
                        }
                        break;
                    case GUI_KEY_6://手动输入管码,批量注册
                        if(GetDetRegStatus(GetWaitRegListViewDlgSelIndex()) == ERROR)
                        {
                            ShowRegDetOutsideInputBatchDlg();
                        }
                        else
                        {
                            DlgParam = GetWindowParam();
                            DlghWin.Dlg = WM_GetParent(hWin);						
                            GUI_EndDialog(hWin, 0);
                            
                            MsgBoxInit();
                            SetMsgBoxCaption("Logging DETs");//雷管注册
                            SetMsgBoxMessage("Selected hole \n\nhas been logged!");//选择的孔已注册!
                            sprintf((char*)Op_Data.Temp, "Log DET:%d",Det.RegCount);//注册雷管:
                            SetMsgBoxHelp1((char*)Op_Data.Temp);
                            SetMsgBoxLeft("Back");
                            if(ShowMsgBox() == GUI_KEY_LEFT)
                            {
                                ShowRegParamMenuDlg(DlgParam);
                            }                        
                        }
                        break;
                }            
            }
			break;		
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowRegParamMenuDlg(u32 Param)
{
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, DlghWin.Dlg, 105, 61);
    SetWindowParam(Param);
}
