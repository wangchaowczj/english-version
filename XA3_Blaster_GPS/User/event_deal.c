#include "includes.h"
#include "display.h"
#include "lcd_show.h"
//#include "DeInfo.h"
#include "event_deal.h"
//#include "scanner.h"
#include "color.h"
#include <math.h>




u16 TempDataIn[20];				//用户输入的数据(临时保存)
u16 TempStep = 0;				//用户临时步骤
u16 TempOverTime = 0;			//用于等待超时时间的数值
u8 USBDkeyCheck = 0;			//用于检查密钥的连接状态。1-检查，0-不检查。如果检查到密钥拔出，则有可能会直接出现提示。
u8 DelaySetState = 0;			//批量设置延期时，用于表示孔间延期和孔内延期是否还需要输入(BIT0表示孔间延期，BIT1表示孔内延期)
u8 DetVolTempState = 0;			//在雷管注册和雷管通信界面时，记录当前是雷管注册还是雷管通信。用于在判断电压异常时，返回到哪个界面




//=================================================================================
//| 函数名称 | Event_SetLcdKeyBrightTime
//|----------|---------------------------------------------------------------------
//| 函数功能 | 设置键盘背光的显示时间
//|----------|---------------------------------------------------------------------
//| 输入参数 | NowState：是否修改屏幕亮度	1-修改，0-不修改
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void Event_SetLcdKeyBrightTime(u8 NowState)
{
	if (AT45SetInfo.KeySwitch)		//键盘背光打开
	{
		switch (AT45SetInfo.KeyTime)
		{
			case 1:
				NowKeyCount = (2*2);		//背光时间为2秒
				break;
			case 2:
				NowKeyCount = (5*2);		//背光时间为5秒
				break;
			case 3:
				NowKeyCount = (10*2);		//背光时间为10秒
				break;
			case 4:
				NowKeyCount = (15*2);		//背光时间为15秒
				break;
			case 5:
				NowKeyCount = (20*2);		//背光时间为20秒
				break;
			case 6:
				NowKeyCount = (30*2);		//背光时间为30秒
				break;
			case 7:
				NowKeyCount = (60*2);		//背光时间为1分
				break;
			case 8:
				NowKeyCount = 0xFFFF;		//背光时间为常亮
				break;
			default:
				NowKeyCount = 0xFFFF;
				break;
		}
		if (Charge_Light == 0)
		{
			KEY_LED_OPEN();					//在进入该函数时，就打开了键盘灯
		}
	}
	else
	{
		NowKeyCount = 0;			//关闭背光(背光关闭前，该位为1，关闭后，该位为0)
	}

	if (NowState)
	{
		if (Charge_Light == 0)
		{
			CLcd_BrightCtrl(32-(AT45SetInfo.LcdBright*2)+1);
		}
	}

	switch (AT45SetInfo.LcdTime)
	{
		case 1:
			NowLcdCount = (5*2);		//背光时间为5秒
			break;
		case 2:
			NowLcdCount = (10*2);		//背光时间为10秒
			break;
		case 3:
			NowLcdCount = (15*2);		//背光时间为15秒
			break;
		case 4:
			NowLcdCount = (20*2);		//背光时间为20秒
			break;
		case 5:
			NowLcdCount = (30*2);		//背光时间为30秒
			break;
		case 6:
			NowLcdCount = (45*2);		//背光时间为45秒
			break;
		case 7:
			NowLcdCount = (60*2);		//背光时间为1分
			break;
		case 8:
			NowLcdCount = 0xFFFF;		//背光时间为常亮
			break;
		default:
			NowLcdCount = 0xFFFF;
			break;
	}




}







