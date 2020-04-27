#include "includes.h"

static STR_COMM  RS485TempBuffer = {0,RECIEVING,0};

SLAVE_INFO SlaveInfo;

/**
  *************************************************************************************
  * @brief  �Ѵӻ�����ֵת��Ϊ��ַ
  * @param  SlaveIndex���ӻ�����  
  * @retval ��ַ
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
u8 GetAddrNameFromIndex(u8 SlaveIndex)
{
	if(SlaveIndex != 0xFF)
	{
		return (SlaveIndex<26)?'A'+SlaveIndex:'a'+(SlaveIndex-26);
	}
	else
	{
		return 0x00;
	}	
}

/**
  *************************************************************************************
  * @brief  �Ѵӻ���ַת��Ϊ����ֵ
  * @param  AddrName����ַ 
  * @retval �ӻ����� 
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
u8 GetIndexFromAddrName(u8 AddrName)
{
	if ((AddrName >= 'A') && (AddrName <= 'Z'))
	{
		return (AddrName-'A');
	}
	else if ((AddrName >= 'a') && (AddrName <= 'z'))
	{
		return (26+AddrName-'a');
	}
	else
	{
		return 0x00;
	}	
}
/**
  *************************************************************************************
  * @brief  ��ȡ�ӻ���ַ
  * @param  Index���豸����  
  * @param  Result�� 
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveAddr(u8* AddIndex)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//Э���ַ
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�			
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 5)
					{
						if ((RS485TempBuffer.uch_buf[1] == 0xFF)  && 
							(RS485TempBuffer.uch_buf[2] == 0xFF) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							*AddIndex = GetIndexFromAddrName(RS485TempBuffer.uch_buf[0]);
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  ���ôӻ�RTC
  * @param  Index��Ҫ�����豸����  
  * @param  Result�� 
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveRtc(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		if(Index == 0xFF)
		{//�㲥ָ��
			RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//Э���ַ
		}
		else
		{
			RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		}
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x15;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RTC_Read_Time();
		
		OSSchedLock();
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r%d\r%d\r%d\r%d\r%d\r", 
			RTC_Type.year,RTC_Type.month,RTC_Type.day,RTC_Type.hour,RTC_Type.minute,RTC_Type.second);
		
		OSSchedUnlock();
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		if(GetAddrNameFromIndex(Index) == 0x00)
		{//�㲥ָ��
			SET_RS485_SEND();//485����Ϊ����״̬
			return SLAVE_TRUE;
		}
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x15) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{							
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����豸�����ߵ�ѹ����״̬
  * @param  Index��Ҫ�����豸����
  * @param  State��Ҫ�����豸����
  *  @arg @b '1' ��ǰ���ߵ�ѹ�Ѿ��ر�
  *  @arg @b '2' ��ѹ��û���,Ϊ��ѹ״̬
  *  @arg @b '3' ��ѹ״̬  
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBusVoltageCtrlState(u8 Index, u8* State)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x17;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '1';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x17) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							*State = RS485TempBuffer.uch_buf[5];
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}

/**
  *************************************************************************************
  * @brief  ͨ��485�����豸�����ߵ�ѹ����״̬
  * @param  Index��Ҫ�����豸����
  * @param  State��Ҫ�����豸����
  *  @arg @b 1 �����ߵ�������ֵ
  *  @arg @b 2 �����ߵ�ѹ����ֵ
  *  @arg @b 3 ����ص�ѹ����ֵ
  *  @arg @b 4 ������©�����ֵ
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveVoltageOrCurrent(u8 Index, u8 Arg, u16* Value)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x18;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", Arg);
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+3500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 9)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x18) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							//���ط��ص���AD����ֵ��ֻ�ж���û�У���ʹ��
							if(ERROR == GetDecValueFromSeparator('\r', 2, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							*Value = (u16)U32Value;
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����豸������汾
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveVerison(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x00) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							if ((RS485TempBuffer.bytenum-6) <= sizeof(SlaveInfo.Info[0].Version))
							{
								memset(SlaveInfo.Info[Index].Version, 0x00, sizeof(SlaveInfo.Info[0].Version));
								memcpy(SlaveInfo.Info[Index].Version, &RS485TempBuffer.uch_buf[5], RS485TempBuffer.bytenum-6);
								return SLAVE_TRUE;
							}
							else
							{
								return SLAVE_FALSE;
							}						
						}			
					}
					return SLAVE_FALSE;					
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����豸��ID
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveID(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x01;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x01) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							if ((RS485TempBuffer.bytenum-6) <= sizeof(SlaveInfo.Info[0].ID))
							{
								memset(SlaveInfo.Info[Index].ID, 0x00, sizeof(SlaveInfo.Info[0].ID));
								memcpy(SlaveInfo.Info[Index].ID, &RS485TempBuffer.uch_buf[5], RS485TempBuffer.bytenum-6);
								return SLAVE_TRUE;
							}
							else
							{
								return SLAVE_FALSE;
							}						
						}			
					}
					return SLAVE_FALSE;
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  ���ƴ��豸��ʼĳ�����
  * @param  Index��Ҫ�����豸����
  * @param  Operate������
  *    @arg @b 0://�ر��豸  
  *    @arg @b 3://��ʼ�������׹ܲ��� 
  *    @arg @b 6://��ʼ��������г��   
  * @param  Result��Ҫ�����豸����  
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveStartOperate(u8 Index, u8 Operate, u8* Result)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		if(Index == 0xFF)
		{//�㲥ָ��
			RS485Data.uch_buf[RS485Data.bytenum++] = 0x00;//Э���ַ
		}
		else
		{
			RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		}
		
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", Operate);
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
        
		if(GetAddrNameFromIndex(Index) == 0x00)
		{//�㲥ָ��
			SET_RS485_SEND();//485����Ϊ����״̬
			return SLAVE_TRUE;
		}
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							if(Result != NULL)
							{
								*Result = RS485TempBuffer.uch_buf[5];
							}
							
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����豸���׹ܲ��Խ��
  * @param  Index��Ҫ�����豸����
  * @param  Data�����ݻ���������˳��Ϊ��ע���׹ܡ������׹ܡ����Ժ��ѹ�����Ժ����������©����
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastDetTestResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '4';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if((RS485TempBuffer.uch_buf[5] == '1') || (RS485TempBuffer.uch_buf[5] == '2'))
							{
								//��˳��Ϊ��ע���׹ܡ������׹ܡ����Ժ��ѹ�����Ժ����������©����
								for(i= 2;i<7;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}								
								if (RS485TempBuffer.uch_buf[5] == '1')
								{
									return SLAVE_TRUE;
								}
								else
								{
									return SLAVE_VOLATILE;	//û�в������
								}
							}																	
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  ͨ��485�����豸����������
  * @param  Index��Ҫ�����豸����
  * @param  Data�����ݻ���������˳��Ϊ�������ѹ��������������ǰ��ѹ�����ǰ����
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastChargeResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '7';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(RS485TempBuffer.uch_buf[5] == '1')
							{
								//��˳��Ϊ�������ѹ��������������ǰ��ѹ�����ǰ����
								for(i= 2;i<6;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}								
								
								return SLAVE_TRUE;	
							}
							return SLAVE_VOLATILE;	//û�����											
						}			
					}
					return SLAVE_FALSE;
				}
				break;				
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  ͨ��485�����豸����������
  * @param  Index��Ҫ�����豸����
  * @param  Data�����ݻ���������˳��Ϊ���𱬽���������ѹ���������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveBlastResult(u8 Index, u16* Data)
{
	u32 c_time;
	u32 U32Value;
	u8 i;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.uch_buf[RS485Data.bytenum++] = '9';
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x0D;
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) &&
							(RS485TempBuffer.uch_buf[6] == 0x0D) &&
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							*Data++ = (u16)RS485TempBuffer.uch_buf[5];
							if(RS485TempBuffer.uch_buf[5] == '1')
							{
								//��˳��Ϊ����ǰ��ѹ����ǰ����
								for(i= 2;i<4;i++)
								{
									if(ERROR == GetDecValueFromSeparator('\r', i, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
									{
										return SLAVE_FALSE;
									}
									*Data++ = (u16)U32Value;								
								}									
							}
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;						
				}
				break;			
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;
}

/**
  *************************************************************************************
  * @brief  ��ע���׹ܡ������׹ܡ��׹����͡���ص�ѹ
  * @param  Index��Ҫ�����豸����
  * @param  SecondArg�����Ʋ�����0��ʾ�����´ӻ����棬1��ʾ����
  * @retval ���
  * @author ZH
  * @date   2015��1��06��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveTestInfo(u8 SlaveIndex, u8 SecondArg)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x1D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "1\r%d\r", SecondArg);
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum > 5)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x1D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].RegCount = (u16)U32Value;
							
							if(ERROR == GetDecValueFromSeparator('\r', 2, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].ErrorCount = (u16)U32Value;
							
							if(ERROR == GetDecValueFromSeparator('\r', 3, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5,&U32Value))
							{
								return SLAVE_FALSE;
							}
							SlaveInfo.Info[SlaveIndex].Type = (u8)U32Value;
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}

/**
  *************************************************************************************
  * @brief  ��ע���׹ܡ������׹ܡ��׹����͡���ص�ѹ
  * @param  Index��Ҫ�����豸����
  * @param  SecondArg�����Ʋ�����0��ʾ�����´ӻ����棬1��ʾ����
  * @retval ���
  * @author ZH
  * @date   2015��1��06��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetIndexInfo(u8 SlaveIndex, u16 StartIndex, u8 Number)
{
	u32 c_time;
	u16 Index, Bytes; 
	u32 U32Value;
	u8 Retry = 5;
	u16 DetIndex;
	u8 Buffer[20];
	u8 U8Value;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2D;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r%d\r", StartIndex, Number);
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+3500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum >= 9)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2D) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[RS485TempBuffer.bytenum-1] == 0x0D))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
							{
								return SLAVE_FALSE;
							}
							if(U32Value != 1)
							{
								return SLAVE_FALSE;
							}
							
							DetIndex = StartIndex;
							U8Value = 0;
							while(U8Value < Number)
							{
								//�׹�����\�׺�\Ƶ��\����\״̬��
								if(ERROR == GetDataFromSeparator('\r', 2+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &Index, &Bytes))
								{
									return SLAVE_FALSE;
								}
								if(Bytes != 16)
								{
									return SLAVE_FALSE;
								}
								HexStrToCharStr(&RS485TempBuffer.uch_buf[5+Index], Bytes, Buffer);
								Det.Info[DetIndex].Serl.U8Buffer[0] = Buffer[0];
								Det.Info[DetIndex].Serl.U8Buffer[1] = Buffer[1];
								Det.Info[DetIndex].Serl.U8Buffer[2] = Buffer[2];
								Det.Info[DetIndex].Serl.U8Buffer[3] = Buffer[3];
								Det.Info[DetIndex].Pwd.U8Buffer[0] = Buffer[4];
								Det.Info[DetIndex].Pwd.U8Buffer[1] = Buffer[5];
								Det.Info[DetIndex].Pwd.U8Buffer[2] = Buffer[6];
								Det.Info[DetIndex].Pwd.U8Buffer[3] = Buffer[7];
								if(ERROR == GetDecValueFromSeparator('\r', 3+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Hole.Name = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 4+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Hole.Inside = (u8)U32Value;
								
								if(ERROR == GetDecValueFromSeparator('\r', 5+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Freq.U32Value = U32Value;	
								
								if(ERROR == GetDecValueFromSeparator('\r', 6+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Delay = U32Value;	
								
								if(ERROR == GetDecValueFromSeparator('\r', 7+6*U8Value, &RS485TempBuffer.uch_buf[5],RS485TempBuffer.bytenum-5, &U32Value))
								{
									return SLAVE_FALSE;
								}
								Det.Info[DetIndex].Err = (u8)U32Value;	

								DetIndex++;
								U8Value++;
							}
							
					
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;	
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ��ע���׹ܡ������׹ܡ��׹����͡���ص�ѹ
  * @param  Index��Ҫ�����豸����
  * @param  SecondArg�����Ʋ�����0��ʾ�����´ӻ����棬1��ʾ����
  * @retval ���
  * @author ZH
  * @date   2015��1��06��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetHoleNumber(u8 SlaveIndex)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(SlaveIndex);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2F;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�				
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{
					if (RS485TempBuffer.bytenum == (5+DET_MAX_SIZE))
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(SlaveIndex)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2F) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF))
						{
							memcpy(&Det.HoleNumber[0], &RS485TempBuffer.uch_buf[5], DET_MAX_SIZE);
							return SLAVE_TRUE;
						}			
					}
					return SLAVE_FALSE;
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ��ȡ�ӻ����׹���Ϣ�����浽�ļ���
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE GetSlaveDetInfo(u8 SlaveIndex)
{
	u8 U8Value;
	u16 StartIndex = 0;
	
	memset((u8*)&Det.Info[0], 0, sizeof(Det.Info[0])*DET_MAX_SIZE);
	
	U8Value =  SlaveInfo.Info[SlaveIndex].RegCount/10;
	while(U8Value--)
	{
		if(GetSlaveDetIndexInfo(SlaveIndex, StartIndex, 10) == SLAVE_FALSE)//ÿ�ζ�10���׹�
		{
			return SLAVE_FALSE;
		}
		StartIndex += 10;
	}
	if(SlaveInfo.Info[SlaveIndex].RegCount%10)
	{
		if(GetSlaveDetIndexInfo(SlaveIndex, StartIndex, SlaveInfo.Info[SlaveIndex].RegCount%10) == SLAVE_FALSE)
		{
			return SLAVE_FALSE;
		}	
	}
	
	return GetSlaveDetHoleNumber(SlaveIndex);
}
void SlaveInitState(void)
{
	u8 i;
	
	OSSchedLock();
	for(i=0; i<SlaveInfo.Number; i++)
	{
		SlaveInfo.Info[i].State = SLAVE_VOLATILE;
		SlaveInfo.Info[i].WarnBit = 0;
		SlaveInfo.Info[i].ErrorBit = 0;
		SlaveInfo.Info[i].Flags = 0;
	}
	OSSchedUnlock();
}

/**
  *************************************************************************************
  * @brief  ��ȡ���ߵĴӻ���
  * @param  �� 
  * @retval �ӻ���
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveStateCount(SLAVE_STATE State)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == State)
		{
			Count++;
		}
	}
	return Count;
}
/**
  *************************************************************************************
  * @brief  ��ȡ���쳣��Ϣ�Ĵӻ���
  * @param  �� 
  * @retval �ӻ���
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveErrorCount(void)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{
			Count++;
		}
		else if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if(SlaveInfo.Info[i].ErrorBit != 0x00)
			{
				Count++;
			}
		}
	}
	return Count;	
}
u8 GetSlaveOnlineErrorIndex(void)
{
	u8 i;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{
			return i;
		}
	}
	return 0xFF;
}
u8 GetSlaveIndexFromErrorBit(u8 ErrorBit)
{
	u8 i;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if((SlaveInfo.Info[i].ErrorBit & SLAVE_ERROR_BIT_DET_ACCREDIT) == SLAVE_ERROR_BIT_DET_ACCREDIT)
			{
				return i;
			}
		}
	}
	return 0xFF;	
}
/**
  *************************************************************************************
  * @brief  ��ȡ�о�ʾ��Ϣ�Ĵӻ���
  * @param  �� 
  * @retval �ӻ���
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
u8 GetSlaveWarnCount(void)
{
	u8 i,Count=0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_TRUE)
		{
			if(SlaveInfo.Info[i].WarnBit != 0x00)
			{
				Count++;
			}
		}
	}
	return Count;	
}

/**
  *************************************************************************************
  * @brief  ͳ���׹�����������
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void GetSlaveDetNumber(void)
{
	u8 i;
	
	OSSchedLock();
	SlaveInfo.RegCount = 0;
	SlaveInfo.ErrorCount = 0;
	SlaveInfo.Type = 0xFF;
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SLAVE_TRUE ==  SlaveInfo.Info[i].State)
		{
			SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
			SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;		
			if(SlaveInfo.Type == 0xFF)
			{//�Ե�1���豸���׹�������Ϊͳһʹ�õ��׹�����
				SlaveInfo.Type = SlaveInfo.Info[i].Type;
			}
//			if(SlaveInfo.Info[i].Type != SlaveInfo.Type)
//			{
//				SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_DET_TYPE;
//			}
//			else
//			{
//				SlaveInfo.Info[i].ErrorBit &= ~SLAVE_ERROR_BIT_DET_TYPE;
//			}
		}	
	}
	if(SlaveInfo.Type == 0xFF)
	{
		SlaveInfo.Type = 0;
	}
	OSSchedUnlock();
}

/**
  *************************************************************************************
  * @brief  ��ʼ���ӻ��׹�����
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void InitSlaveDetNumber(void)
{
	u8 i;
	
	OSSchedLock();
	SlaveInfo.RegCount = 0;
	SlaveInfo.ErrorCount = 0;
    SlaveInfo.OnlineCount = 0;
	SlaveInfo.Type = 0;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{
		SlaveInfo.Info[i].RegCount = 0x0000;
		SlaveInfo.Info[i].ErrorCount = 0x0000;
	}
	OSSchedUnlock();
}
/**
  *************************************************************************************
  * @brief  ��ע���׹ܡ������׹ܡ��׹����͡���ص�ѹ
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void GetAllSlaveInfo(u8 Arg)
{
	u16 i;
	u8 U8Value;
	SLAVE_STATE StateBuffer;
	
	for(i=0; i<SlaveInfo.Number; i++)
	{		
		//�ôӻ��յ������ˢ��
		StateBuffer = GetSlaveTestInfo(i, 0);		
		if(SLAVE_FALSE != StateBuffer)
		{
			//��ID
			StateBuffer = GetSlaveID(i);
		}
		if(SLAVE_FALSE != StateBuffer)
		{
			//������汾
			StateBuffer = GetSlaveVerison(i);
		}
		
		if(SLAVE_FALSE != StateBuffer)
		{
			//����ص�ѹ
			StateBuffer = GetSlaveVoltageOrCurrent(i, 3, &SlaveInfo.Info[i].Battery);
		}
		if(SLAVE_FALSE != StateBuffer)
		{
			//�����ߵ�ѹ����״̬
			StateBuffer = GetSlaveBusVoltageCtrlState(i, &U8Value);
			if (SLAVE_FALSE != StateBuffer)
			{//���߻����������ѹ״̬
				if(U8Value != '2')
				{
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
				}
				else
				{
					//������©����
					StateBuffer = GetSlaveVoltageOrCurrent(i, 4, &SlaveInfo.Info[i].I_Leak);				
				}
			}				
		}		
		OSSchedLock();
		
		if(SLAVE_FALSE != StateBuffer)
		{
			if(SlaveInfo.Info[i].Battery < 3720)
			{
				SlaveInfo.Info[i].WarnBit |= SLAVE_ERROR_BIT_BAT_LOW;
			}			
		}
		if(Arg == 0)
		{
			if(StateBuffer == SLAVE_FALSE)
			{//ֻ��ʧ��ʱ�Ÿ���
				SlaveInfo.Info[i].State = SLAVE_FALSE;
			}		
		}
		else
		{//���������´ӻ�״̬
			SlaveInfo.Info[i].State = StateBuffer;
		}
		
		OSSchedUnlock();
	}
}

/**
  *************************************************************************************
  * @brief  ���ƴӻ��׹ܲ���
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus AllSlaveDetTest(u8 TestType, u8 TimeShare)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u8 Result;
	u16 U16Buffer[10];

	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485����Ϊ����״̬	
	OSTimeDly(CONNECT_RS485_TIME+100);//�ӳ��㹻ʱ�䣬�ôӻ�ȷ������
	
	AllSlaveStatusReset();
	OSTimeDly(500);
	AllSlaveSetRtc();
	GetAllSlaveInfo(0);//���ӻ��Ļ�����Ϣ
		
	//���жϵ�ǰ�˿��Ƿ��Ѿ�����
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
		{
			if(GetSlaveBusVoltageCtrlState(i, &Result) == SLAVE_FALSE)
			{//�дӻ�ͨ��ʧ��
				SlaveInfo.Info[i].State = SLAVE_FALSE;
			}
			else if(Result == '1')
			{//�ӻ���ǰ�˿ڲ��ڵ�ѹ״̬
				SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
				SlaveInfo.Info[i].State = SLAVE_TRUE;
			}		
		}
        //��ÿ���ӻ�ע����
        if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
        {
            if(GetSlaveTestInfo(i, 0) == SLAVE_FALSE)
            {
                SlaveInfo.Info[i].State = SLAVE_FALSE;
            }
            else
            {
                SlaveInfo.Info[i].Flags = 1;//��ʾ�����Ѷ���ע���׹�
            }
        }
	}
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_FALSE)
		{//GUI�������д���ʱ��ֹͣ��ǰ����
			return ERROR;
		}
		else if ((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
		{//GUI�������д���ʱ��ֹͣ��ǰ����
			return ERROR;
		}
	}
	AllSlaveStatusReset();
	OSTimeDly(500);	
	//���ƴӻ���ʼ�����׹�
    if(TimeShare)
    {
        //��ʱ���
        for(i=0; i<SlaveInfo.Number; i++)
        {
            if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
            {
                if(SlaveInfo.Info[i].Flags == 1)
                {
                    SlaveInfo.Info[i].ErrorCount = 0;
                    SlaveInfo.Info[i].Flags = 2;//��ʾ���ڲ����׹ܣ�����GUI��ʾ
                    
                    if(TestType == 0)
                    {//���ٲ���
                        if(SetSlaveStartOperate(i, 5, NULL) == SLAVE_FALSE)
                        {//�дӻ�ͨ��ʧ��
                            SlaveInfo.Info[i].State = SLAVE_FALSE;
                            return ERROR;
                        }			
                    }
                    else
                    {//�����̲���
                        if(SetSlaveStartOperate(i, 3, NULL) == SLAVE_FALSE)
                        {//�дӻ�ͨ��ʧ��
                            SlaveInfo.Info[i].State = SLAVE_FALSE;
                            return ERROR;
                        }			
                    }                
                }			

                OSTimeDlyHMSM(0, 0, 4, 0);
                while(SlaveInfo.Info[i].State != SLAVE_TRUE)
                {
                    OSTimeDlyHMSM(0, 0, 1, 0);
                    StateBuffer = GetSlaveBlastDetTestResult(i, &U16Buffer[0]);
                    if (StateBuffer != SLAVE_FALSE)
                    {
                        OSSchedLock();
                        SlaveInfo.Info[i].RegCount = U16Buffer[0];
                        SlaveInfo.Info[i].ErrorCount = U16Buffer[1];
                        SlaveInfo.Info[i].V_BeforeTest = U16Buffer[2];
                        SlaveInfo.Info[i].I_BeforeTest = U16Buffer[3];
                        SlaveInfo.Info[i].I_Leak = U16Buffer[4];
                        OSSchedUnlock();
                        if(StateBuffer == SLAVE_TRUE)
                        {//�Ѿ��������
                            //�ض�����ǰ�ȿ�����һ�������׹ܣ���ʡʱ��
                            if((i+1) < SlaveInfo.Number)
                            {
                                if(SlaveInfo.Info[i+1].Flags == 1)
                                {
                                    SlaveInfo.Info[i+1].ErrorCount = 0;
                                    SlaveInfo.Info[i+1].Flags = 2;//��ʾ���ڲ����׹ܣ�����GUI��ʾ
                                    
                                    if(TestType == 0)
                                    {//���ٲ���
                                        if(SetSlaveStartOperate(i+1, 5, NULL) == SLAVE_FALSE)
                                        {//�дӻ�ͨ��ʧ��
                                            SlaveInfo.Info[i+1].State = SLAVE_FALSE;
                                            return ERROR;
                                        }			
                                    }
                                    else
                                    {//�����̲���
                                        if(SetSlaveStartOperate(i+1, 3, NULL) == SLAVE_FALSE)
                                        {//�дӻ�ͨ��ʧ��
                                            SlaveInfo.Info[i+1].State = SLAVE_FALSE;
                                            return ERROR;
                                        }			
                                    }                
                                }                        
                            }
                        
                            OSSchedLock();
                            if(abs(SlaveInfo.Info[i].V_BeforeTest - LIN_NORMAL_LOW_VOLTAGE_H_GOAL) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
                            {//�ж����߷��쳣
                                SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
                            }
                            if(SlaveInfo.Info[i].ErrorCount != 0)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_DET_ERROR;
                            }
                            if(SlaveInfo.Info[i].I_Leak > 500)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_BUS_LEAK;
                            }
                            OSSchedUnlock();
                
                            SlaveInfo.Info[i].Flags = 3;//��ʾ���ڻض��׹����룬����GUI��ʾ
                            
                            //�ض����е��׹ܲ������ݼ��׹����뱣�浽����
                            if (GetSlaveDetInfo(i) == SLAVE_TRUE)
                            {
                                if(ERROR != SaveSlaveDetInfo(i))//���浽�ļ���
                                {
                                    //ͳ�Ƶ�ǰ�Ѿ�������ɵ��׹���
                                    OSSchedLock();
                                    SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
                                    SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;
                                    SlaveInfo.OnlineCount += GetDetOnlineCount();
                                    OSSchedUnlock();
                                }
                                else
                                {
                                    SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_SYSTEM;
                                }
                                SlaveInfo.Info[i].State = SLAVE_TRUE;
                            }
                            else
                            {//�дӻ�ͨ��ʧ��
                                SlaveInfo.Info[i].State = SLAVE_FALSE;						
                            }					
                        }																					
                    }
                    else
                    {//�дӻ�ͨ��ʧ��
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                    }
                    if(SlaveInfo.Info[i].State == SLAVE_FALSE)
                    {
                        return ERROR;
                    }
                    else if((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
                    {
                        return ERROR;
                    }            
                }
            }
        }        
    }
    else
    {//������м��
        //���ƴӻ���ʼ�����׹�
        for(i=0; i<SlaveInfo.Number; i++)
        {
            if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
            {
                if(SlaveInfo.Info[i].Flags == 1)
                {
                    SlaveInfo.Info[i].ErrorCount = 0;
                    SlaveInfo.Info[i].Flags = 2;//��ʾ���ڲ����׹ܣ�����GUI��ʾ
                }
                
                
                if(TestType == 0)
                {//���ٲ���
                    if(SetSlaveStartOperate(i, 5, NULL) == SLAVE_FALSE)
                    {//�дӻ�ͨ��ʧ��
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                        return ERROR;
                    }			
                }
                else
                {//�����̲���
                    if(SetSlaveStartOperate(i, 3, NULL) == SLAVE_FALSE)
                    {//�дӻ�ͨ��ʧ��
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                        return ERROR;
                    }			
                }		
            }
        }
        //�ȴ��������
        while(1)
        {
            OSTimeDlyHMSM(0,0,1,0);
            for(i=0; i<SlaveInfo.Number; i++)
            {
                if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
                {//�ôӻ���û�в������
                    StateBuffer = GetSlaveBlastDetTestResult(i, &U16Buffer[0]);
                    if (StateBuffer != SLAVE_FALSE)
                    {
                        OSSchedLock();
                        SlaveInfo.Info[i].RegCount = U16Buffer[0];
                        SlaveInfo.Info[i].ErrorCount = U16Buffer[1];
                        SlaveInfo.Info[i].V_BeforeTest = U16Buffer[2];
                        SlaveInfo.Info[i].I_BeforeTest = U16Buffer[3];
                        SlaveInfo.Info[i].I_Leak = U16Buffer[4];
                        OSSchedUnlock();
                        if(StateBuffer == SLAVE_TRUE)
                        {//�Ѿ��������
                            OSSchedLock();
                            if(abs(SlaveInfo.Info[i].V_BeforeTest - LIN_NORMAL_LOW_VOLTAGE_H_GOAL) > LIN_NORMAL_LOW_VOLTAGE_LIMIT)
                            {//�ж����߷��쳣
                                SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
                            }
                            if(SlaveInfo.Info[i].ErrorCount != 0)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_DET_ERROR;
                            }
                            if(SlaveInfo.Info[i].I_Leak > 500)
                            {
                                SlaveInfo.Info[i].WarnBit |= SLAVE_WARN_BIT_BUS_LEAK;
                            }
                            OSSchedUnlock();
                            
                            SlaveInfo.Info[i].Flags = 3;//��ʾ���ڻض��׹����룬����GUI��ʾ
                            
                            //�ض����е��׹ܲ������ݼ��׹����뱣�浽����
                            if (GetSlaveDetInfo(i) == SLAVE_TRUE)
                            {
                                if(ERROR != SaveSlaveDetInfo(i))//���浽�ļ���
                                {
                                    //ͳ�Ƶ�ǰ�Ѿ�������ɵ��׹���
                                    OSSchedLock();
                                    SlaveInfo.RegCount += SlaveInfo.Info[i].RegCount;
                                    SlaveInfo.ErrorCount += SlaveInfo.Info[i].ErrorCount;
                                    SlaveInfo.OnlineCount += GetDetOnlineCount();							
                                    OSSchedUnlock();
                                }
                                else
                                {
                                    SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_SYSTEM;
                                }
                                SlaveInfo.Info[i].State = SLAVE_TRUE;
                            }
                            else
                            {//�дӻ�ͨ��ʧ��
                                SlaveInfo.Info[i].State = SLAVE_FALSE;						
                            }					
                        }																					
                    }
                    else
                    {//�дӻ�ͨ��ʧ��
                        SlaveInfo.Info[i].State = SLAVE_FALSE;
                    }			
                }
                if(SlaveInfo.Info[i].State == SLAVE_FALSE)
                {
                    return ERROR;
                }
                else if((SlaveInfo.Info[i].State == SLAVE_TRUE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
                {
                    return ERROR;
                }
            }
            
            if(GetSlaveStateCount(SLAVE_VOLATILE) == 0)
            {//���дӻ����Ѿ��������
                break ;
            }		
        }
    }
	GetSlaveDetNumber();
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  ���ƴӻ�������
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus AllSlaveCharge(void)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u8 Result;
	u16 U16Buffer[10];
	
	SlaveInitState();
	
	//���жϵ�ǰ�˿��Ƿ��Ѿ�����
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(GetSlaveBusVoltageCtrlState(i, &Result) == SLAVE_FALSE)
		{//�дӻ�ͨ��ʧ��
			SlaveInfo.Info[i].State = SLAVE_FALSE;
			return ERROR;
		}
		else if(Result == '1')
		{//�ӻ���ǰ�˿ڲ��ڵ�ѹ״̬
			SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
			SlaveInfo.Info[i].State = SLAVE_TRUE;
			return ERROR;
		}
	}
	
	//���ƴӻ���ʼ�����׹�
	for(i=0; i<SlaveInfo.Number; i++)
	{
		if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
		{
			if(SetSlaveStartOperate(i, 6, NULL) == SLAVE_FALSE)
			{//�дӻ�ͨ��ʧ��
				SlaveInfo.Info[i].State = SLAVE_FALSE;
				return ERROR;
			}		
		}
	}
	
	//�ȴ��������
	while(1)
	{
		OSTimeDlyHMSM(0,0,1,0);
		for(i=0; i<SlaveInfo.Number; i++)
		{
			if(SlaveInfo.Info[i].State == SLAVE_VOLATILE)
			{//�ôӻ���û�г�����
				StateBuffer = GetSlaveBlastChargeResult(i, &U16Buffer[0]);
				if (StateBuffer == SLAVE_TRUE)
				{//�Ѿ�������
					OSSchedLock();
					SlaveInfo.Info[i].V_AfterCharge = U16Buffer[0];
					SlaveInfo.Info[i].I_AfterCharge = U16Buffer[1];
					SlaveInfo.Info[i].V_BeforeCharge = U16Buffer[2];
					SlaveInfo.Info[i].I_BeforeCharge = U16Buffer[3];					
//					if(abs(SlaveInfo.Info[i].V_AfterCharge - Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
                    if((SlaveInfo.Info[i].V_AfterCharge < (LIN_NORMAL_HIGH_VOLTAGE_FACTORY_GOAL-1000)) || 
                        (SlaveInfo.Info[i].V_AfterCharge > LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX))
					{//�ж����߷��쳣
						SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
					}
					
					SlaveInfo.Info[i].State = SLAVE_TRUE;
					OSSchedUnlock();																						
				}
				else if (StateBuffer == SLAVE_FALSE)
				{//�дӻ�ͨ��ʧ��
					SlaveInfo.Info[i].State = SLAVE_FALSE;
				}			
			}
			if(SlaveInfo.Info[i].State == SLAVE_FALSE)
			{
				return ERROR;
			}
			else if((SlaveInfo.Info[i].State == SLAVE_VOLATILE) && (SlaveInfo.Info[i].ErrorBit != 0x00))
			{
				return ERROR;
			}			
		}
		
		if(GetSlaveStateCount(SLAVE_VOLATILE) == 0)
		{//���дӻ����Ѿ��������
			break ;
		}		
	}
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  ���ӻ��𱬽��
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void AllSlaveBlastDet(void)
{
	u8 i;
	SLAVE_STATE StateBuffer;
	u16 U16Buffer[10];
	
	SlaveInitState();

	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	SetSlaveStartOperate(0xFF, 8, NULL);
	OSTimeDlyHMSM(0,0,0,10);
	
	//�ȴ��������
	for(i=0; i<SlaveInfo.Number; i++)
	{
		StateBuffer = GetSlaveBlastResult(i, &U16Buffer[0]);
		if (StateBuffer == SLAVE_TRUE)
		{//�Ѿ������
			OSSchedLock();
			if(U16Buffer[0] == '1')
			{//�Ѿ��������ź�
				SlaveInfo.Info[i].V_BeforeFire = U16Buffer[1];
				SlaveInfo.Info[i].I_BeforeFire = U16Buffer[2];				
			}
			else
			{
				if(U16Buffer[0] == '2')
				{//�ӻ���Ϊ�˿ڵ�ѹ�쳣û�з������ź�
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BUS_VOLTAGE;
					SlaveInfo.Info[i].V_BeforeFire = 0xFFFE;
					SlaveInfo.Info[i].I_BeforeFire = 0xFFFE;					
				}
				else
				{//�ӻ�û���յ�������
					SlaveInfo.Info[i].ErrorBit |= SLAVE_ERROR_BIT_BLAST_CMD;
					SlaveInfo.Info[i].V_BeforeFire = 0xFFFD;
					SlaveInfo.Info[i].I_BeforeFire = 0xFFFD;					
				}
			}
			SlaveInfo.Info[i].State = SLAVE_TRUE;
			OSSchedUnlock();			
		}
		else
		{//�дӻ�ͨ��ʧ��
			SlaveInfo.Info[i].V_BeforeFire = 0xFFFF;
			SlaveInfo.Info[i].I_BeforeFire = 0xFFFF;		
			SlaveInfo.Info[i].State = SLAVE_FALSE;
		}					
	}
	SetSlaveStartOperate(0xFF, 0, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 0, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 0, NULL);	
}

/**
  *************************************************************************************
  * @brief  ���ôӻ�״̬���¿�ʼ
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void AllSlaveStatusReset(void)
{
	SetSlaveStartOperate(0xFF, 11, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 11, NULL);
	OSTimeDly(10);
	SetSlaveStartOperate(0xFF, 11, NULL);	
}

/**
  *************************************************************************************
  * @brief  ���ôӻ�ͳһУ׼RTC
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2015��04��03��
  * @note   
  *************************************************************************************  
  */
void AllSlaveSetRtc(void)
{
	SetSlaveRtc(0xFF);
	OSTimeDly(10);
	SetSlaveRtc(0xFF);	
}

/**
  *************************************************************************************
  * @brief  ͨ��485����ӻ����׹�����
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2017��07��26��
  * @note   
  *************************************************************************************  
  */
u8 BackupImportDetDataFromSlave(void)
{
	u8 addr = 0;

	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485����Ϊ����״̬	
	OSTimeDly(CONNECT_RS485_TIME+100);//�ӳ��㹻ʱ�䣬�ôӻ�ȷ������
	
	//���ݱ���ʱֻ�ܽ�һ���豸����ȡ��Ӧ��ͨ�ŵ�ַ
	if(GetSlaveAddr(&addr) == SLAVE_FALSE)
	{
		return 0x01;//û���ҵ��ӻ����п���û�ӣ����߽��˶��
	}
	
	if(GetSlaveVerison(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	if(FormatIsSameForDetInfo(SlaveInfo.Info[addr].Version) == 0)
	{
		return 2;//�豸���Ͳ���ͬ
	}
	
	if(GetSlaveTestInfo(addr, 0) == SLAVE_FALSE)
	{//�дӻ�ͨ��ʧ��
		return 0xFF;
	}			
	
	if(SlaveInfo.Info[addr].RegCount == 0)
	{
		return 3;//û��ע���׹�
	}
	if(SlaveInfo.Info[addr].Type != Det.Type)
	{
		return 4;//�׹����Ͳ�ͬ
	}

	if (GetSlaveDetInfo(addr) != SLAVE_TRUE)
	{
		return 0xFF;
	}
	GetDetNum();
	if(ERROR == SaveAllDetInfo(0))
	{
		return 5;//��������ʧ��
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  ͨ��485�����ƴ��豸ɾ���׹�����
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2017��07��27��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDeleteDetInfo(u8 Index, u8* Result)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2B;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);
        
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2B) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							*Result = RS485TempBuffer.uch_buf[5];
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����ƴ��豸ɾ���׹�����
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2017��07��27��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDetHoleNumber(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x30;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�	

		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], 
				"%u\r%u\r", 0, DET_MAX_SIZE);	
					
		memcpy(&RS485Data.uch_buf[RS485Data.bytenum], &Det.HoleNumber[0], DET_MAX_SIZE);
		RS485Data.bytenum += DET_MAX_SIZE;	
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x30) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����ƴ��豸ɾ���׹�����
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2017��07��27��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveDetInfo(u8 Index, u16 LocalIndex, u16 SlaveIndex, u8 Number)
{
	u32 c_time;
	u8 U8Value, Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x2C;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
		RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], "%d\r", SlaveIndex);
		U8Value = 0;
		while(U8Value < Number)
		{
			//�׹�����\�׺�\Ƶ��\����\״̬��
			RS485Data.bytenum += sprintf((char*)&RS485Data.uch_buf[RS485Data.bytenum], 
				"%02X%02X%02X%02X%02X%02X%02X%02X\r%u\r%u\r%u\r%u\r%u\r", 
				Det.Info[LocalIndex + U8Value].Serl.U8Buffer[0], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[1], 
                Det.Info[LocalIndex + U8Value].Serl.U8Buffer[2], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[3], 
				Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[0], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[1], 
                Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[2], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[3],
				Det.Info[LocalIndex + U8Value].Hole.Name, Det.Info[LocalIndex + U8Value].Hole.Inside,
				Det.Info[LocalIndex + U8Value].Freq.U32Value, Det.Info[LocalIndex + U8Value].Delay,
				Det.Info[LocalIndex + U8Value].Err);
				
			U8Value++;
		}		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x2C) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ͨ��485�����ƴ��豸ɾ���׹�����
  * @param  Index��Ҫ�����豸����
  * @retval ���
  * @author ZH
  * @date   2017��07��27��
  * @note   
  *************************************************************************************  
  */
SLAVE_STATE SetSlaveSaveDetInfo(u8 Index)
{
	u32 c_time;
	u8 Retry = 5;
	
	while(Retry--)
	{
		RS485Data.status = ENCODING;
		RS485Data.bytenum = 0;
		RS485Data.uch_buf[RS485Data.bytenum++] = GetAddrNameFromIndex(Index);//Э���ַ
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x02;//��������ֽ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0x31;//��������ֽ�
			
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		RS485Data.uch_buf[RS485Data.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�			
		
		Pack(&RS485Data, &RS485TempBuffer);//���ݴ��
		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		RS485SendData(RS485TempBuffer.uch_buf, RS485TempBuffer.bytenum);

		RS485Data.bytenum = 0;
		RS485Data.status = RECIEVING;
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+1500/(1000/OS_TICKS_PER_SEC)))
		{
			if(RS485Data.status == RECIEVED)
			{
				SET_RS485_SEND();//485����Ϊ����״̬
				if(Unpack(&RS485Data, &RS485TempBuffer) != ERROR)
				{					
					if (RS485TempBuffer.bytenum == 7)
					{
						if ((RS485TempBuffer.uch_buf[0] == GetAddrNameFromIndex(Index)) && 
							(RS485TempBuffer.uch_buf[1] == 0x02)  && 
							(RS485TempBuffer.uch_buf[2] == 0x31) && 
							(RS485TempBuffer.uch_buf[3] == 0xFF) && 
							(RS485TempBuffer.uch_buf[4] == 0xFF) && 
							(RS485TempBuffer.uch_buf[5] == '1') &&
							(RS485TempBuffer.uch_buf[6] == 0x0D))
						{
							return SLAVE_TRUE;						
						}			
					}
					return SLAVE_FALSE;						
				}
				break;
			}
			OSTimeDly(2);
		}	
	}
	SET_RS485_SEND();//485����Ϊ����״̬
	return SLAVE_FALSE;	
}
/**
  *************************************************************************************
  * @brief  ͨ��485���׹����ݵ������ӻ�
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2017��07��26��
  * @note   
  *************************************************************************************  
  */
u8 BackupExportDetDataToSlave(void)
{
	u8 addr = 0;
	u8 U8Value, Result;
	u16 StartIndex = 0;
	
	SlaveInitState();
	InitSlaveDetNumber();
	
	SET_RS485_SEND();//485����Ϊ����״̬	
	OSTimeDly(CONNECT_RS485_TIME+100);//�ӳ��㹻ʱ�䣬�ôӻ�ȷ������	

	//���ݱ���ʱֻ�ܽ�һ���豸
	if(GetSlaveAddr(&addr) == SLAVE_FALSE)
	{
		return 1;//û���ҵ��ӻ����п���û�ӣ����߽��˶���ӻ�
	}
	
	if(GetSlaveVerison(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	if(FormatIsSameForDetInfo(SlaveInfo.Info[addr].Version) == 0)
	{
		return 2;//�豸���Ͳ���ͬ
	}
	
	if(GetSlaveTestInfo(addr, 0) == SLAVE_FALSE)
	{//�дӻ�ͨ��ʧ��
		return 0xFF;
	}			
	
	if(SlaveInfo.Info[addr].Type != Det.Type)
	{
		return 3;//�׹����Ͳ�ͬ
	}
	//���ƴӻ�ɾ���׹�
	if(SetSlaveDeleteDetInfo(addr, &Result) == SLAVE_FALSE)
	{
		return 0xFF;			
	}
	if(Result != '1')
	{
		return 4;//�ӻ�ɾ������ʧ��
	}
	
	//���������׹���
	if(SetSlaveDetHoleNumber(addr) == SLAVE_FALSE)
	{
		return 0xFF;
	}
	
	//�����׹����뼰��������
	U8Value =  Det.RegCount/10;
	while(U8Value--)
	{
		if(SetSlaveDetInfo(addr, StartIndex, StartIndex, 10) == SLAVE_FALSE)//ÿ��10���׹�
		{
			return 0xFF;
		}
		StartIndex += 10;
	}
	if(Det.RegCount%10)
	{
		if(SetSlaveDetInfo(addr, StartIndex, StartIndex, Det.RegCount%10) == SLAVE_FALSE)
		{
			return 0xFF;
		}	
	}			
	//���ƴӻ���������
	if(SetSlaveSaveDetInfo(addr) == SLAVE_FALSE)
	{
		return 0xFF;				
	}
	return 0;		
}



