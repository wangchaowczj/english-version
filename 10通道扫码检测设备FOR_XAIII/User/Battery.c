#include "includes.h"

/*
锂电池电压与剩余容量的关系
电压:4.16-4.22V涓流补充:100%
电压:4.15v  剩余容量:99%
电压:4.14v  剩余容量:97%
电压:4.12v  剩余容量:95%
电压:4.10v  剩余容量:92%
电压:4.08v  剩余容量:90%
电压:4.05v  剩余容量:87%
电压:4.03v  剩余容量:85%
电压:3.97v  剩余容量:80%
电压:3.93v  剩余容量:75%
电压:3.90v  剩余容量:70%
电压:3.87v  剩余容量:65%
电压:3.84v  剩余容量:60%
电压:3.81v  剩余容量:55%
电压:3.79v  剩余容量:50%
电压:3.77v  剩余容量:45%
电压:3.76v  剩余容量:42%
电压:3.76V （持久电压点）电压:3.76v  剩余容量:40%
电压:3.74v  剩余容量:35%
电压:3.73v  剩余容量:30%
电压:3.72v  剩余容量:25%
电压:3.71v  剩余容量:20%
电压:3.71V （持久电压点）电压:3.69v  剩余容量:15%
电压:3.66v  剩余容量:12%
电压:3.65v  剩余容量:10%
电压:3.64v  剩余容量:8%
电压:3.63v  剩余容量:5%
电压:3.61v  剩余容量:3%
电压:3.59v  剩余容量:1%
电压:3.58v  剩余容量:关机一般手机 MP4等设置在此关机。电池输出电流不足，减小很多。
电压:3.55v  剩余容量:-2%
电压:3.50v  剩余容量:-5%
有电压但电流减小电压:3.42v  剩余容量:-8%
电压:3.3v  剩余容量:-10%影响容量了
电压:3.0v  剩余容量:-12%
电压:2.7v   剩余容量:-13%电池快要报废了，容量大打折扣！！！
- 本文出自“手电大家谈”，原帖：http://www.shoudian.org/thread-155163-1-1.html
*/
const u16 BatTable[] =
{
	4160, 100,
	4150, 99,
	4140, 97,
	4120, 95,
	4100, 92,
	4080, 90,
	4050, 87,
	4030, 85,
	3970, 80,
	3930, 75,
	3900, 70,
	3870, 65,
	3840, 60,
	3810, 55,
	3790, 50,
	3770, 45,
	3760, 42,
	3740, 35,
	3730, 30,
	3720, 25,
	3710, 20,
	3690, 15,
	3660, 12,
	3650, 10,
	3640, 8,
	3630, 5,
	3610, 3,
	3590, 1,
	0,    0
};
BATTERY  Battery;

/**
  *************************************************************************************
  * @brief  电池电压转换成容量
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年12月26日
  * @note   无
  *************************************************************************************  
  */
u16 GetBatteryCapacityByVoltage(u16 Voltage)
{
	u8 i;
	
	//根据电压转换成电量写入芯片
	for(i=0;i<sizeof(BatTable)/4;i++)
	{
		if(Voltage >= (BatTable[i<<1]<<1))
		{
			return BatTable[(i<<1)+1];			
		}
	}
	return 0;
}

/**
  *************************************************************************************
  * @brief  初始化Battery显示值
  * @param  无
  * @retval 返回显示电压
  * @author ZH
  * @date   2016年12月26日
  * @note   
  *************************************************************************************  
  */
u16 InitBatteryDisplayValue(u16 Voltage)
{
	u8 i;
	
	for(i = 0;i<BATTERY_VOLTAGE_BUFFER_SIZE;i++)
	{
		Battery.Voltage[i] = Voltage;		
	}
	Battery.Index = 0;
	Battery.DisValue = Voltage;
	Battery.Timer = OSTimeGet();
	return Battery.DisValue;
}
/**
  *************************************************************************************
  * @brief  获取Battery显示值
  * @param  Voltage 电池电压测量值
  * @retval 显示值
  * @author ZH
  * @date   2016年12月26日
  * @note   
  *************************************************************************************  
  */
u16 GetBatteryDisplayValue(u16 Voltage)
{
	u16 Average, AbsValue;
	
	if(Battery.Index >= BATTERY_VOLTAGE_BUFFER_SIZE)
	{
		Battery.Index = 0;
	}
	Battery.Voltage[Battery.Index++] = Voltage;

	Average = GetU16DataAverage(&Battery.Voltage[0], BATTERY_VOLTAGE_BUFFER_SIZE);
	AbsValue = abs(Average - Battery.DisValue);
	if ((AbsValue >= 50) || (OSTimeGet() >= (Battery.Timer+60000UL/(1000/OS_TICKS_PER_SEC))))
	{//平均值和当前显示的值相差较大或者超过1分钟时，更新显示值
		Battery.DisValue = Average;
		Battery.Timer = OSTimeGet();
	}
	return Battery.DisValue;
}

