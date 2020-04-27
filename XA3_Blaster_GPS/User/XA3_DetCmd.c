#include "includes.h"

static const u8 XA3_RetryCount = 2;//ͨѶʧ��ʱ������

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
void XA3_DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
    DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(300); //��ʼλ
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
  * @brief  �������͵�ָ���ֽڱ����λ
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ��� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ͨ�����ź��Ǹ��ܽŷ���ȥ
  *************************************************************************************  
  */
void XA3_DetCmdEncodeForBlastSignalPin(u8 *Ptr, u8 Bytes)
{
	XA3_DetCmdEncode(Ptr, Bytes);
	DetProtocol.Mode = DET_CMD_BLAST;
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
ErrorStatus XA3_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes)
{
	XA3_DetCmdEncode(Cmd, Bytes);
		
	return XA3_DetSendAndRec(ErrorRetry, RecBytes) ;    
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
ErrorStatus XA3_DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
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
void XA3_DetCheckInNotAck(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(0, 0) ;
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
ErrorStatus XA3_DetCheckInBySimpleSignal(void)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
	DetProtocol.Hook = HOOK_CHECK_IN;		
	Result = XA3_DetSendAndRec(XA3_RetryCount, 1) ;
    DetProtocol.Hook = HOOK_IDLE;
    
    return Result;
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
void XA3_ClearAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CLEAR_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(0, 0) ;
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
void XA3_ResetAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_RESET_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(0, 0) ;
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
void XA3_ChargeAllEnable(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_EN_CHARGE_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(0, 0) ;
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
void XA3_ExpenAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_EXPEN_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
void XA3_ExpenAllByBlastSignalPin(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_EXPEN_ALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncodeForBlastSignalPin(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
ErrorStatus XA3_ReadEEPROM(u8 Addr, u8* Buffer)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
    
	Cmd[Count++] = XA3_SERLRD ;
	Cmd[Count++] = Addr&0x07 ;
    Cmd[Count++] = 0x00 ;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	Result = XA3_DetSendAndRec(XA3_RetryCount, 5) ;
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
ErrorStatus XA3_ReadSerl(u32* Serl)
{
    ErrorStatus Result;
    u8 Buffer[4];
    
    Result = XA3_ReadEEPROM(2, Buffer);
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
ErrorStatus XA3_WriteEEPROM(u8 Addr, u8* Buffer)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
    
	Cmd[Count++] = XA3_WR_EEPROM ;
	Cmd[Count++] = Addr&0x07 ;
    for(i=0; i<4; i++)
    {
        Cmd[Count++] = Buffer[i] ;
    }   
	
	XA3_DetCmdEncode(Cmd, Count);
		
	Result = XA3_DetSendAndRec(0, 1) ;
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
ErrorStatus XA3_ReadPwd(u32 Serl, u32* Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
    
	Cmd[Count++] = XA3_RD_PWD ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
    Result = XA3_DetSendAndRec(XA3_RetryCount, 5) ;
    
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
ErrorStatus XA3_Mask(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_MASK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  ��������
  * @param  Serl ������
  * @retval 0-�Ѿ�����;1-//����ʧ��;0xFF-ͨѶ�쳣
  * @author ZH
  * @date   2019��1��22��
  * @note   ���ñ�����ǰ����Ҫȷ��оƬ�������ѽ���״̬
  *************************************************************************************  
  */
u8 XA3_PwdUnlock(u32 Serl, u32 Pwd)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_PWD_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;

	Cmd[Count++] = Pwd&0xFF;
    Cmd[Count++] = (Pwd>>8)&0xFF;
    Cmd[Count++] = (Pwd>>16)&0xFF;
    Cmd[Count++] = (Pwd>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
	Result = XA3_DetSendAndRec(XA3_RetryCount, 1) ;	
    
    if(Result != ERROR)
    {
        if(DetProtocol.RecData.Data[0]&0x80)
        {
            return 0;//�Ѿ�����
        }
        return 1;//��û�н��ܣ��������
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
ErrorStatus XA3_WriteFixDelayUnlock(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_WR_FIX_DELAY_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
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
ErrorStatus XA3_WriteDataUnlock(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_WR_EE_DATA_UNLOCK ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
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
ErrorStatus XA3_WriteFixDelay(u32 Serl, u16 FixDelay)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_WR_FIX_DELAY ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	Cmd[Count++] = FixDelay&0xFF;
    Cmd[Count++] = (FixDelay>>8)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
    
	Result = XA3_DetSendAndRec(0, 1) ;
    OSTimeDly(50);
	return Result;
}
u8 XA3_WriteFixDelayTest(u32 Serl, u16 FixDelay)
{
    u8 Retry = 3;
    
    while(Retry--)
    {
        /*��EEPROM*/
        if(ERROR == XA3_WriteFixDelayUnlock(Serl))
        {
            return 1;
        }

        /*�ֶ�ʽ����*/
        if(ERROR != XA3_WriteFixDelay(Serl, FixDelay))
        {
            return 0;
        }        
    }
    return 2;//д�̶�����ʧ��
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
ErrorStatus XA3_Reset(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_RESET ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
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
void XA3_CheckOscAll(u8 Max, u8 Min)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHECKOSC_ALL ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = 0x00;
    Cmd[Count++] = Max&0x7F;
    Cmd[Count++] = Min&0x7F;
	
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
ErrorStatus XA3_WriteTime(u32 Serl, u32 DelayClock)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_WR_TIME ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
	
    Cmd[Count++] = DelayClock&0xFF;
    Cmd[Count++] = (DelayClock>>8)&0xFF;
    Cmd[Count++] = (DelayClock>>16)&0xFF;
    
	XA3_DetCmdEncode(Cmd, Count);
		
	return XA3_DetSendAndRec(XA3_RetryCount, 1) ;
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
ErrorStatus XA3_ReadTimeClock(u32 Serl, u32* TimeClk, u16* FixDelay, u8* TimeClkFlag)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_RD_TIME ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA3_DetCmdEncode(Cmd, Count);
		
	Result = XA3_DetSendAndRec(XA3_RetryCount, 6) ;
    
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
ErrorStatus XA3_ReadStatus(u32 Serl, u16* Status)
{
    ErrorStatus Result;
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_RD_STATUS ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA3_DetCmdEncode(Cmd, Count);
	Result = XA3_DetSendAndRec(XA3_RetryCount, 2) ;
    
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
void XA3_ReadStatusNotAck(u32 Serl)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_RD_STATUS ;
	Cmd[Count++] = Serl&0xFF;
    Cmd[Count++] = (Serl>>8)&0xFF;
    Cmd[Count++] = (Serl>>16)&0xFF;
    Cmd[Count++] = (Serl>>24)&0xFF;
    
	XA3_DetCmdEncode(Cmd, Count);
	XA3_DetSendAndRec(0, 0) ;
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
void XA3_Bit(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_BIT ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
void XA3_Charge(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHARGE ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
void XA3_ChargeAll(u8 Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = XA3_CHARGE_ALL ;
	Cmd[Count++] = 0x00;
    Cmd[Count++] = Group&0x1F;
    
	XA3_DetCmdEncode(Cmd, Count);
		
	XA3_DetSendAndRec(XA3_RetryCount, 0) ;
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
ErrorStatus XA3_GetDetStatus(u16 DetStatus, XA3_DET_STATUS_BIT StatusBit)
{
	switch(StatusBit)
	{
		case XA3_DELAY_ZERO_STATUS:
			if((DetStatus&0x40)  == 0x00)
			{
				return SUCCESS;//�Ѿ�д������
			}		
			break;
		case XA3_PWD_UNLOCK_STATUS:
			if(DetStatus&0x80)
			{
				return SUCCESS;//�Ѿ�����
			}		
			break;
		case XA3_EXPEN_STATUS:
			if(DetStatus&0x0100)
			{
				return SUCCESS;//�Ѿ����յ�׼����ָ��
			}		
			break;
		case XA3_CAP_STATUS:
			if((DetStatus&0x0200) == 0x00)
			{
				return SUCCESS;//�𱬵��ݺ�������
			}		
			break;
		case XA3_BIT_STATUS:
			if((DetStatus&0x0400) == 0x00)
			{
				return SUCCESS;//BIT��������
			}		
			break;
		case XA3_CHARGE_STATUS:
			if(DetStatus&0x0800)
			{
				return SUCCESS;//�Ѿ����յ��������
			}		
			break;			
		case XA3_FIX_DELAY_UNLOCK_STATUS:
			if(DetStatus&0x1000)
			{
				return SUCCESS;//д�̶������Ѿ�����
			}		
			break;
		case XA3_EEPROM_UNLOCK_STATUS:
			if(DetStatus&0x2000)
			{
				return SUCCESS;//дEEPROM�Ѿ�����
			}		
			break;
		case XA3_TIME_STATUS:
			if(DetStatus&0x4000)
			{
				return SUCCESS;//оƬ����ʱ���Ѿ��趨
			}		
			break;
		case XA3_BIT_OUT_STATUS:
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
  * @brief  �������͵�ָ���ֽڱ����λ
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ��� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void XA3_DetOneBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
    DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//�� 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(14000);//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//��
    
    DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;//�ߣ��½��ش�����   
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}
/**
  *************************************************************************************
  * @brief  �������ź�,ֻ����1��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��28��
  * @note   ��
  *************************************************************************************  
  */
void XA3_DetBlastOne(void)
{
	XA3_DetOneBlastCmdEncode();
    XA3_DetSendAndRec(0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  �������͵�ָ���ֽڱ����λ
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ��� 
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void XA3_DetBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//�� 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(14000);//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//��
    
    DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;//�ߣ��½��ش�����   
    
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//�� 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(14000);//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = TNUS(28000);//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;//�ߣ��½��ش����� 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}

/**
  *************************************************************************************
  * @brief  �������ź�
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��1��28��
  * @note   ��
  *************************************************************************************  
  */
void XA3_DetBlast(void)
{
	XA3_DetBlastCmdEncode();
    XA3_DetSendAndRec(0, 0) ; 
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
ErrorStatus XA3_CustomDetBlast(u16 *Ptr, u8 Count)
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
    return XA3_DetSendAndRec(0, 0) ; 
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
void XA3_DetFixedPulseEncode(u16 Count, u32 HihtHalf, u32 LowHalf)
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
void XA3_DetFixedPulse(u16 Count, u32 HihtHalf, u32 LowHalf)
{
	XA3_DetFixedPulseEncode(Count, HihtHalf, LowHalf);
    XA3_DetSendAndRec(0, 0) ; 
}

