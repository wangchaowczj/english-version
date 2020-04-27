#include "includes.h"
#include <time.h>


RTC_Typedef RTC_Type;
RTC_Typedef RTC_Type_Temp;

//==================================================================================================
//| 函数名称 | RTC_Config
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 系统实时时钟配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | ERROR   初始化失败
//|          | SUCCESS 初始化成功
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
ErrorStatus RTC_Config(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	u32 dly = 0;
	__IO uint32_t uwAsynchPrediv = 0;
	__IO uint32_t uwSynchPrediv = 0;

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)		//查询RTC的备用寄存器的值是否被改写过
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);			//允许PWR时钟
		PWR_BackupAccessCmd(ENABLE);								//允许PWR时钟连接到RTC

	#if defined (RTC_CLOCK_SOURCE_LSI)		//如果使用内部晶振LSI
		RCC_LSICmd(ENABLE);											//打开LSI
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);			//等待LSI准备完毕
		{
			dly++;
			if(dly > 0xF00000)
			{
				return ERROR;
			}			
		}		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);						//选择RTC时钟源

		//ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
		uwSynchPrediv = 0xFF;										//晶振为32768
		uwAsynchPrediv = 0x7F;

	#elif defined (RTC_CLOCK_SOURCE_LSE)	//如果使用外部晶振LSE
		RCC_LSEConfig(RCC_LSE_ON);									//打开LSE
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//等待LSE准备完毕
		{
			dly++;
			if(dly > 0xF00000)
			{
				return ERROR;
			}			
		}		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//选择LSE时钟源

		//ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
		uwSynchPrediv = 0xFF;										//晶振为32768
		uwAsynchPrediv = 0x7F;

	#else
		#error Please select the RTC Clock source inside the main.c file
	#endif

		RCC_RTCCLKCmd(ENABLE);			//允许RTC时钟
		RTC_WaitForSynchro();			//等待RTC APB寄存器同步

		RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
		RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);	//RTC初始化

		//初始化设置日期为2000年1月1日
		RTC_DateStructure.RTC_Year = 0x00;
		RTC_DateStructure.RTC_Month = RTC_Month_January;
		RTC_DateStructure.RTC_Date = 0x01;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);		//设置日期，按照BCD格式设置

		//初始化设置时间为0时0分0秒
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
		RTC_TimeStructure.RTC_Hours = 0x00;
		RTC_TimeStructure.RTC_Minutes = 0x0;
		RTC_TimeStructure.RTC_Seconds = 0x00;
		RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);		//设置时间，按照BCD格式设置

		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);			//改写RTC备用寄存器的值
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//允许PWR时钟
		PWR_BackupAccessCmd(ENABLE);							//允许PWR时钟连接到RTC
		RTC_WaitForSynchro();									//等待RTC APB寄存器同步
	}
	return SUCCESS;	
}

//==================================================================================================
//| 函数名称 | RTC_Read_Time
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 读取系统时钟
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void RTC_Read_Time(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	OSSchedLock();
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);	//获取时间
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);	//获取日期	
	
	RTC_Type.year = 2000+RTC_DateStructure.RTC_Year;
	RTC_Type.month = RTC_DateStructure.RTC_Month;
	RTC_Type.day = RTC_DateStructure.RTC_Date;
	RTC_Type.hour = RTC_TimeStructure.RTC_Hours;
	RTC_Type.minute = RTC_TimeStructure.RTC_Minutes;
	RTC_Type.second = RTC_TimeStructure.RTC_Seconds;
	OSSchedUnlock();
}

//==================================================================================================
//| 函数名称 | RTC_Write_Time
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 写入系统时钟
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：修改成功		1：数据非法		2：年数小于2000		3：设置出错
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 RTC_Write_Time(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	ErrorStatus state;

	if (RTC_Type_Temp.year < 2000)		//年数小于2000不符合要求
	{
		return 2;
	}
	else
	{
		RTC_Type_Temp.year -= 2000;		//转换成需要写进去的数据
	}

	//年月日时分秒数据格式判断
	//这里对于日的判断不包含相应月份对应的日的大小。即可以设置为2月31日
	if ((IS_RTC_YEAR(RTC_Type_Temp.year)) && (IS_RTC_MONTH(RTC_Type_Temp.month)) && (IS_RTC_DATE(RTC_Type_Temp.day)) && 
		(IS_RTC_HOUR24(RTC_Type_Temp.hour)) && (IS_RTC_MINUTES(RTC_Type_Temp.minute)) && (IS_RTC_SECONDS(RTC_Type_Temp.second)))
	{
		RTC_DateStructure.RTC_Year = RTC_Type_Temp.year;
		RTC_DateStructure.RTC_Month = RTC_Type_Temp.month;
		RTC_DateStructure.RTC_Date = RTC_Type_Temp.day;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
		RTC_TimeStructure.RTC_Hours = RTC_Type_Temp.hour;
		RTC_TimeStructure.RTC_Minutes = RTC_Type_Temp.minute;
		RTC_TimeStructure.RTC_Seconds = RTC_Type_Temp.second;

		state = RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);	//设置日期
		if (state == ERROR)
		{
			return 3;
		}
		state = RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);	//设置时间
		if (state == ERROR)
		{
			return 3;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

//==================================================================================================
//| 函数名称 | RTC_Get_MaxDay
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取该年该月的天数最大值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | NowYear：当前年份		NowMonth：当前月份
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：修改成功		1：数据非法		2：年数小于2000		3：设置出错
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 RTC_Get_MaxDay(u16 NowYear, u8 NowMonth)
{
	if ((NowMonth == 1) || (NowMonth == 3) || (NowMonth == 5) || 
		(NowMonth == 7) || (NowMonth == 8) || (NowMonth == 10) || (NowMonth == 12))
	{
		return 31;
	}
	else if (NowMonth == 2)
	{
		if( ( (NowYear%4==0)&&(NowYear%100!=0) ) || (NowYear%400==0) )		//闰年
		{
			return 29;
		}
		else
		{
			return 28;
		}
	}
	else
	{
		return 30;
	}
}
u16 StringToDecValue(u8* Buffer, u8 MaxLenght)
{
	u8 i;
	u16 Value = 0;
	
	for(i=0;i<MaxLenght;i++)
	{
		if ((Buffer[i] < '0') || (Buffer[i] > '9'))
		{
			break;
		}
		Value *= 10;
		Value += (Buffer[i]-'0');
	}
	return Value;
}
/**
  *************************************************************************************
  * @brief  获取输入时间字符串与当前系统时间相差的秒数
  * @param  时间字符串，格式为"20160618113320"
  * @retval 相差的秒数
  * @author ZH
  * @date   2016年06月18日
  * @note   
  *************************************************************************************  
  */
void TimeStringToRtcStruct(u8* Str, RTC_Typedef* RTC_Type)
{	
	RTC_Type->week = 1;
	RTC_Type->year = StringToDecValue(Str,4);
	RTC_Type->month = StringToDecValue(Str+4,2);
	RTC_Type->day = StringToDecValue(Str+6,2);
	RTC_Type->hour = StringToDecValue(Str+8,2);
	RTC_Type->minute = StringToDecValue(Str+10,2);
	RTC_Type->second = StringToDecValue(Str+12,2);
}
/**
  *************************************************************************************
  * @brief  获取输入时间字符串与当前系统时间相差的秒数
  * @param  时间字符串，格式为"20160618113320"
  * @retval 相差的秒数
  * @author ZH
  * @date   2016年06月18日
  * @note   
  *************************************************************************************  
  */
void RtcStructToTimeString(RTC_Typedef* RTC_Type, u8* Str)
{	
	sprintf((char*)Str, "%04d%02d%02d%02d%02d%02d",
		RTC_Type->year,RTC_Type->month,RTC_Type->day,RTC_Type->hour,RTC_Type->minute,RTC_Type->second);
}
/**
  *************************************************************************************
  * @brief  比较两个时间字符串相差的秒数
  * @param  时间字符串，格式为"20160618113320"
  * @retval 相差的秒数
  * @author ZH
  * @date   2016年06月18日
  * @note   
  *************************************************************************************  
  */
int GetTimeDifferSec(u8* Str1,u8* Str2)
{
	struct tm time_cha;
	RTC_Typedef RTC_Buffer;
	time_t t1,t2;
	
	TimeStringToRtcStruct(Str1, &RTC_Buffer);
	time_cha.tm_year = RTC_Buffer.year-1900; //tm结构记录年为实际-1900
	time_cha.tm_mon = RTC_Buffer.month;
	time_cha.tm_mday = RTC_Buffer.day;
	time_cha.tm_hour = RTC_Buffer.hour;
	time_cha.tm_min = RTC_Buffer.minute;
	time_cha.tm_sec = RTC_Buffer.second;
	
	t1 = mktime(&time_cha);//获得从1970年1月1日0时0分0秒以来过去的时间，秒

	TimeStringToRtcStruct(Str2, &RTC_Buffer);
	time_cha.tm_year = RTC_Buffer.year-1900; //tm结构记录年为实际-1900
	time_cha.tm_mon = RTC_Buffer.month;
	time_cha.tm_mday = RTC_Buffer.day;
	time_cha.tm_hour = RTC_Buffer.hour;
	time_cha.tm_min = RTC_Buffer.minute;
	time_cha.tm_sec = RTC_Buffer.second;
	
	t2 = mktime(&time_cha);//获得从1970年1月1日0时0分0秒以来过去的时间，秒
	
	return abs(t1-t2);
}

u8 UpdateRtcTime(u8* Str)
{
	TimeStringToRtcStruct(Str, &RTC_Type_Temp);
	return RTC_Write_Time();
}



