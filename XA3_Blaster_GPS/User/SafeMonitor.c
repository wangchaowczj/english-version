#include "includes.h"

STR_COMM SendBuffer, ReceiceBuffer, TempBuffer;
extern void UpdateProgbar(u16 Value);
/**
  *************************************************************************************
  * @brief  ��ȡ�豸����ʱ�ϴ�����ϵͳ������(Э���е���������)
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   ��
  *************************************************************************************  
  */  
u16 GetDeviceStartData(u8* Ptr)
{
//	u16 index = 0;
//	u16 Value;

//	/*
//	����������������ͣ�
//	��1������γ������û�ж�λ�ɹ�ʱ��������ID�� +0x0D+0x02+ 0x0D+�豸�汾+0x0D+�豸����+0x0D
//	��2������γ�����궨λ�ɹ�ʱ��������ID�� +0x0D+��γ��+ 0x0D+�豸�汾+0x0D+�豸����+0x0D
//	���������أ�
//	��1��	����ܵ����ݰ���XX+0x0D
//	XX-�ɹ�0x01 ʧ��:������
//	*/	
//	
//	*(Ptr+index++) = 0x41;//Э���ַ
//	*(Ptr+index++) = 0x00;//��������ֽ�
//	*(Ptr+index++) = 0x17;//��������ֽ�
//	
//	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
//	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�
//	
//	/***������ID***/
//	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
//	index += strlen((const char*)device_info.mac);
//	*(Ptr+index++) = 0x0D;//�ָ���
//	
//	/***��γ��,��ʽΪ"11620.1092E,3957.7800N"***/
//	if(GPS_SUCESS != Op_Data.Gps.GPS_state)
//	{
//		strcpy((char*)(Ptr+index), "0x02");
//		index += strlen("0x02");
//	}
//	else
//	{			
//		memcpy(Ptr+index, Op_Data.Gps.GPS_Longitude, 10);//���ȣ����ݸ�ʽ��һ��
//		index += 10;
//		*(Ptr+index++) = Op_Data.Gps.GPS_Longitude[11];
//		*(Ptr+index++) = ',';
//		memcpy(Ptr+index, Op_Data.Gps.GPS_Latitude, 9);//γ�ȣ����ݸ�ʽ��һ��
//		index += 9;	
//		*(Ptr+index++) = Op_Data.Gps.GPS_Latitude[10];	
//	}
//	*(Ptr+index++) = 0x0D;
//	
//	/***����汾***/
//	strcpy((char*)(Ptr+index), VERSION_STRING);
//	index += strlen(VERSION_STRING);
//	*(Ptr+index++) = 0x0D;
//	
//	/***�豸����***/
//	Value = Op_Data.BatteryVoltage;
//	index += sprintf((char*)(Ptr+index), "%d", Value);
//	
//	*(Ptr+index++) = 0x0D;
//	
//	return index;
return 0;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�豸Զ����֤����
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��25��
  * @note   ��
  *************************************************************************************  
  */  
u16 GetDeviceArgumentData(u8* Ptr)
{
	u16 index = 0;

	/*
	����������������ͣ�
	��ȨID�ţ���Ӧ��ϸ��ַ��+0x0D+������ID��+ 0x0D + ��ԿID��+ 0x0D
	���������أ�
	XX+0x0D+YY+0x0D+ZZ+0x0D+KK+0x0D
	X �ɹ��ı�ʾ���ɹ���0x01��ʧ�ܣ������롣
	YY����ȨID��Ч�� ZZ�ǿ�������Ч�� KK��Կ��Ч�ԡ�
	��Ч����1������Ч����2����
	*/	
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x16;//��������ֽ�
	
	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�

	/***��ȨID��***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.License);
	index += strlen((const char*)BlastInfo.DkeyInfo.License);
	*(Ptr+index++) = 0x0D;//�ָ���
	
	/***������ID***/
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//�ָ���	
	
	/***��ԿID***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Mac);
	index += strlen((const char*)BlastInfo.DkeyInfo.Mac);
	*(Ptr+index++) = 0x0D;//�ָ���	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */  
u16 GetBlastDataFirstPackFromCurrentFire(u8* Ptr)
{
	u16 index = 0;
	u16 Value;

	/*
		���ţ�0x0000+��ȨID��+ 0x0D +��Կ��ID�� + 0x0D +������ID�� +0x0D+
		��ʱ�� + 0x0D + ��γ������ + 0x0D + ע���׹����� +0x0D+�����׹����� +0x0D + 
		ע������+ 0x0D+ �׹���Ϣ����+0x0D+����汾��+0x0D+��Կ��¼���û���+0x0D+�ϴ���������+0x0D
	*/

	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	*(Ptr+index++) = 0x00;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0x00;//�ְ����ŵ��ֽ�

	/***��ȨID��***/	
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.License);
	index += strlen((const char*)BlastInfo.DkeyInfo.License);
	*(Ptr+index++) = 0x0D;
	
	/***��ԿID***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Mac);
	index += strlen((const char*)BlastInfo.DkeyInfo.Mac);
	*(Ptr+index++) = 0x0D;//�ָ���	

	/***������ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//�ָ���
	
	/***��ʱ��(��ǰʱ��)***/
	index +=sprintf((char*)(Ptr+index), "%04d%02d%02d%02d%02d%02d", BlastInfo.Time.year, 
				BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);			
	*(Ptr+index++) = 0x0D;	
	
	/***��γ��,��ʽΪ"11620.1092E,3957.7800N"***/
	if((strlen((char*)Op_Data.Gps.Longitude) >1) && (strlen((char*)Op_Data.Gps.Latitude) > 1))
	{
		memcpy(Ptr+index, Op_Data.Gps.Longitude, strlen((char*)Op_Data.Gps.Longitude));//���ȣ����ݸ�ʽ��һ��
		index += strlen((char*)Op_Data.Gps.Longitude);
		*(Ptr+index++) = ',';
		memcpy(Ptr+index, Op_Data.Gps.Latitude, strlen((char*)Op_Data.Gps.Latitude));//γ�ȣ����ݸ�ʽ��һ��
		index += strlen((char*)Op_Data.Gps.Latitude);		
	}	

	*(Ptr+index++) = 0x0D;	
	
	/***ע���׹�����***/
	Value = Det.RegCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***�����׹�����***/
	Value = Det.ErrorCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;		

	/***ע��������***/
	*(Ptr+index++) = '1';	
	*(Ptr+index++) = 0x0D;	

	/***ÿ���׹���Ϣ����14�ֽ�***/
	*(Ptr+index++) = '1';	
	*(Ptr+index++) = '4';
	*(Ptr+index++) = 0x0D;	

	/***����汾***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;
	
	/***��Կ�û���***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Operator);
	index += strlen((const char*)BlastInfo.DkeyInfo.Operator);
	*(Ptr+index++) = 0x0D;//�ָ���
	
	/***�ϴ���������***/
	*(Ptr+index++) = '2';//*(Ptr+index++) = BlastInfo.DkeyInfo.UploadType;//�ϴ���������
	*(Ptr+index++) = 0x0D;//�ָ���
	
	return index;
} 

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */  
u16 GetBlastDataFirstPackFromRecorder(u8* Ptr, u16 RecorderIndex, u8* SlaveNumber)
{
	u16 index = 0;
	u8 Buffer[100], Number = 0;
	u8 FileName[17];
	u32 Value;
    
	/*
		���ţ�0x0000+��ȨID��+ 0x0D +��Կ��ID�� + 0x0D +������ID�� +0x0D+
		��ʱ�� + 0x0D + ��γ������ + 0x0D + ע���׹����� +0x0D+�����׹����� +0x0D + 
		ע������+ 0x0D+ �׹���Ϣ����+0x0D+����汾��+0x0D
	*/
	if(RecorderIndex == 0xFFFF)
	{//��ǰ�𱬼�¼
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}

	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	*(Ptr+index++) = 0x00;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0x00;//�ְ����ŵ��ֽ�
	
	while(1)
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//���½���
		}
		
		Buffer[0] = 0x00;
		switch(Number)
		{
			case 0:
				/***��ȨID��***/
				GetConfigFileString("��Կ��Ȩ��", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "��ȨID_123");	
				break;
			case 1:
				/***��ԿID***/
				GetConfigFileString("��ԿID", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "ID_123");
				break;
			case 2:
				/***������ID***/
				GetConfigFileString("����ID", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "����ID");				
				break;	
			case 3:
				/***��ʱ��***/
				GetConfigFileString("��ʱ��", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 4:
				/***��γ��,��ʽΪ"11620.1092E,3957.7800N"***/
				GetConfigFileString("��γ������", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 5:
				/***ע���׹�����"***/
				GetConfigFileString("ע���׹�����", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 6:
				/***�����׹�����"***/
				GetConfigFileString("�����׹�����", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 7:
				/***ע��������***/
				*SlaveNumber = (u8)GetConfigFileU32("����������", 1, (u8*)GetBlasterPath(),FileName);
				sprintf((char*)&Buffer[0], "%d", *SlaveNumber);
				//GetConfigFileString("����������", &Buffer[0],100,BLASTER_RECORD_FILE_PATH,FileName);
				break;	
			case 8:
				/***ÿ���׹���Ϣ����14�ֽ�***/
				Buffer[0] = '1';
				Buffer[1] = '4';
				Buffer[2] = 0x00;
				break;
			case 9:
				/***����汾"***/
				GetConfigFileString("����汾", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 10:
				/***�û���"***/
				GetConfigFileString("�û���", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 11:
				/***�ϴ���������"***/
				Value = GetConfigFileU32("�ϴ���������",'2', (u8*)GetBlasterPath(),FileName);	
				Buffer[0] = (u8)((Value&0xFF) + '0');
				Buffer[1] = 0x00;
				break;				
			default:
				return index;
//				break;
		}
		strcpy((char*)(Ptr+index), (char*)&Buffer[0]);
		index += strlen((const char*)&Buffer[0]);
		*(Ptr+index++) = 0x0D;//���ݼ�ķָ���	
			
		Number++;
	}
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */  
u16 GetBlastDataSecondPackFromCurrentFire(u8* Ptr)
{
	u16 index = 0;
	u16 Value;
	
	/*
		���ţ�0x0001+ע����ID+0x0D+����汾��+0x0D +ע���׹���+0x0D+�����׹���+0x0D +
		���Ժ����ߵ�ѹ+ 0x0D +���Ժ����ߵ���+ 0x0D +����©����+ 0x0D +��·�������ߵ�ѹ+ 
		0x0D +��·�������ߵ���+ 0x0D +��ǰ���ߵ�ѹ+ 0x0D +��ǰ���ߵ���+ 0x0D
	*/

	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	*(Ptr+index++) = 0x00;//�ְ����Ÿ��ֽ�,��2��
	*(Ptr+index++) = 0x01;//�ְ����ŵ��ֽ�

	/***ע����ID***/
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//�ָ���
	
	/***����汾***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;
	
	/***ע���׹�����***/
	Value = Det.RegCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***�����׹�����***/
	Value = Det.ErrorCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***���Ժ����ߵ�ѹ***/
	Value = BlastInfo.V_BeforeTest;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***���Ժ����ߵ���***/
	Value = BlastInfo.I_BeforeTest;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***����©����***/
	Value = BlastInfo.I_Leak;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***��·�������ߵ�ѹ***/
	Value = BlastInfo.V_AfterCharge;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***��·�������ߵ���***/
	Value = BlastInfo.I_AfterCharge;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;

	/***��ǰ���ߵ�ѹ***/
	Value = BlastInfo.V_BeforeFire;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;		

	/***��ǰ���ߵ���***/
	Value = BlastInfo.I_BeforeFire;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;
	
	return index;
} 
/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */  
u16 GetBlastDataSecondPackFromRecorder(u8* Ptr, u16 RecorderIndex, u8 AddrName)
{
	u16 index = 0;
	u8 Buffer[100], Number = 0, Len;
	u8 FileName[17];
	u8 KeyName[40];
	
	/*
		���ţ�0x0001+ע����ID+0x0D+����汾��+0x0D +ע���׹���+0x0D+�����׹���+0x0D +
		���Ժ����ߵ�ѹ+ 0x0D +���Ժ����ߵ���+ 0x0D +����©����+ 0x0D +��·�������ߵ�ѹ+ 
		0x0D +��·�������ߵ���+ 0x0D +��ǰ���ߵ�ѹ+ 0x0D +��ǰ���ߵ���+ 0x0D
	*/
	if(RecorderIndex == 0xFFFF)
	{//��ǰ�𱬼�¼
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	*(Ptr+index++) = 0x00;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0x01;//�ְ����ŵ��ֽ�

	while(1)
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//���½���
		}
		Buffer[0] = 0x00;
		switch(Number)
		{
			case 0:
				/***ע����ID***/
				sprintf((char*)KeyName, "%c������ID", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "����ID_1");
				break;
			case 1:
				/***����汾"***/
				sprintf((char*)KeyName, "%c����汾", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);					
				break;
			case 2:
				/***ע���׹�����"***/
				sprintf((char*)KeyName, "%cע���׹���", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);			
				break;	
			case 3:
				/***�����׹�����"***/
				sprintf((char*)KeyName, "%c�����׹���", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);			
				break;	
			case 4:
				/***�׹ܲ��Ժ����ߵ�ѹ"***/
				sprintf((char*)KeyName, "%c�׹ܲ��Ժ����ߵ�ѹ", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}
				break;	
			case 5:
				/***�׹ܲ��Ժ����ߵ���"***/
				sprintf((char*)KeyName, "%c�׹ܲ��Ժ����ߵ���", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;	
			case 6:
				/***����©����"***/
				sprintf((char*)KeyName, "%c����©����", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;
			case 7:
				/***�������ߵ�ѹ"***/
				sprintf((char*)KeyName, "%c��·�������ߵ�ѹ", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;	
			case 8:
				/***��·�������ߵ���"***/
				sprintf((char*)KeyName, "%c��·�������ߵ���", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;
			case 9:
				/***��ǰ���ߵ�ѹ"***/
				sprintf((char*)KeyName, "%c��ǰ���ߵ�ѹ", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;			
			case 10:
				/***��ǰ���ߵ���"***/
				sprintf((char*)KeyName, "%c��ǰ���ߵ���", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//ȥ����λ
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;				
			default:
				return index;
//				break;
		}
		strcpy((char*)(Ptr+index), (char*)&Buffer[0]);
		index += strlen((const char*)&Buffer[0]);
		*(Ptr+index++) = 0x0D;	
			
		Number++;
	}
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  Ptr���ݻ������׵�ַ
  * @param  PackIndex ���ݰ�����ֵ����С��2
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   �ڵ��ñ�����ʱ��Ҫȷ���׺��Ѿ������
  *************************************************************************************  
  */  
u16 GetBlastDataDetInfoFromCurrentFire(u8* Ptr, u16* PackIndex)
{
	u16 i, index = 0,Pack;
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	Pack = *PackIndex;
	if(40*(Pack-2+1) >= Det.RegCount)
	{
		*(Ptr+index++) = 0xFF;//��ǰΪ�׹���Ϣ�����һ��
		*(Ptr+index++) = 0xFE;//��ǰΪ�׹���Ϣ�����һ��
		*PackIndex  = 0xFFFE;
	}
	else
	{
		*(Ptr+index++) = (u8)(Pack>>8);//�ְ����Ÿ��ֽ�
		*(Ptr+index++) = (u8)(Pack&0xFF);//�ְ����ŵ��ֽ�		
	}
	
	for(i = 40*(Pack-2); i < Det.RegCount; i++)//ÿ�δ�40���׹���Ϣ,ÿ���׹�ռ14���ֽ�
	{		
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[2];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[3];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[2];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[3];
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[2];//Ƶ�ʲ��ᳬ��3�ֽ�
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Err;
		*(Ptr+index++) = (u8)(Det.Info[i].Delay>>8);
		*(Ptr+index++) = (u8)(Det.Info[i].Delay&0xFF);
		if(index >= 565)//ÿ�δ�40���׹���Ϣ,ÿ���׹�ռ14���ֽ�
		{
			break;
		}
	}
	return index;
}

/**
  *************************************************************************************
  * @brief  ���ݵ�ǰ��״̬��ȡ������
  * @param  Ptr���ݻ������׵�ַ
  * @param  PackIndex ���ݰ�����ֵ����С��2
  * @retval ���ݳ���
  * @author ZH
  * @date   2014��12��11��
  * @note   �ڵ��ñ�����ʱ��Ҫȷ���׺��Ѿ������
  *************************************************************************************  
  */  
u16 GetBlastDataDetInfoFromRecorder(u8* Ptr, u16* PackIndex, u16 RecorderIndex, u8 AddrName)
{
	u16 i, j, Len, index = 0,Pack;
	u16 RegCount,delay;
	u8 KeyBuffer[30], Buffer[100], FileName[17], Count;
	u32 Value;
	const u8 DetCountEveryTime = 30;
	
	if(RecorderIndex == 0xFFFF)
	{//��ǰ�𱬼�¼
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}	
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	Pack = *PackIndex;
	
	sprintf((char*)Buffer, "%cע���׹���", AddrName);
	RegCount = GetConfigFileU32(Buffer, 0, (u8*)GetBlasterPath(),FileName);
	
	if(DetCountEveryTime*(Pack-2+1) >= RegCount)
	{
		*(Ptr+index++) = 0xFF;//��ǰΪ�׹���Ϣ�����һ��
		*(Ptr+index++) = 0xFE;//��ǰΪ�׹���Ϣ�����һ��
		*PackIndex  = 0xFFFE;
	}
	else
	{
		*(Ptr+index++) = (u8)(Pack>>8);//�ְ����Ÿ��ֽ�
		*(Ptr+index++) = (u8)(Pack&0xFF);//�ְ����ŵ��ֽ�		
	}
	
	for(i = DetCountEveryTime*(Pack-2); i < RegCount; i++)//ÿ�δ�30���׹���Ϣ,�ڷ��׹�ռ14���ֽ�
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//���½���
		}
		sprintf((char*)KeyBuffer,"%c�׹���Ϣ%d",AddrName, i+1);
		memset(&Buffer[0], 0, sizeof(Buffer));
		GetConfigFileString(&KeyBuffer[0], &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
		Len = strlen((const char*)&Buffer[0]);
		Count = 0;
		for(j=0; j<Len; j++)
		{
			if(Buffer[j] == '\t')
			{
				Count++;
				Buffer[j] = 0x00;
			}
		}
		if(Count < 5)
		{
			return 0;
		}
		Count = 0;
		for(j=0; j<Len; j++)
		{
			if(Buffer[j] == 0x00)
			{
				Count++;
				if(Count == 2)//����
				{
					HexStrToCharStr(&Buffer[j+1],16,Ptr+index);
					index += 8;
				}
				else if(Count == 3)//Ƶ��
				{
					sscanf((const char*)&Buffer[j+1],"%ld", &Value);
					*(Ptr+index++) = (u8)(Value>>16);
					*(Ptr+index++) = (u8)(Value>>8);
					*(Ptr+index++) = (u8)(Value&0xFF);
				}
				else if(Count == 4)//����
				{
					sscanf((const char*)&Buffer[j+1],"%ld", &Value);
					delay = Value;
				}				
				else if(Count == 5)//״̬��
				{
					HexStrToCharStr(&Buffer[j+1],2,Ptr+index);
					index += 1;
					*(Ptr+index++) = (u8)(delay>>8);
					*(Ptr+index++) = (u8)(delay&0xFF);					
					break;
				}				
			}
		}
		if(index >= (DetCountEveryTime*14+5))//ÿ�δ�30���׹���Ϣ,�ڷ��׹�ռ14���ֽ�
		{
			break;
		}
	}
	return index;
}

u16 GetBlastDataEndPack(u8* Ptr)
{
	u16 index = 0;
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x15;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�,��������
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�,��������	
	
	return index;
}
ErrorStatus WireleTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out, ErrorStatus WifiFlag)
{
	if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (WifiFlag != ERROR))
	{//wifi�Ѿ�����
		if(SUCCESS == WifiTransferData(tbuf, tlen, rbuf, rlen,retry,timer_out))
		{
			return SUCCESS;
		}			
	}
//	else
//	{
//		if(GPRS_info.Result == GPRS_SUCESS)
//		{
//			if(GPRS_SUCESS == GPRSTransferData(tbuf, tlen, rbuf, rlen,retry,timer_out))
//			{
//				return SUCCESS;
//			}		
//		}	
//	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  ��������ߴ�������
  * @param  ��
  * @retval ���ͽ��
  *   @arg @b 0x00 ����ʧ��
  *   @arg @b 0x01 ���ͽ��ճɹ�  
  *   @arg @b 0x02 ���յ������ݽ���ɹ������ش���
  *   @arg @b 0x03 ���յ������ݽ��ʧ��   
  * @author ZH
  * @date   2014��12��26��
  * @note   
  *************************************************************************************  
  */
u8 WireleGeneralTransferData(ErrorStatus WifiFlag)
{
	Pack(&SendBuffer, &TempBuffer);//���ݴ��

	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 7)
			{
				if((TempBuffer.uch_buf[1] == SendBuffer.uch_buf[1])  && (TempBuffer.uch_buf[2] == SendBuffer.uch_buf[2]) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) && (TempBuffer.uch_buf[6] == 0x0D))
				{
					if ((TempBuffer.uch_buf[5] >= '0') && (TempBuffer.uch_buf[5] <= '9'))
					{
						return TempBuffer.uch_buf[5];
					}
					return 0x01;
				}			
			}
			return 0x02;
		}
		return 0x03;
	}
	return 0x00;
}
/**
  *************************************************************************************
  * @brief  �����豸�������ݵ���������
  * @param  ��
  * @retval ���ͽ��
  *   @arg @b SUCCESS �ɹ�
  *   @arg @b ERROR   ʧ��  
  * @author ZH
  * @date   2014��12��11��
  * @note  
  *************************************************************************************  
  */
ErrorStatus SendDeviceStartData(ErrorStatus WifiFlag)
{	
	u8 Result;
	
	SendBuffer.bytenum = GetDeviceStartData(SendBuffer.uch_buf);
	Result = WireleGeneralTransferData(WifiFlag);
	if('1' == Result)
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;//����ʧ��
	}
}

/**
  *************************************************************************************
  * @brief  ����Զ����֤���ݵ���������
  * @param  ��
  * @retval ���ͽ��
  *   @arg @b 0 ͨ��ʧ��
  *   @arg @b 1 ��֤ͨ�� 
  *   @arg @b 2 ��ȨID��Ч
  *   @arg @b 3 ������ID��Ч
  *   @arg @b 4 ��ԿID��Ч
  * @author ZH
  * @date   2014��12��11��
  * @note  
  *************************************************************************************  
  */
u8 SendDeviceArgumentData(ErrorStatus WifiFlag)
{	
	SendBuffer.bytenum = GetDeviceArgumentData(SendBuffer.uch_buf);
	
	Pack(&SendBuffer, &TempBuffer);//���ݴ��
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 13) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x16) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '1')  && (TempBuffer.uch_buf[6] == 0x0D) &&
					(TempBuffer.uch_buf[7] == '1')  && (TempBuffer.uch_buf[8] == 0x0D)  && 
					(TempBuffer.uch_buf[9] == '1')  && (TempBuffer.uch_buf[10] == 0x0D)  && 
					(TempBuffer.uch_buf[11] == '1')  && (TempBuffer.uch_buf[12] == 0x0D))
				{
					return 0x01;//��֤ͨ��				
				}
				else if((TempBuffer.uch_buf[7] != '1')  && (TempBuffer.uch_buf[8] == 0x0D))
				{
					return 0x02;//��ȨID��Ч
				}
				else if((TempBuffer.uch_buf[9] != '1')  && (TempBuffer.uch_buf[10] == 0x0D))
				{
					return 0x03;//������ID��Ч
				}
				else if((TempBuffer.uch_buf[11] != '1')  && (TempBuffer.uch_buf[12] == 0x0D))
				{
					return 0x04;//��ԿID��Ч
				}				
			}
		}
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  ��������Ϣ���ݵ���������
  * @param  index �𱬼�¼����,0xFFFF��ʾ��ǰ�𱬼�¼
  * @retval ���ͽ��
  *   @arg @b 0x01 �ɹ�
  *   @arg @b ����   ʧ��  
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */
u8 SendBlastFireInfoData(u16 index, ErrorStatus WifiFlag)
{
	u8 Result;
	u16 PackIndex;
	u8 SlaveNumber = 0;//������������
	u8 SlaveIndex = 0;//��ǰ���ڷ��͵�����������
	u8 FileName[20];
	
	if(index >= BlastRecorder.Number)
	{
		return 0xFF;
	}
	memcpy(FileName, &BlastRecorder.FileName[index][0],12);
	strcpy((char*)&FileName[12],".txt");	
	if(ConfigFileVerify((u8*)GetBlasterPath(), (u8*)FileName, 0) == ERROR)
	{
		return 0xFF;		
	}
	
	//���͵�1������
	SendBuffer.bytenum = GetBlastDataFirstPackFromRecorder(SendBuffer.uch_buf, index, &SlaveNumber);//��ȡ�𱬼�¼���ݵ�1��������
	if(0x00 == SendBuffer.bytenum)
	{
		return 0xFF;
	}

	Result = WireleGeneralTransferData(WifiFlag);	
	if(Result != '1')
	{
		return Result;
	}
	while(SlaveIndex < SlaveNumber)
	{
		//���͵�2������
		SendBuffer.bytenum = GetBlastDataSecondPackFromRecorder(SendBuffer.uch_buf, index, GetAddrNameFromIndex(SlaveIndex));//��ȡ�𱬼�¼���ݵ�2��������
		if(0x00 == SendBuffer.bytenum)
		{
			return 0xFF;
		}		
		Result = WireleGeneralTransferData(WifiFlag);	
		if(Result != '1')
		{
			return Result;
		}
		
		//�ӵ�3����ʼ�����׹���Ϣ����
		PackIndex = 2;
		while(1)
		{
			if(Op_Data.UpdateGuiFlag)
			{
				UpdateProgbar(0);//���½���
			}
			//��ȡ��ǰ�����ݵ��׹��������ݣ�ÿ�η���40���׹���Ϣ����ɺ����������0xFFFE
			SendBuffer.bytenum = GetBlastDataDetInfoFromRecorder(SendBuffer.uch_buf, &PackIndex, index, GetAddrNameFromIndex(SlaveIndex));
			if(0x00 == SendBuffer.bytenum)
			{
				return 0xFF;
			}			

			Result = WireleGeneralTransferData(WifiFlag);	
			if(Result != '1')
			{
				return Result;
			}
			if(PackIndex == 0xFFFE)
			{//�����׹ܶ��Ѿ�����
				break;
			}
			else
			{
				PackIndex++;//����������һ���׹���Ϣ
			}
		}
		SlaveIndex++;//������һ������������Ϣ
	}

	
	//���ͽ�������־
	SendBuffer.bytenum = GetBlastDataEndPack(SendBuffer.uch_buf);
	Result = WireleGeneralTransferData(WifiFlag);
	
	return Result;
}
/**
  *************************************************************************************
  * @brief  �˳����ܹ��ܣ��ر�GPRSģ���Դ��WIFI SOCKET
  * @param  ��
  * @retval �� 
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */
void ExitSafeMonitor(void)
{
	CloseWifiSocket();
//	GPRS_POWER_OFF();
}

/**
  *************************************************************************************
  * @brief  ��ȡ�𱬼�¼���ϴ�״̬
  * @param  ��
  * @retval ERROR ��ʾδ�ϴ� ��SUCCESS����Ҫ�ϴ�
  * @author ZH
  * @date   2014��12��11��
  * @note   ���ص����ݲ������׹���Ϣ
  *************************************************************************************  
  */
ErrorStatus GetBlastRecoderUploadState(u16 Index)
{
	u8 Buffer[100];
	u8 FileName[17];
	
	if(Index < BlastRecorder.Number)
	{
		memcpy(FileName, &BlastRecorder.FileName[Index][0],12);
		strcpy((char*)&FileName[12],".txt");	
		if(ERROR != GetConfigFileString("��Ϣ�ϴ���־", &Buffer[0],50, (u8*)GetBlasterPath(), FileName))
		{
			if(memcmp(Buffer,"δ�ϴ�",6) == 0)
			{
				return ERROR;
			}
		}	
	}

	return SUCCESS;
}


/**
  *************************************************************************************
  * @brief  ��ȡ�������е�ǰ�ı���ʱ��
  * @param  ��
  * @retval ���ͽ��
  *   @arg @b ERROR ͨ��ʧ��
  *   @arg @b SUCCESS �ɹ�
  * @author ZH
  * @date   2016��06��18��
  * @note  
  *************************************************************************************  
  */
ErrorStatus GetServerCurrentTime(ErrorStatus WifiFlag, u8* ServerTimePtr)
{	
	SendBuffer.bytenum = 0;
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x41;//Э���ַ
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x00;//��������ֽ�
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x18;//��������ֽ�

	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽ�
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽ�

	Pack(&SendBuffer, &TempBuffer);//���ݴ��
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,5000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 20) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x18) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF)  && (TempBuffer.uch_buf[TempBuffer.bytenum-1] == 0x0D))
				{
					memset(ServerTimePtr,0,15);
					memcpy(ServerTimePtr, &TempBuffer.uch_buf[5], 14);					
					return SUCCESS;//��ȡ�ɹ�			
				}				
			}
		}
	}
	return ERROR;
}

