#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 }
};

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param 对话框参数
  *         @arg @b 0 快速测试
  *         @arg @b 1 完全测试 
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[80];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();
            switch(GetWindowParam())
            {
                case 0:
                    ShowInfoTitle("Fast Test");//组网测试
                    break;
                case 1:
                    ShowInfoTitle("Fast Test");//组网测试
                    break;
                case 2:
                case 3:
                    ShowInfoTitle("Blast Progress");//起爆流程
                    break;
            }

			GetDetInfoString(&temp[0], Det.Type);	
			ShowInfoHelp(&temp[0], NULL);
			
			GUI_SetFont(&SMALL_FONT);
			GUI_DispStringHCenterAt("Initializing Test Dat",120,100);//正在初始化测试数据!
			GUI_DispStringHCenterAt("Pls Wait!",120,125);//请稍等!
			break;
		case WM_INIT_DIALOG://初始化界面
			Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_H_GOAL;
			
			OSTaskDel(APP_BACKGROUND_TASK_PRIO);
			TaskCreate(APP_BACKGROUND_TASK_PRIO, (void*)2);/*等待电流稳定*/

			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}
			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);			
			break;
		case WM_BACKGROUND_TASK_END://电流测试结束	
            switch(GetWindowParam())
            {
                case 0://快速测试
                    ShowProgbarQuickDlg();
                    break;
                case 1://完全测试
                    ShowProgbarCompleteDlg();
                    break;
                case 2://起爆测试
                    ShowProgbarBlastDlg();
                    break;
                case 3://起爆测试前的快速测试
                    ShowProgbarQuickBeforeBlastDlg();
                    break;
            }
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					if (Op_Data.BlastVerifyResult == '0')
					{//终止起爆
						BlastTerminationFromAppDlg();
						break;
					}				
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

void ShowWaitLinCurrentReadyDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
