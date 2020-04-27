#include "includes.h"
#include "GuiIncludes.h"

//==================================================================================================
//| 函数名称 | Show_ClearBiaoti
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 清空标题栏
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void ShowClearTopTitle(void)
{
	LcdDrawFillRect(TOP_TITLE_X,TOP_TITLE_Y, TOP_TITLE_WIDTH -1, TOP_TITLE_HEIGHT-1,COLOR_TOP_BACK);
}

//==================================================================================================
//| 函数名称 | Show_BiaotiTime
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 在标题栏显示时间
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void ShowToTitleTime(void)
{
	WM_SelectWindow(WM_HBKWIN);												
	GUI_SetColor(COLOR_TOP_FONT);
	GUI_SetBkColor(COLOR_TOP_BACK);
	
	GUI_GotoXY(88, TOP_TITLE_Y+2);
	
	GUI_DispDec(RTC_Type.hour, 2);
	GUI_DispString(":");
	GUI_DispDec(RTC_Type.minute, 2);
	GUI_DispString(":");
	GUI_DispDec(RTC_Type.second, 2);

}


void ShowInfoTitle(char* ptr)
{
//	GUI_SetColor(INFO_TITLE_COLOR);
  GUI_SetColor(device_info.TextColor);
	GUI_SetFont(&NORMAL_FONT);
	GUI_DispStringHCenterAt(ptr, 120, INFO_TITLE_Y);		
	//绘制横线

	GUI_FillRect(INFO_TITLE_LINE_X, INFO_TITLE_LINE_Y, INFO_TITLE_LINE_X+INFO_TITLE_LINE_WIDTH, INFO_TITLE_LINE_Y+1);
	GUI_SetFont(&SMALL_FONT);
}
void ShowInfoHelp(char* Line1, char* Line2)
{
//	GUI_SetColor(INFO_HELP_COLOR);
  GUI_SetColor(device_info.TextColor);
	GUI_SetFont(&SMALL_FONT);
	
	GUI_FillRect(INFO_HELP_LINE_X, INFO_HELP_LINE_Y, 
		INFO_HELP_LINE_X+INFO_HELP_LINE_WIDTH, INFO_HELP_LINE_Y+1);		//绘制横线
	if((Line1 != NULL) && (Line2 != NULL))
	{//分2行显示
		GUI_DispStringHCenterAt(Line1, 120, INFO_HELP_Y1);//居中显示第1行
		GUI_DispStringHCenterAt(Line2, 120, INFO_HELP_Y2);//居中显示第2行
	}
	else if(Line1 != NULL)
	{
		GUI_DispStringHCenterAt(Line1, 120, INFO_HELP_Y);//居中显示	
	}
	else
	{
		GUI_DispStringHCenterAt(Line2, 120, INFO_TITLE_Y);//居中显示	
	}
}

void GetDetTypeName(char* Name, u8 Type)
{
	switch(Type)
	{
		case DET_TYPE_XA02:
			strcpy(Name, "XA3-02");	
			break;
		case DET_TYPE_XA06:
			strcpy(Name, "XA3-06");	
			break;			
		case DET_TYPE_XA21:
			strcpy(Name, "XA3-21");
			break;			
		default:
			strcpy(Name, "unknown");//未知
			break;
	}	
}
void GetDetInfoString(char* Info, u8 Type)
{
	switch(Type)
	{
		case DET_TYPE_XA02:
			sprintf(Info, "DET Type:XA3-02(0-%dms)", DET_DLY_MAX_XA02);	//雷管类型:XA3-02(0-%dms)",
			break;
		case DET_TYPE_XA06:
			sprintf(Info, "DET Type:XA3-06(0-%dms)", DET_DLY_MAX_XA06);//雷管类型:XA3-06(0-%dms)
			break;
		case DET_TYPE_XA21:
			sprintf(Info, "DET Type:XA3-21(0-%dms)", DET_DLY_MAX_XA21);//雷管类型:XA3-06(0-%dms)
			break;            
		default:
			strcpy(Info, "DET Type:unrecognized");//雷管类型:无法识别
			break;
	}	
}

//void GetGpsInfoString(GPS_INFO* Gps,u8 Type, char* Str)
//{
//	u8 i;
//	u32 Value = 0;
//	u8 Flag1 = 0xFF, Flag2 = 0xFF;
//	u8 du, fen;
//	u8 Buffer[50];
//	float miao;
//	
//	if((0 == Type)||(1 == Type))
//	{//经度,格式为"11620.1092,E", 纬度,格式为"3957.7800,N"
//		//先找到‘.’和','的位置
//		memset(Buffer, 0, 50);
//		if(0 == Type)
//		{			
//			memcpy(Buffer,Gps->GPS_Longitude, sizeof(Gps->GPS_Longitude));	
//		}
//		else
//		{
//			memcpy(Buffer,Gps->GPS_Latitude, sizeof(Gps->GPS_Latitude));				
//		}
//		for(i=0; i<strlen((const char*)Buffer); i++)
//		{
//			if(Flag1 == 0xFF)
//			{
//				if(Buffer[i] == '.')
//				{
//					Flag1 = i;
//				}			
//			}
//			else
//			{
//				if(Buffer[i] == ',')
//				{
//					Flag2 = i;
//					break;
//				}			
//			}
//		}
//		if((0xFF != Flag1) && (0xFF != Flag2) && (Flag1 > 2))
//		{
//			du = 0;
//			for(i=0;i<(Flag1-2);i++)
//			{
//				du *= 10;
//				du += Buffer[i]-0x30;
//			}
//			fen = 0;
//			for(i=Flag1-2;i<Flag1;i++)
//			{
//				fen *= 10;
//				fen += Buffer[i]-0x30;
//			}

//			Value = 0;
//			for(i=Flag1+1;i<Flag2;i++)
//			{
//				Value *= 10;
//				Value += Buffer[i]-0x30;
//			}
//			miao = Value;
//			for(i=0;i<(Flag2-Flag1-1);i++)
//			{
//				miao = (double)miao/10.0;
//			}
//			miao *= 60;
//			
//			if(0 == Type)
//			{
//				sprintf(Str,"经度:%d°%d′%2.2F″",du,fen,miao);			
//			}
//			else
//			{
//				sprintf(Str,"纬度:%d°%d′%2.2F″",du,fen,miao);			
//			}			
//		}
//		else
//		{
//			if(0 == Type)
//			{
//				strcpy(Str, "经度:数据格式错误");			
//			}
//			else
//			{
//				strcpy(Str, "纬度:数据格式错误");			
//			}
//		}
//	}
//	else if(2 == Type)
//	{//时间
//		sprintf((char*)Str, "时间:%04d-%02d-%02d %02d:%02d:%02d", (unsigned int)(Gps->GPS_BJ_year),
//													(unsigned int)Gps->GPS_BJmonth,
//													(unsigned int)Gps->GPS_BJday,
//													(unsigned int)Gps->GPS_BJhour,
//													(unsigned int)Gps->GPS_BJminute,
//													(unsigned int)Gps->GPS_BJsecond);
//															
//	}	
//}
//==================================================================================================
//| 函数名称 | Show_ChongPro
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 充电界面下的充电符号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | NowProcess：进度条进度	0-100：进度大小		0xFF：重新构建进度条
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：姚华光    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void ShowChargeFlag(u8 NowProcess)
{
	static u8 pro = 0;		//保存当前进度条前进的大小
	u8 state;
	u8 Len = CHONGLENX*20/100;		//每一格的大小
	u8 i;

	if (NowProcess == 0xFF)
	{
		pro = 0;
		LcdDrawFillRect(CHONGX-4,CHONGY-4,CHONGX+CHONGLENX+5,CHONGY+CHONGLENY+4,COLOR_CHO_FONT);					//清空电池区域
		LcdDrawFillRect(CHONGX+CHONGLENX+5,CHONGTY,CHONGX+CHONGLENX+5+CHONGTXL,CHONGTY+CHONGTYL,COLOR_CHO_FONT);	//显示电池的小头
		LcdDrawFillRect(CHONGX,CHONGY,CHONGX+CHONGLENX+1,CHONGY+CHONGLENY,COLOR_CHO_BACK);							//显示电池的空的地方
	}
	else
	{
		state = NowProcess/20;		//100%分成5个等份
		if (pro > state)			//进度条减少了
		{
			LcdDrawFillRect(CHONGX+(((u16)state)*Len),CHONGY,CHONGX+CHONGLENX,CHONGY+CHONGLENY,COLOR_CHO_BACK);//清空多余的进度条
		}
		else
		{
			for (i=0;i<state;i++)
			{
				LcdDrawFillRect(CHONGX+(((u16)i)*Len)+2,CHONGY+2,CHONGX+(((u16)i+1)*Len-1),CHONGY+CHONGLENY-2,COLOR_CHO_FONT);	//显示进度条
			}
		}
		pro = state;
	}
}

int ShowDkeyUserInfoError(u8 Result, u8 State)
{
	char temp[50];
    
    MsgBoxInit();
    SetMsgBoxCaption("Verification Failed");//验证失败
    
	if(Result == '2')
	{//该用户已自锁
		SetMsgBoxMessage("User Self-locked!");//该用户已自锁!
	}	
	else if(Result == '3')
	{//用户错误
		if (State== '0')
		{
			SetMsgBoxMessage("User Name Err!\n\nUser Self-locked!");//用户名错误!\n\n该用户已自锁!
		}
		else
		{
			sprintf(temp, "User Name ERR!\n\nRemaining Times:%d",State-'0');//用户名错误!\n\n剩余重试次数:
			SetMsgBoxMessage(temp);
		}						
	}
	else if(Result == '4')
	{//密码错误
		if (State== '0')
		{
			SetMsgBoxMessage("Password ERR!\n\nUser Self-locked!");//密码错误!\n\n该用户已自锁!
		}
		else
		{
			sprintf(temp, "Password ERR!\n\nRemaining Times:%d",State-'0');//密码错误!\n\n剩余重试次数:
			SetMsgBoxMessage(temp);
		}		
	}						
	else 
	{
		SetMsgBoxMessage("Can't connect to the Dongle!");//无法连接数字密钥!
	}
    SetMsgBoxLeft("Back");//返回
    return ShowMsgBox();
}
int ShowSDCardError(void)
{
    MsgBoxInit();
    SetMsgBoxCaption("Device Abnormal");//设备异常
    SetMsgBoxMessage("Dat Save Failed!");//数据保存失败!
    SetMsgBoxHelp1("Is your SD card installed?");//请检查SD卡是否安装可靠

    return ShowMsgBox();
}
void SetProgbarParam(void)
{
	WM_MESSAGE Msg;	
	
	Msg.MsgId = WM_SET_PROGBAR_PARAM;
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
}

void UpdateProgbar(u16 Value)
{
	WM_MESSAGE Msg;	
	
	Msg.MsgId = WM_UPDATE_PROGBAR;
	Msg.Data.v = Value;
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
}

void SetGuiDefaultColor(u32 BackgroundColor, u32 TextColor)
{
  GUI_SetBkColor(BackgroundColor);		//设置背景颜色
	GUI_SetColor(TextColor);       //设置字体颜色	
	GUI_SetDefaultFont(&SMALL_FONT);
    
	WINDOW_SetDefaultBkColor(BackgroundColor);
	
	BUTTON_SetDefaultBkColor(GUI_BLUE, 0);
	BUTTON_SetDefaultTextColor(GUI_WHITE, 0);	
	BUTTON_SetDefaultFont(&SMALL_FONT);
	
	TEXT_SetDefaultFont(&SMALL_FONT);
	TEXT_SetDefaultTextColor(TextColor);
	
	RADIO_SetDefaultFont(&SMALL_FONT);
	RADIO_SetDefaultTextColor(TextColor);
	
	EDIT_SetDefaultFont(&SMALL_FONT);
	EDIT_SetDefaultTextColor(0, TextColor);

	HEADER_SetDefaultFont(&SMALL_FONT);
	LISTVIEW_SetDefaultFont(&SMALL_FONT);
}
u32 GetDifferentColor(void)
{
    if ((device_info.BackgroundColor != GUI_WHITE) && (device_info.TextColor != GUI_WHITE))
    {
        return GUI_WHITE;
    }
    if ((device_info.BackgroundColor != GUI_BLUE) && (device_info.TextColor != GUI_BLUE))
    {
        return GUI_BLUE;
    }
    if ((device_info.BackgroundColor != GUI_BLACK) && (device_info.TextColor != GUI_BLACK))
    {
        return GUI_BLACK;
    }     
    if ((device_info.BackgroundColor != GUI_GREEN) && (device_info.TextColor != GUI_GREEN))
    {
        return GUI_GREEN;
    }

    return GUI_DARKGRAY;
}
void GetAccreditErrorString1ForDL(char* Info, u8 Code)
{
	switch(Code)
	{
		case '1':
			strcpy(Info, "DET in the blacklist!");//雷管在黑名单中!
			break;
		case '2':
			strcpy(Info, "Used DET!");//雷管已使用!
			break;
		case '3':
        case 0xF1:
        case 0xF2:
			strcpy(Info, "Unauthorized DET!");//雷管未授权!//含义是申请的雷管UID不存在
			break;
        case 0xF0:
			strcpy(Info, "Expired DET!");//雷管不在有效期!
			break;
        case 0xFF:
			strcpy(Info, "AUZ info undownloaded!");//授权信息未下载!
			break;            
		default:
			sprintf(Info, "Authorization ERR(%02X)", Code);//授权错误(%02X)
			break;
	}	
}
void GetAccreditErrorString2ForDL(char* Info, u8 Code)
{
	switch(Code)
	{
		case '1':
			strcpy(Info, "Blacklist");//黑名单
			break;
		case '2':
			strcpy(Info, "Used");//已使用
			break;
		case '3':
        case 0xF1:
        case 0xF2:
			strcpy(Info, "Unauthorized");//未授权//含义是申请的雷管UID不存在
			break;
        case 0xF0:
			strcpy(Info, "Expired");//不在有效期
			break;
        case 0xFF:
			strcpy(Info, "Info Undownloaded");//信息未下载
			break;             
		default:
			sprintf(Info, "Abnormal(%02X)", Code);//异常(%02X)
			break;
	}	
}

