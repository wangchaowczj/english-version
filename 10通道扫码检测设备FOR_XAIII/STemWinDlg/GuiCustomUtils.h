#ifndef _GUISTYLECUSTOM_H_
#define _GUISTYLECUSTOM_H_
#include "MULTIPAGE.h"

//extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongTi16;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongTi24;

typedef enum
{		
	WM_CUSTOM_CLOSE_FRAME = (WM_USER+1),
	WM_DET_TASK
}WM_USER_CUSTOM;

#define COLOR_DLG_BK           0xFFFFFF

#define BIG_FONT 		GUI_FontSongTi24
#define NORMAL_FONT 	GUI_FontSongTi24
#define SMALL_FONT  	GUI_FontSongTi24

//用户自定义按键代码，代码不能与GUI.H中定义的重复
#define GUI_CUSTOM_KEY_0                0x30
#define GUI_CUSTOM_KEY_1                0x31
#define GUI_CUSTOM_KEY_2                0x32
#define GUI_CUSTOM_KEY_3                0x33
#define GUI_CUSTOM_KEY_4                0x34
#define GUI_CUSTOM_KEY_5                0x35
#define GUI_CUSTOM_KEY_6                0x36
#define GUI_CUSTOM_KEY_7                0x37
#define GUI_CUSTOM_KEY_8                0x38
#define GUI_CUSTOM_KEY_9                0x39
#define GUI_CUSTOM_KEY_BACK             0x3A
#define GUI_CUSTOM_KEY_OK               0x3B
#define GUI_CUSTOM_KEY_POWER            0x3C
#define GUI_CUSTOM_KEY_F1               0x3D
#define GUI_CUSTOM_KEY_F2             	0x3E
#define GUI_CUSTOM_KEY_F3               0x3F
#define GUI_CUSTOM_KEY_F4               0x40
#define GUI_CUSTOM_KEY_FOOT             0x41


//定时器
#define GUI_TIMER_1MS_ID    	100
#define GUI_TIMER_2MS_ID    	101
#define GUI_TIMER_3MS_ID    	102
#define GUI_TIMER_4MS_ID    	103
#define GUI_TIMER_5MS_ID    	104
#define GUI_TIMER_6MS_ID    	105
#define GUI_TIMER_7MS_ID    	106
#define GUI_TIMER_8MS_ID    	107
#define GUI_TIMER_9MS_ID    	108
#define GUI_TIMER_10MS_ID    	109
#define GUI_TIMER_20MS_ID    	110
#define GUI_TIMER_30MS_ID    	111
#define GUI_TIMER_40MS_ID    	112
#define GUI_TIMER_50MS_ID    	113
#define GUI_TIMER_60MS_ID    	114
#define GUI_TIMER_70MS_ID    	115
#define GUI_TIMER_80MS_ID    	116
#define GUI_TIMER_90MS_ID    	117
#define GUI_TIMER_100MS_ID    	118
#define GUI_TIMER_150MS_ID    	119
#define GUI_TIMER_200MS_ID    	120
#define GUI_TIMER_300MS_ID    	121
#define GUI_TIMER_500MS_ID    	122
#define GUI_TIMER_800MS_ID    	123
#define GUI_TIMER_1000MS_ID   	124
#define GUI_TIMER_1001MS_ID   	125
#define GUI_TIMER_1002MS_ID   	126
#define GUI_TIMER_1003MS_ID   	127
#define GUI_TIMER_1004MS_ID   	128
#define GUI_TIMER_1500MS_ID    	129
#define GUI_TIMER_2000MS_ID   	130
#define GUI_TIMER_3000MS_ID     132
#define GUI_TIMER_5000MS_ID     132
#define GUI_TIMER_8000MS_ID     133
#define GUI_TIMER_10000MS_ID    134
#define GUI_TIMER_5MIN_ID       135
#define GUI_TIMER_120MS_ID    	136

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
#define GUI_TIMER_5MIN  	300000UL


#define GET_ARRAY_LENGTH(array) (sizeof(array))/(sizeof(array[0]))


//屏蔽文件编码格式引起的870警告
#pragma  diag_suppress 870

typedef struct
{		
	WM_HWIN Dlg;
	WM_HWIN Status;
}DLG_HWIN;
extern DLG_HWIN DlghWin;

extern void SetCustomFrameCallback(WM_HWIN FramehWin);
extern void ClearUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj);
extern void InitUserDataToMULTIEDIT(MULTIEDIT_HANDLE hObj, u32 Value);
extern void InitUserStringToMULTIEDIT(MULTIEDIT_HANDLE hObj, char* Str, u16 Len);
extern u32 GetDecFromMULTIEDIT(MULTIEDIT_HANDLE hObj);
extern ErrorStatus SetWindowParam(u8 Param);
extern u8 GetWindowParam(void);
extern void CloseWindowAndParent(WM_HWIN hWin);
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

#endif
