#include "includes.h"


u8 WaitVerifyCode[6];

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
u8 XA_GetDetTypeFromBat(u8 Bat)
{
	return (Bat>>5);//��3bit��ʾ�׹�����
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
u8 XA_DetIsSingleCap(u8 Type)
{
	if(Type == DET_TYPE_XA_BD1)
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  ����оƬ�����ж��Ƿ�Ϊ�̶���������
  * @param[in]  Type �׹�����
  * @retval 0���ǹ̶����ڣ�����ֵ��ʾ�̶�����
  * @author ZH
  * @date   2016��7��6��
  * @note   
  *************************************************************************************  
  */
u8 XA_DetIsFixDelay(u8 Type)
{
	if((Type == DET_TYPE_XA_BD1) || (Type == DET_TYPE_XA_BD2))
	{
		return 1;
	}
	return 0;	
}



u16 XA_GetDetPwd(u8 Serl_1, u8 Serl_2, u8 Serl_3, u8 Factor, u8 Bat, u8 Year)
{
	const u8 Bytes = 6;
	u8 Buffer[6];
	u8 DetPwd[2];
	u8 Sum = 0;
	u8 i;
	
	if((Serl_1 == 0) && (Serl_2 == 0) && (Serl_3 == 0) && (Factor == 0) && (Bat == 0) && (Year == 0))
	{//��û��д������
		return 0x0000;
	}	
	Buffer[0] = Serl_1;
	Buffer[1] = Serl_2;
	Buffer[2] = Serl_3;
	Buffer[3] = Factor;
	Buffer[4] = Bat;
	Buffer[5] = Year;
	GetCrc16Value(0xFFFF, &DetPwd[0], &Buffer[0], 6);

	for(i=0; i<Bytes; i++)
	{
		Sum += Buffer[i];
	}
	DetPwd[0] ^= Sum;
	DetPwd[1] ^= Sum;	
	return ((((u16)DetPwd[0])<<8) + DetPwd[1]);
}
/**
  *************************************************************************************
  * @brief  �������
  * @param  TimeOut ��ʱʱ��
  * @retval SUCCESS �ɹ��� ERRORʧ��
  * @author ZH
  * @date   2018��3��26��
  * @note   ��
  *************************************************************************************
  */
ErrorStatus XA_DetPwdTest(DET_INFO *det)
{
	u8 Pwd1, Pwd2;
    ErrorStatus result;
    
    Pwd1 = det->Pwd1;
    Pwd2 = det->Pwd2;    
	result = XA_DetUnLockPwd(det->ser1,det->ser2,det->ser3, Pwd1, Pwd2);
	if(result == ERROR )
	{
        Pwd1 = DET_PWD_FIX_VALUE&0xFF;
        Pwd2 = (DET_PWD_FIX_VALUE>>8)&0xFF;
        if((det->Pwd1 != Pwd1) || (det->Pwd2 != Pwd2))
        {
            result = XA_DetUnLockPwd(det->ser1,det->ser2,det->ser3, Pwd1, Pwd2);
        }		
	}
    if(result == ERROR)
    {
        return ERROR;//�������
    }

    det->Pwd1 = Pwd1;
    det->Pwd2 = Pwd2; 
    
    return SUCCESS;
}
void XA_SaveDetInfo(u8 Index)
{
	u16 DetPwd;
	
	OSSchedLock();
	
	Channel.Info[Index].ser1 = DetProtocol.RecData.Data[1];   //�����׹������Ϣ
    Channel.Info[Index].ser2 = DetProtocol.RecData.Data[2];
    Channel.Info[Index].ser3  = DetProtocol.RecData.Data[3];
    Channel.Info[Index].fac = DetProtocol.RecData.Data[4];
    Channel.Info[Index].bat  = DetProtocol.RecData.Data[5];
    Channel.Info[Index].year = DetProtocol.RecData.Data[6];
    Channel.Info[Index].freq = GetDetFreqFromHead();
	Channel.Info[Index].type = Channel.Info[Index].bat>>5;
	
	DetPwd = XA_GetDetPwd(Channel.Info[Index].ser1, Channel.Info[Index].ser2, Channel.Info[Index].ser3, Channel.Info[Index].fac, Channel.Info[Index].bat, Channel.Info[Index].year);
	
	Channel.Info[Index].Pwd1 = DetPwd&0xFF;
	Channel.Info[Index].Pwd2 = DetPwd>>8;
	OSSchedUnlock();
}




DET_ERR_CODE XA_DetTypeTest(u8 DetType)
{        
	if((DetType&0x07) == (UserParam.DetType&0x07))
	{
		return DET_PASS;
	}
	else 
	{
		return DET_TYPE_ERR; 
	} 					
}

DET_ERR_CODE XA_DetBitTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
	
	retry1=3;
	do
	{  
		XA_DetClrAll();	     //����ȫ�ָ�λָ��
		if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(det->bat)) == 0)
		{//˫����
			XA_DetBit(det->ser1);     //����BIT1�����жϷ���
			XA_DetBit(det->ser1);     //����BIT1�����жϷ���
			OSTimeDlyHMSM(0,0,1,500);//
		}		
 		       
		result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);	 //��������ʱ��ض�ָ���ʱ�ض���������£�����1
		if(result == ERROR )
		{//״̬�ض����ִ���
			err_code = DET_BIT_STATUS_READ_ERR;	
		}
		else
		{
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(det->bat)))
			{//������
				if(XA_GetDetStatus(BIT_OUT_STATUS)== ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit����
				}
				else
				{
					err_code = DET_PASS;//BIT����ͨ��
				}						
			}
			else
			{//˫����
				if(XA_GetDetStatus(BIT_STATUS)== ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit����
				}
				else
				{
					err_code = DET_PASS;//BIT����ͨ��
				}			
			}
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
}

DET_ERR_CODE XA_DetChargeTest(DET_INFO* det)
{
	//u8 retry=3;
	ErrorStatus result;
	
	result = XA_DetPwdTest(det);
	if(result == ERROR )
	{
		return DET_PASSWORD_ERR;//�������
	}
	XA_DetCharge(det->ser1);	   //����CHARGE
	XA_DetCharge(det->ser1);	   //����CHARGE
	OSTimeDly(200);
	WaitRxPinReady(4000);
//	OSTimeDlyHMSM(0,0,0,90);
	
	result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);
	if(result == ERROR )
	{//���ִ���
		return DET_CHARGE_STATUS_READ_ERR;		//BB
	}
	else
	{
		if(XA_GetDetStatus(CHARGE_STATUS) == ERROR)
		{	
			return DET_CHARGE_ERR;	//������		
		}
		else
		{
			return DET_PASS;//������ͨ��
		}
	}
}

DET_ERR_CODE XA_DetExpenTest(DET_INFO* det)
{
	ErrorStatus result;
	
	XA_DetExpen();                  //׼��ָ��
	XA_DetExpen();                  //׼��ָ��

	OSTimeDlyHMSM(0,0,0,20);	
	result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);
	if(result == ERROR )
	{//����ʱ��ض����ִ���
		return DET_EXPEN_TIMERD_ERR;		//BB
	}
	else
	{
		if(XA_GetDetStatus(EXPEN_STATUS) == ERROR)
		{
			return DET_EXPEN_ERR;	//׼���𱬴���			
		}
		else
		{
			return DET_PASS;//׼���𱬲���ͨ��
		}
	}
}

DET_ERR_CODE XA_DetWriteCode(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
	
	memcpy((char*)&WaitVerifyCode[0], (char*)&Channel.BarCode[Index].DetCode[0], 6);
	//WaitVerifyCode[3] = (Channel.BarCode[Index].DetCode[3]&0xF0) + (Channel.Info[Index].fac&0x0F);//������ ,��4λ��ʾ��Բ���ţ����޸�
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	Buffer[i++] = WaitVerifyCode[0];//������1
	Buffer[i++] = WaitVerifyCode[1];//������2
	Buffer[i++] = WaitVerifyCode[2];//������3
	Buffer[i++] = DetPwd&0xFF;//����1
	Buffer[i++] = (DetPwd>>8)&0xFF;//����2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//����
	Buffer[i++] = WaitVerifyCode[5];//���
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_CODE_ERR;//дEEPROM����
	}
	return DET_PASS;
}
DET_ERR_CODE XA_ChangeDetType(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
		
	WaitVerifyCode[i++] = Channel.Info[Index].ser1;//������1
	WaitVerifyCode[i++] = Channel.Info[Index].ser2;//������2
	WaitVerifyCode[i++] = Channel.Info[Index].ser3;//������3
	WaitVerifyCode[i++] = Channel.Info[Index].fac;
	WaitVerifyCode[i++] = (Channel.Info[Index].bat&0x1F)+ (UserParam.DetType<<5);//����,��3λ��ʾ�׹�����
	WaitVerifyCode[i++] = Channel.Info[Index].year;//���
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	i = 0;
	Buffer[i++] = WaitVerifyCode[0];//������1
	Buffer[i++] = WaitVerifyCode[1];//������2
	Buffer[i++] = WaitVerifyCode[2];//������3
	Buffer[i++] = DetPwd&0xFF;//����1
	Buffer[i++] = (DetPwd>>8)&0xFF;//����2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//����,��3λ��ʾ�׹�����
	Buffer[i++] = WaitVerifyCode[5];//���
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_TYPE_ERR;//дEEPROM����
	}
	return DET_PASS;
}
DET_ERR_CODE XA_ChangeDetPwd(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
		
	WaitVerifyCode[i++] = Channel.Info[Index].ser1;//������1
	WaitVerifyCode[i++] = Channel.Info[Index].ser2;//������2
	WaitVerifyCode[i++] = Channel.Info[Index].ser3;//������3
	WaitVerifyCode[i++] = Channel.Info[Index].fac;
	WaitVerifyCode[i++] = Channel.Info[Index].bat;
	WaitVerifyCode[i++] = Channel.Info[Index].year;//���
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	i = 0;
	Buffer[i++] = WaitVerifyCode[0];//������1
	Buffer[i++] = WaitVerifyCode[1];//������2
	Buffer[i++] = WaitVerifyCode[2];//������3
	Buffer[i++] = DetPwd&0xFF;//����1
	Buffer[i++] = (DetPwd>>8)&0xFF;//����2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//����,��3λ��ʾ�׹�����
	Buffer[i++] = WaitVerifyCode[5];//���
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_TYPE_ERR;//дEEPROM����
	}
	return DET_PASS;
}
ErrorStatus XA_VerifyCodeWriteResult(u8 Index)
{
	if ((Channel.Info[Index].ser1 == WaitVerifyCode[0]) && (Channel.Info[Index].ser2 == WaitVerifyCode[1]) && 
		(Channel.Info[Index].ser3== WaitVerifyCode[2]) && (Channel.Info[Index].fac == WaitVerifyCode[3]) && 
		(Channel.Info[Index].bat == WaitVerifyCode[4]) && (Channel.Info[Index].year == WaitVerifyCode[5]))
	{
		return SUCCESS;
	}
	return ERROR;
}


/**
  *************************************************************************************
  * @brief  ��������
  * @param  ReverseFlag  0ʱ������1�ⷴ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void XA_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if (XA_DetSerlRd() == ERROR)//����������ض�ָ��	                     
			{
				Channel.Info[i].Status = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
			}
			else
			{
				XA_SaveDetInfo(i);
			}					
		}		
	}	
}
void XA_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (XA_DetSerlRd() == ERROR)//����������ض�ָ��	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
			}					
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  ���׹ܽ���
  * @param  ErrCode  ����ʱ�Ĵ���
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void XA_AllChannelDetUnLockPwd(DET_ERR_CODE ErrCode)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = ErrCode;//�������
			}					
		}		
	}	
}


/**
  *************************************************************************************
  * @brief  �׹����Ͳ���
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void XA_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{		
			Result = XA_DetTypeTest(Channel.Info[i].type); //�ж�оƬ����
			if(DET_PASS != Result)
			{//PD1��BD2�����໥ת��
				if(((Channel.Info[i].type == DET_TYPE_XA_PD1) && (UserParam.DetType == DET_TYPE_XA_BD2)) ||
					((Channel.Info[i].type == DET_TYPE_XA_BD2) && (UserParam.DetType == DET_TYPE_XA_PD1)))
				{
					Result = DET_PASS;//��߻��޸��׹�����
				}
			}
			if(Result != DET_PASS)
			{
				Channel.Info[i].Status = Result;
			}
		}		
	}	

}


/**
  *************************************************************************************
  * @brief  bit����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void XA_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			XA_DetClrAll();	     //����ȫ�ָ�λָ��
            OSTimeDly(50);
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)) == 0)
			{//˫����
				XA_DetBit(Channel.Info[i].ser1);     //����BIT1�����жϷ���
				XA_DetBit(Channel.Info[i].ser1);     //����BIT1�����жϷ���
			}		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)) == 0)
			{//˫����
				OSTimeDlyHMSM(0,0,1,500);//��ʱ�ȴ�BIT�������,�Ӳ��ο�����Ҫ600ms����/6.7V, 5.5V,�𱬵���68ufҪ1.3��
				break;
			}		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA_DetStatusRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3);	 //��������ʱ��ض�ָ���ʱ�ض���������£�����1
			if(Result == ERROR )
			{//״̬�ض����ִ���
				Channel.Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)))
				{//������
					if(XA_GetDetStatus(BIT_OUT_STATUS)== ERROR)
					{
						Channel.Info[i].Status = DET_BIT_ERR;	//Bit����
					}					
				}
				else
				{//˫����
					if(XA_GetDetStatus(BIT_STATUS)== ERROR)
					{
						Channel.Info[i].Status = DET_BIT_ERR;	//Bit����
					}		
				}
			}		
		}
	}
}

/**
  *************************************************************************************
  * @brief  �׹�������֤
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void XA_AllChannelDetCodeVerify(void)
{
	u8 i=0;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)	
		{			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if(memcmp(&Channel.Info[i].ser1, &Channel.BarCode[i].DetCode[0], 6) != 0)
				{
					Channel.Info[i].Status = DET_CODE_NOT_SAME;//����д���벻һ��
				}				
			}
			else
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}

void XA_AllChannelDetChargeTest(void)
{
	u8 i;
	
	for(i = 0; i < Channel.Count;i++)
	{
		ConnectChannel(i);
		
		//д����ʱ����
		if(DET_TESTING == Channel.Info[i].Status)
		{
			if(XA_DetTimeWr(Channel.Info[i].ser1,Channel.Info[i].ser2,Channel.Info[i].ser3, 0x55) == ERROR)
			{
				Channel.Info[i].Status = DET_WRITE_DELAY_ERR;      //д���ڴ���
			}		
		}
		//����
		if(DET_TESTING == Channel.Info[i].Status)
		{
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_PASSWORD_ERR;//�������
			}		
		}
		if(DET_TESTING == Channel.Info[i].Status)
		{
			XA_DetCharge(Channel.Info[i].ser1);	   //����CHARGE
			XA_DetCharge(Channel.Info[i].ser1);	   //����CHARGE			
		}
	
	}
	//�ȴ����
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			OSTimeDly(200);
			break;
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			WaitRxPinReady(4000);
			
			if(XA_DetStatusRd(Channel.Info[i].ser1,Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR )
			{//���ִ���
				Channel.Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else
			{
				if(XA_GetDetStatus(CHARGE_STATUS) == ERROR)
				{	
					Channel.Info[i].Status = DET_CHARGE_ERR;	//������		
				}
				else
				{
					;//������ͨ��
				}
			}		
		}
	}
}


void XA_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA_DetClrAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA_DetClrAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA_AllChannelWriteFixedDelay(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetTimeRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3)  == ERROR)
			{
				Channel.Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(XA_GetDetFixedDelay() != (UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
					{
						Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
					}				
					else
					{
						if(XA_DetFixed(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3, UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE) == ERROR)
						{
							Channel.Info[i].Status = DET_WR_CODE_WR_FIXED_ERR;//д�̶����ڴ���	
						}
					}
					//оƬ��������⣬д��̶����ں���ø�λ�׹�
					XA_DetClrAll();	     //����ȫ�ָ�λָ��
				}			
			}			
		}
	}	
}
void XA_AllChannelDetCodeWrite(void)
{
	DET_ERR_CODE Status;
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
				{
					Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//�������
				}			

				if(DET_TESTING == Channel.Info[i].Status)
				{
					if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
					{
						Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
					}
					
				}				
				if(DET_TESTING == Channel.Info[i].Status)
				{
					Status = XA_DetWriteCode(i);
					if(DET_PASS == Status)
					{					
						if (XA_DetSerlRd() == ERROR)//����������ض�ָ��	                     
						{
							Channel.Info[i].Status = DET_WR_CODE_READ_ERR;//������ͨ�Ŵ���		
						}
						else
						{
							XA_SaveDetInfo(i);
							if(XA_VerifyCodeWriteResult(i) == ERROR)
							{
								Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//д�����ݴ���
							}
						}				
					}
					else
					{
						Channel.Info[i].Status = Status;
					}
					Channel.BarCode[i].Status = BARCODE_WR_END;//����дûд�ɹ�������Ϊ�������Ѿ�ʹ��
				}			
			}
			else
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}

}
void XA_AllChannelChangeDetType(void)
{
	u8 i=0;
	DET_ERR_CODE Status;
	
	for(i=0; i<Channel.Count; i++)
	{
		if ((DET_TESTING == Channel.Info[i].Status) && (DET_PASS != XA_DetTypeTest(Channel.Info[i].type)))
		{
			ConnectChannel(i);
			
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//�������
			}				

			if(DET_TESTING == Channel.Info[i].Status)
			{
				if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
				{
					Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
				}				
			}				
			if(DET_TESTING == Channel.Info[i].Status)
			{
				Status = XA_ChangeDetType(i);				
				if(DET_PASS ==Status)
				{					
					if (XA_DetSerlRd() == ERROR)//����������ض�ָ��	                     
					{
						Channel.Info[i].Status = DET_WR_TYPE_READ_ERR;//������ͨ�Ŵ���		
					}
					else
					{
						XA_SaveDetInfo(i);
						if(XA_VerifyCodeWriteResult(i) == ERROR)
						{
							Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//д�����ݴ���
						}								
					}				
				}
				else
				{
					Channel.Info[i].Status = Status;
				}
			}		
		}
	}
}
void XA_AllChannelChangeDetPwd(void)
{
	u8 i=0;
	DET_ERR_CODE Status;
	
	for(i=0; i<Channel.Count; i++)
	{
		if (DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//�������
			}				
            if(DET_TESTING == Channel.Info[i].Status)
            {
                if(((Channel.Info[i].Pwd2<<8)+Channel.Info[i].Pwd1) != DET_PWD_FIX_VALUE)
                {
                    if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
                    {
                        Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
                    }				
                    if(DET_TESTING == Channel.Info[i].Status)
                    {
                        Status = XA_ChangeDetPwd(i);				
                        if(DET_PASS ==Status)
                        {					
                            if (XA_DetSerlRd() == ERROR)//����������ض�ָ��	                     
                            {
                                Channel.Info[i].Status = DET_WR_TYPE_READ_ERR;//������ͨ�Ŵ���		
                            }
                            else
                            {
                                XA_SaveDetInfo(i);
                                if(XA_VerifyCodeWriteResult(i) == ERROR)
                                {
                                    Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//д�����ݴ���
                                }								
                            }				
                        }
                        else
                        {
                            Channel.Info[i].Status = Status;
                        }
                    }                
                }
            }
		
		}
	}
}
void XA_AllChannelFixedDelayCompare(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetTimeRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3)  == ERROR)
			{
				Channel.Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(XA_GetDetFixedDelay() != (UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					Channel.Info[i].Status = DET_FIX_DELAY_NOT_EQUAL;
				}			
			}			
		}
	}
}
//==================================================================================================
//| �������� | DetTest 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹ܲ���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | �׹ܲ��Դ���
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void XA_DetTest(void)
{	
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******�ŵ�******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//���߶�·����
	AllChannelOpenCircuitTest(0);//���߶�·����
			
	/********������Ե���*******/
	if(TestParamIsSet(DET_LV_CURRENT_TEST_MASK_BIT))//����������Թ���ѡ��
	{
		AllChannelCurrentTest(0);
	}	

	/*******������ٴβ��Ե���*******/
	if(TestParamIsSet(DET_LV_CURRENT_TEST_MASK_BIT))//����������Թ���ѡ��
	{
		SET_LIN_REVERSE();
		OSTimeDlyHMSM(0,0,0,50);
		AllChannelCurrentTest(1);
		SET_LIN_REVERSE();
	}

	/*******���˿ڵ�ѹ�Ƿ�����******/
	AllChannelVoltageTest(0);//�Ȳ��Ե������ٲ��Ե�ѹ����Ȼ��ѹ���Բ�׼����ΪAD������ĵ��ݳ�����
	
	/***������ض����****/
	XA_AllChannelDetSerlRd();
	
	/***��������Ƿ���ȷ****/	
	XA_AllChannelDetUnLockPwd(DET_WR_CODE_UNLOCK_PWD_ERR);//�������
	
	/*******�ж�оƬƵ���Ƿ�����*******/
	AllChannelDetFreqTest();

	/*********�ж�оƬ���͵Ĺ���************/
	XA_AllChannelDetTypeTest();
	
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
    {       
		if(!(((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED)) && UserParam.DetCodeWrite))
		{
            u8 Buffer[8];
            
            Buffer[0] = Channel.Info[0].ser1;
            Buffer[1] = Channel.Info[0].ser2;
            Buffer[2] = Channel.Info[0].ser3;
            Buffer[3] = Channel.Info[0].fac;
            Buffer[4] = Channel.Info[0].bat;
            Buffer[5] = Channel.Info[0].year;
            if(DetInfoQueueItemIsExist(&DetInfoQueue, &Buffer[0]))
            {
                Channel.Info[0].Status = DET_CODE_REPEAT_TEST;//�ظ����	
            }        
		}
    }
	//BIT����
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//����bit��⹦�ܵ�ѡ��
	{ 	
		XA_AllChannelDetBitTest();
	}

	/*����⣬���Ե���*/
	if(XA_DetIsSingleCap(UserParam.DetType) == 0)
	{//������ʱ����	
		if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
		{    
			XA_AllChannelDetChargeTest();

			if(TestParamIsSet(DET_CHARGE_HV_CURRENT_TEST_MASK_BIT)) //����ĸ�ѹ�����������
			{
				if(AllChannelTestEnd() == 0)
				{
					OPEN_HV_SW();//�����ѹ
					OSTimeDly(50);
					AllChannelDetHVCurrentTest(0);//����ĸ�ѹ�����������	
				}
											
				/*******���˿ڵ�ѹ�Ƿ�����******/
				if(AllChannelTestEnd() == 0)
				{
					AllChannelVoltageTest(1);
				}
					

				if(AllChannelTestEnd() == 0)
				{
					OPEN_HV_SW();//�����ѹ
					SET_LIN_REVERSE();//����ĸ�ѹ�����������
					OSTimeDly(10);	
					AllChannelDetHVCurrentTest(1);
					
					if(AllChannelTestEnd() == 0)
					{
						AllChannelVoltageTest(1);
					}
					
					SET_LIN_REVERSE();	
				}
				CLOSE_HV_SW();//�رո�ѹ�������
				OSTimeDly(50);				
			}						
		} 
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DelayWrite)
			{
				XA_AllChannelWriteFixedDelay();//д����
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DetCodeWrite)
			{
				XA_AllChannelDetCodeWrite();	//д����
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		XA_AllChannelChangeDetType();//PD1��BD2��Ҫת��	
	}
	if(AllChannelTestEnd() == 0)
	{
		XA_AllChannelChangeDetPwd();//���оƬ���벻�ǹ̶����룬��ǿ�Ƹĳɹ̶�����
	}    
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if(UserParam.CheckDelay)
			{	
				if(XA_DetIsFixDelay(UserParam.DetType))
				{//�̶�����оƬ,�ж��׹�����ֵ�Ƿ�������ֵһ��
					XA_AllChannelFixedDelayCompare();
				}				
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		/***����ȫ0���****/
		XA_AllChannelDetCodeAllZeroVerify();
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.CheckDetCode)
			{
				/***������֤****/
				XA_AllChannelDetCodeVerify();
			}
		}	
	}

	XA_AllChannelDetClrAll();
	SET_LIN_OPEN();

	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();
    
    if ((Channel.Count == 1) && (GetChannelOldStatusErrorCount() == 0))
	{
        u8 Buffer[8];
        
		Buffer[0] = Channel.Info[0].ser1;
		Buffer[1] = Channel.Info[0].ser2;
		Buffer[2] = Channel.Info[0].ser3;
		Buffer[3] = Channel.Info[0].fac;
		Buffer[4] = Channel.Info[0].bat;
		Buffer[5] = Channel.Info[0].year;	
		SaveDetInfoQueueItem(&DetInfoQueue, &Buffer[0]);//������Ϣ���������е��׹���û�����ǰ�������ټ��
		if(NeedScan())
		{//�������룬���㹤����10��һ�н���װ�У�ÿ���е��׹�Ҫ������ǰ12λ��ͬ
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 XA_DetAnalyst(void)
{
	u16 AnalystResult = 0;
	u16 temp[3];
	u32 c_time;
	u8 Result;
	
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	
	OPEN_PW_HV();
	ClearDetInfo();
	
	ConnectChannel(0);
	
	/*******�ŵ�******/
	OSTimeDlyHMSM(0,0,0,500);	
	XA_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	XA_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	
	if(GetLinABVoltage(0, NULL) < 1000)
	{
		AnalystResult |= 0x8000;//�׹ܶ�·
	}
	else if(GetCurrentValue(10, NULL) < 500)
	{
		AnalystResult |= 0x4000;//�׹ܶ�·
	}
	else
	{
		/*******�������******/
		c_time = OSTimeGet();
		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
		{
			OSTimeDlyHMSM(0,0,0,100);
			temp[0] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			OSTimeDlyHMSM(0,0,0,100);
			temp[1] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			OSTimeDlyHMSM(0,0,0,100);
			temp[2] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			
			Channel.Info[0].lv_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//��ֵС��2uA
				if(Channel.Info[0].lv_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}
		
		/*******�������******/
		SET_LIN_REVERSE();
		c_time = OSTimeGet();
		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
		{
			OSTimeDlyHMSM(0,0,0,100);
			temp[0] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			OSTimeDlyHMSM(0,0,0,100);
			temp[1] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			OSTimeDlyHMSM(0,0,0,100);
			temp[2] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
			
			Channel.Info[0].lv_reverse_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//��ֵС��2uA
				if(Channel.Info[0].lv_reverse_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}	
		SET_LIN_REVERSE();
		
		OSTimeDlyHMSM(0,0,0,50);
		if(ERROR != XA_DetSerlRd())
		{
			XA_SaveDetInfo(0);
			AnalystResult |= 0x0001;//�������ȡ�ɹ�
			
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != XA_DetTimeRd(Channel.Info[0].ser1, Channel.Info[0].ser2, Channel.Info[0].ser3)) 
			{
				AnalystResult |= 0x0002;//����ʱ��ض�����
			}
			OSTimeDlyHMSM(0,0,0,10);
			if(DET_PASS == XA_DetBitTest(&Channel.Info[0]))
			{
				AnalystResult |= 0x0004;//BIT��������
			}
			
			//д����ʱ����
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != XA_DetTimeWr(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].ser3, 0x100))
			{
				AnalystResult |= 0x0008;//д��������
			}
		
			OSTimeDlyHMSM(0,0,0,10);
			Result = XA_DetChargeTest(&Channel.Info[0]);
			if(DET_PASSWORD_ERR != Result)
			{
				AnalystResult |= 0x0010;//������ȷ
			}
			if(DET_PASS == Result) 
			{			
				AnalystResult |= 0x0020;//�������
			}
			
			XA_DetMask(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].ser3);//�����׹ܣ��������·������׹�
			
			OPEN_HV_SW();//�����ѹ
			OSTimeDlyHMSM(0,0,3,0);
			
			c_time = OSTimeGet();
			
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{
				OSTimeDlyHMSM(0,0,0,100);
				temp[0] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
				OSTimeDlyHMSM(0,0,0,100);
				temp[1] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
				OSTimeDlyHMSM(0,0,0,100);
				temp[2] = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
				
				Channel.Info[0].hv_current = (temp[0]+temp[1]+temp[2])/3;
				
				if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
				{//��ֵС��2uA
					if(Channel.Info[0].hv_current <= test_param.det_hv_current_max)
					{
						break;				
					}
				}
			}
			CLOSE_HV_SW();//�رո�ѹ�������
			OSTimeDlyHMSM(0,0,0,100);
			XA_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);
			XA_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);			
		}	
	}

	return AnalystResult;
}
void XA_DetCodeVerify(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******�ŵ�******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//���߶�·����
	AllChannelOpenCircuitTest(0);//���߶�·����
	
	/***������ض����****/
	XA_AllChannelDetSerlRd();

    XA_AllChannelChangeDetPwd();//���оƬ���벻�ǹ̶����룬��ǿ�Ƹĳɹ̶�����
    
	/***������֤****/
	XA_AllChannelDetCodeVerify();
	
	XA_AllChannelDetClrAll();
	SET_LIN_OPEN();
       
	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();	
}
/**
  *************************************************************************************
  * @brief  �жϸ���ͨ���Ƿ��Ѿ������׹�
  * @param[in]  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��5��
  * @note   
  *************************************************************************************  
  */
void XA_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******�ŵ�******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//���߶�·����
	AllChannelOpenCircuitTest(1);//���߶�·����
			
	/*******���˿ڵ�ѹ�Ƿ�����******/
//	AllChannelVoltageTest(0);//�Ȳ��Ե������ٲ��Ե�ѹ����Ȼ��ѹ���Բ�׼����ΪAD������ĵ��ݳ�����
	
	/***������ض����****/
	XA_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void XA_WaitAllChannelDetOff(void)
{
	u8 i=0;
	
	while(1)
	{
		for(i=0; i<Channel.Count; i++)
		{
			ConnectChannel(i);
			OSTimeDly(5);
			
			if(GetCurrentValue(10, NULL) >= 500)
			{//�е���˵���׹�û������
				break;
			}
			if (XA_DetSerlRd() != ERROR)//����������ض�ָ��	                     
			{//���źŷ��أ�˵��û������
				break;
			}		
		}
		if(i == Channel.Count)
		{//���Ѿ�����
			break;
		}
	}	
}





