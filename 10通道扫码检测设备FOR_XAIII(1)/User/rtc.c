#include "includes.h"



RTC_Typedef RTC_Type;
RTC_Typedef RTC_Type_Temp;

//==================================================================================================
//| �������� | RTC_Config
//|----------|--------------------------------------------------------------------------------------
//| �������� | ϵͳʵʱʱ������
//|----------|--------------------------------------------------------------------------------------
//| ������� | 
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ERROR   ��ʼ��ʧ��
//|          | SUCCESS ��ʼ���ɹ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
ErrorStatus RTC_Config(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	u32 dly = 0;
	__IO uint32_t uwAsynchPrediv = 0;
	__IO uint32_t uwSynchPrediv = 0;

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)		//��ѯRTC�ı��üĴ�����ֵ�Ƿ񱻸�д��
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);			//����PWRʱ��
		PWR_BackupAccessCmd(ENABLE);								//����PWRʱ�����ӵ�RTC

	#if defined (RTC_CLOCK_SOURCE_LSI)		//���ʹ���ڲ�����LSI
		RCC_LSICmd(ENABLE);											//��LSI
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);			//�ȴ�LSI׼�����
		{
			dly++;
			if(dly > 0xF00000)
			{
				return ERROR;
			}			
		}		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);						//ѡ��RTCʱ��Դ

		//ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
		uwSynchPrediv = 0xFF;										//����Ϊ32768
		uwAsynchPrediv = 0x7F;

	#elif defined (RTC_CLOCK_SOURCE_LSE)	//���ʹ���ⲿ����LSE
		RCC_LSEConfig(RCC_LSE_ON);									//��LSE
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//�ȴ�LSE׼�����
		{
			dly++;
			if(dly > 0xF00000)
			{
				return ERROR;
			}			
		}		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//ѡ��LSEʱ��Դ

		//ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
		uwSynchPrediv = 0xFF;										//����Ϊ32768
		uwAsynchPrediv = 0x7F;

	#else
		#error Please select the RTC Clock source inside the main.c file
	#endif

		RCC_RTCCLKCmd(ENABLE);			//����RTCʱ��
		RTC_WaitForSynchro();			//�ȴ�RTC APB�Ĵ���ͬ��

		RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
		RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);	//RTC��ʼ��

		//��ʼ����������Ϊ2016��1��1��
		RTC_DateStructure.RTC_Year = 0x16;
		RTC_DateStructure.RTC_Month = RTC_Month_January;
		RTC_DateStructure.RTC_Date = 0x01;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);		//�������ڣ�����BCD��ʽ����

		//��ʼ������ʱ��Ϊ0ʱ0��0��
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
		RTC_TimeStructure.RTC_Hours = 0x00;
		RTC_TimeStructure.RTC_Minutes = 0x0;
		RTC_TimeStructure.RTC_Seconds = 0x00;
		RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);		//����ʱ�䣬����BCD��ʽ����

		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);			//��дRTC���üĴ�����ֵ
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//����PWRʱ��
		PWR_BackupAccessCmd(ENABLE);							//����PWRʱ�����ӵ�RTC
		RTC_WaitForSynchro();									//�ȴ�RTC APB�Ĵ���ͬ��
	}
	return SUCCESS;	
}
void RTC_Set_WakeUp(u16 waketime)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RTC_WakeUpCmd(DISABLE);//�ر�Wake Up
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//ʱ��ѡ��1Hz
    RTC_SetWakeUpCounter(waketime);//����WakeUp�Զ���װ��ֵ
    RTC_ClearITPendingBit(RTC_IT_WUT);//���WakeUp�жϱ�־
    EXTI_ClearITPendingBit(EXTI_Line22);//���Line22�жϱ�־
    RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WakeUp�жϱ�־
    RTC_WakeUpCmd(ENABLE);//����WakeUp

    EXTI_InitStruct.EXTI_Line=EXTI_Line22;//LINE22
    EXTI_InitStruct.EXTI_LineCmd=ENABLE;
    EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;//�����ش���
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel=RTC_WKUP_IRQn;//ѡ��WakeUp�ж�
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
    NVIC_Init(&NVIC_InitStruct);
}

//==================================================================================================
//| �������� | RTC_Read_Time
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡϵͳʱ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | 
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void RTC_Read_Time(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	OSSchedLock();
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);	//��ȡʱ��
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);	//��ȡ����	
	
	RTC_Type.year = 2000+RTC_DateStructure.RTC_Year;
	RTC_Type.month = RTC_DateStructure.RTC_Month;
	RTC_Type.day = RTC_DateStructure.RTC_Date;
	RTC_Type.hour = RTC_TimeStructure.RTC_Hours;
	RTC_Type.minute = RTC_TimeStructure.RTC_Minutes;
	RTC_Type.second = RTC_TimeStructure.RTC_Seconds;
	OSSchedUnlock();
}

//==================================================================================================
//| �������� | RTC_Write_Time
//|----------|--------------------------------------------------------------------------------------
//| �������� | д��ϵͳʱ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | 
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���޸ĳɹ�		1�����ݷǷ�		2������С��2000		3�����ó���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u8 RTC_Write_Time(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	ErrorStatus state;

	if (RTC_Type_Temp.year < 2000)		//����С��2000������Ҫ��
	{
		return 2;
	}
	else
	{
		RTC_Type_Temp.year -= 2000;		//ת������Ҫд��ȥ������
	}

	//������ʱ�������ݸ�ʽ�ж�
	//��������յ��жϲ�������Ӧ�·ݶ�Ӧ���յĴ�С������������Ϊ2��31��
	if ((IS_RTC_YEAR(RTC_Type_Temp.year)) && (IS_RTC_MONTH(RTC_Type_Temp.month)) && (IS_RTC_DATE(RTC_Type_Temp.day)) && 
		(IS_RTC_HOUR24(RTC_Type_Temp.hour)) && (IS_RTC_MINUTES(RTC_Type_Temp.minute)) && (IS_RTC_SECONDS(RTC_Type_Temp.second)))
	{
		RTC_DateStructure.RTC_Year = RTC_Type_Temp.year;
		RTC_DateStructure.RTC_Month = RTC_Type_Temp.month;
		RTC_DateStructure.RTC_Date = RTC_Type_Temp.day;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
		RTC_TimeStructure.RTC_Hours = RTC_Type_Temp.hour;
		RTC_TimeStructure.RTC_Minutes = RTC_Type_Temp.minute;
		RTC_TimeStructure.RTC_Seconds = RTC_Type_Temp.second;

		state = RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);	//��������
		if (state == ERROR)
		{
			return 3;
		}
		state = RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);	//����ʱ��
		if (state == ERROR)
		{
			return 3;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

//==================================================================================================
//| �������� | RTC_Get_MaxDay
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ������µ��������ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | NowYear����ǰ���		NowMonth����ǰ�·�
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���޸ĳɹ�		1�����ݷǷ�		2������С��2000		3�����ó���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u8 RTC_Get_MaxDay(u16 NowYear, u8 NowMonth)
{
	if ((NowMonth == 1) || (NowMonth == 3) || (NowMonth == 5) || 
		(NowMonth == 7) || (NowMonth == 8) || (NowMonth == 10) || (NowMonth == 12))
	{
		return 31;
	}
	else if (NowMonth == 2)
	{
		if( ( (NowYear%4==0)&&(NowYear%100!=0) ) || (NowYear%400==0) )		//����
		{
			return 29;
		}
		else
		{
			return 28;
		}
	}
	else
	{
		return 30;
	}
}







