#include "includes.h"

/**
  *************************************************************************************
  * @brief  判断芯片内码是否需要修复
  * @param  Serl_2 序列码2 
  * @param  Serl_3 序列码3   
  * @retval 结果
  *      @arg 0 不需要修复
  *      @arg 其他 需要修复
  * @author ZH
  * @date   2018年3月30日
  * @note   只过滤一小部分
  *************************************************************************************  
  */
u8 DetCodeNeedPepairByNotComplete(u8 Serl_2, u8 Serl_3)
{
	if(Serl_2 == 0x5A)
	{
		return 1;
	}
	if(Serl_2 == 0xB4)
	{
		return 2;
	}	
	if((Serl_2 == 0x14) && (Serl_3 == 0x00))
	{
		return 3;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  判断芯片内码是否需要修复
  * @param  DetInfo 芯片内码数据  
  * @retval 结果
  *      @arg 0 不需要修复
  *      @arg 1 需要修复
  * @author ZH
  * @date   2018年3月24日
  * @note   
  *************************************************************************************  
  */
u8 DetCodeNeedPepair(u8 Serl_2)
{
	u8 Value;

	/*
	SERL2[0] ==0 此位设置固定为0；
	Serl2[7:1]    != 7’b1000010  查漏
				  != 7’b0001010  复位
				  != 7’b0010010  序列码回读
				  != 7’b1111010  准备起爆
				  != 7’b1011010  BIT
				  != 7’b1101010  充电
				  != 7’b0110010  屏蔽
				  != 7’b1011010  EE解锁
				  != 7’b0101010  checkosc
				  != 7’b1010010  时间回读
				  != 7’b1111010  状态回读
	*/
	
	if(Serl_2&0x01)//序列码2的bit0位要求固定为0
	{
		return 1;
	}
	Value = Serl_2>>1;
	switch(Value)
	{
		case 0x42://7’b1000010  查漏
		case 0x0A://7’b0001010  复位
		case 0x12://7’b0010010  序列码回读
		case 0x7A://7’b1111010  准备起爆
		case 0x5A://7’b1011010  BIT
		case 0x6A://7’b1101010  充电
		case 0x32://7’b0110010  屏蔽
//		case 0x5A://7’b1011010  EE解锁
		case 0x2A://7’b0101010  checkosc
		case 0x52://7’b1010010  时间回读
//		case 0x7A://7’b1111010  状态回读
			return 1;
//			break;
		default:
			break;
	}		
	return 0;
}


/**
  *************************************************************************************
  * @brief  判断芯片密码是否需要修复
  * @param  DetInfo 芯片内码数据  
  * @retval 结果
  *      @arg 0 不需要修复
  *      @arg 1 需要修复
  * @author ZH
  * @date   2018年3月24日
  * @note   
  *************************************************************************************  
  */
u8 DetPwdNeedPepair(DET_INFO *DetInfo)
{
	u8 Value;
	u16 DetPwd;

	/*
	Code1[0] =0; 此位固定设置为0
	Code1[7:1]  != 7’b1011010  BIT
	!= 7’b1101010  充电
	Code2[7:1] != 7’b0000000， 密码2
	*/	
	DetPwd = GetDetPwd(DetInfo->Serl_1, DetInfo->Serl_2, DetInfo->Serl_3,
		DetInfo->Factor, DetInfo->bat, DetInfo->year);	
	
	Value = DetPwd&0xFF;	
	if(Value&0x01)//密码1的bit0位要求固定为0
	{
		return 1;
	}	
	Value = Value>>1;//Code1[7:1]
	switch(Value)
	{
		case 0x5A://7’b1011010  BIT
		case 0x6A://7’b1101010  充电
			return 1;
		default:
			break;
	}	
	Value = DetPwd>>9;//Code2[7:1]
	switch(Value)
	{
		case 0x00://7’b0000000， 密码2
			return 1;
		default:
			break;
	}		
	return 0;	
}


/**
  *************************************************************************************
  * @brief  修复芯片延时时钟值
  * @param  DelayClk 待修复前的值  
  * @retval 修复反的值
  * @author ZH
  * @date   2018年3月24日
  * @note   要求T_L [0] =0，如果延期时间中间8位T_M[15:8] ==8’h00时则要求T_L[1] == 1’b1
  *************************************************************************************  
  */
u32 DetDelayClkPepair(u32 DelayClk)
{
	if(DelayClk&0x01)//要求最低位必需为0
	{
		DelayClk &= 0xFFFFFFFE;
	}
	if(((DelayClk>>8)&0xFF) == 0)
	{
		DelayClk |= 0x02;
	}

	return DelayClk;
}

/**
  *************************************************************************************
  * @brief  起爆器是否使用过滤规则
  * @param  无 
  * @retval 结果
  *      @arg 0 不需要修复
  *      @arg 1 需要修复
  * @date   2018年3月30日
  * @note   
  *************************************************************************************  
  */
u8 BlasterNeedPepair(void)
{
	//江西的起爆器不使用内码过虑规则
	if(memcmp(device_info.mac, "F34", 3) == 0)
	{
		return 0;
	}
	else if(memcmp(device_info.mac, "f34", 3) == 0)
	{
		return 0;
	}
	return 1;
}




