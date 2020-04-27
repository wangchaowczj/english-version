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
  * @brief  UFT8�ַ����ַ�����
  * @param[in]  Str��UFT8�ַ���
  * @retval �ַ�����
  * @author ZH
  * @date   2014��9��15��
  * @note   UFT8�ַ���Ӣ�ĵ�1�ֽڣ�����ռ3�ֽ�
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
//| �������� | U32DataCMP 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �Ƚ�ָ���������е�����(32λ����),����������ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr:��Ҫ�Ƚϵ����ݻ�������ַ
//|          | length:��Ҫ�Ƚϵ����ݻ���������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ������������������ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u32 U32DataCMP(const u32* ptr, u8 length)
{
	u32 max_value, min_value ;
	u8 i ;

	max_value = *ptr; //���ֵ
	min_value = *ptr; //��Сֵ
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
	return (max_value - min_value);	//�������ֵ
}

//==================================================================================================
//| �������� | U16DataCMP 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �Ƚ�ָ���������е�����(16λ����),����������ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr:��Ҫ�Ƚϵ����ݻ�������ַ
//|          | length:��Ҫ�Ƚϵ����ݻ���������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ������������������ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 U16DataCMP(const u16* ptr, u8 length)
{
	u16 max_value, min_value ;
	u8 i ;

	max_value = *ptr; //���ֵ
	min_value = *ptr; //��Сֵ
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
	return (max_value - min_value);	//�������ֵ
}

//==================================================================================================
//| �������� | GetU16DataMax 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��һ������(16λ)�����ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ�룬ָҪ��ƽ��ֵ������
//|          | lenght���ݸ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ���ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetU16DataMax(const u16* ptr, u8 length)
{
	u16 max_value =0;
	u8 i ;

	max_value = *ptr; //���ֵ
	for(i=1;i<length;i++)
	{
		if(*(ptr+i)>max_value)
		{
			max_value = *(ptr+i) ;
		}
	}
	return max_value ;	//�������ֵ
}

//==================================================================================================
//| �������� | GetU16DataMin 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��һ������(16λ)����Сֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ�룬ָҪ��ƽ��ֵ������
//|          | lenght���ݸ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��Сֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetU16DataMin(const u16* ptr, u8 length)
{
	u16 min_value = 0 ;
	u8 i ;

	min_value = *ptr; //��Сֵ
	for(i=1;i<length;i++)
	{
		if(*(ptr+i) < min_value)
		{
			min_value = *(ptr+i) ;
		}
	}
	return min_value ;	//������Сֵ
}

//==================================================================================================
//| �������� | GetU16DataAverage 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ������(16λ)��ƽ��ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr ָ�룬ָҪ��ƽ��ֵ������
//|          | lenght���ݸ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ƽ��ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | FindUnregHole 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������������ò�ͬ�ķ�������δע���׹ܵĿ׺�
//|----------|--------------------------------------------------------------------------------------
//| ������� | current ��ǰ���ڵĿ�����
//|          | ���ҷ�ʽ 0 �Ӻ���ǰ����������1��δע��׺�ֹͣ
//|          |          1 �Ե�ǰ��Ϊ��㣬��ǰ����δע���
//|          |          2 �Ե�ǰ��Ϊ��㣬������δע���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 FindUnregHole(u16 current, u8 dir)
{
	s16 i;
	
	switch(dir)
	{
		case 0:
			for(i=DET_MAX_SIZE -1; i>=0; i--) //�ӿ׺Ŵ�����׺�С�ķ������
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
			for(i=DET_MAX_SIZE -1; i>=0; i--) //�Ե�ǰ��Ϊ��㣬��ǰ����δע���
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
			for(i=0; i<=DET_MAX_SIZE -1; i++) //�Ե�ǰ��Ϊ��㣬������δע���
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
//| �������� | U16DataFallRank 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ������(16λ)�Ӵ�С��˳������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr_s ָ�룬ָ������ǰ������
//|          | ptr_d ָ�룬ָ������������
//|          | lenghtҪ��������ݸ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
}

//==================================================================================================
//| �������� | RemoveCharFromString 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ɾ���ַ����е�ָ���ַ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr  ָ�룬ָ��ɾ��ǰ�Ļ�����
//|          | char_val Ҫɾ�����ַ�
//|          | lenght    ɾ��ǰ�ĳ���
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | HexCharToValue 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �ѵ���16�����ַ�ת��Ϊ��ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | c Ҫת�����ַ�
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ת�����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | HexStrToCharStr 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��16�����ַ���ת��Ϊ��ͨ�ַ���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ptr_in  ָ�룬ָ��ת��ǰ�Ļ�����
//|          | in_len  Ҫת������������
//|          | ptr_out ת���õ������ݻ�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ת�����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-11-02 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | in_lenΪ2������������ת������"414243"ת����Ϊ"ABC"
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
  * @brief  ����ַ����ǲ�����ֵ
  * @param  Str:  �ַ���
  * @param  FloatFlag:  �Ƿ�֧�ָ���  
  * @retval ���
  * @author ZH
  * @date   2015��7��14��
  * @note   ��
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
		temp[i--] = value%10 + '0'; //��λת�������õ���洢�ַ���ʡȥ�˺������ַ��ߵ�����
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
  * @brief  ��������ע����׹ܰ��׺Ŵ�С������������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
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
		//�������Ѿ�ע����׹���ǰ��,ȷ��������ע����׹ܶ��Ǳ�������ǰ��
		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{//�Ѹÿ�������ǰ��
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
	
	//ð�����򣬰��׺Ŵ�С��������
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
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  ��������ע����׹ܰ����ڴ�С������������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��10��30��
  * @note   ��
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
		//�������Ѿ�ע����׹���ǰ��,ȷ��������ע����׹ܶ��Ǳ�������ǰ��
		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
		{
			if (GetDetRegStatus(i) != ERROR)
			{//�Ѹÿ�������ǰ��
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
	
	//ð�����򣬰��׺Ŵ�С��������
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
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  ��������ע����׹ܰ�����Ӵ�С��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��02��25��
  * @note   ��
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
//		//�������Ѿ�ע����׹���ǰ��,ȷ��������ע����׹ܶ��Ǳ�������ǰ��
//		for(i=DET_MAX_SIZE-1; i >= Det.RegCount; i--)
//		{
//			if (GetDetRegStatus(i) != ERROR)
//			{//�Ѹÿ�������ǰ��
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
//	//ð�����򣬰�����Ӵ�С����
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
//			 {//Temp1 С�� Temp2
//				 flag_bit = 0 ;
//				 DetBuffer = Det.Info[j] ;
//				 Det.Info[j] = Det.Info[j+1];
//				 Det.Info[j+1]= DetBuffer ;
//			 }
//		}
//		if(flag_bit)//��ǰ��������
//		{
//			break ;
//		}		
//	}	
//}
/**
  *************************************************************************************
  * @brief  ��������Ŀ���Ϣ��������ֵ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��11��17��
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
  * @brief  ��������Ŀ���Ϣ���Ҹ����׺ŵ�����ֵ(�����ҿ׺ű���С��)
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��11��15��
  * @note   ���б�֤��ע���׹ܣ�
  *************************************************************************************  
  */
u16 GetNearbyDetIndexFromHoleInfo(HOLE* Hole)
{
	u16 i;
	u32 Value1, Value2;
    
    AllRegDetRankBaseOnHoleInfo();//�������׹ܰ��׺Ŵ�С������������
    
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
  * @brief  ��ȡһ��û��ע����׹�����ֵ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��11��17��
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
//| �������� | GetErrorDetIndex 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������Ĵ����׹��б�����ֵ���Ҷ�Ӧ���׹��б�������
//|----------|--------------------------------------------------------------------------------------
//| ������� | err_index �����׹��б��е�����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | �׹��б��е�����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 GetErrorDetIndex(u16 err_index)
{
	u16 i, err_num = 0;
	
	AllRegDetRankBaseOnHoleInfo();//�������׹ܰ��׺Ŵ�С������������
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
  * @brief  ��������Ŀ���Ϣ�ҳ���ע���������С�������
  * @param  Hole   Ҫ���ҵ�����Ϣ
  * @param  MaxInside ��ע������������
  * @param  MinInside ��ע�����С�������  
  * @retval ���ҽ��
  * @author ZH
  * @date   2015��03��26��
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
		{//��ע��
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
  * @brief  ��������Ŀ���Ϣ�������ڽ��Ŀ������(��ע�ᣩ
  * @param  Hole   Ҫ���ҵ�����Ϣ
  * @param  Inside ���ҵ������ڽ��Ŀ������(��ע�ᣩ
  * @retval ���ҽ��
  * @author ZH
  * @date   2015��03��26��
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
		{//��ע��
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
  * @brief  ��ȡ��ע������׺�
  * @param  Type �Լ���
  * @retval �Լ���
  * @author ZH
  * @date   2018��11��02��
  * @note   ȷ����ע���׹�ʱ����
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
  * @brief  ����Ƿ�����ͬ����
  * @param  Type �Լ���
  * @retval �Լ���
  * @author ZH
  * @date   2015��03��02��
  * @note   
  *************************************************************************************  
  */
u8 DetInfoHasSame(void)
{
	u16 i, j;

	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//�Ѿ�ע��
			for(j=i+1; j < DET_MAX_SIZE; j++)
			{
				if (GetDetRegStatus(j) != ERROR)
				{//�Ѿ�ע��
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
  * @brief  ��������Ŀ׺ż���������Ϊ1���׹�����
  * @param  HoleName �׺�
  * @retval �������Ϊ1���׹�����
  * @author ZH
  * @date   2015��03��27��
  * @note   �׺�N-1������=M-1������+�׼����ڣ�N=M+1,����һ���׵�������ǰһ���׵õ�
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
		{//�Ѿ�ע��
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
		//û���ҵ�N-1���Ѿ�ע�����׹�
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
  * @brief  �����������ʼ/��ֹ����Ϣ����ͳ��ע���׹���
  * @param  ��
  * @retval ע���׹���
  * @author ZH
  * @date   2016��11��28��
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
		{//�Ѿ�ע��
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
  * @brief  �����������ʼ����Ϣ��ȡ��N��ע��׶�Ӧ�Ŀ�����
  * @param  StartHole ��ʼ��
  * @param  Pos ��ʼ������ڼ�������1��ʼ
  * @retval �ҵ��Ŀ�����
  * @author ZH
  * @date   2016��11��28��
  * @note   ��Ҫ��ǰ�ѿ׺Ž��д�С��������
  *************************************************************************************  
  */
u16 GetHoleIndexSpecifiedStartHole(HOLE* StartHole, u16 Pos)
{
	u16 i;
	u16 Count=0;
	
	for(i=0; i <= DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{//�Ѿ�ע��
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
  * @brief  ��������Ŀ���Ϣ�����Ӧ������ֵ
  * @param  ��
  * @retval ����ֵ
  * @author ZH
  * @date   2019��3��29��
  * @note   �����������ֵ�����׹ܵ��������ʱ����0xFFFF
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
  * @brief  ������������ڲ��������ж�����ֵ�Ƿ���
  * @param  ��
  * @retval ErrorStatus
  *         @arg @b ERROR ���ڳ���
  *         @arg @b SUCCESS ����û�г��� 
  * @author ZH
  * @date   2014��11��17��
  * @note   ֻ�����Ƿ��ޣ����޸�����ֵ
  *************************************************************************************  
  */
ErrorStatus DelayParamVerify(HOLE* StartHole, HOLE* EndHole, u16 StartDly, u16 SpacesDly, u16 InsideSpacesDly)
{
	u16 i;
	int64_t Dly, FirstHoleDlg, Value, StartValue, EndValue;
	
	StartValue = ((int64_t)StartHole->Name<<16) + StartHole->Inside;
	EndValue = ((int64_t)EndHole->Name<<16) + EndHole->Inside;
	
	//�ȼ����1���ף��������Ϊ1��Ӧ������ֵ,�п����Ǹ���
	FirstHoleDlg = (int64_t)StartDly - (int64_t)InsideSpacesDly*((StartValue&0xFFFF)-1);
	
	if(StartValue <= EndValue)
	{//����
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
	{//����
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
  * @brief  ������������ڲ����޸��׹�����ֵ
  * @param  ��
  * @retval ErrorStatus
  *         @rag @b ERROR ���ڳ���
  *         @rag @b SUCCESS ����û�г���  
  * @author ZH
  * @date   2014��11��17��
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
	
	//�ȼ����1���ף��������Ϊ1��Ӧ������ֵ,�п����Ǹ���
	FirstHoleDlg = (int64_t)StartDly - (int64_t)InsideSpacesDly*((StartValue&0xFFFF)-1);
		
	if(StartValue <= EndValue)
	{//����
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
	{//����
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
  * @brief  ������ע���׹ܵ�����ֵ
  * @param  ����ֵ
  * @retval ��
  * @author ZH
  * @date   2016��8��15��
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
  * @brief  ������������ڲ��������ж�����ֵ�Ƿ���
  * @param  ��
  * @retval ErrorStatus
  *         @rag @b ERROR ���ڳ���
  *         @rag @b SUCCESS ����û�г���  
  * @author ZH
  * @date   2014��11��17��
  * @note   Ϊ�˱����������⣬���ص�����ֵΪ���׺Ŵ�С������ֵ
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
		return 0xFFFF;//û���ҵ�
	}
	
	AllRegDetRankBaseOnHoleInfo();//��������ע��װ���С����ĳ�����������
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
	return 0xFFFF;//û���ҵ�
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
//| �������� | ValidateDetTypeAndDelay 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ȷ���׹����ͼ������Ƿ�һ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-11-21 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | �ڵ��ñ�����ǰ������ȶ�ȡ���ò���
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
//| �������� | GetDetMaxDelay 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����׹����ͻ�ȡ��Ӧ���������ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | �׹�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��Ӧ���������ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-09-23 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
  * @brief  ����״̬�Ƿ����
  * @param  ��   
  * @retval ����״̬
  *         @arg @b ERROR ������
  *         @arg @b SUCCESS ���� 
  * @author ZH
  * @date   2014��11��20��
  * @note   ֻ����6.7V��11.3V״̬�²���Ϊ���У�5V���߹ر�����ʱ������
  *************************************************************************************  
  */
ErrorStatus BusStatusIsIdle(void)
{
	if((DET_END == DetProtocol.SendBuffer.State) && (GET_DET_RECEIVE_RES_STATE() == DET_RECEIVE_RES_ON))
	{
		if (0 != GET_LINAB_SHORT_SW_CTRL_STATE())
		{//��ǰ���ߵ�ѹ��û�йر�
			if(GET_HV_SW_CTRL_STATE() == 0)
			{//��ǰΪ��ѹ״̬
				if ((0 == GET_LV_5V_SW_STATE()) && (0 != GET_LV_6_7V_SW_STATE()))
				{//��ǰû���л�����5V״̬
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
  * @brief  ����״̬���
  * @param  Voltage ���ߵ�ѹֵ
  * @param  Current ���ߵ���ֵ   
  * @retval ����״̬
  *         @arg @b 0 ��������
  *         @arg @b 1 ���߶�·
  *         @arg @b 2 ���ߵ�ѹ�쳣  
  * @author ZH
  * @date   2014��11��20��
  * @note   
  *************************************************************************************  
  */
u8 BusStateMonitor(u16 Voltage, u16 Current)
{
	static u8 VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
	static u8 ShortCount = 0;//���߶�·������

	if ((Op_Data.BusMonitorDisable) || (Op_Data.AdjustStatus != 0))
	{
		VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
		ShortCount = 0;//���߶�·������		
	}
	else if(ERROR == BusStatusIsIdle())
	{//�����׹�ͨ�š�5V���߹ر�����
		VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
		ShortCount = 0;//���߶�·������	
	}
	else if ((0 == GET_LINAB_SHORT_SW_CTRL_STATE()) || (OSTimeGet() < Op_Data.LinAdjustTime))
	{//��ǰ���ߵ�ѹ�Ѿ��رա���ǰ�����仯̫�󣬽�ֹ������ѹ
		VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
		ShortCount = 0;//���߶�·������
	}
	else 
	{
		if (BusIsShort())
		{	
			ShortCount++;
			if(ShortCount>20)//����10��
			{
				if(Op_Data.BusMonitorCfg == 0)
				{
					CLOSE_BUS_VOLTAGE();
				}
				
				return 1;//��������15000uA����Ϊ���߶�·		
			}            
		}
		else
		{
			ShortCount = 0;//���߶�·����ֵ���
		}
		if(GET_HV_SW_CTRL_STATE() == 0)
		{//��ǰΪ��ѹ״̬
			if ((0 == GET_LV_5V_SW_STATE()) && (0 != GET_LV_6_7V_SW_STATE()))
			{//��ǰû���л�����5V״̬
				if (abs(Voltage -  Op_Data.LinNormalLowVoltageGoal) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
				{
					VoltageErrorCount++;//���ߵ�ѹ�쳣������
				}
				else
				{
					VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
				}				
			}
			else
			{
				VoltageErrorCount = 0;//��ǰΪ5V״̬��������ѹ
			}
		}
		else
		{//��ѹ�Ѿ����
			if (abs(Voltage -  Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
            {
                VoltageErrorCount++;//���ߵ�ѹ�쳣������
            }
            else
            {
                VoltageErrorCount = 0;//���ߵ�ѹ�쳣������
            }			
		}
		if(VoltageErrorCount >= 25)//�ȵ�������Ҫ�࣬�����߶�·ʱ������ʾ��ѹ�쳣
		{
			if(Op_Data.BusMonitorCfg == 0)
			{
				CLOSE_BUS_VOLTAGE();			
			}
			return 2;//��ѹ�쳣
		}
	}
    if ((ShortCount == 6) || (ShortCount == 10) || (VoltageErrorCount == 6) || (VoltageErrorCount == 10))
    {//3�롢5��
        WriteSystemLog("��ǰ����״̬", 0, CURRENT_STATUS);
    }

	return 0;//��������
}
//==================================================================================================
//| �������� | CheckDetDelayIsSame 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �ж��Ƿ������׹ܵ�����ֵ����ͬ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��ͬ������ֵ��0xFFFF��ʾ����ֵ����ͬ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2014-07-17
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | û��ע���׹ܿ���Ϊ�׹����ڲ���ͬ
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u16 CheckDetDelayIsSame(void)
{
	u16 i, Delay=0xFFFF;
	
	if(Det.RegCount <= 0x0001)
	{
		return 0xFFFF;//û��ע���׹ܻ�ֻ��1���׹ܿ���Ϊ�׹����ڲ���ͬ
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
  * @brief  �ȴ����ߵ����ȶ�
  * @param  TimeOut �ȴ���ʱʱ��
  * @retval �ȴ����
  *         @arg @b SUCCESS �����ȶ�
  *         @arg @b ERROR �Ѿ���ʱ
  * @author ZH
  * @date   2015��01��17��
  * @note   
  *************************************************************************************  
  */
ErrorStatus WaiLinCurrentSteady(u32 TimeOut)
{	
	u16 CurrentBuffer[10];
	u8 Count = 0, Flag = 0;
	u16 Average, MaxMin;
	u32 c_time;
	
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
	    OSTimeDlyHMSM(0, 0, 0, 30);
		if(Count >= 10)
		{
			Flag = 1;
			Count %= 10;
		}
		
		CurrentBuffer[Count++] = GetBusCurrent(10, NULL);	
		
		if(Flag != 0)//�����Ѿ�����10��
		{
			Average = GetU16DataAverage(CurrentBuffer, 10);//���5�ε�ƽ��ֵ
			MaxMin = U16DataCMP(CurrentBuffer, 10);//�����С��ֵ
			if (Average < 100)
			{
				if(MaxMin < 10)//��ֵ���ܳ���10uA
				{
					return SUCCESS;
				}			
			}
			else if (Average < 500)
			{//ƽ������ֵС��500uA
				if(MaxMin < 15)//��ֵ���ܳ���15uA
				{
					return SUCCESS;
				}
			}			
			else
			{//��ֵ���ܳ���15uA
				if(MaxMin < 20)//��ֵ���ܳ���20uA
				{
					return SUCCESS;
				}						
			}
		}		
	}
	return ERROR;//�Ѿ���ʱ��������û���ȶ�
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ�Ŀ׺Ų����ڲ�û��ע������
  * @param  HoleName ��ǰ�׺�
  * @param  index ��Ҫ���ҵڼ���δע���  
  * @retval ���ҽ��
  * @author ZH
  * @date   2015��03��26��
  * @note   ��Ҫ��ǰ�������׹ܰ��׺Ŵ�С������������
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
			Buffer[Det.Info[i].Hole.Inside-1] = 1;//��ʾ�Ѿ�ע��
		}
		else if(Det.Info[i].Hole.Name > HoleName)
		{
			break;
		}
	}
	
	for(i=0; i < NET_PARAM_MAX; i++)
	{
		if(0x00 == Buffer[i])
		{//û��ע��
			if(Count == UnRegIndex)
			{
				*Inside = i+1;
				return SUCCESS;
			}

			Count++;
		}
	}
	return ERROR;//û���ҵ�	
}

/**
  *************************************************************************************
  * @brief  ע��׳�ʼ��
  * @param  Hole ע���
  * @retval ��
  * @author ZH
  * @date   2015��01��22��
  * @note   �Ӻ���ǰ�ҵ���һ��δע��Ŀף�Ȼ���1
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
    {//��û��ע���׹�ʱ������ΪĬ�Ͽ�������
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
        {//�����û��ע��
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
    	//ð�����򣬰��׺Ŵ�С��������
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
            if(flag_bit)//��ǰ��������
            {
                break ;
            }		
        }
        
        //��û��ע��Ŀ���������ֵ
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
    //�Ӻ���ǰ�ҵ���һ��δע��Ŀף�Ȼ���1
    for(i=DET_MAX_SIZE-1; i >= 0; i--)
    {
        if (GetDetRegStatus(i) != ERROR)
        {//��ע��
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
  * @brief  ���ݵ�ǰ�Ŀ���Ϣ��ȡ��һ����ע��Ŀ���Ϣ
  * @param  Current ��ǰ����Ϣ
  * @param  Next ��һ����ע��Ŀ���Ϣ
  * @retval ��ȡ��� 
  *    @arg @b ERROR ���صĿף��Ѿ�ע��(Ҳ�����Ѿ��ҵ�ͷ�ˣ���û��δע��Ŀ�)
  *    @arg @b SUCCESS ���صĿף���û��ע��
  * @author ZH
  * @date   2015��01��22��
  * @note   ���صĿ����Ϊ1-1
  *************************************************************************************  
  */
ErrorStatus GetSmallRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Small)
{
	u16 i;
	HOLE NextHole;
	u32 ValueMin = 0xFFFFFFFF, Value;
	
	if(Det.RegCount >= DET_MAX_SIZE)
	{//��ǰ�Ѿ�ע���������ص�ǰ��С�׺�
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			Value = (((u32)Det.Info[i].Hole.Name)<<16) + Det.Info[i].Hole.Inside;//�ѿ׺źͿ���˳�����ϳ�һ�������������������
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
			//�ӵ�ǰ����ǰ��
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
			{//û���ҵ�
				Small->Name = 1;
				Small->Inside = 1;	
				return ERROR;
			}
			
			for(i=0; i < DET_MAX_SIZE; i++)
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if((Det.Info[i].Hole.Name == NextHole.Name) && ((Det.Info[i].Hole.Inside == NextHole.Inside)))
					{//�ÿ��Ѿ�ע��
						break;
					}
				}
			}
			if(DET_MAX_SIZE == i)
			{//�ÿ���Ϣ��û��ע��
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
  * @brief  ���ݵ�ǰ�Ŀ���Ϣ��ȡ��һ����ע��Ŀ���Ϣ
  * @param  Current ��ǰ����Ϣ
  * @param  Next ��һ����ע��Ŀ���Ϣ
  * @retval ��ȡ��� 
  *    @arg @b ERROR ���صĿף��Ѿ�ע��(Ҳ�����Ѿ��ҵ�ͷ�ˣ���û��δע��Ŀ�)
  *    @arg @b SUCCESS ���صĿף���û��ע��
  * @author ZH
  * @date   2014��11��17��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBigRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Big)
{
	u16 i;
	HOLE NextHole;
	u32 ValueMax = 0, Value;
	
	if(Det.RegCount >= DET_MAX_SIZE)
	{//��ǰ�Ѿ�ע���������ص�ǰ���׺�
		for(i=0; i < DET_MAX_SIZE; i++)
		{
			Value = (((u32)Det.Info[i].Hole.Name)<<16) + Det.Info[i].Hole.Inside;//�ѿ׺źͿ���˳�����ϳ�һ�������������������
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
			//�ӵ�ǰ��������
			NextHole.Inside++;
			if(NextHole.Inside > Det.HoleNumber[NextHole.Name-1])
			{
				NextHole.Name++;
				NextHole.Inside = 1;
			}
			if(NextHole.Name > DET_MAX_SIZE)
			{//û���ҵ�,�������Ŀ׺�
				Big->Name = DET_MAX_SIZE;
				Big->Inside = Det.HoleNumber[DET_MAX_SIZE-1];	
				return ERROR;	
			}
			
			for(i=0; i < DET_MAX_SIZE; i++)
			{
				if (GetDetRegStatus(i) != ERROR)
				{
					if((Det.Info[i].Hole.Name == NextHole.Name) && ((Det.Info[i].Hole.Inside == NextHole.Inside)))
					{//�ÿ��Ѿ�ע��
						break;
					}
				}
			}
			if(DET_MAX_SIZE == i)
			{//�ÿ���Ϣ��û��ע��
				*Big = NextHole;
				return SUCCESS;
			}
		}	
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ�Ŀ׺�ͳ����ע���׹���
  * @param  HoleName ��ǰ�׺�
  * @retval ͬһ�������Ѿ�ע�������
  * @author ZH
  * @date   2014��11��17��
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
  * @brief  ������źŷ��͵�·�Ƿ�����
  * @param  Slave 1 �ӻ�ģʽ
  * @retval ��
  * @author ZH
  * @date   2016��05��25��
  * @note   
  *************************************************************************************  
  */
ErrorStatus BlastSingleSelfTest(u8 Slave)
{
    u8 i;
    s16 SignedValue;
    
    //��Ϊ������ʱ��˫���𱬵�·							
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
            WriteSystemLog("�𱬵�·(ģ��˫��)�쳣!",strlen("�𱬵�·(ģ��˫��)�쳣!"), NEW_STR_INFO);
        }
        else
        {
            WriteSystemLog("�𱬵�·�쳣!",strlen("�𱬵�·�쳣!"), NEW_STR_INFO);
        }
        
        return ERROR;
    }     
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  �豸�Լ�
  * @param  Type �Լ���
  * @retval �Լ���
  * @author ZH
  * @date   2015��03��02��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeviceSelfTest(u8 Type)
{
	u8 i;
	u16 Value;
	s16 SignedValue;

	
	if(Type&0x01)
	{//�Լ��ѹ����	
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
			WriteSystemLog("�Լ�6.7V�쳣,��ѹֵ:",strlen("�Լ�6.7V�쳣,��ѹֵ:"), NEW_STR_INFO);
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
			WriteSystemLog("�Լ�5V�쳣,��ѹֵ:",strlen("�Լ�5V�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);			
			return ERROR;
		}
		
		//����COMMΪ5V
		OPEN_BUS_6_7V();
		SET_LV_5_0V();
		DET_COM_SEND_TOGGLE();//��Ϊ�˿�һ�����������裬һ��û�У���û������������иߵ�ƽ
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
			WriteSystemLog("�Լ�COMM 5V�쳣,��ѹֵ:",strlen("�Լ�COMM 5V�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//����COMMΪ6.7V
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
			WriteSystemLog("�Լ�COMM 6.7V�쳣,��ѹֵ:",strlen("�Լ�COMM 6.7V�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//�Լ�˿�5v��ѹ
		OPEN_BUS_6_7V();
		SET_LV_5_0V();//�л���5V		
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
			WriteSystemLog("�Լ�˿�5V��ѹ�쳣,��ѹֵ:",strlen("�Լ�˿�5V��ѹ�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		
		//�Լ�˿�6.7v��ѹ		
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
			WriteSystemLog("�Լ�˿�6.7V��ѹ�쳣,��ѹֵ:",strlen("�Լ�˿�6.7V��ѹ�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
		//�Լ��ѹ��ѹ���ֵ�·
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
			WriteSystemLog("�Լ�AD_+11.3V��ѹ�쳣,��ѹֵ:",strlen("�Լ�AD_+11.3V��ѹ�쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}		
		//�Լ�ͨ�ŵ�·
		SET_CMD_SEND_STATE();		
		OSTimeDlyHMSM(0,0,0,20);
		for(i=0; i<10; i++)
		{
			DET_COM_SEND_PIN_INIT();//���͸ߵ�ƽ				
			OSTimeDly(2);
			SignedValue = GetLinABAdcValue();
			DET_COM_SEND_TOGGLE();//���͵͵�ƽ		
			OSTimeDly(2);
			if(((SignedValue > 100) && (GetLinABAdcValue() < -100)) ||
				((SignedValue < -100) && (GetLinABAdcValue() > 100)))
			{
				break;
			}
			OSTimeDlyHMSM(0,0,0,10);
		}
		DET_COM_SEND_PIN_INIT();
		SET_CMD_RECEIVE_STATE();//����Ϊ����״̬
		SET_LV_6_7V();
		if(i == 10)
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("�Լ�ͨ�ŵ�·�쳣!",strlen("�Լ�ͨ�ŵ�·�쳣!"), NEW_STR_INFO);			
			return ERROR;
		}
		
		//RTC�Լ�
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
			WriteSystemLog("RTC�쳣!",strlen("RTC�쳣!"), NEW_STR_INFO);			
			return ERROR;
		}
		if(device_info.HasScan)
		{
            u8 Result;
            
            Result = ScanHardwareState();
			if(0 == Result)
			{
                Op_Data.ScanIsZEBRA = 0;
				WriteSystemLog("ɨ��ģ���쳣!",strlen("ɨ��ģ���쳣!"), NEW_STR_INFO);			
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
//        {//��λ�ɹ���������0ʱ��˵��GPSͨѶ�϶���û�����
//            u8 working;
//            u32 c_timeout;
//            
//            working = GPSIsWorking();
//            
//            //1����֮�ڶ�û�����ݣ�˵��ͨѶ�쳣
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
//            {//�ָ��Լ�ǰ��״̬
//                ReceiveGPSInfoStop();
//            }
//            if(OSTimeGet() >= c_timeout)
//            {
//				WriteSystemLog("GPSģ���쳣!",strlen("GPSģ���쳣!"), NEW_STR_INFO);			
//				return ERROR;
//            }
//        }
		WriteSystemLog("�Լ��ѹ��������",strlen("�Լ��ѹ��������"), NEW_STR_INFO);
	}
	
	if(Type&0x02)
	{//�Լ��ѹ����
		OPEN_BUS_6_7V();
        Op_Data.LinNormalHighVoltageGoal = 12500;
        setDacHvValueMinFromVoltage(13000);//������ѹֻ��16V
		if(ERROR ==CheckHVProtectPoint())
		{
			CLOSE_HV_SW_INSIDE();
			WriteSystemLog("�Լ��ѹ��·�쳣!",strlen("�Լ��ѹ��·�쳣!"), NEW_STR_INFO);			
			return ERROR;			
		}
		OPEN_HV_SW();//���������
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
			WriteSystemLog("�̵����쳣,��ѹֵ:",strlen("�̵����쳣,��ѹֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Value,sizeof(Value), U16_DATA);				
			return ERROR;
		}
	
		WriteSystemLog("�Լ��ѹ��������",strlen("�Լ��ѹ��������"), NEW_STR_INFO);		
	}
	
	if(Type&0x04)
	{//�Լ����źŲ���
		//��Ϊһ�廯�豸ʱ��˫���𱬵�·
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
			WriteSystemLog("�Լ��𱬵�·1�쳣!",strlen("�Լ��𱬵�·1�쳣!"), NEW_STR_INFO);			
			return ERROR;
		}
		
		//��Ϊ������ʱ��˫���𱬵�·				
		OSTimeDlyHMSM(0,0,0,100);			
		OPEN_BUS_6_7V();
		if(ERROR == BlastSingleSelfTest(1))
		{
			OPEN_BUS_6_7V();
			WriteSystemLog("�Լ��𱬵�·(ģ��˫��)�쳣!",strlen("�Լ��𱬵�·(ģ��˫��)�쳣!"), NEW_STR_INFO);			
			return ERROR;
		}
		WriteSystemLog("�Լ����źŲ�������",strlen("�Լ����źŲ�������"), NEW_STR_INFO);
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
  * @brief  LCD����ˢ������
  * @param  RefreshFlag 0 ��ǿ��ˢ�£�1��ʾǿ��ˢ��
  * @retval ��
  * @author ZH
  * @date   2016��05��25��
  * @note   
  *************************************************************************************  
  */
void LcdBrightRefreshProcess(u8 RefreshFlag)
{
	static u32 LcdBrightTimeOutValue = 0;
	
	LcdBrightLock();
	
	if(RefreshFlag)
	{//ˢ�±���״̬
		if(device_info.LcdBrightTime == 0)
		{//���ⳣ��
			LcdSetBright(LCD_BRIGHT_RANK);		//��Ļ�����赽�
			LcdBrightTimeOutValue = 0xFFFFFFFF;//�����ظ�����(��Ļ����һ�����)
		}
		else if(device_info.LcdBrightTime >= 100)
		{//���ⳣ��
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);
			LcdBrightTimeOutValue = 0xFFFFFFFF;//�����ظ�����(��Ļ����һ�����)
		}
		else
		{//���ǳ���Ҳ���ǳ���
			LcdBrightTimeOutValue = OSTimeGet() + device_info.LcdBrightTime*10000UL/(1000/OS_TICKS_PER_SEC);  
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);				
		}	
	}
	else if(LcdBrightTimeOutValue != 0xFFFFFFFF)
	{	
		if(device_info.LcdBrightTime == 0)
		{//���ⳣ��
			LcdSetBright(LCD_BRIGHT_RANK);		//��Ļ�����赽�
			LcdBrightTimeOutValue = 0xFFFFFFFF;//�����ظ�����(��Ļ����һ�����)
		}
		else if(device_info.LcdBrightTime >= 100)
		{//���ⳣ��
			LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength*2+1);
			LcdBrightTimeOutValue = 0xFFFFFFFF;//�����ظ�����(��Ļ����һ�����)
		}
		else
		{//���ǳ���Ҳ���ǳ���
			if(OSTimeGet() >= LcdBrightTimeOutValue)
			{
				LcdSetBright(LCD_BRIGHT_RANK);		//��Ļ�����赽�
				LcdBrightTimeOutValue = 0xFFFFFFFF;//�����ظ�����(��Ļ����һ�����)
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
	UpdateProgbar(Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value);//���½�������100%
}
/**
  *************************************************************************************
  * @brief  �𱬵�ѹ���ݴ�����������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��11��06��
  * @note   �𱬵�ѹ���ݴ�����������,����Ϊ0ʱ11V, ÿ��100����+0.2V��400��ʱ��Ӧ11.6V��
  * ��ʽ��11.0+����ע���׹�����-1��/100��*0.2
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
  * @brief  �����Ƿ�Ϊɽ��ʥ����
  * @param  �� 
  * @retval ���
  *      @arg 0 ����
  *      @arg 1 ��
  * @date   2018��3��30��
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
  * @brief  �����Ƿ�Ϊ�ӱ�����
  * @param  �� 
  * @retval ���
  *      @arg 0 ����
  *      @arg 1 ��
  * @date   2018��5��6��
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
  * @brief  ����������Ż�ȡ��������
  * @retval ���
  *      @arg 0 δ֪
  *      @arg ���� ��������
  * @date   2018��3��30��
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
  * @brief  �׹ܵĹ��������Ƿ��������һ��
  * @retval ���
  *      @arg 0 ��һ��
  *      @arg 1 һ��
  * @date   2018��3��30��
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
  * @brief  ��ȡ�����׹�����
  * @param  �� 
  * @retval ����
  * @date   2019��5��6��
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
  * @brief  ͨ�������������ߵ������׶ε�ֵ��Ԥ���Ƿ��в����׹ܵ��ߡ�
  * @param  �� 
  * @retval 0��û�У�����ֵ�� ��
  * @date   2019��5��29��
  * @note   �ڰ�����˫������ñ�����
  *************************************************************************************  
  */
u8 DetOfflinePossible(u16 I_BeforeFire)
{
    u16 Count;
    
    //��������£��������Ӧ�ò���ȳ��ǰ�����Ͳ���ǰ����С
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
    
    //�����жϳ����ߵ��˵������
    Count = GetDetOnlineCount();
    if((Count > 0) && ((BlastInfo.I_BeforeTest/Count) > 10))
    {//�����׹���������0
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
  * @brief  �ж��ǲ���������ע��
  * @param  �� 
  * @retval 0��û�У�����ֵ�� ��
  * @date   2019��5��29��
  * @note   ���������ע�Ṧ��ʱ������������׹ܣ�������ʾ
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
            Flag = 1;//�ҵ�δע���
        }
	}
    return 0;    
}
void DetCode2UIDStr(U32_UNION* Serl, U32_UNION* Pwd, u8* UIDStr)
{
    u8 i;
    u8 index = 0;
    
    //ɽ��ʥ������׹���Ҫ���⴦��
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
            
            //�������ڹ���ǰ��һ��'0'���ճ�16�����ַ���������CRC16
            Buffer[0] = HexCharToValue(*UIDStr);
            HexStrToCharStr(UIDStr+1, 12, &Buffer[1]);
            Crc16 = GetCrc16Value(0xFFFF, NULL, Buffer, 7);
            sprintf((char*)(UIDStr+13), "%02X%02X", Crc16&0xFF, (Crc16>>8)&0xFF);
            return;
        }
        else if ((*UIDStr =='0') && (*(UIDStr+1) =='7'))
        {//ɽ������
            u16 Crc16;
            
            Crc16 = GetCrc16Value(0xFFFF, NULL, UIDStr, 13);
            sprintf((char*)(UIDStr+13), "%02X%02X", Crc16&0xFF, (Crc16>>8)&0xFF);
            return;
        }
        if(Date >= 20191101)
        {//��UID����  
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
    {//�Ѿ��ҵ�
        if(YXQ[1] == 0)//�·�Ϊ0,��ʾ��ǰ������Ч��
        {
            if(YXQ[0] == '2')//��ʹ�ù�
            {
                return 1;
            }
            *ErrorCode = YXQ[0];//
        }
        else
        {//�ж���Ч��
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
    {//��Ȩ��ʽ����
        *ErrorCode = 0xF1;
    }
    else if (U8Value == 0xFF)
    {//û���ҵ���Ȩ
        *ErrorCode = 0xF2;
    }    
    else
    {//��Ȩ�ļ�������,���ñ�����ǰ�Ѿ�ȷ�Ϲ�����Ȩ�ļ�
        *ErrorCode = 0xFF;
    }
    {
        char Buffuer[35];
        
        sprintf(Buffuer, "��Ȩʧ��(%02X):%02X%02X%02X%02X%02X%02X%02X%02X", *ErrorCode, 
            Serl->U8Buffer[0], Serl->U8Buffer[1], Serl->U8Buffer[2], Serl->U8Buffer[3],
            Pwd->U8Buffer[0], Pwd->U8Buffer[1], Pwd->U8Buffer[2], Pwd->U8Buffer[3]);
            
        WriteSystemLog(Buffuer, strlen(Buffuer), NEW_STR_INFO);
    }
    return 0;
}
//��Ȩ��֤
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
                WriteAccreditErrorFile(FileIndex, FileData, 3);//���������Ϣ���ļ��� ����+�������
            }
            OSTimeDly(1);
		}
	}
    return Count;
}
/**
  *************************************************************************************
  * @brief  �׹���Ȩ��֤(����)
  * @param  �� 
  * @retval δ��Ȩ����
  * @date   2019��6��29��
  * @note   
  *************************************************************************************  
  */
u16 AllDetAccreditForDL(void)
{
    u16 Count = 0;
    u8 i;
    
    WriteSystemLog("��ʼ������Ȩ��֤", strlen("��ʼ������Ȩ��֤"), CURRENT_STATUS);
    
    DeleteAllAccreditErrorFile();
      
    if((strlen((char*)Op_Data.Gps.Longitude) == 0) || (strlen((char*)Op_Data.Gps.Latitude) == 0))
    {
        u8 FileBuffer[20];
        
        //����Ȩ�ļ��ж�����γ��
        memset(FileBuffer, 0, sizeof(FileBuffer));
        GetConfigFileString("����ʱ��", FileBuffer, sizeof(FileBuffer), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);
        sscanf((char*)&FileBuffer[0], "%04d", (int*)&Op_Data.Gps.Year);
        sscanf((char*)&FileBuffer[4], "%02d", (int*)&Op_Data.Gps.Month);
        sscanf((char*)&FileBuffer[6], "%02d", (int*)&Op_Data.Gps.Day);
        sscanf((char*)&FileBuffer[8], "%02d", (int*)&Op_Data.Gps.Hour);
        sscanf((char*)&FileBuffer[10], "%02d", (int*)&Op_Data.Gps.Minute);
        sscanf((char*)&FileBuffer[12], "%02d", (int*)&Op_Data.Gps.Second);
        
        memset(Op_Data.Gps.Longitude, 0, sizeof(Op_Data.Gps.Longitude));
        GetConfigFileString("���ص㾭��", Op_Data.Gps.Longitude, sizeof(Op_Data.Gps.Longitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);

        memset(Op_Data.Gps.Latitude, 0, sizeof(Op_Data.Gps.Latitude));
        GetConfigFileString("���ص�γ��", Op_Data.Gps.Latitude, sizeof(Op_Data.Gps.Latitude), OTHER_FILE_PATH, DL_ACCREDIT_INFO_FILE2_ONLY_NAME);        
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
    WriteSystemLog("������Ȩ��֤����", strlen("������Ȩ��֤����"), CURRENT_STATUS);
    return Count;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�𱬼�¼�����ļ���·��
  * @param  �� 
  * @retval ·��
  * @date   2019��6��29��
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
            case DEVICE_TYPE_SAFE_MONITOR://����
                return BLASTER_RECORD_FILE_PATH_SAFE_MONITOR;
                
            case DEVICE_TYPE_SAFE_MONITOR_OFFLINE://����ģʽ
                return BLASTER_RECORD_FILE_PATH_OFFLINE;
                
            default://�ǰ���
                return BLASTER_RECORD_FILE_PATH_NOT_SAFE_MONITOR;
        }
	}
}
/**
  *************************************************************************************
  * @brief  �ж�GPS��׼�����Ƿ��Ѿ�ʧЧ
  * @param  �� 
  * @retval ·��
  * @date   2019��8��3��
  * @note   ��Чʱ��Ϊ24Сʱ
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
        
    if(GetTimeDifferSec(t1, t2) < 86400L)//24Сʱ����Ч
    {
        return 1;
    }
    return 0;
}
/**
  *************************************************************************************
  * @brief  �׹���Ϣ����ĸ�ʽ����
  * @param  �� 
  * @retval 0δ֪
  * @date   2019��9��25��
  * @note   
  *************************************************************************************  
  */
u8 FormatForDetInfo(u8* Version)
{
    if ((memcmp(Version, "MB05_", 5) == 0) || (memcmp(Version, "MQ03_", 5) == 0))
    {
        return 1;//VAоƬ,6��
    }
    if ((memcmp(Version, "MB02_", 5) == 0) || (memcmp(Version, "MQ02_", 5) == 0))
    {
        return 2;//XA02оƬ
    }
    if ((memcmp(Version, "MB10_", 5) == 0) || (memcmp(Version, "MQ01_", 5) == 0))
    {
        return 3;//XA03оƬ
    }
    return 0;//δ֪
}

/**
  *************************************************************************************
  * @brief  �ð汾�ŵĳ���ͱ��汾���׹���Ϣ��ʽ�Ƿ����
  * @param  �� 
  * @retval 0δ֪
  * @date   2019��9��25��
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
  * @brief  �������汾��Ӳ���汾�Ƿ�ƥ��
  * @param  �� 
  * @retval 0 ��ƥ�䣬 1 ƥ�䣬2 δ֪
  * @date   2020��2��28��
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
STM32ÿ��ϵ�ж�����Ψһ��һ��оƬ���кţ�96λbit, Ҳ����12�ֽڣ���
STM32F10X �ĵ�ַ�� 0x1FFFF7E8
STM32F20X �ĵ�ַ�� 0x1FFF7A10
STM32F30X �ĵ�ַ�� 0x1FFFF7AC
STM32F40X �ĵ�ַ�� 0x1FFF7A10
STM32L1XX �ĵ�ַ�� 0x1FF80050
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


