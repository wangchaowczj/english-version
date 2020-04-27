#include "includes.h"
#include "GuiIncludes.h"


/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },	
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },
	{ SLIDER_CreateIndirect, "", GUI_ID_SLIDER0,     20, 120, 200, 20,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     0, 95, 240, 20,  0,          0 },
};
/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 液晶亮度
  *         @arg @b 1 液晶背光时间
  *         @arg @b 2 起爆延时  
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
        case WM_PAINT://绘制界面
            GUI_Clear();
            switch(GetWindowParam())
            {
                case 0:
                    ShowInfoTitle("LED Bright");//液晶亮度
                    break;
                case 1:
                    ShowInfoTitle("Backlighting Time");//背光时间
                    break;
                case 2:
                    ShowInfoTitle("Device Delaytime");//起爆延时
                    break;
            }
            GUI_SetFont(&SMALL_FONT);
            ShowInfoHelp("Pls use the Arrow Key", NULL);//请通过方向键选择合适值
            break;
        case WM_INIT_DIALOG://初始化界面	
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);//这条语句是后来加的	
            break;
        case WM_KEY:

            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_LEFT:				
                    ShowSystemSetMenuDlg();				
                    break;
                case GUI_KEY_2:
                    if(GetWindowParam() == 0)
                    {//液晶亮度
                        if(device_info.LcdBrightStrength < LCD_BRIGHT_RANK/2)
                        {
                            device_info.LcdBrightStrength++;
                        }

                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightStrength);
                        LcdBrightRefreshProcess(1);
                    }
                    else if(GetWindowParam() == 1)
                    {//背光时间
                        if(device_info.LcdBrightTime < 100)
                        {
                            device_info.LcdBrightTime++;
                        }
                        
                        if(device_info.LcdBrightTime == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is off");//当前背光为常暗
                        }
                        else if(device_info.LcdBrightTime >= 100)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is on");//当前背光为常亮
                        }
                        else 
                        {
                            if(device_info.LcdBrightTime >= 6)
                            {
                                sprintf(temp, "Backlight on time:%dm%ds",device_info.LcdBrightTime*10/60, device_info.LcdBrightTime*10%60);		//当前背光打开时间:%d分%d秒			
                            }
                            else
                            {
                                sprintf(temp, "Backlight on time:%ds",device_info.LcdBrightTime*10);//当前背光打开时间:%d秒
                                            
                            }
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightTime);
                        LcdBrightRefreshProcess(1);					
                    }
                    if(GetWindowParam() == 2)
                    {//起爆延时
                        if(device_info.BlastDelay < BLAST_DELAY_MAX)
                        {
                            device_info.BlastDelay++;
                        }

                        if(device_info.BlastDelay == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:0s");//当前起爆延时:0秒
                        }
                        else if(device_info.BlastDelay == 1)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:5s");//当前起爆延时:5秒
                        }
                        else 
                        {
                            sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);	//当前起爆延时:%d时%d分0秒					
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);				
                    }
                    if(SaveUserConfigParam() == ERROR)
                    {
                        ShowSDCardError();					
                    }				
                    break;				
                case GUI_KEY_8:
                    if(GetWindowParam() == 0)
                    {//液晶亮度
                        if(device_info.LcdBrightStrength > 1)
                        {
                            device_info.LcdBrightStrength--;
                        }

                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightStrength);
                        LcdBrightRefreshProcess(1);					
                    }
                    else if(GetWindowParam() == 1)
                    {//背光时间
                        if(device_info.LcdBrightTime != 0)
                        {
                            device_info.LcdBrightTime--;
                        }
                        
                        if(device_info.LcdBrightTime == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is off");//当前背光为常暗
                        }
                        else if(device_info.LcdBrightTime >= 100)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is on");//当前背光为常亮
                        }
                        else 
                        {
                            if(device_info.LcdBrightTime >= 6)
                            {
                                sprintf(temp, "Backlight on time:%dm%ds",device_info.LcdBrightTime*10/60, device_info.LcdBrightTime*10%60);	//当前背光打开时间:%d分%d秒				
                            }
                            else
                            {
                                sprintf(temp, "Backlight on time:%ds",device_info.LcdBrightTime*10);	//当前背光打开时间:%d秒								
                            }
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightTime);
                        LcdBrightRefreshProcess(1);
                    }
                    if(GetWindowParam() == 2)
                    {//起爆延时
                        if(device_info.BlastDelay != 0)
                        {
                            device_info.BlastDelay--;
                        }

                        if(device_info.BlastDelay == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:0s");//当前起爆延时:0秒
                        }
                        else if(device_info.BlastDelay == 1)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:5s");//当前起爆延时:5秒
                        }
                        else 
                        {
                            sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);	//当前起爆延时:%d时%d分0秒					
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);				
                    }
                    if(SaveUserConfigParam() == ERROR)
                    {
                        ShowSDCardError();					
                    }
                    break;
                case GUI_KEY_4:
                    if(GetWindowParam() == 0)
                    {//液晶亮度
                        if(device_info.LcdBrightStrength > 2)
                        {
                            device_info.LcdBrightStrength -= 2;
                        }
                        else
                        {
                            device_info.LcdBrightStrength = 1;
                        }

                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightStrength);
                        LcdBrightRefreshProcess(1);
                    }
                    else if(GetWindowParam() == 1)
                    {//背光时间
                        if(device_info.LcdBrightTime >= 6)
                        {
                            device_info.LcdBrightTime -= 6;
                        }
                        else
                        {
                            device_info.LcdBrightTime = 0;
                        }
                        
                        if(device_info.LcdBrightTime == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is off");//当前背光为常暗
                        }
                        else if(device_info.LcdBrightTime >= 100)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is on");//当前背光为常亮
                        }
                        else 
                        {
                            if(device_info.LcdBrightTime >= 6)
                            {
                                sprintf(temp, "Backlight on time:%dm%ds",device_info.LcdBrightTime*10/60, device_info.LcdBrightTime*10%60);	//当前背光打开时间:%d分%d秒				
                            }
                            else
                            {
                                sprintf(temp, "Backlight on time:%ds",device_info.LcdBrightTime*10);//当前背光打开时间:%d秒
                                            
                            }
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightTime);
                        LcdBrightRefreshProcess(1);
                    }
                    if(GetWindowParam() == 2)
                    {//起爆延时
                        if(device_info.BlastDelay >= 10)
                        {
                            device_info.BlastDelay -= 10;
                        }
                        else
                        {
                            device_info.BlastDelay = 0;
                        }

                        if(device_info.BlastDelay == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:0s");//当前起爆延时:0秒
                        }
                        else if(device_info.BlastDelay == 1)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:5s");//当前起爆延时:5秒
                        }
                        else 
                        {
                            sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);	//当前起爆延时:%d时%d分0秒					
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);				
                    }
                    if(SaveUserConfigParam() == ERROR)
                    {
                        ShowSDCardError();					
                    }				
                    break;
                case GUI_KEY_6:
                    if(GetWindowParam() == 0)
                    {//液晶亮度
                        device_info.LcdBrightStrength += 2;
                        if(device_info.LcdBrightStrength > LCD_BRIGHT_RANK/2)
                        {
                            device_info.LcdBrightStrength = LCD_BRIGHT_RANK/2;
                        }

                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightStrength);
                        LcdBrightRefreshProcess(1);
                    }
                    else if(GetWindowParam() == 1)
                    {//背光时间
                        device_info.LcdBrightTime += 6;
                        if(device_info.LcdBrightTime > 100)
                        {
                            device_info.LcdBrightTime = 100;
                        }
                        
                        if(device_info.LcdBrightTime == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is off");//当前背光为常暗
                        }
                        else if(device_info.LcdBrightTime >= 100)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is on");//当前背光为常亮
                        }
                        else 
                        {
                            if(device_info.LcdBrightTime >= 6)
                            {
                                sprintf(temp, "Backlight on time:%dm%ds",device_info.LcdBrightTime*10/60, device_info.LcdBrightTime*10%60);//当前背光打开时间:%d分%d秒					
                            }
                            else
                            {
                                sprintf(temp, "Backlight on time:%ds",device_info.LcdBrightTime*10);//当前背光打开时间:%d秒
                                            
                            }
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightTime);
                        LcdBrightRefreshProcess(1);
                    }
                    if(GetWindowParam() == 2)
                    {//起爆延时
                        device_info.BlastDelay += 10;
                        if(device_info.BlastDelay > BLAST_DELAY_MAX)
                        {
                            device_info.BlastDelay = BLAST_DELAY_MAX;
                        }			

                        if(device_info.BlastDelay == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:0s");//当前起爆延时:0秒
                        }
                        else if(device_info.BlastDelay == 1)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:5s");//当前起爆延时:5秒
                        }
                        else 
                        {
                            sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);//当前起爆延时:%d时%d分0秒						
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);				
                    }
                    if(SaveUserConfigParam() == ERROR)
                    {
                        ShowSDCardError();					
                    }				
                    break;					
            }
            break;
        case WM_TIMER:
            switch(WM_GetTimerId(pMsg->Data.v))
            {
                case GUI_TIMER_1MS_ID:
                    WM_DeleteTimer(pMsg->Data.v);

                    if(GetWindowParam() == 0)
                    {//液晶亮度			
                        SLIDER_SetRange(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 1, LCD_BRIGHT_RANK/2);
                        SLIDER_SetNumTicks(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 2);
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightStrength);			

                        WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_TEXT0));
                    }
                    else if(GetWindowParam() == 1)
                    {//背光时间
                        SLIDER_SetRange(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 0, 100);
                        SLIDER_SetNumTicks(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 5);
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.LcdBrightTime);			

                        if(device_info.LcdBrightTime == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is off");//当前背光为常暗
                        }
                        else if(device_info.LcdBrightTime >= 100)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current Backlighting is on");//当前背光为常亮
                        }
                        else 
                        {
                            if(device_info.LcdBrightTime >= 6)
                            {
                                sprintf(temp, "Backlight on time:%dm%ds",device_info.LcdBrightTime*10/60, device_info.LcdBrightTime*10%60);	//当前背光打开时间:%d分%d秒				
                            }
                            else
                            {
                                sprintf(temp, "Backlight on time:%ds",device_info.LcdBrightTime*10);//当前背光打开时间:%d秒
                                            
                            }
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                    }
                    else if(GetWindowParam() == 2)
                    {//起爆延时
                        SLIDER_SetRange(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 0, BLAST_DELAY_MAX);
                        SLIDER_SetNumTicks(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), 2);
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);			

                        if(device_info.BlastDelay == 0)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:0s");//当前起爆延时:0秒
                        }
                        else if(device_info.BlastDelay == 1)
                        {
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Current DLY:5s");//当前起爆延时:5秒
                        }
                        else 
                        {
                            sprintf(temp, "Current DLY:%dh%dm0s",(device_info.BlastDelay-1)/60, (device_info.BlastDelay-1)%60);	//当前起爆延时:%d时%d分0秒					
                            TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),temp);
                        }
                        SLIDER_SetValue(WM_GetDialogItem(hWin , GUI_ID_SLIDER0), device_info.BlastDelay);			
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
                    break;
                default:
                    break;
            }		
            break;		
        default:
            WM_DefaultProc(pMsg);
	}
}

void ShowSliderDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}

