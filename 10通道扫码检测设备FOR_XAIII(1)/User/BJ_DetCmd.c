#include "includes.h"

static const u8 BJ_RetryCount = 2;//通讯失败时重试

/**
  *************************************************************************************
  * @brief  将待发送的指令字节编码成位
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数 
  * @retval 无
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
void BJ_DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; //起始位
    
    for(i=0;i<9;i++)
	{
		DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;
		DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;	
	}

	for(i=0; i<Bytes; i++)
	{
		Verify = 0;
		Mask = 1;
		for(j=0; j<8; j++)
		{
			if(Ptr[i]&Mask)
			{//1
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
				Verify++;//1的位数
			}
			else
			{//0
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;			
			}
	        Mask <<= 1 ;			
		}
		if(Verify&0x01)
		{//发送命令时采用偶校验
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
		}
		else
		{
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;			
		}
	}
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T400US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
}

/**
  *************************************************************************************
  * @brief  自定义指令
  * @param  Cmd 要发送的命令
  * @param  Bytes 要发送的命令长度
  * @param  ErrorRetry 错误是要重试的次数
  * @param  RecBytes 要接收的数据长度（同步头算1个字节）  
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes)
{
	BJ_DetCmdEncode(Cmd, Bytes);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;    
}
/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  复位指令
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
void BJ_DetClrAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CLRALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	BJ_DetSendAndRec(0, 0) ;
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
ErrorStatus BJ_DetSerlRd(void)
{	
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 Buffer[6];
	u8 i;
	u8 isExist = 0;
	
	Cmd[Count++] = BJ_SERLRD ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	for(i=0; i<5;i++)
	{
		BJ_DetCmdEncode(Cmd, Count);
		if(ERROR != BJ_DetSendAndRec(BJ_RetryCount, 7))
		{
			if(isExist)
			{
				if(memcmp(Buffer, (char*)&DetProtocol.RecData.Data[1], 6) == 0)
				{
					return SUCCESS;
				}
			}
			isExist = 1;
			memcpy(Buffer, (char*)&DetProtocol.RecData.Data[1], 6);
		}		
	}

	return ERROR;	
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
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetMask(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_MASK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  写EEPROM指令
  * @param  Ptr Serl1	Serl2	Code1	factory	Group1	Group2	Year
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片接收到序列码编程指令后到完成所有数据写入EEPROM中
  *         总共需要等待6146个内部时钟周期
  *         在这段时间内不允许进行其他操作，否则数据编程可能出错。
  *************************************************************************************  
  */
ErrorStatus BJ_DetWrEEPROM(u8* Ptr)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = BJ_WREEPROM ;
	for(i=0; i<7; i++)
	{
		Cmd[Count++] = *(Ptr+i) ;//序列码1、序列码2、密码、 工厂编号、生产批号1、生产批号2、年号
	}
	
	BJ_DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = BJ_DetSendAndRec(0, 1);
		OSTimeDlyHMSM(0,0,0,51);
		if(Result != ERROR)
		{
			break;
		}
	}
	return Result;
}
/**
  *************************************************************************************
  * @brief  BIT测试
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2
  * @retval 无
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetBit(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;  
	
	Cmd[Count++] = BJ_BIT ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
			
	return BJ_DetSendAndRec(0, 0);
}
/**
  *************************************************************************************
  * @brief  回读延期时间
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2 
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetTimeRd(u8 Ser1, u8 Ser2)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 Buffer[6];
	u8 i;
	u8 isExist = 0;
	
	Cmd[Count++] = BJ_TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	
	for(i=0; i<5;i++)
	{
		BJ_DetCmdEncode(Cmd, Count);
		if(ERROR != BJ_DetSendAndRec(BJ_RetryCount, 5))
		{
			if(isExist)
			{
				if(memcmp(Buffer, (char*)&DetProtocol.RecData.Data[1], 6) == 0)
				{
					return SUCCESS;
				}
			}
			isExist = 1;
			memcpy(Buffer, (char*)&DetProtocol.RecData.Data[1], 6);
		}		
	}
		
	return ERROR ;
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
ErrorStatus BJ_DetTimeWr(u8 Ser1, u8 Ser2, u8 Pwd, u32 DelayClk)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_TIMEWR;
	Cmd[Count++] = Ser1;
	Cmd[Count++] = Ser2;
	Cmd[Count++] = Pwd;
    Cmd[Count++] = Cmd[0];//手册说可以是任意编码
	Cmd[Count++] = DelayClk&0xFF;
	Cmd[Count++] = (DelayClk>>8)&0xFF;
	Cmd[Count++] = (DelayClk>>16)&0x3F;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;   
}
/**
  *************************************************************************************
  * @brief  充电指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2 
  * @param  Pwd 密码
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetCharge(u8 Ser1, u8 Ser2, u8 Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CHARGE ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
    Cmd[Count++] = Pwd ;
    Cmd[Count++] = Cmd[1];//手册说可以是任意编码
	
	BJ_DetCmdEncode(Cmd, Count);
	
	return BJ_DetSendAndRec(0, 0);
}
/**
  *************************************************************************************
  * @brief  延期计数器检测
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2 
  * @param  Pwd 密码 
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月12日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetTestDelayCount(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_TEST_CNT ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 5) ;  
}
/**
  *************************************************************************************
  * @brief  准备起爆指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2 
  * @param  Pwd 密码 
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetExpen(u8 Ser1, u8 Ser2, u8 Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_EXPEN ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
    Cmd[Count++] = Pwd ;
    Cmd[Count++] = Cmd[1];//手册说可以是任意编码
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;  
}
/**
  *************************************************************************************
  * @brief  准备起爆指令
  * @param  Ser1 序列码1
  * @param  Ser2 序列码2 
  * @param  Pwd 密码 
  * @retval 结果
  *      @arg SUCCESS 成功
  *      @arg ERROR 接收数据失败
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_DetEraseEEPROM(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_ERASE_EEPROM ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;

	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(0, 1) ;  
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
void BJ_DetBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
}
/**
  *************************************************************************************
  * @brief  发送起爆信号
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年3月10日
  * @note   无
  *************************************************************************************  
  */
void BJ_DetBlast(void)
{
	BJ_DetBlastCmdEncode();
    BJ_DetSendAndRec(0, 0) ; 
}

/**
  *************************************************************************************
  * @brief  自定义起爆信号
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数
  * @param  Test3Monitor 是否监测TEST3信号
  * @retval 无
  * @author ZH
  * @date   2015年8月7日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus BJ_CustomDetBlast(u16 *Ptr, u8 Count)
{
    u8 i;
    
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
    
    for(i=0;i<Count;i++)
    {
        DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = *(Ptr+i);
    }

	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
    return BJ_DetSendAndRec(0, 0) ; 
}

