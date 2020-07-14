#include "includes.h"

TEST_PARAM test_param;
CHANNEL Channel;
TEST_PARAM test_param_cfg[3];//�볧��0�����Ʒ����1����Ʒ����2



//==================================================================================================
//| �������� | DetCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹������������
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
DET_ERR_CODE DetLVCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
	
	OSSchedLock();
	Channel.XA2_Info[Index].lv_current = value; //��ѹ�µķ������ֵ
	OSSchedUnlock();	
	
	if(Channel.XA2_Info[Index].lv_current > test_param.det_lv_current_max)
	{
	   	return LV_CURRENT_MORE;//���������
	}
	else if(Channel.XA2_Info[Index].lv_current < test_param.det_lv_current_min) 
	{
	    return LV_CURRENT_LESS;//�������С
	}
	else
	{
	  	return DET_PASS;//�����������Ҫ��
	}
}

//==================================================================================================
//| �������� | DetReverseCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹ܷ����������
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
DET_ERR_CODE DetLVReverseCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //��ѹ�µķ������ֵ
	
	OSSchedLock();
	Channel.XA2_Info[Index].lv_reverse_current = value; //��ѹ�µķ������ֵ
	OSSchedUnlock();	
	 
	if(Channel.XA2_Info[Index].lv_reverse_current > test_param.det_lv_current_max)
	{
	   	return LV_REVERSE_CURRENT_MORE;//���������
	}
	else if(Channel.XA2_Info[Index].lv_reverse_current < test_param.det_lv_current_min) 
	{
	    return LV_REVERSE_CURRENT_LESS;//�������С
	}
	else
	{
	  	return DET_PASS;//�����������Ҫ��
	}
}

//==================================================================================================
//| �������� | DetCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹������������
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
DET_ERR_CODE DetHVCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //��ѹ�µ��������ֵ
	
	OSSchedLock();
	Channel.XA2_Info[Index].hv_current = value; //��ѹ�µķ������ֵ
	OSSchedUnlock();	

	if(Channel.XA2_Info[Index].hv_current > test_param.det_hv_current_max)
	{
	   	return HV_CURRENT_MORE;//���������
	}
	else if(Channel.XA2_Info[Index].hv_current < test_param.det_hv_current_min) 
	{
	    return HV_CURRENT_LESS;//�������С
	}
	else
	{
	  	return DET_PASS;//�����������Ҫ��
	}
}

//==================================================================================================
//| �������� | DetReverseCurrentTest 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �׹ܷ����������
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
DET_ERR_CODE DetHVReverseCurrentTest(u8 Index)
{
	u16 value;
	
	value = GetCurrentValue(10, NULL); //��ѹ�µķ������ֵ
	
	OSSchedLock();
	Channel.XA2_Info[Index].hv_reverse_current = value; //��ѹ�µķ������ֵ
	OSSchedUnlock();
	
	if(Channel.XA2_Info[Index].hv_reverse_current > test_param.det_hv_current_max)
	{
	   	return HV_REVERSE_CURRENT_MORE;//���������
	}
	else if(Channel.XA2_Info[Index].hv_reverse_current < test_param.det_hv_current_min) 
	{
	    return HV_REVERSE_CURRENT_LESS;//�������С
	}
	else
	{
	  	return DET_PASS;//�����������Ҫ��
	}
}

void ClearDetInfo(void)
{
	u8 i = 0;
	
	OSSchedLock();
	for(i = 0; i < Channel.Count;i++)
	{
		memset(&Channel.XA2_Info[i], 0x00,sizeof(Channel.XA2_Info[i]));
        memset(&Channel.XA2_Info[i], 0x00,sizeof(Channel.XA2_Info[i]));
		Channel.XA2_Info[i].Status = DET_TESTING;	
	}
	OSSchedUnlock();
}
void ClearNewStatus(void)
{
	u8 i = 0;
	
	OSSchedLock();
	for(i = 0; i < Channel.Count;i++)
	{
		Channel.NewStatus[i] = DET_TESTING;	
	}
	OSSchedUnlock();
}
u32 GetDetFreqFromTimerCount(u16 count)
{
	
	//��ʱ��3������6��Ƶ��Ϊ12M,count��Ӧ�����׹ܷ��ص�1λ����ʱ�䣬��Ӧ�׹�16��CLK
	//t = 1/12000000*count/16

	return (192000000/count);
}
DET_ERR_CODE DetFreqTest(u32 freq)
{          	   
	if(freq > test_param.det_freq_max)
	{
		return DET_FREQ_MAX;   //Ƶ�ʹ������
	}
	else if(freq < test_param.det_freq_min)
	{
		return DET_FREQ_MIN;   //Ƶ�ʹ������
  	}
	else
	{
		return DET_PASS;
	}
}
/**
  *************************************************************************************
  * @brief  �ȴ�����С��ָ��ֵ
  * @param  CurrentValue ������uA
  * @param  TimeOut ��ʱʱ�� 
  * @retval SUCCESS �ɹ��� ERRORʧ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
ErrorStatus WaitCurrentReady(u8 CurrentValue, u16 TimeOut)
{
	u32 c_time;
	
	//��ʱ�ȴ����,ʾ����ʵ�⣬����1�롣�޸�Ӳ���󣬿ɼ��ٵ�90ms����
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GetCurrentValue(10, NULL) < (CurrentValue*100UL))
		{
			OSTimeDlyHMSM(0,0,0,5);
			if(GetCurrentValue(10, NULL) < (CurrentValue*100UL))
			{
				return SUCCESS;
			}			
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  �ȴ��������Ŵ�������״̬(�͵�ƽ)
  * @param  TimeOut ��ʱʱ��
  * @retval SUCCESS �ɹ��� ERRORʧ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
ErrorStatus WaitRxPinReady(u16 TimeOut)
{
	u32 c_time;
	
	//��ʱ�ȴ����,ʾ����ʵ�⣬����1�롣�޸�Ӳ���󣬿ɼ��ٵ�90ms����
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GET_RX_PIN() == 0)
		{
			OSTimeDlyHMSM(0,0,0,5);
			if(GET_RX_PIN() == 0)
			{
				return SUCCESS;
			}			
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  �ȴ��������Ŵ��ڸߵ�ƽ
  * @param  TimeOut ��ʱʱ��
  * @retval SUCCESS �ɹ��� ERRORʧ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************
  */
ErrorStatus WaitRxPinHigh(u16 TimeOut)
{
	u32 c_time;
	
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(GET_RX_PIN() != 0)
		{
			return SUCCESS;			
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  �жϲ��Բ�����Ӧ����λ�Ƿ��Ѿ���λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
u8 TestParamIsSet(u32 MaskBit)
{
	if((test_param.test_ctrl & MaskBit) == MaskBit)
	{
		return 1;
	}
	return 0;
}
void SoundActionByStatus(void)
{
	u8 i=0;
	
	switch(UserParam.Sound)
	{
		case FINISHED_RING:
			TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
			break;
		case NOT_RING:
			break;
		case RIGHT_RING://ȫ����ȷʱ��
			for(i=0;i<Channel.Count;i++)
			{
				if(DET_PASS != Channel.XA2_Info[i].Status)
				{
					return;
				}
			}		
			TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
			break;
		case ERROR_RING://�д���ʱ��
			for(i=0;i<Channel.Count;i++)
			{
				if(DET_PASS != Channel.XA2_Info[i].Status)
				{
					TaskCreate(APP_SOUND_TASK_PRIO, (void*)1);
					return;
				}
			}		
			break;								
	}
}

u8 GetChannelStatusCount(DET_ERR_CODE Status)
{
	u8 i=0;
    u8 Count = 0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(Status == Channel.XA2_Info[i].Status)
		{
			Count++;
		}
	}
	OSSchedUnlock();
    return Count;
}
u8 GetChannelNewStatusCount(DET_ERR_CODE Status)
{
	u8 i=0;
    u8 Count = 0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(Status == Channel.NewStatus[i])
		{
			Count++;
		}
	}
	OSSchedUnlock();
    return Count;
}
void AllChannelStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus)
{
	u8 i=0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(OldStatus == Channel.XA2_Info[i].Status)
		{
			Channel.XA2_Info[i].Status = NewStatus;
		}
	}
	OSSchedUnlock();
}
void AllChannelNewStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus)
{
	u8 i=0;
	
	OSSchedLock();
	for(i=0;i<Channel.Count;i++)
	{
		if(OldStatus == Channel.NewStatus[i])
		{
			Channel.NewStatus[i] = NewStatus;
		}
	}
	OSSchedUnlock();
}
/**
  *************************************************************************************
  * @brief  �ж��Ƿ�����ͨ�����Ѿ�������
  * @param  ��
  * @retval 0 ��������ͨ����������
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
u8 AllChannelTestEnd(void)
{
	u8 i;
	
	for(i=0;i<Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			return 0;			
		}
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  �ж��Ƿ�����ͨ�����Ѿ�������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
u8 AllChannelTestEndFromBuffer(DET_ERR_CODE* Status)
{
	u8 i;
	
	for(i=0;i<Channel.Count;i++)
	{
		if(DET_TESTING == Status[i])
		{
			return 0;			
		}
	}
	return 1;
}

/**
  *************************************************************************************
  * @brief  ���߶�·����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelLinShortTest(u8 type)
{
	u8 i;
	DET_ERR_CODE* Status;
      
	for(i=0;i<Channel.Count;i++)
	{
        if(type == 0)
        {
            Status = &Channel.XA2_Info[i].Status;
        }
        else
        {
            Status = &Channel.NewStatus[i];
        }    
		if(DET_TESTING == (*Status))
		{
			ConnectChannel(i);
			OSTimeDly(5);
			if(GetLinABVoltage(i, NULL) < 1000)
			{
				*Status = DET_SHORT_ERR;//���߶�·
			}		
		}
	}
}

/**
  *************************************************************************************
  * @brief  ���߶�·����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelOpenCircuitTest(u8 type)
{
	u8 i;
	DET_ERR_CODE* Status;
    	
	for(i=0;i<Channel.Count;i++)
	{
        if(type == 0)
        {
            Status = &Channel.XA2_Info[i].Status;
        }
        else
        {
            Status = &Channel.NewStatus[i];
        }    
		if(DET_TESTING == (*Status))
		{
			ConnectChannel(i);
			OSTimeDly(5);
			if(GetCurrentValue(10, NULL) < 500)
			{//����С��2uA
				*Status = DET_OPEN_CIRCUIT_ERR;//���߶�·
			}			
		}
	}
}
/**
  *************************************************************************************
  * @brief  ��ѹ�µ�����/�������
  * @param  ReverseFlag  0ʱ������1�ⷴ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelCurrentTest(u8 ReverseFlag)
{
	DET_ERR_CODE result = DET_PASS;
	u8 i;
	u8 Ok[DET_CHANNEL_MAX];
	u8 Less[DET_CHANNEL_MAX];
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u32 c_time, timer_out;
	
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;
		Ok[i] = 0;
		Less[i] = 0;		
	}
	
	timer_out = test_param.det_lv_current_time_out;//��λΪ��
	timer_out *= 1000;//ms
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(50);	
		for(i=0; i<Channel.Count; i++)
		{
			if(DET_TESTING == Status[i])
			{
				ConnectChannel(i);
				OSTimeDly(5);
				if(ReverseFlag == 0)
				{//���������
					result = DetLVCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (LV_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				else
				{//�ⷴ�����
					result = DetLVReverseCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (LV_REVERSE_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				if ((Ok[i] >= 3) || (Less[i] >= 3))
				{
					Status[i] = result;
				}
			}				
		}
		//�ж��Ƿ�����ͨ�����Ѿ�������
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_TESTING == Status[i])
		{
			if(ReverseFlag == 0)
			{
				Status[i] = LV_CURRENT_MORE;
			}
			else
			{
				Status[i] = LV_REVERSE_CURRENT_MORE;
			}		
		}
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}
}

/**
  *************************************************************************************
  * @brief  ���ߵ�ѹ����
  * @param  Flag  0ʱ��ѹ��1ʱ��ѹ
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelVoltageTest(u8 Flag)
{
	u16 Voltage;
	u32 c_time;
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;	
	}
	
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(10);	
		for(i=0; i<Channel.Count; i++)
		{
			if((DET_TESTING == Channel.XA2_Info[i].Status) && (DET_PASS != Status[i]))
			{
				ConnectChannel(i);
				OSTimeDly(5);
				Voltage = GetLinABVoltage(i, NULL);
				if(Flag == 0)
				{//��ѹ
					if((Voltage > LINAB_LV_VOLTAGE_MIN) && (Voltage < LINAB_LV_VOLTAGE_MAX))
					{
						Status[i] = DET_PASS;				
					}
					else
					{
						if(Voltage < LINAB_LV_VOLTAGE_MIN)
						{
							Status[i] = LV_VOLTAGE_MIN;
						}
						else
						{
							Status[i] = LV_VOLTAGE_MAX;
						}
					}				
				}
				else
				{//��ѹ
					if((Voltage > LINAB_HV_VOLTAGE_MIN) && (Voltage < LINAB_HV_VOLTAGE_MAX))
					{
						Status[i] = DET_PASS;					
					}
					else
					{
						if(Voltage < LINAB_HV_VOLTAGE_MIN)
						{
							Status[i] = HV_VOLTAGE_MIN;
						}
						else
						{
							Status[i] = HV_VOLTAGE_MAX;
						}
					}				
				}
			}		
		}
		//�ж��Ƿ�����ͨ�����Ѿ�������
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}		
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}	
}
/**
  *************************************************************************************
  * @brief  Ƶ�ʲ���
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelDetFreqTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			Result = DetFreqTest(Channel.XA2_Info[i].freq);
			if(Result != DET_PASS)
			{
				Channel.XA2_Info[i].Status = Result;
			}					
		}		
	}
}
void SetAllChannelBarCodeIdle(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		Channel.BarCode[i].Status = BARCODE_IDLE;
	}	
}
u8 AllChannelIsOpenCircuit(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		OSTimeDly(5);
		
		if(GetCurrentValue(10, NULL) >= 500)
		{
			return 0;
		}		
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  ��ѹ�µ�����/�������
  * @param  ReverseFlag  0ʱ������1�ⷴ��
  * @retval ��
  * @author ZH
  * @date   2016��11��21��
  * @note   ��
  *************************************************************************************
  */
void AllChannelDetHVCurrentTest(u8 ReverseFlag)
{
	DET_ERR_CODE result = DET_PASS;
	u8 i;
	u8 Ok[DET_CHANNEL_MAX];
	u8 Less[DET_CHANNEL_MAX];
	DET_ERR_CODE Status[DET_CHANNEL_MAX];
	u32 c_time, timer_out;
	
	if(AllChannelTestEnd())
	{
		return;
	}
	for(i=0; i<Channel.Count; i++)
	{
		Status[i] = Channel.XA2_Info[i].Status;
		Ok[i] = 0;
		Less[i] = 0;		
	}
	
	timer_out = test_param.det_hv_current_time_out;//��λΪ��
	timer_out *= 1000;//ms
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+timer_out/(1000/OS_TICKS_PER_SEC)))	
	{
		OSTimeDly(50);	
		for(i=0; i<Channel.Count; i++)
		{
			if(DET_TESTING == Status[i])
			{
				ConnectChannel(i);
				OSTimeDly(5);
				if(ReverseFlag == 0)
				{//���������
					result = DetHVCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (HV_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				else
				{//�ⷴ�����
					result = DetHVReverseCurrentTest(i);
					if (DET_PASS == result) 
					{
						Ok[i]++;
					}
					else if (HV_REVERSE_CURRENT_LESS == result) 
					{  
						Less[i]++;
					}
					else
					{
						Ok[i] = 0;
						Less[i] = 0;
					}				
				}
				if ((Ok[i] >= 3) || (Less[i] >= 3))
				{
					Status[i] = result;
				}
			}				
		}
		//�ж��Ƿ�����ͨ�����Ѿ�������
		if(AllChannelTestEndFromBuffer(Status))
		{
			break;
		}
	}
	for(i=0; i<Channel.Count; i++)
	{	
		if(DET_TESTING == Status[i])
		{
			if(ReverseFlag == 0)
			{
				Status[i] = HV_CURRENT_MORE;
			}
			else
			{
				Status[i] = HV_REVERSE_CURRENT_MORE;
			}		
		}
		if(DET_PASS != Status[i])
		{
			Channel.XA2_Info[i].Status = Status[i];			
		}
	}
}
/**
  *************************************************************************************
  * @brief  ����ȫ0���
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��01��19��
  * @note   ��
  *************************************************************************************
  */
u8 DetCodeIsAllZero(DET_INFO* Det)
{
    if ((0 == Det->ser1) && (0 == Det->ser2) && (0 == Det->ser3) && 
        (0 == Det->fac) && (0 == Det->bat) && (0 == Det->year))
    {
        return 1;
    }    
    return 0;
}
/**
  *************************************************************************************
  * @brief  ����ȫ0���
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2017��02��10��
  * @note   ��
  *************************************************************************************
  */
//void XA_AllChannelDetCodeAllZeroVerify(void)
//{
//	u8 i=0;
//	
//	for(i=0; i<Channel.Count; i++)
//	{
//		if(DET_TESTING == Channel.XA2_Info[i].Status)
//		{
//			if (DetCodeIsAllZero(&Channel.XA2_Info[i]))
//			{
//				Channel.XA2_Info[i].Status = DET_CODE_ALL_ZERO;//����ȫ0
//			}							
//		}		
//	}	
//}


/**
  *************************************************************************************
  * @brief  �׹ܼ��
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2017��02��10��
  * @note   ��
  *************************************************************************************
  */
void DetTest(void)
{
    OpData.NetModeNext = 0;
    if(UserParam.NetMode)
    {
        OPEN_IO_RELAY1();    
    }

    if(UserParam.Stage == DET_CODE_VERIFY)
	{
		XA2_DetCodeVerify();
	}
	else
	{
		XA2_DetTest();
	}
    OpData.NetModeNext = 0;
    CLOSE_IO_RELAY1();
}
/**
  *************************************************************************************
  * @brief  �׹ܷ���,��ͨ��1
  * @param[in]  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��5��
  * @note   
  *************************************************************************************  
  */
u16 DetAnalyst(void)
{
//    if(DeviceInfo.DetClass == 0)
//    {
        return XA2_DetAnalyst();
//    }

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
void QueryDetConnectStatus(void)
{
    XA2_QueryDetConnectStatus();
}
/**
  *************************************************************************************
  * @brief  �ȴ�����ͨ���׹ܶ�����
  * @param[in]  ��
  * @retval ��
  * @author ZH
  * @date   2016��12��5��
  * @note   
  *************************************************************************************  
  */
void WaitAllChannelDetOff(void)
{
     XA2_WaitAllChannelDetOff();
}
