#include "includes.h"
#include "GuiIncludes.h"

static WM_CALLBACK*     _pcbOldFrame = 0;


#define TIMER_HANDLE_MAX  3
typedef struct{
	WM_HTIMER Htimer;
	u8 Valid;
}TIMER_HANDLE_BUFFER;
static TIMER_HANDLE_BUFFER TimerHandleBuffer[TIMER_HANDLE_MAX];

/**
  *************************************************************************************
  * @brief  对话框回调函数
  * @param[in]  GUI消息
  * @retval 结果 
  * @author ZH
  * @date   2016年12月10日
  * @note   主要用于处理用户点击标题栏中的关闭按钮事件
  *************************************************************************************  
  */
static void CustomFrameCallback(WM_MESSAGE * pMsg) 
{
	WM_MESSAGE Message;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) 
			{
				if (WM_GetId(pMsg->hWinSrc) == GUI_ID_CLOSE) 
				{
					Message.MsgId  = WM_CUSTOM_CLOSE_FRAME;		
					WM_SendMessage(WM_GetClientWindow(pMsg->hWin), &Message);					
					return;//防止调用旧回调函数
				}
			}
			break;
		default:		
			break;
	}
	if (_pcbOldFrame) 
	{
		(*_pcbOldFrame)(pMsg);
	}	
}
/**
  *************************************************************************************
  * @brief  定义自己的对话框回调函数
  * @param[in]  FramehWin：对话框
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   无
  *************************************************************************************  
  */
void SetCustomFrameCallback(WM_HWIN FramehWin)
{
	_pcbOldFrame = WM_SetCallback(FramehWin, CustomFrameCallback);	
}

void ClearUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj)
{
	while(MULTIEDIT_GetTextSize(hObj) > 1)
	{						
		MULTIEDIT_AddKey(hObj, GUI_KEY_BACKSPACE);
	}	
}

void InitUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj, u32 Value)
{
	char temp[11];
	u8 i=0;
	
	sprintf(temp,"%u", Value);
	while(temp[i] != 0x00)
	{
		MULTIEDIT_AddKey(hObj, temp[i]);
		i++;
	}
}
void InitUserStringToMULTIEDIT(MULTIEDIT_HANDLE hObj, char* Str, u16 Len)
{
	while(Len--)
	{
		MULTIEDIT_AddKey(hObj, *Str++);
	}
}
u32 GetDecFromMULTIEDIT(MULTIEDIT_HANDLE hObj)
{
	char Buffer[50];
	u32 Value = 0;
	
	if(MULTIEDIT_GetTextSize(hObj) <= 1)
	{
		return 0;
	}
	
	MULTIEDIT_GetText(hObj, Buffer, sizeof(Buffer));
	sscanf((const char*)Buffer, "%u", &Value);
	
	return Value;
}
/**
  *************************************************************************************
  * @brief  设置对话框窗口参数
  * @param[in]  Param：参数
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   要求在生成对话框后马上调用
  *************************************************************************************  
  */
ErrorStatus SetWindowParam(u8 Param)
{
	if(WM_IsWindow(DlghWin.Dlg))
	{
		if (1 == WM_SetUserData(DlghWin.Dlg, &Param, 1))
		{
			return SUCCESS;
		}	
	}

	return ERROR;
}
/**
  *************************************************************************************
  * @brief  获取设置的对话框窗口参数
  * @param[in]  无
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   需要特别注意：新窗口在WM_INIT_DIALOG 中调用本函数不能获取到正确的参数值
  *************************************************************************************  
  */
u8 GetWindowParam(void)
{
	u8 Param = 0;
	
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_GetUserData(DlghWin.Dlg, &Param, 1);	
	}

	return Param;
}
/**
  *************************************************************************************
  * @brief  关闭当前对话框窗口及所有父窗口
  * @param[in]  hWin 当前对话框窗口
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   
  *************************************************************************************  
  */
void CloseWindowAndParent(WM_HWIN hWin)
{
	WM_HWIN Parent;
	
	if(GetCustomMessageBoxStatus())
	{
		ColseCustomMessageBoxAndParentForce();
		while(GetCustomMessageBoxStatus())
		{
			GUI_Delay(1);
		}		
	}
	else
	{
		while((WM_HBKWIN != hWin) && WM_IsWindow(hWin))
		{
			Parent = WM_GetParent(hWin);		
			GUI_EndDialog(hWin, 0);
			hWin = Parent;
		}	
	}	
}
/**
  *************************************************************************************
  * @brief  显示光标
  * @param[in]  TextWin：MULTIEDIT 句柄
  * @param[in]  state：光标状态 
  * @retval 结果 
  * @author ZH
  * @date   2016年12月10日
  * @note   
  *************************************************************************************  
  */
u8 ShowInputCursor(WM_HWIN hWin, u8 state)
{
	u8 LineCurrent, LineCharCount, LineCurrentCharCount;
	u16 U16Value;
	char Buffer[50];
	
	WM_SelectWindow(hWin);				
	GUI_SetBkColor(GUI_WHITE);
	if((state&0x01)==0)
	{
		GUI_SetColor(GUI_WHITE);				
	}
	else
	{
		GUI_SetColor(GUI_BLACK);
	}
//	if(WM_GetWindowOrgY(hWin) < 25)
//	{
//		GUI_DrawLine(GUI_GetCharDistX('1')*(MULTIEDIT_GetTextSize(hWin)-1)+4, 4, 
//			GUI_GetCharDistX('1')*(MULTIEDIT_GetTextSize(hWin)-1)+4, WM_GetWindowSizeY(hWin)-4);	
//	}
//	else
//	{
		//计算每行显示多少个字符
		LineCharCount = WM_GetWindowSizeX(hWin)/12;
		
		//当前在第几行显示
//		LineCurrent = (MULTIEDIT_GetTextSize(hWin)-1);
//		LineCurrent = LineCurrent/LineCharCount;
		
		//当前行显示的字符数 
//		LineCurrentCharCount = (MULTIEDIT_GetTextSize(hWin)-1)%LineCharCount;
		MULTIEDIT_GetText(hWin, (char*)Buffer, sizeof(Buffer));
		U16Value = GetUtf8StringCharsCount(Buffer);
		LineCurrentCharCount = ((U16Value>>8)*2 + (U16Value&0xFF))%LineCharCount;
		//当前在第几行显示
		LineCurrent = LineCurrentCharCount/LineCharCount;
		
		GUI_DrawLine(12*LineCurrentCharCount+4, 4+24*LineCurrent, 
			12*LineCurrentCharCount+4, 24*(LineCurrent+1));		
//	}
	return LineCurrent;
}
/**
  *************************************************************************************
  * @brief  显示光标
  * @param[in]  TextWin：MULTIEDIT 句柄
  * @retval 结果 
  * @author ZH
  * @date   2016年12月10日
  * @note   
  *************************************************************************************  
  */
u8 ShowCursor(WM_HWIN hWin)
{
	static u8 State = 0;
	
	State++;
	return ShowInputCursor(hWin, State);
}
/**
  *************************************************************************************
  * @brief  更新文本中进度点数量,用于显示操作状态
  * @param[in]  TextWin：TEXT 句柄
  * @retval 格式是否符合要求 ERROR 格式不符合要求
  * @author ZH
  * @date   2016年12月10日
  * @note   字符串最后3位必需是由点和空格组成且必需先点后空格，如"文件导出中.  "
  *************************************************************************************  
  */
ErrorStatus UpdateTextPointCount(WM_HWIN TextWin)
{
	char Buffer[50];
	u8 i,Len, StartIndex, Point = 0;
	
	TEXT_GetText(TextWin, Buffer,  sizeof(Buffer));
	Len = strlen(Buffer);
	if(Len >= 3)
	{
		StartIndex = Len-3;
		for(i=0; i<3; i++)
		{
			if(Buffer[StartIndex+i] == '.')
			{
				Point++;
			}
			else if(Buffer[StartIndex+i] == ' ')
			{
				break;
			}
			else
			{
				return ERROR;//格式不符合要求
			}
		}
		Point++;
		if(Point > 3)
		{
			Point = 0;
		}
		for(i=0; i<3; i++)
		{
			if(i < Point)
			{
				Buffer[StartIndex+i] = '.';
			}
			else
			{
				Buffer[StartIndex+i] = ' ';
			}
		}
		Buffer[StartIndex+i] = 0x00;
		
		TEXT_SetText(TextWin, Buffer);
	}
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  清空对话框定时器句柄缓冲区数据
  * @param[in]  无
  * @retval 结果 
  * @author ZH
  * @date   2016年12月10日
  * @note   无
  *************************************************************************************  
  */
void ClearTimerHandleBuffer(void)
{	
	u8 i;
	
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		TimerHandleBuffer[i].Valid = 0;
		TimerHandleBuffer[i].Htimer = 0;
	}
}
/**
  *************************************************************************************
  * @brief  保存对话框定时器句柄到缓冲区中
  * @param[in]  Htimer：TEXT 对话框定时器句柄
  * @retval 结果 成功或失败
  * @author ZH
  * @date   每个对话框初次使用时请先调用ClearTimerHandleBuffer
  *************************************************************************************  
  */
ErrorStatus SaveTimerHandleToBuffer(WM_HTIMER Htimer)
{
	u8 i;
	
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		if(TimerHandleBuffer[i].Valid)
		{
			if(WM_GetTimerId(TimerHandleBuffer[i].Htimer) == WM_GetTimerId(Htimer))
			{
				TimerHandleBuffer[i].Htimer = Htimer;
				return SUCCESS;
			}		
		}

	}
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		if(TimerHandleBuffer[i].Valid == 0)
		{
			TimerHandleBuffer[i].Htimer = Htimer;
			TimerHandleBuffer[i].Valid = 1;
			return SUCCESS;
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  从缓冲区中获取对话框定时器句柄
  * @param[in]  Id：定时器ID
  * @param[out]  Htimer：TEXT 对话框定时器句柄 
  * @retval 结果 
  * @author ZH
  * @date   2016年12月10日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetTimerHandleById(int Id, WM_HTIMER *Htimer)
{
	u8 i;
	
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		if(TimerHandleBuffer[i].Valid)
		{
			if(WM_GetTimerId(TimerHandleBuffer[i].Htimer) == Id)
			{
				if (Htimer != NULL)
				{
					*Htimer = TimerHandleBuffer[i].Htimer;			
				}
				return SUCCESS;			
			}
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  从缓冲区中删除指定的对话框定时器句柄
  * @param[in]  TextWin：Id：定时器ID
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   为保持数据一致性，尽可能使用RemoveTimerHandleAndDeleteById
  *************************************************************************************  
  */
void RemoveTimerHandleById(int Id)
{
	u8 i;
	
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		if(TimerHandleBuffer[i].Valid)
		{
			if(WM_GetTimerId(TimerHandleBuffer[i].Htimer) == Id)
			{
				TimerHandleBuffer[i].Valid = 0;
				TimerHandleBuffer[i].Htimer = 0;	
			}
		}
	}
}
/**
  *************************************************************************************
  * @brief  从缓冲区中删除指定的对话框定时器句柄,并调用WM_DeleteTimer进行删除定时器
  * @param[in]  TextWin：Id：定时器ID
  * @retval 无
  * @author ZH
  * @date   2016年12月10日
  * @note   
  *************************************************************************************  
  */
void RemoveTimerHandleAndDeleteById(int Id)
{
	u8 i;
	
	for(i=0; i<TIMER_HANDLE_MAX; i++)
	{
		if(TimerHandleBuffer[i].Valid)
		{
			if(WM_GetTimerId(TimerHandleBuffer[i].Htimer) == Id)
			{
				WM_DeleteTimer(TimerHandleBuffer[i].Htimer);
				TimerHandleBuffer[i].Valid = 0;
				TimerHandleBuffer[i].Htimer = 0;	
			}
		}
	}	
}

/**
  *************************************************************************************
  * @brief  获取当前字节串在字符串数组中的索引
  * @param[in]  StringArray 字符串数组，即可选择的显示项
  * @param[in]  ArrayLenght 字符串数组长度
  * @param[in]  Content 当前显示的内容  
  * @retval 索引，0xFFFF表示没有找到 
  * @author ZH
  * @date   2018年9月10日
  * @note   
  *************************************************************************************  
  */
u16 GetStringArrayIndex(const char* StringArray[], u16 ArrayLenght, const char* Content)
{
    u16 i;
      
    for(i=0; i<ArrayLenght; i++)
    {
        if(strcmp(Content, StringArray[i]) == 0)
        {
            return i;
        }
    }
    return 0xFFFF;
}
/**
  *************************************************************************************
  * @brief  上一个显示项
  * @param[in]  StringArray 字符串数组，即可选择的显示项
  * @param[in]  ArrayLenght 字符串数组长度
  * @param[in]  Content 当前显示的内容  
  * @retval 无 
  * @author ZH
  * @date   2018年9月10日
  * @note   
  *************************************************************************************  
  */
const char* GetStringArrayPrevious(const char* StringArray[], u16 ArrayLenght, const char* Content)
{
    u16 Index;
    
    Index = GetStringArrayIndex(StringArray, ArrayLenght, Content);
   
    if(Index == 0)
    {
        Index = ArrayLenght-1;
    }
    else
    {
        Index--;
    }
    return StringArray[Index];
}
/**
  *************************************************************************************
  * @brief  下一个显示项
  * @param[in]  StringArray 字符串数组，即可选择的显示项
  * @param[in]  ArrayLenght 字符串数组长度
  * @param[in]  Content 当前显示的内容  
  * @retval 无 
  * @author ZH
  * @date   2018年9月10日
  * @note   
  *************************************************************************************  
  */
const char* GetStringArrayNext(const char* StringArray[], u16 ArrayLenght, const char* Content)
{
    u16 Index;
    

    Index = GetStringArrayIndex(StringArray, ArrayLenght, Content);
    Index++;
    
    if(Index >= ArrayLenght)
    {
        Index = 0;
    }
    return StringArray[Index];
}

/**
  *************************************************************************************
  * @brief  显示上一个显示项
  * @param[in]  hObj 消息
  * @param[in]  StringArray 字符串数组，即可选择的显示项
  * @param[in]  ArrayLenght 字符串数组长度   
  * @retval 无 
  * @author ZH
  * @date   2018年9月10日
  * @note   
  *************************************************************************************  
  */
void ShowEditArrayPrevious(MULTIEDIT_HANDLE hObj,const char* StringArray[], u16 ArrayLenght)
{
    char Buffer[50];
    
    ShowInputCursor(hObj, 0);
    MULTIEDIT_GetText(hObj, Buffer, sizeof(Buffer));
    MULTIEDIT_SetText(hObj, GetStringArrayPrevious(StringArray, ArrayLenght, Buffer));
}
/**
  *************************************************************************************
  * @brief  显示下一个显示项
  * @param[in]  hObj 要显示的MULTIEDIT句柄
  * @param[in]  StringArray 字符串数组，即可选择的显示项
  * @param[in]  ArrayLenght 字符串数组长度   
  * @retval 无 
  * @author ZH
  * @date   2018年9月10日
  * @note   
  *************************************************************************************  
  */
void ShowEditArrayNext(MULTIEDIT_HANDLE hObj,const char* StringArray[], u16 ArrayLenght)
{
    char Buffer[50];
    
    ShowInputCursor(hObj, 0);
    MULTIEDIT_GetText(hObj, Buffer, sizeof(Buffer));
    MULTIEDIT_SetText(hObj, GetStringArrayNext(StringArray, ArrayLenght, Buffer)); 
}

