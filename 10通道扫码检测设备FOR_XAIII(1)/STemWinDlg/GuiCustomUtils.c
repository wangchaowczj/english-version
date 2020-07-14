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
  * @brief  �Ի���ص�����
  * @param[in]  GUI��Ϣ
  * @retval ��� 
  * @author ZH
  * @date   2016��12��10��
  * @note   ��Ҫ���ڴ����û�����������еĹرհ�ť�¼�
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
					return;//��ֹ���þɻص�����
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
  * @brief  �����Լ��ĶԻ���ص�����
  * @param[in]  FramehWin���Ի���
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
  * @note   ��
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
  * @brief  ���öԻ��򴰿ڲ���
  * @param[in]  Param������
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
  * @note   Ҫ�������ɶԻ�������ϵ���
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
  * @brief  ��ȡ���õĶԻ��򴰿ڲ���
  * @param[in]  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
  * @note   ��Ҫ�ر�ע�⣺�´�����WM_INIT_DIALOG �е��ñ��������ܻ�ȡ����ȷ�Ĳ���ֵ
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
  * @brief  �رյ�ǰ�Ի��򴰿ڼ����и�����
  * @param[in]  hWin ��ǰ�Ի��򴰿�
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
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
  * @brief  ��ʾ���
  * @param[in]  TextWin��MULTIEDIT ���
  * @param[in]  state�����״̬ 
  * @retval ��� 
  * @author ZH
  * @date   2016��12��10��
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
		//����ÿ����ʾ���ٸ��ַ�
		LineCharCount = WM_GetWindowSizeX(hWin)/12;
		
		//��ǰ�ڵڼ�����ʾ
//		LineCurrent = (MULTIEDIT_GetTextSize(hWin)-1);
//		LineCurrent = LineCurrent/LineCharCount;
		
		//��ǰ����ʾ���ַ��� 
//		LineCurrentCharCount = (MULTIEDIT_GetTextSize(hWin)-1)%LineCharCount;
		MULTIEDIT_GetText(hWin, (char*)Buffer, sizeof(Buffer));
		U16Value = GetUtf8StringCharsCount(Buffer);
		LineCurrentCharCount = ((U16Value>>8)*2 + (U16Value&0xFF))%LineCharCount;
		//��ǰ�ڵڼ�����ʾ
		LineCurrent = LineCurrentCharCount/LineCharCount;
		
		GUI_DrawLine(12*LineCurrentCharCount+4, 4+24*LineCurrent, 
			12*LineCurrentCharCount+4, 24*(LineCurrent+1));		
//	}
	return LineCurrent;
}
/**
  *************************************************************************************
  * @brief  ��ʾ���
  * @param[in]  TextWin��MULTIEDIT ���
  * @retval ��� 
  * @author ZH
  * @date   2016��12��10��
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
  * @brief  �����ı��н��ȵ�����,������ʾ����״̬
  * @param[in]  TextWin��TEXT ���
  * @retval ��ʽ�Ƿ����Ҫ�� ERROR ��ʽ������Ҫ��
  * @author ZH
  * @date   2016��12��10��
  * @note   �ַ������3λ�������ɵ�Ϳո�����ұ����ȵ��ո���"�ļ�������.  "
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
				return ERROR;//��ʽ������Ҫ��
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
  * @brief  ��նԻ���ʱ���������������
  * @param[in]  ��
  * @retval ��� 
  * @author ZH
  * @date   2016��12��10��
  * @note   ��
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
  * @brief  ����Ի���ʱ���������������
  * @param[in]  Htimer��TEXT �Ի���ʱ�����
  * @retval ��� �ɹ���ʧ��
  * @author ZH
  * @date   ÿ���Ի������ʹ��ʱ���ȵ���ClearTimerHandleBuffer
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
  * @brief  �ӻ������л�ȡ�Ի���ʱ�����
  * @param[in]  Id����ʱ��ID
  * @param[out]  Htimer��TEXT �Ի���ʱ����� 
  * @retval ��� 
  * @author ZH
  * @date   2016��12��10��
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
  * @brief  �ӻ�������ɾ��ָ���ĶԻ���ʱ�����
  * @param[in]  TextWin��Id����ʱ��ID
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
  * @note   Ϊ��������һ���ԣ�������ʹ��RemoveTimerHandleAndDeleteById
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
  * @brief  �ӻ�������ɾ��ָ���ĶԻ���ʱ�����,������WM_DeleteTimer����ɾ����ʱ��
  * @param[in]  TextWin��Id����ʱ��ID
  * @retval ��
  * @author ZH
  * @date   2016��12��10��
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
  * @brief  ��ȡ��ǰ�ֽڴ����ַ��������е�����
  * @param[in]  StringArray �ַ������飬����ѡ�����ʾ��
  * @param[in]  ArrayLenght �ַ������鳤��
  * @param[in]  Content ��ǰ��ʾ������  
  * @retval ������0xFFFF��ʾû���ҵ� 
  * @author ZH
  * @date   2018��9��10��
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
  * @brief  ��һ����ʾ��
  * @param[in]  StringArray �ַ������飬����ѡ�����ʾ��
  * @param[in]  ArrayLenght �ַ������鳤��
  * @param[in]  Content ��ǰ��ʾ������  
  * @retval �� 
  * @author ZH
  * @date   2018��9��10��
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
  * @brief  ��һ����ʾ��
  * @param[in]  StringArray �ַ������飬����ѡ�����ʾ��
  * @param[in]  ArrayLenght �ַ������鳤��
  * @param[in]  Content ��ǰ��ʾ������  
  * @retval �� 
  * @author ZH
  * @date   2018��9��10��
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
  * @brief  ��ʾ��һ����ʾ��
  * @param[in]  hObj ��Ϣ
  * @param[in]  StringArray �ַ������飬����ѡ�����ʾ��
  * @param[in]  ArrayLenght �ַ������鳤��   
  * @retval �� 
  * @author ZH
  * @date   2018��9��10��
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
  * @brief  ��ʾ��һ����ʾ��
  * @param[in]  hObj Ҫ��ʾ��MULTIEDIT���
  * @param[in]  StringArray �ַ������飬����ѡ�����ʾ��
  * @param[in]  ArrayLenght �ַ������鳤��   
  * @retval �� 
  * @author ZH
  * @date   2018��9��10��
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

