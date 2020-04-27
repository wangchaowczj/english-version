#include "includes.h"

static STR_COMM DkeySendBuffer, DkeyTempBuffer;

//==================================================================================================
//| �������� | GetSecondTimeFromYear2000 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���㵱ǰʱ�䵽2000��1��1��0ʱ0��0�������
//|----------|--------------------------------------------------------------------------------------
//| ������� | year  ��ǰ���-2000��Ҫ�����2000��
//|          | month ��ǰ�·�
//|          | day   ��ǰ��
//|          | hour  ��ǰʱ
//|          | min   ��ǰ��
//|          | sec   ��ǰ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ERROR   �����������
//|          | SUCCESS ����ɹ�
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-04-08 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u32 GetSecondTimeFromYear2000(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
	u8 i;
	u32 sec_value = 0;
	const u8 month_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//������ÿ������

	/*���㵱ǰ���1��1��0ʱ0��0�뵽2000��1��1��0ʱ0��0�������*/
	for(i = 0; i < year; i++)
	{
		if(((0 == ((i+2000)%4)) && (0 != ((i+2000)%100))) || (0 == ((i+2000)%400)))
		{//����
			sec_value += 366*24*60*60;
		}
		else
		{
			sec_value += 365*24*60*60;
		}		
	}
	
	/*���㵱ǰ���month��1��0ʱ0��0�뵽1��1��0ʱ0��0�������*/
	for(i = 1; i < month; i++)
	{
		if(2 == i)
		{
			if(((0 == ((year+2000)%4)) && (0 != ((year+2000)%100))) || (0 == ((year+2000)%400)))
			{//����
				sec_value += 86400;//1�죬60*60*24
			}			
		}		
		sec_value += 60*60*24*month_day[i];//��		
	}
	
	/*���㵱ǰ����day��0ʱ0��0�뵽1��0ʱ0��0�������*/
	sec_value += (day-1)*24*60*60;
	
	/*���㵱ǰ����day��hourʱ0��0�뵽0ʱ0��0�������*/
	sec_value += hour*60*60;
	
	/*���㵱ǰ������hourʱmin��0�뵽hourʱ0��0�������*/
	sec_value += min*60;	

	/*���㵱ǰ������hourʱmin��sec�뵽hourʱmin��0�������*/
	sec_value += sec;
	
	return sec_value;
}
//==================================================================================================
//| �������� | GetTimeStringSecondTime 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���������2��ʱ���ַ�������ʱ���
//|----------|--------------------------------------------------------------------------------------
//| ������� | old_time ��ʱ���ַ���(ʱ�����)����ʽ��"20131020134221"
//|          | new_time ��ʱ���ַ���(ʱ�����)
//|          | value    ����ʱ����λΪ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ERROR   �����������
//|          | SUCCESS ����ɹ�
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-04-08 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus GetTimeStringSecondTime(u8* old_time, u8* new_time, u32* value)
{
//	const u8 month_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//�������������
	u16 old_year, new_year;
	u8  old_month, new_month, old_day, new_day, old_hour, new_hour, old_min, new_min, old_sec, new_sec;
	u32 old_sec_value = 0, new_sec_value = 0;
	
	
	old_year = (*old_time-0x30)*1000 + (*(old_time+1)-0x30)*100 + (*(old_time+2)-0x30)*10 + (*(old_time+3)-0x30);
	new_year = (*new_time-0x30)*1000 + (*(new_time+1)-0x30)*100 + (*(new_time+2)-0x30)*10 + (*(new_time+3)-0x30);
	if((old_year < 2000) || (old_year > 2099) || (new_year < 2000) || (new_year > 2099))
	{
		return ERROR;
	}
	
	old_month = (*(old_time+4)-0x30)*10 + (*(old_time+5)-0x30);
	new_month = (*(new_time+4)-0x30)*10 + (*(new_time+5)-0x30);
	if((old_month < 1) || (old_month > 12))
	{
		return ERROR;
	}
	if((new_month < 1) || (new_month > 12))
	{
		return ERROR;
	}
	
	old_day = (*(old_time+6)-0x30)*10 + (*(old_time+7)-0x30);
	new_day = (*(new_time+6)-0x30)*10 + (*(new_time+7)-0x30);
	if((old_day < 1) || (old_day > 31))//ֻ�����жϣ����������ж�
	{
		return ERROR;
	}
	if((new_day < 1) || (new_day > 31))//ֻ�����жϣ����������ж�
	{
		return ERROR;
	}	
	
	old_hour = (*(old_time+8)-0x30)*10 + (*(old_time+9)-0x30);
	new_hour = (*(new_time+8)-0x30)*10 + (*(new_time+9)-0x30);
	if((old_hour > 23) || (new_hour > 23))
	{
		return ERROR;
	}
	
	old_min = (*(old_time+10)-0x30)*10 + (*(old_time+11)-0x30);
	new_min = (*(new_time+10)-0x30)*10 + (*(new_time+11)-0x30);	
	if((old_min > 59) || (new_min > 59))
	{
		return ERROR;
	}
	
	old_sec = (*(old_time+12)-0x30)*10 + (*(old_time+13)-0x30);
	new_sec = (*(new_time+12)-0x30)*10 + (*(new_time+13)-0x30);		
	if((old_sec > 59) || (new_sec > 59))
	{
		return ERROR;
	}
	
	old_sec_value = GetSecondTimeFromYear2000(old_year-2000, old_month, old_day, old_hour, old_min, old_sec);
	new_sec_value = GetSecondTimeFromYear2000(new_year-2000, new_month, new_day, new_hour, new_min, new_sec);
	if(new_sec_value < old_sec_value)
	{
		return ERROR;
	}
	*value = (new_sec_value - old_sec_value) ;

	return SUCCESS;		
}

/**
  *************************************************************************************
  * @brief  ����Կ���ַ���(ͨ��)
  * @param  Cmd  ����
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @param  TimerOut ��ʱʱ�䣬MSΪ��λ 
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyGeneralString(u16 Cmd, u8* StrPtr, u8 MaxBytes, u16 TimerOut)
{
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = Cmd>>8;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = Cmd&0xFF;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);

		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+TimerOut/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if((DkeyTempBuffer.bytenum > 5) && (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(MaxBytes >= (DkeyTempBuffer.bytenum-6))//��߻���1��0x0D,����Ҫ���1���ֽ�
							{
								if(StrPtr != NULL)
								{
									memset(StrPtr, 0, MaxBytes);
									memcpy(StrPtr, &DkeyTempBuffer.uch_buf[5], DkeyTempBuffer.bytenum-6);								
								}
								return SUCCESS;
							}						
						}			
					}
				}			
			}
			OSTimeDly(2);
		}					
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ��Կ����У��
  * @param  �� 
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyGeneralVerify(u32 TimeOut)
{
	u32 c_time;
	
	Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
	HidData.bytenum = 0;
	HidData.status = RECIEVING;
	usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
	
	c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
	while(OSTimeGet() < (c_time+TimeOut/(1000/OS_TICKS_PER_SEC)))
	{
		if(HidData.status == RECIEVED)
		{
			if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
			{
				if ((DkeyTempBuffer.bytenum == 7) && (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
				{
					if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
						(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
					{
						return DkeyTempBuffer.uch_buf[5];						
					}			
				}
			}
			return 0x01;
		}
		OSTimeDly(2);
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  ����Կ������汾
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��   
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeySoftwareVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0100, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����ԿID
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyMac(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0101, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����ԿӲ���汾
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyHWVersion(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0103, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����Կ��Ȩ��
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyLicense(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x012E, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����Կ�����ϴ�����
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyUploadType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x011C, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����Կ�����ϴ�����
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyDetVerifyType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0116, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����ԿԶ����֤��ʶ
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyArgumentType(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0118, StrPtr, MaxBytes, 200);
}
/**
  *************************************************************************************
  * @brief  ����Կ��Ȩ��
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyCtrlBit(u8* StrPtr, u8 MaxBytes)
{
	return GetDkeyGeneralString(0x0124, StrPtr, MaxBytes, 200);
}

/**
  *************************************************************************************
  * @brief  ����Կ��Ȩ��
  * @param  StrPtr ���ݻ������׵�ַ
  * @param  MaxBytes ���ݻ��峤��  
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyServerAddr(u32* Ip, u16* Port)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2D;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator(0x0D,1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;
							}							
							*Ip = U32Value;
							
							if(ERROR == GetDecValueFromSeparator(0x0D,2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return ERROR;
							}							
							*Port = U32Value;
							return SUCCESS;						
						}			
					}
					return ERROR;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ����Կ����Ϣ
  * @param  ��
  * @retval ���
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
ErrorStatus GetDkeyInfo(void)
{
	ErrorStatus Result;
	u8 Temp[50];
	return SUCCESS;
	//����Կ����汾
	Result = GetDkeySoftwareVersion(&BlastInfo.DkeyInfo.Version[0],sizeof(BlastInfo.DkeyInfo.Version));
	if(ERROR == Result)
	{
		return ERROR;
	}
	//����ԿID
	Result = GetDkeyMac(&BlastInfo.DkeyInfo.Mac[0],sizeof(BlastInfo.DkeyInfo.Mac));
	if(ERROR == Result)
	{
		return ERROR;
	}
	//����ԿӲ���汾
	Result = GetDkeyHWVersion(&BlastInfo.DkeyInfo.HwVersion[0],sizeof(BlastInfo.DkeyInfo.HwVersion));
	if(ERROR == Result)
	{
		return ERROR;
	}
	
	//����Կ��ȨID
	Result = GetDkeyLicense(&BlastInfo.DkeyInfo.License[0],sizeof(BlastInfo.DkeyInfo.License));	
	if(ERROR == Result)
	{
		return ERROR;
	}
	
	//����������ַ
	Result = GetDkeyServerAddr(&BlastInfo.DkeyInfo.Server.Ip, &BlastInfo.DkeyInfo.Server.Port);
	if(ERROR == Result)
	{
		return ERROR;
	}

	//����Կ�����ϴ���ʽ
	Result = GetDkeyUploadType(&Temp[0], 2);//2�ֽ�����Ϊ����һ���ַ���������
	if(ERROR == Result)
	{
		return ERROR;
	}
	BlastInfo.DkeyInfo.UploadType = Temp[0];

	//����Կ�׹���Ȩ��ʽ
	Result = GetDkeyDetVerifyType(&Temp[0], 2);//2�ֽ�����Ϊ����һ���ַ���������
	if(ERROR == Result)
	{
		return ERROR;
	}					
	BlastInfo.DkeyInfo.DetVerifyType = Temp[0];
	
	//����ԿԶ����֤��ʶ
	Result = GetDkeyArgumentType(&Temp[0], 2);//2�ֽ�����Ϊ����һ���ַ���������
	if(ERROR == Result)
	{
		return ERROR;
	}
	BlastInfo.DkeyInfo.ArgumentType = Temp[0];
	
	//����Կ���ݱ��������ֽ�
	Result = GetDkeyCtrlBit(&Temp[0], 50);
	if(ERROR == Result)
	{
		return ERROR;
	}
	sscanf((char*)Temp,"%lld",&BlastInfo.DkeyInfo.CtrlBit);
	
	if ((BlastInfo.DkeyInfo.Server.Ip != 0) && (BlastInfo.DkeyInfo.Server.Port != 0))
	{
		if ((BlastInfo.DkeyInfo.Server.Ip != device_info.Server.Ip) || 
			(BlastInfo.DkeyInfo.Server.Port != device_info.Server.Port))
		{
			device_info.Server = BlastInfo.DkeyInfo.Server;
			Temp[0] = device_info.Server.Ip>>24;
			Temp[1] = device_info.Server.Ip>>16;
			Temp[2] = device_info.Server.Ip>>8;
			Temp[3] = device_info.Server.Ip;
			Temp[4] = device_info.Server.Port>>8;
			Temp[5] = device_info.Server.Port;
			CreateNewFileData(INTERNET_PARAM_FILE_PATH, SAFE_MONITOR_INTERNET_PARAM_FILE_PATH, (u8*)&Temp[0], 6);							
		}
	}	
	return SUCCESS;		
}

/**
  *************************************************************************************
  * @brief  �û���������֤
  * @param  UserName �û���
  * @param  Password ����
  * @param  State �û�״̬���������ʱ��Ч:
  *         ��4λ��ʾ��ǰ�����Դ�������4λ��ʾ�������Դ���(ȫΪ1ʱ��ʾ��������������)
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ�� 
  *       @arg @b ��2����ʾ�������
  *       @arg @b ��3�����û��Ѿ�����
  *       @arg @b ��4��û���ҵ��û�   
  *       @arg @b ��5����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyUserInfoVerify(u8* UserName, u8* Password, u8* State)
{
	u8 Result = 0;
	u32 U32Value;
	u32 c_time;
	return '1';
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x1F;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r", UserName, Password);
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							if ((U32Value < '1') || (U32Value > '5'))
							{
								return 0x01;
							}
							Result = (u8)U32Value;
							if(ERROR == GetDecValueFromSeparator('\r', 2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							*State = (u8)U32Value;

							return Result;						
						}			
					}
					return 0x01;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  �޸��û�������
  * @param  UserName �û���
  * @param  Password ����
  * @param  State �û�״̬���������ʱ��Ч  
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ�޸�ͨ�� 
  *       @arg @b ��2����ʾ�������
  *       @arg @b ��3�����û��Ѿ�����
  *       @arg @b ��4��û���ҵ��û�   
  *       @arg @b ��5����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyModifyUserInfo(u8* UserName, u8* OldPassword, u8* NewPassword, u8* State)
{
	u8 Result = 0;
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2F;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r%s\r", UserName, OldPassword, NewPassword);
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if(DkeyTempBuffer.bytenum >= 9)
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							if ((U32Value < '1') || (U32Value > '5'))
							{
								return 0x01;
							}
							Result = (u8)U32Value;
							if(ERROR == GetDecValueFromSeparator('\r', 2, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 0x01;
							}
							*State = (u8)U32Value;

							return Result;						
						}			
					}
					return 0x01;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0x00;	
}
/**
  *************************************************************************************
  * @brief  �𱬵ص㾭γ����֤
  * @param  Longitude ����"11620.1092E"
  * @param  Latitude γ��"3957.7800N"
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ��
  *       @arg @b ��2����ʾ��֤��ͨ��   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyBlastAreaVerify(u8* Longitude, u8* Latitude)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x20;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r%s\r", Longitude, Latitude);
			
		return DkeyGeneralVerify(500);					
	}
	return 0x00;	
}
/**
  *************************************************************************************
  * @brief  ��ʱ����֤
  * @param  Time ʱ���ַ���"20150205152100"
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ��
  *       @arg @b ��2����ʾ��֤��ͨ��   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyBlastTimeVerify(u8* Time)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x21;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%s\r", Time);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  GPS��ʱ��֤
  * @param  Time ʱ���ַ���"20150205152100"
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ��
  *       @arg @b ��2����ʾ��֤��ͨ��   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */ 
u8 DkeyGpsDelayVerify(u16 GpsDelayHour)
{	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x22;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%u\r", GpsDelayHour);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;	
}

/**
  *************************************************************************************
  * @brief  �׹�������֤
  * @param  DetInfo �׹�������Ϣ
  * @param  Bytes �׹�������Ϣ����  
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ��
  *       @arg @b ��2����ʾ��֤��ͨ��   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyDetInfoVerify(u8* DetInfo, u8 Bytes)
{
	u8 i;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x27;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		for(i = 0; i < Bytes; i++)
		{
			DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = *(DetInfo+i);
		}		
		return DkeyGeneralVerify(10000);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  ����Կ�е�ʣ���׹���
  * @param  DetCount ʣ���׹��� 
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ�ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetCount(u16* DetCount)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x25;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
			
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if ((DkeyTempBuffer.bytenum >= 9)|| (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 1;
							}
							*DetCount = (u16)U32Value;
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}
/**
  *************************************************************************************
  * @brief  ������Կ�е�ʣ���׹���
  * @param  DetNumber ����ʹ�õ�����
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ���³ɹ�
  *       @arg @b ��2����ʾ�����׹�������   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��  
  *       @arg @b ��4����Կ�쳣
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyUpdateDetCount(u16 DetNumber)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x26;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"%u\r", DetNumber);
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  ����Ϣ��֤
  * @param  DetErrIndex �׹�������֤���ַ���Ȩ�׹�ʱ��Ӧ������ֵ
  * @retval ���  
  *       @arg @b 0 ��������֤ͨ��
  *       @arg @b 1 ��ʱ��Ƿ�
  *       @arg @b 2 �𱬵ص�Ƿ�
  *       @arg @b 3 GPS��ʱ�Ƿ�
  *       @arg @b 4 ����������Կ��ʣ�෶Χ(������֤ʱ)
  *       @arg @b 5 ��δ��Ȩ���׹�(�׹�������֤ʱ)
  *       @arg @b 6 �дӻ�ͨ���쳣
  *       @arg @b 7 �дӻ��׹����ʹ���  
  *       @arg @b 0xFF �����쳣����ͨ���쳣
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyBlastInfoVerify(u16* DetErrIndex)
{
	u8 Temp1[30],Temp2[30];
	RTC_Typedef RTC_Temp;
	u8 Result = 0, SlaveIndex = 0;
	u16 U16Value = 0, i;
	u32 U32Value = 0;
	
	//��ʱ����֤
	OSSchedLock();
	RTC_Temp = RTC_Type;
	OSSchedUnlock();
	sprintf((char*)&Temp1[0],"%04d%02d%02d%02d%02d%02d",
							RTC_Temp.year, RTC_Temp.month, RTC_Temp.day, RTC_Temp.hour,
							RTC_Temp.minute, RTC_Temp.second);
							
	Result = DkeyBlastTimeVerify(Temp1);
	if(Result == '2')
	{//��֤��ͨ��
		return 1;//��ʱ��Ƿ�
	}
	else if(Result != '1')
	{//�����쳣
		return 0xFF;
	}
	
	//�𱬵ص㾭γ����֤
	//����,��ʽΪ"11620.1092,E", γ��,��ʽΪ"3957.7800,N"
	memset(Temp1, 0,sizeof(Temp1));
	memset(Temp2, 0,sizeof(Temp2));
	
	OSSchedLock();
	if((0x00 != Op_Data.Gps.GPS_Longitude[0])&&(0x00 != Op_Data.Gps.GPS_Latitude[0]))
	{//����GPS��λ�ɹ�
		memcpy(Temp1, Op_Data.Gps.GPS_Longitude, strlen((char*)Op_Data.Gps.GPS_Longitude));
		memcpy(Temp2, Op_Data.Gps.GPS_Latitude, strlen((char*)Op_Data.Gps.GPS_Latitude));
		OSSchedUnlock();
	}
//	else if(Op_Data.OldGps.GPS_state == GPS_SUCESS)
//	{//ʹ����һ�ε�GPS��Ϣ
//		memcpy(Temp1, Op_Data.OldGps.GPS_Longitude, sizeof(Op_Data.OldGps.GPS_Longitude));
//		memcpy(Temp2, Op_Data.OldGps.GPS_Latitude, sizeof(Op_Data.OldGps.GPS_Latitude));
//		OSSchedUnlock();
//	}
	else
	{
		OSSchedUnlock();
		return 2;//�𱬵ص�Ƿ�
	}	
	RemoveCharFromString(Temp1,',',strlen((const char*)Temp1));
	RemoveCharFromString(Temp2,',',strlen((const char*)Temp1));
	Result = DkeyBlastAreaVerify(Temp1, Temp2);
	if(Result == '2')
	{//��֤��ͨ��
		return 2;//�𱬵ص�Ƿ�
	}
	else if(Result != '1')
	{//�����쳣
		return 0xFF;
	}
	
	//GPS��ʱ��֤
	sprintf((char*)&Temp1[0],"%04d%02d%02d%02d%02d%02d",
							RTC_Temp.year, RTC_Temp.month, RTC_Temp.day, RTC_Temp.hour,
							RTC_Temp.minute, RTC_Temp.second);	
	if((0x00 != Op_Data.Gps.GPS_Longitude[0])&&(0x00 != Op_Data.Gps.GPS_Latitude[0]))
	{
		sprintf((char*)&Temp2[0], "%04d%02d%02d%02d%02d%02d", Op_Data.Gps.GPS_BJ_year, Op_Data.Gps.GPS_BJmonth,
			Op_Data.Gps.GPS_BJday, Op_Data.Gps.GPS_BJhour, Op_Data.Gps.GPS_BJminute, Op_Data.Gps.GPS_BJsecond);		
	}
	else
	{
		return 3;//GPS��ʱ�Ƿ�
	}

	if(ERROR == GetTimeStringSecondTime(Temp2, Temp1, &U32Value))
	{//��ǰʱ���GPSʱ�仹��
		return 3;//GPS��ʱ�Ƿ�
	}
	U32Value /=3600;//����ת��ΪСʱ
	if(U32Value > 0xFFFF)
	{//GPS��ʱСʱ�������ܳ���65535
		return 3;//GPS��ʱ�Ƿ�
	}
	Result = DkeyGpsDelayVerify(U32Value);
	if(Result == '2')
	{//��֤��ͨ��
		return 3;//GPS��ʱ�Ƿ�
	}
	else if(Result != '1')
	{//�����쳣
		return 0xFF;
	}	
	
	//���ӻ�ģʽʱ��������Ҫ��ǰ�����ӻ����׹�����
	if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
	{
		GetAllSlaveInfo(1);
		GetSlaveDetNumber();
		if (GetSlaveStateCount(SLAVE_TRUE) != SlaveInfo.Number)
		{
			return 0x06;//
		}
		else if(GetSlaveIndexFromErrorBit(SLAVE_ERROR_BIT_DET_TYPE) != 0xFF)
		{
			return 0x07;
		}
	}
	
	
	if(BlastInfo.DkeyInfo.DetVerifyType == '1')
	{//������֤
		Result = DkeyReadDetCount(&U16Value);
		if(Result != '1')
		{
			return 0xFF;
		}
		if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
		{
			if(U16Value < SlaveInfo.RegCount)
			{
				return 4;//����������Կ��ʣ�෶Χ
			}				
		}
		else
		{
			if(U16Value < Det.RegCount)
			{
				return 4;//����������Կ��ʣ�෶Χ
			}		
		}
	}
	else if(BlastInfo.DkeyInfo.DetVerifyType == '2')
	{//�׹�������֤
		SlaveIndex = 0x00;
		while(SlaveIndex != 0xFF)
		{
			//���ӻ�ģʽʱ���߶�����֤
			if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				SlaveInfo.Info[SlaveIndex].State = GetSlaveDetInfo(SlaveIndex);
				if(SlaveInfo.Info[SlaveIndex].State == SLAVE_FALSE)
				{
					return 0x06;//�ӻ��쳣
				}
			}
			
			AllRegDetRankBaseOnDetInfo();
			for(i=0; i<Det.RegCount;i++)
			{
				Temp1[0] = Det.Info[i].Serl_1;
				Temp1[1] = Det.Info[i].Serl_2;
				Temp1[2] = Det.Info[i].Serl_3;
				Temp1[3] = Det.Info[i].Factor;
				Temp1[4] = Det.Info[i].bat;
				Temp1[5] = Det.Info[i].year;
				Result = DkeyDetInfoVerify(&Temp1[0], 6);
				if(Result == '2')
				{//��֤��ͨ��
					*DetErrIndex = i;
					if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
					{
						SlaveInfo.Info[SlaveIndex].ErrorBit |= SLAVE_ERROR_BIT_DET_ACCREDIT;
					}
					return 5;//��δ��Ȩ���׹�
				}
				else if(Result != '1')
				{//�����쳣
					AllRegDetRankBaseOnHoleInfo();//�������׹ܰ��׺Ŵ�С������������
					return 0xFF;
				}			
			}
			if (DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				SlaveIndex++;
				if(SlaveIndex>= SlaveInfo.Number)
				{
					SlaveIndex = 0xFF;
				}
			}
			else
			{
				SlaveIndex = 0xFF;
				AllRegDetRankBaseOnHoleInfo();//�������׹ܰ��׺Ŵ�С������������			
			}	
		}

	}
		
	return 0x00;//�������֤ͨ��
}


/**
  *************************************************************************************
  * @brief  ����Կ�е��׹ܱ������ݴ�С
  * @param  Bytes ���ݴ�С
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��ȡ�ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUpDataBytes(u32* Bytes)
{
	u32 U32Value;
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2A;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = '1';
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x0D;
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if ((DkeyTempBuffer.bytenum >= 7)|| (DkeyTempBuffer.uch_buf[DkeyTempBuffer.bytenum-1] == 0x0D))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && (DkeyTempBuffer.uch_buf[4] == 0xFF))
						{
							if(ERROR == GetDecValueFromSeparator('\r', 1, &DkeyTempBuffer.uch_buf[5],DkeyTempBuffer.bytenum-5,&U32Value))
							{
								return 1;
							}
							*Bytes = (u16)U32Value;
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  ����Կ�е��׹ܱ�������(ֻ��1��)
  * @param  PackIndex ����
  * @param  Data �������ݵ��׵�ַ
  * @param  CurrentBytes ��ǰ�Ѿ���ȡ���ĳ���  
  * @param  NextBytes ���ζ�ȡ������   
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ���³ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUpData(u16 PackCount, u8* Data, u32 CurrentBytes, u16 NextBytes)
{
	u32 c_time;
	
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2A;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		DkeySendBuffer.bytenum += sprintf((char*)&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum],
			"2\r%u\r%lu\r%lu\r", PackCount, CurrentBytes, NextBytes);
		
		Pack(&DkeySendBuffer, &DkeyTempBuffer);//���ݴ��
		HidData.bytenum = 0;
		HidData.status = RECIEVING;
		usb_host_send(&DkeyTempBuffer.uch_buf[0], DkeyTempBuffer.bytenum);
		
		c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC)))
		{
			if(HidData.status == RECIEVED)
			{
				if(Unpack(&HidData, &DkeyTempBuffer) != ERROR)
				{
					if (DkeyTempBuffer.bytenum == (7+NextBytes))
					{
						if((DkeyTempBuffer.uch_buf[0] == DkeySendBuffer.uch_buf[0]) && (DkeyTempBuffer.uch_buf[1] == DkeySendBuffer.uch_buf[1])  && 
							(DkeyTempBuffer.uch_buf[2] == DkeySendBuffer.uch_buf[2]) && (DkeyTempBuffer.uch_buf[3] == 0xFF) && 
							(DkeyTempBuffer.uch_buf[4] == 0xFF) && (DkeyTempBuffer.uch_buf[5] == (PackCount>>8)) && 
							(DkeyTempBuffer.uch_buf[6] == (PackCount&0xFF)))
						{
							memcpy(Data, &DkeyTempBuffer.uch_buf[7], NextBytes);
							return '1';						
						}			
					}
					return 1;
				}			
			}
			OSTimeDly(2);
		}					
	}
	return 0;	
}

/**
  *************************************************************************************
  * @brief  ����Կ�е��׹ܱ�������
  * @param  ��  
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b 2 ��Կ��û�б�������
  *       @arg @b 3 �������Ͳ�����Ҫ��   
  *       @arg @b ��1����ʾ��ȡ�ɹ�
  *       @arg @b ��2����ʾ����ʧ�� 
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyReadDetInfoBackUp(void)
{
	u8 Result=0;
	u32 BackUpDataBytes = 0;
	u8 Data[20];
	u16 i;
	
	Result = DkeyReadDetInfoBackUpDataBytes(&BackUpDataBytes);//����Կ�е����ݳ���
	if(Result == '1')
	{
		if(BackUpDataBytes <= 10)
		{
			return 2;//û������
		}
		
		//ǰ10���ֽ����ڱ�ʾ���ݺ���
		//Data[0]��ʾ���ݸ�ʽ����,ĿǰΪ0
		//��������			
		Result = DkeyReadDetInfoBackUpData(0, Data, 0,10);
		if(Result == '1')
		{	
			if ((Data[0] == 0) && 
				(BackUpDataBytes == (16*DET_MAX_SIZE/*ÿ���׹�16�ֽ�*/ + 10/*������ǰ10�ֽ�*/ + DET_MAX_SIZE/*�����׹���*/)))
			{//���ݸ�ʽ����Ҫ�� 
				memset((char*)&Det.Info[0], 0, sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				
				//���׹�����
				for(i=0;i< DET_MAX_SIZE;i++)
				{
					Result = DkeyReadDetInfoBackUpData(i, Data, 10+i*16, 16);
					if(Result == '1')
					{
						Det.Info[i].Serl_1 = Data[0];
						Det.Info[i].Serl_2 = Data[1];
						Det.Info[i].Serl_3 = Data[2];
						Det.Info[i].Factor = Data[3];
						Det.Info[i].bat = Data[4];
						Det.Info[i].year = Data[5];
						Det.Info[i].freq = ((u32)Data[6]<<24)+ ((u32)Data[7]<<16) + ((u32)Data[8]<<8) + ((u32)Data[9]&0xFF);
						Det.Info[i].delay = ((u16)Data[10]<<8) + ((u16)Data[11]&0xFF);					
						Det.Info[i].Hole.Name = ((u16)Data[12]<<8) + ((u16)Data[13]&0xFF);	
						Det.Info[i].Hole.Inside = Data[14];
						Det.Info[i].err = Data[15];	
						
						if(ERROR == CheckDetType(Det.Info[i].bat, Det.Info[i].year))
						{
							memset((char*)&Det.Info[0],0,sizeof(Det.Info));
							memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
							Det.ErrorCount = 0;
							Det.RegCount = 0;
							return 3;//���Ͳ�����Ҫ��
						}
						if(Det.Info[i].delay > Det.DlyMax)
						{
							Det.Info[i].delay =  Det.DlyMax;
						}
					}
					else
					{
						memset((char*)&Det.Info[0],0,sizeof(Det.Info));
						memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
						Det.ErrorCount = 0;
						Det.RegCount = 0;
						return Result;
					}
				}
				//��ÿ���׵Ŀ����׹���
				Result = DkeyReadDetInfoBackUpData(0, &Det.HoleNumber[0], 10+DET_MAX_SIZE*16, DET_MAX_SIZE);
				if(Result != '1')
				{
					memset((char*)&Det.Info[0],0,sizeof(Det.Info));
					memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
					Det.ErrorCount = 0;
					Det.RegCount = 0;
					return Result;				
				}
				
				if(ERROR == SaveAllDetInfo())
				{
					return '2';
				}
				else
				{
					return '1';				
				}
			}
			return 2;//��ʽ������Ҫ��
		}
	}	
	
	return Result;
}

/**
  *************************************************************************************
  * @brief  д����Կ���׹ܱ�������
  * @param  PackIndex ����
  * @param  Data ����
  * @param  Bytes ���ݳ���
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ��Կ���صĸ�ʽ����
  *       @arg @b ��1����ʾ��֤ͨ��
  *       @arg @b ��2����ʾ��֤��ͨ��   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��    
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyDetInfoBackUpData(u16 PackIndex, u8* Data, u16 Bytes)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x29;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex>>8;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex&0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		if(Bytes > 0)
		{
			memcpy(&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum], Data, Bytes);
			DkeySendBuffer.bytenum += Bytes;	
		}
			
		return DkeyGeneralVerify(500);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  д����Կ���׹ܱ�������
  * @param  ��
  * @retval ���  
  *       @arg @b ERROR ʧ��
  *       @arg @b SUCCESS �ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyDetInfoBackUp(void)
{
	u8 Data[230], Bytes = 10;
	u8 Result = 0;
	u16 PackIndex = 0,i;
	
	//ǰ10���ֽ����ڱ�ʾ���ݺ���
	//Data[0]��ʾ���ݸ�ʽ����,ĿǰΪ0
	//��������
	memset(Data, 0x00,sizeof(Data));
	Result = DkeyDetInfoBackUpData(PackIndex++,Data, Bytes);
	if(Result == '1')
	{
		//�����׹�����
		Bytes = 0;
		for(i=0;i<DET_MAX_SIZE;i++)
		{			
			Data[Bytes++] = Det.Info[i].Serl_1;
			Data[Bytes++] = Det.Info[i].Serl_2;
			Data[Bytes++] = Det.Info[i].Serl_3;
			Data[Bytes++] = Det.Info[i].Factor;
			Data[Bytes++] = Det.Info[i].bat;
			Data[Bytes++] = Det.Info[i].year;
			Data[Bytes++] = Det.Info[i].freq>>24;
			Data[Bytes++] = Det.Info[i].freq>>16;
			Data[Bytes++] = Det.Info[i].freq>>8;
			Data[Bytes++] = Det.Info[i].freq&0xFF;
			Data[Bytes++] = Det.Info[i].delay>>8;
			Data[Bytes++] = Det.Info[i].delay&0xFF;
			
			Data[Bytes++] = Det.Info[i].Hole.Name>>8;
			Data[Bytes++] = Det.Info[i].Hole.Name;
			Data[Bytes++] = Det.Info[i].Hole.Inside;
			Data[Bytes++] = Det.Info[i].err;
			
			if ((Bytes > 200) || ((i+1) >= DET_MAX_SIZE))
			{
				Result = DkeyDetInfoBackUpData(PackIndex++,Data, Bytes);
				if(Result != '1')
				{
					return ERROR;
				}
				else
				{
					Bytes = 0;
				}
			}
		}
		//����ÿ���׵Ŀ����׹���
		Result = DkeyDetInfoBackUpData(PackIndex++, &Det.HoleNumber[0], DET_MAX_SIZE);
		if(Result != '1')
		{
			return ERROR;
		}		
		Result = DkeyDetInfoBackUpData(0xFFFF,NULL, 0);//������
		if(Result == '1')
		{
			return SUCCESS;
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  �����׹ܱ�������
  * @param  DetNumber ����ʹ�õ�����
  * @retval ���  
  *       @arg @b ERROR ʧ��
  *       @arg @b SUCCESS �ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyDeleteDetInfoBackUpData(void)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x2B;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = '3';
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x0D;
			
		if('1' == DkeyGeneralVerify(1000))
		{
			return SUCCESS;
		}
	}
	return ERROR;
}


/**
  *************************************************************************************
  * @brief  ���𱬼�¼���ݷ��͵�������Կ��
  * @param  PackIndex ����
  * @param  Data ����
  * @param  Bytes ���ݳ���
  * @retval ���  
  *       @arg @b 0 û������
  *       @arg @b 1 ����¼����
  *       @arg @b ��1����ʾ���³ɹ�
  *       @arg @b ��2����ʾ�����׹�������   
  *       @arg @b ��3����Կ���յ������ݲ�����Ҫ��  
  *       @arg @b ��4����Կ�쳣
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
u8 DkeyBlastRecorderBackUpData(u16 PackIndex, u8* Data, u16 Bytes)
{
	if (USB_SUCESS == usb_into_host())
	{
		DkeySendBuffer.bytenum = 0;
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x41;//Э���ַ
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x01;//��������ֽ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = 0x14;//��������ֽ�
			
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex>>8;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
		DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum++] = PackIndex&0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
		
		if(Bytes > 0)
		{
			memcpy(&DkeySendBuffer.uch_buf[DkeySendBuffer.bytenum], Data, Bytes);
			DkeySendBuffer.bytenum += Bytes;		
		}
			
		return DkeyGeneralVerify(2000);			
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  ���𱬼�¼���浽������Կ��
  * @param  DetNumber ����ʹ�õ�����
  * @retval ���  
  *       @arg @b ERROR ʧ��
  *       @arg @b SUCCESS �ɹ�
  * @author ZH
  * @date   2015��02��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DkeyBlastRecorderBackUp(void)
{
	FIL fdst;      /* file objects */
	u8 Buffer[256];	
	UINT bw;
	u16 PackIndex = 0;
	u8 Result = 0;

	FileSystemLock();
	
	sprintf((char*)Buffer, "%s/%02d%02d%02d%02d%02d%02d.txt", BLASTER_RECORD_FILE_PATH, BlastInfo.Time.year-2000, 
		BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);

	if(f_open(&fdst, (const char*)Buffer, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		f_lseek(&fdst,0);
		
		while(1)
		{
			if(FR_OK == f_read(&fdst, Buffer, 200, &bw))//ÿ��200�ֽ�
			{
				if(bw > 0)
				{
					Result = DkeyBlastRecorderBackUpData(PackIndex++,Buffer,bw);
					if(Result != '1')
					{
						FileSystemUnlock();
						WriteSystemLog("ͨ��ʧ��1!",strlen("ͨ��ʧ��1!"),STR_INFO);
						PackIndex = f_tell(&fdst);
						WriteSystemLog("  ",strlen("  "),STR_INFO);
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						PackIndex = Result;
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						if (USB_SUCESS == usb_into_host())
						{
							WriteSystemLog("��ǰ��Կ������!",strlen("��ǰ��Կ������!"),STR_INFO);
						}
						else
						{
							WriteSystemLog("��ǰ��Կû������!",strlen("��ǰ��Կû������!"),STR_INFO);
						}
						return ERROR;
					}
				}
				if(bw != 200)
				{
					Result = DkeyBlastRecorderBackUpData(0xFFFF,NULL,0);					
					if(Result == '1')
					{
						FileSystemUnlock();
						return SUCCESS;
					}
					else
					{
						FileSystemUnlock();
						WriteSystemLog("ͨ��ʧ��2!",strlen("ͨ��ʧ��2!"),STR_INFO);
						PackIndex = f_tell(&fdst);
						WriteSystemLog("  ",strlen("  "),STR_INFO);						
						PackIndex = Result;
						WriteSystemLog(&PackIndex,sizeof(PackIndex),U16_DATA);
						if (USB_SUCESS == usb_into_host())
						{
							WriteSystemLog("��ǰ��Կ������!",strlen("��ǰ��Կ������!"),STR_INFO);
						}
						else
						{
							WriteSystemLog("��ǰ��Կû������!",strlen("��ǰ��Կû������!"),STR_INFO);
						}						
						return ERROR;
					}
				}
			}
			else
			{
				FileSystemUnlock();
				WriteSystemLog("���ļ�ʧ��!",strlen("���ļ�ʧ��!"),STR_INFO);
				WriteSystemLog(&bw,sizeof(bw),U16_DATA);
				return ERROR;
			}			
		}
	}	
	FileSystemUnlock();
	WriteSystemLog("���ļ�ʧ��!",strlen("���ļ�ʧ��!"),STR_INFO);
	return ERROR;
}


