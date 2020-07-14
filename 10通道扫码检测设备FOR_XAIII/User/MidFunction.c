#include "includes.h"


//==================================================================================================
//| 函数名称 | U32DataCMP 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 比较指定缓冲区中的数据(32位数据),返回最大相差值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr:需要比较的数据缓冲区首址
//|          | length:需要比较的数据缓冲区长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 缓冲区中数据最大相差值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u32 U32DataCMP(const u32* ptr, u8 length)
{
	u32 max_value, min_value ;
	u8 i ;

	max_value = *ptr; //最大值
	min_value = *ptr; //最小值
	for(i=1;i<length;i++)
	{
		if(*(ptr+i)>max_value)
		{
			max_value = *(ptr+i) ;
		}
		else if(*(ptr+i) < min_value)
		{
			min_value = *(ptr+i) ;
		}
	}
	return (max_value - min_value);	//返回相差值
}

//==================================================================================================
//| 函数名称 | U16DataCMP 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 比较指定缓冲区中的数据(16位数据),返回最大相差值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr:需要比较的数据缓冲区首址
//|          | length:需要比较的数据缓冲区长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 缓冲区中数据最大相差值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
//u16 U16DataCMP(const u16* ptr, u8 length)
//{
//	u16 max_value, min_value ;
//	u8 i ;

//	max_value = *ptr; //最大值
//	min_value = *ptr; //最小值
//	for(i=1;i<length;i++)
//	{
//		if(*(ptr+i)>max_value)
//		{
//			max_value = *(ptr+i) ;
//		}
//		else if(*(ptr+i) < min_value)
//		{
//			min_value = *(ptr+i) ;
//		}
//	}
//	return (max_value - min_value);	//返回相差值
//}

//==================================================================================================
//| 函数名称 | GetU16DataMax 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对一组数据(16位)求最大值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指针，指要求平均值的数据
//|          | lenght数据个数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 最大值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetU16DataMax(const u16* ptr, u8 length)
{
	u16 max_value =0;
	u8 i ;

	max_value = *ptr; //最大值
	for(i=1;i<length;i++)
	{
		if(*(ptr+i)>max_value)
		{
			max_value = *(ptr+i) ;
		}
	}
	return max_value ;	//返回最大值
}

u32 GetU32DataMax(const u32* ptr, u8 length)
{
	u32 max_value =0;
	u8 i ;

	max_value = *ptr; //最大值
	for(i=1;i<length;i++)
	{
		if(*(ptr+i)>max_value)
		{
			max_value = *(ptr+i) ;
		}
	}
	return max_value ;	//返回最大值
}
//==================================================================================================
//| 函数名称 | GetU16DataMin 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对一组数据(16位)求最小值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指针，指要求平均值的数据
//|          | lenght数据个数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 最小值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetU16DataMin(const u16* ptr, u8 length)
{
	u16 min_value = 0 ;
	u8 i ;

	min_value = *ptr; //最小值
	for(i=1;i<length;i++)
	{
		if(*(ptr+i) < min_value)
		{
			min_value = *(ptr+i) ;
		}
	}
	return min_value ;	//返回最小值
}

u32 GetU32DataMin(const u32* ptr, u8 length)
{
	u32 min_value = 0 ;
	u8 i ;

	min_value = *ptr; //最小值
	for(i=1;i<length;i++)
	{
		if(*(ptr+i) < min_value)
		{
			min_value = *(ptr+i) ;
		}
	}
	return min_value ;	//返回最小值
}

//==================================================================================================
//| 函数名称 | GetU16DataAverage 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对数据(16位)求平均值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指针，指要求平均值的数据
//|          | lenght数据个数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 平均值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetU16DataAverage(u16* ptr, u16 lenght)
{
	u16 i;
	u32 sum = 0;
	
	for(i=0; i< lenght; i++)
	{
		sum += *ptr++;
	}
	if(lenght!=0)
	{
		sum /= lenght ;
	}

	return (u16)sum;
}



//==================================================================================================
//| 函数名称 | U16DataFallRank 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 把数据(16位)从大到小按顺序排列
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr_s 指针，指向排列前的数据
//|          | ptr_d 指针，指向排序后的数据
//|          | lenght要排序的数据个数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght)
{
	u16 i,j,temp;
	u8 flag_bit;
	
	if(lenght == 0)
	{
		return ;
	}
	memcpy((u8*)ptr_d, (u8*)ptr_s, lenght*2);
	for(i=0; i < lenght-1; i++)
	{
		flag_bit = 1;
		for(j=0; j < lenght-i-1; j++)
		{
			 if(*(ptr_d+j)<*(ptr_d+j+1))
			 {
				 flag_bit = 0 ;
				 temp = *(ptr_d+j) ;
				 *(ptr_d+j) = *(ptr_d+j+1);
				 *(ptr_d+j+1)= temp ;
			 }
		}
		if(flag_bit)//提前结束排序
		{
			break ;
		}		
	}
}

//==================================================================================================
//| 函数名称 | RemoveCharFromString 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 删除字符串中的指定字符
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr  指针，指向删除前的缓冲区
//|          | char_val 要删除的字符
//|          | lenght    删除前的长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void RemoveCharFromString(u8*ptr, u8 char_val, u16 len)
{
	u16 i;
	u16 index = 0;
	
	for(i=0;i<len;i++)
	{
		if(*(ptr+i) != char_val)
		{
			*(ptr+index++) = *(ptr+i);
		}
	}
	*(ptr+index++) = 0x00;
}

/**
  *************************************************************************************
  * @brief  获取字符串中指定字符的索引
  * @param[in] ptr 字符串
  * @param[in] char_val 指定的字符
  * @param[in] len 字符串长度  
  * @retval  
  * @author ZH
  * @date   2015年7月16日
  * @note   无
  *************************************************************************************  
  */
u16 GetCharIndexFromString(u8*ptr, u8 char_val, u16 len)
{
	u16 i;
	
	for(i=0;i<len;i++)
	{
		if(*(ptr+i) == char_val)
		{
			return i;
		}
	}
	return 0xFFFF;
}

//==================================================================================================
//| 函数名称 | HexCharToValue 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 把单个16进制字符转换为数值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | c 要转换的字符
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 转换后的值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u8 HexCharToValue(u8 c)
{
	if( c >= '0' && c <= '9')
	{
		return (unsigned int) (c - 48);
	}
	else if( c >= 'A' && c <= 'F')
	{
		return (unsigned int) (c - 'A' + 10);
	}
	else if( c >= 'a' && c <= 'f')
	{
		return (unsigned int) (c - 'a' + 10);
	}
	else
	{
		return 0;
	}
}

//==================================================================================================
//| 函数名称 | HexStrToCharStr 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 把16进度字符串转换为普通字符串
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr_in  指针，指向转换前的缓冲区
//|          | in_len  要转换的数符个数
//|          | ptr_out 转换得到的数据缓冲区
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 转换后的值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | in_len为2的整数倍进行转换，如"414243"转换后为"ABC"
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void HexStrToCharStr(u8* ptr_in, u16 in_len, u8* ptr_out)
{
	u16 i=0 ;
	u16 j,loop_length;
	u8 char_h, char_l ;
	
	loop_length = (u16)(2*(in_len/2));
	for(j =0; j < loop_length;)
	{
		char_h =  HexCharToValue(ptr_in[j++]);
		char_l =  HexCharToValue(ptr_in[j++]);
		*(ptr_out+i++) = char_h *16 + char_l;		
	}
	*(ptr_out+i) = '\0';
}

/**
  *************************************************************************************
  * @brief  检查字符串是不是数值
  * @param  Str:  字符串
  * @param  FloatFlag:  是否支持浮点 0不支持浮点与特征号字母 1支持浮点不支持字母  2支持特征号字母 
  * @retval 结果
  * @author ZH
  * @date   2015年7月14日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus CheckNumberChars(char* Str, u8 FloatFlag, u16 Len)
{
	u8 PointFlag = 0;
	u16 i;
	
	for(i=0;i<Len;i++)
	{
		if((Str[i] < '0')||(Str[i] >'9'))
		{
			if(FloatFlag == 0)
			{
				return ERROR;
			}
			else if(FloatFlag == 1)
			{
				if(Str[i] != '.')
				{
					return ERROR;
				}
				else
				{
					PointFlag++;
					if((PointFlag > 1)||(i == 0))
					{
						return ERROR;
					}
				}
			}
			else
			{
				if(((Str[i] < 'A')||(Str[i] >'Z'))&&(i == 8))//第8位为特征号
				{
					return ERROR;
				}
			}

		}
	}
	return SUCCESS;
}
u8 GetDetTypeForXA(const char* DetTypeString)
{
    if(strcmp(DetTypeString, "XA-02") == 0)
    {
        return DET_TYPE_XA_02;
    }
    else if(strcmp(DetTypeString, "XA-06") == 0)
    {
        return DET_TYPE_XA_06;
    }		
    else if(strcmp(DetTypeString, "XA-21") == 0)
    {
        return DET_TYPE_XA_21;
    }
    return 0xFF;
}

const char* GetDetTypeStringForXA(u8 DetType)
{   
    if(DetType == DET_TYPE_XA_02)
    {
        return "XA-02";
    }
    else if(DetType == DET_TYPE_XA_06)
    {
        return "XA-06";
    }		
    else if(DetType == DET_TYPE_XA_21)
    {
        return "XA-21";
    }
    return NULL;
}
u8 VerifyDetCode(void)
{
//    if(DeviceInfo.DetClass == 1)
//    {
//        return 0;
//    }
	if(DET_CODE_VERIFY == UserParam.Stage)
	{
		return 1;
	}
	if((UserParam.Stage == FINISHED) || (UserParam.Stage == SEMIFINISHED))
	{
		if(UserParam.CheckDetCode)
		{
			return 1;
		}
	}	
	return 0;
}
u8 NeedScan(void)
{
//    if(DeviceInfo.DetClass == 1)
//    {
//        return 0;
//    }
	if(DET_CODE_VERIFY == UserParam.Stage)
	{
		return 1;
	}	
	if(ENTER_FACTORY == UserParam.Stage)
	{
		return 0;
	}	
	if ((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
	{
		if(UserParam.DetCodeWrite || UserParam.CheckDetCode)
		{
			return 1;
		}
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  检查雷管是否上线
  * @param  无
  * @retval 结果
  * @author ZH
  * @date   2015年7月14日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus CheckDetCurrentForDetAutoTest(void)
{
	static u8 det_leave = 0;//雷管未下线
	static u16 bus_current[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
	static u8 Ptr = 0;

	bus_current[Ptr++ & 3] = GetCurrentValue(1, NULL); //低压下的正向电流值
	
	if(GetU16DataMax(bus_current, 4) < 1000)//所有电流都小于10
	{
		det_leave = 1;//雷管已下线
	}

	if( det_leave == 1)
	{
		if (GetU16DataAverage(bus_current, 4) >= 3000)//均值电流大于40uA
		{
			if(GetU16DataMin(bus_current, 4) >= 1000)//所有电流都大于10uA
			{
				det_leave = 0;//等待下线				
				return SUCCESS;//有雷管上线
			}			
		}
	}
	return ERROR;//没有雷管上线
}

/**
  *************************************************************************************
  * @brief  检查雷管是否已经下线
  * @param  无  
  * @retval 结果
  * @author ZH
  * @date   2015年7月14日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus CheckDetOff(void)
{
	u16 current = 0;
	u32 ser;
	
	current = GetCurrentValue(10, NULL);
	
	if(current >= 500)
	{
		return ERROR;
	}
	if(ERROR != XA2_ReadSerl(&ser))
	{
		return ERROR;
	}
	return SUCCESS;
}

void UsbAutoIdentify(void)
{
	if(OpData.UsbState != 0xFF)
	{//启用USB
		if(0x80 == (OpData.UsbState&0x80))
		{//强制为主/从机
			if((OpData.UsbState&0x07) != ((OpData.UsbState>>4)&0x07))
			{//未强制识别成功
				OpData.UsbState &= 0xF0;
				if(0x10 == (OpData.UsbState&0x70))
				{
					OpData.UsbState |= 0x01;
					OSTaskDel(APP_CH374_TASK_PRIO);
					TaskCreate(APP_CH374_TASK_PRIO, (void*)0);
				}
				else if(0x20 == (OpData.UsbState&0x70))
				{
					OpData.UsbState |= 0x02;
					OSTaskDel(APP_CH374_TASK_PRIO);
					CH374DeviceInit(DEVICE_HID);
				}
				else if(0x40 == (OpData.UsbState&0x70))
				{
					OpData.UsbState |= 0x04;			
					CH374DeviceInit(DEVICE_MSC);
				}
				else
				{
					OSTaskDel(APP_CH374_TASK_PRIO);
				}
			}			
		}
		else 
		{//允许自动识别
			if(READ_USB_GET_IN_PIN() == USB_CONNET_DEVICE)//平时默认为从机
			{//自身做主机
				if(0x01 != (OpData.UsbState&0x07))
				{
					OpData.UsbState = 0x01;	
					OSTaskDel(APP_CH374_TASK_PRIO);
					TaskCreate(APP_CH374_TASK_PRIO, (void*)0);
				}
			}
			else
			{
				if(0x02 != (OpData.UsbState&0x07))
				{
					OpData.UsbState = 0x02;	
					OSTaskDel(APP_CH374_TASK_PRIO);						
					CH374DeviceInit(DEVICE_HID);
				}
			}		
		}
	}
	else
	{
		OSTaskDel(APP_CH374_TASK_PRIO);	
	}
}

u16 GetMaxDelay(u8 DetType)
{
	u16 Delay = 0;
	
	switch(DetType)
	{
		case DET_TYPE_XA_02:
			Delay = DET_DLY_MAX_XA_02;
			break;
		case DET_TYPE_XA_06:
			Delay = DET_DLY_MAX_XA_06;
			break;			
		case DET_TYPE_XA_21:
			Delay = DET_DLY_MAX_XA_21;
			break;				
		default:
			break;	
	}
	return Delay;
}
u8 AllChannelBarCodeHasData(void)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		if ((Channel.BarCode[i].Status != BARCODE_NEW) || (Channel.BarCode[i].Result != BARCODE_PASS))
		{
			return 0;
		}
	}
	return 1;
}
u8 GetChannelBarCodeEmptyIndex(void)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		if ((Channel.BarCode[i].Status != BARCODE_NEW) || (Channel.BarCode[i].Result != BARCODE_PASS))
		{
			return i;
		}
	}
	return 0xFF;
}
u8 GetBarCodeSameIndex(BARCODE* BarCode)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		if ((Channel.BarCode[i].Status == BARCODE_NEW) && (Channel.BarCode[i].Result == BARCODE_PASS))
		{
			if(memcmp(&Channel.BarCode[i], BarCode, sizeof(BARCODE)) == 0)
			{
				return i;
			}
		}
	}
	return 0xFF;	
}
u8 GetChannelOldStatusErrorCount(void)
{
	u8 i;
	u8 Count = 0;

	for(i=0; i < Channel.Count; i++)
	{
		if ((Channel.OldStatus[i] != DET_PASS) && (Channel.OldStatus[i] != DET_TESTING))
		{
			Count++;
		}
	}
	return Count;
}
u8 GetChannelOldStatusErrorIndex(u8 Order)
{
	u8 i=0;
	u8 Count = 0;
	
	for(i=0;i<Channel.Count;i++)
	{
		if(DET_PASS != Channel.OldStatus[i])
		{
			if(Count == Order)
			{
				return i;
			}
			Count++;
		}
	}
	return 0xFF;	
}
void SaveChannelOldStatus(void)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		Channel.OldStatus[i] = Channel.XA2_Info[i].Status;
	}
}
void ClearChannelOldStatus(void)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		Channel.OldStatus[i] = DET_TESTING;
	}
}
void UpdateDetDetectorPassCount(void)
{
	u8 i;

	for(i=0; i < Channel.Count; i++)
	{
		if (Channel.XA2_Info[i].Status == DET_PASS)
		{
			OpData.DetDetectorPassCount++;
		}
	}
}
ErrorStatus GetCurrentDetOutsideStart(u8* DetOutside)
{
    if(UserParam.DetOutsideStartBoxIndex > 99999L)
    {
        return ERROR;
    }
//    if(UserParam.DetOutsideStartHead > 99999999L)
//    {
//        return ERROR;
//    }    
    sprintf((char*)DetOutside, "%s%05d", UserParam.DetOutsideStartHead, UserParam.DetOutsideStartBoxIndex);
    return DetOutsideVerify(DetOutside);
}
/**
  *************************************************************************************
  * @brief  判断管码是否非法
  * @param  无  
  * @retval 结果
  * @author ZH
  * @date   2019年1月15日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DetOutsideVerify(u8* Ptr)
{
    u8 U8Value;
	   
    if(CheckNumberChars((char*)Ptr, 2, 13) == ERROR)
    {
        return ERROR;
    }
    U8Value = (*Ptr-'0')*10 + (*(Ptr+1)-'0');
    if((U8Value == 0) || (U8Value > 74))//工厂代码
    {
        return ERROR;
    }
    U8Value = (*(Ptr+3)-'0')*10 + (*(Ptr+4)-'0');
    if((U8Value == 0) || (U8Value > 12))//月
    {
        return ERROR;
    }
    U8Value = (*(Ptr+3)-'0')*10 + (*(Ptr+4)-'0');
    if((U8Value == 0) || (U8Value > 31))//日
    {
        return ERROR;
    }
    return SUCCESS;
}
void CreateChannelDetOutside(void)
{
	u8 i;
    u32 BoxIndex;
    
    BoxIndex = UserParam.DetOutsideStartBoxIndex;
	for(i=0; i < Channel.Count; i++)
	{
        if(((BoxIndex)%100) >= UserParam.InBoxNumber)
        {
            BoxIndex = ((BoxIndex/100)+1)*100;
        }
        if((BoxIndex/100) > 999)
        {
            BoxIndex = 0;
        }
		sprintf((char*)Channel.BarCode[i].CodeExternal, "%s%05d", UserParam.DetOutsideStartHead, BoxIndex);        
        BoxIndex++;
	}
}

/**
  *************************************************************************************
  * @brief  从管码中得到盒号
  * @param  无  
  * @retval 盒号
  * @author ZH
  * @date   2019年1月15日
  * @note   无
  *************************************************************************************  
  */
u16 GetBoxNumberFromDetOutside(u8* DetOutside)
{
    return (*(DetOutside+8)-'0')*100 + (*(DetOutside+9)-'0')*10 + (*(DetOutside+10)-'0');
}

/**
  *************************************************************************************
  * @brief  从管码中得到盒内流水号
  * @param  无  
  * @retval 盒内流水号
  * @author ZH
  * @date   2019年1月15日
  * @note   无
  *************************************************************************************  
  */
u8 GetInBoxIndexFromDetOutside(u8* DetOutside)
{
    return (*(DetOutside+11)-'0')*10 + (*(DetOutside+12)-'0');
}
/**
  *************************************************************************************
  * @brief  从管码中得到盒号+流水号
  * @param  无  
  * @retval 盒号
  * @author ZH
  * @date   2019年1月15日
  * @note   无
  *************************************************************************************  
  */
u32 GetBoxIndexFromDetOutside(u8* DetOutside)
{
    return GetBoxNumberFromDetOutside(DetOutside)*100+GetInBoxIndexFromDetOutside(DetOutside);
}

