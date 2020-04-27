#include "includes.h"
/*
﮵�ص�ѹ��ʣ�������Ĺ�ϵ
��ѹ:4.16-4.22V�������:100%
��ѹ:4.15v  ʣ������:99%
��ѹ:4.14v  ʣ������:97%
��ѹ:4.12v  ʣ������:95%
��ѹ:4.10v  ʣ������:92%
��ѹ:4.08v  ʣ������:90%
��ѹ:4.05v  ʣ������:87%
��ѹ:4.03v  ʣ������:85%
��ѹ:3.97v  ʣ������:80%
��ѹ:3.93v  ʣ������:75%
��ѹ:3.90v  ʣ������:70%
��ѹ:3.87v  ʣ������:65%
��ѹ:3.84v  ʣ������:60%
��ѹ:3.81v  ʣ������:55%
��ѹ:3.79v  ʣ������:50%
��ѹ:3.77v  ʣ������:45%
��ѹ:3.76v  ʣ������:42%
��ѹ:3.76V ���־õ�ѹ�㣩��ѹ:3.76v  ʣ������:40%
��ѹ:3.74v  ʣ������:35%
��ѹ:3.73v  ʣ������:30%
��ѹ:3.72v  ʣ������:25%
��ѹ:3.71v  ʣ������:20%
��ѹ:3.71V ���־õ�ѹ�㣩��ѹ:3.69v  ʣ������:15%
��ѹ:3.66v  ʣ������:12%
��ѹ:3.65v  ʣ������:10%
��ѹ:3.64v  ʣ������:8%
��ѹ:3.63v  ʣ������:5%
��ѹ:3.61v  ʣ������:3%
��ѹ:3.59v  ʣ������:1%
��ѹ:3.58v  ʣ������:�ػ�һ���ֻ� MP4�������ڴ˹ػ����������������㣬��С�ܶࡣ
��ѹ:3.55v  ʣ������:-2%
��ѹ:3.50v  ʣ������:-5%
�е�ѹ��������С��ѹ:3.42v  ʣ������:-8%
��ѹ:3.3v  ʣ������:-10%Ӱ��������
��ѹ:3.0v  ʣ������:-12%
��ѹ:2.7v   ʣ������:-13%��ؿ�Ҫ�����ˣ���������ۿۣ�����
- ���ĳ��ԡ��ֵ���̸����ԭ����http://www.shoudian.org/thread-155163-1-1.html
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
  * @brief  ��ع���оƬLTC2942��ʼ��
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��10��28��
  * @note   
  *************************************************************************************  
  */
ErrorStatus LTC2942Init(void)
{
	u8 Result=0, U8Value=0, i;
	u16 Voltage, U16Value;
	
	for(i=0;i<3;i++)
	{
		Result = I2C_ReadByte(0x01, LTC2942_ADDR, &U8Value);	//���ƼĴ���
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
			{//���ڳ��
				if(Voltage < LI_BAT_CHARGE_VOLTAGE_MAX)
				{
					if(Voltage >= 70)
					{
						Voltage -= 70;//���ʱ�⵽�ĵ�ص�ѹ���ʵ���Ըߣ�����ֵ���������С����ǰ��ѹֵ���й�ϵ��0.1Vֻ��һ�����ֵ��
					}
					else
					{
						Voltage = 0;
					}				
				}
			}
			//���ݵ�ѹת���ɵ���д��оƬ
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
	{//���ڳ��
		if(Voltage < LI_BAT_CHARGE_VOLTAGE_MAX)
		{
			if(Voltage >= 70)
			{
				Voltage -= 70;//���ʱ�⵽�ĵ�ص�ѹ���ʵ���Ըߣ�����ֵ���������С����ǰ��ѹֵ���й�ϵ��0.1Vֻ��һ�����ֵ��
			}
			else
			{
				Voltage = 0;
			}				
		}
	}
	//���ݵ�ѹת���ɵ���д��оƬ
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
  * @brief  ����ع���оƬLTC2942�е�ص�����Ӧֵ
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��10��28��
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
  * @brief  ���µ�ʱ��ع���оƬLTC2942�е�ص���
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��10��28��
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
  * @brief  ���³�����ʱ��ع���оƬLTC2942�е�ص���
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��10��28��
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
  * @brief  �ӵ�ع���оƬLTC2942�л�ȡ��ص�ѹֵ
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��10��28��
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
  * @brief  �ӵ�ع���оƬLTC2942�л�ȡ��ص���
  * @param  ��
  * @retval ���,0-100,��Ӧ������0%-100%
  * @author ZH
  * @date   2015��10��28��
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
  * @brief  ��ص�ѹת���ɵ���
  * @param  Voltage��ѹ
  * @retval ���,0-100,��Ӧ������0%-100%
  * @author ZH
  * @date   2018��05��15��
  * @note   
  *************************************************************************************  
  */
u8 BatVoltageToAccumulated(u16 Voltage)
{
	u16 i;

	//���ݵ�ѹת���ɵ���д��оƬ
	for(i=0;i<sizeof(BatTable)/4;i++)
	{
		if(Voltage >= BatTable[i<<1])
		{			
			return BatTable[(i<<1)+1];
		}
	}
	return 0;
}
