#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 320, 0,          0 }
};


/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 正常手动关机
  *         @arg @b 1 电池电量不足   
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
            GUI_SetColor(device_info.TextColor);
            GUI_SetBkColor(device_info.BackgroundColor);         
			GUI_Clear();
				
			GUI_SetFont(&NORMAL_FONT);
			if(GetWindowParam() == 0)
			{
				GUI_DispStringHCenterAt("Powering Off",120,120);//正在关机
				GUI_DispStringHCenterAt("Thanks For Use",120,150);//谢谢使用			
			}
			else
			{
				GUI_DispStringHCenterAt("Low Battery ",120,120);//电量不足
				GUI_DispStringHCenterAt("Powering Off",120,150);	//正在关机					
			}
			break;
		case WM_INIT_DIALOG://初始化界面
            Op_Data.OfflineMonitorBlastProcess = 0;
            Op_Data.DoubleKeySwitch = 0;
            CLOSE_DOUBLE_KEY_SW();        
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);
                    
                    if(GetWindowParam() == 0)
                    {
                        UTF8StringToGBK("关机", (char*)Op_Data.Temp);
                        WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);
                    }
                    else
                    {
                        UTF8StringToGBK("电量不足关机", (char*)Op_Data.Temp);
                        WriteSystemLog((char*)Op_Data.Temp, strlen((char*)Op_Data.Temp), NEW_STR_INFO);				
                    }
                    
                    WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_3000MS_ID, GUI_TIMER_3000MS, 0);
                    break;
				case GUI_TIMER_3000MS_ID:				
					Lcd_CloseShow();		   //关显示
					LCD_BRIGHT_L();//关背光
                    Op_Data.DoubleKeySwitch = 0;
                    CLOSE_DOUBLE_KEY_SW();
					OPEN_BUS_6_7V();
					AllDisCharge();
					POWER_OFF();
					OSTimeDlyHMSM(0,0,0,100);
					__disable_irq();
					__set_FAULTMASK(1);//关闭所有中断
					NVIC_SystemReset();//软件复位
					while(1);			
//					break;
				default:
					break;
			}		
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowPowerOffDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, 0);
    SetWindowParam(Param);
}
