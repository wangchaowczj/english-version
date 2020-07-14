#include "includes.h"

ErrorStatus XA2_ReadPwdRetry(u32 Serl, u32* Pwd)
{
    u8 Retry = 2;
    
    if(ERROR != XA2_ReadPwd(Serl, Pwd))
    {
        return SUCCESS;
    }
    while(Retry--)
    {
        XA2_Reset(Serl);
        OSTimeDlyHMSM(0,0,0,80);//��ʱ80ms
        XA2_DetCheckIn();
        OSTimeDlyHMSM(0,0,0,80);//��ʱ��оƬ��EEPROM     
        if(ERROR != XA2_ReadPwd(Serl, Pwd))
        {
            return SUCCESS;
        }
    }
    return ERROR;
}
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
void XA2_AllChannelDetResetAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ResetAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ResetAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA2_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ClearAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ClearAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA2_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value) == ERROR)//����������ض�ָ��	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
			}					
		}		
	}	
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
void XA2_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			if (XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value) == ERROR)//����������ض�ָ��	                     
			{
				Channel.XA2_Info[i].Status = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
			}
			else
			{
				Channel.XA2_Info[i].freq = GetDetFreqFromHead();
			}					
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��4��23��
  * @note   ��
  *************************************************************************************
  */
void XA2_AllChannelDetPwdRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			if (XA2_ReadPwdRetry(Channel.XA2_Info[i].Serl.U32Value, &Channel.XA2_Info[i].Pwd.U32Value) == ERROR)                     
			{
				Channel.XA2_Info[i].Status = DET_PASSWORD_READ_ERR;		
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
void XA2_AllChannelDetUnLockPwd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			switch(XA2_PwdUnlock(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value))
			{
                case 0:
                    break;
                case 1:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_ERR;//�������
                    break;
                default:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_NOT_ACK;//ͨѶ�쳣
                    break;                    
				
			}					
		}		
	}	
}

u8 XA2_GetDetType(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>46)&0x1F;//  bit46-bit50��5λ
}
/**
  *************************************************************************************
  * @brief  ����оƬ�����ж��Ƿ�Ϊ����������
  * @param[in]  ��
  * @retval 0���ǵ����ݣ�����ֵ��ʾ������
  * @author ZH
  * @date   2016��7��6��
  * @note   
  *************************************************************************************  
  */
DET_ERR_CODE XA2_DetTypeTest(u8 DetType)
{          	      
	if((DetType&0x1F) == (UserParam.DetType&0x1F))
	{
		return DET_PASS;
	}
	else 
	{
		return DET_TYPE_ERR; 
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
void XA2_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{		
			Result = XA2_DetTypeTest(XA2_GetDetType(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value)); //�ж�оƬ����
			if(DET_PASS != Result)
			{
				Channel.XA2_Info[i].Status = Result;
			}
		}		
	}	
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
u8 XA2_DetIsFixDelay(u8 Type)
{
	if((Type == DET_TYPE_XA_02) || (Type == DET_TYPE_XA_06) || (Type == DET_TYPE_XA_21))
	{
		return 0;
	}
	else
	{
		return 1;
	}		
}
DET_ERR_CODE XA2_DetBitTest(XA2_DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
    u16 Status;
	
	retry1=3;
	do
	{  
		XA2_ClearAll();
        OSTimeDly(50);
		XA2_Bit(det->Serl.U8Buffer[0]);
        XA2_Bit(det->Serl.U8Buffer[0]);
 		OSTimeDlyHMSM(0,0,1,500);       
		result = XA2_ReadStatus(det->Serl.U32Value, &Status);	 //��������ʱ��ض�ָ���ʱ�ض���������£�����1
		if(result == ERROR )
		{//״̬�ض����ִ���
			err_code = DET_BIT_STATUS_READ_ERR;	
		}
		else
		{
			if(XA2_GetDetStatus(Status, XA2_BIT_STATUS)== ERROR)
            {
                err_code = DET_BIT_ERR;	//Bit����
            }
            else
            {
                err_code = DET_PASS;//BIT����ͨ��
            }
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
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
void XA2_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	u16 Status;
    
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			XA2_ClearAll();	     //����ȫ�ָ�λָ��
            OSTimeDly(50);
			XA2_Bit(Channel.XA2_Info[i].Serl.U8Buffer[0]);     //����BIT1�����жϷ���
            XA2_Bit(Channel.XA2_Info[i].Serl.U8Buffer[0]);
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			OSTimeDlyHMSM(0,0,1,500);//��ʱ�ȴ�BIT�������,�Ӳ��ο�����Ҫ600ms����/6.7V, 5.5V,�𱬵���68ufҪ1.3��
			break;		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA2_ReadStatus(Channel.XA2_Info[i].Serl.U32Value, &Status);	 //��������ʱ��ض�ָ���ʱ�ض���������£�����1
			if(Result == ERROR )
			{//״̬�ض����ִ���
				Channel.XA2_Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(XA2_GetDetStatus(Status, XA2_BIT_STATUS)== ERROR)
                {
                    Channel.XA2_Info[i].Status = DET_BIT_ERR;	//Bit����
                }
			}		
		}
	}
}
DET_ERR_CODE XA2_DetChargeTest(XA2_DET_INFO* det)
{
	//u8 retry=3;
	ErrorStatus result;
    u16 Status;
    
    switch(XA2_PwdUnlock(det->Serl.U32Value, det->Pwd.U32Value))
    {
        case 0:
            break;
        case 1:
            return DET_PASSWORD_ERR;//�������
        default:
            return DET_PASSWORD_NOT_ACK;//ͨѶ�쳣                   
        
    }

	XA2_Charge(det->Serl.U8Buffer[0]);	   //����CHARGE
	XA2_Charge(det->Serl.U8Buffer[0]);	   //����CHARGE
	OSTimeDly(200);
	WaitRxPinReady(4000);
//	OSTimeDlyHMSM(0,0,0,90);
	
	result = XA2_ReadStatus(det->Serl.U32Value, &Status);
	if(result == ERROR )
	{//���ִ���
		return DET_CHARGE_STATUS_READ_ERR;		//BB
	}
	else
	{
		if(XA2_GetDetStatus(Status, XA2_CHARGE_STATUS) == ERROR)
		{	
			return DET_CHARGE_ERR;	//������		
		}
		else
		{
			return DET_PASS;//������ͨ��
		}
	}
}
void XA2_AllChannelDetChargeTest(void)
{
	u8 i;
	u16 Status;
    
	for(i = 0; i < Channel.Count;i++)
	{
		ConnectChannel(i);
		
		//д����ʱ����
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			if(XA2_WriteTime(Channel.XA2_Info[i].Serl.U32Value, 0x55) == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_WRITE_DELAY_ERR;      //д���ڴ���
			}		
		}
		//����
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			switch(XA2_PwdUnlock(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value))
            {
                case 0:
                    break;
                case 1:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_ERR;//�������
                    break;
                default:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_NOT_ACK;//ͨѶ�쳣
                    break;
                
            }		
		}
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			XA2_Charge(Channel.XA2_Info[i].Serl.U8Buffer[0]);	   //����CHARGE
			XA2_Charge(Channel.XA2_Info[i].Serl.U8Buffer[0]);	   //����CHARGE			
		}
	}
	//�ȴ����
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			OSTimeDly(200);
			break;
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			WaitRxPinReady(4000);
			
			if(XA2_ReadStatus(Channel.XA2_Info[i].Serl.U32Value, &Status) == ERROR )
			{//���ִ���
				Channel.XA2_Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else
			{
				if(XA2_GetDetStatus(Status, XA2_CHARGE_STATUS) == ERROR)
				{	
					Channel.XA2_Info[i].Status = DET_CHARGE_ERR;	//������		
				}
				else
				{
					;//������ͨ��
				}
			}		
		}
	}
}
u8 XA2_WriteFixDelayTest(u32 Serl, u16 FixDelay)
{
	u8 Retry = 3;
	
	while(Retry--)
	{
		if(XA2_WriteFixDelayUnlock(Serl) == ERROR)
		{
			return 1;//��EEPROM����
		}				
		else if(XA2_WriteFixDelay(Serl, FixDelay) != ERROR)
		{
			return 0;
		}	
	}
	return 2;//д�̶����ڴ���
}
void XA2_AllChannelWriteFixedDelay(void)
{
	u8 i=0;
	u16 FixDelay;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA2_ReadTimeClock(Channel.XA2_Info[i].Serl.U32Value, NULL, &FixDelay, NULL)  == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(FixDelay != (UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					switch(XA2_WriteFixDelayTest(Channel.XA2_Info[i].Serl.U32Value, UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
					{
						case 0:
							break;
						case 1:
							Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
							break;
						default:
							Channel.XA2_Info[i].Status = DET_WR_CODE_WR_FIXED_ERR;//д�̶����ڴ���
							break;
					}
				}			
			}			
		}
	}	
}
u8 XA2_UnlockAndWriteEEPROM(u32 Serl, u8 Addr, u8* Buffer)
{
    u8 Retry = 2;
    
    while(Retry--)
    {
        if(ERROR == XA2_WriteDataUnlock(Serl))
        {
            return 1;//��EEPROM����
        }
        if(ERROR == XA2_WriteEEPROM(Addr, Buffer))
        {
            if(Addr == 2)
            {
                XA2_ReadSerl(&Serl);
            }
            continue;
        }
        return 0;    
    }
    return 2;//дEEPROM����
}
void XA2_AllChannelDetCodeWrite(void)
{
	u8 i=0;
	u8 U8Value;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
                Channel.BarCode[i].Status = BARCODE_WR_END;//����дûд�ɹ�������Ϊ�������Ѿ�ʹ��
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 4, &Channel.BarCode[i].DetCode[0]);
                if(U8Value == 1)
                {//��EEPROM����
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
                    continue;
                }
                else if(U8Value != 0)
                {//дʧ��
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 5, &Channel.BarCode[i].DetCode[4]);
                if(U8Value == 1)
                {//��EEPROM����
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
                    continue;
                }
                else if(U8Value != 0)
                {//дʧ��
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 3, &Channel.BarCode[i].DetCode[4]);
                if(U8Value == 1)
                {//��EEPROM����
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
                    continue;
                }
                else if(U8Value != 0)
                {//дʧ��
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 2, &Channel.BarCode[i].DetCode[0]);
                if(U8Value == 1)
                {//��EEPROM����
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//��EEPROM����
                    continue;
                }
                else if(U8Value != 0)
                {//дʧ��
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                //ȷ��оƬ�Ƿ��Ѿ���ȷд������
                XA2_ResetAll();
                XA2_ResetAll();
                OSTimeDlyHMSM(0,0,0,50);
                XA2_ClearAll();
                XA2_ClearAll();
                OSTimeDly(50);
                
                if(ERROR == XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_READ_ERR;
                    continue;
                }
                if (0 != memcmp(&Channel.BarCode[i].DetCode[0], Channel.XA2_Info[i].Serl.U8Buffer, 4))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_VERIFY_ERR;//д����ʧ��
                    continue;
                }    
                if(ERROR == XA2_ReadPwdRetry(Channel.XA2_Info[i].Serl.U32Value, &Channel.XA2_Info[i].Pwd.U32Value))
                {
                    Channel.XA2_Info[i].Status = DET_READ_EEPROM_ERR;
                    continue;
                }
                if (0 != memcmp(&Channel.BarCode[i].DetCode[4], &Channel.XA2_Info[i].Pwd.U8Buffer[0], 4))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_VERIFY_ERR;//д����ʧ��
                    continue;
                }     			
			}
			else
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}


void XA2_AllChannelFixedDelayCompare(void)
{
	u8 i=0;
	u16 FixedDelay;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA2_ReadTimeClock(Channel.XA2_Info[i].Serl.U32Value, NULL, &FixedDelay, NULL)  == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(FixedDelay != (UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					Channel.XA2_Info[i].Status = DET_FIX_DELAY_NOT_EQUAL;
				}			
			}			
		}
	}
}
/**
  *************************************************************************************
  * @brief  �׹�д�����¼
  * @param[in]  ��
  * @retval ERROR ��û��д�� ��SUCCESS�Ѿ�д��
  * @author ZH
  * @date   2014��9��15��
  * @note   DATA1-DATA4��Ϊ������ı��ݣ�DATA5-DATA8��Ϊ����ı��ݡ�
  * Ҳ����˵��д��ʱ��ͬʱд2�飬��ʹ����������������������仯ʱ�����Դӱ��������лָ���
  * DATA9��BIT0-BIT4�����ڱ�ʾ�Ѿ���I�͡�II�͡��볧�졢���Ʒ�졢��Ʒ��
  *************************************************************************************  
  */
DET_ERR_CODE DetWriteTestRecorder(XA2_DET_INFO* det)
{
    u8 Buffer[4];
    u8 Crc8;
    u8 NeedWrite = 0;
    
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 == Buffer[3])
    {
        if((Buffer[1] != 0x00) || (Buffer[2] != 0x00))
        {
            return DET_WR_CODE_VERIFY_ERR;
        }
		if((Buffer[0]&0x01) == 0)
		{//û�о���I�ͼ��
			return DET_TEST_RECORDER_ERR1;
		}
		if((Buffer[0]&0x02) == 0)
		{//û�о���II�ͼ��
			return DET_TEST_RECORDER_ERR2;
		}
        switch(UserParam.Stage)
        {    
            case ENTER_FACTORY:
                if((Buffer[0]&0x04) == 0)
                {
                    Buffer[0] |= 0x04;
                    NeedWrite = 1;
                }
                break;
            case SEMIFINISHED:
                if((Buffer[0]&0x08) == 0)
                {
                    Buffer[0] |= 0x08;
                    NeedWrite = 1;
                }
//				else if((UserParam.NetMode == 0) && (UserParam.DetCodeWrite == 1))
//				{//���Ʒ�ظ����� ������ģʽ��д��ģʽ��������
//					return DET_TEST_RECORDER_REPEAT_ERR1;
//				}
                break;
            case FINISHED:
                if(((Buffer[0]&0x08) == 0) && (UserParam.NetMode == 0))
                {//û�о������Ʒ��⣨��Ϊ�Զ����������еĲ������Ʒ���ʷ�����ģʽ���жϣ�
                    return DET_TEST_RECORDER_ERR3;
                }
                if((Buffer[0]&0x10) == 0)
                {
                    Buffer[0] |= 0x10;
                    NeedWrite = 1;
                }
//				else  if((UserParam.NetMode == 0) && (UserParam.DetCodeWrite == 1))
//				{//��Ʒ�ظ����� ������ģʽ��д��ģʽ��������
//					return DET_TEST_RECORDER_REPEAT_ERR2;
//				}
                break;
            default:
                break;
        }
    }
    else
    {
		u8 Buffer1[4];
		
		if(ERROR == XA2_ReadEEPROM(0x06, Buffer1))
		{
			return DET_READ_EEPROM_ERR;
		}
		if(0 != memcmp(Buffer1, Buffer,4))
		{
			return DET_TEST_RECORDER_CRC_DIFFERENT_ERR;//�������ݲ�ͬ
		}
		return DET_TEST_RECORDER_CRC_ERR;//CRCУ�鲻��
			
    }

    if(NeedWrite)
    {       
        Buffer[3] = (~GetCrc8Value(Buffer, 3))&0xFF;
        
        switch(XA2_UnlockAndWriteEEPROM(det->Serl.U32Value, 0x06, &Buffer[0]))
        {
            case 0:
                break;
            case 1:
                return DET_WR_CODE_UNLOCK_EEPROM_ERR;
            default:
                return DET_WR_CODE_ERR;
        }
    }
    
    return DET_PASS;
}
/**
  *************************************************************************************
  * @brief  �׹ܳ�Ʒ����Ʒ�ظ�����¼
  * @param[in]  ChannelCountͨ����
  * @retval ERROR �ظ�д�� ��SUCCESS���ظ�
  * @author ZH
  * @date   2014��9��15��
  * @note   DATA1-DATA4��Ϊ������ı��ݣ�DATA5-DATA8��Ϊ����ı��ݡ�
  * Ҳ����˵��д��ʱ��ͬʱд2�飬��ʹ����������������������仯ʱ�����Դӱ��������лָ���
  * DATA9��BIT0-BIT4�����ڱ�ʾ�Ѿ���I�͡�II�͡��볧�졢���Ʒ�졢��Ʒ��
  *************************************************************************************  
  */
DET_ERR_CODE DetWriteTestRecorderCheck(u8 ChannelCount)
{
    u8 Buffer[4];
    u8 Crc8;
	
//    OSTimeDly(50);
	ConnectChannel(ChannelCount);
	
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 == Buffer[3])
	{
		switch(UserParam.Stage)
        {    
            case SEMIFINISHED:
                if((Buffer[0]&0x08) == 0x08)
                {//�Ѿ����й����Ʒ��
					return DET_TEST_RECORDER_REPEAT_ERR;
                }
				else
				{
					return DET_PASS;
				}
				
            case FINISHED:
                if((Buffer[0]&0x10) == 0x10)
                {//�Ѿ����й���Ʒ��
					return DET_TEST_RECORDER_REPEAT_ERR;
                }
				else
				{
					return DET_PASS;
				}
            default:
				return DET_PASS;
		}
	}
	else
	{
		return DET_TEST_RECORDER_CRC_ERR;//CRCУ�鲻��
	}		
}
void XA2_AllChannelDetWriteTestRecorder(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = DetWriteTestRecorder(&Channel.XA2_Info[i]);
            if(Result != DET_PASS)
            {
                Channel.XA2_Info[i].Status = Result;
            }
		}
	}
}
/**
  *************************************************************************************
  * @brief  �׹�����д��״̬
  * @param[in]  ��
  * @retval ERROR ��û��д�� ��SUCCESS�Ѿ�д��
  * @author ZH
  * @date   2014��9��15��
  * @note   DATA1-DATA4��Ϊ������ı��ݣ�DATA5-DATA8��Ϊ����ı��ݡ�
  * Ҳ����˵��д��ʱ��ͬʱд2�飬��ʹ����������������������仯ʱ�����Դӱ��������лָ���
  * DATA9��BIT0-BIT4�����ڱ�ʾ�Ѿ���I�͡�II�͡��볧�졢���Ʒ�졢��Ʒ��
  *************************************************************************************  
  */
DET_ERR_CODE XA2_DetCodeWriteStatusTest(XA2_DET_INFO* det)
{
    u8 Buffer[4];
    u8 Crc8;
    
    if(ERROR == DetCodeRulePass(det->Serl.U32Value, det->Pwd.U32Value))
    {
        return DET_CODE_RULE_ERR;
    }
    
    if(ERROR == XA2_ReadEEPROM(0x04, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    if (0 != memcmp(Buffer, det->Serl.U8Buffer, 4))
    {
        return DET_CODE_BACKUP_ERR1;
    }
    if(ERROR == XA2_ReadEEPROM(0x05, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    if (0 != memcmp(Buffer, det->Pwd.U8Buffer, 4))
    {
        return DET_CODE_BACKUP_ERR2;
    }
    
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 != Buffer[3])
    {
        return DET_TEST_RECORDER_DATA_ERR2;
    }
    return DET_PASS;
}
void XA2_AllChannelDetCodeWriteStatusTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA2_DetCodeWriteStatusTest(&Channel.XA2_Info[i]);
            if(Result != DET_PASS)
            {
                Channel.XA2_Info[i].Status = Result;
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
void XA2_AllChannelDetCodeVerify(void)
{
	u8 i=0;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)	
		{			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if ((memcmp(&Channel.XA2_Info[i].Serl.U8Buffer, &Channel.BarCode[i].DetCode[0], 4) != 0) || 
                    (memcmp(&Channel.XA2_Info[i].Pwd.U8Buffer, &Channel.BarCode[i].DetCode[4], 4) != 0))
				{
					Channel.XA2_Info[i].Status = DET_CODE_NOT_SAME;//����д���벻һ��
				}				
			}
			else
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}
void XA2_DetTest(void)
{	
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
    XA2_AllChannelDetResetAll();
    
	/*******�ŵ�******/
	XA2_AllChannelDetClrAll();
	
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
	XA2_AllChannelDetSerlRd();
	
    /***PWD�ض����****/
    XA2_AllChannelDetPwdRd();
    
	/***��������Ƿ���ȷ****/	
	XA2_AllChannelDetUnLockPwd();
	
	/*******�ж�оƬƵ���Ƿ�����*******/
	AllChannelDetFreqTest();

	/*********�ж�оƬ���͵Ĺ���************/
	XA2_AllChannelDetTypeTest();
	
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
    {       
		if(!(((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED)) && UserParam.DetCodeWrite))
		{
            u8 Buffer[8];
            
            Buffer[0] = Channel.XA2_Info[0].Serl.U8Buffer[0];
            Buffer[1] = Channel.XA2_Info[0].Serl.U8Buffer[1];
            Buffer[2] = Channel.XA2_Info[0].Serl.U8Buffer[2];
            Buffer[3] = Channel.XA2_Info[0].Serl.U8Buffer[3];
            Buffer[4] = Channel.XA2_Info[0].Pwd.U8Buffer[0];
            Buffer[5] = Channel.XA2_Info[0].Pwd.U8Buffer[1];
            Buffer[6] = Channel.XA2_Info[0].Pwd.U8Buffer[2];
            Buffer[7] = Channel.XA2_Info[0].Pwd.U8Buffer[3];
            if(DetInfoQueueItemIsExist(&DetInfoQueue, &Buffer[0]))
            {
                Channel.XA2_Info[0].Status = DET_CODE_REPEAT_TEST;//�ظ����	
            } 

		}
    }
	//BIT����
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//����bit��⹦�ܵ�ѡ��
	{ 	
		XA2_AllChannelDetBitTest();
	}

	/*����⣬���Ե���*/
    if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
    {    
        XA2_AllChannelDetChargeTest();

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
	
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DelayWrite)
			{
				XA2_AllChannelWriteFixedDelay();//д����
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DetCodeWrite)
			{
				XA2_AllChannelDetCodeWrite();	//д����
			}
		}
	}
   
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if(UserParam.CheckDelay)
			{	
				XA2_AllChannelFixedDelayCompare();				
			}
		}
	}
    if(AllChannelTestEnd() == 0)
    {
        /***д�����¼****/
        XA2_AllChannelDetWriteTestRecorder();    
    }
   
	if(AllChannelTestEnd() == 0)
	{
		/***���EEPROM�������Ƿ���Ϲ���****/
		XA2_AllChannelDetCodeWriteStatusTest();
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.CheckDetCode)
			{
				/***������֤****/
				XA2_AllChannelDetCodeVerify();
			}
		}	
	}

	XA2_AllChannelDetClrAll();
	SET_LIN_OPEN();

	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();
    
    if ((Channel.Count == 1) && (GetChannelOldStatusErrorCount() == 0))
	{
        u8 Buffer[8];
        
		Buffer[0] = Channel.XA2_Info[0].Serl.U8Buffer[0];
		Buffer[1] = Channel.XA2_Info[0].Serl.U8Buffer[1];
		Buffer[2] = Channel.XA2_Info[0].Serl.U8Buffer[2];
		Buffer[3] = Channel.XA2_Info[0].Serl.U8Buffer[3];
		Buffer[4] = Channel.XA2_Info[0].Pwd.U8Buffer[0];
		Buffer[5] = Channel.XA2_Info[0].Pwd.U8Buffer[1];
        Buffer[6] = Channel.XA2_Info[0].Pwd.U8Buffer[2];
        Buffer[7] = Channel.XA2_Info[0].Pwd.U8Buffer[3];        
		SaveDetInfoQueueItem(&DetInfoQueue, &Buffer[0]);//������Ϣ���������е��׹���û�����ǰ�������ټ��
		if(NeedScan())
		{//�������룬���㹤����10��һ�н���װ�У�ÿ���е��׹�Ҫ������ǰ12λ��ͬ
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 XA2_DetAnalyst(void)
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
    OSTimeDlyHMSM(0,0,0,200);
    XA2_ResetAll();
    XA2_ResetAll();
	OSTimeDlyHMSM(0,0,0,50);	
	XA2_ClearAll();
	XA2_ClearAll();
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
			
			Channel.XA2_Info[0].lv_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//��ֵС��2uA
				if(Channel.XA2_Info[0].lv_current <= test_param.det_lv_current_max)
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
			
			Channel.XA2_Info[0].lv_reverse_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//��ֵС��2uA
				if(Channel.XA2_Info[0].lv_reverse_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}	
		SET_LIN_REVERSE();
		
		OSTimeDlyHMSM(0,0,0,50);
		if(ERROR != XA2_ReadSerl(&Channel.XA2_Info[0].Serl.U32Value))
		{
            if(ERROR != XA2_ReadPwd(Channel.XA2_Info[0].Serl.U32Value, &Channel.XA2_Info[0].Pwd.U32Value))
            {
                AnalystResult |= 0x0001;//�������ȡ�ɹ�
                
                Channel.XA2_Info[0].freq = GetDetFreqFromHead();
                
                OSTimeDlyHMSM(0,0,0,10);
                if(ERROR != XA2_ReadTimeClock(Channel.XA2_Info[0].Serl.U32Value, NULL,NULL,NULL)) 
                {
                    AnalystResult |= 0x0002;//����ʱ��ض�����
                }
                OSTimeDlyHMSM(0,0,0,10);
                if(DET_PASS == XA2_DetBitTest(&Channel.XA2_Info[0]))
                {
                    AnalystResult |= 0x0004;//BIT��������
                }
                
                //д����ʱ����
                OSTimeDlyHMSM(0,0,0,10);
                if(ERROR != XA2_WriteTime(Channel.XA2_Info[0].Serl.U32Value, 0x100))
                {
                    AnalystResult |= 0x0008;//д��������
                }
            
                OSTimeDlyHMSM(0,0,0,10);
                Result = XA2_DetChargeTest(&Channel.XA2_Info[0]);
                if(DET_PASSWORD_ERR != Result)
                {
                    AnalystResult |= 0x0010;//������ȷ
                }
                if(DET_PASS == Result) 
                {			
                    AnalystResult |= 0x0020;//�������
                }
                
                XA2_Mask(Channel.XA2_Info[0].Serl.U32Value);//�����׹ܣ��������·������׹�
                
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
                    
                    Channel.XA2_Info[0].hv_current = (temp[0]+temp[1]+temp[2])/3;
                    
                    if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
                    {//��ֵС��2uA
                        if(Channel.XA2_Info[0].hv_current <= test_param.det_hv_current_max)
                        {
                            break;				
                        }
                    }
                }
                CLOSE_HV_SW();//�رո�ѹ�������
                OSTimeDlyHMSM(0,0,0,100);
                XA2_ClearAll();
                OSTimeDlyHMSM(0,0,0,100);
                XA2_ClearAll();
                OSTimeDlyHMSM(0,0,0,100);			
            }            
        }	
	}

	return AnalystResult;
}
void XA2_DetCodeVerify(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******�ŵ�******/
	XA2_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//���߶�·����
	AllChannelOpenCircuitTest(0);//���߶�·����
	
	/***������ض����****/
	XA2_AllChannelDetSerlRd();

    /***PWD�ض����****/
    XA2_AllChannelDetPwdRd();
    
    XA2_AllChannelDetCodeWriteStatusTest();
    
	/***������֤****/
	XA2_AllChannelDetCodeVerify();
	
	XA2_AllChannelDetClrAll();
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
void XA2_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******�ŵ�******/
	XA2_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//���߶�·����
	AllChannelOpenCircuitTest(1);//���߶�·����
			
	/*******���˿ڵ�ѹ�Ƿ�����******/
//	AllChannelVoltageTest(0);//�Ȳ��Ե������ٲ��Ե�ѹ����Ȼ��ѹ���Բ�׼����ΪAD������ĵ��ݳ�����
	
	/***������ض����****/
	XA2_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void XA2_WaitAllChannelDetOff(void)
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
			if (XA2_ReadSerl(NULL) != ERROR)//����������ض�ָ��	                     
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

