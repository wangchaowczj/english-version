#include "includes.h"
#include "GuiCustomUtils.h"
extern void SetProgbarParam(void);


DET Det;
DET_SERL_GROUP_INFO DetSerlGroupInfo;

/**
  *************************************************************************************
  * @brief  �ж�����������Ƿ���Ϲ���
  * @param  Serl ����
  * @param  Pwd ����
  * @retval 1 ���Ϲ��� 0������
  * @author ZH
  * @date   2019��4��8��
  * @note   ��
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
  * @brief  ����ת����
  * @param  Serl ������
  * @param  ���� Pwd
  * @param  Ptr ����
  * @retval ���
  * @author ZH
  * @date   2019��4��20��
  * @note   ��
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
    
    U16Value = (U64Value>>39)&0x7F;//��������bit39-bit45��7λ
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)Ptr, "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>35)&0x0F;//��bit35-bit38��4λ
    if(U16Value > 9)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+2), "%d", U16Value);    
    }

    U16Value = (U64Value>>22)&0x0F;//��bit22-bit25��4λ
    if((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+3), "%02d", U16Value);    
    }

    U16Value = (U64Value>>17)&0x1F;//��bit17-bit21��5λ
    if((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+5), "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>27)&0xFF;//������bit27-bit34��8λ,���ֻ�Ӣ��ASCII��
    if((U16Value < 0x30) || (U16Value > 0x80))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        *(Ptr+7) = U16Value;    
    }
     
    U16Value = (U64Value>>7)&0x3FF;//�к�bit7-bit16��10λ
    if(U16Value > 999)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+8), "%03d", U16Value);    
    }
    
    U16Value = U64Value&0x7F;//������ˮ��bit0-bi6��7λ
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
  * @brief  ����ת����
  * @param  Serl ������
  * @param  ���� Pwd
  * @param  Ptr ����
  * @retval ���
  * @author ZH
  * @date   2019��4��20��
  * @note   ��
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
    //������ˮ��bit0-bi6��7λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+11, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 99)
    {
        return ERROR;
    }
    
    U64Value = Value;

    //����bit7-bit16��10λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+8, 3);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 999)
    {
        return ERROR;
    }
    U64Value |= Value<<7;

    //�� bit17-bit21��5λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+5, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 31))
    {
        return ERROR;
    }
    U64Value |= Value<<17;

    //�� bit22-bit25��4λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 

    //�� bit22-bit25��4λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 
    
    // ��ĵ�λ bit26��1λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+2, 1);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 9)
    {
        return ERROR;
    }
    U64Value |= (Value&0x01)<<26; 
    
    //�� bit35-bit38��4λ
    U64Value |= ((uint64_t)Value)<<35; 
    
    //������bit27-bit34��8λ ���ֻ�Ӣ��ASCII��
    Value = *(Ptr+7);
    if((Value < 0x30) || (Value > 0x80))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<27; 
    
    //�������� bit39-bit45��7λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0x00) || (Value > 74))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<39; 

    //�׹�����  bit46-bit50��5λ
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
  * @brief  ������д��EEPROM
  * @param  Serl ������
  * @param  Addr ��ַ
  * @param  Buffer ����
  * @retval �����0�ɹ��� 1����ʧ�ܣ�2дEEPROMʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u8 XA3_UnlockAndWriteEEPROM(u32 Serl, u8 Addr, u8* Buffer)
{
    u8 Retry = 2;
    
    while(Retry--)
    {
        if(ERROR == XA3_WriteDataUnlock(Serl))
        {
            return 1;//��EEPROM����
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
    return 2;//дEEPROM����
}
/**
  *************************************************************************************
  * @brief  ����ʱ����һ���������ʱʱ��
  * @param  DelayMs ����ʱʱ��,��λms 
  * @retval оƬ��ʱʱ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
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
  * @brief  ��������ʱ��оƬƵ�ʼ���оƬ��ʱ����ֵ
  * @param  DelayMs ����ʱʱ��,��λms
  * @param  Freq оƬƵ��   
  * @retval ��Ӧ��оƬ��ʱ����ֵ
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
u32 BlastDelayConvertToDetDelayClock(u16 DelayMs, u32 Freq)
{
	return 1+ BlastDelayConvertToDetDelay(DelayMs)*Freq/1000;
}
//==================================================================================================
//| �������� | SaveErrCode 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������׹ܴ���   
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� |  
//|----------|--------------------------------------------------------------------------------------
//| ������� | err_index�������׹ܺ�   err_code���������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� |     
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� | det_info������Ϣ  
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�֣  ��    ʱ�䣺2012-3-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |      
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void SaveErrCode(u16 err_index, u8 err_code)
{
	if(Det.Info[err_index].Err != err_code)
	{
		if(0x00 == Det.Info[err_index].Err) //�׹�ԭ���޴��� 
		{
			Det.Info[err_index].Err = err_code ;
			Det.ErrorCount++;					
		}
		if(0x00 == err_code) //����׹ܴ������
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
		if(0x00 == Det.Info[err_index].RetryErr) //�׹�ԭ���޴��� 
		{
			Det.Info[err_index].RetryErr = err_code ;
		}
		if(0x00 == err_code) //����׹ܴ������
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
  * @brief  ����������
  * @param  ��
  * @retval 0xFF û�ҵ�
  * @author ZH
  * @date   2016��7��2��
  * @note   ��
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
  * @brief  �������Ϣ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��7��2��
  * @note   ��
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
//| �������� | TestInfoInit 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ������Ϣ��ʼ�� 
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� |  
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��    
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� | det_info������Ϣ  
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�֣  ��    ʱ�䣺2012-3-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |      
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void TestInfoInit(void)
{
	u16 i;
	u8 GroupIndex;
	
	ClearSerlGroup();
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		Det.Info[i].Err = 0x00 ;  //��մ������
		Det.Info[i].RetryErr = 0x00 ;
		if (GetDetRegStatus(i) != ERROR)
		{
			GroupIndex = GetSerlGroupIndex(Det.Info[i].Serl.U32Value);
			if(GroupIndex >= DET_SERL_GROUP_INFO_MAX)
			{//��û��
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
//| �������� | GetDetNum
//|----------|--------------------------------------------------------------------------------------
//| �������� | ͳ��ע�����ʹ�����   
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� |  
//|----------|--------------------------------------------------------------------------------------
//| ������� |  
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� |     
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� |  
//|          |  
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-04-17  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |      
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void GetDetNum(void)
{
	u16 i, Reg = 0, Error = 0;
	
	/****ͳ��ע���׹����ʹ����׹���****/
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
	
	/****�豸��������****/
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
//| �������� | AllDisCharge
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����2�θ�λָ����50ms
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� | 	��
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	��
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� |  ��
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-04-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void AllDisCharge(void)
{
	XA3_ClearAll();
	OSTimeDlyHMSM(0,0,0,50);//��ʱ80ms������С��50ms
	XA3_ClearAll();
	OSTimeDlyHMSM(0,0,0,50);//��ʱ80ms
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
        OSTimeDlyHMSM(0,0,0,80);//��ʱ80ms
        WriteSystemLog("��λ���ض�PWD",strlen("��λ���ض�PWD"), NEW_STR_INFO);
        XA3_DetCheckIn();
        OSTimeDlyHMSM(0,0,0,80);//��ʱ��оƬ��EEPROM     
        if(ERROR != XA3_ReadPwd(Serl, Pwd))
        {
            return SUCCESS;
        }
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  �ж��������Ƿ��Ѿ�ע���׹�
  * @param  Index ��������
  * @retval ���
  *      @arg SUCCESS ��ע��
  *      @arg ERROR û��ע��
  * @author ZH
  * @date   2015��8��7��
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
//| �������� | CheckOverlap 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����ϵͳ��¼�е����к��Ƿ����ظ�  
//|----------|--------------------------------------------------------------------------------------
//| ����ģ�� |  
//|----------|--------------------------------------------------------------------------------------
//| ������� | det_buffer Ҫ�Ƚϵ��׹���Ϣ�׵�ַ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� |SUCCESS��  û���ظ���������     
//|          |ERROR:    ���ظ���������  
//|----------|--------------------------------------------------------------------------------------       
//| ȫ�ֱ��� |det_hole_num_overlaped���׹���ע��ʱ���صĿ׺�  
//|          |hole_info:�׹���Ϣ  
//|----------|-------------------------------------------------------------------------------------- 
//| ������� | ��д�ˣ�֣  ��    ʱ�䣺2013-4-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   |      
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus CheckOverlap(const u8* Serl)
{
	u16 i;

    for(i=0; i<DET_MAX_SIZE; i++)
    {
		if(GetDetRegStatus(i) != ERROR)
		{
			if(0  == memcmp(Serl, &Det.Info[i].Serl.U8Buffer[0], 4))//�Ƚ��������Ƿ�����ͬ
			{
				 Op_Data.overlaped = i ;  //�����غ�����
				 return ERROR ;			
			}		
		}
    }
    return SUCCESS ;  // û���ظ����׹�
}

//==================================================================================================
//| �������� | CheckDetFreq
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������Ƶ���Ƿ�����
//|----------|--------------------------------------------------------------------------------------
//| ������� |	freq  �����Ƶ������
//|          |  check_state  ���״̬��0Ϊע�ᣬ1Ϊ������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	DET_PASS  Ƶ������,FREQ_MIN Ƶ�ʹ�С��FREQ_MAXƵ�ʹ���
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ע��״̬��оƬƵ��Ҫ����ϣ�������оƬƵ�ʷ�Χ����
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
CHECK_RESULT CheckDetFreq(u32 freq, u8 check_state)
{
	if(0 == check_state)
	{//ע��״̬
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
	{//������״̬
		if(freq < (device_info.det_freq_min-20))//������״̬��оƬƵ�ʷ�Χ����
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
//| �������� | CheckDetType
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����׹������Ƿ����Ҫ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | det_bat2 ���ź�2
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	SUCCESS  �׹����ͷ���Ҫ��
//|          | 	ERROR     �׹����ͷ��ϲ�Ҫ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
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
  * @brief  ��������ת��оƬ����
  * @param[in]  Type �׹�����
  * @retval ����
  * @author ZH
  * @date   2016��7��6��
  * @note   
  *************************************************************************************  
  */
u8 GetDetType(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>46)&0x1F;//  bit46-bit50��5λ
}
/**
  *************************************************************************************
  * @brief  ��������ת��оƬ����
  * @param[in]  ��������
  * @retval ����
  * @author ZH
  * @date   2016��7��6��
  * @note   
  *************************************************************************************  
  */
u8 GetDetFactoryCode(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>39)&0x7F;//  bit39-bit45��7λ
}
/**
  *************************************************************************************
  * @brief  ����оƬ�����ж��Ƿ�Ϊ����������
  * @param[in]  Type �׹�����
  * @retval 0���ǵ����ݣ�����ֵ��ʾ������
  * @author ZH
  * @date   2016��7��6��
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
  * @brief  ��ȡУ׼У׼����
  * @param[in]  ��
  * @retval �� 
  * @author ZH
  * @date   2016��7��6��
  * @note   
  *************************************************************************************  
  */
u16 GetFixedPulseCount(void)
{
	return (1000 + BlastDelayConvertToDetDelay(GetDetDelayMaxFromRegStatus()));
}

/**
  *************************************************************************************
  * @brief  ���Ӳ����׹�ʱ�����������࣬����������ʾ�޷����ӻ���ͨѶ�쳣
  * @param  ��
  * @retval ���
  *      @arg DET_TEST_CONNECT_ERR �޷�����
  *      @arg DET_TEST_COMM_ERR ͨѶ�쳣
  * @author ZH
  * @date   2018��6��28��
  * @note   ��
  *************************************************************************************  
  */
u8 ConnectErrorCodeIdentify(void)
{
	if((DetProtocol.Rise.Count >= 1) && (DetProtocol.Fall.Count >= 1))
	{
		return DET_TEST_COMM_ERR;//�ܽ��յ��ź�,����������������Ϊ��ͨѶ����
	}
	return DET_TEST_CONNECT_ERR;
}


//==================================================================================================
//| �������� | CheckDet
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������Ƿ����׹�����
//|----------|--------------------------------------------------------------------------------------
//| ������� |	det_buffer �����⵽���׹���Ϣ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	DET_PASS,			  //��⵽�׹�
//|			 |	CHECK_UNDONE,         //���δ���
//|			 |	COMM_ERR, 	          //ͨ�ų���
//|			 |	TEST_ERR,			  //BIT���� 
//|			 |	SECU_CODE_ERR,		  //����У�����
//|			 |	FREQ_MIN,			  //Ƶ�ʹ�С
//|			 |	FREQ_MAX,			  //Ƶ�ʹ���
//|			 |	OVERLAPED,			  //���߹�����������к��Ѿ�ע�� 
//|			 |	TYPE_ERR,			  //�׹����ʹ���
//|			 |	CURRENT_MIN,		  //����С
//|			 |	CURRENT_MAX,		  //������
//|			 |	CURRENT_UNSTABLE      //�������ȶ�
//|----------|--------------------------------------------------------------------------------------        
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
CHECK_RESULT CheckDet(DET_INFO* det_buffer, u8 Reg)
{	 
	ErrorStatus Result;
	CHECK_RESULT check_result;
    u8 U8Value;
	u16 DetStatus;	
	u8 i;
	
	/*�建��������*/
	det_buffer->Serl.U32Value = 0;
	det_buffer->Pwd.U32Value  = 0;
	
	WriteSystemLog("��ʼע���׹�",strlen("��ʼע���׹�"), CURRENT_STATUS);
	
	for(i=0; i<5;i++)
	{
		/*����ȫ�ָ�λָ��*/
		AllDisCharge();
		Result = XA3_DetCheckIn();// ��©�����success����ôִ�����Ķ��������򣬷���û���׹�
		if(ERROR != Result)
		{
			break;
		}	
	}
	if(ERROR == Result)
	{
		WriteSystemLog("��©ʧ��",strlen("��©ʧ��"), NEW_STR_INFO);
		return COMM_ERR;
	}

	/*��ȡ������*/
	Result = XA3_ReadSerl(&det_buffer->Serl.U32Value); 
	if(ERROR == Result)
	{
		WriteSystemLog("��ȡ������ʧ��",strlen("��ȡ������ʧ��"), NEW_STR_INFO);
		return COMM_ERR;
	}

	WriteSystemLog("ע��,��������ɹ�:",strlen("ע��,��������ɹ�:"), STR_INFO);
	WriteSystemLog(&det_buffer->Serl.U8Buffer[0], 4, HEX_ARRAY);
     
    Result = XA3_ReadPwdRetry(det_buffer->Serl.U32Value, &det_buffer->Pwd.U32Value); 
	if(ERROR == Result)
	{//
        WriteSystemLog("��ȡPWDʧ��",strlen("��ȡPWDʧ��"), NEW_STR_INFO);
        return COMM_ERR;   
	}

	WriteSystemLog("��PWD�ɹ�:",strlen("��PWD�ɹ�:"), STR_INFO);
	WriteSystemLog(&det_buffer->Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(!DetCodeRulePass(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
    {//�����Ϲ�������룬�ض�һ�ο���
        U32_UNION Serl, Pwd;
        
		WriteSystemLog("���벻���Ϲ���",strlen("���벻���Ϲ���"), NEW_STR_INFO);

        /*�ض�������*/
        Result = XA3_ReadSerl(&Serl.U32Value); 
        if(ERROR == Result)
        {
            WriteSystemLog("�ض�������ʧ��",strlen("�ض�������ʧ��"), NEW_STR_INFO);
            return COMM_ERR;
        }

        WriteSystemLog("�ض�������ɹ�:",strlen("�ض�������ɹ�:"), STR_INFO);
        WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
         
        Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value); 
        if(ERROR == Result)
        {//
            WriteSystemLog("�ض�PWDʧ��",strlen("�ض�PWDʧ��"), NEW_STR_INFO);
            return COMM_ERR;   
        }
        WriteSystemLog("�ض�PWD�ɹ�:",strlen("�ض�PWD�ɹ�:"), STR_INFO);
        WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
        
        if(det_buffer->Serl.U32Value != Serl.U32Value)
        {
            WriteSystemLog("�����벻һ��",strlen("�����벻һ��"), NEW_STR_INFO);
        }
        if(det_buffer->Pwd.U32Value != Pwd.U32Value)
        {
            WriteSystemLog("PWD��һ��",strlen("PWD��һ��"), NEW_STR_INFO);
        }

        /**����оƬ**/
        Result = XA3_Mask(det_buffer->Serl.U32Value);
        if(ERROR == Result)
        {
            WriteSystemLog("����ʧ��",strlen("����ʧ��"), NEW_STR_INFO);
            return COMM_ERR;
        }
        
        if(!DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
        {
            WriteSystemLog("�ض����ݲ����Ϲ���", 0, NEW_STR_INFO);
            return DET_CODE_ERR;//�׹����벻���Ϲ��򣬿������������
        }
        det_buffer->Serl.U32Value = Serl.U32Value;
        det_buffer->Pwd.U32Value = Pwd.U32Value;
    }
	/**����оƬ**/
	Result = XA3_Mask(det_buffer->Serl.U32Value);
	if(ERROR == Result)
	{
		WriteSystemLog("����ʧ��",strlen("����ʧ��"), NEW_STR_INFO);
		return COMM_ERR;
	}
    //����ID�������豸ֻ��ʹ�ö�Ӧ�ܳ��������׹�
    if(!DetFactoryCodeIsRulePass(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
    {
        WriteSystemLog("�������벻һ��",strlen("�������벻һ��"), NEW_STR_INFO);
        return DET_FAC_CODE_ERR;
    }
	/**���оƬ����**/
	Result = CheckDetType(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);
	if(ERROR == Result)
	{
		WriteSystemLog("оƬ���ʹ���",strlen("оƬ���ʹ���"), NEW_STR_INFO);
		return TYPE_ERR;
	}
    /*����*/
	U8Value = XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);	
	if(U8Value == 1)
	{
        WriteSystemLog("��������",0, NEW_STR_INFO);
		U8Value = XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value);
	}
    
	if(U8Value == 1)
	{
		WriteSystemLog("�������:",strlen("�������:"), NEW_STR_INFO);
		return  SECU_CODE_ERR;//�������
	}
	else if(U8Value != 0)
	{
		WriteSystemLog("����ͨѶ����",strlen("����ͨѶ����"), NEW_STR_INFO);
		return  COMM_ERR;//�������
	}    
	/*����BIT����*/
    if(Reg)
    {//ע��
        XA3_Charge(det_buffer->Serl.U8Buffer[0]);
        XA3_Charge(det_buffer->Serl.U8Buffer[0]);
        OSTimeDlyHMSM(0,0,0,500);//5.5Vʱ��250-300ms���ң�7Vʱ��200ms���ҡ���ʱ0.45��
        
        Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//�ض����Խ��
        if(ERROR == Result)
        {
            WriteSystemLog("״̬�ض�����",strlen("״̬�ض�����"), NEW_STR_INFO);
            return COMM_ERR;
        }
        /*��ȡBIT״̬*/
        if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // �ж�BIT���Ե�״̬
        {
            WriteSystemLog("оƬ�Ƚ���״̬����",strlen("оƬ�Ƚ���״̬����"), NEW_STR_INFO);
            return TEST_ERR;	
        }    
    }
    else
    {//�������
        XA3_Bit(det_buffer->Serl.U32Value);// bit����
        XA3_Bit(det_buffer->Serl.U32Value);// bit����

        OSTimeDlyHMSM(0,0,1,0);//��ʱ1��
        
        Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//�ض����Խ��
        if(ERROR == Result)
        {
            WriteSystemLog("״̬�ض�����",strlen("״̬�ض�����"), NEW_STR_INFO);
            return COMM_ERR;
        }
        /*��ȡBIT״̬*/
        if (XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) == ERROR)  // �ж�BIT���Ե�״̬
        {
            WriteSystemLog("оƬBIT����",strlen("оƬBIT����"), NEW_STR_INFO);
            return TEST_ERR;	
        }       
    }
	
	/*��ȡƵ�ʣ�ͨ��ͬ��ͷ���*/	
	det_buffer->Freq.U32Value = GetDetFreqFromHead();
	
	/*�ж�Ƶ���Ƿ�����*/
	check_result = CheckDetFreq(det_buffer->Freq.U32Value, 0);
	if(DET_PASS != check_result)
	{
        char Buffur[20];
        
        sprintf(Buffur, "Ƶ�ʴ���:%dHz", det_buffer->Freq.U32Value);
        WriteSystemLog(Buffur,strlen(Buffur), NEW_STR_INFO);
		return check_result;
	}	
			
	/*����غ�*/
	Result = CheckOverlap(&det_buffer->Serl.U8Buffer[0]);
	if(ERROR == Result)
	{
        u8 Buffer[8];
        
		WriteSystemLog("�غŴ���:",strlen("�غŴ���:"), NEW_STR_INFO);
        memcpy(&Buffer[0], &det_buffer->Serl.U8Buffer[0], 4);
        memcpy(&Buffer[4], &det_buffer->Pwd.U8Buffer[0], 4);
        WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);

        memcpy(&Buffer[0], &Det.Info[Op_Data.overlaped].Serl.U8Buffer[0], 4);
        memcpy(&Buffer[4], &Det.Info[Op_Data.overlaped].Pwd.U8Buffer[0], 4);
        WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
        
		return OVERLAPED;
	}
    
	WriteSystemLog("�׹�����",strlen("�׹�����"), NEW_STR_INFO);
	
	return DET_PASS;
}
/**
  *************************************************************************************
  * @brief  оƬ����
  * @param  Param  det_buffer ������оƬоƬ
  * @retval ���������bit0
  *     @arg @b bit0 ������ض����
  *     @arg @b bit1 BIT���Խ��
  *     @arg @b bit2 д���ڽ��
  *     @arg @b bit3 ��Ƶ��� 
  *     @arg @b bit4 �����    
  * @author ZH
  * @date   2014��12��04��
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
    
	/*�建��������*/
	det_buffer->Serl.U32Value = 0;
	det_buffer->Pwd.U32Value = 0;
	det_buffer->Freq.U32Value = 0;
	det_buffer->Err = 0;
    
    AllReset();
    
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	
	
	/*�׹ܸ������п��ܲ��ȶ�*/
	OSTimeDly(300);
	c_t = OSTimeGet();
	while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
	{
		i_temp = Op_Data.LinABCurrent;
		OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
		if(abs(Op_Data.LinABCurrent - i_temp) < 20)
		{
			i_temp = Op_Data.LinABCurrent;
			OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
			if(abs(Op_Data.LinABCurrent - i_temp) < 20)
			{
                break;	
			}
		}		
	}	

	BlastInfo.I_BeforeTest = Op_Data.LinABCurrent;//����̬����
	OSTimeDlyHMSM(0, 0, 0, 100);
	
	c_t = OSTimeGet();
	while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
	{
		i_temp = Op_Data.LinABCurrent;
		OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
		if(abs(Op_Data.LinABCurrent - i_temp) < 20)
		{
			i_temp = Op_Data.LinABCurrent;
			OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
			if(abs(Op_Data.LinABCurrent - i_temp) < 20)
			{
                break;
			}
		}		
	}
	
	BlastInfo.I_BeforeFire = Op_Data.LinABCurrent;//����̬����
	
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	
	OSTimeDlyHMSM(0, 0, 0, 50);//��ʱ
	/*��ȡ������*/
	Result = XA3_ReadSerl(&det_buffer->Serl.U32Value); 
	if(ERROR != Result)
	{        
		Result = XA3_ReadPwdRetry(det_buffer->Serl.U32Value, &det_buffer->Pwd.U32Value); 
        if(ERROR != Result)
        {
            AnalystResult |= DET_ANALYST_READ_SN_MASK_BIT;//������ض�����
            
            det_buffer->Freq.U32Value = GetDetFreqFromHead();
            
            AnalystResult |= DET_ANALYST_FREQ_MASK_BIT;//��Ƶ����
            
            memset(Buffer, 0, sizeof(Buffer));
            memcpy(Buffer, &det_buffer->Serl.U8Buffer[0], 4);
            memcpy(&Buffer[4], &det_buffer->Pwd.U8Buffer[0], 4);
            WriteSystemLog("оƬ��������:",strlen("оƬ��������:"), CURRENT_STATUS);
            WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
            
            memset(Buffer, 0, sizeof(Buffer));
            strcpy((char*)Buffer, "����:");
            if(ERROR != XA3_DetInside2Outside(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
            {
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
            }
            
//            for(i=4;i<7;i++)
//            {
//                if(XA3_ReadEEPROM(i, (u8*)&Other.U8Buffer[0]) == ERROR)
//                {
//                    sprintf((char*)Buffer, "����ַ%dʧ��", i);
//                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
//                }
//                else
//                {
//                    sprintf((char*)Buffer, "����ַ%d�ɹ�:", i);
//                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
//                    
//                    WriteSystemLog(&Other.U8Buffer[0], 4, HEX_ARRAY);                
//                }
//            }
            
            /*����BIT����*/
            XA3_Bit(det_buffer->Serl.U8Buffer[0]);// bit����
            XA3_Bit(det_buffer->Serl.U8Buffer[0]);// bit����
            OSTimeDlyHMSM(0,0,1,0);//��ʱ1��	

            /*��ȡBIT״̬*/
            Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//�ض����Խ��
            if(ERROR == Result)
            {
                return 0;		
            }
            if (XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) != ERROR)  // �ж�BIT���Ե�״̬
            {
                AnalystResult |= DET_ANALYST_BIT_RESULT_MASK_BIT;//BIT����	
            }
            
            Result = XA3_WriteTime(det_buffer->Serl.U32Value, DET_FREQ_MEASURE_WRITE_VALUE);
            if(ERROR == Result)
            {
                return 0;
            }
            AnalystResult |= DET_ANALYST_WRITE_DELAY_MASK_BIT;//д��������
            
            /*����*/
            if(0 == XA3_PwdUnlock(det_buffer->Serl.U32Value, det_buffer->Pwd.U32Value))
            {
                AnalystResult |= DET_ANALYST_UNLOCK_MASK_BIT;//��������
            }
            XA3_Charge(det_buffer->Serl.U8Buffer[0]);
            XA3_Charge(det_buffer->Serl.U8Buffer[0]);
            OSTimeDlyHMSM(0,0,0,500);//5.5Vʱ��250-300ms���ң�7Vʱ��200ms���ҡ���ʱ0.45��
            
            if((DEVICE_USAGES_ALONE == device_info.DeviceUsages) && (Op_Data.DelaySampleCfg))
            {//�ܳ�ר�õ��豸����Ҫ
                XA3_ExpenAll();
                XA3_ExpenAll();	
                OSTimeDlyHMSM(0,0,0,100);
            }
            
            /*��ȡ���״̬*/
            Result = XA3_ReadStatus(det_buffer->Serl.U32Value, &DetStatus);//�ض����Խ��
            if(ERROR == Result)
            {
                return 0;
            }
            if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) != ERROR)  // �жϳ����Ե�״̬
            {
                if(XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) != ERROR)
                {
                    AnalystResult |= DET_ANALYST_CHARGE_MASK_BIT;//�������
                }
            }
            
            XA3_Mask(det_buffer->Serl.U32Value);//�����׹ܣ��������·������׹�
            
            OSTimeDlyHMSM(0, 0, 1, 0);
            c_t = OSTimeGet();
            while(OSTimeGet() < (c_t+2000/(1000/OS_TICKS_PER_SEC)))
            {
                i_temp = Op_Data.LinABCurrent;
                OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
                if(abs(Op_Data.LinABCurrent - i_temp) < 20)
                {
                    i_temp = Op_Data.LinABCurrent;
                    OSTimeDlyHMSM(0, 0, 0, 200);//��ʱ
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
  * @brief  ���������׹�
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��04��
  * @note   
  *************************************************************************************  
  */
TEST_RESULT DetMaskAll(void)
{
	ErrorStatus Result;
	u16 i;		

	//��ʼ������������
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount;
	SetProgbarParam();
	
    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	

	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����			
			/*��������ָ�Ŀ����Ϊ�˲�©*/
			XA3_Mask(Det.Info[i].Serl.U32Value);
		}	
	}
	FillCurrentProgbar();
	
	/*��©,����5�η��͵�����������ݷ�����˵����δע���׹�*/
    Op_Data.FindUnRegDetInfo = 0;
	if(HaveUnregisteredDetBySimpleSignal())
	{
        WriteSystemLog("��⵽��δע���׹�",strlen("��⵽��δע���׹�"), CURRENT_STATUS);
        
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
                WriteSystemLog("����:",strlen("����:"), STR_INFO);
                WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
                
                memset(Buffer, 0, sizeof(Buffer));
                strcpy((char*)Buffer, "����:");
                if(ERROR != XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, 
                    Op_Data.UnRegDetInfo.Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
                {
                    WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
                }
            }            
        }
		return UNREG_DET;//��⵽��δע���׹�
	}
	return TEST_END;  //û�м�⵽δע���׹�	
}

//==================================================================================================
//| �������� | TestDetQuick
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��·���ٲ���
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	
//|			 |	TEST_END    �������
//|			 |	UNREG_DET   ������ɣ���δע���׹�
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
TEST_RESULT TestDetQuick(void)
{
	ErrorStatus Result;
    u8 U8Value;
	u16 i;
    u8 FindUnRegDet = 0;
    TEST_RESULT TestResult = TEST_END;  //û�м�⵽δע���׹�
    
	/*��ʼ����ǰ�ȳ�ʼ��������뼰����оƬ���ڶ�Ӧ��ʱ����*/
	TestInfoInit();

	//��ʼ������������
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount;
	SetProgbarParam();
    
    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();
	
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
			
			/*��������ָ�Ŀ����Ϊ�˲�©*/
			Result = XA3_Mask(Det.Info[i].Serl.U32Value);
			if(ERROR == Result)
			{
				SaveErrCode(i,(0<<4)+ConnectErrorCodeIdentify()) ;       //����������      
                continue;
			}
            
            U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            if(1 == U8Value)
            {//����������ʱ��������һ�Σ������оƬ�ڲ����������ʱ���п���ͨ����
				U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
                if(U8Value == 0)
                {
                    u8 Buffer[20];
                    
                    sprintf((char*)Buffer, "%d-%d ���Խ��ܳɹ�",
                        Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside);
                    WriteSystemLog(Buffer,strlen((char*)Buffer), NEW_STR_INFO);
                }
            }
			if(1 == U8Value)
            {
				SaveErrCode(i,(1<<4)+DET_TEST_PWD_ERR) ;       //����������
            }
            else if(0 != U8Value)
            {
                SaveErrCode(i,(1<<4)+ConnectErrorCodeIdentify()) ;       //����������
            } 
		}	
	}

	/*��©,����5�η��͵�����������ݷ�����˵����δע���׹�*/
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
		FindUnRegDet = 1;//��⵽��δע���׹�
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
                        sprintf((char*)Buffer, "%d-%d %02X%02X%02X%02X%02X%02X%02X%02X ʵ��PWD:%02X%02X%02X%02X",
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
                    sprintf((char*)Buffer, "%d-%d ��PWDʧ��",
                        Det.Info[i].Hole.Name,Det.Info[i].Hole.Inside);
                    WriteSystemLog(Buffer,strlen((char*)Buffer), NEW_STR_INFO);
                }
            }            
        }
    }
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();
	
	/*ͳ�ƴ�����*/
	GetDetNum();

	FillCurrentProgbar();
    
	if(FindUnRegDet)
	{
        WriteSystemLog("��⵽��δע���׹�",strlen("��⵽��δע���׹�"), CURRENT_STATUS);
        if(Op_Data.FindUnRegDetInfo)
        {
            u8 Buffer[20];
            
            memset(Buffer, 0, sizeof(Buffer));
            memcpy(Buffer, &Op_Data.UnRegDetInfo.Serl.U8Buffer[0], 4);
            memcpy(&Buffer[4], &Op_Data.UnRegDetInfo.Pwd.U8Buffer[0], 4);
            WriteSystemLog("����:",strlen("����:"), STR_INFO);
            WriteSystemLog(&Buffer[0], 8, HEX_ARRAY);
            
            memset(Buffer, 0, sizeof(Buffer));
            strcpy((char*)Buffer, "����:");
            if(ERROR != XA3_DetInside2Outside(Op_Data.UnRegDetInfo.Serl.U32Value, 
                Op_Data.UnRegDetInfo.Pwd.U32Value, &Buffer[strlen((char*)Buffer)]))
            {
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
            }
        }
		TestResult = UNREG_DET;//��⵽��δע���׹�
	}
    if(Det.ErrorCount == 1)
    {//����������Ļ������Ծ���
        for(i=0; i < DET_MAX_SIZE; i++)
        {
            if ((GetDetRegStatus(i) != ERROR) && (Det.Info[i].Err != 0))
            {
                if((Det.Info[i].Err&0x0F) == DET_TEST_CONNECT_ERR)
                {//δ����,�п������������
                    if(FindUnRegDet && Op_Data.FindUnRegDetInfo)
                    {//����δע���׹ܣ����һض�����������
                        if(DetCodeRulePass(Op_Data.UnRegDetInfo.Serl.U32Value, Op_Data.UnRegDetInfo.Pwd.U32Value))
                        {//���ֵ�δע���׹ܣ����Ϲ��򣬲��������
                            break;
                        }
                        WriteSystemLog("��δע��,���Ծ�������", 0, STR_INFO);
                        
                        //���κ��ٲ�©��ȷ��������ֻ��һ��δע���׹�
                        Result = XA3_Mask(Op_Data.UnRegDetInfo.Serl.U32Value);
                        if(ERROR == Result)
                        {
                            WriteSystemLog("����ʧ��", 0, STR_INFO);
                            break;
                        }
                        if(HaveUnregisteredDetBySimpleSignal())
                        {//���κ���δע���׹�
                            WriteSystemLog("���κ���δע��", 0, STR_INFO);
                            break;
                        }
                        
                        if(Op_Data.UnRegDetInfo.Pwd.U32Value != Det.Info[i].Pwd.U32Value)
                        {//�������Ҳ���˾���д����
                            U8Value = XA3_UnlockAndWriteEEPROM(Op_Data.UnRegDetInfo.Serl.U32Value, 3,  Det.Info[i].Pwd.U8Buffer);
                            if(U8Value == 1)
                            {//��EEPROMʧ��
                                WriteSystemLog("��EEPROMʧ��1", 0, STR_INFO);
                                break;
                            }
                            else if(U8Value != 0)
                            {//дʧ��
                                WriteSystemLog("дPWDʧ��", 0, STR_INFO);
                                break;
                            }
                            WriteSystemLog("��дPWD", 0, STR_INFO);                            
                        }
                        if(Op_Data.UnRegDetInfo.Serl.U32Value != Det.Info[i].Serl.U32Value)    
                        {//�����������
                            U8Value = XA3_UnlockAndWriteEEPROM(Op_Data.UnRegDetInfo.Serl.U32Value, 2,  Det.Info[i].Serl.U8Buffer);
                            if(U8Value == 1)
                            {//��EEPROMʧ��
                                WriteSystemLog("��EEPROMʧ��2", 0, STR_INFO);
                                break;
                            }
                            else if(U8Value != 0)
                            {//дʧ��
                                WriteSystemLog("д������ʧ��", 0, STR_INFO);
                                break;
                            }
                            WriteSystemLog("��д������", 0, STR_INFO);
                        }
                    }
                    else
                    {
                        break;
                    }
                }            
                else if((Det.Info[i].Err&0x0F) == DET_TEST_PWD_ERR)
                {//��������п������������
                    U32_UNION Pwd;
                    
                    //��ǰ���߲�����δע���׹�
                    if(HaveUnregisteredDetBySimpleSignal())
                    {//��δע���׹�
                        break;
                    }
                    WriteSystemLog("�������,���Ծ�������", 0, STR_INFO);
                    
                    //�ض�����
                    if(ERROR == XA3_ReadPwdRetry(Det.Info[i].Serl.U32Value, &Pwd.U32Value))
                    {
                        WriteSystemLog("�ض�PWDʧ��", 0, STR_INFO);
                        break;
                    }
                    if(DetCodeRulePass(Det.Info[i].Serl.U32Value, Pwd.U32Value))
                    {//���Ϲ��򣬲��������
                        WriteSystemLog("PWD���Ϲ���", 0, STR_INFO);
                        break;
                    }
                    if(Det.Info[i].Pwd.U32Value == Pwd.U32Value)
                    {
                        WriteSystemLog("PWD��ͬ", 0, STR_INFO);//��Ӧ�ó���������ͬ�ֽ���ʧ�ܣ��п�����оƬ�ڲ�������ʱ����
                        break;
                    }

                    U8Value = XA3_UnlockAndWriteEEPROM(Det.Info[i].Serl.U32Value, 3,  Det.Info[i].Pwd.U8Buffer);
                    if(U8Value == 1)
                    {//��EEPROMʧ��
                        WriteSystemLog("��EEPROMʧ��", 0, STR_INFO);
                        break;
                    }
                    else if(U8Value != 0)
                    {//дʧ��
                        WriteSystemLog("дPWDʧ��", 0, STR_INFO);
                        break;
                    }
                    WriteSystemLog("��дPWD", 0, STR_INFO);     
                }
                else
                {
                    break;
                }
                //��λоƬ���ض�����
                WriteSystemLog("��λ��֤", 0, STR_INFO);
                XA3_Reset(Det.Info[i].Serl.U32Value);
                XA3_Reset(Det.Info[i].Serl.U32Value);
                OSTimeDlyHMSM(0,0,0,50);
                AllDisCharge();
                
                Result = XA3_Mask(Det.Info[i].Serl.U32Value);
                if(ERROR == Result)
                {
                    WriteSystemLog("����ʧ��", 0, STR_INFO);
                    break;
                }
                U8Value = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
                if(U8Value == 0)
                {
                    WriteSystemLog("�����Ѿ�����", 0, STR_INFO);
                    
                    Det.Info[i].Err = 0;
                    GetDetNum();
                    FindUnRegDet = 0;
                    TestResult = TEST_END;  //û�м�⵽δע���׹�
                }
                else if(U8Value == 1)
                {
                    WriteSystemLog("PWD����", 0, STR_INFO);
                }
                else
                {
                    WriteSystemLog("ͨѶ�쳣", 0, STR_INFO);
                }
                break;
            }
        }
    }
    return TestResult;
}
//==================================================================================================
//| �������� | TestDetComplete
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��·��ȫ����
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	TEST_UNDONE  δ�������
//|			 |	TEST_END   �������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-17 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void TestDetComplete(void)
{
	ErrorStatus Result;
    u16 DetStatus;
	u16 i;
    
	/*��ʼ����ǰ�ȳ�ʼ��������뼰����оƬ���ڶ�Ӧ��ʱ����*/
	TestInfoInit();

	//��ʼ������������
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = DetSerlGroupInfo.Count+ Det.RegCount;
	SetProgbarParam();
	
    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	
	
	for(i=0; i < DetSerlGroupInfo.Count; i++)
	{
		Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
		if(DetIsSingleCap(Det.Type) == 0)
		{//˫����
			/*BIT����*/
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
	{//˫����
		OSTimeDlyHMSM(0,0,1,0);//��ʱ�ȴ������׹�BIT�������
	}
		
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����	

			/*��ȡBIT״̬*/
			Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);//�ض����Խ��
			if(ERROR == Result)
			{
				SaveErrCode(i, (1<<4)+ConnectErrorCodeIdentify()) ;       //����������
				continue;//��һ���׹�
			}	
			
            if(XA3_GetDetStatus(DetStatus, XA3_BIT_STATUS) == ERROR)  // �ж�BIT���Ե�״̬
            {//BIT���Դ����־
                if(XA3_GetDetStatus(DetStatus, XA3_CAP_STATUS) == ERROR)
                {//�𱬵��ݺ��Ӵ����־
                    SaveErrCode(i, (3<<4)+DET_TEST_BIT_ERR) ;       //����������	
                }
                else
                {
                    SaveErrCode(i, (4<<4)+DET_TEST_BIT_ERR) ;       //����������	
                }
            }
		}	
	}
    
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();
	
	/*ͳ�ƴ�����*/
	GetDetNum();	

	FillCurrentProgbar();//���½�������100%
}

//==================================================================================================
//| �������� | BlasteTestComplete
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������ȫ����	
//|----------|--------------------------------------------------------------------------------------
//| ������� |	��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 	TEST_END     �������
//|			 |	TEST_UNDONE  δ�������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�ZH    ʱ�䣺2013-08-30 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ʹ��ǰ���ʼ��op_data.u32_temp1Ϊ0xFFFF
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void BlasteTestComplete(void)
{
	ErrorStatus Result;
    u8 ResultCode;
	u16 i;
    u32 TimeClk;
    u8 TimeClkFlag;
    u16 DetStatus;
    
	/*��ʼ����ǰ�ȳ�ʼ��������뼰����оƬ���ڶ�Ӧ��ʱ����*/
	TestInfoInit();

	//��ʼ������������
	ClearProgbarData();
	Op_Data.Progbar.Bar[0].Max = Det.RegCount*2+100+GetFixedPulseCount()/200;
	Op_Data.Progbar.Bar[1].Max = Det.RegCount+DetSerlGroupInfo.Count;
	Op_Data.Progbar.Bar[2].Max = Det.RegCount;
	SetProgbarParam();


    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();
    
	/************************д���ڽ׶�***************************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
            
			Result = XA3_Mask(Det.Info[i].Serl.U32Value);
			if(ERROR == Result)
			{
				SaveErrCode(i,(0<<4)+ConnectErrorCodeIdentify()) ;       //����������
			}
            else
            {
                Det.Info[i].Freq.U32Value = GetDetFreqFromHead();//����ͬ��ͷ�������Ƶ��
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
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
            
			if((Det.Info[i].Freq.U32Value < DET_RECEIVE_FREQ_MIN) || (Det.Info[i].Freq.U32Value > DET_RECEIVE_FREQ_MAX))
			{
				Det.Info[i].Freq.U32Value = 80000;//Ĭ��Ƶ��
			}			
			
			Det.Info[i].Delayclk.U32Value = BlastDelayConvertToDetDelayClock(Det.Info[i].Delay, Det.Info[i].Freq.U32Value);
			if(BlastDelayConvertToDetDelay(Det.Info[i].Delay) != 0)
			{//д��̶�����
                ResultCode = XA3_WriteFixDelayTest(Det.Info[i].Serl.U32Value, BlastDelayConvertToDetDelay(Det.Info[i].Delay));
                if(ResultCode == 1)
                {//����ʧ��
                    SaveErrCode(i,(2<<4)+ConnectErrorCodeIdentify()) ;       //����������
                }
                else if(ResultCode != 0)
                {//д�̶�����ʧ��
                    SaveErrCode(i,(3<<4)+ConnectErrorCodeIdentify()) ;       //����������   
                }				
			}
			else
			{
				/*д����*/
				Result = XA3_WriteTime(Det.Info[i].Serl.U32Value, Det.Info[i].Delayclk.U32Value);
				if(ERROR == Result)
				{
					SaveErrCode(i,(4<<4)+ConnectErrorCodeIdentify()) ;       //����������
				}			
			}			
		}
	}
	
	//��оƬ���ڽ���ʱ��У׼
    if(Det.RegCount > 0)
    {
        if(BlastDelayConvertToDetDelay(BlastDelayConvertToDetDelay(GetDetDelayMaxFromRegStatus())) != 0)
        {
            XA3_CheckOscAll(DET_FIXED_PULSE_MAX_FOR_MS, DET_FIXED_PULSE_MIN_FOR_MS);
            XA3_CheckOscAll(DET_FIXED_PULSE_MAX_FOR_MS, DET_FIXED_PULSE_MIN_FOR_MS);
            XA3_DetFixedPulse(GetFixedPulseCount(), TNUS(100), TNUS(900));
        }
        //����ʱ�����߸��ȶ�Щ
        for(i=0; i < 100; i++)
        {
            OSTimeDly(50);
            Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
        }    
    }

	FillCurrentProgbar();//���½�������100%
	Op_Data.Progbar.Index++;//�л�����һ��������
	
	/************************����׼���𱬽׶�**************************/
	//��оƬ����
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����

            ResultCode = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            if(ResultCode == 1)
            {//����������ʱ��������һ�Σ������оƬ�ڲ����������ʱ���п���ͨ����
                ResultCode = XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
            }
            
			if(ResultCode == 0xFF)
			{
				SaveErrCode(i,(5<<4)+ConnectErrorCodeIdentify()) ;       //����������
			}
            else if(ResultCode != 0)
            {
                SaveErrCode(i,(5<<4)+DET_TEST_PWD_ERR) ;
            }
		}		
	}

	for(i=0; i < DetSerlGroupInfo.Count; i++)
	{
		Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����

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

	FillCurrentProgbar();//���½�������100%
	Op_Data.Progbar.Index++;//�л�����һ��������
	
	/************************��֤��ʱʱ��׶�**************************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����

			Result = XA3_ReadTimeClock(Det.Info[i].Serl.U32Value, &TimeClk, NULL, &TimeClkFlag);
			OSTimeDly(10);//����ʱ��ض����ص����ݽ϶࣬�����ʱ�����ݲ���
			if(ERROR == Result)
			{
				SaveErrCode(i,(6<<4)+ConnectErrorCodeIdentify()) ;       //����������
			}
			else
			{
				/*�жϴ�оƬ�ж�ȡ������ʱʱ����д��ֵ�Ƿ���ͬ*/
//				if(Det.Info[i].delayclk != U32Value)				
				if((TimeClkFlag == 0) || 
					((TimeClk > ((u32)(Det.Info[i].Delayclk.U32Value*1.1)))||(TimeClk < ((u32)(Det.Info[i].Delayclk.U32Value*0.9)))))
//				if((GetDetWriteDelayFLag() == 0) || (Det.Info[i].delayclk != U32Value))
				{//���ڿ���û��д��					
					/*д����*/
					Result = XA3_WriteTime(Det.Info[i].Serl.U32Value, Det.Info[i].Delayclk.U32Value);
					if(ERROR == Result)
					{
						SaveErrCode(i,(7<<4)+ConnectErrorCodeIdentify()) ;       //����������
					}
					
					Result = XA3_ReadTimeClock(Det.Info[i].Serl.U32Value, &TimeClk, NULL, &TimeClkFlag);
					OSTimeDly(10);//����ʱ��ض����ص����ݽ϶࣬�����ʱ�����ݲ���
					if(ERROR == Result)
					{
						SaveErrCode(i,(8<<4)+ConnectErrorCodeIdentify()) ;       //����������
					}
					else
					{
						if((TimeClkFlag == 0) ||(Det.Info[i].Delayclk.U32Value != TimeClk))
						{
							SaveErrCode(i,(8<<4)+DET_TEST_DELAY_CLOCK_ERR) ;       //����ֵ����
						}
					}
					//׼����ָ������оƬ����־��chargeΪ1������ʱ���Ѿ��趨��־Time_ok Ϊ1 ������ǰ���²�����ȷ���գ�
					//�����ʱû��д�룬�ᵼ��׼����ָ��ʧ�ܡ�����Ҫ�ط�׼����ָ��				
//					XA3_ExpenAll();					
				}					
			}
			Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);
			if(ERROR == Result)
			{
				SaveErrCode(i,(9<<4)+ConnectErrorCodeIdentify()) ;       //����������
			}
			else
			{		
				if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) == ERROR)
				{
					if (Det.Info[i].Err == 0)
					{
						SaveRetryErrCode(i,(9<<4)+DET_TEST_CHARGE_ERR) ;       //����쳣,����
					}					
				}
                else if(DetIsSingleCap(Det.Type) == 0)              
                {//˫����
                    if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // �ж�BIT���Ե�״̬,��ʱӦ����1
                    {
                        SaveErrCode(i,(9<<4)+DET_TEST_BIT_ERR) ;                    
                    }
                }
//				if(XA3_GetDetStatus(DetStatus, XA3_EXPEN_STATUS) == ERROR)
//				{
//					SaveErrCode(i,(9<<4)+DET_TEST_EXPEN_ERR) ;       //׼�����쳣					
//				}				
			}			
		}
	}		
	/***************��Գ�缰׼���𱬴����׹����Խ׶�********************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if((DET_TEST_CHARGE_ERR == (Det.Info[i].RetryErr&0x0F)) || (DET_TEST_EXPEN_ERR == (Det.Info[i].RetryErr&0x0F)))
			{
				/************************���ǰ�Ƚ���**************************/
//				DetUnLockPwdAttemptPwd(&Det.Info[i], 0, NULL);
                XA3_PwdUnlock(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value);
				
				/************************�������**************************/
				XA3_Charge(Det.Info[i].Serl.U32Value);
				XA3_Charge(Det.Info[i].Serl.U32Value);
				OSTimeDlyHMSM(0,0,0,50);//��ʱ50ms	

				/************************׼��������**********************/
//				XA3_ExpenAll();
//				XA3_ExpenAll();
//				OSTimeDlyHMSM(0,0,0,5);//��ʱ5ms
			}		
		}
	}
	/*************��Գ�缰׼���𱬴����׹�������֤�׶�*******************/
	for(i=0; i < DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if((DET_TEST_CHARGE_ERR == (Det.Info[i].RetryErr&0x0F)) || (DET_TEST_EXPEN_ERR == (Det.Info[i].RetryErr&0x0F)))
			{
				/*��յ�ǰ�׹ܴ������*/
				SaveErrCode(i, 0x00) ;      

				/*���¶�ȡ��ʱֵ�����ж�*/
				Result = XA3_ReadStatus(Det.Info[i].Serl.U32Value, &DetStatus);
				if(ERROR == Result)
				{
					SaveErrCode(i,(10<<4)+ConnectErrorCodeIdentify()) ;
				}
				else
				{
					if(XA3_GetDetStatus(DetStatus, XA3_CHARGE_STATUS) == ERROR)
					{
						SaveErrCode(i,(10<<4)+DET_TEST_CHARGE_ERR) ;       //����쳣		
					}
                    else if(DetIsSingleCap(Det.Type) == 0)              
                    {//˫����
                        if (XA3_GetDetStatus(DetStatus, XA3_BIT_OUT_STATUS) == ERROR)  // �ж�BIT���Ե�״̬,��ʱӦ����1
                        {
                            SaveErrCode(i,(10<<4)+DET_TEST_BIT_ERR) ;                    
                        }
                    }
//					if(XA3_GetDetStatus(DetStatus, XA3_EXPEN_STATUS) == ERROR)
//					{
//						SaveErrCode(i,(10<<4)+DET_TEST_EXPEN_ERR) ;       //׼�����쳣	
//					}				
				}
			}		
		}
	}
    
	/*ͳ�ƴ�����*/
	GetDetNum();
	FillCurrentProgbar();//���½�������100%
}

/**
  *************************************************************************************
  * @brief  ��д�׹�����
  * @param  NewSerl �µ�������
  * @param  NewPwd �µ�����
  * @retval ��
  * @author ZH
  * @date   2019��4��20��
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
    
    WriteSystemLog("��ʼ��д������:",strlen("��ʼ��д������:"), CURRENT_STATUS);
    WriteSystemLog(&NewSerl->U8Buffer[0], 4, HEX_ARRAY);
    WriteSystemLog(&NewPwd->U8Buffer[0], 4, HEX_ARRAY);
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(NewSerl->U32Value, NewPwd->U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("�¹���:",strlen("�¹���:"), CURRENT_STATUS);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }
    
    //�ȶ��������������
    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("��������ʧ��",strlen("��������ʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("��������ɹ�:",strlen("��������ɹ�:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("��PWDʧ��",strlen("��PWDʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("��PWD�ɹ�:",strlen("��PWD�ɹ�:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
    {
        WriteSystemLog("���Ϲ���",strlen("���Ϲ���"), STR_INFO);
    }
    else
    {
        WriteSystemLog("�����Ϲ���",strlen("�����Ϲ���"), STR_INFO);
    }
    
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("ԭ����:",strlen("ԭ����:"), CURRENT_STATUS);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }

    //д����
    U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 3,  NewPwd->U8Buffer);
    if(U8Value == 1)
    {//��EEPROMʧ��
        WriteSystemLog("��EEPROMʧ��",strlen("��EEPROMʧ��"), STR_INFO);
        return 0; 
    }
    else if(U8Value != 0)
    {//дʧ��
        WriteSystemLog("дʧ��",strlen("дʧ��"), STR_INFO);
        return 0; 
    }
    
    //д������
    U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 2,  NewSerl->U8Buffer);
    if(U8Value == 1)
    {//��EEPROMʧ��
        WriteSystemLog("��EEPROMʧ��",strlen("��EEPROMʧ��"), STR_INFO);
        return 0; 
    }
    else if(U8Value != 0)
    {//дʧ��
        WriteSystemLog("дʧ��",strlen("дʧ��"), STR_INFO);
        return 0; 
    }
    
    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	

    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("�ض�������ʧ��",strlen("�ض�������ʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("�ض�������ɹ�:",strlen("�ض�������ɹ�:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Serl.U32Value != NewSerl->U32Value)
    {
        WriteSystemLog("�����벻һ��",strlen("�����벻һ��"), STR_INFO);
        return 0;
    }
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("�ض�PWDʧ��",strlen("�ض�PWDʧ��"), STR_INFO);
		return 0;
    } 
    WriteSystemLog("�ض�PWD�ɹ�:",strlen("�ض�PWD�ɹ�:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Pwd.U32Value != NewPwd->U32Value)
    {
        WriteSystemLog("PWD��һ��",strlen("PWD��һ��"), STR_INFO);
        return 0;
    }
    WriteSystemLog("������д�ɹ�",strlen("������д�ɹ�"), STR_INFO);
    return 1;
}
/**
  *************************************************************************************
  * @brief  ���ñ��ݵ�������д�׹�����
  * @param  ��
  * @retval 0 ͨѶ�쳣��1��д�ɹ� ��2�������ݲ����Ϲ���
  * @author ZH
  * @date   2019��4��20��
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
    
    WriteSystemLog("��ʼ�ñ�����д����",strlen("��ʼ�ñ�����д����"), CURRENT_STATUS);
    
    //�ȶ��������������
    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("��������ʧ��",strlen("��������ʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("��������ɹ�:",strlen("��������ɹ�:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("��PWDʧ��",strlen("��PWDʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("��PWD�ɹ�:",strlen("��PWD�ɹ�:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
    {
        WriteSystemLog("���Ϲ���",strlen("���Ϲ���"), STR_INFO);
    }
    else
    {
        WriteSystemLog("�����Ϲ���",strlen("�����Ϲ���"), STR_INFO);
    }
    
    memset(Buffer, 0, sizeof(Buffer));
    if(XA3_DetInside2Outside(Serl.U32Value, Pwd.U32Value, Buffer) != ERROR)
    {
        WriteSystemLog("ԭ����:",strlen("ԭ����:"), STR_INFO);
        WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
    }
    
    for(i=0;i<8;i++)
    {
        if((i != 2) &&(i != 3))
        {
            if(XA3_ReadEEPROM(i, (u8*)&Other.U8Buffer[0]) == ERROR)
            {
                sprintf((char*)Buffer, "����ַ%dʧ��", i);
                WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);
                return 0;
            }
            sprintf((char*)Buffer, "����ַ%d�ɹ�:", i);
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
        WriteSystemLog("��������Ϲ���",strlen("��������Ϲ���"), STR_INFO);
    }
    else
    {
        WriteSystemLog("�����벻���Ϲ���",strlen("�����벻���Ϲ���"), STR_INFO);
        return 2;
    }
    if((NewSerl.U32Value == Serl.U32Value) && (NewPwd.U32Value == Pwd.U32Value))
    {
        WriteSystemLog("�¾�������ͬ",strlen("�¾�������ͬ"), STR_INFO);
        
        Op_Data.RegDetInfo.Serl.U32Value = Serl.U32Value;//���ڽ�����ʾ
        Op_Data.RegDetInfo.Pwd.U32Value = Pwd.U32Value;
        return 3;
    }
    if(NewPwd.U32Value != Pwd.U32Value)
    {
        WriteSystemLog("��Ҫ��дPWD",strlen("��Ҫ��дPWD"), STR_INFO);
        //д����
        U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 3,  NewPwd.U8Buffer);
        if(U8Value == 1)
        {//��EEPROMʧ��
            WriteSystemLog("��EEPROMʧ��",strlen("��EEPROMʧ��"), STR_INFO);
            return ERROR; 
        }
        else if(U8Value != 0)
        {//дʧ��
            WriteSystemLog("дʧ��",strlen("дʧ��"), STR_INFO);
            return ERROR; 
        }    
    }

    if(NewSerl.U32Value != Serl.U32Value)
    {
        WriteSystemLog("��Ҫ��д������",strlen("��Ҫ��д������"), STR_INFO);
        //д������
        U8Value = XA3_UnlockAndWriteEEPROM(Serl.U32Value, 2,  NewSerl.U8Buffer);
        if(U8Value == 1)
        {//��EEPROMʧ��
            WriteSystemLog("��EEPROMʧ��",strlen("��EEPROMʧ��"), STR_INFO);
            return ERROR; 
        }
        else if(U8Value != 0)
        {//дʧ��
            WriteSystemLog("дʧ��",strlen("дʧ��"), STR_INFO);
            return ERROR; 
        }    
    }
    
    AllReset();
	/*����ȫ�ָ�λָ��*/
	AllDisCharge();	

    Result = XA3_ReadSerl(&Serl.U32Value); 
	if(ERROR == Result)
	{
        WriteSystemLog("�ض�������ʧ��",strlen("�ض�������ʧ��"), STR_INFO);
		return 0;
    }
    WriteSystemLog("�ض�������ɹ�:",strlen("�ض�������ɹ�:"), STR_INFO);
    WriteSystemLog(&Serl.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Serl.U32Value != NewSerl.U32Value)
    {
        WriteSystemLog("�����벻һ��",strlen("�����벻һ��"), STR_INFO);
        return 0;
    }
    
    Result = XA3_ReadPwdRetry(Serl.U32Value, &Pwd.U32Value);  
	if(ERROR == Result)
	{
        WriteSystemLog("�ض�PWDʧ��",strlen("�ض�PWDʧ��"), STR_INFO);
		return 0;
    } 
    WriteSystemLog("�ض�PWD�ɹ�:",strlen("�ض�PWD�ɹ�:"), STR_INFO);
    WriteSystemLog(&Pwd.U8Buffer[0], 4, HEX_ARRAY);
    
    if(Pwd.U32Value != NewPwd.U32Value)
    {
        WriteSystemLog("PWD��һ��",strlen("PWD��һ��"), STR_INFO);
        return 0;
    }
    WriteSystemLog("������д�ɹ�",strlen("������д�ɹ�"), STR_INFO);
    
    Op_Data.RegDetInfo.Serl.U32Value = Serl.U32Value;//���ڽ�����ʾ
    Op_Data.RegDetInfo.Pwd.U32Value = Pwd.U32Value;
    return 1;
}
/**
  *************************************************************************************
  * @brief  ��ȡ�׹ܷ���ʱ�ĵ�������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��03��
  * @note   
  *************************************************************************************  
  */
void GetDetAckCurrentWave(void)
{
	u16 i;
	
	if(Det.RegCount == 0)
	{
		XA3_DetCheckInNotAck();//û��ע���׹�ʱ�����Ͳ�©����
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
  * @brief  �ж������Ƿ���δע���׹�
  * @param  ��
  * @retval 0��ʾû�У�1��ʾ��
  * @author ZH
  * @date   2018��3��25��
  * @note   ��©,����5�η��͵�����������ݷ�����˵����δע���׹�
  *************************************************************************************  
  */
u8 HaveUnregisteredDetBySimpleSignal(void)
{
	u8 i;
	
	/*��©,����5�η��͵�����������ݷ�����˵����δע���׹�*/
	for(i=0; i < 5; i++)
	{
		if(ERROR == XA3_DetCheckInBySimpleSignal())
		{
			return 0;  //û�м�⵽δע���׹�
		}
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  ���׹�
  * @param  ��
  * @retval 0��ʾû�У�1��ʾ��
  * @author ZH
  * @date   2018��3��25��
  * @note   �������ź�ǰ������׼����ָ��
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
        
        sprintf((char*)Buffer, "׼����ǰ�쳣:%dmV", Voltage);
        WriteSystemLog(Buffer, 0, NEW_STR_INFO);
        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)//�ӻ�ʱ����������
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
        
        sprintf((char*)Buffer, "��ǰ�쳣:%dmV", Voltage);
        WriteSystemLog(Buffer, 0, NEW_STR_INFO);
        if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)//�ӻ�ʱ����������
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
  * @brief  ���׹�, ͨ���½������ȼ�����ʽ����
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2020��3��17��
  * @note   
  *************************************************************************************  
  */
ErrorStatus CreateStartBlastTask(void)
{
    OSTaskDel(APP_DET_OPERATE_TASK_PRIO);
    TaskCreate(APP_DET_OPERATE_TASK_PRIO, (void*)100);
    while(TaskIsExist(APP_DET_OPERATE_TASK_PRIO))
    {//�ȴ����ͽ���
        OSTimeDly(1);
    }
    
    return (ErrorStatus)AppTaskResult.DetOperate;
}

