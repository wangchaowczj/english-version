#ifndef _RTC_H_
#define _RTC_H_

#define RTC_CLOCK_SOURCE_LSE		//ʹ���ⲿ����
//#define RTC_CLOCK_SOURCE_LSI		//ʹ���ڲ�����

typedef struct RTC_DATA
{
	u16 year;		//��
	u8 month;		//��
	u8 day;			//��
	u8 hour;		//ʱ
	u8 minute;		//��
	u8 second;		//��
	u8 week;		//��
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

