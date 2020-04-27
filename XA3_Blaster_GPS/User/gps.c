#include "includes.h"
#include <Math.h>

static GPS_INFO GPS_Info;
static u8 SatelliteCount = 0;//��������
GPS_BUFFER GPS_Buffer;

static  OS_EVENT   *GPSMutex;
u32 GPSPassTime;//���һ�ζ�λ�ɹ�ʱ��ʱ��
u32 GPSFailTime;//���һ�ζ�λʧ��ʱ��ʱ��
u8 GPSPassCount = 0;

static void GPSLock(void)
{
	INT8U  err;
	
	if(GPSMutex == 0)
	{
		GPSMutex = OSMutexCreate(RES_GPS_MUTEX_PRIO, &err);
	}
	
	OSMutexPend(GPSMutex,0,&err);
}
static void GPSUnlock(void)
{	
	OSMutexPost(GPSMutex);
}

/**
  *************************************************************************************
  * @brief  ����6����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
static void USART6_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//���ô���ʱ��
	RCC_AHB1PeriphClockCmd(GPS_COM_PORT_RCC, ENABLE);	//PBʱ��
	RCC_APB2PeriphClockCmd(GPS_COM_RCC, ENABLE);		//����6ʱ��

	//��������ӳ�䣬TX�ź�RX��
	GPIO_PinAFConfig(GPS_COM_PORT, GPS_COM_RX_PINSOURCE, GPIO_AF_USART6); 
	GPIO_PinAFConfig(GPS_COM_PORT, GPS_COM_TX_PINSOURCE, GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	//TX��������
	GPIO_InitStructure.GPIO_Pin = GPS_COM_TX_PIN;	
	GPIO_Init(GPS_COM_PORT, &GPIO_InitStructure);

	//RX��������
	GPIO_InitStructure.GPIO_Pin = GPS_COM_RX_PIN;
	GPIO_Init(GPS_COM_PORT, &GPIO_InitStructure);

	//��������
	USART_InitStructure.USART_BaudRate = GPS_COM_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

	//���ڳ�ʼ��
	USART_Init(GPS_COM, &USART_InitStructure);

	USART_ClearITPendingBit(GPS_COM, USART_IT_RXNE);
	USART_ITConfig(GPS_COM, USART_IT_RXNE, ENABLE);

	/***���ô����ж�***/
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(GPS_COM, DISABLE);	
}
/**
  *************************************************************************************
  * @brief  GPS��IO������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
static void GPSPortConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//LED״̬����
	RCC_AHB1PeriphClockCmd(GPS_PPS_PORT_RCC|GPS_OFF_PORT_RCC|GPS_RESET_PORT_RCC|GPS_PWR_EN_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPS_PPS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPS_PPS_PORT, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPS_OFF_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPS_OFF_PORT, &GPIO_InitStructure);

	GPS_OFF_ON();
	
	//GPS��Դ������������	
	GPIO_InitStructure.GPIO_Pin = GPS_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPS_RESET_PORT, &GPIO_InitStructure);
	GPS_RESET_H();	
	
	//GPS��Դ������������	
	GPIO_InitStructure.GPIO_Pin = GPS_PWR_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPS_PWR_EN_PORT, &GPIO_InitStructure);
	GPS_PWR_EN_OFF();		
}
/**
  *************************************************************************************
  * @brief  GPSģ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
void GPSConfig(void)
{
	USART6_Config();
	GPSPortConfig();
}
/**
  *************************************************************************************
  * @brief  ��GPSģ�鷢������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
void GPSSendData(u8* ptr, u16 bytes)
{
	u16 i;

	for(i=0; i < bytes ;i++)
	{
		USART_SendData(GPS_COM, *ptr++);
		while(USART_GetFlagStatus(GPS_COM, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(GPS_COM, USART_FLAG_TC) == RESET);
}

/**
  *************************************************************************************
  * @brief  ��ʼ����GPSģ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
void ReceiveGPSInfoStart(void)
{
    GPSLock();
    memset(&GPS_Info, 0, sizeof(GPS_INFO));
    GPSPassTime = 0;
    GPSFailTime = 0;
    GPSUnlock();
	SatelliteCount = 0;
	GPS_Buffer.Bytes = 0;
    GPS_Buffer.Status = GPS_RECEIVING;
    GPSPassCount = 0;
	GPS_PWR_EN_ON();	
	USART_Cmd(GPS_COM, ENABLE);
}


/**
  *************************************************************************************
  * @brief  ֹͣ����GPSģ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
void ReceiveGPSInfoStop(void)
{	
	GPS_PWR_EN_OFF();
	USART_Cmd(GPS_COM, DISABLE);
    SatelliteCount = 0;
	GPS_Buffer.Bytes = 0;
}
/**
  *************************************************************************************
  * @brief  GPSģ���Ƿ���������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   
  *************************************************************************************  
  */
u8 GPSIsWorking(void)
{
    return (GET_GPS_POWER_STATE() == 0)?1:0;
}
/**
  *************************************************************************************
  * @brief  ��λ�ɹ�����
  * @param  
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
u8 GetGPSPassCount(void)
{
    return GPSPassCount;
}
/**
  *************************************************************************************
  * @brief  ��λ�ɹ�����
  * @param  
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
void SetGPSPassCount(u8 Count)
{
    GPSPassCount = Count;
}
/**
  *************************************************************************************
  * @brief  ��ASCII��ת����16����
  * @param  
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
uint8_t Asc_Hex(uint8_t *p)
{
	uint8_t hex;
	uint8_t asc[2];

	asc[0] = *p;		//��ȡ16�������ݵ���λ
	asc[1] = *(p+1);

	//****�Ƚ�Сдת���ɴ�д****//
	if(asc[0]>='a')
	{
		asc[0] = asc[0] - 32;		//ת���ɴ�д
	}
	if(asc[1]>='a')
	{
		asc[1] = asc[1] - 32;
	}

	//****�ٽ�A��Fת��Ϊ��Ӧ��10����****//
	if(asc[0]>='A')
	{
		asc[0] = asc[0] - 55;		//ת����16����
	}
	if(asc[1]>='A')
	{
		asc[1] = asc[1] - 55;
	}

	//****���0��9ת��Ϊ��Ӧ��10����****//
	if(asc[0]>='0')
	{
	    asc[0] = asc[0] - 48;
	}
	if(asc[1]>='0')
	{
	    asc[1] = asc[1] - 48;
	}

	hex = (asc[0]<<4) + asc[1];		//��λ����ת����ɺ�"��λx16+��λ"�õ�����ת����ɵ�10��������

	return hex;
}
/**
  *************************************************************************************
  * @brief  ASCII��ת��������
  * @param  
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
uint8_t GpsAscDec(uint8_t *p)
{
	uint8_t dec = 0;
	uint8_t asc[2];

	asc[0] = *p;
	asc[1] = *(p+1);
	
	if ((asc[0] >= '0') && (asc[0] <= '9'))
	{
		dec = asc[0]-'0';
	}
	else
	{
		return 0;
	}
	if ((asc[1] >= '0') && (asc[1] <= '9'))
	{
		dec *= 10;
		dec += asc[1]-'0';
	}

	return dec;
}
/**
  *************************************************************************************
  * @brief  UTCʱ��ת���ɱ���ʱ��
  * @param  index: ��λ����
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
void Gps_UTC_to_BJ(GPS_INFO *Location)
{
	uint8_t temp_hour;

	temp_hour = Location->Hour;
	Location->Hour = ( temp_hour > 15 ) ? temp_hour-16 : temp_hour + 8;		//��UTCʱ������ϼ�8Сʱ

	//�����п�Խʱ�Ĵ���
	if(temp_hour>15)
	{
		Location->Day++ ;			//�ռ�1
		if(Location->Month == 2)		//���Ϊ2��
		{
			if(((Location->Year%4==0)&&(Location->Year%100!=0)) || (Location->Year%400==0))	//�������
			{
				if(Location->Day==30)
				{
					Location->Month++;
					Location->Day = 1;
				}
			}
			else					//ƽ�����
			{
				if(Location->Day==29)
				{
					Location->Month++;
					Location->Day = 1;
				}
			}
		}
		else if((Location->Month==1)||(Location->Month==3)||(Location->Month==5)||(Location->Month==7)|| \
				(Location->Month==8)||(Location->Month==10)||(Location->Month==12))	//����
		{
			if(Location->Day==32)
			{
				Location->Month++;
				Location->Day = 1;
			} 
		}
		else		//С��
		{
			if(Location->Day==31)
			{
				Location->Month++;
				Location->Day = 1;
			}
		}
		if(Location->Month==13)		//����
		{
			Location->Month = 1;
			Location->Year++;
		}
	}
}
//$GPRMC���ĸ�ʽ$GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
//$GNRMC���ĸ�ʽ$GNRMC,061149.00,A,3725.71601,N,11626.38965,E,0.284,,240719,,,A*63
//�ο�http://jingyan.baidu.com/article/6079ad0e81189a28ff86db8c.html
//https://wenku.baidu.com/view/a5fafe600066f5335a812174.html?tdsourcetag=s_pcqq_aiomsg&qq-pf-to=pcqq.c2c
ErrorStatus GPS_BufferVerify(void)
{
    u8 Verify1, Verify2;
    u16 i;
    
    if(GPS_Buffer.Bytes < 10)
    {
        return ERROR;
    }
    if ((GPS_Buffer.Buffer[0] != '$') || 
        (GPS_Buffer.Buffer[GPS_Buffer.Bytes-5] != '*') ||
        (GPS_Buffer.Buffer[GPS_Buffer.Bytes-2] != 0x0D) || (GPS_Buffer.Buffer[GPS_Buffer.Bytes-1] != 0x0A))
    {
        return ERROR;
    }
    Verify1 = GPS_Buffer.Buffer[1];
    for(i=2; i<GPS_Buffer.Bytes-5; i++)
    {
        Verify1 ^= GPS_Buffer.Buffer[i];
    }
    Verify2 = Asc_Hex((u8 *)(&GPS_Buffer.Buffer[GPS_Buffer.Bytes-4]));
    if(Verify1 == Verify2)
    {
        return SUCCESS;
    }
    return ERROR;
}

ErrorStatus RMC_FormatAnalyze(void)
{
    if(GPS_WAIT_ANALYZE != GPS_Buffer.Status)
    {
        return ERROR;
    }
    GPS_Buffer.Status = GPS_ANALYZING;
    
	if((GPS_Buffer.Bytes > 6) &&
        ('G' == GPS_Buffer.Buffer[1]) && 
        (('N' == GPS_Buffer.Buffer[2]) || ('P' == GPS_Buffer.Buffer[2])) && 
        ('R' == GPS_Buffer.Buffer[3]) && 
		('M' == GPS_Buffer.Buffer[4]) && ('C' == GPS_Buffer.Buffer[5]))
    {
        uint8 Split[12];
        u8 Count = 0;
        u8 i;
        
        for(i=0; i<GPS_Buffer.Bytes; i++)
        {
            if(GPS_Buffer.Buffer[i] == ',')
            {
                if(Count >= 12)
                {
                    GPS_Buffer.Bytes = 0;
                    GPS_Buffer.Status = GPS_RECEIVING;
                    return ERROR;           
                }
                Split[Count++] = i;       
            }
        }
        if(Count != 12)
        {
            GPS_Buffer.Bytes = 0;
            GPS_Buffer.Status = GPS_RECEIVING;
            return ERROR;
        }
        if(GPS_Buffer.Buffer[Split[1]+1] != 'A')//״̬��A=��λ��V=δ��λ
        {
            GPS_Buffer.Bytes = 0;
            GPS_Buffer.Status = GPS_RECEIVING;
            
            GPSLock();
            GPSFailTime = OSTimeGet();
            GPSUnlock();
            
            return ERROR;        
        }

        GPSLock();

        Count = Split[3]-Split[2]-1;
        if(Count >= sizeof(GPS_Info.Latitude))
        {
            Count = sizeof(GPS_Info.Latitude)-1;//���һλ���ڱ���γ��N����γ����S����γ��
        }
        memset(&GPS_Info.Latitude[0], 0, sizeof(GPS_Info.Latitude));
        memcpy(&GPS_Info.Latitude[0], &GPS_Buffer.Buffer[Split[2]+1], Count);//γ��
        GPS_Info.Latitude[Count] = GPS_Buffer.Buffer[Split[3]+1];//γ��N����γ����S����γ��

        Count = Split[5]-Split[4]-1;
        if(Count >= sizeof(GPS_Info.Longitude))
        {
            Count = sizeof(GPS_Info.Longitude)-1;//���һλ���ڱ��澭��E����������W��������
        }
        memset(&GPS_Info.Longitude[0], 0, sizeof(GPS_Info.Longitude));
        memcpy(&GPS_Info.Longitude[0], &GPS_Buffer.Buffer[Split[4]+1], Count);//����
        GPS_Info.Longitude[Count] = GPS_Buffer.Buffer[Split[5]+1];//����E����������W��������

        GPS_Info.Year = 2000+GpsAscDec(&GPS_Buffer.Buffer[Split[8]+5]);	//�꣬UTC���ڣ�DDMMYY��ʽ
        GPS_Info.Month = GpsAscDec(&GPS_Buffer.Buffer[Split[8]+3]);	//��
        GPS_Info.Day = GpsAscDec(&GPS_Buffer.Buffer[Split[8]+1]);		//��
        GPS_Info.Hour = GpsAscDec(&GPS_Buffer.Buffer[Split[0]+1]);	//ʱUTCʱ�䣬hhmmss.sss��ʽ
        GPS_Info.Minute = GpsAscDec(&GPS_Buffer.Buffer[Split[0]+3]);	//��
        GPS_Info.Second = GpsAscDec(&GPS_Buffer.Buffer[Split[0]+5]);	//��
        Gps_UTC_to_BJ(&GPS_Info);//��UTCʱ��ת�ɱ���ʱ��

        GPSPassTime = OSTimeGet();

        GPSUnlock();  
        if(GPSPassCount != 0xFF)
        {
            GPSPassCount++;
        }
        
        GPS_Buffer.Bytes = 0;
        GPS_Buffer.Status = GPS_RECEIVING;
        return SUCCESS;
    }
    GPS_Buffer.Bytes = 0;
    GPS_Buffer.Status = GPS_RECEIVING;
    return ERROR;    
}
/**
  *************************************************************************************
  * @brief  GPS���ݴ���
  * @param  ptr:���յ�������
  * @param  length ���յ������ݳ���
  * @retval ���������ݳ���
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
void GPSReceiveDataDeal(void)
{
    if(GPS_Buffer.Bytes == 0)
    {
        return;
    }
	if(GPS_Buffer.Buffer[0] != '$')
	{
		GPS_Buffer.Bytes = 0;
        return;
	}
	else if (GPS_Buffer.Buffer[GPS_Buffer.Bytes-1] != 0x0A)
	{
		return;
	}
    
    if(GPS_BufferVerify() == ERROR)
    {
		GPS_Buffer.Bytes = 0;
        return;        
    }
    
	if((GPS_Buffer.Bytes > 6) &&
        ('G' == GPS_Buffer.Buffer[1]) && ('P' == GPS_Buffer.Buffer[2]) && 
        ('G' == GPS_Buffer.Buffer[3]) && ('S' == GPS_Buffer.Buffer[4]) && ('V' == GPS_Buffer.Buffer[5]))
	{//������Ϣ�����浱ǰ������
		u8 Count = 0;
        u8 i;
        
		for(i=0; i<GPS_Buffer.Bytes; i++)
		{
			if(GPS_Buffer.Buffer[i] == ',')
			{
				Count++;
				if (Count == 3)
				{//��ʱ��i��ֵΪ��������������λ�á����������������
					SatelliteCount = GpsAscDec((uint8_t *)&GPS_Buffer.Buffer[i+1]);
                    break;
				}        
            }
        }
		GPS_Buffer.Bytes = 0;
        return;          
	}
	if((GPS_Buffer.Bytes > 6) &&
        ('G' == GPS_Buffer.Buffer[1]) && 
        (('N' == GPS_Buffer.Buffer[2]) || ('P' == GPS_Buffer.Buffer[2])) && 
        ('R' == GPS_Buffer.Buffer[3]) && 
		('M' == GPS_Buffer.Buffer[4]) && ('C' == GPS_Buffer.Buffer[5]))
	{//��λ��Ϣ
        GPS_Buffer.Status = GPS_WAIT_ANALYZE;
        return;
//        RMC_FormatAnalyze();//���ж��д�����ʱ�е㳤����̫��
	}
    GPS_Buffer.Bytes = 0;
}
/**
  *************************************************************************************
  * @brief  GPS���ڻص�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
void GPSUartCallback(void)
{
	//���ڽ�������
	if(USART_GetITStatus(GPS_COM, USART_IT_RXNE) != RESET)
	{
        u8 Temp;
        
        Temp = USART_ReceiveData(GPS_COM);
        if(GPS_RECEIVING == GPS_Buffer.Status)
        {
            if(GPS_Buffer.Bytes >= GPS_BUFFER_SIZE)
            {
                GPS_Buffer.Bytes = 0;
            }
            GPS_Buffer.Buffer[GPS_Buffer.Bytes++] = Temp;
            GPSReceiveDataDeal();        
        }
	}
}

/**
  *************************************************************************************
  * @brief  ��ȡ��ǰ�Ŀɼ���������
  * @param  ��
  * @retval ��������
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
u8 GetSatelliteCount(void)
{
    return SatelliteCount;
}

/**
  *************************************************************************************
  * @brief  ��ȡʵʱ��GPS��Ϣ
  * @param  ��
  * @retval GPS��Ϣ
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
u8 GetCurrentGPSInfo(GPS_INFO* Ptr)
{
    u8 IsPass;
    
    GPSLock();
    
    if(GPSPassTime > GPSFailTime)
    {
        IsPass = 1;
        if(Ptr != NULL)
        {
            memcpy(Ptr, &GPS_Info, sizeof(GPS_INFO));
        }
    }
    else
    {
        IsPass = 0;
        if(Ptr != NULL)
        {
            memset(Ptr, 0, sizeof(GPS_INFO));
        }
    }
    GPSUnlock();
    
    return IsPass;
}
/**
  *************************************************************************************
  * @brief  ��ȡ���һ�ζ�λ�ɹ���GPS��Ϣ
  * @param  ��
  * @retval GPS��Ϣ
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
u8 GetLastGPSInfo(GPS_INFO* Ptr)
{
    u8 IsPass;
    
    GPSLock();
    
    if(Ptr != NULL)
    {
        memcpy(Ptr, &GPS_Info, sizeof(GPS_INFO));    
    }

    IsPass = GPSInfoIsPass(&GPS_Info);
    GPSUnlock();
    
    return IsPass;
}

/**
  *************************************************************************************
  * @brief  ��λ�����Ƿ���Ч
  * @param  ��
  * @retval 0��Ч��1��Ч
  * @author ZH
  * @date   2019��7��31��
  * @note   ��
  *************************************************************************************  
  */
u8 GPSInfoIsPass(GPS_INFO* Ptr)
{
    if ((Ptr->Latitude[0] == 0) || (Ptr->Longitude[0] == 0))
    {
        return 0;
    }
    return 1;
}

/**
  *************************************************************************************
  * @brief  GPS��ʽ������(�ȷ����ʽ)ת���ɶȸ�ʽ
  * @param  �� 
  * @retval ��γ��
  * @date   2019��7��16��
  * @note   
  *************************************************************************************  
  */
double GPSFormatConvert(u8* gpsFormat, u8 Bytes)
{
    double DValue;
    u16 U16Value;
    char Buffer[30];
    
    if(Bytes > sizeof(Buffer))
    {
        return 0;
    }
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, (char*)gpsFormat, Bytes);
    U16Value = Buffer[strlen(Buffer)-1];
    if ((U16Value < '0') || (U16Value > '9'))
    {
        Buffer[strlen(Buffer)-1] = 0x00;
    }
    sscanf((char*)Buffer, "%lf", (double*)&DValue);
    U16Value = (u16)DValue;
    U16Value = U16Value-(U16Value%100);
    return (double)((int)(U16Value/100) + (DValue-U16Value)/(double)60.0);
}
/**
  *************************************************************************************
  * @brief  ����2���ص�(��γ��)֮��ľ���
  * @param  �� 
  * @retval ���룬��λ��
  * @date   2019��7��16��
  * @note   
  *************************************************************************************  
  */
double GetDistance(double longitude1, double latitude1, double longitude2, double latitude2)
{
    float PI = 3.14159;
    double lat1 = (PI/180)*latitude1;
    double lat2 = (PI/180)*latitude2;

    double lon1 = (PI/180)*longitude1;
    double lon2 = (PI/180)*longitude2;

    //����뾶
    double R = 6371.004;

    //�������� m
    double dis =  acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon2-lon1))*R*1000.0;
    return dis;
}
