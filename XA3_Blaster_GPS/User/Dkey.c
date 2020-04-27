#include "includes.h"

static STR_COMM DkeySendBuffer, DkeyTempBuffer;

//==================================================================================================
//| 函数名称 | GetSecondTimeFromYear2000 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 计算当前时间到2000年1月1日0时0分0秒的秒数
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | year  当前年份-2000，要求大于2000年
//|          | month 当前月份
//|          | day   当前日
//|          | hour  当前时
//|          | min   当前分
//|          | sec   当前秒
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | ERROR   输入参数有误
//|          | SUCCESS 计算成功
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-04-08 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static u32 GetSecondTimeFromYear2000(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
	u8 i;
	u32 sec_value = 0;
	const u8 month_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//非润年每月天数

	/*计算当前年的1月1日0时0分0秒到2000年1月1日0时0分0秒的秒数*/
	for(i = 0; i < year; i++)
	{
		if(((0 == ((i+2000)%4)) && (0 != ((i+2000)%100))) || (0 == ((i+2000)%400)))
		{//润年
			sec_value += 366*24*60*60;
		}
		else
		{
			sec_value += 365*24*60*60;
		}		
	}
	
	/*计算当前年的month月1日0时0分0秒到1月1日0时0分0秒的秒数*/
	for(i = 1; i < month; i++)
	{
		if(2 == i)
		{
			if(((0 == ((year+2000)%4)) && (0 != ((year+2000)%100))) || (0 == ((year+2000)%400)))
			{//润年
				sec_value += 86400;//1天，60*60*24
			}			
		}		
		sec_value += 60*60*24*month_day[i];//月		
	}
	
	/*计算当前年月day日0时0分0秒到1日0时0分0秒的秒数*/
	sec_value += (day-1)*24*60*60;
	
	/*计算当前年月day日hour时0分0秒到0时0分0秒的秒数*/
	sec_value += hour*60*60;
	
	/*计算当前年月日hour时min分0秒到hour时0分0秒的秒数*/
	sec_value += min*60;	

	/*计算当前年月日hour时min分sec秒到hour时min分0秒的秒数*/
	sec_value += sec;
	
	return sec_value;
}
//==================================================================================================
//| 函数名称 | GetTimeStringSecondTime 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据输入的2个时间字符串计算时间差
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | old_time 旧时间字符串(时间较早)，格式如"20131020134221"
//|          | new_time 新时间字符串(时间较晚)
//|          | value    保存时间差，单位为秒
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | ERROR   输入参数有误
//|          | SUCCESS 计算成功
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-04-08 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus GetTimeStringSecondTime(u8* old_time, u8* new_time, u32* value)
{
//	const u8 month_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//非润年第月天数
	u16 old_year, new_year;
	u8  old_month, new_month, old_day, new_day, old_hour, new_hour, old_min, new_min, old_sec, new_sec;
	u32 old_sec_value = 0, new_sec_value = 0;
	
	
	old_year = (*old_time-0x30)*1000 + (*(old_time+1)-0x30)*100 + (*(old_time+2)-0x30)*10 + (*(old_time+3)-0x30);
	new_year = (*new_time-0x30)*1000 + (*(new_time+1)-0x30)*100 + (*(new_time+2)-0x30)*10 + (*(new_time+3)-0x30);
	if((old_year < 2000) || (old_year > 2099) || (new_year < 2000) || (new_year > 2099))
	{
		return ERROR;
	}
	
	old_month = (*(old_time+4)-0x30)*10 + (*(old_time+5)-0x30);
	new_month = (*(new_time+4)-0x30)*10 + (*(new_time+5)-0x30);
	if((old_month < 1) || (old_month > 12))
	{
		return ERROR;
	}
	if((new_month < 1) || (new_month > 12))
	{
		return ERROR;
	}
	
	old_day = (*(old_time+6)-0x30)*10 + (*(old_time+7)-0x30);
	new_day = (*(new_time+6)-0x30)*10 + (*(new_time+7)-0x30);
	if((old_day < 1) || (old_day > 31))//只做简单判断，不作润月判断
	{
		return ERROR;
	}
	if((new_day < 1) || (new_day > 31))//只做简单判断，不作润月判断
	{
		return ERROR;
	}	
	
	old_hour = (*(old_time+8)-0x30)*10 + (*(old_time+9)-0x30);
	new_hour = (*(new_time+8)-0x30)*10 + (*(new_time+9)-0x30);
	if((old_hour > 23) || (new_hour > 23))
	{
		return ERROR;
	}
	
	old_min = (*(old_time+10)-0x30)*10 + (*(old_time+11)-0x30);
	new_min = (*(new_time+10)-0x30)*10 + (*(new_time+11)-0x30);	
	if((old_min > 59) || (new_min > 59))
	{
		return ERROR;
	}
	
	old_sec = (*(old_time+12)-0x30)*10 + (*(old_time+13)-0x30);
	new_sec = (*(new_time+12)-0x30)*10 + (*(new_time+13)-0x30);		
	if((old_sec > 59) || (new_sec > 59))
	{
		return ERROR;
	}
	
	old_sec_value = GetSecondTimeFromYear2000(old_year-2000, old_month, old_day, old_hour, old_min, old_sec);
	new_sec_value = GetSecondTimeFromYear2000(new_year-2000, new_month, new_day, new_hour, new_min, new_sec);
	if(new_sec_value < old_sec_value)
	{
		return ERROR;
	}
	*value = (new_sec_value - old_sec_value) ;

	return SUCCESS;		
}

/**
  *************************************************************************************
  * @brief  读密钥的字符串(通用)
  * @param  Cmd  命令
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @param  TimerOut 超时时间，MS为单位 
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyGeneralString(u16 Cmd, u8* StrPtr, u8 MaxBytes, u16 TimerOut)
{
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = Cmd>>8;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = Cmd&0xFF;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);

		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+TimerOut/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if((DkeyTempBuffer.bytenum > 5) && (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(MaxBytes >= (DkeyTempBuffer.bytenum-6))//后边还有1个0x0D,所以要多减1个字节
							{
								if(StrPtr != NULL)
								{
									memset(StrPtr, 0, MaxBytes);
									memcpy(StrPtr, &DkeyTempBuffer.uch_buf[5], DkeyTempBuffer.bytenum-6);								
								}
								return SUCCESS;
							}						
						}			
					}
				}			
			}
			OSTimeDly(2);
		}					
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  密钥常规校验
  * @param  无 
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyGeneralVerify(u32 TimeOut)
{
	u32 c_time;
	
	Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
	HidData.bytenum = 0;
	HidData.status = RECIEVING;
	usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
	
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(HidData.status == RECIEVED)
		{
			if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
			{
				if ((DkeyTempBuffer.bytenum == 7) && (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
				{
					if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
						(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
					{
						return DkeyTempBuffer.uch_buf[5];						
					}			
				}
			}
			return 0x01;
		}
		OSTimeDly(2);
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  读密钥的软件版本
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度   
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeySoftwareVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0100, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥ID
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyMac(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0101, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥硬件版本
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyHWVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0103, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥授权码
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyLicense(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x012E, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥数据上传类型
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyUploadType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x011C, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥数据上传类型
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyDetVerifyType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0116, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥远程论证标识
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyArgumentType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0118, StrPtr, MaxBytes, 200);
}
/**
  *************************************************************************************
  * @brief  读密钥授权码
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyCtrlBit(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0124, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  读密钥授权码
  * @param  StrPtr 数据缓冲区首地址
  * @param  MaxBytes 数据缓冲长度  
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyServerAddr(u32* Ip, u16* Port)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2D;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator(0x0D,1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;
							}							
							*Ip = U32Value;
							
							if(ERROR == GetDecValueFromSeparator(0x0D,2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;
							}							
							*Port = U32Value;
							return SUCCESS;						
						}			
					}
					return ERROR;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  读密钥授信息
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyInfo(void)
{
	ErrorStatus Result;
	u8 Temp[50];
	return SUCCESS;
	//读密钥软件版本
	Result = GetDkeySoftwareVersion(&BlastInfo.DkeyInfo.Version[0],sizeof(BlastInfo.DkeyInfo.Version));
	if(ERROR == Result)
	{
		return ERROR;
	}
	//读密钥ID
	Result = GetDkeyMac(&BlastInfo.DkeyInfo.Mac[0],sizeof(BlastInfo.DkeyInfo.Mac));
	if(ERROR == Result)
	{
		return ERROR;
	}
	//读密钥硬件版本
	Result = GetDkeyHWVersion(&BlastInfo.DkeyInfo.HwVersion[0],sizeof(BlastInfo.DkeyInfo.HwVersion));
	if(ERROR == Result)
	{
		return ERROR;
	}
	
	//读密钥授权ID
	Result = GetDkeyLicense(&BlastInfo.DkeyInfo.License[0],sizeof(BlastInfo.DkeyInfo.License));	
	if(ERROR == Result)
	{
		return ERROR;
	}
	
	//读服务器地址
	Result = GetDkeyServerAddr(&BlastInfo.DkeyInfo.Server.Ip, &BlastInfo.DkeyInfo.Server.Port);
	if(ERROR == Result)
	{
		return ERROR;
	}

	//读密钥数据上传方式
	Result = GetDkeyUploadType(&Temp[0], 2);//2字节是因为，有一个字符串结束符
	if(ERROR == Result)
	{
		return ERROR;
	}
	BlastInfo.DkeyInfo.UploadType = Temp[0];

	//读密钥雷管授权方式
	Result = GetDkeyDetVerifyType(&Temp[0], 2);//2字节是因为，有一个字符串结束符
	if(ERROR == Result)
	{
		return ERROR;
	}					
	BlastInfo.DkeyInfo.DetVerifyType = Temp[0];
	
	//读密钥远程论证标识
	Result = GetDkeyArgumentType(&Temp[0], 2);//2字节是因为，有一个字符串结束符
	if(ERROR == Result)
	{
		return ERROR;
	}
	BlastInfo.DkeyInfo.ArgumentType = Temp[0];
	
	//读密钥数据保留控制字节
	Result = GetDkeyCtrlBit(&Temp[0], 50);
	if(ERROR == Result)
	{
		return ERROR;
	}
	sscanf((char*)Temp,"%lld",&BlastInfo.DkeyInfo.CtrlBit);
	
	if ((BlastInfo.DkeyInfo.Server.Ip != 0) && (BlastInfo.DkeyInfo.Server.Port != 0))
	{
		if ((BlastInfo.DkeyInfo.Server.Ip != device_info.Server.Ip) || 
			(BlastInfo.DkeyInfo.Server.Port != device_info.Server.Port))
		{
			device_info.Server = BlastInfo.DkeyInfo.Server;
			Temp[0] = device_info.Server.Ip>>24;
			Temp[1] = device_info.Server.Ip>>16;
			Temp[2] = device_info.Server.Ip>>8;
			Temp[3] = device_info.Server.Ip;
			Temp[4] = device_info.Server.Port>>8;
			Temp[5] = device_info.Server.Port;
			CreateNewFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&Temp[0], 6);							
		}
	}	
	return SUCCESS;		
}

/**
  *************************************************************************************
  * @brief  用户名密码验证
  * @param  UserName 用户名
  * @param  Password 密码
  * @param  State 用户状态，密码错误时有效:
  *         低4位表示当前可重试次数，高4位表示最大可重试次数(全为1时表示不启用自锁功能)
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过 
  *       @arg @b ‘2’表示密码错误
  *       @arg @b ‘3’该用户已经自锁
  *       @arg @b ‘4’没有找到用户   
  *       @arg @b ‘5’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyUserInfoVerify(u8* UserName, u8* Password, u8* State)
{
	u8 Result = 0;
	u32 U32Value;
	u32 c_time;
	return '1';
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x1F;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r", UserName, Password);
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							if ((U32Value < '1') || (U32Value > '5'))
							{
								return 0x01;
							}
							Result = (u8)U32Value;
							if(ERROR == GetDecValueFromSeparator('\r', 2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							*State = (u8)U32Value;

							return Result;						
						}			
					}
					return 0x01;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  修改用户名密码
  * @param  UserName 用户名
  * @param  Password 密码
  * @param  State 用户状态，密码错误时有效  
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示修改通过 
  *       @arg @b ‘2’表示密码错误
  *       @arg @b ‘3’该用户已经自锁
  *       @arg @b ‘4’没有找到用户   
  *       @arg @b ‘5’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyModifyUserInfo(u8* UserName, u8* OldPassword, u8* NewPassword, u8* State)
{
	u8 Result = 0;
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2F;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r%s\r", UserName, OldPassword, NewPassword);
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							if ((U32Value < '1') || (U32Value > '5'))
							{
								return 0x01;
							}
							Result = (u8)U32Value;
							if(ERROR == GetDecValueFromSeparator('\r', 2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							*State = (u8)U32Value;

							return Result;						
						}			
					}
					return 0x01;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0x00;	
}
/**
  *************************************************************************************
  * @brief  起爆地点经纬度验证
  * @param  Longitude 经度"11620.1092E"
  * @param  Latitude 纬度"3957.7800N"
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过
  *       @arg @b ‘2’表示验证不通过   
  *       @arg @b ‘3’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyBlastAreaVerify(u8* Longitude, u8* Latitude)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x20;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r", Longitude, Latitude);
			
		return DkeyGeneralVerify(500);					
	}
	return 0x00;	
}
/**
  *************************************************************************************
  * @brief  起爆时间验证
  * @param  Time 时间字符串"20150205152100"
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过
  *       @arg @b ‘2’表示验证不通过   
  *       @arg @b ‘3’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyBlastTimeVerify(u8* Time)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x21;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r", Time);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  GPS延时验证
  * @param  Time 时间字符串"20150205152100"
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过
  *       @arg @b ‘2’表示验证不通过   
  *       @arg @b ‘3’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyGpsDelayVerify(u16 GpsDelayHour)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x22;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%u\r", GpsDelayHour);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  雷管内码验证
  * @param  DetInfo 雷管内码信息
  * @param  Bytes 雷管内码信息长度  
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过
  *       @arg @b ‘2’表示验证不通过   
  *       @arg @b ‘3’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyDetInfoVerify(u8* DetInfo, u8 Bytes)
{
	u8 i;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x27;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		
		for(i = 0; i < Bytes; i++)
		{
			DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = *(DetInfo+i);
		}		
		return DkeyGeneralVerify(10000);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  读密钥中的剩余雷管数
  * @param  DetCount 剩余雷管数 
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetCount(u16* DetCount)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x25;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if ((DkeyTempBuffer.bytenum >= 9)|| (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 1;
							}
							*DetCount = (u16)U32Value;
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}
/**
  *************************************************************************************
  * @brief  更新密钥中的剩余雷管数
  * @param  DetNumber 本次使用的数量
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示更新成功
  *       @arg @b ‘2’表示可用雷管数不足   
  *       @arg @b ‘3’密钥接收到的数据不符合要求  
  *       @arg @b ‘4’密钥异常
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyUpdateDetCount(u16 DetNumber)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x26;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%u\r", DetNumber);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  起爆信息验证
  * @param  DetErrIndex 雷管内码验证出现非授权雷管时对应的索引值
  * @retval 结果  
  *       @arg @b 0 所有项验证通过
  *       @arg @b 1 起爆时间非法
  *       @arg @b 2 起爆地点非法
  *       @arg @b 3 GPS延时非法
  *       @arg @b 4 数量超出密钥中剩余范围(数量验证时)
  *       @arg @b 5 有未授权的雷管(雷管内码验证时)
  *       @arg @b 6 有从机通信异常
  *       @arg @b 7 有从机雷管类型错误  
  *       @arg @b 0xFF 其他异常，如通信异常
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyBlastInfoVerify(u16* DetErrIndex)
{
	u8 Temp1[30],Temp2[30];
	RTC_Typedef RTC_Temp;
	u8 Result = 0, SlaveIndex = 0;
	u16 U16Value = 0, i;
	u32 U32Value = 0;
	
	//起爆时间验证
	OSSchedLock();
	RTC_Temp = RTC_Type;
	OSSchedUnlock();
	sprintf((char*)&Temp1[0],"%04d%02d%02d%02d%02d%02d",
							RTC_Temp.year, RTC_Temp.month, RTC_Temp.day, RTC_Temp.hour,
							RTC_Temp.minute, RTC_Temp.second);
							
	Result = DkeyBlastTimeVerify(Temp1);
	if(Result == '2')
	{//验证不通过
		return 1;//起爆时间非法
	}
	else if(Result != '1')
	{//其他异常
		return 0xFF;
	}
	
	//起爆地点经纬度验证
	//经度,格式为"11620.1092,E", 纬度,格式为"3957.7800,N"
	memset(Temp1, 0,sizeof(Temp1));
	memset(Temp2, 0,sizeof(Temp2));
	
	OSSchedLock();
	if((0x00 != Op_Data.Gps.GPS_Longitude[0])&&(0x00 != Op_Data.Gps.GPS_Latitude[0]))
	{//开机GPS定位成功
		memcpy(Temp1, Op_Data.Gps.GPS_Longitude, strlen((char*)Op_Data.Gps.GPS_Longitude));
		memcpy(Temp2, Op_Data.Gps.GPS_Latitude, strlen((char*)Op_Data.Gps.GPS_Latitude));
		OSSchedUnlock();
	}
//	else if(Op_Data.OldGps.GPS_state == GPS_SUCESS)
//	{//使用上一次的GPS信息
//		memcpy(Temp1, Op_Data.OldGps.GPS_Longitude, sizeof(Op_Data.OldGps.GPS_Longitude));
//		memcpy(Temp2, Op_Data.OldGps.GPS_Latitude, sizeof(Op_Data.OldGps.GPS_Latitude));
//		OSSchedUnlock();
//	}
	else
	{
		OSSchedUnlock();
		return 2;//起爆地点非法
	}	
	RemoveCharFromString(Temp1,',',strlen((const char*)Temp1));
	RemoveCharFromString(Temp2,',',strlen((const char*)Temp1));
	Result = DkeyBlastAreaVerify(Temp1, Temp2);
	if(Result == '2')
	{//验证不通过
		return 2;//起爆地点非法
	}
	else if(Result != '1')
	{//其他异常
		return 0xFF;
	}
	
	//GPS延时验证
	sprintf((char*)&Temp1[0],"%04d%02d%02d%02d%02d%02d",
							RTC_Temp.year, RTC_Temp.month, RTC_Temp.day, RTC_Temp.hour,
							RTC_Temp.minute, RTC_Temp.second);	
	if((0x00 != Op_Data.Gps.GPS_Longitude[0])&&(0x00 != Op_Data.Gps.GPS_Latitude[0]))
	{
		sprintf((char*)&Temp2[0], "%04d%02d%02d%02d%02d%02d", Op_Data.Gps.GPS_BJ_year, Op_Data.Gps.GPS_BJmonth,
			Op_Data.Gps.GPS_BJday, Op_Data.Gps.GPS_BJhour, Op_Data.Gps.GPS_BJminute, Op_Data.Gps.GPS_BJsecond);		
	}
	else
	{
		return 3;//GPS延时非法
	}

	if(ERROR == GetTimeStringSecondTime(Temp2, Temp1, &U32Value))
	{//当前时间比GPS时间还早
		return 3;//GPS延时非法
	}
	U32Value /=3600;//把秒转换为小时
	if(U32Value > 0xFFFF)
	{//GPS延时小时数，不能超过65535
		return 3;//GPS延时非法
	}
	Result = DkeyGpsDelayVerify(U32Value);
	if(Result == '2')
	{//验证不通过
		return 3;//GPS延时非法
	}
	else if(Result != '1')
	{//其他异常
		return 0xFF;
	}	
	
	//主从机模式时，主机需要提前读出从机的雷管数量
	if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		GetAllSlaveInfo(1);
		GetSlaveDetNumber();
		if (GetSlaveStateCount(SLAVE_TRUE) != SlaveInfo.Number)
		{
			return 0x06;//
		}
		else if(GetSlaveIndexFromErrorBit(SLAVE_ERROR_BIT_DET_TYPE) != 0xFF)
		{
			return 0x07;
		}
	}
	
	
	if(BlastInfo.DkeyInfo.DetVerifyType == '1')
	{//数量验证
		Result = DkeyReadDetCount(&U16Value);
		if(Result != '1')
		{
			return 0xFF;
		}
		if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			if(U16Value < SlaveInfo.RegCount)
			{
				return 4;//数量超出密钥中剩余范围
			}				
		}
		else
		{
			if(U16Value < Det.RegCount)
			{
				return 4;//数量超出密钥中剩余范围
			}		
		}
	}
	else if(BlastInfo.DkeyInfo.DetVerifyType == '2')
	{//雷管内码验证
		SlaveIndex = 0x00;
		while(SlaveIndex != 0xFF)
		{
			//主从机模式时，边读边验证
			if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				SlaveInfo.Info[SlaveIndex].State = GetSlaveDetInfo(SlaveIndex);
				if(SlaveInfo.Info[SlaveIndex].State == SLAVE_FALSE)
				{
					return 0x06;//从机异常
				}
			}
			
			AllRegDetRankBaseOnDetInfo();
			for(i=0; i<Det.RegCount;i++)
			{
				Temp1[0] = Det.Info[i].Serl_1;
				Temp1[1] = Det.Info[i].Serl_2;
				Temp1[2] = Det.Info[i].Serl_3;
				Temp1[3] = Det.Info[i].Factor;
				Temp1[4] = Det.Info[i].bat;
				Temp1[5] = Det.Info[i].year;
				Result = DkeyDetInfoVerify(&Temp1[0], 6);
				if(Result == '2')
				{//验证不通过
					*DetErrIndex = i;
					if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						SlaveInfo.Info[SlaveIndex].ErrorBit |= SLAVE_ERROR_BIT_DET_ACCREDIT;
					}
					return 5;//有未授权的雷管
				}
				else if(Result != '1')
				{//其他异常
					AllRegDetRankBaseOnHoleInfo();//把所有雷管按孔号从小到大重新排列
					return 0xFF;
				}			
			}
			if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				SlaveIndex++;
				if(SlaveIndex>= SlaveInfo.Number)
				{
					SlaveIndex = 0xFF;
				}
			}
			else
			{
				SlaveIndex = 0xFF;
				AllRegDetRankBaseOnHoleInfo();//把所有雷管按孔号从小到大重新排列			
			}	
		}

	}
		
	return 0x00;//所有项都验证通过
}


/**
  *************************************************************************************
  * @brief  读密钥中的雷管备份数据大小
  * @param  Bytes 数据大小
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示读取成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUpDataBytes(u32* Bytes)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2A;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = '1';
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x0D;
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if ((DkeyTempBuffer.bytenum >= 7)|| (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 1;
							}
							*Bytes = (u16)U32Value;
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  读密钥中的雷管备份数据(只读1包)
  * @param  PackIndex 包号
  * @param  Data 保存数据的首地址
  * @param  CurrentBytes 当前已经读取到的长度  
  * @param  NextBytes 本次读取的数量   
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示更新成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUpData(u16 PackCount, u8* Data, u32 CurrentBytes, u16 NextBytes)
{
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2A;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"2\r%u\r%lu\r%lu\r", PackCount, CurrentBytes, NextBytes);
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//数据打包
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if (DkeyTempBuffer.bytenum == (7+NextBytes))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && 
							(DkeyTempBuffer.uch_buf[4] == 0xFF) && (DkeyTempBuffer.uch_buf[5] == (PackCount>>8)) && 
							(DkeyTempBuffer.uch_buf[6] == (PackCount&0xFF)))
						{
							memcpy(Data, &DkeyTempBuffer.uch_buf[7], NextBytes);
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  读密钥中的雷管备份数据
  * @param  无  
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b 2 密钥中没有备份数据
  *       @arg @b 3 数据类型不合适要求   
  *       @arg @b ‘1’表示读取成功
  *       @arg @b ‘2’表示保存失败 
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUp(void)
{
	u8 Result=0;
	u32 BackUpDataBytes = 0;
	u8 Data[20];
	u16 i;
	
	Result = DkeyReadDetInfoBackUpDataBytes(&BackUpDataBytes);//读密钥中的数据长度
	if(Result == '1')
	{
		if(BackUpDataBytes <= 10)
		{
			return 2;//没有数据
		}
		
		//前10字字节用于表示数据含义
		//Data[0]表示数据格式类型,目前为0
		//其他保留			
		Result = DkeyReadDetInfoBackUpData(0, Data, 0,10);
		if(Result == '1')
		{	
			if ((Data[0] == 0) && 
				(BackUpDataBytes == (16*DET_MAX_SIZE/*每个雷管16字节*/ + 10/*保留的前10字节*/ + DET_MAX_SIZE/*孔内雷管数*/)))
			{//数据格式满足要求 
				memset((char*)&Det.Info[0], 0, sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				
				//读雷管内码
				for(i=0;i< DET_MAX_SIZE;i++)
				{
					Result = DkeyReadDetInfoBackUpData(i, Data, 10+i*16, 16);
					if(Result == '1')
					{
						Det.Info[i].Serl_1 = Data[0];
						Det.Info[i].Serl_2 = Data[1];
						Det.Info[i].Serl_3 = Data[2];
						Det.Info[i].Factor = Data[3];
						Det.Info[i].bat = Data[4];
						Det.Info[i].year = Data[5];
						Det.Info[i].freq = ((u32)Data[6]<<24)+ ((u32)Data[7]<<16) + ((u32)Data[8]<<8) + ((u32)Data[9]&0xFF);
						Det.Info[i].delay = ((u16)Data[10]<<8) + ((u16)Data[11]&0xFF);					
						Det.Info[i].Hole.Name = ((u16)Data[12]<<8) + ((u16)Data[13]&0xFF);	
						Det.Info[i].Hole.Inside = Data[14];
						Det.Info[i].err = Data[15];	
						
						if(ERROR == CheckDetType(Det.Info[i].bat, Det.Info[i].year))
						{
							memset((char*)&Det.Info[0],0,sizeof(Det.Info));
							memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
							Det.ErrorCount = 0;
							Det.RegCount = 0;
							return 3;//类型不符合要求
						}
						if(Det.Info[i].delay > Det.DlyMax)
						{
							Det.Info[i].delay =  Det.DlyMax;
						}
					}
					else
					{
						memset((char*)&Det.Info[0],0,sizeof(Det.Info));
						memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
						Det.ErrorCount = 0;
						Det.RegCount = 0;
						return Result;
					}
				}
				//读每个孔的孔内雷管数
				Result = DkeyReadDetInfoBackUpData(0, &Det.HoleNumber[0], 10+DET_MAX_SIZE*16, DET_MAX_SIZE);
				if(Result != '1')
				{
					memset((char*)&Det.Info[0],0,sizeof(Det.Info));
					memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
					Det.ErrorCount = 0;
					Det.RegCount = 0;
					return Result;				
				}
				
				if(ERROR == SaveAllDetInfo())
				{
					return '2';
				}
				else
				{
					return '1';				
				}
			}
			return 2;//格式不符合要求
		}
	}	
	
	return Result;
}

/**
  *************************************************************************************
  * @brief  写入密钥中雷管备份数据
  * @param  PackIndex 包号
  * @param  Data 数据
  * @param  Bytes 数据长度
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 密钥返回的格式错误
  *       @arg @b ‘1’表示验证通过
  *       @arg @b ‘2’表示验证不通过   
  *       @arg @b ‘3’密钥接收到的数据不符合要求    
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyDetInfoBackUpData(u16 PackIndex, u8* Data, u16 Bytes)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x29;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex>>8;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex&0xFF;//分包包号低字节，不需要分包
		
		if(Bytes > 0)
		{
			memcpy(&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum], Data, Bytes);
			DkeySendBuffer.bytenum += Bytes;	
		}
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  写入密钥中雷管备份数据
  * @param  无
  * @retval 结果  
  *       @arg @b ERROR 失败
  *       @arg @b SUCCESS 成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyDetInfoBackUp(void)
{
	u8 Data[230], Bytes = 10;
	u8 Result = 0;
	u16 PackIndex = 0,i;
	
	//前10字字节用于表示数据含义
	//Data[0]表示数据格式类型,目前为0
	//其他保留
	memset(Data, 0x00,sizeof(Data));
	Result = DkeyDetInfoBackUpData(PackIndex++,Data, Bytes);
	if(Result == '1')
	{
		//保存雷管内码
		Bytes = 0;
		for(i=0;i<DET_MAX_SIZE;i++)
		{			
			Data[Bytes++] = Det.Info[i].Serl_1;
			Data[Bytes++] = Det.Info[i].Serl_2;
			Data[Bytes++] = Det.Info[i].Serl_3;
			Data[Bytes++] = Det.Info[i].Factor;
			Data[Bytes++] = Det.Info[i].bat;
			Data[Bytes++] = Det.Info[i].year;
			Data[Bytes++] = Det.Info[i].freq>>24;
			Data[Bytes++] = Det.Info[i].freq>>16;
			Data[Bytes++] = Det.Info[i].freq>>8;
			Data[Bytes++] = Det.Info[i].freq&0xFF;
			Data[Bytes++] = Det.Info[i].delay>>8;
			Data[Bytes++] = Det.Info[i].delay&0xFF;
			
			Data[Bytes++] = Det.Info[i].Hole.Name>>8;
			Data[Bytes++] = Det.Info[i].Hole.Name;
			Data[Bytes++] = Det.Info[i].Hole.Inside;
			Data[Bytes++] = Det.Info[i].err;
			
			if ((Bytes > 200) || ((i+1) >= DET_MAX_SIZE))
			{
				Result = DkeyDetInfoBackUpData(PackIndex++,Data, Bytes);
				if(Result != '1')
				{
					return ERROR;
				}
				else
				{
					Bytes = 0;
				}
			}
		}
		//保存每个孔的孔内雷管数
		Result = DkeyDetInfoBackUpData(PackIndex++, &Det.HoleNumber[0], DET_MAX_SIZE);
		if(Result != '1')
		{
			return ERROR;
		}		
		Result = DkeyDetInfoBackUpData(0xFFFF,NULL, 0);//结束包
		if(Result == '1')
		{
			return SUCCESS;
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  擦除雷管备份数据
  * @param  DetNumber 本次使用的数量
  * @retval 结果  
  *       @arg @b ERROR 失败
  *       @arg @b SUCCESS 成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyDeleteDetInfoBackUpData(void)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2B;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//分包包号低字节，不需要分包
		
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = '3';
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x0D;
			
		if('1' == DkeyGeneralVerify(1000))
		{
			return SUCCESS;
		}
	}
	return ERROR;
}


/**
  *************************************************************************************
  * @brief  把起爆记录数据发送到数字密钥中
  * @param  PackIndex 包号
  * @param  Data 数据
  * @param  Bytes 数据长度
  * @retval 结果  
  *       @arg @b 0 没有连接
  *       @arg @b 1 读记录出错
  *       @arg @b ‘1’表示更新成功
  *       @arg @b ‘2’表示可用雷管数不足   
  *       @arg @b ‘3’密钥接收到的数据不符合要求  
  *       @arg @b ‘4’密钥异常
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
u8 DkeyBlastRecorderBackUpData(u16 PackIndex, u8* Data, u16 Bytes)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//协议地址
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//命令码高字节
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x14;//命令码低字节
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex>>8;//分包包号高字节，不需要分包
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex&0xFF;//分包包号低字节，不需要分包
		
		if(Bytes > 0)
		{
			memcpy(&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum], Data, Bytes);
			DkeySendBuffer.bytenum += Bytes;		
		}
			
		return DkeyGeneralVerify(2000);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  把起爆记录保存到数字密钥中
  * @param  DetNumber 本次使用的数量
  * @retval 结果  
  *       @arg @b ERROR 失败
  *       @arg @b SUCCESS 成功
  * @author ZH
  * @date   2015年02月03日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyBlastRecorderBackUp(void)
{
	FIL fdst;      /* file objects */
	u8 Buffer[256];	
	UINT bw;
	u16 PackIndex = 0;
	u8 Result = 0;

	FileSystemLock();
	
	sprintf((char*)Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", BLASTER_RECORD_FILE_PATH, BlastInfo.Time.year-2000, 
		BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);

	if(f_open(&fdst, (const char*)Buffer, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		f_lseek(&fdst,0);
		
		while(1)
		{
			if(FR_OK == f_read(&fdst, Buffer, 200, &bw))//每次200字节
			{
				if(bw > 0)
				{
					Result = DkeyBlastRecorderBackUpData(PackIndex++,Buffer,bw);
					if(Result != '1')
					{
						FileSystemUnlock();
						WriteSystemLog("通信失败1!",strlen("通信失败1!"),STR_INFO);
						PackIndex = f_tell(&fdst);
						WriteSystemLog("  ",strlen("  "),STR_INFO);
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						PackIndex = Result;
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						if (USB_SUCESS == usb_into_host())
						{
							WriteSystemLog("当前密钥连接中!",strlen("当前密钥连接中!"),STR_INFO);
						}
						else
						{
							WriteSystemLog("当前密钥没有连接!",strlen("当前密钥没有连接!"),STR_INFO);
						}
						return ERROR;
					}
				}
				if(bw != 200)
				{
					Result = DkeyBlastRecorderBackUpData(0xFFFF,NULL,0);					
					if(Result == '1')
					{
						FileSystemUnlock();
						return SUCCESS;
					}
					else
					{
						FileSystemUnlock();
						WriteSystemLog("通信失败2!",strlen("通信失败2!"),STR_INFO);
						PackIndex = f_tell(&fdst);
						WriteSystemLog("  ",strlen("  "),STR_INFO);						
						PackIndex = Result;
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						if (USB_SUCESS == usb_into_host())
						{
							WriteSystemLog("当前密钥连接中!",strlen("当前密钥连接中!"),STR_INFO);
						}
						else
						{
							WriteSystemLog("当前密钥没有连接!",strlen("当前密钥没有连接!"),STR_INFO);
						}						
						return ERROR;
					}
				}
			}
			else
			{
				FileSystemUnlock();
				WriteSystemLog("读文件失败!",strlen("读文件失败!"),STR_INFO);
				WriteSystemLog(&bw,sizeof(bw),U16_DATA);
				return ERROR;
			}			
		}
	}	
	FileSystemUnlock();
	WriteSystemLog("打开文件失败!",strlen("打开文件失败!"),STR_INFO);
	return ERROR;
}


