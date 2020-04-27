#ifndef _GUISTYLECUSTOM_H_
#define _GUISTYLECUSTOM_H_
#include "MULTIPAGE.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongTi16;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongTi24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongTi48;

#define BIG_FONT 		GUI_FontSongTi48
#define NORMAL_FONT 	GUI_FontSongTi24
#define SMALL_FONT  	GUI_FontSongTi16

typedef enum
{		
	WM_UPDATE_PROGBAR = (WM_USER+1),
    WM_BACKGROUND_TASK_END,
    WM_SET_PROGBAR_PARAM,
    WM_SAFE_MONITOR_TASK_END,
    WM_SOFTWARE_UPDATE,
    WM_SHOW_GPS_WJ_UPDATE,
    WM_WIFI_TASK_END
}WM_USER_CUSTOM;


//用户自定义按键代码，代码不能与GUI.H中定义的重复
#define GUI_KEY_0                0x30
#define GUI_KEY_1                0x31
#define GUI_KEY_2                0x32
#define GUI_KEY_3                0x33
#define GUI_KEY_4                0x34
#define GUI_KEY_5                0x35
#define GUI_KEY_6                0x36
#define GUI_KEY_7                0x37
#define GUI_KEY_8                0x38
#define GUI_KEY_9                0x39
#define GUI_KEY_POWER            0x3A
#define GUI_KEY_DOUBLE           0x3B

//定时器
typedef enum
{		
	GUI_TIMER_1MS_ID = 100,
	GUI_TIMER_2MS_ID,
	GUI_TIMER_3MS_ID,
	GUI_TIMER_4MS_ID,
	GUI_TIMER_5MS_ID,
	GUI_TIMER_6MS_ID,
	GUI_TIMER_7MS_ID,
	GUI_TIMER_8MS_ID,
	GUI_TIMER_9MS_ID,
	GUI_TIMER_10MS_ID,
	GUI_TIMER_20MS_ID,
	GUI_TIMER_30MS_ID,
	GUI_TIMER_40MS_ID,
	GUI_TIMER_50MS_ID,
	GUI_TIMER_60MS_ID,
	GUI_TIMER_70MS_ID,
	GUI_TIMER_80MS_ID,
	GUI_TIMER_90MS_ID,
	GUI_TIMER_100MS_ID,
    GUI_TIMER_120MS_ID,
	GUI_TIMER_150MS_ID,
	GUI_TIMER_200MS_ID,
	GUI_TIMER_300MS_ID,
	GUI_TIMER_500MS_ID,
	GUI_TIMER_800MS_ID,
	GUI_TIMER_1000MS_ID,
	GUI_TIMER_1001MS_ID,
	GUI_TIMER_1002MS_ID,
	GUI_TIMER_1003MS_ID,
	GUI_TIMER_1004MS_ID,
	GUI_TIMER_1500MS_ID,
	GUI_TIMER_2000MS_ID,
	GUI_TIMER_3000MS_ID,
	GUI_TIMER_5000MS_ID,
	GUI_TIMER_8000MS_ID,
	GUI_TIMER_10000MS_ID,
	GUI_TIMER_20000MS_ID,
	GUI_TIMER_30000MS_ID,
	GUI_TIMER_5MIN_ID	
}CUSTOM_GUI_TIMER_ID;


#define GUI_TIMER_1MS     	1
#define GUI_TIMER_2MS     	2
#define GUI_TIMER_3MS     	3
#define GUI_TIMER_4MS     	4
#define GUI_TIMER_5MS     	5
#define GUI_TIMER_6MS     	6
#define GUI_TIMER_7MS     	7
#define GUI_TIMER_8MS     	8
#define GUI_TIMER_9MS     	9
#define GUI_TIMER_10MS     	10
#define GUI_TIMER_20MS     	20
#define GUI_TIMER_30MS     	30
#define GUI_TIMER_40MS     	40
#define GUI_TIMER_50MS     	50
#define GUI_TIMER_60MS     	60
#define GUI_TIMER_70MS     	70
#define GUI_TIMER_80MS     	80
#define GUI_TIMER_90MS     	90
#define GUI_TIMER_100MS    	100
#define GUI_TIMER_120MS     120
#define GUI_TIMER_150MS     150
#define GUI_TIMER_200MS    	200
#define GUI_TIMER_300MS    	300
#define GUI_TIMER_500MS    	500
#define GUI_TIMER_800MS    	800
#define GUI_TIMER_1000MS   	1000
#define GUI_TIMER_1001MS   	1001
#define GUI_TIMER_1002MS   	1002
#define GUI_TIMER_1003MS   	1003
#define GUI_TIMER_1004MS   	1004
#define GUI_TIMER_1500MS   	1500
#define GUI_TIMER_2000MS   	2000
#define GUI_TIMER_3000MS   	3000   
#define GUI_TIMER_5000MS   	5000
#define GUI_TIMER_8000MS   	8000
#define GUI_TIMER_10000MS  	10000
#define GUI_TIMER_20000MS  	20000
#define GUI_TIMER_30000MS  	30000
#define GUI_TIMER_5MIN  	300000UL


#define GET_ARRAY_LENGTH(array) (sizeof(array))/(sizeof(array[0]))


//屏蔽文件编码格式引起的870警告
#pragma  diag_suppress 870

typedef struct
{		
	WM_HWIN Dlg;
	WM_HWIN Status;
    WM_HWIN Loading;
    WM_HMEM Timer;
    WM_HWIN CurrentInput;
    u8 KeyboardStatus;
    u8 KeyboardText[11];
}DLG_HWIN;
extern DLG_HWIN DlghWin;

typedef struct
{		
	u8 DetOperate;
}APP_TASK_RESULT;
extern APP_TASK_RESULT AppTaskResult;

//extern void SetCustomFrameCallback(WM_HWIN FramehWin);
extern void ClearUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj);
extern void InitUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj, u32 Value);
extern void InitUserStringToMULTIEDIT(MULTIEDIT_HANDLE hObj, char* Str, u16 Len);
extern u32 GetDecFromMULTIEDIT(MULTIEDIT_HANDLE hObj);
extern ErrorStatus SetWindowParam(u32 Param);
extern u32 GetWindowParam(void);
extern u8 WindowHasChild(void);
extern void CloseWindowAndParent(WM_HWIN hWin);
extern void CloseMainDialog(void);
extern u8 ShowCursor(WM_HWIN hWin);
extern u8 ShowInputCursor(WM_HWIN hWin, u8 state);
extern ErrorStatus UpdateTextPointCount(WM_HWIN TextWin);
extern void ClearTimerHandleBuffer(void);
extern ErrorStatus SaveTimerHandleToBuffer(WM_HTIMER Htimer);
extern ErrorStatus GetTimerHandleById(int Id, WM_HTIMER *Htimer);
extern void RemoveTimerHandleById(int Id);
extern void RemoveTimerHandleAndDeleteById(int Id);

extern u16 GetStringArrayIndex(const char* StringArray[], u16 ArrayLenght, const char* Content);
extern const char* GetStringArrayNext(const char* StringArray[], u16 ArrayLenght, const char* Content);
extern const char* GetStringArrayPrevious(const char* StringArray[], u16 ArrayLenght, const char* Content);
extern void ShowEditArrayPrevious(MULTIEDIT_HANDLE hObj,const char* StringArray[], u16 ArrayLenght);
extern void ShowEditArrayNext(MULTIEDIT_HANDLE hObj,const char* StringArray[], u16 ArrayLenght);
extern void ShowKeyboardCharSelectList(WM_HWIN hWin, WM_HWIN buttonHWin, WM_HWIN textHWin);
extern void CustomSendMessage(WM_HWIN hWin, WM_MESSAGE * p);

#endif
