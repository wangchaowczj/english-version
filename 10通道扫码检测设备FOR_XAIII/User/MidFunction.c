#include "includes.h"


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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
//u16 U16DataCMP(const u16* ptr, u8 length)
//{
//	u16 max_value, min_value ;
//	u8 i ;

//	max_value = *ptr; //���ֵ
//	min_value = *ptr; //��Сֵ
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
//	return (max_value - min_value);	//�������ֵ
//}

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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
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

u32 GetU32DataMax(const u32* ptr, u8 length)
{
	u32 max_value =0;
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
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

u32 GetU32DataMin(const u32* ptr, u8 length)
{
	u32 min_value = 0 ;
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-11-02 
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

/**
  *************************************************************************************
  * @brief  ��ȡ�ַ�����ָ���ַ�������
  * @param[in] ptr �ַ���
  * @param[in] char_val ָ�����ַ�
  * @param[in] len �ַ�������  
  * @retval  
  * @author ZH
  * @date   2015��7��16��
  * @note   ��
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
//| �������� | HexCharToValue 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �ѵ���16�����ַ�ת��Ϊ��ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | c Ҫת�����ַ�
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ת�����ֵ
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-11-02 
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
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-11-02 
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
  * @param  FloatFlag:  �Ƿ�֧�ָ��� 0��֧�ָ�������������ĸ 1֧�ָ��㲻֧����ĸ  2֧����������ĸ 
  * @retval ���
  * @author ZH
  * @date   2015��7��14��
  * @note   ��
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
				if(((Str[i] < 'A')||(Str[i] >'Z'))&&(i == 8))//��8λΪ������
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
  * @brief  ����׹��Ƿ�����
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��7��14��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus CheckDetCurrentForDetAutoTest(void)
{
	static u8 det_leave = 0;//�׹�δ����
	static u16 bus_current[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
	static u8 Ptr = 0;

	bus_current[Ptr++ & 3] = GetCurrentValue(1, NULL); //��ѹ�µ��������ֵ
	
	if(GetU16DataMax(bus_current, 4) < 1000)//���е�����С��10
	{
		det_leave = 1;//�׹�������
	}

	if( det_leave == 1)
	{
		if (GetU16DataAverage(bus_current, 4) >= 3000)//��ֵ��������40uA
		{
			if(GetU16DataMin(bus_current, 4) >= 1000)//���е���������10uA
			{
				det_leave = 0;//�ȴ�����				
				return SUCCESS;//���׹�����
			}			
		}
	}
	return ERROR;//û���׹�����
}

/**
  *************************************************************************************
  * @brief  ����׹��Ƿ��Ѿ�����
  * @param  ��  
  * @retval ���
  * @author ZH
  * @date   2015��7��14��
  * @note   ��
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
	{//����USB
		if(0x80 == (OpData.UsbState&0x80))
		{//ǿ��Ϊ��/�ӻ�
			if((OpData.UsbState&0x07) != ((OpData.UsbState>>4)&0x07))
			{//δǿ��ʶ��ɹ�
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
		{//�����Զ�ʶ��
			if(READ_USB_GET_IN_PIN() == USB_CONNET_DEVICE)//ƽʱĬ��Ϊ�ӻ�
			{//����������
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
  * @brief  �жϹ����Ƿ�Ƿ�
  * @param  ��  
  * @retval ���
  * @author ZH
  * @date   2019��1��15��
  * @note   ��
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
    if((U8Value == 0) || (U8Value > 74))//��������
    {
        return ERROR;
    }
    U8Value = (*(Ptr+3)-'0')*10 + (*(Ptr+4)-'0');
    if((U8Value == 0) || (U8Value > 12))//��
    {
        return ERROR;
    }
    U8Value = (*(Ptr+3)-'0')*10 + (*(Ptr+4)-'0');
    if((U8Value == 0) || (U8Value > 31))//��
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
  * @brief  �ӹ����еõ��к�
  * @param  ��  
  * @retval �к�
  * @author ZH
  * @date   2019��1��15��
  * @note   ��
  *************************************************************************************  
  */
u16 GetBoxNumberFromDetOutside(u8* DetOutside)
{
    return (*(DetOutside+8)-'0')*100 + (*(DetOutside+9)-'0')*10 + (*(DetOutside+10)-'0');
}

/**
  *************************************************************************************
  * @brief  �ӹ����еõ�������ˮ��
  * @param  ��  
  * @retval ������ˮ��
  * @author ZH
  * @date   2019��1��15��
  * @note   ��
  *************************************************************************************  
  */
u8 GetInBoxIndexFromDetOutside(u8* DetOutside)
{
    return (*(DetOutside+11)-'0')*10 + (*(DetOutside+12)-'0');
}
/**
  *************************************************************************************
  * @brief  �ӹ����еõ��к�+��ˮ��
  * @param  ��  
  * @retval �к�
  * @author ZH
  * @date   2019��1��15��
  * @note   ��
  *************************************************************************************  
  */
u32 GetBoxIndexFromDetOutside(u8* DetOutside)
{
    return GetBoxNumberFromDetOutside(DetOutside)*100+GetInBoxIndexFromDetOutside(DetOutside);
}

