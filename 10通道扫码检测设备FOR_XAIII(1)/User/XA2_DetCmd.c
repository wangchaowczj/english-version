#include "includes.h"

static const u8 XA2_RetryCount = 2;//通讯失败时重试

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
void XA2_DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(300); //起始位
	for(i=0; i<Bytes; i++)
	{
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
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(2200);//因为采用TIM_SetAutoreload方式发送，需要多加载一次，默认等待接收数据
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
ErrorStatus XA2_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes)
{
	XA2_DetCmdEncode(Cmd, Bytes);
		
	return XA2_DetSendAndRec(ErrorRetry, RecBytes) ;    
}

/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}


/**
  *************************************************************************************
  * @brief  查漏指令，检测雷管是否在线
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_DetCheckInNotAck(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(0, 0) ;
}

/**
  *************************************************************************************
  * @brief  全局软件复位
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_ClearAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CLEAR_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(0, 0) ;
}
/**
  *************************************************************************************
  * @brief  全局硬件复位
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_ResetAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_RESET_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(0, 0) ;
}
/**
  *************************************************************************************
  * @brief  允许全局充电
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_ChargeAllEnable(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_EN_CHARGE_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(0, 0) ;
}
/**
  *************************************************************************************
  * @brief  全局准备起爆命令
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_ExpenAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_EXPEN_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(XA2_RetryCount, 0) ;
}

/**
  *************************************************************************************
  * @brief  读EEPROM命令
  * @param  Addr 地址
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   不能读取密码数据（密码只能由密码读取指令读取)
  *************************************************************************************  
  */
ErrorStatus XA2_ReadEEPROM(u8 Addr, u8* Buffer)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
    
	Cmd[Count++] = XA2_SERLRD ;
	Cmd[Count++] = Addr&0x07 ;
    Cmd[Count++] = 0x00 ;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	Result = XA2_DetSendAndRec(XA2_RetryCount, 5) ;
    if ((Result != ERROR) && (Buffer != NULL))
    {
        memcpy(Buffer, (u8*)&DetProtocol.RecData.Data[1], 4);
    }
	return Result;
}
/**
  *************************************************************************************
  * @brief  读芯片序列码
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   
  *************************************************************************************  
  */
ErrorStatus XA2_ReadSerl(u32* Serl)
{
    ErrorStatus Result;
    u8 Buffer[4];
    
    Result = XA2_ReadEEPROM(2, Buffer);
    if ((Result != ERROR) && (Serl != NULL))
    {
        *Serl = Buffer[0]+ (Buffer[1]<<8)+ ((u32)Buffer[2]<<16)+ ((u32)Buffer[3]<<24);
    }
	return Result;
}
/**
  *************************************************************************************
  * @brief  写EERPOM命令
  * @param  Addr 地址
  * @param  Buffer 要写入的数据
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   数据编程指令必须在“写EE解锁2”发送后才允许，否则无效。序列码编程指令每次写入4个字节数据
  *************************************************************************************  
  */
ErrorStatus XA2_WriteEEPROM(u8 Addr, u8* Buffer)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
    
	Cmd[Count++] = XA2_WR_EEPROM ;
	Cmd[Count++] = Addr&0x07 ;
    for(i=0; i<4; i++)
    {
        Cmd[Count++] = Buffer[i] ;
    }   
	
	XA2_DetCmdEncode(Cmd, Count);
		
	Result = XA2_DetSendAndRec(0, 1) ;
    OSTimeDly(50);
    return Result;
}
/**
  *************************************************************************************
  * @brief  读密码命令
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   每次上电完成后仅允许读取一次，要再次读取需重新上电复位或者全局硬件复位后等待EEPROM数据读取完成后发送
  *************************************************************************************  
  */
ErrorStatus XA2_ReadPwd(u32 Serl, u32* Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
    
	Cmd[Count++] = XA2_RD_PWD ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
    Result = XA2_DetSendAndRec(XA2_RetryCount, 5) ;
    
    if ((Result != ERROR) && (Pwd != NULL))
    {
        *Pwd = DetProtocol.RecData.Data[1]+ (DetProtocol.RecData.Data[2]<<8)+ ((u32)DetProtocol.RecData.Data[3]<<16)+ ((u32)DetProtocol.RecData.Data[4]<<24);
    }
    
	return Result;
}
/**
  *************************************************************************************
  * @brief  屏蔽命令
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_Mask(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_MASK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  解密命令
  * @param  Serl 序列码
  * @retval 0-已经解密;1-//密码失败;0xFF-通讯异常
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
u8 XA2_PwdUnlock(u32 Serl, u32 Pwd)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_PWD_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;

	Cmd[Count++] = Pwd&0xFF;
    Cmd[Count++] = (Pwd>>8)&0xFF;
    Cmd[Count++] = (Pwd>>16)&0xFF;
    Cmd[Count++] = (Pwd>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
	Result = XA2_DetSendAndRec(XA2_RetryCount, 1) ;	
    
    if(Result != ERROR)
    {
        if(DetProtocol.RecData.Data[0]&0x80)
        {
            return 0;//已经解密
        }
        return 1;//密码失败
    }
    
	return 0xFF;//通讯异常
}
/**
  *************************************************************************************
  * @brief  写固定延期解锁命令
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   该使能在写入固定时间一次后自动失效
  *************************************************************************************  
  */
ErrorStatus XA2_WriteFixDelayUnlock(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_WR_FIX_DELAY_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  写数据解锁解锁
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note  “写EE解锁指令2”使能在每次写入4个字节数据完成后失效，如果需要写其他数据，必须重新发送“写EE解锁指令2”
  *************************************************************************************  
  */
ErrorStatus XA2_WriteDataUnlock(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_WR_EE_DATA_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  固定延期时间编程
  * @param  Serl 序列码
  * @param  FixDelay 固定延期时间
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   固定时间编程指令必须在“写EE解锁1”发送后才允许，否则无效， 该使能在写入固定时间一次后自动失效
  *************************************************************************************  
  */
ErrorStatus XA2_WriteFixDelay(u32 Serl, u16 FixDelay)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_WR_FIX_DELAY ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	Cmd[Count++] = FixDelay&0xFF;
    Cmd[Count++] = (FixDelay>>8)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
    
	Result = XA2_DetSendAndRec(0, 1) ;
    OSTimeDly(50);
	return Result;
}
/**
  *************************************************************************************
  * @brief  单个硬件复位
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_Reset(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_RESET ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  全局时钟校准
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_CheckOscAll(u8 Max, u8 Min)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CHECKOSC_ALL ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = 0x00;
    Cmd[Count++] = Max&0x7F;
    Cmd[Count++] = Min&0x7F;
	
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(XA2_RetryCount, 0) ;
}
/**
  *************************************************************************************
  * @brief  写延期时间
  * @param  Serl 序列码
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_WriteTime(u32 Serl, u32 DelayClock)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_WR_TIME ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
    Cmd[Count++] = DelayClock&0xFF;
    Cmd[Count++] = (DelayClock>>8)&0xFF;
    Cmd[Count++] = (DelayClock>>16)&0xFF;
    
	XA2_DetCmdEncode(Cmd, Count);
		
	return XA2_DetSendAndRec(XA2_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  延期时间回读
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_ReadTimeClock(u32 Serl, u32* TimeClk, u16* FixDelay, u8* TimeClkFlag)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_RD_TIME ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA2_DetCmdEncode(Cmd, Count);
		
	Result = XA2_DetSendAndRec(XA2_RetryCount, 6) ;
    
    if(Result != ERROR)
    {   
        u32 U32Value;
        
        if(TimeClk != NULL)
        {
            U32Value = DetProtocol.RecData.Data[3] | ((u32)DetProtocol.RecData.Data[4]<<8) | ((u32)DetProtocol.RecData.Data[5]<<16);
            U32Value &= 0x3FFFFF; 
            *TimeClk = U32Value; 
        }
        if(FixDelay != NULL)
        {
            U32Value = DetProtocol.RecData.Data[1] | ((u16)DetProtocol.RecData.Data[2]<<8);
            U32Value &= 0x7FFF;
            *FixDelay = U32Value; 
        }
        if(TimeClkFlag != NULL)
        {
            *TimeClkFlag = DetProtocol.RecData.Data[5]&0x80; 
        }
    }
    return Result;
}
/**
  *************************************************************************************
  * @brief  状态回读
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_ReadStatus(u32 Serl, u16* Status)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_RD_STATUS ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA2_DetCmdEncode(Cmd, Count);
	Result = XA2_DetSendAndRec(XA2_RetryCount, 2) ;
    
    if((Result != ERROR) && (Status != NULL))
    {
        *Status = (DetProtocol.RecData.Data[0]&0xC0) | ((u16)DetProtocol.RecData.Data[1]<<8);
    }
    return Result;
}
/**
  *************************************************************************************
  * @brief  状态回读
  * @param  Serl 序列码
  * @retval 结果
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_ReadStatusNotAck(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_RD_STATUS ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA2_DetCmdEncode(Cmd, Count);
	XA2_DetSendAndRec(0, 0) ;
}

/**
  *************************************************************************************
  * @brief  状态回读
  * @param  组号(序列码低5位)
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   无
  *************************************************************************************  
  */
void XA2_Bit(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_BIT ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(XA2_RetryCount, 0) ;
}
/**
  *************************************************************************************
  * @brief  分组充电命令
  * @param  组号(序列码低5位)
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   需在解密状态下进行，否则该指令无效
  *************************************************************************************  
  */
void XA2_Charge(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CHARGE ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(XA2_RetryCount, 0) ;
}
/**
  *************************************************************************************
  * @brief  全局充电命令
  * @param  组号(序列码低5位)
  * @retval 无
  * @author ZH
  * @date   2019年1月22日
  * @note   全局充电指令，必须先发送全局充电允许指令后再发送
  *************************************************************************************  
  */
void XA2_ChargeAll(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA2_CHARGE_ALL ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA2_DetCmdEncode(Cmd, Count);
		
	XA2_DetSendAndRec(XA2_RetryCount, 0) ;
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
  * @note   状态回读后判断状态位
  *************************************************************************************  
  */
ErrorStatus XA2_GetDetStatus(u16 DetStatus, XA2_DET_STATUS_BIT StatusBit)
{
	switch(StatusBit)
	{
		case XA2_DELAY_ZERO_STATUS:
			if((DetStatus&0x40)  == 0x00)
			{
				return SUCCESS;//已经写了延期
			}		
			break;
		case XA2_PWD_UNLOCK_STATUS:
			if(DetStatus&0x80)
			{
				return SUCCESS;//已经解密
			}		
			break;
		case XA2_EXPEN_STATUS:
			if(DetStatus&0x0100)
			{
				return SUCCESS;//已经接收到准备起爆指令
			}		
			break;
		case XA2_CAP_STATUS:
			if((DetStatus&0x0200) == 0x00)
			{
				return SUCCESS;//起爆电容焊接正常
			}		
			break;
		case XA2_BIT_STATUS:
			if((DetStatus&0x0400) == 0x00)
			{
				return SUCCESS;//BIT测试正常
			}		
			break;
		case XA2_CHARGE_STATUS:
			if(DetStatus&0x0800)
			{
				return SUCCESS;//已经接收到充电命令
			}		
			break;			
		case XA2_FIX_DELAY_UNLOCK_STATUS:
			if(DetStatus&0x1000)
			{
				return SUCCESS;//写固定延期已经解锁
			}		
			break;
		case XA2_EEPROM_UNLOCK_STATUS:
			if(DetStatus&0x2000)
			{
				return SUCCESS;//写EEPROM已经解锁
			}		
			break;
		case XA2_TIME_STATUS:
			if(DetStatus&0x4000)
			{
				return SUCCESS;//芯片延期时间已经设定
			}		
			break;
		case XA2_BIT_OUT_STATUS:
			if((DetStatus&0x8000) == 0x00)
			{
				return SUCCESS;//BIT测试比较器输出状态正常(已经充电)
			}		
			break;			
		default:
			break;
	}

	return ERROR;
}



/**
  *************************************************************************************
  * @brief  自定义起爆信号
  * @param  Ptr 待发送的指令
  * @param  Bytes 待发送的指令字节数
  * @retval 无
  * @author ZH
  * @date   2019年1月28日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_CustomDetBlast(u16 *Ptr, u8 Count)
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
    return XA2_DetSendAndRec(0, 0) ; 
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
void XA2_DetFixedPulseEncode(u16 Count, u32 HihtHalf, u32 LowHalf)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = HihtHalf; //不能超过320uS,否则芯片极性判断会出错
	DetProtocol.SendBuffer.Timer[1] = LowHalf;
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
void XA2_DetFixedPulse(u16 Count, u32 HihtHalf, u32 LowHalf)
{
	XA2_DetFixedPulseEncode(Count, HihtHalf, LowHalf);
    XA2_DetSendAndRec(0, 0) ; 
}

