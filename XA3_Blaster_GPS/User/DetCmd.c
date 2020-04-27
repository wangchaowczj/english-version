#include "includes.h"

/**
  *************************************************************************************
  * @brief  将待发送的指令字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; //起始位
	for(i=0; i<Bytes; i++)
	{
		if(DetProtocol.SendCmd.Count < DET_SEND_CMD_MAX)
		{
			DetProtocol.SendCmd.Buffer[DetProtocol.SendCmd.Count++] = Ptr[i];
		}		
		Verify = 0;
		Mask = 1;
		for(j=0; j<8; j++)
		{
			if(Ptr[i]&Mask)
			{//1
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;
				Verify++;//1的位数
			}
			else
			{//0
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
			}
	        Mask <<= 1 ;			
		}
		if(Verify&0x01)
		{//发送命令时采用偶校验
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;			
		}
		else
		{
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
		}
	}
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T800US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
}

/**
  *************************************************************************************
  * @brief  将待发送的指令字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   通过起爆信号那个管脚发出去
  *************************************************************************************  
  */
void DetCmdEncodeForBlastSignalPin(u8 *Ptr, u8 Bytes)
{
	DetCmdEncode(Ptr, Bytes);
	DetProtocol.Mode = DET_CMD_BLAST;
}

/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
    
	DetProtocol.Hook = HOOK_IDLE;
	
	return DetSendAndRec(2, 1, 0) ;
}
/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   只要RISE接收到信号就认为有雷管在线,用于多发雷管时
  *************************************************************************************  
  */
ErrorStatus DetCheckInBySimpleSignal(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result; 
	
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
	DetProtocol.Hook = HOOK_CHECK_IN;	
	Result = DetSendAndRec(2, 1, 0) ;
	DetProtocol.Hook = HOOK_IDLE;
	
	return Result;
}
/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月03日
  * @note   只发送，不接收
  *************************************************************************************  
  */
void DetCheckInNotAck(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}
/**
  *************************************************************************************
  * @brief  屏蔽雷管指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetMask(u8 Ser1, u8 Ser2, u8 Ser3)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = MASK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;
}
/**
  *************************************************************************************
  * @brief  写EEPROM指令
  * @param  Ptr 序列码1、序列码2、序列码3、密码1、密码2、工厂编号、工厂批号、年号
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   芯片接收到序列码编程指令后到完成所有数据写入EEPROM中
  *         总共需要等待6146个内部时钟周期
  *         在这段时间内不允许进行其他操作，否则数据编程可能出错。
  *************************************************************************************  
  */
ErrorStatus DetWrEEPROM(u8* Ptr)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = WREEPROM ;
	for(i=0; i<8; i++)
	{
		Cmd[Count++] = *(Ptr+i) ;//序列码1、序列码2、序列码3、密码1、密码2、工厂编号、生产批号、年号
	}
	
	DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = DetSendAndRec(0, 1, 0);
		OSTimeDlyHMSM(0,0,0,150);
		if(Result != ERROR)
		{
			break;
		}
	}
	return Result;
}

/**
  *************************************************************************************
  * @brief  解密指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3 
  * @param  Pwd1 密码1
  * @param  Pwd2 密码2  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   解密后才能发充电
  *************************************************************************************  
  */
ErrorStatus DetUnLockPwd(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	
	Cmd[Count++] = PWD_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Pwd1 ;
	Cmd[Count++] = Pwd2 ;
	
	DetCmdEncode(Cmd, Count);
	return DetSendAndRec(2, 1, 0);
}
/**
  *************************************************************************************
  * @brief  解密指令,错误时不重试
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3 
  * @param  Pwd1 密码1
  * @param  Pwd2 密码2  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月14日
  * @note   解密后才能发充电
  *************************************************************************************  
  */
ErrorStatus DetUnLockPwdNotRetry(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	
	Cmd[Count++] = PWD_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Pwd1 ;
	Cmd[Count++] = Pwd2 ;
	
	DetCmdEncode(Cmd, Count);
	return DetSendAndRec(0, 1, 0);
}
/**
  *************************************************************************************
  * @brief  EEPROM解锁指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetUnLockEEPROM(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EE_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;
}

/**
  *************************************************************************************
  * @brief  固定分段编程指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3
  * @param  Time 分段数  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   芯片接收到指令后到完成所有数据写入EEPROM中
  *         总共需要等待1539个内部时钟周期
  *         在这段时间内不允许进行其他操作，否则数据编程可能出错。
  *************************************************************************************  
  */
ErrorStatus DetFixed(u8 Ser1, u8 Ser2, u8 Ser3, u16 Time)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = FIXED ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Time&0xFF ;
	Cmd[Count++] = (Time>>8)&0x7F ;
	
	DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = DetSendAndRec(0, 1, 0);
		OSTimeDlyHMSM(0,0,0,40);
		if(Result != ERROR)
		{
			break;
		}
	}

	return Result;
}

/**
  *************************************************************************************
  * @brief  时钟校准指令
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   芯片接收完时钟校准指令后，直接进入等待标准脉冲状态
  *************************************************************************************  
  */
void DetCheckOSC(u8 Max, u8 Min)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKOSC ;
	Cmd[Count++] = 0x00 ;
	Cmd[Count++] = Max ;
	Cmd[Count++] = Min&0x7F ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}
/**
  *************************************************************************************
  * @brief  复位指令
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetClrAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CLRALL ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}

/**
  *************************************************************************************
  * @brief  回读序列码
  * @param  无
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus DetSerlRd(void)
{	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = SERLRD ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 7, 0) ;	
}
/**
  *************************************************************************************
  * @brief  BIT测试
  * @param  Ser1Group 序列码1组
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetBit(u8 Ser1Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BIT ;
	Cmd[Count++] = Ser1Group&0x1F ;
	
	DetCmdEncode(Cmd, Count);
	
	DetSendAndRec(0, 0, 0);
}

/**
  *************************************************************************************
  * @brief  回读延期时间
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetTimeRd(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 6, 0) ;
}
/**
  *************************************************************************************
  * @brief  回读延期时间
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetTimeRdForFreqMeasure(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 6, 1) ;
}
/**
  *************************************************************************************
  * @brief  写延期时间
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3 
  * @param  Delay 延期(对应雷管延期时钟个数)   
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetTimeWr(u8 Ser1, u8 Ser2, u8 Ser3, u32 Delay)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMEWR;
	Cmd[Count++] = Ser1;
	Cmd[Count++] = Ser2;
	Cmd[Count++] = Ser3;
	Cmd[Count++] = Delay&0xFF;
	Cmd[Count++] = (Delay>>8)&0xFF;
	Cmd[Count++] = (Delay>>16)&0x3F;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;   
}

/**
  *************************************************************************************
  * @brief  充电指令
  * @param  Ser1Group 序列码1组
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetCharge(u8 Ser1Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHARGE ;
	Cmd[Count++] = Ser1Group&0x1F ;
	
	DetCmdEncode(Cmd, Count);
	
	DetSendAndRec(0, 0, 0);
}
/**
  *************************************************************************************
  * @brief  准备起爆指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3   
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetExpen(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EXPEN;
	Cmd[Count++] = 0x00;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;  
}
/**
  *************************************************************************************
  * @brief  准备起爆指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3   
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetExpenByBlastSignalPin(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EXPEN;
	Cmd[Count++] = 0x00;
	
	DetCmdEncodeForBlastSignalPin(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;  
}
/**
  *************************************************************************************
  * @brief  回读状态
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetStatusRd(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = STATUSRD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 2, 0) ;
}
/**
  *************************************************************************************
  * @brief  回读状态
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @param  Ser3 序列码3  
  * @retval 无
  * @author ZH
  * @date   2016年11月03日
  * @note   只发送，不接收
  *************************************************************************************  
  */
void DetStatusRdNotAck(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = STATUSRD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}
/**
  *************************************************************************************
  * @brief  判断芯片状态是否正常
  * @param  Ser1 芯片状态
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus GetDetStatus(DET_STATUS_BIT StatusBit)
{
	switch(StatusBit)
	{
		case EXPEN_STATUS:
			if(DetProtocol.RecData.Data[1]&0x01)
			{
				return SUCCESS;
			}		
			break;
		case CAP_STATUS:
			if((DetProtocol.RecData.Data[1]&0x02) == 0x00)
			{
				return SUCCESS;
			}		
			break;
		case BIT_STATUS:
			if((DetProtocol.RecData.Data[1]&0x04) == 0x00)
			{
				return SUCCESS;
			}		
			break;
		case CHARGE_STATUS:
			if(DetProtocol.RecData.Data[1]&0x08)
			{
				return SUCCESS;
			}		
			break;			
		case DISCHARGE_STATUS:
			if(DetProtocol.RecData.Data[1]&0x10)
			{
				return SUCCESS;
			}		
			break;
		case TIME_STATUS:
			if(DetProtocol.RecData.Data[1]&0x40)
			{
				return SUCCESS;
			}		
			break;
		case BIT_OUT_STATUS:
			if((DetProtocol.RecData.Data[1]&0x80) == 0x00)
			{
				return SUCCESS;
			}		
			break;			
		default:
			break;
	}

	return ERROR;
}
/**
  *************************************************************************************
  * @brief  将待发送的指令字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetOneBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //低
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;	//高
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //低
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	//高，下降沿触发起爆
    DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
}
/**************************************************/
/*函数功能：发送起爆信号,只发1次		  		  */
/*输入参数：无								  	  */
/*返回值：  无									  */
/**************************************************/
void DetBlastOne(void)
{
	DetOneBlastCmdEncode();
    DetSendAndRec(0, 0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  将待发送的指令字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //低
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;	//高
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //低
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	//高，下降沿触发起爆

	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
}
/**************************************************/
/*函数功能：发送起爆信号		  				  */
/*输入参数：无								  	  */
/*返回值：  无									  */
/**************************************************/
void DetBlast(void)
{
	DetBlastCmdEncode();
    DetSendAndRec(0, 0, 0) ; 
}

/**
  *************************************************************************************
  * @brief  将待发送的校准脉冲字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //不能超过320uS,否则芯片极性判断会出错
	DetProtocol.SendBuffer.Timer[1] = TNUS(900);
}
/**
  *************************************************************************************
  * @brief  将待发送的校准脉冲字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetSingleCapFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //不能超过320uS,否则芯片极性判断会出错
	DetProtocol.SendBuffer.Timer[1] = TNUS(904.5);
}
/**
  *************************************************************************************
  * @brief  将待发送的校准脉冲字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetDoubleCapFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //不能超过320uS,否则芯片极性判断会出错
	DetProtocol.SendBuffer.Timer[1] = TNUS(901.5);
}
/**
  *************************************************************************************
  * @brief  发送校准脉冲
  * @param  无  
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetFixedPulse(u16 Count)
{
	DetFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  发送校准脉冲(针对单电容芯片进行了修正)
  * @param  无  
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetSingleCapFixedPulse(u16 Count)
{
	DetSingleCapFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  发送校准脉冲(针对双电容芯片进行了修正)
  * @param  无  
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
void DetDoubleCapFixedPulse(u16 Count)
{
	DetDoubleCapFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
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


