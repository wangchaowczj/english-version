#include "includes.h"
#include "GuiCustomUtils.h"
extern void SetProgbarParam(void);


DET Det;
DET_SERL_GROUP_INFO DetSerlGroupInfo;

/**
  *************************************************************************************
  * @brief  判断内码和密码是否符合规则
  * @param  Serl 内码
  * @param  Pwd 密码
  * @retval 1 符合规则， 0不符合
  * @author ZH
  * @date   2019年4月8日
  * @note   无
  *************************************************************************************  
  */
u8 DetCodeRulePass(u32 Serl, u32 Pwd)
{
    u8 Buffer[8];
    u8 Crc8;
    
    Buffer[0] = Serl&0xFF;
    Buffer[1] = (Serl>>8)&0xFF;
    Buffer[2] = (Serl>>16)&0xFF;
    Buffer[3] = (Serl>>24)&0xFF;
    Buffer[4] = Pwd&0xFF;
    Buffer[5] = (Pwd>>8)&0xFF;
    Buffer[6] = (Pwd>>16)&0xFF;
    Buffer[7] = (Pwd>>24)&0xFF;
    
    Crc8 = (~GetCrc8Value(Buffer, 7))&0xFF;
    
    return (Crc8==Buffer[7])?1:0;  
}
/**
  *************************************************************************************
  * @brief  内码转管码
  * @param  Serl 序列码
  * @param  密码 Pwd
  * @param  Ptr 管码
  * @retval 结果
  * @author ZH
  * @date   2019年4月20日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA3_DetInside2Outside(u32 Serl, u32 Pwd, u8* Ptr)
{
    uint64_t U64Value;
    u16 U16Value;
    char Buffer[3];
    
    if(!DetCodeRulePass(Serl, Pwd))
    {
        return ERROR;
    }
    
    U64Value = Pwd;
    U64Value <<= 32;
    U64Value |= Serl;
    
    U16Value = (U64Value>>39)&0x7F;//工厂代码bit39-bit45共7位
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)Ptr, "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>35)&0x0F;//年bit35-bit38共4位
    if(U16Value > 9)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+2), "%d", U16Value);    
    }

    U16Value = (U64Value>>22)&0x0F;//月bit22-bit25共4位
    if((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+3), "%02d", U16Value);    
    }

    U16Value = (U64Value>>17)&0x1F;//日bit17-bit21共5位
    if((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+5), "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>27)&0xFF;//特征号bit27-bit34共8位,数字或英文ASCII码
    if((U16Value < 0x30) || (U16Value > 0x80))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        *(Ptr+7) = U16Value;    
    }
     
    U16Value = (U64Value>>7)&0x3FF;//盒号bit7-bit16共10位
    if(U16Value > 999)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+8), "%03d", U16Value);    
    }
    
    U16Value = U64Value&0x7F;//盒内流水号bit0-bi6共7位
    if(U16Value > 99)
    {
        return ERROR;
    }
    sprintf(Buffer, "%02d", U16Value);
    if(Ptr != NULL)
    {
        memcpy((char*)(Ptr+11), Buffer, 2);
    } 
    
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  内码转管码
  * @param  Serl 序列码
  * @param  密码 Pwd
  * @param  Ptr 管码
  * @retval 结果
  * @author ZH
  * @date   2019年4月20日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA3_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd)
{
    u8 i;
    uint64_t U64Value = 0;
    u32 Value;
    u8 Buffer[8];

    for(i=0; i< 7; i++)
    {
        if((Ptr[i] < '0') || (Ptr[i] >'9'))
        {
            return ERROR;
        }
    }
    if(!(((Ptr[7] >= '0') && (Ptr[7] <='9')) || ((Ptr[7] >= 'A') && (Ptr[7] <='Z')) || ((Ptr[7] >= 'a') && (Ptr[7] <='z'))))
    {
        return ERROR;
    }
    for(i=8; i< 13; i++)
    {
        if((Ptr[i] < '0') || (Ptr[i] >'9'))
        {
            return ERROR;
        }
    }
    //盒内流水号bit0-bi6共7位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+11, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 99)
    {
        return ERROR;
    }
    
    U64Value = Value;

    //盒内bit7-bit16共10位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+8, 3);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 999)
    {
        return ERROR;
    }
    U64Value |= Value<<7;

    //日 bit17-bit21共5位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+5, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 31))
    {
        return ERROR;
    }
    U64Value |= Value<<17;

    //月 bit22-bit25共4位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 

    //月 bit22-bit25共4位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 
    
    // 年的低位 bit26共1位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+2, 1);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 9)
    {
        return ERROR;
    }
    U64Value |= (Value&0x01)<<26; 
    
    //年 bit35-bit38共4位
    U64Value |= ((uint64_t)Value)<<35; 
    
    //特征号bit27-bit34共8位 数字或英文ASCII码
    Value = *(Ptr+7);
    if((Value < 0x30) || (Value > 0x80))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<27; 
    
    //工厂代码 bit39-bit45共7位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0x00) || (Value > 74))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<39; 

    //雷管类型  bit46-bit50共5位
    Value = DetType&0x1F;
    U64Value |= ((uint64_t)Value)<<46; 
    
    memset(Buffer, 0, sizeof(Buffer));
    Buffer[0] = U64Value&0xFF;
    Buffer[1] = (U64Value>>8)&0xFF;
    Buffer[2] = (U64Value>>16)&0xFF;
    Buffer[3] = (U64Value>>24)&0xFF;
    Buffer[4] = (U64Value>>32)&0xFF;
    Buffer[5] = (U64Value>>40)&0xFF;
    Buffer[6] = (U64Value>>48)&0xFF;
    Buffer[7] = (~GetCrc8Value(Buffer, 7))&0xFF;
    
    if(Serl != NULL)
    {
        *Serl = U64Value;
    }
    if(Pwd != NULL)
    {
        Value = Buffer[7];
        Value <<= 24;
        Value |= (U64Value>>32)&0xFFFFFF;
        *Pwd = Value;
    }
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  解锁并写入EEPROM
  * @param  Serl 序列码
  * @param  Addr 地址
  * @param  Buffer 数据
  * @retval 结果，0成功， 1解锁失败，2写EEPROM失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
u8 XA3_UnlockAndWriteEEPROM(u32 Serl, u8 Addr, u8* Buffer)
{
    u8 Retry = 2;
    
    while(Retry--)
    {
        if(ERROR == XA3_WriteDataUnlock(Serl))
        {
            return 1;//解EEPROM错误
        }
        if(ERROR == XA3_WriteEEPROM(Addr, Buffer))
        {
            if(Addr == 2)
            {
                XA3_ReadSerl(&Serl);
            }
            continue;
        }
        return 0;    
    }
    return 2;//写EEPROM错误
}
/**
  *************************************************************************************
  * @brief  起爆延时增加一个额外的延时时间
  * @param  DelayMs 起爆延时时间,单位ms 
  * @retval 芯片延时时间
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
u16 BlastDelayConvertToDetDelay(u16 DelayMs)
{
    if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && Op_Data.DelaySampleCfg && Op_Data.DelaySampleFlag)
    {
        return DelayMs;
    }  
	return (DelayMs+BLAST_DELAY_WHOLE_ADD_VALUE);
}

/**
  *************************************************************************************
  * @brief  根据起爆延时和芯片频率计算芯片延时计数值
  * @param  DelayMs 起爆延时时间,单位ms
  * @param  Freq 芯片频率   
  * @retval 对应的芯片延时计数值
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
u32 BlastDelayConvertToDetDelayClock(u16 DelayMs, u32 Freq)
{
	return 1+ BlastDelayConvertToDetDelay(DelayMs)*Freq/1000;
}
//==================================================================================================
//| 函数名称 | SaveErrCode 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 保存错误雷管代码   
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 |  
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | err_index：错误雷管号   err_code：错误代码
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 |     
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 | det_info：孔信息  
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：郑  海    时间：2012-3-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |      
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void SaveErrCode(u16 err_index, u8 err_code)
{
	if(Det.Info[err_index].Err != err_code)
	{
		if(0x00 == Det.Info[err_index].Err) //雷管原来无错误 
		{
			Det.Info[err_index].Err = err_code ;
			Det.ErrorCount++;					
		}
		if(0x00 == err_code) //清空雷管错误代码
		{
			Det.Info[err_index].Err = err_code ;
			Det.ErrorCount--;
		}	
	}
}
void SaveRetryErrCode(u16 err_index, u8 err_code)
{
	if(Det.Info[err_index].RetryErr != err_code)
	{
		if(0x00 == Det.Info[err_index].RetryErr) //雷管原来无错误 
		{
			Det.Info[err_index].RetryErr = err_code ;
		}
		if(0x00 == err_code) //清空雷管错误代码
		{
			Det.Info[err_index].RetryErr = err_code ;
		}	
	}
}
u16 GetRetryErrCount(void)
{
	u16 i, Count = 0;
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(Det.Info[i].RetryErr != 0)
			{
				Count++;
			}
		}
	}
	return Count;
}
/**
  *************************************************************************************
  * @brief  查找组索引
  * @param  无
  * @retval 0xFF 没找到
  * @author ZH
  * @date   2016年7月2日
  * @note   无
  *************************************************************************************  
  */
U8 GetSerlGroupIndex(u8 Ser1Group)
{
	u8 i;
	
	for(i=0; i < DetSerlGroupInfo.Count; i++)
	{
		if((Ser1Group&0x1F) == (DetSerlGroupInfo.Group[i].Serl&0x1F))
		{
			return i;
		}
	}
	return 0xFF;
}
/**
  *************************************************************************************
  * @brief  清空组信息
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年7月2日
  * @note   无
  *************************************************************************************  
  */
void ClearSerlGroup(void)
{
	u8 i;
	
	for(i=0; i < DET_SERL_GROUP_INFO_MAX; i++)
	{
		DetSerlGroupInfo.Group[i].Count = 0x00;
		DetSerlGroupInfo.Group[i].Serl = 0x00;
	}
	DetSerlGroupInfo.Count = 0;
}
//==================================================================================================
//| 函数名称 | TestInfoInit 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 测试信息初始化 
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 |  
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无    
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 | det_info：孔信息  
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：郑  海    时间：2012-3-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |      
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void TestInfoInit(void)
{
	u16 i;
	u8 GroupIndex;
	
	ClearSerlGroup();
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		Det.Info[i].Err = 0x00 ;  //清空错误代码
		Det.Info[i].RetryErr = 0x00 ;
		if (GetDetRegStatus(i) != ERROR)
		{
			GroupIndex = GetSerlGroupIndex(Det.Info[i].Serl.U32Value);
			if(GroupIndex >= DET_SERL_GROUP_INFO_MAX)
			{//还没有
				DetSerlGroupInfo.Group[DetSerlGroupInfo.Count].Serl = Det.Info[i].Serl.U32Value&0x1F;
				DetSerlGroupInfo.Group[DetSerlGroupInfo.Count].Count++;
				DetSerlGroupInfo.Count++;
			}
			else
			{
				DetSerlGroupInfo.Group[GroupIndex].Count++;
			}
		}		
	}
	Det.ErrorCount = 0;	
}
u16 GetDetDelayMaxFromRegStatus(void)
{
	u16 i,MaxDly = 0;
	
	if(Det.RegCount == 0)
	{
		return 0;
	}
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(Det.Info[i].Delay > MaxDly)
			{
				MaxDly = Det.Info[i].Delay;
			}		
		}
	}
	return MaxDly;
}
//==================================================================================================
//| 函数名称 | GetDetNum
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 统计注册数和错误数   
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 |  
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |  
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 |     
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 |  
//|          |  
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：ZH    时间：2013-04-17  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |      
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void GetDetNum(void)
{
	u16 i, Reg = 0, Error = 0;
	
	/****统计注册雷管数和错误雷管数****/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Reg++;
			if(0x00 != Det.Info[i].Err)
			{
				Error++;
			}		
		}
	}
	
	/****设备带载数量****/
	Det.RegCount = Reg;
	Det.ErrorCount = Error;	
}
void AllReset(void)
{  
    XA3_ResetAll();
    OSTimeDlyHMSM(0, 0, 0, 50);    
    XA3_ResetAll();
    OSTimeDlyHMSM(0, 0, 0, 50);
}
//==================================================================================================
//| 函数名称 | AllDisCharge
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 发送2次复位指令，间隔50ms
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 | 	无
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	无
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 |  无
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：ZH    时间：2013-04-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void AllDisCharge(void)
{
	XA3_ClearAll();
	OSTimeDlyHMSM(0,0,0,50);//延时80ms，不能小于50ms
	XA3_ClearAll();
	OSTimeDlyHMSM(0,0,0,50);//延时80ms
}
ErrorStatus XA3_ReadPwdRetry(u32 Serl, u32* Pwd)
{
    u8 Retry = 2;
    
    if(ERROR != XA3_ReadPwd(Serl, Pwd))
    {
        return SUCCESS;
    }
    while(Retry--)
    {
        XA3_Reset(Serl);
        OSTimeDlyHMSM(0,0,0,80);//延时80ms
        WriteSystemLog("复位后重读PWD",strlen("复位后重读PWD"), NEW_STR_INFO);
        XA3_DetCheckIn();
        OSTimeDlyHMSM(0,0,0,80);//延时等芯片读EEPROM     
        if(ERROR != XA3_ReadPwd(Serl, Pwd))
        {
            return SUCCESS;
        }
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  判断数组中是否已经注册雷管
  * @param  Index 数组索引
  * @retval 结果
  *      @arg SUCCESS 已注册
  *      @arg ERROR 没有注册
  * @author ZH
  * @date   2015年8月7日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetDetRegStatus(u16 Index)
{
	if(Index >= DET_MAX_SIZE)
	{
		return ERROR;
	}
	if(0x00 == Det.Info[Index].Serl.U32Value)
	{
		return ERROR;
	}
	return SUCCESS;
}

//==================================================================================================
//| 函数名称 | CheckOverlap 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检查与系统记录中的序列号是否有重复  
//|----------|--------------------------------------------------------------------------------------
//| 调用模块 |  
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | det_buffer 要比较的雷管信息首地址
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 |SUCCESS：  没有重复的序列码     
//|          |ERROR:    有重复的序列码  
//|----------|--------------------------------------------------------------------------------------       
//| 全局变量 |det_hole_num_overlaped：雷管已注册时返回的孔号  
//|          |hole_info:雷管信息  
//|----------|-------------------------------------------------------------------------------------- 
//| 函数设计 | 编写人：郑  海    时间：2013-4-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |      
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus CheckOverlap(const u8* Serl)
{
	u16 i;

    for(i=0; i<DET_MAX_SIZE; i++)
    {
		if(GetDetRegStatus(i) != ERROR)
		{
			if(0  == memcmp(Serl, &Det.Info[i].Serl.U8Buffer[0], 4))//比较序列码是否有相同
			{
				 Op_Data.overlaped = i ;  //保存重号索引
				 return ERROR ;			
			}		
		}
    }
    return SUCCESS ;  // 没有重复的雷管
}

//==================================================================================================
//| 函数名称 | CheckDetFreq
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检测输入的频率是否正常
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	freq  输入的频率数据
//|          |  check_state  检测状态，0为注册，1为起爆流程
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	DET_PASS  频率正常,FREQ_MIN 频率过小，FREQ_MAX频率过大
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 注册状态下芯片频率要求更严，起爆流程芯片频率范围更宽
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
CHECK_RESULT CheckDetFreq(u32 freq, u8 check_state)
{
	if(0 == check_state)
	{//注册状态
		if(freq < device_info.det_freq_min)
		{
			return FREQ_MIN;	
		}
		if(freq > device_info.det_freq_max)
		{
			return FREQ_MAX;
		}
	}
	else
	{//起爆流程状态
		if(freq < (device_info.det_freq_min-20))//起爆流程状态对芯片频率范围更宽
		{
			return FREQ_MIN;	
		}
		if(freq > (device_info.det_freq_max+20))
		{
			return FREQ_MAX;
		}
	}
	return DET_PASS;
}

//==================================================================================================
//| 函数名称 | CheckDetType
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检查雷管类型是否符合要求
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | det_bat2 批号号2
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	SUCCESS  雷管类型符合要求
//|          | 	ERROR     雷管类型符合不要求
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus CheckDetType(u32 Serl, u32 Pwd)
{
	if(GetDetType(Serl, Pwd) == Det.Type)
	{
		return SUCCESS;
	}
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  根据批号转出芯片类型
  * @param[in]  Type 雷管批号
  * @retval 类型
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 GetDetType(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>46)&0x1F;//  bit46-bit50共5位
}
/**
  *************************************************************************************
  * @brief  根据批号转出芯片类型
  * @param[in]  工厂代码
  * @retval 类型
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 GetDetFactoryCode(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>39)&0x7F;//  bit39-bit45共7位
}
/**
  *************************************************************************************
  * @brief  根据芯片类型判断是否为单电容类型
  * @param[in]  Type 雷管类型
  * @retval 0不是单电容，其他值表示单电容
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 DetIsSingleCap(u8 Type)
{
//	if(Type == DET_TYPE_XA_BD1)
//	{
//		return 1;
//	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  获取校准校准数量
  * @param[in]  无
  * @retval 无 
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u16 GetFixedPulseCount(void)
{
	return (1000 + BlastDelayConvertToDetDelay(GetDetDelayMaxFromRegStatus()));
}

/**
  *************************************************************************************
  * @brief  连接不上雷管时，错误代码分类，用于区分显示无法连接还是通讯异常
  * @param  无
  * @retval 结果
  *      @arg DET_TEST_CONNECT_ERR 无法连接
  *      @arg DET_TEST_COMM_ERR 通讯异常
  * @author ZH
  * @date   2018年6月28日
  * @note   无
  *************************************************************************************  
  */
u8 ConnectErrorCodeIdentify(void)
{
	if((DetProtocol.Rise.Count >= 1) && (DetProtocol.Fall.Count >= 1))
	{
		return DET_TEST_COMM_ERR;//能接收到信号,但解析不出来，认为是通讯错误
	}
	return DET_TEST_CONNECT_ERR;
}


//==================================================================================================
//| 函数名称 | CheckDet
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检测总线是否有雷管在线
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	det_buffer 保存检测到的雷管信息
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	DET_PASS,			  //检测到雷管
//|			 |	CHECK_UNDONE,         //检测未完成
//|			 |	COMM_ERR, 	          //通信出错
//|			 |	TEST_ERR,			  //BIT错误 
//|			 |	SECU_CODE_ERR,		  //密码校验错误
//|			 |	FREQ_MIN,			  //频率过小
//|			 |	FREQ_MAX,			  //频率过大
//|			 |	OVERLAPED,			  //上线过程中这个序列号已经注册 
//|			 |	TYPE_ERR,			  //雷管类型错误
//|			 |	CURRENT_MIN,		  //电流小
//|			 |	CURRENT_MAX,		  //电流大
//|			 |	CURRENT_UNSTABLE      //电流不稳定
//|----------|--------------------------------------------------------------------------------------        
//| 函数设计 | 编写人：ZH    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
CHECK_RESULT CheckDet(DET_INFO* det_buffer, u8 Reg)
{	 
	ErrorStatus Result;
	CHECK_RESULT check_result;
    u8 U8Value;
	u16 DetStatus;	
	u8 i;
	
	/*清缓冲区数据*/
	det_buffer->Serl.U32Value = 0;
	det_buffer->Pwd.U32Value  = 0;
	
	WriteSystemLog("开始注册雷管",strlen("开始注册雷管"), CURRENT_STATUS);
	
	for(i=0; i<5;i++)
	{
		/*发送全局复位指令*/
		AllDisCharge();
		Result = XA3_DetCheckIn();// 查漏，如果success，那么执行其后的动作，否则，返回没有雷管
		if(ERROR != Result)
		{
			break;
		}	
	}
	if(ERROR == Result)
	{
		WriteSystemLog("查漏失败",strlen("查漏失败"), NEW_STR_INFO);
		return COMM_ERR;
	}

	/*读取序列码*/
	Result = XA3_ReadSerl(&det_buffer->Serl.U32Value); 
	if(ERROR == Result)
	{
		WriteSystemLog("读取序列码失败",strlen("读取序列码失败"), NEW_STR_INFO);
		return COMM_ERR;
	}

	WriteSystemLog("注册,读序列码成功:",strlen("注册,读序列码成功:"), STR_INFO);
	WriteSystemLog(&det_buffer->Serl.U8Buffer[0], 4, HEX_ARRAY);
     
    Result = XA3_ReadPwdRetry(det_buffer->Serl.U32Value, &det_buffer->Pwd.U32Value); 
	if(ERROR == Result)
	{//
        WriteSystemLog("读取PWD失败",strlen("读取PWD失败"), NEW_STR_INFO);
        return COMM_ERR;   
	}

	WriteSystemLog("读PWD成功:",strlen("读PWD成功:"), STR_INFO);
	WriteSystemLog(&det_buffer->Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(!DetCodeRulePass(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
    {//不符合规则的内码，重读一次看看
        U32_UNION Serl, Pwd;
        
		WriteSystemLog("内码不符合规则",strlen("内码不符合规则"), NEW_STR_INFO);

        /*重读序列码*/
        Result = XA3_ReadSerl(&Serl.U32Value); 
        if(ERROR == Result)
        {
            WriteSystemLog("重读序列码失败",strlen("重读序列码失败"), NEW_STR_INFO);
            return COMM_ERR;
        }

        WriteSystemLog("重读序列码成功:",strlen("重读序列码成功:"), STR_INFO);
        WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
         
        Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value); 
        if(ERROR == Result)
        {//
            WriteSystemLog("重读PWD失败",strlen("重读PWD失败"), NEW_STR_INFO);
            return COMM_ERR;   
        }
        WriteSystemLog("重读PWD成功:",strlen("重读PWD成功:"), STR_INFO);
        WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
        
        if(det_buffer->Serl.U32Value != Serl.U32Value)
        {
            WriteSystemLog("序列码不一致",strlen("序列码不一致"), NEW_STR_INFO);
        }
        if(det_buffer->Pwd.U32Value != Pwd.U32Value)
        {
            WriteSystemLog("PWD不一致",strlen("PWD不一致"), NEW_STR_INFO);
        }

        /**屏蔽芯片**/
        Result = XA3_Mask(det_buffer->Serl.U32Value);
        if(ERROR == Result)
        {
            WriteSystemLog("屏蔽失败",strlen("屏蔽失败"), NEW_STR_INFO);
            return COMM_ERR;
        }
        
        if(!DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
        {
            WriteSystemLog("重读数据不符合规则", 0, NEW_STR_INFO);
            return DET_CODE_ERR;//雷管内码不符合规则，可能是内码变了
        }
        det_buffer->Serl.U32Value = Serl.U32Value;
        det_buffer->Pwd.U32Value = Pwd.U32Value;
    }
	/**屏蔽芯片**/
	Result = XA3_Mask(det_buffer->Serl.U32Value);
	if(ERROR == Result)
	{
		WriteSystemLog("屏蔽失败",strlen("屏蔽失败"), NEW_STR_INFO);
		return COMM_ERR;
	}
    //根据ID号限制设备只能使用对应管厂生产的雷管
    if(!DetFactoryCodeIsRulePass(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
    {
        WriteSystemLog("工厂代码不一致",strlen("工厂代码不一致"), NEW_STR_INFO);
        return DET_FAC_CODE_ERR;
    }
	/**检测芯片类型**/
	Result = CheckDetType(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);
	if(ERROR == Result)
	{
		WriteSystemLog("芯片类型错误",strlen("芯片类型错误"), NEW_STR_INFO);
		return TYPE_ERR;
	}
    /*解密*/
	U8Value = XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);	
	if(U8Value == 1)
	{
        WriteSystemLog("密码重试",0, NEW_STR_INFO);
		U8Value = XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);
	}
    
	if(U8Value == 1)
	{
		WriteSystemLog("密码错误:",strlen("密码错误:"), NEW_STR_INFO);
		return  SECU_CODE_ERR;//密码错误
	}
	else if(U8Value != 0)
	{
		WriteSystemLog("解密通讯错误",strlen("解密通讯错误"), NEW_STR_INFO);
		return  COMM_ERR;//密码错误
	}    
	/*测试BIT功能*/
    if(Reg)
    {//注册
        XA3_Charge(det_buffer->Serl.U8Buffer[0]);
        XA3_Charge(det_buffer->Serl.U8Buffer[0]);
        OSTimeDlyHMSM(0,0,0,500);//5.5V时，250-300ms左右，7V时，200ms左右。延时0.45秒
        
        Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//回读测试结果
        if(ERROR == Result)
        {
            WriteSystemLog("状态回读错误",strlen("状态回读错误"), NEW_STR_INFO);
            return COMM_ERR;
        }
        /*读取BIT状态*/
        if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // 判断BIT测试的状态
        {
            WriteSystemLog("芯片比较器状态错误",strlen("芯片比较器状态错误"), NEW_STR_INFO);
            return TEST_ERR;	
        }    
    }
    else
    {//单发检测
        XA3_Bit(det_buffer->Serl.U32Value);// bit测试
        XA3_Bit(det_buffer->Serl.U32Value);// bit测试

        OSTimeDlyHMSM(0,0,1,0);//延时1秒
        
        Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//回读测试结果
        if(ERROR == Result)
        {
            WriteSystemLog("状态回读错误",strlen("状态回读错误"), NEW_STR_INFO);
            return COMM_ERR;
        }
        /*读取BIT状态*/
        if (XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) == ERROR)  // 判断BIT测试的状态
        {
            WriteSystemLog("芯片BIT错误",strlen("芯片BIT错误"), NEW_STR_INFO);
            return TEST_ERR;	
        }       
    }
	
	/*获取频率，通过同步头测出*/	
	det_buffer->Freq.U32Value = GetDetFreqFromHead();
	
	/*判断频率是否正常*/
	check_result = CheckDetFreq(det_buffer->Freq.U32Value, 0);
	if(DET_PASS != check_result)
	{
        char Buffur[20];
        
        sprintf(Buffur, "频率错误:%dHz", det_buffer->Freq.U32Value);
        WriteSystemLog(Buffur,strlen(Buffur), NEW_STR_INFO);
		return check_result;
	}	
			
	/*检查重号*/
	Result = CheckOverlap(&det_buffer->Serl.U8Buffer[0]);
	if(ERROR == Result)
	{
        u8 Buffer[8];
        
		WriteSystemLog("重号错误:",strlen("重号错误:"), NEW_STR_INFO);
        memcpy(&Buffer[0], &det_buffer->Serl.U8Buffer[0], 4);
        memcpy(&Buffer[4], &det_buffer->Pwd.U8Buffer[0], 4);
        WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);

        memcpy(&Buffer[0], &Det.Info[Op_Data.overlaped].Serl.U8Buffer[0], 4);
        memcpy(&Buffer[4], &Det.Info[Op_Data.overlaped].Pwd.U8Buffer[0], 4);
        WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
        
		return OVERLAPED;
	}
    
	WriteSystemLog("雷管正常",strlen("雷管正常"), NEW_STR_INFO);
	
	return DET_PASS;
}
/**
  *************************************************************************************
  * @brief  芯片分析
  * @param  Param  det_buffer 读到的芯片芯片
  * @retval 分析结果，bit0
  *     @arg @b bit0 序列码回读结果
  *     @arg @b bit1 BIT测试结果
  *     @arg @b bit2 写延期结果
  *     @arg @b bit3 测频结果 
  *     @arg @b bit4 充电结果    
  * @author ZH
  * @date   2014年12月04日
  * @note   
  *************************************************************************************  
  */
u16 DetAnalyst(DET_INFO* det_buffer)
{
	ErrorStatus Result;
	u32 c_t;
	u16 AnalystResult = 0;
	u16 i_temp;
    u16 DetStatus;
    u8 Buffer[20];
//    U32_UNION Other;
//    u8 i;
    
	/*清缓冲区数据*/
	det_buffer->Serl.U32Value = 0;
	det_buffer->Pwd.U32Value = 0;
	det_buffer->Freq.U32Value = 0;
	det_buffer->Err = 0;
    
    AllReset();
    
	/*发送全局复位指令*/
	AllDisCharge();	
	
	/*雷管刚上线有可能不稳定*/
	OSTimeDly(300);
	c_t = OSTimeGet();
	while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
	{
		i_temp = Op_Data.LinABCurrent;
		OSTimeDlyHMSM(0, 0, 0, 200);//延时
		if(abs(Op_Data.LinABCurrent - i_temp) < 20)
		{
			i_temp = Op_Data.LinABCurrent;
			OSTimeDlyHMSM(0, 0, 0, 200);//延时
			if(abs(Op_Data.LinABCurrent - i_temp) < 20)
			{
                break;	
			}
		}		
	}	

	BlastInfo.I_BeforeTest = Op_Data.LinABCurrent;//正向静态电流
	OSTimeDlyHMSM(0, 0, 0, 100);
	
	c_t = OSTimeGet();
	while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
	{
		i_temp = Op_Data.LinABCurrent;
		OSTimeDlyHMSM(0, 0, 0, 200);//延时
		if(abs(Op_Data.LinABCurrent - i_temp) < 20)
		{
			i_temp = Op_Data.LinABCurrent;
			OSTimeDlyHMSM(0, 0, 0, 200);//延时
			if(abs(Op_Data.LinABCurrent - i_temp) < 20)
			{
                break;
			}
		}		
	}
	
	BlastInfo.I_BeforeFire = Op_Data.LinABCurrent;//反向静态电流
	
	/*发送全局复位指令*/
	AllDisCharge();	
	OSTimeDlyHMSM(0, 0, 0, 50);//延时
	/*读取序列码*/
	Result = XA3_ReadSerl(&det_buffer->Serl.U32Value); 
	if(ERROR != Result)
	{        
		Result = XA3_ReadPwdRetry(det_buffer->Serl.U32Value, &det_buffer->Pwd.U32Value); 
        if(ERROR != Result)
        {
            AnalystResult |= DET_ANALYST_READ_SN_MASK_BIT;//序列码回读正常
            
            det_buffer->Freq.U32Value = GetDetFreqFromHead();
            
            AnalystResult |= DET_ANALYST_FREQ_MASK_BIT;//测频正常
            
            memset(Buffer, 0, sizeof(Buffer));
            memcpy(Buffer, &det_buffer->Serl.U8Buffer[0], 4);
            memcpy(&Buffer[4], &det_buffer->Pwd.U8Buffer[0], 4);
            WriteSystemLog("芯片分析内码:",strlen("芯片分析内码:"), CURRENT_STATUS);
            WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
            
            memset(Buffer, 0, sizeof(Buffer));
            strcpy((char*)Buffer, "管码:");
            if(ERROR != XA3_DetInside2Outside(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
            {
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
            }
            
//            for(i=4;i<7;i++)
//            {
//                if(XA3_ReadEEPROM(i, (u8*)&Other.U8Buffer[0]) == ERROR)
//                {
//                    sprintf((char*)Buffer, "读地址%d失败", i);
//                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
//                }
//                else
//                {
//                    sprintf((char*)Buffer, "读地址%d成功:", i);
//                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
//                    
//                    WriteSystemLog(&Other.U8Buffer[0], 4, HEX_ARRAY);                
//                }
//            }
            
            /*测试BIT功能*/
            XA3_Bit(det_buffer->Serl.U8Buffer[0]);// bit测试
            XA3_Bit(det_buffer->Serl.U8Buffer[0]);// bit测试
            OSTimeDlyHMSM(0,0,1,0);//延时1秒	

            /*读取BIT状态*/
            Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//回读测试结果
            if(ERROR == Result)
            {
                return 0;		
            }
            if (XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) != ERROR)  // 判断BIT测试的状态
            {
                AnalystResult |= DET_ANALYST_BIT_RESULT_MASK_BIT;//BIT正常	
            }
            
            Result = XA3_WriteTime(det_buffer->Serl.U32Value, DET_FREQ_MEASURE_WRITE_VALUE);
            if(ERROR == Result)
            {
                return 0;
            }
            AnalystResult |= DET_ANALYST_WRITE_DELAY_MASK_BIT;//写延期正常
            
            /*解密*/
            if(0 == XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
            {
                AnalystResult |= DET_ANALYST_UNLOCK_MASK_BIT;//解密正常
            }
            XA3_Charge(det_buffer->Serl.U8Buffer[0]);
            XA3_Charge(det_buffer->Serl.U8Buffer[0]);
            OSTimeDlyHMSM(0,0,0,500);//5.5V时，250-300ms左右，7V时，200ms左右。延时0.45秒
            
            if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Op_Data.DelaySampleCfg))
            {//管厂专用的设备才需要
                XA3_ExpenAll();
                XA3_ExpenAll();	
                OSTimeDlyHMSM(0,0,0,100);
            }
            
            /*读取充电状态*/
            Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//回读测试结果
            if(ERROR == Result)
            {
                return 0;
            }
            if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) != ERROR)  // 判断充电测试的状态
            {
                if(XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) != ERROR)
                {
                    AnalystResult |= DET_ANALYST_CHARGE_MASK_BIT;//充电正常
                }
            }
            
            XA3_Mask(det_buffer->Serl.U32Value);//屏蔽雷管，方便重新发现新雷管
            
            OSTimeDlyHMSM(0, 0, 1, 0);
            c_t = OSTimeGet();
            while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
            {
                i_temp = Op_Data.LinABCurrent;
                OSTimeDlyHMSM(0, 0, 0, 200);//延时
                if(abs(Op_Data.LinABCurrent - i_temp) < 20)
                {
                    i_temp = Op_Data.LinABCurrent;
                    OSTimeDlyHMSM(0, 0, 0, 200);//延时
                    if(abs(Op_Data.LinABCurrent - i_temp) < 20)
                    {
                        break;
                    }
                }		
            }
            BlastInfo.I_AfterCharge = Op_Data.LinABCurrent;            
        }
	}
	
	return AnalystResult;
}

/**
  *************************************************************************************
  * @brief  屏蔽所有雷管
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月04日
  * @note   
  *************************************************************************************  
  */
TEST_RESULT DetMaskAll(void)
{
	ErrorStatus Result;
	u16 i;		

	//初始化进度条数据
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount;
	SetProgbarParam();
	
    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();	

	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条			
			/*发送屏蔽指令，目的是为了查漏*/
			XA3_Mask(Det.Info[i].Serl.U32Value);
		}	
	}
	FillCurrentProgbar();
	
	/*查漏,连续5次发送点名命令都有数据返回则说明有未注册雷管*/
    Op_Data.FindUnRegDetInfo = 0;
	if(HaveUnregisteredDetBySimpleSignal())
	{
        WriteSystemLog("检测到有未注册雷管",strlen("检测到有未注册雷管"), CURRENT_STATUS);
        
        Result = XA3_ReadSerl(&Op_Data.UnRegDetInfo.Serl.U32Value); 
        if(ERROR != Result)
        {
            Result = XA3_ReadPwdRetry(Op_Data.UnRegDetInfo.Serl.U32Value, &Op_Data.UnRegDetInfo.Pwd.U32Value);
            if(ERROR != Result)
            {
                u8 Buffer[20];
                
                Op_Data.FindUnRegDetInfo = 1;
                    
                memset(Buffer, 0, sizeof(Buffer));
                memcpy(Buffer, &Op_Data.UnRegDetInfo.Serl.U8Buffer[0], 4);
                memcpy(&Buffer[4], &Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], 4);
                WriteSystemLog("内码:",strlen("内码:"), STR_INFO);
                WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
                
                memset(Buffer, 0, sizeof(Buffer));
                strcpy((char*)Buffer, "管码:");
                if(ERROR != XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, 
                    Op_Data.UnRegDetInfo.Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
                {
                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
                }
            }            
        }
		return UNREG_DET;//检测到有未注册雷管
	}
	return TEST_END;  //没有检测到未注册雷管	
}

//==================================================================================================
//| 函数名称 | TestDetQuick
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 网路快速测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	
//|			 |	TEST_END    测试完成
//|			 |	UNREG_DET   测试完成，有未注册雷管
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
TEST_RESULT TestDetQuick(void)
{
	ErrorStatus Result;
    u8 U8Value;
	u16 i;
    u8 FindUnRegDet = 0;
    TEST_RESULT TestResult = TEST_END;  //没有检测到未注册雷管
    
	/*开始测试前先初始化错误代码及计算芯片延期对应的时钟数*/
	TestInfoInit();

	//初始化进度条数据
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount;
	SetProgbarParam();
    
    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
			
			/*发送屏蔽指令，目的是为了查漏*/
			Result = XA3_Mask(Det.Info[i].Serl.U32Value);
			if(ERROR == Result)
			{
				SaveErrCode(i,(0<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码      
                continue;
			}
            
            U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            if(1 == U8Value)
            {//如果密码错误时，再重试一次，如果是芯片内部读密码错误时，有可能通过。
				U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
                if(U8Value == 0)
                {
                    u8 Buffer[20];
                    
                    sprintf((char*)Buffer, "%d-%d 重试解密成功",
                        Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside);
                    WriteSystemLog(Buffer,strlen((char*)Buffer), NEW_STR_INFO);
                }
            }
			if(1 == U8Value)
            {
				SaveErrCode(i,(1<<4)+DET_TEST_PWD_ERR) ;       //保存错误代码
            }
            else if(0 != U8Value)
            {
                SaveErrCode(i,(1<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
            } 
		}	
	}

	/*查漏,连续5次发送点名命令都有数据返回则说明有未注册雷管*/
    Op_Data.FindUnRegDetInfo = 0;
	if(HaveUnregisteredDetBySimpleSignal())
	{
        Result = XA3_ReadSerl(&Op_Data.UnRegDetInfo.Serl.U32Value); 
        if(ERROR != Result)
        {
            Result = XA3_ReadPwdRetry(Op_Data.UnRegDetInfo.Serl.U32Value, &Op_Data.UnRegDetInfo.Pwd.U32Value);
            if(ERROR != Result)
            {
                Op_Data.FindUnRegDetInfo = 1;
            }            
        }
		FindUnRegDet = 1;//检测到有未注册雷管
	}
    
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{            
            if((Det.Info[i].Err&0x0F) == DET_TEST_PWD_ERR)
            {
                U32_UNION Pwd;
                u8 Buffer[40];
                
                if(ERROR != XA3_ReadPwdRetry(Det.Info[i].Serl.U32Value, &Pwd.U32Value))
                {
                    if(Pwd.U32Value != Det.Info[i].Pwd.U32Value)
                    {
                        sprintf((char*)Buffer, "%d-%d %02X%02X%02X%02X%02X%02X%02X%02X 实际PWD:%02X%02X%02X%02X",
                            Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside,
                            Det.Info[i].Serl.U8Buffer[0],Det.Info[i].Serl.U8Buffer[1],Det.Info[i].Serl.U8Buffer[2],Det.Info[i].Serl.U8Buffer[3],
                            Det.Info[i].Pwd.U8Buffer[0],Det.Info[i].Pwd.U8Buffer[1],Det.Info[i].Pwd.U8Buffer[2],Det.Info[i].Pwd.U8Buffer[3],
                            Pwd.U8Buffer[0],Pwd.U8Buffer[1],Pwd.U8Buffer[2],Pwd.U8Buffer[3]);
                        WriteSystemLog(Buffer,strlen((char*)Buffer), NEW_STR_INFO);
                    }
                    if(!DetCodeRulePass(Det.Info[i].Serl.U32Value, Pwd.U32Value))
                    {
                        Det.Info[i].Err = (2<<4)+DET_TEST_PWD_ERR;
                        continue;
                    }
                    
                    if(GetDetType(Det.Info[i].Serl.U32Value, Pwd.U32Value) != 
                        GetDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
                    {
                        Det.Info[i].Err = (2<<4)+DET_TEST_TYPE_ERR;
                        continue;
                    }
                }
                else
                {
                    sprintf((char*)Buffer, "%d-%d 读PWD失败",
                        Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside);
                    WriteSystemLog(Buffer,strlen((char*)Buffer), NEW_STR_INFO);
                }
            }            
        }
    }
	/*发送全局复位指令*/
	AllDisCharge();
	
	/*统计错误数*/
	GetDetNum();

	FillCurrentProgbar();
    
	if(FindUnRegDet)
	{
        WriteSystemLog("检测到有未注册雷管",strlen("检测到有未注册雷管"), CURRENT_STATUS);
        if(Op_Data.FindUnRegDetInfo)
        {
            u8 Buffer[20];
            
            memset(Buffer, 0, sizeof(Buffer));
            memcpy(Buffer, &Op_Data.UnRegDetInfo.Serl.U8Buffer[0], 4);
            memcpy(&Buffer[4], &Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], 4);
            WriteSystemLog("内码:",strlen("内码:"), STR_INFO);
            WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
            
            memset(Buffer, 0, sizeof(Buffer));
            strcpy((char*)Buffer, "管码:");
            if(ERROR != XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, 
                Op_Data.UnRegDetInfo.Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
            {
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
            }
        }
		TestResult = UNREG_DET;//检测到有未注册雷管
	}
    if(Det.ErrorCount == 1)
    {//如果是内码变的话，尝试纠正
        for(i=0; i < DET_MAX_SIZE; i++)
        {
            if ((GetDetRegStatus(i) != ERROR) && (Det.Info[i].Err != 0))
            {
                if((Det.Info[i].Err&0x0F) == DET_TEST_CONNECT_ERR)
                {//未连接,有可能序列码变了
                    if(FindUnRegDet && Op_Data.FindUnRegDetInfo)
                    {//发现未注册雷管，并且回读到了序列码
                        if(DetCodeRulePass(Op_Data.UnRegDetInfo.Serl.U32Value, Op_Data.UnRegDetInfo.Pwd.U32Value))
                        {//发现的未注册雷管，符合规则，不是内码变
                            break;
                        }
                        WriteSystemLog("有未注册,尝试纠正内码", 0, STR_INFO);
                        
                        //屏蔽后再查漏，确保总线上只有一发未注册雷管
                        Result = XA3_Mask(Op_Data.UnRegDetInfo.Serl.U32Value);
                        if(ERROR == Result)
                        {
                            WriteSystemLog("屏蔽失败", 0, STR_INFO);
                            break;
                        }
                        if(HaveUnregisteredDetBySimpleSignal())
                        {//屏蔽后还有未注册雷管
                            WriteSystemLog("屏蔽后还有未注册", 0, STR_INFO);
                            break;
                        }
                        
                        if(Op_Data.UnRegDetInfo.Pwd.U32Value != Det.Info[i].Pwd.U32Value)
                        {//如果密码也变了就先写密码
                            U8Value = XA3_UnlockAndWriteEEPROM(Op_Data.UnRegDetInfo.Serl.U32Value, 3,  Det.Info[i].Pwd.U8Buffer);
                            if(U8Value == 1)
                            {//解EEPROM失败
                                WriteSystemLog("解EEPROM失败1", 0, STR_INFO);
                                break;
                            }
                            else if(U8Value != 0)
                            {//写失败
                                WriteSystemLog("写PWD失败", 0, STR_INFO);
                                break;
                            }
                            WriteSystemLog("已写PWD", 0, STR_INFO);                            
                        }
                        if(Op_Data.UnRegDetInfo.Serl.U32Value != Det.Info[i].Serl.U32Value)    
                        {//如果改序列码
                            U8Value = XA3_UnlockAndWriteEEPROM(Op_Data.UnRegDetInfo.Serl.U32Value, 2,  Det.Info[i].Serl.U8Buffer);
                            if(U8Value == 1)
                            {//解EEPROM失败
                                WriteSystemLog("解EEPROM失败2", 0, STR_INFO);
                                break;
                            }
                            else if(U8Value != 0)
                            {//写失败
                                WriteSystemLog("写序列码失败", 0, STR_INFO);
                                break;
                            }
                            WriteSystemLog("已写序列码", 0, STR_INFO);
                        }
                    }
                    else
                    {
                        break;
                    }
                }            
                else if((Det.Info[i].Err&0x0F) == DET_TEST_PWD_ERR)
                {//密码错误，有可能是密码变了
                    U32_UNION Pwd;
                    
                    //当前总线不能有未注册雷管
                    if(HaveUnregisteredDetBySimpleSignal())
                    {//有未注册雷管
                        break;
                    }
                    WriteSystemLog("密码错误,尝试纠正内码", 0, STR_INFO);
                    
                    //回读密码
                    if(ERROR == XA3_ReadPwdRetry(Det.Info[i].Serl.U32Value, &Pwd.U32Value))
                    {
                        WriteSystemLog("回读PWD失败", 0, STR_INFO);
                        break;
                    }
                    if(DetCodeRulePass(Det.Info[i].Serl.U32Value, Pwd.U32Value))
                    {//符合规则，不是内码变
                        WriteSystemLog("PWD符合规则", 0, STR_INFO);
                        break;
                    }
                    if(Det.Info[i].Pwd.U32Value == Pwd.U32Value)
                    {
                        WriteSystemLog("PWD相同", 0, STR_INFO);//不应该出现密码相同又解密失败，有可能是芯片内部读数据时出错
                        break;
                    }

                    U8Value = XA3_UnlockAndWriteEEPROM(Det.Info[i].Serl.U32Value, 3,  Det.Info[i].Pwd.U8Buffer);
                    if(U8Value == 1)
                    {//解EEPROM失败
                        WriteSystemLog("解EEPROM失败", 0, STR_INFO);
                        break;
                    }
                    else if(U8Value != 0)
                    {//写失败
                        WriteSystemLog("写PWD失败", 0, STR_INFO);
                        break;
                    }
                    WriteSystemLog("已写PWD", 0, STR_INFO);     
                }
                else
                {
                    break;
                }
                //复位芯片，重读内码
                WriteSystemLog("复位验证", 0, STR_INFO);
                XA3_Reset(Det.Info[i].Serl.U32Value);
                XA3_Reset(Det.Info[i].Serl.U32Value);
                OSTimeDlyHMSM(0,0,0,50);
                AllDisCharge();
                
                Result = XA3_Mask(Det.Info[i].Serl.U32Value);
                if(ERROR == Result)
                {
                    WriteSystemLog("屏蔽失败", 0, STR_INFO);
                    break;
                }
                U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
                if(U8Value == 0)
                {
                    WriteSystemLog("内码已经纠正", 0, STR_INFO);
                    
                    Det.Info[i].Err = 0;
                    GetDetNum();
                    FindUnRegDet = 0;
                    TestResult = TEST_END;  //没有检测到未注册雷管
                }
                else if(U8Value == 1)
                {
                    WriteSystemLog("PWD错误", 0, STR_INFO);
                }
                else
                {
                    WriteSystemLog("通讯异常", 0, STR_INFO);
                }
                break;
            }
        }
    }
    return TestResult;
}
//==================================================================================================
//| 函数名称 | TestDetComplete
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 网路完全测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	TEST_UNDONE  未测试完成
//|			 |	TEST_END   测试完成
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void TestDetComplete(void)
{
	ErrorStatus Result;
    u16 DetStatus;
	u16 i;
    
	/*开始测试前先初始化错误代码及计算芯片延期对应的时钟数*/
	TestInfoInit();

	//初始化进度条数据
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = DetSerlGroupInfo.Count+ Det.RegCount;
	SetProgbarParam();
	
    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();	
	
	for(i=0; i < DetSerlGroupInfo.Count; i++)
	{
		Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
		if(DetIsSingleCap(Det.Type) == 0)
		{//双电容
			/*BIT命令*/
			XA3_Bit(DetSerlGroupInfo.Group[i].Serl);
			XA3_Bit(DetSerlGroupInfo.Group[i].Serl);
			if(DetSerlGroupInfo.Group[i].Count > 50)
			{
				OSTimeDly(1000);
			}
			else if(DetSerlGroupInfo.Group[i].Count > 10)
			{
				OSTimeDly(500);
			}
			else
			{
				OSTimeDly(50*DetSerlGroupInfo.Group[i].Count);
			}
		}
	}
	if(DetIsSingleCap(Det.Type) == 0)
	{//双电容
		OSTimeDlyHMSM(0,0,1,0);//延时等待所有雷管BIT测试完成
	}
		
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条	

			/*读取BIT状态*/
			Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);//回读测试结果
			if(ERROR == Result)
			{
				SaveErrCode(i, (1<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
				continue;//下一发雷管
			}	
			
            if(XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) == ERROR)  // 判断BIT测试的状态
            {//BIT测试错误标志
                if(XA3_GetDetStatus(DetStatus, XA3_CAP_STATUS) == ERROR)
                {//起爆电容焊接错误标志
                    SaveErrCode(i, (3<<4)+DET_TEST_BIT_ERR) ;       //保存错误代码	
                }
                else
                {
                    SaveErrCode(i, (4<<4)+DET_TEST_BIT_ERR) ;       //保存错误代码	
                }
            }
		}	
	}
    
	/*发送全局复位指令*/
	AllDisCharge();
	
	/*统计错误数*/
	GetDetNum();	

	FillCurrentProgbar();//更新进度条到100%
}

//==================================================================================================
//| 函数名称 | BlasteTestComplete
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 起爆流程完全测试	
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	TEST_END     测试完成
//|			 |	TEST_UNDONE  未测试完成
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-30 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 使用前请初始化op_data.u32_temp1为0xFFFF
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void BlasteTestComplete(void)
{
	ErrorStatus Result;
    u8 ResultCode;
	u16 i;
    u32 TimeClk;
    u8 TimeClkFlag;
    u16 DetStatus;
    
	/*开始测试前先初始化错误代码及计算芯片延期对应的时钟数*/
	TestInfoInit();

	//初始化进度条数据
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount*2+100+GetFixedPulseCount()/200;
	Op_Data.Progbar.Bar[1].Max = Det.RegCount+DetSerlGroupInfo.Count;
	Op_Data.Progbar.Bar[2].Max = Det.RegCount;
	SetProgbarParam();


    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();
    
	/************************写延期阶段***************************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
            
			Result = XA3_Mask(Det.Info[i].Serl.U32Value);
			if(ERROR == Result)
			{
				SaveErrCode(i,(0<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
			}
            else
            {
                Det.Info[i].Freq.U32Value = GetDetFreqFromHead();//根据同步头计算出的频率
                switch(CheckDetFreq(Det.Info[i].Freq.U32Value, 1))
                {
                    case FREQ_MIN:
                        SaveErrCode(i,(0<<4)+DET_TEST_FREQ_MIN_ERR) ;
                        break;
                    case FREQ_MAX:
                        SaveErrCode(i,(0<<4)+DET_TEST_FREQ_MAX_ERR) ;
                        break;
                    default:
                        break;
                }
            }
		}	
	}	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
            
			if((Det.Info[i].Freq.U32Value < DET_RECEIVE_FREQ_MIN) || (Det.Info[i].Freq.U32Value > DET_RECEIVE_FREQ_MAX))
			{
				Det.Info[i].Freq.U32Value = 80000;//默认频率
			}			
			
			Det.Info[i].Delayclk.U32Value = BlastDelayConvertToDetDelayClock(Det.Info[i].Delay, Det.Info[i].Freq.U32Value);
			if(BlastDelayConvertToDetDelay(Det.Info[i].Delay) != 0)
			{//写入固定延期
                ResultCode = XA3_WriteFixDelayTest(Det.Info[i].Serl.U32Value, BlastDelayConvertToDetDelay(Det.Info[i].Delay));
                if(ResultCode == 1)
                {//解锁失败
                    SaveErrCode(i,(2<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
                }
                else if(ResultCode != 0)
                {//写固定延期失败
                    SaveErrCode(i,(3<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码   
                }				
			}
			else
			{
				/*写延期*/
				Result = XA3_WriteTime(Det.Info[i].Serl.U32Value, Det.Info[i].Delayclk.U32Value);
				if(ERROR == Result)
				{
					SaveErrCode(i,(4<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
				}			
			}			
		}
	}
	
	//对芯片延期进行时钟校准
    if(Det.RegCount > 0)
    {
        if(BlastDelayConvertToDetDelay(BlastDelayConvertToDetDelay(GetDetDelayMaxFromRegStatus())) != 0)
        {
            XA3_CheckOscAll(DET_FIXED_PULSE_MAX_FOR_MS, DET_FIXED_PULSE_MIN_FOR_MS);
            XA3_CheckOscAll(DET_FIXED_PULSE_MAX_FOR_MS, DET_FIXED_PULSE_MIN_FOR_MS);
            XA3_DetFixedPulse(GetFixedPulseCount(), TNUS(100), TNUS(900));
        }
        //加延时让总线更稳定些
        for(i=0; i < 100; i++)
        {
            OSTimeDly(50);
            Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
        }    
    }

	FillCurrentProgbar();//更新进度条到100%
	Op_Data.Progbar.Index++;//切换到下一个进度条
	
	/************************充电和准备起爆阶段**************************/
	//对芯片解密
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条

            ResultCode = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            if(ResultCode == 1)
            {//如果密码错误时，再重试一次，如果是芯片内部读密码错误时，有可能通过。
                ResultCode = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            }
            
			if(ResultCode == 0xFF)
			{
				SaveErrCode(i,(5<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
			}
            else if(ResultCode != 0)
            {
                SaveErrCode(i,(5<<4)+DET_TEST_PWD_ERR) ;
            }
		}		
	}

	for(i=0; i < DetSerlGroupInfo.Count; i++)
	{
		Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条

		XA3_Charge(DetSerlGroupInfo.Group[i].Serl);
		XA3_Charge(DetSerlGroupInfo.Group[i].Serl);
		if(DetSerlGroupInfo.Group[i].Count > 50)
		{
			OSTimeDly(1000);
		}
		else if(DetSerlGroupInfo.Group[i].Count > 10)
		{
			OSTimeDly(500);
		}
		else
		{
			OSTimeDly(50*DetSerlGroupInfo.Group[i].Count);
		}
	}
	OSTimeDlyHMSM(0,0,1,0);
//	XA2_ExpenAll();
//	XA2_ExpenAll();

	FillCurrentProgbar();//更新进度条到100%
	Op_Data.Progbar.Index++;//切换到下一个进度条
	
	/************************验证延时时间阶段**************************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条

			Result = XA3_ReadTimeClock(Det.Info[i].Serl.U32Value, &TimeClk, NULL, &TimeClkFlag);
			OSTimeDly(10);//延期时间回读返回的数据较多，多加延时给电容补电
			if(ERROR == Result)
			{
				SaveErrCode(i,(6<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
			}
			else
			{
				/*判断从芯片中读取到的延时时钟与写入值是否相同*/
//				if(Det.Info[i].delayclk != U32Value)				
				if((TimeClkFlag == 0) || 
					((TimeClk > ((u32)(Det.Info[i].Delayclk.U32Value*1.1)))||(TimeClk < ((u32)(Det.Info[i].Delayclk.U32Value*0.9)))))
//				if((GetDetWriteDelayFLag() == 0) || (Det.Info[i].delayclk != U32Value))
				{//延期可能没有写入					
					/*写延期*/
					Result = XA3_WriteTime(Det.Info[i].Serl.U32Value, Det.Info[i].Delayclk.U32Value);
					if(ERROR == Result)
					{
						SaveErrCode(i,(7<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
					}
					
					Result = XA3_ReadTimeClock(Det.Info[i].Serl.U32Value, &TimeClk, NULL, &TimeClkFlag);
					OSTimeDly(10);//延期时间回读返回的数据较多，多加延时给电容补电
					if(ERROR == Result)
					{
						SaveErrCode(i,(8<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
					}
					else
					{
						if((TimeClkFlag == 0) ||(Det.Info[i].Delayclk.U32Value != TimeClk))
						{
							SaveErrCode(i,(8<<4)+DET_TEST_DELAY_CLOCK_ERR) ;       //延期值错误
						}
					}
					//准备起爆指令需在芯片充电标志：charge为1，延期时间已经设定标志Time_ok 为1 这两个前提下才能正确接收；
					//如果延时没有写入，会导致准备起爆指令失败。则需要重发准备起爆指令				
//					XA3_ExpenAll();					
				}					
			}
			Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);
			if(ERROR == Result)
			{
				SaveErrCode(i,(9<<4)+ConnectErrorCodeIdentify()) ;       //保存错误代码
			}
			else
			{		
				if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) == ERROR)
				{
					if (Det.Info[i].Err == 0)
					{
						SaveRetryErrCode(i,(9<<4)+DET_TEST_CHARGE_ERR) ;       //充电异常,重试
					}					
				}
                else if(DetIsSingleCap(Det.Type) == 0)              
                {//双电容
                    if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // 判断BIT测试的状态,此时应该是1
                    {
                        SaveErrCode(i,(9<<4)+DET_TEST_BIT_ERR) ;                    
                    }
                }
//				if(XA3_GetDetStatus(DetStatus, XA3_EXPEN_STATUS) == ERROR)
//				{
//					SaveErrCode(i,(9<<4)+DET_TEST_EXPEN_ERR) ;       //准备起爆异常					
//				}				
			}			
		}
	}		
	/***************针对充电及准备起爆错误雷管重试阶段********************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if((DET_TEST_CHARGE_ERR == (Det.Info[i].RetryErr&0x0F)) || (DET_TEST_EXPEN_ERR == (Det.Info[i].RetryErr&0x0F)))
			{
				/************************充电前先解密**************************/
//				DetUnLockPwdAttemptPwd(&Det.Info[i], 0, NULL);
                XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
				
				/************************充电命令**************************/
				XA3_Charge(Det.Info[i].Serl.U32Value);
				XA3_Charge(Det.Info[i].Serl.U32Value);
				OSTimeDlyHMSM(0,0,0,50);//延时50ms	

				/************************准备起爆命令**********************/
//				XA3_ExpenAll();
//				XA3_ExpenAll();
//				OSTimeDlyHMSM(0,0,0,5);//延时5ms
			}		
		}
	}
	/*************针对充电及准备起爆错误雷管重新验证阶段*******************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if((DET_TEST_CHARGE_ERR == (Det.Info[i].RetryErr&0x0F)) || (DET_TEST_EXPEN_ERR == (Det.Info[i].RetryErr&0x0F)))
			{
				/*清空当前雷管错误代码*/
				SaveErrCode(i, 0x00) ;      

				/*重新读取延时值进行判断*/
				Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);
				if(ERROR == Result)
				{
					SaveErrCode(i,(10<<4)+ConnectErrorCodeIdentify()) ;
				}
				else
				{
					if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) == ERROR)
					{
						SaveErrCode(i,(10<<4)+DET_TEST_CHARGE_ERR) ;       //充电异常		
					}
                    else if(DetIsSingleCap(Det.Type) == 0)              
                    {//双电容
                        if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // 判断BIT测试的状态,此时应该是1
                        {
                            SaveErrCode(i,(10<<4)+DET_TEST_BIT_ERR) ;                    
                        }
                    }
//					if(XA3_GetDetStatus(DetStatus, XA3_EXPEN_STATUS) == ERROR)
//					{
//						SaveErrCode(i,(10<<4)+DET_TEST_EXPEN_ERR) ;       //准备起爆异常	
//					}				
				}
			}		
		}
	}
    
	/*统计错误数*/
	GetDetNum();
	FillCurrentProgbar();//更新进度条到100%
}

/**
  *************************************************************************************
  * @brief  重写雷管内码
  * @param  NewSerl 新的序列码
  * @param  NewPwd 新的密码
  * @retval 无
  * @author ZH
  * @date   2019年4月20日
  * @note   
  *************************************************************************************  
  */
u8 XA3_DetCodeWriteNew(U32_UNION* NewSerl, U32_UNION* NewPwd)
{
    ErrorStatus Result;
    u8 U8Value;
    U32_UNION Serl, Pwd;
    u8 Buffer[20];
    
    AllDisCharge();
    
    WriteSystemLog("开始重写新内码:",strlen("开始重写新内码:"), CURRENT_STATUS);
    WriteSystemLog(&NewSerl->U8Buffer[0], 4, HEX_ARRAY);
    WriteSystemLog(&NewPwd->U8Buffer[0], 4, HEX_ARRAY);
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(NewSerl->U32Value, NewPwd->U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("新管码:",strlen("新管码:"), CURRENT_STATUS);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }
    
    //先读出序列码和密码
    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("读序列码失败",strlen("读序列码失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("读序列码成功:",strlen("读序列码成功:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("读PWD失败",strlen("读PWD失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("读PWD成功:",strlen("读PWD成功:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
    {
        WriteSystemLog("符合规则",strlen("符合规则"), STR_INFO);
    }
    else
    {
        WriteSystemLog("不符合规则",strlen("不符合规则"), STR_INFO);
    }
    
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("原管码:",strlen("原管码:"), CURRENT_STATUS);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }

    //写密码
    U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 3,  NewPwd->U8Buffer);
    if(U8Value == 1)
    {//解EEPROM失败
        WriteSystemLog("解EEPROM失败",strlen("解EEPROM失败"), STR_INFO);
        return 0; 
    }
    else if(U8Value != 0)
    {//写失败
        WriteSystemLog("写失败",strlen("写失败"), STR_INFO);
        return 0; 
    }
    
    //写序列码
    U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 2,  NewSerl->U8Buffer);
    if(U8Value == 1)
    {//解EEPROM失败
        WriteSystemLog("解EEPROM失败",strlen("解EEPROM失败"), STR_INFO);
        return 0; 
    }
    else if(U8Value != 0)
    {//写失败
        WriteSystemLog("写失败",strlen("写失败"), STR_INFO);
        return 0; 
    }
    
    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();	

    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("重读序列码失败",strlen("重读序列码失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("重读序列码成功:",strlen("重读序列码成功:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Serl.U32Value != NewSerl->U32Value)
    {
        WriteSystemLog("序列码不一致",strlen("序列码不一致"), STR_INFO);
        return 0;
    }
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("重读PWD失败",strlen("重读PWD失败"), STR_INFO);
		return 0;
    } 
    WriteSystemLog("重读PWD成功:",strlen("重读PWD成功:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Pwd.U32Value != NewPwd->U32Value)
    {
        WriteSystemLog("PWD不一致",strlen("PWD不一致"), STR_INFO);
        return 0;
    }
    WriteSystemLog("内码重写成功",strlen("内码重写成功"), STR_INFO);
    return 1;
}
/**
  *************************************************************************************
  * @brief  利用备份的数据重写雷管内码
  * @param  无
  * @retval 0 通讯异常，1改写成功 ，2备份数据不符合规则
  * @author ZH
  * @date   2019年4月20日
  * @note   
  *************************************************************************************  
  */
u8 XA3_DetCodeWriteFromBackup(void)
{
    ErrorStatus Result;
    u8 i;
    u8 U8Value;
    U32_UNION Serl, Pwd, Other, NewSerl, NewPwd;
    u8 Buffer[20];
    
    AllDisCharge();
    
    WriteSystemLog("开始用备份重写内码",strlen("开始用备份重写内码"), CURRENT_STATUS);
    
    //先读出序列码和密码
    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("读序列码失败",strlen("读序列码失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("读序列码成功:",strlen("读序列码成功:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("读PWD失败",strlen("读PWD失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("读PWD成功:",strlen("读PWD成功:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
    {
        WriteSystemLog("符合规则",strlen("符合规则"), STR_INFO);
    }
    else
    {
        WriteSystemLog("不符合规则",strlen("不符合规则"), STR_INFO);
    }
    
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("原管码:",strlen("原管码:"), STR_INFO);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }
    
    for(i=0;i<8;i++)
    {
        if((i != 2) &&(i != 3))
        {
            if(XA3_ReadEEPROM(i, (u8*)&Other.U8Buffer[0]) == ERROR)
            {
                sprintf((char*)Buffer, "读地址%d失败", i);
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
                return 0;
            }
            sprintf((char*)Buffer, "读地址%d成功:", i);
            WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
            
            WriteSystemLog(&Other.U8Buffer[0], 4, HEX_ARRAY); 
            
            if(i==4)
            {
                NewSerl.U32Value = Other.U32Value;
            }
            else if(i==5)
            {
                NewPwd.U32Value = Other.U32Value;
            }
        }  
    }
    if(DetCodeRulePass(NewSerl.U32Value, NewPwd.U32Value))
    {
        WriteSystemLog("新内码符合规则",strlen("新内码符合规则"), STR_INFO);
    }
    else
    {
        WriteSystemLog("新内码不符合规则",strlen("新内码不符合规则"), STR_INFO);
        return 2;
    }
    if((NewSerl.U32Value == Serl.U32Value) && (NewPwd.U32Value == Pwd.U32Value))
    {
        WriteSystemLog("新旧内码相同",strlen("新旧内码相同"), STR_INFO);
        
        Op_Data.RegDetInfo.Serl.U32Value = Serl.U32Value;//用于界面显示
        Op_Data.RegDetInfo.Pwd.U32Value = Pwd.U32Value;
        return 3;
    }
    if(NewPwd.U32Value != Pwd.U32Value)
    {
        WriteSystemLog("需要改写PWD",strlen("需要改写PWD"), STR_INFO);
        //写密码
        U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 3,  NewPwd.U8Buffer);
        if(U8Value == 1)
        {//解EEPROM失败
            WriteSystemLog("解EEPROM失败",strlen("解EEPROM失败"), STR_INFO);
            return ERROR; 
        }
        else if(U8Value != 0)
        {//写失败
            WriteSystemLog("写失败",strlen("写失败"), STR_INFO);
            return ERROR; 
        }    
    }

    if(NewSerl.U32Value != Serl.U32Value)
    {
        WriteSystemLog("需要改写序列码",strlen("需要改写序列码"), STR_INFO);
        //写序列码
        U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 2,  NewSerl.U8Buffer);
        if(U8Value == 1)
        {//解EEPROM失败
            WriteSystemLog("解EEPROM失败",strlen("解EEPROM失败"), STR_INFO);
            return ERROR; 
        }
        else if(U8Value != 0)
        {//写失败
            WriteSystemLog("写失败",strlen("写失败"), STR_INFO);
            return ERROR; 
        }    
    }
    
    AllReset();
	/*发送全局复位指令*/
	AllDisCharge();	

    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("重读序列码失败",strlen("重读序列码失败"), STR_INFO);
		return 0;
    }
    WriteSystemLog("重读序列码成功:",strlen("重读序列码成功:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Serl.U32Value != NewSerl.U32Value)
    {
        WriteSystemLog("序列码不一致",strlen("序列码不一致"), STR_INFO);
        return 0;
    }
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("重读PWD失败",strlen("重读PWD失败"), STR_INFO);
		return 0;
    } 
    WriteSystemLog("重读PWD成功:",strlen("重读PWD成功:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Pwd.U32Value != NewPwd.U32Value)
    {
        WriteSystemLog("PWD不一致",strlen("PWD不一致"), STR_INFO);
        return 0;
    }
    WriteSystemLog("内码重写成功",strlen("内码重写成功"), STR_INFO);
    
    Op_Data.RegDetInfo.Serl.U32Value = Serl.U32Value;//用于界面显示
    Op_Data.RegDetInfo.Pwd.U32Value = Pwd.U32Value;
    return 1;
}
/**
  *************************************************************************************
  * @brief  获取雷管返回时的电流波形
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月03日
  * @note   
  *************************************************************************************  
  */
void GetDetAckCurrentWave(void)
{
	u16 i;
	
	if(Det.RegCount == 0)
	{
		XA3_DetCheckInNotAck();//没有注册雷管时，发送查漏命令
	}
	else
	{		
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				XA3_ReadStatusNotAck(Det.Info[i].Serl.U32Value);	
				break;
			}
		}
	}

	GetDetWaveADCValue();
}

/**
  *************************************************************************************
  * @brief  判断总线是否有未注册雷管
  * @param  无
  * @retval 0表示没有，1表示有
  * @author ZH
  * @date   2018年3月25日
  * @note   查漏,连续5次发送点名命令都有数据返回则说明有未注册雷管
  *************************************************************************************  
  */
u8 HaveUnregisteredDetBySimpleSignal(void)
{
	u8 i;
	
	/*查漏,连续5次发送点名命令都有数据返回则说明有未注册雷管*/
	for(i=0; i < 5; i++)
	{
		if(ERROR == XA3_DetCheckInBySimpleSignal())
		{
			return 0;  //没有检测到未注册雷管
		}
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  起爆雷管
  * @param  无
  * @retval 0表示没有，1表示有
  * @author ZH
  * @date   2018年3月25日
  * @note   发送起爆信号前，发送准备起爆指令
  *************************************************************************************  
  */
ErrorStatus AllDetStartBlast(void)
{
    u16 Voltage;
    ErrorStatus Result = SUCCESS;
    
    Voltage = GetBusVoltage(10, NULL);
    if(Voltage < 5000)
    {
        u8 Buffer[20];
        
        sprintf((char*)Buffer, "准备起爆前异常:%dmV", Voltage);
        WriteSystemLog(Buffer, 0, NEW_STR_INFO);
        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)//从机时，继续发送
        {
            return ERROR;
        }
        Result = ERROR;
    }

	XA3_ExpenAllByBlastSignalPin();
	XA3_ExpenAllByBlastSignalPin();
	XA3_ExpenAllByBlastSignalPin();
	System168MDelay1ms(10);
    
    Voltage = GetBusVoltage(10, NULL);
    if(Voltage < 5000)
    {
        u8 Buffer[20];
        
        sprintf((char*)Buffer, "起爆前异常:%dmV", Voltage);
        WriteSystemLog(Buffer, 0, NEW_STR_INFO);
        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)//从机时，继续发送
        {
            return ERROR;
        }
        Result = ERROR;
    }
    if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && Op_Data.DelaySampleCfg && Op_Data.DelaySampleFlag)
    {
        XA3_DetBlastOne();
    }
    else
    {
        XA3_DetBlast();    
    }
    return Result;
}
/**
  *************************************************************************************
  * @brief  起爆雷管, 通过新建高优先级任务方式发送
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2020年3月17日
  * @note   
  *************************************************************************************  
  */
ErrorStatus CreateStartBlastTask(void)
{
    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)100);
    while(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
    {//等待发送结束
        OSTimeDly(1);
    }
    
    return (ErrorStatus)AppTaskResult.DetOperate;
}

