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

/**
  *************************************************************************************
  * @brief  电池管理芯片LTC2942初始化
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
ErrorStatus LTC2942Init(void)
{
	u8 Result=0, U8Value=0, i;
	u16 Voltage, U16Value;
	
	for(i=0;i<3;i++)
	{
		Result = I2C_ReadByte(0x01, LTC2942_ADDR, &U8Value);	//控制寄存器
		if((Result == 0) || (0xF8 != U8Value))
		{
			U8Value = 0xF8;
			I2C_WriteData(LTC2942_ADDR,0x01, &U8Value, 1);
			Result = I2C_ReadByte(0x01, LTC2942_ADDR, &U8Value);	
			if((Result == 0) || (0xF8 != U8Value))
			{				
				continue;
			}
			System168MDelay1ms(50);
			Voltage = LTC2942BatVoltage();
			if(Voltage == 0)
			{
				Voltage = LTC2942BatVoltage();
				if(Voltage == 0)
				{
					return ERROR;
				}
			}
			if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
			{//正在充电
				if(Voltage < LI_BAT_CHARGE_VOLTAGE_MAX)
				{
					if(Voltage >= 70)
					{
						Voltage -= 70;//充电时测到的电池电压会比实际略高，具体值与充电电流大小、当前电压值都有关系，0.1V只是一个大概值。
					}
					else
					{
						Voltage = 0;
					}				
				}
			}
			//根据电压转换成电量写入芯片
			for(i=0;i<sizeof(BatTable)/4;i++)
			{
				if(Voltage >= BatTable[i<<1])
				{
					U8Value = BatTable[(i<<1)+1];
					U16Value = 655.35*U8Value;
					
					return LTC2942UpdateBatAccumulated(U16Value);
				}
			}
		}
		return SUCCESS;
	}

	return ERROR;	
}

ErrorStatus LTC2942BatAccumulatedAdjust(void)
{
	u8 U8Value=0, i;
	u16 Voltage, U16Value;
	
	Voltage = LTC2942BatVoltage();
	if(Voltage == 0)
	{
		Voltage = LTC2942BatVoltage();
		if(Voltage == 0)
		{
			return ERROR;
		}
	}
	if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
	{//正在充电
		if(Voltage < LI_BAT_CHARGE_VOLTAGE_MAX)
		{
			if(Voltage >= 70)
			{
				Voltage -= 70;//充电时测到的电池电压会比实际略高，具体值与充电电流大小、当前电压值都有关系，0.1V只是一个大概值。
			}
			else
			{
				Voltage = 0;
			}				
		}
	}
	//根据电压转换成电量写入芯片
	for(i=0;i<sizeof(BatTable)/4;i++)
	{
		if(Voltage >= BatTable[i<<1])
		{
			U8Value = BatTable[(i<<1)+1];
			U16Value = 655.35*U8Value;
			
			return LTC2942UpdateBatAccumulated(U16Value);
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  读电池管理芯片LTC2942中电池电量对应值
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
u16 LTC2942BatAccumulatedValue(void)
{
	u16 Accumulated = 0;
	u8 Buffer[2];
	u8 Retry = 3;
	
	while(Retry--)
	{
		if(0 != I2C_ReadData(LTC2942_ADDR,0x02,Buffer,0x02))
		{
			Accumulated = ((u16)Buffer[0]<<8) + Buffer[1];	
			break;
		}	
	}

	return Accumulated;
}

/**
  *************************************************************************************
  * @brief  更新电时电池管理芯片LTC2942中电池电量
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
ErrorStatus LTC2942UpdateBatAccumulated(u16 Value)
{
	u8 Buffer[2];
	
	Buffer[0] = Value>>8;
	Buffer[1] = Value&0xFF;
	
	if(0 == I2C_WriteData(LTC2942_ADDR,0x02, &Buffer[0], 2))
	{
		return ERROR;
	}						

	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  更新充满电时电池管理芯片LTC2942中电池电量
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
ErrorStatus LTC2942ChargeCompleteUpdate(void)
{
	if(ERROR == LTC2942UpdateBatAccumulated(LI_BAT_CHARGE_COMPLETE_VALUE))
	{
		return LTC2942UpdateBatAccumulated(LI_BAT_CHARGE_COMPLETE_VALUE);
	}
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  从电池管理芯片LTC2942中获取电池电压值
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
u16 LTC2942BatVoltage(void)
{
	u16 Voltage = 0, ADValue;
	u8 Buffer[2];
	u8 Retry = 3;
	
	while(Retry--)
	{
		if(0 != I2C_ReadData(LTC2942_ADDR,0x08,Buffer,0x02))
		{
			ADValue = ((u16)Buffer[0]<<8) + Buffer[1];
			//Voltage = (double)ADValue/65535.0*6000;
			Voltage = (double)ADValue*0.09155413138;
			
			break;
		}	
	}


	return Voltage;
}

/**
  *************************************************************************************
  * @brief  从电池管理芯片LTC2942中获取电池电量
  * @param  无
  * @retval 结果,0-100,对应电量的0%-100%
  * @author ZH
  * @date   2015年10月28日
  * @note   
  *************************************************************************************  
  */
u16 LTC2942BatAccumulated(void)
{
	double DValue;
	u16 Accumulated = 0;
	u8 Buffer[2];
	u8 Retry = 3;
	
	while(Retry--)
	{
		if(0 != I2C_ReadData(LTC2942_ADDR,0x02,Buffer,0x02))
		{
			DValue = ((u16)Buffer[0]<<8) + Buffer[1];
			DValue *= 0.0015259;//1/65535
			Accumulated = DValue + 0.05;
			
			break;
		}	
	}

	if(Accumulated > 100)
	{
		Accumulated = 100;
	}
	return Accumulated;
}
/**
  *************************************************************************************
  * @brief  电池电压转换成电量
  * @param  Voltage电压
  * @retval 结果,0-100,对应电量的0%-100%
  * @author ZH
  * @date   2018年05月15日
  * @note   
  *************************************************************************************  
  */
u8 BatVoltageToAccumulated(u16 Voltage)
{
	u16 i;

	//根据电压转换成电量写入芯片
	for(i=0;i<sizeof(BatTable)/4;i++)
	{
		if(Voltage >= BatTable[i<<1])
		{			
			return BatTable[(i<<1)+1];
		}
	}
	return 0;
}
