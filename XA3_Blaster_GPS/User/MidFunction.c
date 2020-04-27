#include "includes.h"
extern void UpdateProgbar(u16 Value);

static  OS_EVENT   *LcdBrightMutex;

void LcdBrightInitOS(void)
{
	INT8U  err;
	
	LcdBrightMutex = OSMutexCreate(RES_LCD_BRIGHT_MUTEX_PRIO, &err);
}
static void LcdBrightLock(void)
{
	INT8U  err;
	
	OSMutexPend(LcdBrightMutex,0,&err);
}
static void LcdBrightUnlock(void)
{	
	OSMutexPost(LcdBrightMutex);
}
/**
  *************************************************************************************
  * @brief  UFT8字符串字符数量
  * @param[in]  Str：UFT8字符串
  * @retval 字符数量
  * @author ZH
  * @date   2014年9月15日
  * @note   UFT8字符串英文点1字节，汉字占3字节
  *************************************************************************************  
  */
u16 GetUtf8StrCharsCount(const char* Str)
{
	u16 EnNumber = 0,ZhNumber = 0;
	u8 *Ptr;
	
	Ptr = (u8*)Str;
	
	while(*Ptr != 0)
	{
		if(*Ptr <= 0x80)
		{
			EnNumber++;
			Ptr++;
		}
		else
		{
			ZhNumber++;
			Ptr+=3;
		}
	}
	return (EnNumber+ZhNumber*2);
}

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
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
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
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 U16DataCMP(const u16* ptr, u8 length)
{
	u16 max_value, min_value ;
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
//| 函数名称 | GetU16DataMax 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对一组数据(16位)求最大值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | ptr 指针，指要求平均值的数据
//|          | lenght数据个数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 最大值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
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
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
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
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
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
//| 函数名称 | FindUnregHole 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据输入参数，用不同的方法查找未注册雷管的孔号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | current 当前所在的孔索引
//|          | 查找方式 0 从后向前查找碰到第1个未注册孔后停止
//|          |          1 以当前孔为起点，向前查找未注册孔
//|          |          2 以当前孔为起点，向后查找未注册孔
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 FindUnregHole(u16 current, u8 dir)
{
	s16 i;
	
	switch(dir)
	{
		case 0:
			for(i=DET_MAX_SIZE -1; i>=0; i--) //从孔号大的往孔号小的方向查找
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if(i == DET_MAX_SIZE-1)
					{
						return i;
					}
					else
					{
						return (i+1);			
					}
				}
			}			
			break;
		case 1:
			for(i=DET_MAX_SIZE -1; i>=0; i--) //以当前孔为起点，向前查找未注册孔
			{
				if(0==current)
				{
					current = DET_MAX_SIZE;
				}
				current = (current - 1) % DET_MAX_SIZE;
				if (GetDetRegStatus(i) == ERROR)
				{
					return current;
				}
			}			
			break;
		case 2:
			for(i=0; i<=DET_MAX_SIZE -1; i++) //以当前孔为起点，向后查找未注册孔
			{
				current = (current + 1) % DET_MAX_SIZE;
				if (GetDetRegStatus(i) == ERROR)
				{
					return current;
				}
			}
			break;		
		default:
			break;
	}
	return 0;	
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
//| 函数设计 | 编写人：ZH    时间：2013-08-25 
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
//| 函数设计 | 编写人：ZH    时间：2013-11-02 
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

//==================================================================================================
//| 函数名称 | HexCharToValue 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 把单个16进制字符转换为数值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | c 要转换的字符
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 转换后的值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-11-02 
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
u8 IsHexChar(u8 c)
{
	if( c >= '0' && c <= '9')
	{
		return 1;
	}
	else if( c >= 'A' && c <= 'F')
	{
		return 1;
	}
	else if( c >= 'a' && c <= 'f')
	{
		return 1;
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
//| 函数设计 | 编写人：ZH    时间：2013-11-02 
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
  * @param  FloatFlag:  是否支持浮点  
  * @retval 结果
  * @author ZH
  * @date   2015年7月14日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus CheckNumberChars(char* Str, u8 FloatFlag)
{
	u8 PointFlag = 0;
	u16 i;
	
	for(i=0;i<strlen(Str);i++)
	{
		if((Str[i] < '0')||(Str[i] >'9'))
		{
			if(FloatFlag == 0)
			{
				return ERROR;
			}
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
	}
	return SUCCESS;
}
void U32ToAscii(u32 value, u8* str)
{
	u8 i = 9;
	u8 temp[11];
	
	temp[10] = 0x00;
	do
	{
		temp[i--] = value%10 + '0'; //逐位转换，采用倒序存储字符，省去了后续的字符颠倒工作
	}while((value /= 10)>0);
	strcpy((char*)str, (char*)&temp[i+1]);
}
u32 AsciiToU32(u8* str)
{
	u32 value = 0;
	
	while(*str != NULL)
	{
		value *= 10;
		value += *str-'0';
		str++;
	}
	return value;
}

/**
  *************************************************************************************
  * @brief  把所有已注册的雷管按孔号从小到大重新排序
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
void AllRegDetRankBaseOnHoleInfo(void)
{
	u16 i,j;
	u8 flag_bit;
	DET_INFO DetBuffer;
	u32 Value1, Value2;

	if(Det.RegCount == 0)
	{
		return ;
	}
	if(Det.RegCount != DET_MAX_SIZE)
	{
		//把所有已经注册的雷管往前移,确保所有已注册的雷管都是保存在最前边
		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{//把该孔数据往前移
				for(j=0; j < Det.RegCount; j++)
				{
					if (GetDetRegStatus(j) == ERROR)
					{
						DetBuffer = Det.Info[j];
						Det.Info[j] = Det.Info[i];
						Det.Info[i] = DetBuffer;
					}
				}				
			}
		}			
	}	
	
	//冒泡排序，按孔号从小到大排列
	for(i=0; i < Det.RegCount-1; i++)
	{
		flag_bit = 1;
		for(j=0; j < Det.RegCount-i-1; j++)
		{
			Value1 = ((u32)Det.Info[j].Hole.Name<<16) + Det.Info[j].Hole.Inside;
			Value2 = ((u32)Det.Info[j+1].Hole.Name<<16) + Det.Info[j+1].Hole.Inside;
			 if(Value1 > Value2)
			 {
				 flag_bit = 0 ;
				 DetBuffer = Det.Info[j] ;
				 Det.Info[j] = Det.Info[j+1];
				 Det.Info[j+1]= DetBuffer ;
			 }
		}
		if(flag_bit)//提前结束排序
		{
			break ;
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  把所有已注册的雷管按延期从小到大重新排序
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年10月30日
  * @note   无
  *************************************************************************************  
  */
void AllRegDetRankBaseOnDelay(void)
{
	u16 i,j;
	u8 flag_bit;
	DET_INFO DetBuffer;
	u32 Value1, Value2;

	if(Det.RegCount == 0)
	{
		return ;
	}
	if(Det.RegCount != DET_MAX_SIZE)
	{
		//把所有已经注册的雷管往前移,确保所有已注册的雷管都是保存在最前边
		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{//把该孔数据往前移
				for(j=0; j < Det.RegCount; j++)
				{
					if (GetDetRegStatus(j) == ERROR)
					{
						DetBuffer = Det.Info[j];
						Det.Info[j] = Det.Info[i];
						Det.Info[i] = DetBuffer;
					}
				}				
			}
		}			
	}	
	
	//冒泡排序，按孔号从小到大排列
	for(i=0; i < Det.RegCount-1; i++)
	{
		flag_bit = 1;
		for(j=0; j < Det.RegCount-i-1; j++)
		{
            Value1 = Det.Info[j].Delay;
			Value2 = Det.Info[j+1].Delay;
            if(Value1 > Value2)
            {
                flag_bit = 0 ;
                DetBuffer = Det.Info[j] ;
                Det.Info[j] = Det.Info[j+1];
                Det.Info[j+1]= DetBuffer ;
            }
		}
		if(flag_bit)//提前结束排序
		{
			break ;
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  把所有已注册的雷管按内码从大到小重新排序
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年02月25日
  * @note   无
  *************************************************************************************  
  */
//void AllRegDetRankBaseOnDetInfo(void)
//{
//	u16 i,j;
//	u8 flag_bit;
//	DET_INFO DetBuffer;
//	u8 Temp1[6],Temp2[6];

//	if(Det.RegCount == 0)
//	{
//		return ;
//	}
//	if(Det.RegCount != DET_MAX_SIZE)
//	{
//		//把所有已经注册的雷管往前移,确保所有已注册的雷管都是保存在最前边
//		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
//		{
//			if (GetDetRegStatus(i) != ERROR)
//			{//把该孔数据往前移
//				for(j=0; j < Det.RegCount; j++)
//				{
//					if((Det.Info[j].Serl_1 == 0x00) && (Det.Info[j].Serl_2 == 0x00))
//					{
//						DetBuffer = Det.Info[j];
//						Det.Info[j] = Det.Info[i];
//						Det.Info[i] = DetBuffer;
//					}
//				}				
//			}
//		}			
//	}	
//	
//	//冒泡排序，按内码从大到小排列
//	for(i=0; i < Det.RegCount-1; i++)
//	{
//		flag_bit = 1;
//		for(j=0; j < Det.RegCount-i-1; j++)
//		{
//			Temp1[0] = Det.Info[j].Serl_1;
//			Temp1[1] = Det.Info[j].Serl_2;
//			Temp1[2] = Det.Info[j].Serl_3;
//			Temp1[3] = Det.Info[j].Factor;
//			Temp1[4] = Det.Info[j].bat;
//			Temp1[5] = Det.Info[j].year;
//			
//			Temp2[0] = Det.Info[j+1].Serl_1;
//			Temp2[1] = Det.Info[j+1].Serl_2;
//			Temp2[2] = Det.Info[j+1].Serl_3;
//			Temp2[3] = Det.Info[j+1].Factor;
//			Temp2[4] = Det.Info[j+1].bat;
//			Temp2[5] = Det.Info[j+1].year;
//			 if(memcmp((const char*)&Temp1[0], (const char*)&Temp2[0], 6) < 0)
//			 {//Temp1 小于 Temp2
//				 flag_bit = 0 ;
//				 DetBuffer = Det.Info[j] ;
//				 Det.Info[j] = Det.Info[j+1];
//				 Det.Info[j+1]= DetBuffer ;
//			 }
//		}
//		if(flag_bit)//提前结束排序
//		{
//			break ;
//		}		
//	}	
//}
/**
  *************************************************************************************
  * @brief  根据输入的孔信息查找索引值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年11月17日
  * @note   
  *************************************************************************************  
  */
u16 GetDetIndexFromHoleInfo(HOLE* Hole)
{
	u16 i;
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if ((Hole->Name == Det.Info[i].Hole.Name) && (Hole->Inside == Det.Info[i].Hole.Inside))
			{
				return i;
			}		
		}
	}
	return 0xFFFF;
}
/**
  *************************************************************************************
  * @brief  根据输入的孔信息查找附近孔号的索引值(优先找孔号比他小的)
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年11月15日
  * @note   自行保证有注册雷管，
  *************************************************************************************  
  */
u16 GetNearbyDetIndexFromHoleInfo(HOLE* Hole)
{
	u16 i;
	u32 Value1, Value2;
    
    AllRegDetRankBaseOnHoleInfo();//把所有雷管按孔号从小到大重新排列
    
    Value2 = ((u32)Hole->Name<<16) + Hole->Inside;
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
            Value1 = ((u32)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
			if (Value1 > Value2)
			{
                if(i > 0)
                {
                    return i-1;
                }
				return i;
			}
            else if (Value1 == Value2)
			{
				return i;
			}
		}
	}
    if(Det.RegCount > 0)
    {
        return Det.RegCount - 1;
    }
	return 0;
}
/**
  *************************************************************************************
  * @brief  获取一个没有注册的雷管索引值
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年11月17日
  * @note   
  *************************************************************************************  
  */
u16 GetEmptyDetIndex(void)
{
	u16 i;
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) == ERROR)
		{
			return i;
		}
	}
	return 0;
}
//==================================================================================================
//| 函数名称 | GetErrorDetIndex 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据输入的错误雷管列表索引值查找对应的雷管列表索引号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | err_index 错误雷管列表中的索引值
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管列表中的索引值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetErrorDetIndex(u16 err_index)
{
	u16 i, err_num = 0;
	
	AllRegDetRankBaseOnHoleInfo();//把所有雷管按孔号从小到大重新排列
	for(i=0;i<DET_MAX_SIZE;i++)
	{
		if((GetDetRegStatus(i) != ERROR) && (Det.Info[i].Err != 0x00))
		{
			if(err_index == err_num)
			{
				return i ;
			}
			err_num++;			
		}		
	}
	return 0;
}

/**
  *************************************************************************************
  * @brief  根据输入的孔信息找出已注册的最大和最小孔内序号
  * @param  Hole   要查找到孔信息
  * @param  MaxInside 已注册的最大孔内序号
  * @param  MinInside 已注册的最小孔内序号  
  * @retval 查找结果
  * @author ZH
  * @date   2015年03月26日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetCurrentHoleMaxMinRegInside(u16 HoleName, u8* MaxInside, u8* MinInside)
{
	u16 i;
	u16 Buffer[NET_PARAM_MAX], Count = 0;
	
	memset((u8*)&Buffer[0], 0, sizeof(Buffer));
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if ((GetDetRegStatus(i) != ERROR) && (Det.Info[i].Hole.Name == HoleName))
		{//已注册
			Buffer[Count++] = Det.Info[i].Hole.Inside;
		}	
	}
	if(Count == 0)
	{
		return ERROR;
	}
	U16DataFallRank(Buffer, Buffer, Count);
	if(MaxInside != NULL)
	{
		*MaxInside = Buffer[0];
	}
	if(MinInside != NULL)
	{
		*MinInside = Buffer[Count-1];
	}
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  根据输入的孔信息计算最邻近的孔内序号(已注册）
  * @param  Hole   要查找到孔信息
  * @param  Inside 查找到的最邻近的孔内序号(已注册）
  * @retval 查找结果
  * @author ZH
  * @date   2015年03月26日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetCurrentHoleLastRegInside(HOLE* Hole, u8* Inside)
{
	u16 i;
	u16 Buffer[NET_PARAM_MAX], Count = 0;
	
	memset((u8*)&Buffer[0], 0, sizeof(Buffer));
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if ((GetDetRegStatus(i) != ERROR) && (Det.Info[i].Hole.Name == Hole->Name))
		{//已注册
			Buffer[Count++] = Det.Info[i].Hole.Inside;
		}	
	}
	if(Count == 0)
	{
		return ERROR;
	}
	U16DataFallRank(Buffer, Buffer, Count);
	for(i=0; i<Count; i++)
	{
		if(Buffer[i] < Hole->Inside)
		{
			*Inside = Buffer[i];
			return SUCCESS;
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  获取已注册的最大孔号
  * @param  Type 自检项
  * @retval 自检结果
  * @author ZH
  * @date   2018年11月02日
  * @note   确保有注册雷管时调用
  *************************************************************************************  
  */
u16 GetRegDetHoleMax(void)
{
	u16 i, index;
    u32 U32Value,U32Max;
    
    index = 0;
    U32Max = 0xFFFFFF;
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
        {
            U32Value = (Det.Info[i].Hole.Name<<8)+Det.Info[i].Hole.Inside;
            if ((U32Max == 0xFFFFFF) || (U32Value > U32Max))
            {
                index = i;
                U32Max = U32Value;
            }
        }
    }
    return index;
}
/**
  *************************************************************************************
  * @brief  检查是否有相同内码
  * @param  Type 自检项
  * @retval 自检结果
  * @author ZH
  * @date   2015年03月02日
  * @note   
  *************************************************************************************  
  */
u8 DetInfoHasSame(void)
{
	u16 i, j;

	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//已经注册
			for(j=i+1; j < DET_MAX_SIZE; j++)
			{
				if (GetDetRegStatus(j) != ERROR)
				{//已经注册
					if(Det.Info[i].Serl.U32Value == Det.Info[j].Serl.U32Value)
					{
						return 1;
					}					
				}
			}	
		}
	}	
	return 0;
} 
/**
  *************************************************************************************
  * @brief  根据输入的孔号计算孔内序号为1的雷管延期
  * @param  HoleName 孔号
  * @retval 孔内序号为1的雷管延期
  * @author ZH
  * @date   2015年03月27日
  * @note   孔号N-1的延期=M-1的延期+孔间延期，N=M+1,即后一个孔的延期由前一个孔得到
  *************************************************************************************  
  */
u16 GetHoleNameFristInsideDelay(u16 HoleName)
{
	u16 i;
	u16 Index=0, MaxRegHoleName;
	int64_t Delay;
	
	Index = 0;
	MaxRegHoleName = 0;
	for(i=0; i <= DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//已经注册
			if ((Det.Info[i].Hole.Inside == 1) && (Det.Info[i].Hole.Name <= HoleName))
			{
				if (Det.Info[i].Hole.Name > MaxRegHoleName)
				{
					Index = i;
					MaxRegHoleName = Det.Info[i].Hole.Name;
				}
			}
		}
	}
	if(MaxRegHoleName == 0)
	{
		//没有找到N-1中已经注册有雷管
		Delay = 0 + (int64_t)NetParam[Det.HoleNumber[HoleName-1]-1].SpacesDly* (HoleName-1);	
	}
	else
	{
		Delay = (int64_t)Det.Info[Index].Delay + 
			(int64_t)NetParam[Det.HoleNumber[HoleName-1]-1].SpacesDly* (HoleName - Det.Info[Index].Hole.Name);	
	}

	if(Delay > Det.DlyMax)
	{
		Delay = Det.DlyMax;
	}	
	return Delay;
}
/**
  *************************************************************************************
  * @brief  根据输入的起始/截止孔信息计算统计注册雷管数
  * @param  无
  * @retval 注册雷管数
  * @author ZH
  * @date   2016年11月28日
  * @note   
  *************************************************************************************  
  */
u16 GetCountSpecifiedHoleRange(HOLE* StartHole, HOLE* EndHole)
{
	u16 i;
	u16 Count=0;

	for(i=0; i <= DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//已经注册
			if ((Det.Info[i].Hole.Name > StartHole->Name) && (Det.Info[i].Hole.Name < EndHole->Name))
			{
				Count++;
			}
			else if ((Det.Info[i].Hole.Name == StartHole->Name) && (Det.Info[i].Hole.Inside >= StartHole->Inside))
			{
				Count++;
			}
			else if ((Det.Info[i].Hole.Name == EndHole->Name) && (Det.Info[i].Hole.Inside <= EndHole->Inside))
			{
				Count++;
			}			
		}
	}
	return Count;
}

/**
  *************************************************************************************
  * @brief  根据输入的起始孔信息获取第N个注册孔对应的孔索引
  * @param  StartHole 起始孔
  * @param  Pos 起始孔往后第几个，从1开始
  * @retval 找到的孔索引
  * @author ZH
  * @date   2016年11月28日
  * @note   需要提前把孔号进行从小到大排列
  *************************************************************************************  
  */
u16 GetHoleIndexSpecifiedStartHole(HOLE* StartHole, u16 Pos)
{
	u16 i;
	u16 Count=0;
	
	for(i=0; i <= DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//已经注册
			if (Det.Info[i].Hole.Name > StartHole->Name)
			{
				Count++;
			}
			else if ((Det.Info[i].Hole.Name == StartHole->Name) && (Det.Info[i].Hole.Inside >= StartHole->Inside))
			{
				Count++;
			}
			if(Count == Pos)
			{
				return i;
			}
		}
	}
	return 0xFFFF;
}
/**
  *************************************************************************************
  * @brief  根据输入的孔信息计算对应的延期值
  * @param  无
  * @retval 延期值
  * @author ZH
  * @date   2019年3月29日
  * @note   计算出的延期值超出雷管的最大延期时返回0xFFFF
  *************************************************************************************  
  */
u16 CalcRegHoleDelayFromNetParam(u16 Index)
{
	int64_t Value = 0;
	int i ;
	NET_PARAM Param;
	HOLE Hole;
    
    Hole = Det.Info[Index].Hole;
    Param = NetParam[Det.HoleNumber[Hole.Name-1]-1];
    
    Value = (Hole.Name-1)*Param.SpacesDly+ (Hole.Inside-1)*Param.InsideSpacesDly;
	if(Index != 0)
    {
        for(i=Index-1; i>=0; i--)
        {
            if(Det.Info[i].Hole.Name == Hole.Name)
            {
                Value = Det.Info[i].Delay+(Hole.Inside-Det.Info[i].Hole.Inside)*Param.InsideSpacesDly;
                break;
            }
            else if(Det.Info[i].Hole.Inside == 1)
            {
                Value = Det.Info[i].Delay + (Hole.Name - Det.Info[i].Hole.Name)*Param.SpacesDly+ (Hole.Inside-1)*Param.InsideSpacesDly;
                break;
            }
        }
    }

	if(Value > Det.DlyMax)
	{
		Value = 0xFFFF;
	}
	return (u16)Value;	
}

/**
  *************************************************************************************
  * @brief  根据输入的延期参数计算判断延期值是否超限
  * @param  无
  * @retval ErrorStatus
  *         @arg @b ERROR 延期超限
  *         @arg @b SUCCESS 延期没有超限 
  * @author ZH
  * @date   2014年11月17日
  * @note   只计算是否超限，不修改延期值
  *************************************************************************************  
  */
ErrorStatus DelayParamVerify(HOLE* StartHole, HOLE* EndHole, u16 StartDly, u16 SpacesDly, u16 InsideSpacesDly)
{
	u16 i;
	int64_t Dly, FirstHoleDlg, Value, StartValue, EndValue;
	
	StartValue = ((int64_t)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((int64_t)EndHole->Name<<16) + EndHole->Inside;
	
	//先计算第1个孔，孔内序号为1对应的延期值,有可能是负数
	FirstHoleDlg = (int64_t)StartDly - (int64_t)InsideSpacesDly*((StartValue&0xFFFF)-1);
	
	if(StartValue <= EndValue)
	{//正序
		for(i=0;i<DET_MAX_SIZE;i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				Value = ((int64_t)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
				if((Value >= StartValue) && (Value <= EndValue))
				{
					Dly = FirstHoleDlg + (int64_t)SpacesDly* (Det.Info[i].Hole.Name-(StartValue>>16)) + 
						(int64_t)InsideSpacesDly*(Det.Info[i].Hole.Inside-1);
						
					if((Dly > Det.DlyMax) || (Dly < 0))
					{
						return ERROR;
					}
				}
			}
		}		
	}
	else
	{//倒序
		for(i=0;i<DET_MAX_SIZE;i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				Value = ((int64_t)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
				if((Value <= StartValue) && (Value >= EndValue))
				{
					Dly = FirstHoleDlg + (int64_t)SpacesDly* ((StartValue>>16) - Det.Info[i].Hole.Name) + 
						(int64_t)InsideSpacesDly*(Det.Info[i].Hole.Inside-1);
						
					if((Dly > Det.DlyMax) || (Dly < 0))
					{
						return ERROR;
					}
				}
			}
		}		
	}
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  根据输入的延期参数修改雷管延期值
  * @param  无
  * @retval ErrorStatus
  *         @rag @b ERROR 延期超限
  *         @rag @b SUCCESS 延期没有超限  
  * @author ZH
  * @date   2014年11月17日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DelayModifyBaseOnParam(HOLE* StartHole, HOLE* EndHole, u16 StartDly, u16 SpacesDly, u16 InsideSpacesDly)
{
	u16 i;
	int64_t Dly, FirstHoleDlg, Value, StartValue, EndValue;

	if(ERROR == DelayParamVerify(StartHole, EndHole, StartDly, SpacesDly, InsideSpacesDly))
	{
		return ERROR;
	}
	
	StartValue = ((int64_t)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((int64_t)EndHole->Name<<16) + EndHole->Inside;
	
	//先计算第1个孔，孔内序号为1对应的延期值,有可能是负数
	FirstHoleDlg = (int64_t)StartDly - (int64_t)InsideSpacesDly*((StartValue&0xFFFF)-1);
		
	if(StartValue <= EndValue)
	{//正序
		for(i=0;i<DET_MAX_SIZE;i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				Value = ((int64_t)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
				if((Value >= StartValue) && (Value <= EndValue))
				{
					Dly = FirstHoleDlg + (int64_t)SpacesDly* (Det.Info[i].Hole.Name-(StartValue>>16)) + 
						(int64_t)InsideSpacesDly*(Det.Info[i].Hole.Inside-1);
						
					if((Dly > Det.DlyMax) || (Dly < 0))
					{
						return ERROR;
					}
					Det.Info[i].Delay = (u16)Dly;
				}
			}
		}	
	}
	else
	{//倒序
		for(i=0;i<DET_MAX_SIZE;i++)
		{
			if (GetDetRegStatus(i) != ERROR)
			{
				Value = ((int64_t)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
				if((Value <= StartValue) && (Value >= EndValue))
				{
					Dly = FirstHoleDlg + (int64_t)SpacesDly* ((StartValue>>16) - Det.Info[i].Hole.Name) + 
						(int64_t)InsideSpacesDly*(Det.Info[i].Hole.Inside-1);
						
					if((Dly > Det.DlyMax) || (Dly < 0))
					{
						return ERROR;
					}
					Det.Info[i].Delay = (u16)Dly;
				}
			}
		}		
	}
		
	return SaveAllDetInfo(0);
}
/**
  *************************************************************************************
  * @brief  设置已注册雷管的延期值
  * @param  延期值
  * @retval 无
  * @author ZH
  * @date   2016年8月15日
  * @note   
  *************************************************************************************  
  */
void SetRegDetDelay(u16 Delay)
{
	u16 i;
	
	for(i=0;i<DET_MAX_SIZE;i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Det.Info[i].Delay = Delay;
		}
	}
}
/**
  *************************************************************************************
  * @brief  根据输入的延期参数计算判断延期值是否超限
  * @param  无
  * @retval ErrorStatus
  *         @rag @b ERROR 延期超限
  *         @rag @b SUCCESS 延期没有超限  
  * @author ZH
  * @date   2014年11月17日
  * @note   为了避免其他问题，返回的索引值为按孔号大小排序后的值
  *************************************************************************************  
  */
u16 GetRegHoleMinIndexFromHoleRange(HOLE* StartHole, HOLE* EndHole)
{
	u16 i;
	u32 Value, StartValue, EndValue;
	
	StartValue = ((u32)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((u32)EndHole->Name<<16) + EndHole->Inside;
	
	if(0x00 == Det.RegCount)
	{
		return 0xFFFF;//没有找到
	}
	
	AllRegDetRankBaseOnHoleInfo();//把所有已注册孔按从小到大的程序重新排列
	for(i=0;i<Det.RegCount;i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Value = ((u32)Det.Info[i].Hole.Name<<16) + Det.Info[i].Hole.Inside;
			if((Value >= StartValue) && (Value <= EndValue))
			{
				return i;
			}
		}
	}
	return 0xFFFF;//没有找到
}

ErrorStatus ValidateDetType(void)
{
	u16 i;
	
	for(i=0; i<DET_MAX_SIZE;i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				return ERROR;
			}		
		}
	}
	return SUCCESS;
}

//==================================================================================================
//| 函数名称 | ValidateDetTypeAndDelay 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 确认雷管类型及延期是否一致
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-11-21 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 在调用本函数前请务必先读取配置参数
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void ValidateDetTypeAndDelay(void)
{
	u16 i, Flag = 0;
	
	for(i=0; i<DET_MAX_SIZE;i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				DeleteAllDetInfo();
				Flag = 0;
				break;
			}			
		}
		if(Det.Info[i].Delay > Det.DlyMax)
		{
			Det.Info[i].Delay = Det.DlyMax;
			Flag = 1;
		}
	}
	if(Flag != 0)
	{
		SaveAllDetInfo(0);	
	}
}

//==================================================================================================
//| 函数名称 | GetDetMaxDelay 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据雷管类型获取对应的最大延期值
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 雷管类型
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 对应的最大延期值
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2013-09-23 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 无
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 GetDetMaxDelay(u8 dettype)
{
	u16 temp = 0;
	
	switch(dettype)
	{
		case DET_TYPE_XA02:
			temp = DET_DLY_MAX_XA02;
			break;
		case DET_TYPE_XA06:
			temp = DET_DLY_MAX_XA06;
			break;
		case DET_TYPE_XA21:
			temp = DET_DLY_MAX_XA21;
			break;            
		default:
			break;
	}
	return temp;	
}
/**
  *************************************************************************************
  * @brief  总线状态是否空闲
  * @param  无   
  * @retval 总线状态
  *         @arg @b ERROR 不空闲
  *         @arg @b SUCCESS 空闲 
  * @author ZH
  * @date   2014年11月20日
  * @note   只有在6.7V和11.3V状态下才认为空闲，5V或者关闭总线时不空闲
  *************************************************************************************  
  */
ErrorStatus BusStatusIsIdle(void)
{
	if((DET_END == DetProtocol.SendBuffer.State) && (GET_DET_RECEIVE_RES_STATE() == DET_RECEIVE_RES_ON))
	{
		if (0 != GET_LINAB_SHORT_SW_CTRL_STATE())
		{//当前总线电压还没有关闭
			if(GET_HV_SW_CTRL_STATE() == 0)
			{//当前为低压状态
				if ((0 == GET_LV_5V_SW_STATE()) && (0 != GET_LV_6_7V_SW_STATE()))
				{//当前没有切换到了5V状态
					return SUCCESS;
				}		
			}
			else
			{
				return SUCCESS;
			}		
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  总线状态监测
  * @param  Voltage 总线电压值
  * @param  Current 总线电流值   
  * @retval 总线状态
  *         @arg @b 0 总线正常
  *         @arg @b 1 总线短路
  *         @arg @b 2 总线电压异常  
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
u8 BusStateMonitor(u16 Voltage, u16 Current)
{
	static u8 VoltageErrorCount = 0;//总线电压异常计数器
	static u8 ShortCount = 0;//总线短路计数器

	if ((Op_Data.BusMonitorDisable) || (Op_Data.AdjustStatus != 0))
	{
		VoltageErrorCount = 0;//总线电压异常计数器
		ShortCount = 0;//总线短路计数器		
	}
	else if(ERROR == BusStatusIsIdle())
	{//正在雷管通信、5V或者关闭总线
		VoltageErrorCount = 0;//总线电压异常计数器
		ShortCount = 0;//总线短路计数器	
	}
	else if ((0 == GET_LINAB_SHORT_SW_CTRL_STATE()) || (OSTimeGet() < Op_Data.LinAdjustTime))
	{//当前总线电压已经关闭、当前电流变化太大，禁止调整电压
		VoltageErrorCount = 0;//总线电压异常计数器
		ShortCount = 0;//总线短路计数器
	}
	else 
	{
		if (BusIsShort())
		{	
			ShortCount++;
			if(ShortCount>20)//连续10秒
			{
				if(Op_Data.BusMonitorCfg == 0)
				{
					CLOSE_BUS_VOLTAGE();
				}
				
				return 1;//电流大于15000uA，认为总线短路		
			}            
		}
		else
		{
			ShortCount = 0;//总线短路计数值清空
		}
		if(GET_HV_SW_CTRL_STATE() == 0)
		{//当前为低压状态
			if ((0 == GET_LV_5V_SW_STATE()) && (0 != GET_LV_6_7V_SW_STATE()))
			{//当前没有切换到了5V状态
				if (abs(Voltage -  Op_Data.LinNormalLowVoltageGoal) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
				{
					VoltageErrorCount++;//总线电压异常计数器
				}
				else
				{
					VoltageErrorCount = 0;//总线电压异常计数器
				}				
			}
			else
			{
				VoltageErrorCount = 0;//当前为5V状态，不检测电压
			}
		}
		else
		{//高压已经输出
			if (abs(Voltage -  Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
            {
                VoltageErrorCount++;//总线电压异常计数器
            }
            else
            {
                VoltageErrorCount = 0;//总线电压异常计数器
            }			
		}
		if(VoltageErrorCount >= 25)//比电流计数要多，在总线短路时不会提示电压异常
		{
			if(Op_Data.BusMonitorCfg == 0)
			{
				CLOSE_BUS_VOLTAGE();			
			}
			return 2;//电压异常
		}
	}
    if ((ShortCount == 6) || (ShortCount == 10) || (VoltageErrorCount == 6) || (VoltageErrorCount == 10))
    {//3秒、5秒
        WriteSystemLog("当前总线状态", 0, CURRENT_STATUS);
    }

	return 0;//总线正常
}
//==================================================================================================
//| 函数名称 | CheckDetDelayIsSame 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 判断是否所有雷管的延期值都相同
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 相同的延期值，0xFFFF表示延期值不相同
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：ZH    时间：2014-07-17
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 没有注册雷管可认为雷管延期不相同
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u16 CheckDetDelayIsSame(void)
{
	u16 i, Delay=0xFFFF;
	
	if(Det.RegCount <= 0x0001)
	{
		return 0xFFFF;//没有注册雷管或只有1发雷管可认为雷管延期不相同
	}
	for(i=0; i< DET_MAX_SIZE ;i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(0xFFFF == Delay)
			{
				Delay = Det.Info[i].Delay;
			}
			else
			{
				if(Det.Info[i].Delay != Delay)
				{
					return 0xFFFF;
				}
			}
		}
	}
	return Delay;
}

/**
  *************************************************************************************
  * @brief  等待总线电流稳定
  * @param  TimeOut 等待超时时间
  * @retval 等待结果
  *         @arg @b SUCCESS 电流稳定
  *         @arg @b ERROR 已经超时
  * @author ZH
  * @date   2015年01月17日
  * @note   
  *************************************************************************************  
  */
ErrorStatus WaiLinCurrentSteady(u32 TimeOut)
{	
	u16 CurrentBuffer[10];
	u8 Count = 0, Flag = 0;
	u16 Average, MaxMin;
	u32 c_time;
	
	c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
	    OSTimeDlyHMSM(0, 0, 0, 30);
		if(Count >= 10)
		{
			Flag = 1;
			Count %= 10;
		}
		
		CurrentBuffer[Count++] = GetBusCurrent(10, NULL);	
		
		if(Flag != 0)//至少已经测了10次
		{
			Average = GetU16DataAverage(CurrentBuffer, 10);//最近5次的平均值
			MaxMin = U16DataCMP(CurrentBuffer, 10);//最大最小差值
			if (Average < 100)
			{
				if(MaxMin < 10)//差值不能超过10uA
				{
					return SUCCESS;
				}			
			}
			else if (Average < 500)
			{//平均电流值小于500uA
				if(MaxMin < 15)//差值不能超过15uA
				{
					return SUCCESS;
				}
			}			
			else
			{//差值不能超过15uA
				if(MaxMin < 20)//差值不能超过20uA
				{
					return SUCCESS;
				}						
			}
		}		
	}
	return ERROR;//已经超时，电流还没有稳定
}

/**
  *************************************************************************************
  * @brief  根据当前的孔号查找内部没有注册的序号
  * @param  HoleName 当前孔号
  * @param  index 需要查找第几个未注册孔  
  * @retval 查找结果
  * @author ZH
  * @date   2015年03月26日
  * @note   需要提前把所有雷管按孔号从小到大重新排列
  *************************************************************************************  
  */
ErrorStatus GetCurrentHoleNameUnRegInside(u16 HoleName, u8 UnRegIndex, u8* Inside)
{
	u16 i;
	u8 Count = 0;
	u8 Buffer[NET_PARAM_MAX];
	
	memset(Buffer, 0x00, NET_PARAM_MAX);
	
	for(i=0; i < Det.RegCount; i++)
	{
		if(Det.Info[i].Hole.Name == HoleName)
		{
			Buffer[Det.Info[i].Hole.Inside-1] = 1;//表示已经注册
		}
		else if(Det.Info[i].Hole.Name > HoleName)
		{
			break;
		}
	}
	
	for(i=0; i < NET_PARAM_MAX; i++)
	{
		if(0x00 == Buffer[i])
		{//没有注册
			if(Count == UnRegIndex)
			{
				*Inside = i+1;
				return SUCCESS;
			}

			Count++;
		}
	}
	return ERROR;//没有找到	
}

/**
  *************************************************************************************
  * @brief  注册孔初始化
  * @param  Hole 注册孔
  * @retval 无
  * @author ZH
  * @date   2015年01月22日
  * @note   从后往前找到第一个未注册的孔，然后加1
  *************************************************************************************  
  */
u16 RegHoleInit(void)
{
    int i;
    u16 j;
    u16 index, UnRegIndex; 
    u32 Value1, Value2;
    u8 flag_bit;
    DET_INFO DetBuffer;
    HOLE Hole;
    
    if(0 == Det.RegCount)
    {//还没有注册雷管时，设置为默认孔内数量
        if(device_info.RegNetParamDefault >= NET_PARAM_MAX)
        {
            device_info.RegNetParamDefault = 0;
        }
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			Det.HoleNumber[i] = device_info.RegNetParamDefault+1;
		}    
    }
    else
    {
        AllRegDetRankBaseOnHoleInfo();
    }
        
    Hole.Name = 1;
    Hole.Inside = 1;
    UnRegIndex = Det.RegCount;
    while(UnRegIndex < DET_MAX_SIZE)
    {
        index = GetDetIndexFromHoleInfo(&Hole);
        if(index >= DET_MAX_SIZE)
        {//这个孔没有注册
            Det.Info[UnRegIndex].Hole.Name = Hole.Name;
            Det.Info[UnRegIndex].Hole.Inside = Hole.Inside;
            UnRegIndex++;
        }
        Hole.Inside++;
        if(Hole.Inside > Det.HoleNumber[Hole.Name-1])
        {
            Hole.Name++;
            Hole.Inside = 1;
        }
    }
    if(Det.RegCount < DET_MAX_SIZE)
    {
    	//冒泡排序，按孔号从小到大排列
        for(i=0; i < DET_MAX_SIZE-1; i++)
        {
            flag_bit = 1;
            for(j=0; j < DET_MAX_SIZE-i-1; j++)
            {
                Value1 = ((u32)Det.Info[j].Hole.Name<<16) + Det.Info[j].Hole.Inside;
                Value2 = ((u32)Det.Info[j+1].Hole.Name<<16) + Det.Info[j+1].Hole.Inside;
                if(Value1 > Value2)
                {
                    flag_bit = 0 ;
                    DetBuffer = Det.Info[j] ;
                    Det.Info[j] = Det.Info[j+1];
                    Det.Info[j+1]= DetBuffer ;
                }
            }
            if(flag_bit)//提前结束排序
            {
                break ;
            }		
        }
        
        //给没有注册的孔设置延期值
        for(i=0; i < DET_MAX_SIZE; i++)
        {
            if (GetDetRegStatus(i) == ERROR)
            {
                Det.Info[i].Delay = CalcRegHoleDelayFromNetParam(i);
            }
            if(Det.Info[i].Delay > Det.DlyMax)
            {
                Det.Info[i].Delay = Det.DlyMax;
            }
        }        
    }
    if(Det.RegCount == 0)
    {
        return 0;
    }
    else if(Det.RegCount == DET_MAX_SIZE)
    {
        return DET_MAX_SIZE-1;
    }
    //从后往前找到第一个未注册的孔，然后加1
    for(i=DET_MAX_SIZE-1; i >= 0; i--)
    {
        if (GetDetRegStatus(i) != ERROR)
        {//已注册
            index = i+1;
            if(index >= DET_MAX_SIZE)
            {
                index = DET_MAX_SIZE-1;
            }
            return index;
        }
    }
    return 0;
}

/**
  *************************************************************************************
  * @brief  根据当前的孔信息获取上一个待注册的孔信息
  * @param  Current 当前孔信息
  * @param  Next 上一个待注册的孔信息
  * @retval 获取结果 
  *    @arg @b ERROR 返回的孔，已经注册(也就是已经找到头了，都没有未注册的孔)
  *    @arg @b SUCCESS 返回的孔，还没有注册
  * @author ZH
  * @date   2015年01月22日
  * @note   返回的孔如果为1-1
  *************************************************************************************  
  */
ErrorStatus GetSmallRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Small)
{
	u16 i;
	HOLE NextHole;
	u32 ValueMin = 0xFFFFFFFF, Value;
	
	if(Det.RegCount >= DET_MAX_SIZE)
	{//当前已经注册满，返回当前最小孔号
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			Value = (((u32)Det.Info[i].Hole.Name)<<16) + Det.Info[i].Hole.Inside;//把孔号和孔内顺序号组合成一个长整形数，方便计算
			if(Value < ValueMin)
			{
				ValueMin = Value;
			}
		}
		Small->Name = (ValueMin>>16)&0xFF;
		Small->Inside = ValueMin&0xFF;			
	}
	else
	{
		NextHole = *Current;

		while(1)
		{
			//从当前孔往前找
			if(NextHole.Inside > 1)
			{
				NextHole.Inside--;
			}
			else
			{
				NextHole.Name--;
				NextHole.Inside = Det.HoleNumber[NextHole.Name-1];				
			}
			if(NextHole.Name < 1)
			{//没有找到
				Small->Name = 1;
				Small->Inside = 1;	
				return ERROR;
			}
			
			for(i=0; i < DET_MAX_SIZE; i++)
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if((Det.Info[i].Hole.Name == NextHole.Name) && ((Det.Info[i].Hole.Inside == NextHole.Inside)))
					{//该孔已经注册
						break;
					}
				}
			}
			if(DET_MAX_SIZE == i)
			{//该孔信息还没有注册
				*Small = NextHole;
				return SUCCESS;
			}
		}	
	}
	return ERROR;	
}

u16 GetRegDetIndexFirst(u16 Start)
{
    u16 i;
    
    for(i=Start; i<DET_MAX_SIZE; i++)
    {
        if(GetDetRegStatus(i) != ERROR)
        {
            return i;
        }
    }
    return 0xFFFF;
}
/**
  *************************************************************************************
  * @brief  根据当前的孔信息获取下一个待注册的孔信息
  * @param  Current 当前孔信息
  * @param  Next 下一个待注册的孔信息
  * @retval 获取结果 
  *    @arg @b ERROR 返回的孔，已经注册(也就是已经找到头了，都没有未注册的孔)
  *    @arg @b SUCCESS 返回的孔，还没有注册
  * @author ZH
  * @date   2014年11月17日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBigRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Big)
{
	u16 i;
	HOLE NextHole;
	u32 ValueMax = 0, Value;
	
	if(Det.RegCount >= DET_MAX_SIZE)
	{//当前已经注册满，返回当前最大孔号
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			Value = (((u32)Det.Info[i].Hole.Name)<<16) + Det.Info[i].Hole.Inside;//把孔号和孔内顺序号组合成一个长整形数，方便计算
			if(Value > ValueMax)
			{
				ValueMax = Value;
			}
		}
		Big->Name = (ValueMax>>16)&0xFF;
		Big->Inside = ValueMax&0xFF;			
	}
	else
	{
		NextHole = *Current;

		while(1)
		{
			//从当前孔往后找
			NextHole.Inside++;
			if(NextHole.Inside > Det.HoleNumber[NextHole.Name-1])
			{
				NextHole.Name++;
				NextHole.Inside = 1;
			}
			if(NextHole.Name > DET_MAX_SIZE)
			{//没有找到,返回最大的孔号
				Big->Name = DET_MAX_SIZE;
				Big->Inside = Det.HoleNumber[DET_MAX_SIZE-1];	
				return ERROR;	
			}
			
			for(i=0; i < DET_MAX_SIZE; i++)
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if((Det.Info[i].Hole.Name == NextHole.Name) && ((Det.Info[i].Hole.Inside == NextHole.Inside)))
					{//该孔已经注册
						break;
					}
				}
			}
			if(DET_MAX_SIZE == i)
			{//该孔信息还没有注册
				*Big = NextHole;
				return SUCCESS;
			}
		}	
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  根据当前的孔号统计已注册雷管数
  * @param  HoleName 当前孔号
  * @retval 同一个孔内已经注册的数量
  * @author ZH
  * @date   2014年11月17日
  * @note   
  *************************************************************************************  
  */
u8 GetCurrentHoleNameDetCount(u16 HoleName)
{
	u16 i;
	u8 Count = 0;;
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(Det.Info[i].Hole.Name == HoleName)
			{
				Count++;
			}
		}
	}
	return Count;
}
  
/**
  *************************************************************************************
  * @brief  检测起爆信号发送电路是否正常
  * @param  Slave 1 从机模式
  * @retval 无
  * @author ZH
  * @date   2016年05月25日
  * @note   
  *************************************************************************************  
  */
ErrorStatus BlastSingleSelfTest(u8 Slave)
{
    u8 i;
    s16 SignedValue;
    
    //作为译码器时的双键起爆电路							
    OPEN_BUS_6_7V();
    if(Slave)
    {
        OPEN_DOUBLE_KEY1_SIM_SW();
        OPEN_DOUBLE_KEY2_SIM_SW();    
    }

    OSTimeDly(2);
    for(i=0; i<10; i++)
    {
        BLAST_SIGNAL_TOGGLE();
        OSTimeDly(3);
        SignedValue = GetLinABAdcValue();
        BLAST_SIGNAL_TOGGLE();
        OSTimeDly(3);
        if(((SignedValue > 100) && (GetLinABAdcValue() < -100)) ||
            ((SignedValue < -100) && (GetLinABAdcValue() > 100)))
        {
            break;
        }
        OSTimeDlyHMSM(0,0,0,10);
    }
    if(Slave)
    {
        CLOSE_DOUBLE_KEY1_SIM_SW();
        CLOSE_DOUBLE_KEY2_SIM_SW();    
    }

    OPEN_BUS_6_7V();
    if(i == 10)
    {
        if(Slave)
        {
            WriteSystemLog("起爆电路(模拟双键)异常!",strlen("起爆电路(模拟双键)异常!"), NEW_STR_INFO);
        }
        else
        {
            WriteSystemLog("起爆电路异常!",strlen("起爆电路异常!"), NEW_STR_INFO);
        }
        
        return ERROR;
    }     
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  设备自检
  * @param  Type 自检项
  * @retval 自检结果
  * @author ZH
  * @date   2015年03月02日
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeviceSelfTest(u8 Type)
{
	u8 i;
	u16 Value;
	s16 SignedValue;

	
	if(Type&0x01)
	{//自检低压部分	
		for(i=0; i<10; i++)
		{
			Value = GetChannelxVoltage(ADC_CH_6_7V, 10);
			if (abs(Value -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT)
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}
		if(i == 10)
		{
			WriteSystemLog("自检6.7V异常,电压值:",strlen("自检6.7V异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);			
			return ERROR;
		}
		
		for(i=0; i<10; i++)
		{
			Value = GetChannelxVoltage(ADC_CH_5V, 10);
			if((Value > 4700) && (Value < 5300))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}			
		if(i == 10)
		{
			WriteSystemLog("自检5V异常,电压值:",strlen("自检5V异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);			
			return ERROR;
		}
		
		//设置COMM为5V
		OPEN_BUS_6_7V();
		SET_LV_5_0V();
		DET_COM_SEND_TOGGLE();//因为端口一边有下拉电阻，一边没有，让没有下拉电阻的有高电平
		for(i=0; i<10; i++)
		{
			Value = GetChannelxVoltage(ADC_CH_COMM, 10);
			if((Value > 4700) && (Value < 5300))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}
		DET_COM_SEND_TOGGLE();		
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检COMM 5V异常,电压值:",strlen("自检COMM 5V异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//设置COMM为6.7V
		SET_LV_6_7V();
		for(i=0; i<10; i++)
		{
			Value = GetChannelxVoltage(ADC_CH_COMM, 10);
			if (abs(Value -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT)
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}			
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检COMM 6.7V异常,电压值:",strlen("自检COMM 6.7V异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//自检端口5v电压
		OPEN_BUS_6_7V();
		SET_LV_5_0V();//切换到5V		
		for(i=0; i<10; i++)
		{
			Value = GetBusVoltage(10, NULL);
			if((Value > 4700) && (Value < 5300))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}			
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检端口5V电压异常,电压值:",strlen("自检端口5V电压异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//自检端口6.7v电压		
		OPEN_BUS_6_7V();
		for(i=0; i<10; i++)
		{
			Value = GetBusVoltage(10, NULL);
			if (abs(Value -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT)
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}			
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检端口6.7V电压异常,电压值:",strlen("自检端口6.7V电压异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		//自检高压按压部分电路
		for(i=0; i<10; i++)
		{
			Value = GetChannelxVoltage(ADC_CH_11_3V, 10);
			if(Value < 1000)
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}			
		if(i == 10)
		{
			WriteSystemLog("自检AD_+11.3V电压异常,电压值:",strlen("自检AD_+11.3V电压异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}		
		//自检通信电路
		SET_CMD_SEND_STATE();		
		OSTimeDlyHMSM(0,0,0,20);
		for(i=0; i<10; i++)
		{
			DET_COM_SEND_PIN_INIT();//发送高电平				
			OSTimeDly(2);
			SignedValue = GetLinABAdcValue();
			DET_COM_SEND_TOGGLE();//发送低电平		
			OSTimeDly(2);
			if(((SignedValue > 100) && (GetLinABAdcValue() < -100)) ||
				((SignedValue < -100) && (GetLinABAdcValue() > 100)))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,10);
		}
		DET_COM_SEND_PIN_INIT();
		SET_CMD_RECEIVE_STATE();//设置为接收状态
		SET_LV_6_7V();
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检通信电路异常!",strlen("自检通信电路异常!"), NEW_STR_INFO);			
			return ERROR;
		}
		
		//RTC自检
		RTC_Read_Time();
		Value = RTC_Type.second;
		for(i=0; i<10; i++)
		{
			OSTimeDlyHMSM(0,0,0,200);
			RTC_Read_Time();
			if(Value != RTC_Type.second)
			{
				break;
			}
		}
		if(i == 10)
		{
			WriteSystemLog("RTC异常!",strlen("RTC异常!"), NEW_STR_INFO);			
			return ERROR;
		}
		if(device_info.HasScan)
		{
            u8 Result;
            
            Result = ScanHardwareState();
			if(0 == Result)
			{
                Op_Data.ScanIsZEBRA = 0;
				WriteSystemLog("扫码模块异常!",strlen("扫码模块异常!"), NEW_STR_INFO);			
				return ERROR;		
			}
            else if(3 == Result)
            {
                Op_Data.ScanIsZEBRA = 1;
            }
            else
            {
                Op_Data.ScanIsZEBRA = 0;
            }
		}
//        if ((GetGPSPassCount() == 0) && (GetSatelliteCount() == 0))
//        {//定位成功次数大于0时，说明GPS通讯肯定是没问题的
//            u8 working;
//            u32 c_timeout;
//            
//            working = GPSIsWorking();
//            
//            //1秒钟之内都没有数据，说明通讯异常
//            c_timeout = OSTimeGet()+1000/(1000/OS_TICKS_PER_SEC);
//            while(OSTimeGet() < c_timeout)
//            {
//                if(!GPSIsWorking())
//                {
//                    ReceiveGPSInfoStart();
//                }
//                if((GPS_Buffer.Bytes > 0) && (GPS_Buffer.Buffer[0] == '$'))
//                {
//                    break;
//                }
//            }
//            if(!working)
//            {//恢复自检前的状态
//                ReceiveGPSInfoStop();
//            }
//            if(OSTimeGet() >= c_timeout)
//            {
//				WriteSystemLog("GPS模块异常!",strlen("GPS模块异常!"), NEW_STR_INFO);			
//				return ERROR;
//            }
//        }
		WriteSystemLog("自检低压部分正常",strlen("自检低压部分正常"), NEW_STR_INFO);
	}
	
	if(Type&0x02)
	{//自检高压部分
		OPEN_BUS_6_7V();
        Op_Data.LinNormalHighVoltageGoal = 12500;
        setDacHvValueMinFromVoltage(13000);//电容耐压只有16V
		if(ERROR ==CheckHVProtectPoint())
		{
			CLOSE_HV_SW_INSIDE();
			WriteSystemLog("自检高压电路异常!",strlen("自检高压电路异常!"), NEW_STR_INFO);			
			return ERROR;			
		}
		OPEN_HV_SW();//输出到总线
		for(i=0; i<10; i++)
		{
			Value = GetBusVoltage(10, NULL);
			if(abs(Op_Data.LinABVoltage - Op_Data.LinNormalHighVoltageGoal) < LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,200);
		}
		
		OPEN_BUS_6_7V();		
		if(i == 10)
		{			
			WriteSystemLog("继电器异常,电压值:",strlen("继电器异常,电压值:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
	
		WriteSystemLog("自检高压部分正常",strlen("自检高压部分正常"), NEW_STR_INFO);		
	}
	
	if(Type&0x04)
	{//自检起爆信号部分
		//作为一体化设备时的双键起爆电路
		OPEN_BUS_6_7V();
		OSTimeDlyHMSM(0,0,0,100);
		for(i=0; i<10; i++)
		{
			BLAST_SIGNAL_TOGGLE();
			OSTimeDly(2);
			SignedValue = GetLinABAdcValue();
			BLAST_SIGNAL_TOGGLE();
			OSTimeDly(2);
			if(((SignedValue > 100) && (GetLinABAdcValue() < -100)) ||
				((SignedValue < -100) && (GetLinABAdcValue() > 100)))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,10);
		}
		Op_Data.DoubleKeySwitch = 0;
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检起爆电路1异常!",strlen("自检起爆电路1异常!"), NEW_STR_INFO);			
			return ERROR;
		}
		
		//作为译码器时的双键起爆电路				
		OSTimeDlyHMSM(0,0,0,100);			
		OPEN_BUS_6_7V();
		if(ERROR == BlastSingleSelfTest(1))
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("自检起爆电路(模拟双键)异常!",strlen("自检起爆电路(模拟双键)异常!"), NEW_STR_INFO);			
			return ERROR;
		}
		WriteSystemLog("自检起爆信号部分正常",strlen("自检起爆信号部分正常"), NEW_STR_INFO);
	}
	
	if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		CLOSE_BUS_VOLTAGE();
	}
	else
	{
		OPEN_BUS_6_7V();	
	}

	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  LCD背光刷新流程
  * @param  RefreshFlag 0 不强制刷新，1表示强制刷新
  * @retval 无
  * @author ZH
  * @date   2016年05月25日
  * @note   
  *************************************************************************************  
  */
void LcdBrightRefreshProcess(u8 RefreshFlag)
{
	static u32 LcdBrightTimeOutValue = 0;
	
	LcdBrightLock();
	
	if(RefreshFlag)
	{//刷新背光状态
		if(device_info.LcdBrightTime == 0)
		{//背光常暗
			LcdSetBright(LCD_BRIGHT_RANK);		//屏幕亮度设到最暗
			LcdBrightTimeOutValue = 0xFFFFFFFF;//避免重复设置(屏幕会有一点点闪)
		}
		else if(device_info.LcdBrightTime >= 100)
		{//背光常亮
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);
			LcdBrightTimeOutValue = 0xFFFFFFFF;//避免重复设置(屏幕会有一点点闪)
		}
		else
		{//不是常亮也不是常暗
			LcdBrightTimeOutValue = OSTimeGet() + device_info.LcdBrightTime*10000UL/(1000/OS_TICKS_PER_SEC);  
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);				
		}	
	}
	else if(LcdBrightTimeOutValue != 0xFFFFFFFF)
	{	
		if(device_info.LcdBrightTime == 0)
		{//背光常暗
			LcdSetBright(LCD_BRIGHT_RANK);		//屏幕亮度设到最暗
			LcdBrightTimeOutValue = 0xFFFFFFFF;//避免重复设置(屏幕会有一点点闪)
		}
		else if(device_info.LcdBrightTime >= 100)
		{//背光常亮
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);
			LcdBrightTimeOutValue = 0xFFFFFFFF;//避免重复设置(屏幕会有一点点闪)
		}
		else
		{//不是常亮也不是常暗
			if(OSTimeGet() >= LcdBrightTimeOutValue)
			{
				LcdSetBright(LCD_BRIGHT_RANK);		//屏幕亮度设到最暗
				LcdBrightTimeOutValue = 0xFFFFFFFF;//避免重复设置(屏幕会有一点点闪)
			}					
		}		
	}
	LcdBrightUnlock();
}

void ClearProgbarData(void)
{
	memset((char*)&Op_Data.Progbar, 0, sizeof(Op_Data.Progbar));
}
void FillCurrentProgbar(void)
{
	Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value = 0xFFFF;
	UpdateProgbar(Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value);//更新进度条到100%
}
/**
  *************************************************************************************
  * @brief  起爆电压根据带载数量递增
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年11月06日
  * @note   起爆电压根据带载数量递增,数量为0时11V, 每多100发，+0.2V，400发时对应11.6V，
  * 公式：11.0+（（注册雷管数量-1）/100）*0.2
  *************************************************************************************  
  */
void SetNormalHighVoltageGoal(void)
{
    if(Op_Data.DelaySampleCfg)
    {
        Op_Data.LinNormalHighVoltageGoal = LIN_NORMAL_HIGH_VOLTAGE_FACTORY_GOAL;
    }
    else
    {
        Op_Data.LinNormalHighVoltageGoal = LIN_NORMAL_HIGH_VOLTAGE_GOAL;    
    }

    if ((Det.RegCount > 0) && (Det.RegCount <= DET_MAX_SIZE))
    {
        Op_Data.LinNormalHighVoltageGoal += 200*((Det.RegCount-1)/100);
    } 
    setDacHvValueMinFromVoltage(LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX);
}
/**
  *************************************************************************************
  * @brief  起爆器是否为山东圣世达
  * @param  无 
  * @retval 结果
  *      @arg 0 不是
  *      @arg 1 是
  * @date   2018年3月30日
  * @note   
  *************************************************************************************  
  */
u8 BlasterIs36(void)
{
	if(memcmp(device_info.mac, "F36", 3) == 0)
	{
		return 1;
	}
	else if(memcmp(device_info.mac, "f36", 3) == 0)
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  起爆器是否为河北卫星
  * @param  无 
  * @retval 结果
  *      @arg 0 不是
  *      @arg 1 是
  * @date   2018年5月6日
  * @note   
  *************************************************************************************  
  */
u8 BlasterIs04(void)
{
	if(memcmp(device_info.mac, "F04", 3) == 0)
	{
		return 1;
	}
	else if(memcmp(device_info.mac, "f04", 3) == 0)
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  根据起爆器编号获取工厂代码
  * @retval 结果
  *      @arg 0 未知
  *      @arg 其他 工厂代码
  * @date   2018年3月30日
  * @note   
  *************************************************************************************  
  */
u8 GetBlasterFactoryCode(void)
{
	if((device_info.mac[0] == 'F') || (device_info.mac[0] == 'f'))
	{
		if((device_info.mac[1] >= '0') && (device_info.mac[1] <= '9'))
        {
            if((device_info.mac[2] >= '0') && (device_info.mac[2] <= '9'))
            {
                u8 Fac;
                
                Fac = (device_info.mac[1]-'0')*10 + (device_info.mac[2]-'0');
                if ((Fac > 0) && (Fac <= 74))
                {
                    return Fac;
                }
            }
        }
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  雷管的工厂代码是否和起爆器的一致
  * @retval 结果
  *      @arg 0 不一至
  *      @arg 1 一致
  * @date   2018年3月30日
  * @note   
  *************************************************************************************  
  */
u8 DetFactoryCodeIsRulePass(u32 Serl, u32 Pwd)
{
    u8 DetCode, BlasterCode;

    DetCode = GetDetFactoryCode(Serl, Pwd);
    if(DetCode == 0)
    {
        return 1;
    }
    BlasterCode = GetBlasterFactoryCode();
    if(BlasterCode == 0)
    {
        return 1;
    }
    if(DetCode == BlasterCode)
    {
        return 1;
    }
    return 0;
}
u8 AllDetFactoryCodeIsRulePass(void)
{
    u8 DetCode, BlasterCode;
    u16 i;
    
    BlasterCode = GetBlasterFactoryCode();
    if(BlasterCode == 0)
    {
        return 1;
    }
    for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
            DetCode = GetDetFactoryCode(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
			if((DetCode != 0) && (DetCode != BlasterCode))
			{
				return 0;
			}
		}
	}
    return 1;
}
u8 AllRegDetCanInside2Outside(void)
{
	u16 i;
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(ERROR == XA3_DetInside2Outside(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value, NULL))
			{
				return 0;
			}
		}
	}
    return 1;
}

/**
  *************************************************************************************
  * @brief  获取在线雷管数量
  * @param  无 
  * @retval 数量
  * @date   2019年5月6日
  * @note   
  *************************************************************************************  
  */
u16 GetDetOnlineCount(void)
{
	u16 i;
	u16 Count = 0;
    
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if((Det.Info[i].Err&0x0F) != DET_TEST_CONNECT_ERR)
            {
                Count++;
            }
		}
	}
    return Count;
}

/**
  *************************************************************************************
  * @brief  通过起爆流程中总线电流各阶段的值，预测是否有部分雷管掉线。
  * @param  无 
  * @retval 0：没有，其他值： 有
  * @date   2019年5月29日
  * @note   在按下起爆双键后调用本函数
  *************************************************************************************  
  */
u8 DetOfflinePossible(u16 I_BeforeFire)
{
    u16 Count;
    
    //正常情况下，充电后电流应该不会比充电前电流和测试前电流小
    if ((I_BeforeFire < BlastInfo.I_AfterCharge) && 
        (abs(I_BeforeFire - BlastInfo.I_AfterCharge) > 500))
    {
        return 1;
    }
    
    if ((BlastInfo.I_AfterCharge < BlastInfo.I_BeforeCharge) && 
        (abs(BlastInfo.I_AfterCharge - BlastInfo.I_BeforeCharge) > 500))
    {
        return 2;
    }
    if ((BlastInfo.I_BeforeCharge < BlastInfo.I_BeforeTest) && 
        (abs(BlastInfo.I_BeforeCharge - BlastInfo.I_BeforeTest) > 500))
    {
        return 3;
    }    
    if ((BlastInfo.I_AfterCharge < BlastInfo.I_BeforeTest) && 
        (abs(BlastInfo.I_AfterCharge - BlastInfo.I_BeforeTest) > 500))
    {
        return 4;
    }
    
    //可以判断出总线掉了的情况。
    Count = GetDetOnlineCount();
    if((Count > 0) && ((BlastInfo.I_BeforeTest/Count) > 10))
    {//在线雷管数量大于0
        if ((BlastInfo.I_BeforeCharge/Count) < 10)
        {
            return 5;
        }
        if ((BlastInfo.I_AfterCharge/Count) < 10)
        {
            return 6;
        }
        if ((I_BeforeFire/Count) < 10)
        {
            return 7;
        }         
    }
    
    return 0;
}

/**
  *************************************************************************************
  * @brief  判断是不是有跳孔注册
  * @param  无 
  * @retval 0：没有，其他值： 有
  * @date   2019年5月29日
  * @note   主界面进入注册功能时，如果有跳孔雷管，给出提示
  *************************************************************************************  
  */
u8 HasDivideRegHole(void)
{
	u16 i;
	u8 Flag = 0;
    
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if(Flag)
            {
                return 1;
            }
		}
        else
        {
            Flag = 1;//找到未注册孔
        }
	}
    return 0;    
}
void DetCode2UIDStr(U32_UNION* Serl, U32_UNION* Pwd, u8* UIDStr)
{
    u8 i;
    u8 index = 0;
    
    //山东圣世达的雷管需要特殊处理
    if(XA3_DetInside2Outside(Serl->U32Value, Pwd->U32Value, UIDStr) != ERROR)
    {
        u16 Year;
        u32 Date;
        
        Year = UIDStr[2] - '0';
        if ((Year >= 5) && (Year <= 9))
        {
            Year += 2010;
        }
        else
        {
            Year += 2020;
        }
        Date = Year*10000 + ((UIDStr[3] - '0')*10+(UIDStr[4] - '0'))*100 + ((UIDStr[5] - '0')*10+(UIDStr[6] - '0'));
        
        if ((*UIDStr =='3') && (*(UIDStr+1) =='6'))
        {
            u8 Buffer[8];
            u16 Crc16;
            
            //方法：在管码前加一个'0'，凑成16进制字符串，计算CRC16
            Buffer[0] = HexCharToValue(*UIDStr);
            HexStrToCharStr(UIDStr+1, 12, &Buffer[1]);
            Crc16 = GetCrc16Value(0xFFFF, NULL, Buffer, 7);
            sprintf((char*)(UIDStr+13), "%02X%02X", Crc16&0xFF, (Crc16>>8)&0xFF);
            return;
        }
        else if ((*UIDStr =='0') && (*(UIDStr+1) =='7'))
        {//山西焦化
            u16 Crc16;
            
            Crc16 = GetCrc16Value(0xFFFF, NULL, UIDStr, 13);
            sprintf((char*)(UIDStr+13), "%02X%02X", Crc16&0xFF, (Crc16>>8)&0xFF);
            return;
        }
        if(Date >= 20191101)
        {//新UID规则  
            sprintf((char*)(UIDStr+2), "%02d", Year-2000);
            index = 4;
        }
    }
    for(i=0; i < 4; i++)
    {
        sprintf((char*)(UIDStr+index), "%02X", Serl->U8Buffer[i]);
        index += 2;
    }
    for(i=0; i < 4; i++)
    {
        sprintf((char*)(UIDStr+index), "%02X", Pwd->U8Buffer[i]);
        index += 2;
    }
}
u8 DetIsAccreditPassForDL(U32_UNION* Serl, U32_UNION* Pwd, u8* ErrorCode)
{
    u8 UIDStr[22];
    u8 YXQ[6];
    u8 U8Value;
    
    DetCode2UIDStr(Serl, Pwd, UIDStr);
    U8Value = GetAccreditDetInfoForDL(UIDStr, YXQ);
    if(U8Value == 0)
    {//已经找到
        if(YXQ[1] == 0)//月份为0,表示当前不是有效期
        {
            if(YXQ[0] == '2')//已使用过
            {
                return 1;
            }
            *ErrorCode = YXQ[0];//
        }
        else
        {//判断有效期
            char Buffer1[13];
            char Buffer2[13];

            sprintf(Buffer1, "%02d%02d%02d%02d%02d%02d", YXQ[0], YXQ[1], YXQ[2], 
                YXQ[3], YXQ[4], YXQ[5]);
                
            sprintf(Buffer2, "%02d%02d%02d%02d%02d%02d", RTC_Type.year%100, RTC_Type.month, RTC_Type.day, 
                RTC_Type.hour, RTC_Type.minute, RTC_Type.second);
                

            if(strcmp(Buffer1, Buffer2) >= 0)
            {
                return 1;
            }
            *ErrorCode = 0xF0;
        }
    }
    else if (U8Value == 0x02)
    {//授权格式不对
        *ErrorCode = 0xF1;
    }
    else if (U8Value == 0xFF)
    {//没有找到授权
        *ErrorCode = 0xF2;
    }    
    else
    {//授权文件有问题,调用本函数前已经确认过有授权文件
        *ErrorCode = 0xFF;
    }
    {
        char Buffuer[35];
        
        sprintf(Buffuer, "授权失败(%02X):%02X%02X%02X%02X%02X%02X%02X%02X", *ErrorCode, 
            Serl->U8Buffer[0], Serl->U8Buffer[1], Serl->U8Buffer[2], Serl->U8Buffer[3],
            Pwd->U8Buffer[0], Pwd->U8Buffer[1], Pwd->U8Buffer[2], Pwd->U8Buffer[3]);
            
        WriteSystemLog(Buffuer, strlen(Buffuer), NEW_STR_INFO);
    }
    return 0;
}
//授权验证
u16 AccreditOneDeviceForDL(u8 FileIndex)
{
	u16 i;
    u16 Count = 0;   
    u8 FileData[3];
    
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
            FileData[0] = i>>8;
            FileData[1] = i;
            
            if(!DetIsAccreditPassForDL(&Det.Info[i].Serl, &Det.Info[i].Pwd, &FileData[2]))
            {
                Count++;
                WriteAccreditErrorFile(FileIndex, FileData, 3);//保存错误信息到文件中 索引+错误代码
            }
            OSTimeDly(1);
		}
	}
    return Count;
}
/**
  *************************************************************************************
  * @brief  雷管授权验证(丹灵)
  * @param  无 
  * @retval 未授权数量
  * @date   2019年6月29日
  * @note   
  *************************************************************************************  
  */
u16 AllDetAccreditForDL(void)
{
    u16 Count = 0;
    u8 i;
    
    WriteSystemLog("开始离线授权验证", strlen("开始离线授权验证"), CURRENT_STATUS);
    
    DeleteAllAccreditErrorFile();
      
    if((strlen((char*)Op_Data.Gps.Longitude) == 0) || (strlen((char*)Op_Data.Gps.Latitude) == 0))
    {
        u8 FileBuffer[20];
        
        //从授权文件中读出经纬度
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("下载时间", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)&FileBuffer[0], "%04d", (int*)&Op_Data.Gps.Year);
        sscanf((char*)&FileBuffer[4], "%02d", (int*)&Op_Data.Gps.Month);
        sscanf((char*)&FileBuffer[6], "%02d", (int*)&Op_Data.Gps.Day);
        sscanf((char*)&FileBuffer[8], "%02d", (int*)&Op_Data.Gps.Hour);
        sscanf((char*)&FileBuffer[10], "%02d", (int*)&Op_Data.Gps.Minute);
        sscanf((char*)&FileBuffer[12], "%02d", (int*)&Op_Data.Gps.Second);
        
        memset(Op_Data.Gps.Longitude, 0, sizeof(Op_Data.Gps.Longitude));
        GetConfigFileString("下载点经度", Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);

        memset(Op_Data.Gps.Latitude, 0, sizeof(Op_Data.Gps.Latitude));
        GetConfigFileString("下载点纬度", Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);        
    }

    if(device_info.DeviceUsages == DEVICE_USAGES_ALONE)
    {
        return AccreditOneDeviceForDL(0x00);
    }
    else if(device_info.DeviceUsages == DEVICE_USAGES_MASTER)
    {
        for(i=0; i<SlaveInfo.Number; i++)
        {
            ReadSlaveDetInfo(i);
            Count += AccreditOneDeviceForDL(i);
        }
    }
    WriteSystemLog("离线授权验证结束", strlen("离线授权验证结束"), CURRENT_STATUS);
    return Count;
}

/**
  *************************************************************************************
  * @brief  获取起爆记录保存文件夹路径
  * @param  无 
  * @retval 路径
  * @date   2019年6月29日
  * @note   
  *************************************************************************************  
  */
char* GetBlasterPath(void)
{
    if(DEVICE_USAGES_SLAVE==device_info.DeviceUsages)
	{
		return SLAVE_BLASTER_RECORD_FILE_PATH;
	}
	else
	{
		switch(device_info.DeviceType)
        {
            case DEVICE_TYPE_SAFE_MONITOR://安管
                return BLASTER_RECORD_FILE_PATH_SAFE_MONITOR;
                
            case DEVICE_TYPE_SAFE_MONITOR_OFFLINE://离线模式
                return BLASTER_RECORD_FILE_PATH_OFFLINE;
                
            default://非安管
                return BLASTER_RECORD_FILE_PATH_NOT_SAFE_MONITOR;
        }
	}
}
/**
  *************************************************************************************
  * @brief  判断GPS定准数据是否已经失效
  * @param  无 
  * @retval 路径
  * @date   2019年8月3日
  * @note   有效时间为24小时
  *************************************************************************************  
  */
u8 GPSInfoTimeIsPass(GPS_INFO* Gps, RTC_Typedef* rtc)
{
    u8 t1[14];
    u8 t2[14];
    
    sprintf((char*)t1, "%04d%02d%02d%02d%02d%02d", 
        Gps->Year, Gps->Month, Gps->Day,
        Gps->Hour, Gps->Minute, Gps->Second);

    sprintf((char*)t2, "%04d%02d%02d%02d%02d%02d", 
        rtc->year, rtc->month, rtc->day,
        rtc->hour, rtc->minute, rtc->second);
        
    if(GetTimeDifferSec(t1, t2) < 86400L)//24小时内有效
    {
        return 1;
    }
    return 0;
}
/**
  *************************************************************************************
  * @brief  雷管信息保存的格式代码
  * @param  无 
  * @retval 0未知
  * @date   2019年9月25日
  * @note   
  *************************************************************************************  
  */
u8 FormatForDetInfo(u8* Version)
{
    if ((memcmp(Version, "MB05_", 5) == 0) || (memcmp(Version, "MQ03_", 5) == 0))
    {
        return 1;//VA芯片,6秒
    }
    if ((memcmp(Version, "MB02_", 5) == 0) || (memcmp(Version, "MQ02_", 5) == 0))
    {
        return 2;//XA02芯片
    }
    if ((memcmp(Version, "MB10_", 5) == 0) || (memcmp(Version, "MQ01_", 5) == 0))
    {
        return 3;//XA03芯片
    }
    return 0;//未知
}

/**
  *************************************************************************************
  * @brief  该版本号的程序和本版本，雷管信息格式是否兼容
  * @param  无 
  * @retval 0未知
  * @date   2019年9月25日
  * @note   
  *************************************************************************************  
  */
u8 FormatIsSameForDetInfo(u8* Version)
{
    u8 Format;
    
    if(memcmp(Version, VERSION_STRING, 5) == 0)
    {
        return 1;
    }
    
    Format = FormatForDetInfo(VERSION_STRING);
    if((Format != 0) && (Format == FormatForDetInfo(Version)))
    {
        return 2;
    }
    return 0;
}
/**
  *************************************************************************************
  * @brief  检查软件版本与硬件版本是否匹配
  * @param  无 
  * @retval 0 不匹配， 1 匹配，2 未知
  * @date   2020年2月28日
  * @note   
  *************************************************************************************  
  */
u8 CheckHardwareVersion(void)
{
    u8 i;
    u8 len;
    
    len = strlen((const char*)device_info.hw_version);
    if(len > 4)
    {
        len -= 4;
        for(i=0; i<=len; i++)
        {
            //BLA_V3.21_1902A
            //blaster_V1.73
            if((device_info.hw_version[i] == '_') && (device_info.hw_version[i+1] == 'V') && (device_info.hw_version[i+3] == '.'))
            {
                if(device_info.hw_version[i+2] == '3') 
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
        }    
    }

    return 2;
}
/*
STM32每个系列都会有唯一的一个芯片序列号（96位bit, 也就是12字节）：
STM32F10X 的地址是 0x1FFFF7E8
STM32F20X 的地址是 0x1FFF7A10
STM32F30X 的地址是 0x1FFFF7AC
STM32F40X 的地址是 0x1FFF7A10
STM32L1XX 的地址是 0x1FF80050
*/
void GetSTM32ChipID(u8* Id)
{
    const u32 Addr = 0x1FFF7A10;
    u8 i;
    
    for(i=0; i<12; i++)
    {
        *(Id+i) = *(__IO u8*)(Addr+i);
    }
}
void GetSTM32ChipIDStr(u8* IdStr)
{
    const u32 Addr = 0x1FFF7A10;
    u8 i;
    
    for(i=0; i<12; i++)
    {
        sprintf((char*)(IdStr+i*2), "%02X", *(__IO u8*)(Addr+i));
    }
}


