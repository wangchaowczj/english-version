#include "includes.h"

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
void DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; //��ʼλ
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
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T800US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
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
void DetCmdEncodeForBlastSignalPin(u8 *Ptr, u8 Bytes)
{
	DetCmdEncode(Ptr, Bytes);
	DetProtocol.Mode = DET_CMD_BLAST;
}

/**
  *************************************************************************************
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
    
	DetProtocol.Hook = HOOK_IDLE;
	
	return DetSendAndRec(2, 1, 0) ;
}
/**
  *************************************************************************************
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ֻҪRISE���յ��źž���Ϊ���׹�����,���ڶ෢�׹�ʱ
  *************************************************************************************  
  */
ErrorStatus DetCheckInBySimpleSignal(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result; 
	
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
	DetProtocol.Hook = HOOK_CHECK_IN;	
	Result = DetSendAndRec(2, 1, 0) ;
	DetProtocol.Hook = HOOK_IDLE;
	
	return Result;
}
/**
  *************************************************************************************
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��03��
  * @note   ֻ���ͣ�������
  *************************************************************************************  
  */
void DetCheckInNotAck(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKIN ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}
/**
  *************************************************************************************
  * @brief  �����׹�ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetMask(u8 Ser1, u8 Ser2, u8 Ser3)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = MASK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;
}
/**
  *************************************************************************************
  * @brief  дEEPROMָ��
  * @param  Ptr ������1��������2��������3������1������2��������š��������š����
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   оƬ���յ���������ָ��������������д��EEPROM��
  *         �ܹ���Ҫ�ȴ�6146���ڲ�ʱ������
  *         �����ʱ���ڲ�������������������������ݱ�̿��ܳ���
  *************************************************************************************  
  */
ErrorStatus DetWrEEPROM(u8* Ptr)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = WREEPROM ;
	for(i=0; i<8; i++)
	{
		Cmd[Count++] = *(Ptr+i) ;//������1��������2��������3������1������2��������š��������š����
	}
	
	DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = DetSendAndRec(0, 1, 0);
		OSTimeDlyHMSM(0,0,0,150);
		if(Result != ERROR)
		{
			break;
		}
	}
	return Result;
}

/**
  *************************************************************************************
  * @brief  ����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3 
  * @param  Pwd1 ����1
  * @param  Pwd2 ����2  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ���ܺ���ܷ����
  *************************************************************************************  
  */
ErrorStatus DetUnLockPwd(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	
	Cmd[Count++] = PWD_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Pwd1 ;
	Cmd[Count++] = Pwd2 ;
	
	DetCmdEncode(Cmd, Count);
	return DetSendAndRec(2, 1, 0);
}
/**
  *************************************************************************************
  * @brief  ����ָ��,����ʱ������
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3 
  * @param  Pwd1 ����1
  * @param  Pwd2 ����2  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��14��
  * @note   ���ܺ���ܷ����
  *************************************************************************************  
  */
ErrorStatus DetUnLockPwdNotRetry(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	
	Cmd[Count++] = PWD_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Pwd1 ;
	Cmd[Count++] = Pwd2 ;
	
	DetCmdEncode(Cmd, Count);
	return DetSendAndRec(0, 1, 0);
}
/**
  *************************************************************************************
  * @brief  EEPROM����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetUnLockEEPROM(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EE_UNLOCK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;
}

/**
  *************************************************************************************
  * @brief  �̶��ֶα��ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3
  * @param  Time �ֶ���  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   оƬ���յ�ָ��������������д��EEPROM��
  *         �ܹ���Ҫ�ȴ�1539���ڲ�ʱ������
  *         �����ʱ���ڲ�������������������������ݱ�̿��ܳ���
  *************************************************************************************  
  */
ErrorStatus DetFixed(u8 Ser1, u8 Ser2, u8 Ser3, u16 Time)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = FIXED ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	Cmd[Count++] = Time&0xFF ;
	Cmd[Count++] = (Time>>8)&0x7F ;
	
	DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = DetSendAndRec(0, 1, 0);
		OSTimeDlyHMSM(0,0,0,40);
		if(Result != ERROR)
		{
			break;
		}
	}

	return Result;
}

/**
  *************************************************************************************
  * @brief  ʱ��У׼ָ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   оƬ������ʱ��У׼ָ���ֱ�ӽ���ȴ���׼����״̬
  *************************************************************************************  
  */
void DetCheckOSC(u8 Max, u8 Min)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHECKOSC ;
	Cmd[Count++] = 0x00 ;
	Cmd[Count++] = Max ;
	Cmd[Count++] = Min&0x7F ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}
/**
  *************************************************************************************
  * @brief  ��λָ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetClrAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CLRALL ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
}

/**
  *************************************************************************************
  * @brief  �ض�������
  * @param  ��
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus DetSerlRd(void)
{	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = SERLRD ;
	Cmd[Count++] = 0x00 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 7, 0) ;	
}
/**
  *************************************************************************************
  * @brief  BIT����
  * @param  Ser1Group ������1��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetBit(u8 Ser1Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BIT ;
	Cmd[Count++] = Ser1Group&0x1F ;
	
	DetCmdEncode(Cmd, Count);
	
	DetSendAndRec(0, 0, 0);
}

/**
  *************************************************************************************
  * @brief  �ض�����ʱ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetTimeRd(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 6, 0) ;
}
/**
  *************************************************************************************
  * @brief  �ض�����ʱ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetTimeRdForFreqMeasure(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 6, 1) ;
}
/**
  *************************************************************************************
  * @brief  д����ʱ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3 
  * @param  Delay ����(��Ӧ�׹�����ʱ�Ӹ���)   
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetTimeWr(u8 Ser1, u8 Ser2, u8 Ser3, u32 Delay)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = TIMEWR;
	Cmd[Count++] = Ser1;
	Cmd[Count++] = Ser2;
	Cmd[Count++] = Ser3;
	Cmd[Count++] = Delay&0xFF;
	Cmd[Count++] = (Delay>>8)&0xFF;
	Cmd[Count++] = (Delay>>16)&0x3F;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 1, 0) ;   
}

/**
  *************************************************************************************
  * @brief  ���ָ��
  * @param  Ser1Group ������1��
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetCharge(u8 Ser1Group)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = CHARGE ;
	Cmd[Count++] = Ser1Group&0x1F ;
	
	DetCmdEncode(Cmd, Count);
	
	DetSendAndRec(0, 0, 0);
}
/**
  *************************************************************************************
  * @brief  ׼����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3   
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetExpen(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EXPEN;
	Cmd[Count++] = 0x00;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;  
}
/**
  *************************************************************************************
  * @brief  ׼����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3   
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetExpenByBlastSignalPin(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = EXPEN;
	Cmd[Count++] = 0x00;
	
	DetCmdEncodeForBlastSignalPin(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;  
}
/**
  *************************************************************************************
  * @brief  �ض�״̬
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus DetStatusRd(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = STATUSRD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	return DetSendAndRec(2, 2, 0) ;
}
/**
  *************************************************************************************
  * @brief  �ض�״̬
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @param  Ser3 ������3  
  * @retval ��
  * @author ZH
  * @date   2016��11��03��
  * @note   ֻ���ͣ�������
  *************************************************************************************  
  */
void DetStatusRdNotAck(u8 Ser1, u8 Ser2, u8 Ser3)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = STATUSRD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	Cmd[Count++] = Ser3 ;
	
	DetCmdEncode(Cmd, Count);
		
	DetSendAndRec(0, 0, 0) ;
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
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus GetDetStatus(DET_STATUS_BIT StatusBit)
{
	switch(StatusBit)
	{
		case EXPEN_STATUS:
			if(DetProtocol.RecData.Data[1]&0x01)
			{
				return SUCCESS;
			}		
			break;
		case CAP_STATUS:
			if((DetProtocol.RecData.Data[1]&0x02) == 0x00)
			{
				return SUCCESS;
			}		
			break;
		case BIT_STATUS:
			if((DetProtocol.RecData.Data[1]&0x04) == 0x00)
			{
				return SUCCESS;
			}		
			break;
		case CHARGE_STATUS:
			if(DetProtocol.RecData.Data[1]&0x08)
			{
				return SUCCESS;
			}		
			break;			
		case DISCHARGE_STATUS:
			if(DetProtocol.RecData.Data[1]&0x10)
			{
				return SUCCESS;
			}		
			break;
		case TIME_STATUS:
			if(DetProtocol.RecData.Data[1]&0x40)
			{
				return SUCCESS;
			}		
			break;
		case BIT_OUT_STATUS:
			if((DetProtocol.RecData.Data[1]&0x80) == 0x00)
			{
				return SUCCESS;
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
void DetOneBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;	//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //��
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	//�ߣ��½��ش�����
    DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}
/**************************************************/
/*�������ܣ��������ź�,ֻ��1��		  		  */
/*�����������								  	  */
/*����ֵ��  ��									  */
/**************************************************/
void DetBlastOne(void)
{
	DetOneBlastCmdEncode();
    DetSendAndRec(0, 0, 0) ; 
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
void DetBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;	//��
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; //��
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	//�ߣ��½��ش�����

	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T1000US;	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}
/**************************************************/
/*�������ܣ��������ź�		  				  */
/*�����������								  	  */
/*����ֵ��  ��									  */
/**************************************************/
void DetBlast(void)
{
	DetBlastCmdEncode();
    DetSendAndRec(0, 0, 0) ; 
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
void DetFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //���ܳ���320uS,����оƬ�����жϻ����
	DetProtocol.SendBuffer.Timer[1] = TNUS(900);
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
void DetSingleCapFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //���ܳ���320uS,����оƬ�����жϻ����
	DetProtocol.SendBuffer.Timer[1] = TNUS(904.5);
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
void DetDoubleCapFixedPulseEncode(u16 Count)
{
	DetProtocol.Mode = DET_FIXED_PULSE;
	DetProtocol.SendCmd.Count = 0;
	DetProtocol.SendBuffer.Count = Count*2;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[0] = T100US; //���ܳ���320uS,����оƬ�����жϻ����
	DetProtocol.SendBuffer.Timer[1] = TNUS(901.5);
}
/**
  *************************************************************************************
  * @brief  ����У׼����
  * @param  ��  
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetFixedPulse(u16 Count)
{
	DetFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
}
/**
  *************************************************************************************
  * @brief  ����У׼����(��Ե�����оƬ����������)
  * @param  ��  
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
void DetSingleCapFixedPulse(u16 Count)
{
	DetSingleCapFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
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
void DetDoubleCapFixedPulse(u16 Count)
{
	DetDoubleCapFixedPulseEncode(Count);
    DetSendAndRec(0, 0, 0) ; 
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


