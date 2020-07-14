#include "includes.h"
#include "GuiIncludes.h"


/*
*********************************************************************************************************
*	函 数 名: _cbBk
*	功能说明: 桌面窗口回调函数
*	形    参：pMsg  指针地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void DeskCallback(WM_MESSAGE * pMsg) 
{
//	uint32_t NCode, Id, sel;

	switch (pMsg->MsgId) 
	{
		/* 重绘消息 */
		case WM_PAINT:
			GUI_SetBkColor(GUI_RED);
			GUI_Clear();
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void SystemReset(void)
{
	POWER_OFF();
	OSTimeDlyHMSM(0,0,0,100);
	__disable_irq();
	__set_FAULTMASK(1);//关闭所有中断
	NVIC_SystemReset();//软件复位
	while(1);	
}
/*
*********************************************************************************************************
*	函 数 名: Demo Main menu
*	功能说明: 主菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DeskMenu(void) 
{	
	GUI_SetBkColor(GUI_BLUE);		//设置背景颜色
	GUI_Clear();					//清屏
//	OpenFontXBF();
	GUI_UC_SetEncodeUTF8();
	//GUI_SetFont(&GUI_Font24_ASCII); //设置字体
//	GUI_SetFont(&FontXBF.Big); //设置字体	
//	GUI_SetColor(GUI_YELLOW);       //设置字体颜色
	BUTTON_SetDefaultFont(&NORMAL_FONT);
	TEXT_SetDefaultFont(&NORMAL_FONT);
	FRAMEWIN_SetDefaultFont(&NORMAL_FONT);
	RADIO_SetDefaultFont(&NORMAL_FONT);
	
	LISTVIEW_SetDefaultFont(&NORMAL_FONT);
	HEADER_SetDefaultFont(&NORMAL_FONT);
	DROPDOWN_SetDefaultFont(&NORMAL_FONT);
	FRAMEWIN_SetDefaultTitleHeight(0);
	EDIT_SetDefaultFont(&NORMAL_FONT);
	
	
	TEXT_SetDefaultTextColor(GUI_BLUE);
	RADIO_SetDefaultTextColor(GUI_BLUE);
	
	//GUI_MOUSE_StoreState();//游标位置
//	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
//	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
//	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
//	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
//	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
//	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	SCROLLBAR_SetDefaultWidth(20);

	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_SEL, GUI_DARKGRAY);
	
 	//GUI_CURSOR_Show();
	/* 启用多缓冲 */
	WM_MULTIBUF_Enable(1);
	WM_EnableMemdev(WM_HBKWIN);
	
	WM_SetCreateFlags(WM_CF_MEMDEV);	
	
	/* 桌面窗口的回调函数 */
	WM_SetCallback(WM_HBKWIN, DeskCallback);

//	GUI_CURSOR_SetPosition(LCD_WIDTH_PIXEL/2,LCD_HEIGHT_PIXEL/2);//默认光标位置

	if((GetBatteryCapacityByVoltage(GetBatVoltage(NULL)) <= 5)&&(GET_AC_IN_STATE() == 0))		
	{//剩余容量5%，禁止开机
		OSTimeDly(200);
		if(GetBatteryCapacityByVoltage(GetBatVoltage(NULL)) <= 5)
		{
			u8 Count = 5;
			
			while(Count--)
			{
				OPEN_LED1();
				OPEN_LED2();
				OSTimeDly(50);
				CLOSE_LED1();
				CLOSE_LED2();
				OSTimeDly(150);			
			}
			SystemReset();//复位系统		
		}							
	}		
	/* 创建一个状态窗口 */
	ShowStatus();	
	ShowDeviceStartDlg();

	while(1)
	{
		GUI_Delay(1);
	}
}
