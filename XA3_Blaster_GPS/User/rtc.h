#ifndef _RTC_H_
#define _RTC_H_

#define RTC_CLOCK_SOURCE_LSE		//使用外部晶振
//#define RTC_CLOCK_SOURCE_LSI		//使用内部晶振

typedef struct RTC_DATA
{
	u16 year;		//年
	u8 month;		//月
	u8 day;			//日
	u8 hour;		//时
	u8 minute;		//分
	u8 second;		//秒
	u8 week;		//周
} RTC_Typedef;

extern RTC_Typedef RTC_Type;
extern RTC_Typedef RTC_Type_Temp;

extern ErrorStatus RTC_Config(void);
extern void RTC_Read_Time(void);
extern u8 RTC_Write_Time(void);
extern u8 RTC_Get_MaxDay(u16 NowYear, u8 NowMonth);

extern void TimeStringToRtcStruct(u8* Str, RTC_Typedef* RTC_Type);
extern void RtcStructToTimeString(RTC_Typedef* RTC_Type, u8* Str);
extern int GetTimeDifferSec(u8* Str1,u8* Str2);
extern u8 UpdateRtcTime(u8* Str);










#endif

