#include "includes.h"

static const u8 BJ_RetryCount = 2;//ͨѶʧ��ʱ����

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
void BJ_DetCmdEncode(u8 *Ptr, u8 Bytes)
{
    u8 Verify, Mask, i,j;

	DetProtocol.Mode = DET_CMD;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; //��ʼλ
    
    for(i=0;i<9;i++)
	{
		DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;
		DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;	
	}

	for(i=0; i<Bytes; i++)
	{
		Verify = 0;
		Mask = 1;
		for(j=0; j<8; j++)
		{
			if(Ptr[i]&Mask)
			{//1
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
				Verify++;//1��λ��
			}
			else
			{//0
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US; 
				DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;			
			}
	        Mask <<= 1 ;			
		}
		if(Verify&0x01)
		{//��������ʱ����żУ��
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;			
		}
		else
		{
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US; 
			DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T50US;			
		}
	}
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T400US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
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
ErrorStatus BJ_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes)
{
	BJ_DetCmdEncode(Cmd, Bytes);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;    
}
/**
  *************************************************************************************
  * @brief  ��©ָ�����׹��Ƿ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetCheckIn(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CHECK_IN ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  ��λָ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
void BJ_DetClrAll(void)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CLRALL ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	BJ_DetSendAndRec(0, 0) ;
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
ErrorStatus BJ_DetSerlRd(void)
{	
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 Buffer[6];
	u8 i;
	u8 isExist = 0;
	
	Cmd[Count++] = BJ_SERLRD ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;
	
	for(i=0; i<5;i++)
	{
		BJ_DetCmdEncode(Cmd, Count);
		if(ERROR != BJ_DetSendAndRec(BJ_RetryCount, 7))
		{
			if(isExist)
			{
				if(memcmp(Buffer, (char*)&DetProtocol.RecData.Data[1], 6) == 0)
				{
					return SUCCESS;
				}
			}
			isExist = 1;
			memcpy(Buffer, (char*)&DetProtocol.RecData.Data[1], 6);
		}		
	}

	return ERROR;	
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
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetMask(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_MASK ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;
}
/**
  *************************************************************************************
  * @brief  дEEPROMָ��
  * @param  Ptr Serl1	Serl2	Code1	factory	Group1	Group2	Year
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���յ���������ָ��������������д��EEPROM��
  *         �ܹ���Ҫ�ȴ�6146���ڲ�ʱ������
  *         �����ʱ���ڲ�������������������������ݱ�̿��ܳ���
  *************************************************************************************  
  */
ErrorStatus BJ_DetWrEEPROM(u8* Ptr)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 i;
	ErrorStatus Result;
	u8 Retry = 3;
	
	Cmd[Count++] = BJ_WREEPROM ;
	for(i=0; i<7; i++)
	{
		Cmd[Count++] = *(Ptr+i) ;//������1��������2�����롢 ������š���������1����������2�����
	}
	
	BJ_DetCmdEncode(Cmd, Count);
	while(Retry--)
	{
		Result = BJ_DetSendAndRec(0, 1);
		OSTimeDlyHMSM(0,0,0,51);
		if(Result != ERROR)
		{
			break;
		}
	}
	return Result;
}
/**
  *************************************************************************************
  * @brief  BIT����
  * @param  Ser1 ������1
  * @param  Ser2 ������2
  * @retval ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetBit(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;  
	
	Cmd[Count++] = BJ_BIT ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
			
	return BJ_DetSendAndRec(0, 0);
}
/**
  *************************************************************************************
  * @brief  �ض�����ʱ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2 
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetTimeRd(u8 Ser1, u8 Ser2)
{  	
	u8 Count = 0;
	u8 Cmd[10] ;
	u8 Buffer[6];
	u8 i;
	u8 isExist = 0;
	
	Cmd[Count++] = BJ_TIMERD ;
	Cmd[Count++] = Ser1 ;
	Cmd[Count++] = Ser2 ;
	
	for(i=0; i<5;i++)
	{
		BJ_DetCmdEncode(Cmd, Count);
		if(ERROR != BJ_DetSendAndRec(BJ_RetryCount, 5))
		{
			if(isExist)
			{
				if(memcmp(Buffer, (char*)&DetProtocol.RecData.Data[1], 6) == 0)
				{
					return SUCCESS;
				}
			}
			isExist = 1;
			memcpy(Buffer, (char*)&DetProtocol.RecData.Data[1], 6);
		}		
	}
		
	return ERROR ;
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
ErrorStatus BJ_DetTimeWr(u8 Ser1, u8 Ser2, u8 Pwd, u32 DelayClk)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_TIMEWR;
	Cmd[Count++] = Ser1;
	Cmd[Count++] = Ser2;
	Cmd[Count++] = Pwd;
    Cmd[Count++] = Cmd[0];//�ֲ�˵�������������
	Cmd[Count++] = DelayClk&0xFF;
	Cmd[Count++] = (DelayClk>>8)&0xFF;
	Cmd[Count++] = (DelayClk>>16)&0x3F;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;   
}
/**
  *************************************************************************************
  * @brief  ���ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2 
  * @param  Pwd ����
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetCharge(u8 Ser1, u8 Ser2, u8 Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_CHARGE ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
    Cmd[Count++] = Pwd ;
    Cmd[Count++] = Cmd[1];//�ֲ�˵�������������
	
	BJ_DetCmdEncode(Cmd, Count);
	
	return BJ_DetSendAndRec(0, 0);
}
/**
  *************************************************************************************
  * @brief  ���ڼ��������
  * @param  Ser1 ������1
  * @param  Ser2 ������2 
  * @param  Pwd ���� 
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��12��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetTestDelayCount(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_TEST_CNT ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 5) ;  
}
/**
  *************************************************************************************
  * @brief  ׼����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2 
  * @param  Pwd ���� 
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetExpen(u8 Ser1, u8 Ser2, u8 Pwd)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_EXPEN ;
	Cmd[Count++] = Ser1 ;
    Cmd[Count++] = Ser2 ;
    Cmd[Count++] = Pwd ;
    Cmd[Count++] = Cmd[1];//�ֲ�˵�������������
	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(BJ_RetryCount, 1) ;  
}
/**
  *************************************************************************************
  * @brief  ׼����ָ��
  * @param  Ser1 ������1
  * @param  Ser2 ������2 
  * @param  Pwd ���� 
  * @retval ���
  *      @arg SUCCESS �ɹ�
  *      @arg ERROR ��������ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_DetEraseEEPROM(u8 Ser1, u8 Ser2)
{
	u8 Count = 0;
	u8 Cmd[10] ;
	                 
	Cmd[Count++] = BJ_ERASE_EEPROM ;
	Cmd[Count++] = 0x00 ;
    Cmd[Count++] = 0x00 ;

	
	BJ_DetCmdEncode(Cmd, Count);
		
	return BJ_DetSendAndRec(0, 1) ;  
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
void BJ_DetBlastCmdEncode(void)
{
	DetProtocol.Mode = DET_BLAST;
	DetProtocol.SendBuffer.Count = 0;
	DetProtocol.SendBuffer.Index = 0;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T3700US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T7000US; 
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T100US;
	
	DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Count++] = T40US;//��Ϊ����TIM_SetAutoreload��ʽ���ͣ���Ҫ�����һ�Σ�Ĭ�ϵȴ���������
}
/**
  *************************************************************************************
  * @brief  �������ź�
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��3��10��
  * @note   ��
  *************************************************************************************  
  */
void BJ_DetBlast(void)
{
	BJ_DetBlastCmdEncode();
    BJ_DetSendAndRec(0, 0) ; 
}

/**
  *************************************************************************************
  * @brief  �Զ������ź�
  * @param  Ptr �����͵�ָ��
  * @param  Bytes �����͵�ָ���ֽ���
  * @param  Test3Monitor �Ƿ���TEST3�ź�
  * @retval ��
  * @author ZH
  * @date   2015��8��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus BJ_CustomDetBlast(u16 *Ptr, u8 Count)
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
    return BJ_DetSendAndRec(0, 0) ; 
}

