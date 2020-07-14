#include "includes.h"



/**
  *************************************************************************************
  * @brief  �ж�BIT״̬λ
  * @param  ��
  * @retval ���
  *      @arg SUCCESS BIT���Գɹ���
  *      @arg ERROR BIT����ʧ��
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus BJ_GetBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_TEST_ERR) != 0)  // �ж�BIT���Ե�״̬
    {
        return ERROR;
    }
    return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  �жϳ��״̬λ
  * @param  ��
  * @retval ���
  *      @arg SUCCESS �Ѿ����
  *      @arg ERROR û�г��
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus BJ_GetChargeBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_CHARGE))  // �ж�BIT���Ե�״̬
    {
        return SUCCESS;
    }
    return ERROR;
}

/**
  *************************************************************************************
  * @brief  �жϷŵ�״̬λ
  * @param  ��
  * @retval ���
  *      @arg SUCCESS �Ѿ��ŵ�
  *      @arg ERROR û�зŵ�
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus BJ_GetDisChargeBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_DISCHARGE))  // �ж�BIT���Ե�״̬
    {
        return SUCCESS;
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  �жϷŵ�״̬λ
  * @param  ��
  * @retval ���
  *      @arg SUCCESS �Ѿ��ŵ�
  *      @arg ERROR û�зŵ�
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus BJ_GetExpenBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_WAIT_EXPEN))  // �ж�BIT���Ե�״̬
    {
        return SUCCESS;
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  �ж�д����״̬λ
  * @param  ��
  * @retval ���
  *      @arg SUCCESS �Ѿ�д����
  *      @arg ERROR û��д����
  * @author ZH
  * @date   2018��3��10��
  * @note   оƬ���뱣����ȫ�ֱ���DetProtocol.RecData.Data,��1���ֽ���ͬ��ͷ
  *************************************************************************************  
  */
ErrorStatus BJ_GetTimeWriteBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_SETTIME_OK))  // �ж�BIT���Ե�״̬
    {
        return SUCCESS;
    }
    return ERROR;
}

void BJ_SaveDetInfo(u8 Index)
{
	OSSchedLock();
	
	Channel.Info[Index].ser1 = DetProtocol.RecData.Data[1];   //�����׹������Ϣ
    Channel.Info[Index].ser2 = DetProtocol.RecData.Data[2];
    Channel.Info[Index].ser3  = DetProtocol.RecData.Data[3];
    Channel.Info[Index].fac = DetProtocol.RecData.Data[4];
    Channel.Info[Index].bat  = DetProtocol.RecData.Data[5];
    Channel.Info[Index].year = DetProtocol.RecData.Data[6];
    Channel.Info[Index].freq = GetDetFreqFromHead();
	Channel.Info[Index].type = Channel.Info[Index].bat&0x0F;
	
	Channel.Info[Index].Pwd1 = Channel.Info[Index].ser3;//Ĭ������
	Channel.Info[Index].Pwd2 = 0XFF-Channel.Info[Index].ser1;//Ϊ�˼�����оƬ��������������1���
	OSSchedUnlock();
}
DET_ERR_CODE BJ_DetTypeTest(u8 DetType)
{
	if(UserParam.DetType == DET_TYPE_BJ_T)
	{
		if(DetType != UserParam.DetType)
		{
			return DET_TYPE_ERR; 
		}	
	}
	return DET_PASS;					
}
DET_ERR_CODE BJ_DetPwdTest(DET_INFO* det)
{
	ErrorStatus result;
	u8 pwd;
	
	pwd = det->Pwd1;
	
	result = BJ_DetTimeWr(det->ser1, det->ser2, pwd, 1000);
	if((result == ERROR) && (det->Pwd1 != det->Pwd2))
	{
		pwd = det->Pwd2;
		result = BJ_DetTimeWr(det->ser1, det->ser2, pwd, 1000);
	}
	if(result == ERROR)
	{
		return DET_PASSWORD_ERR;
	}
	det->Pwd1 = pwd;
	
	return DET_PASS;
}

DET_ERR_CODE BJ_DetBitTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
	
	retry1=3;
	do
	{  
		BJ_DetClrAll();	     //����ȫ�ָ�λָ��
		OSTimeDly(50);
		BJ_DetClrAll();	     //����ȫ�ָ�λָ��
		OSTimeDly(50);
		
		BJ_DetBit(det->ser1, det->ser2);     //����BIT1
		result = WaitRxPinHigh(5);
		if(ERROR != result)
		{
			//��ʱ�ȴ�BIT�������,�Ӳ��ο�����Ҫ600ms����/6.7V, 5.5V,�𱬵���68ufҪ1.3��
			OSTimeDlyHMSM(0,0,1,500);
			result = BJ_DetTimeRd(det->ser1,det->ser2);
			if(result == ERROR )
			{//״̬�ض����ִ���
				err_code = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(BJ_GetBitStatus() == ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit����
				}
				else
				{
					err_code = DET_PASS;//BIT����ͨ��
				}				
			}		
		}
		else
		{
			err_code = DET_BIT_NO_RESPONSE;
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
}
DET_ERR_CODE BJ_DetChargeTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
   
	retry1=3;
	do
	{    
		BJ_DetCharge(det->ser1, det->ser2, det->Pwd1);	   //����CHARGE
		result = WaitRxPinHigh(5);
		if(ERROR == result)
		{
			err_code = DET_CHARGE_NO_RESPONSE;			
		}
		else
		{
			WaitRxPinReady(5000);
			OSTimeDly(100);
			
			result = BJ_DetTimeRd(det->ser1,det->ser2);
			if(result == ERROR )
			{//״̬�ض����ִ���
				err_code = DET_CHARGE_STATUS_READ_ERR;	
			}
			else
			{
				if(BJ_GetChargeBitStatus() == ERROR)
				{
					err_code = DET_CHARGE_ERR;	//����
				}
				else
				{
					err_code = DET_PASS;//����ͨ��
				}				
			}				
		}
	}while((result == ERROR)&&(--retry1));
	return err_code;
}
/**
  *************************************************************************************
  * @brief  ׼���𱬲���
  * @param  det оƬ����
  * @retval ���
  * @author ZH
  * @date   2018��6��13��
  * @note   ��
  *************************************************************************************
  */
DET_ERR_CODE BJ_DetExpenTest(DET_INFO* det)
{
	ErrorStatus result;
	
	result = BJ_DetExpen(det->ser1, det->ser2, det->Pwd1); 
	if(result == ERROR )
	{
		return DET_NO_RESPONSE_EXPEN;
	}
	result = BJ_DetTimeRd(det->ser1,det->ser2);
	if(result == ERROR )
	{//����ʱ��ض����ִ���
		return DET_EXPEN_TIMERD_ERR;		//BB
	}

	if(BJ_GetExpenBitStatus() == ERROR)
	{
		return DET_EXPEN_ERR;	//׼���𱬴���			
	}
	return DET_PASS;//׼���𱬲���ͨ��	
}
void BJ_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		BJ_DetClrAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		BJ_DetClrAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
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
void BJ_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if (BJ_DetSerlRd() == ERROR)//����������ض�ָ��	                     
			{
				Channel.Info[i].Status = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
			}
			else
			{
				BJ_SaveDetInfo(i);
			}					
		}		
	}	
}
void BJ_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (BJ_DetSerlRd() == ERROR)//����������ض�ָ��	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//������ͨ�Ŵ���		
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
void BJ_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{		
			Result = BJ_DetTypeTest(Channel.Info[i].type); //�ж�оƬ����
			if(Result != DET_PASS)
			{
				Channel.Info[i].Status = Result;
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
void BJ_AllChannelDetPwdTest(DET_ERR_CODE ErrCode)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if(BJ_DetPwdTest(&Channel.Info[i]) != DET_PASS)
			{
				Channel.Info[i].Status = ErrCode;//�������
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
void BJ_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			BJ_DetClrAll();	     //����ȫ�ָ�λָ��
            OSTimeDly(50);
            BJ_DetBit(Channel.Info[i].ser1, Channel.Info[i].ser2);     //����BIT1
            if(WaitRxPinHigh(5) == ERROR)
            {
                OSTimeDly(50);
                BJ_DetClrAll();	     //����ȫ�ָ�λָ��
                OSTimeDly(100);
                BJ_DetBit(Channel.Info[i].ser1, Channel.Info[i].ser2);     //����BIT1  
                if(WaitRxPinHigh(5) == ERROR)
                {
                    Channel.Info[i].Status = DET_BIT_NO_RESPONSE;	
                }
            }                       
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
            //��ʱ�ȴ�BIT�������,�Ӳ��ο�����Ҫ600ms����/6.7V, 5.5V,�𱬵���68ufҪ1.3��
			OSTimeDlyHMSM(0,0,1,500);
			break;		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			Result = BJ_DetTimeRd(Channel.Info[i].ser1,Channel.Info[i].ser2);          
			if(Result == ERROR)
			{//״̬�ض����ִ���
				Channel.Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else if(BJ_GetBitStatus() == ERROR)
			{				
                Channel.Info[i].Status = DET_BIT_ERR;	//Bit����
			}		
		}
	}
}
void BJ_AllChannelDetChargeTest(void)
{
	u8 i;
	
	for(i = 0; i < Channel.Count;i++)
	{
        if(DET_TESTING == Channel.Info[i].Status)
        {
            ConnectChannel(i);
            BJ_DetCharge(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].Pwd1);	   //����CHARGE
            if(WaitRxPinHigh(5) == ERROR)
            {
                OSTimeDly(50);
                BJ_DetClrAll();	     //����ȫ�ָ�λָ��
                OSTimeDly(100);
                BJ_DetCharge(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].Pwd1);	   //����CHARGE
                if(WaitRxPinHigh(5) == ERROR)
                {
                    Channel.Info[i].Status = DET_CHARGE_NO_RESPONSE;	
                }
            }         
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
			
			if(BJ_DetTimeRd(Channel.Info[i].ser1,Channel.Info[i].ser2) == ERROR)
			{//���ִ���
				Channel.Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else if(BJ_GetChargeBitStatus() == ERROR)
			{
				Channel.Info[i].Status = DET_CHARGE_ERR;	//������
			}		
		}
	}
}

void BJ_DetTest(void)
{	
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******�ŵ�******/
	BJ_AllChannelDetClrAll();
	
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
	BJ_AllChannelDetSerlRd();	
	
    BJ_AllChannelDetPwdTest(DET_WR_CODE_UNLOCK_PWD_ERR);
     
	/*******�ж�оƬƵ���Ƿ�����*******/
	AllChannelDetFreqTest();

	/*********�ж�оƬ���͵Ĺ���************/
	BJ_AllChannelDetTypeTest();

    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
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
	//BIT����
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//����bit��⹦�ܵ�ѡ��
	{ 	
		BJ_AllChannelDetBitTest();
	}

	/*����⣬���Ե���*/
    if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
    {    
        BJ_AllChannelDetChargeTest();

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

	BJ_AllChannelDetClrAll();
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
		if(NeedScan() && (DetOutsideVerify(Channel.BarCode[0].CodeExternal) != ERROR))
		{//�������룬���㹤����10��һ�н���װ�У�ÿ���е��׹�Ҫ������ǰ12λ��ͬ
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 BJ_DetAnalyst(void)
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
	BJ_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	BJ_DetClrAll();
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
		if(ERROR != BJ_DetSerlRd())
		{
			BJ_SaveDetInfo(0);
			AnalystResult |= 0x0001;//�������ȡ�ɹ�
			
            if(DET_PASS == BJ_DetPwdTest(&Channel.Info[0]))
            {
                AnalystResult |= 0x0010;//������ȷ
            }
            
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != BJ_DetTimeRd(Channel.Info[0].ser1, Channel.Info[0].ser2)) 
			{
				AnalystResult |= 0x0002;//����ʱ��ض�����
			}
			OSTimeDlyHMSM(0,0,0,10);
			if(DET_PASS == BJ_DetBitTest(&Channel.Info[0]))
			{
				AnalystResult |= 0x0004;//BIT��������
			}
			
			//д����ʱ����
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != BJ_DetTimeWr(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].Pwd1, 0x100))
			{
				AnalystResult |= 0x0008;//д��������
			}
		
			OSTimeDlyHMSM(0,0,0,10);
			Result = BJ_DetChargeTest(&Channel.Info[0]);
			if(DET_PASS == Result) 
			{			
				AnalystResult |= 0x0020;//�������
			}
			
			BJ_DetMask(Channel.Info[0].ser1,Channel.Info[0].ser2);//�����׹ܣ��������·������׹�
			
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
			BJ_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);
			BJ_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);			
		}	
	}

	return AnalystResult;
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
void BJ_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******�ŵ�******/
	BJ_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//���߶�·����
	AllChannelOpenCircuitTest(1);//���߶�·����
			
	/*******���˿ڵ�ѹ�Ƿ�����******/
//	AllChannelVoltageTest(0);//�Ȳ��Ե������ٲ��Ե�ѹ����Ȼ��ѹ���Բ�׼����ΪAD������ĵ��ݳ�����
	
	/***������ض����****/
	BJ_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void BJ_WaitAllChannelDetOff(void)
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
			if (BJ_DetCheckIn() != ERROR)//����������ض�ָ��	                     
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
void BJ_DetDataBindingTest(void)
{
	CLOSE_HV_SW();//�رո�ѹ�������
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******�ŵ�******/
	BJ_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//���߶�·����
	AllChannelOpenCircuitTest(0);//���߶�·����
    
	/***������ض����****/
	BJ_AllChannelDetSerlRd();	
     
	/*******�ж�оƬƵ���Ƿ�����*******/
	AllChannelDetFreqTest();

	/*********�ж�оƬ���͵Ĺ���************/
	BJ_AllChannelDetTypeTest();
    
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
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
    
	BJ_AllChannelDetClrAll();
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
		if(NeedScan() && (DetOutsideVerify(Channel.BarCode[0].CodeExternal) != ERROR))
		{//�������룬���㹤����10��һ�н���װ�У�ÿ���е��׹�Ҫ������ǰ12λ��ͬ
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}


