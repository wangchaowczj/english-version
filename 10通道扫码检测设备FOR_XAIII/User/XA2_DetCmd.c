#include "includes.h"

static const u8 XA2_RetryCount = 2;//ͨѶʧ��ʱ����

/**
  *************************************************************************************
  * @brief  �������͵�ָ���ֽڱ����λ
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ��� 
  * @retval ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
void XA2_DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(300); //��ʼλ
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
				Verify++;//1��λ��
			}
			else
			{//0
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
			}
	        Mask <<= 1 ;			
		}
		if(Verify&0x01)
		{//��������ʱ����żУ��
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;			
		}
		else
		{
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
		}
	}
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(2200);//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}

/**
  *************************************************************************************
  * @brief  �Զ���ָ��
  * @param  Cmd Ҫ���͵�����
  * @param  Bytes Ҫ���͵������
  * @param  ErrorRetry ������Ҫ���ԵĴ���
  * @param  RecBytes Ҫ���յ����ݳ��ȣ�ͬ��ͷ��1���ֽڣ�  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus XA2_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes)
{
	XA2_DetCmdEncode(Cmd, Bytes);
		
	return XA2_DetSendAndRec(ErrorRetry, RecBytes) ;    
}

/**
  *************************************************************************************
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ȫ�������λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ȫ��Ӳ����λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ����ȫ�ֳ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ȫ��׼��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ��EEPROM����
  * @param  Addr ��ַ
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ���ܶ�ȡ�������ݣ�����ֻ���������ȡָ���ȡ)
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
  * @brief  ��оƬ������
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
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
  * @brief  дEERPOM����
  * @param  Addr ��ַ
  * @param  Buffer Ҫд�������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ���ݱ��ָ������ڡ�дEE����2�����ͺ������������Ч����������ָ��ÿ��д��4���ֽ�����
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
  * @brief  ����������
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ÿ���ϵ���ɺ�������ȡһ�Σ�Ҫ�ٴζ�ȡ�������ϵ縴λ����ȫ��Ӳ����λ��ȴ�EEPROM���ݶ�ȡ��ɺ���
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
  * @brief  ��������
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ��������
  * @param  Serl ������
  * @retval 0-�Ѿ�����;1-//����ʧ��;0xFF-ͨѶ�쳣
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
            return 0;//�Ѿ�����
        }
        return 1;//����ʧ��
    }
    
	return 0xFF;//ͨѶ�쳣
}
/**
  *************************************************************************************
  * @brief  д�̶����ڽ�������
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��ʹ����д��̶�ʱ��һ�κ��Զ�ʧЧ
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
  * @brief  д���ݽ�������
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note  ��дEE����ָ��2��ʹ����ÿ��д��4���ֽ�������ɺ�ʧЧ�������Ҫд�������ݣ��������·��͡�дEE����ָ��2��
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
  * @brief  �̶�����ʱ����
  * @param  Serl ������
  * @param  FixDelay �̶�����ʱ��
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   �̶�ʱ����ָ������ڡ�дEE����1�����ͺ������������Ч�� ��ʹ����д��̶�ʱ��һ�κ��Զ�ʧЧ
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
  * @brief  ����Ӳ����λ
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ȫ��ʱ��У׼
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  д����ʱ��
  * @param  Serl ������
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ����ʱ��ض�
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ״̬�ض�
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ״̬�ض�
  * @param  Serl ������
  * @retval ���
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ״̬�ض�
  * @param  ���(�������5λ)
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ��
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
  * @brief  ����������
  * @param  ���(�������5λ)
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ���ڽ���״̬�½��У������ָ����Ч
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
  * @brief  ȫ�ֳ������
  * @param  ���(�������5λ)
  * @retval ��
  * @author ZH
  * @date   2019��1��22��
  * @note   ȫ�ֳ��ָ������ȷ���ȫ�ֳ������ָ����ٷ���
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
  * @brief  �ж�оƬ״̬�Ƿ�����
  * @param  Ser1 оƬ״̬
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR
  * @author ZH
  * @date   2015��8��7��
  * @note   ״̬�ض����ж�״̬λ
  *************************************************************************************  
  */
ErrorStatus XA2_GetDetStatus(u16 DetStatus, XA2_DET_STATUS_BIT StatusBit)
{
	switch(StatusBit)
	{
		case XA2_DELAY_ZERO_STATUS:
			if((DetStatus&0x40)  == 0x00)
			{
				return SUCCESS;//�Ѿ�д������
			}		
			break;
		case XA2_PWD_UNLOCK_STATUS:
			if(DetStatus&0x80)
			{
				return SUCCESS;//�Ѿ�����
			}		
			break;
		case XA2_EXPEN_STATUS:
			if(DetStatus&0x0100)
			{
				return SUCCESS;//�Ѿ����յ�׼����ָ��
			}		
			break;
		case XA2_CAP_STATUS:
			if((DetStatus&0x0200) == 0x00)
			{
				return SUCCESS;//�𱬵��ݺ�������
			}		
			break;
		case XA2_BIT_STATUS:
			if((DetStatus&0x0400) == 0x00)
			{
				return SUCCESS;//BIT��������
			}		
			break;
		case XA2_CHARGE_STATUS:
			if(DetStatus&0x0800)
			{
				return SUCCESS;//�Ѿ����յ��������
			}		
			break;			
		case XA2_FIX_DELAY_UNLOCK_STATUS:
			if(DetStatus&0x1000)
			{
				return SUCCESS;//д�̶������Ѿ�����
			}		
			break;
		case XA2_EEPROM_UNLOCK_STATUS:
			if(DetStatus&0x2000)
			{
				return SUCCESS;//дEEPROM�Ѿ�����
			}		
			break;
		case XA2_TIME_STATUS:
			if(DetStatus&0x4000)
			{
				return SUCCESS;//оƬ����ʱ���Ѿ��趨
			}		
			break;
		case XA2_BIT_OUT_STATUS:
			if((DetStatus&0x8000) == 0x00)
			{
				return SUCCESS;//BIT���ԱȽ������״̬����(�Ѿ����)
			}		
			break;			
		default:
			break;
	}

	return ERROR;
}



/**
  *************************************************************************************
  * @brief  �Զ������ź�
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ���
  * @retval ��
  * @author ZH
  * @date   2019��1��28��
  * @note   ��
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

	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
    return XA2_DetSendAndRec(0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  �������͵�У׼�����ֽڱ����λ
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ��� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void XA2_DetFixedPulseEncode(u16 Count, u32 HihtHalf, u32 LowHalf)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = HihtHalf; //���ܳ���320uS,����оƬ�����жϻ����
	DetProtocol.SendBuffer.Timer[1] = LowHalf;
}
/**
  *************************************************************************************
  * @brief  ����У׼����(���˫����оƬ����������)
  * @param  ��  
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void XA2_DetFixedPulse(u16 Count, u32 HihtHalf, u32 LowHalf)
{
	XA2_DetFixedPulseEncode(Count, HihtHalf, LowHalf);
    XA2_DetSendAndRec(0, 0) ; 
}

