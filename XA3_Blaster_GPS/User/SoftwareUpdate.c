#include "includes.h"
#include "GuiIncludes.h"

HEX_LOADER HexLoader;


/**
  *************************************************************************************
  * @brief  ��ȡ����������еĹ̼��汾������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2015��01��27��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetServerDeviceVersionInfoData(u8* Ptr)
{
	u16 index = 0;
	
	/*
		����������������ͣ�1+0x0d������ID��+0x0D+�豸��ǰ����汾+0x0D+�豸Ӳ���汾+0x0D
	����������������أ�
	(1)	����Ҫ������ ��1��
	(2)	��Ҫ��������2��+��0x0D��+�ļ���С+��0x0D��+�汾��+��0x0D��
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x14;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�

	*(Ptr+index++) = '1';
	*(Ptr+index++) = 0x0D;

	/***������ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//�ָ���

	/***����汾***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;	
	
	/***Ӳ���汾***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.hw_version);
	index += strlen((const char*)device_info.hw_version);
	*(Ptr+index++) = 0x0D;//�ָ���	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  ��ȡ���÷�����׼�����¹̼�������
  * @param  Ptr ���ݻ������׵�ַ
  * @param  NewVersion �¹̼��汾  
  * @retval ���ݳ���
  * @author ZH
  * @date   2015��01��27��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetSetServerReadyForUpdateData(u8* Ptr, u8* NewVersion)
{
	u16 index = 0;
	
	/*
			����������������ͣ�2+0x0D+������ID��+0x0D+�豸��ǰ����汾+0x0D+�豸Ӳ���汾+0x0D+��������������汾+0x0D
			����������������أ�
			(1)	�ɹ��� ��1��
			(2)	ʧ�ܣ���2��
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x14;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�

	*(Ptr+index++) = '2';
	*(Ptr+index++) = 0x0D;

	/***������ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//�ָ���

	/***����汾***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;	
	
	/***Ӳ���汾***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.hw_version);
	index += strlen((const char*)device_info.hw_version);
	*(Ptr+index++) = 0x0D;//�ָ���	

	/***��������������汾***/	
	strcpy((char*)(Ptr+index), (const char*)NewVersion);
	index += strlen((const char*)NewVersion);
	*(Ptr+index++) = 0x0D;//�ָ���	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�������̼��ļ�����
  * @param  Ptr ���ݻ������׵�ַ
  * @param  Pack ���ݰ���
  * @param  CurrentSize �ѽ������ݳ���
  * @param  NextSize ����Ҫ��ȡ�����ݳ���  
  * @retval ���ݳ���
  * @author ZH
  * @date   2015��01��27��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetServerHexFileDataData(u8* Ptr, u16 PackCount, u32 CurrentSize, u16 NextSize)
{
	u16 index = 0;
	
	/*
	����������������ͣ�3+0x0D+����+0x0D+�ѽ������ݳ���+0x0D+��Ҫ��ȡ�����������ֽڳ��ȣ����Ը���gprsģ����ź�ǿ������̬�����������ݵİ����ȣ�+0x0D
	����������������أ�
	���ţ������ֽ��޷�������+��������		
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x14;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�

	*(Ptr+index++) = '3';
	*(Ptr+index++) = 0x0D;

	index += sprintf((char*)(Ptr+index), "%u\r", PackCount);//����

	index += sprintf((char*)(Ptr+index), "%ld\r", CurrentSize);
	index += sprintf((char*)(Ptr+index), "%ld\r", NextSize);
	
	return index;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�������̼��ļ�����У��������
  * @param  Ptr ���ݻ������׵�ַ
  * @param  VerifyValue �̼��ļ�����У����
  * @param  NewVersion �̼��汾  
  * @retval ���ݳ���
  * @author ZH
  * @date   2015��01��27��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetServerHexFileVerifyResultData(u8* Ptr, u16 VerifyValue, u8* NewVersion)
{
	u16 index = 0;
	
	/*
	У�������ļ�����������������ͣ�4+0x0D+У���루�ݶ�crc16��+0x0D+ҪУ��������ļ��汾+0x0D
	����������������أ�
	(1)	�ɹ��� ��1��
	(2)	ʧ�ܣ���2��		
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x14;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�

	*(Ptr+index++) = '4';
	*(Ptr+index++) = 0x0D;

	index += sprintf((char*)(Ptr+index), "%u\r", VerifyValue);
	
	/***��������������汾***/	
	strcpy((char*)(Ptr+index), (const char*)NewVersion);
	index += strlen((const char*)NewVersion);
	*(Ptr+index++) = 0x0D;//�ָ���	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�������еĹ̼���Ϣ
  * @param  FileSize �ļ���С
  * @param  VersionPtr �̼��汾
  * @param  BufferSize VersionPtr��������С
  * @param  WifiFlag WIFI�Ƿ����  
  * @retval ���ͽ��
  *   @arg @b 0x00 ͨ���쳣
  *   @arg @b 0x01 ����Ҫ����
  *   @arg @b 0x02 ��Ҫ����
  * @author ZH
  * @date   2015��01��27��
  * @note   
  *************************************************************************************  
  */
u8 GetServerDeviceVersionInfo(u32* FileSize, u8* VersionPtr, u8 BufferSize, ErrorStatus WifiFlag)
{	
	u16 StartIndex = 0;
	u16 Bytes = 0;
	
	SendBuffer.bytenum = GetServerDeviceVersionInfoData(SendBuffer.uch_buf);
	Pack(&SendBuffer, &TempBuffer);//���ݴ��
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if((TempBuffer.bytenum == 7))
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x14) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '1')  && (TempBuffer.uch_buf[6] == 0x0D))
				{
					return 0x01;//����Ҫ����			
				}			
			}
			else if(TempBuffer.bytenum > 7) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x14) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '2')  && (TempBuffer.uch_buf[6] == 0x0D) &&
					(TempBuffer.uch_buf[TempBuffer.bytenum-1] == 0x0D))
				{
					if(ERROR != GetDecValueFromSeparator('\r', 2, &TempBuffer.uch_buf[5],TempBuffer.bytenum-5,FileSize))
					{
						if(ERROR != GetDataFromSeparator('\r', 3, &TempBuffer.uch_buf[5],TempBuffer.bytenum-5, &StartIndex, &Bytes))
						{
							if(BufferSize > Bytes)
							{
								if(VersionPtr != NULL)
								{
									memcpy(VersionPtr, &TempBuffer.uch_buf[5+StartIndex], Bytes);
									*(VersionPtr+Bytes) = 0x00;								
								}
								return 0x02;//��Ҫ����
							}								
						}									
					}
				}				
			}
		}
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  ֪ͨ������׼����ʼ�����̼�
  * @param  NewVersion �¹̼��汾��
  * @param  WifiFlag WIFI�Ƿ����  
  * @retval ���ͽ��
  *   @arg @b SUCCESS �ɹ�
  *   @arg @b ERROR ʧ��
  * @author ZH
  * @date   2015��01��27��
  * @note   
  *************************************************************************************  
  */
ErrorStatus SetServerReadyForUpdate(u8* NewVersion, ErrorStatus WifiFlag)
{
	u8 Result = 0;
	
	SendBuffer.bytenum = GetSetServerReadyForUpdateData(SendBuffer.uch_buf, NewVersion);

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
  * @brief  ��ȡ�������е�HEX�ļ�����
  * @param  CurrentSize �ѽ������ݳ���
  * @param  NextSize ����Ҫ��ȡ�����ݳ��� 
  * @param  WifiFlag WIFI�Ƿ����  
  * @retval ���ͽ��
  *   @arg @b SUCCESS �ɹ�
  *   @arg @b ERROR ʧ��
  * @author ZH
  * @date   2015��01��27��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetServerHexFileData(u16 PackCount, u32 CurrentSize, u16 NextSize, u8* FileData, ErrorStatus WifiFlag)
{
	SendBuffer.bytenum = GetServerHexFileDataData(SendBuffer.uch_buf, PackCount, CurrentSize, NextSize);
	
	Pack(&SendBuffer,&TempBuffer);//���ݴ��
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == (NextSize+7))
			{
				if((TempBuffer.uch_buf[1] == SendBuffer.uch_buf[1])  && (TempBuffer.uch_buf[2] == SendBuffer.uch_buf[2]) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == (PackCount>>8))  && (TempBuffer.uch_buf[6] == (PackCount&0xFF)))
				{
					memcpy(FileData, &TempBuffer.uch_buf[7], NextSize);
					return SUCCESS;
				}			
			}
		}
	}		
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ��ȡ�������е�HEX�ļ�У��������
  * @param  VerifyValue У��ֵ
  * @param  NewVersion ҪУ����ļ��汾��
  * @param  WifiFlag WIFI�Ƿ����  
  * @retval ���ͽ��
  *   @arg @b SUCCESS �ɹ�
  *   @arg @b ERROR ʧ��
  * @author ZH
  * @date   2015��01��27��
  * @note   
  *************************************************************************************  
  */
u8 GetServerHexFileVerifyResult(u16 VerifyValue, u8* NewVersion, ErrorStatus WifiFlag)
{
	u8 Result = 0;
	
	SendBuffer.bytenum = GetServerHexFileVerifyResultData(SendBuffer.uch_buf, VerifyValue, NewVersion);
	Result = WireleGeneralTransferData(WifiFlag);

	return Result;
}

/**
  *************************************************************************************
  * @brief  �ӷ����������ع̼�
  * @param  ��
  * @retval ���
  *   @arg @b 0 ͨ���쳣
  *   @arg @b 1 ���سɹ�
  *   @arg @b 2 ����Ҫ���� 
  *   @arg @b 3 �ļ�У�鲻ͨ��
  *   @arg @b 4 �ļ�д��ʧ��  
  * @author ZH
  * @date   2015��01��27��
  * @note   
  *************************************************************************************  
  */
u8 DeviceHexFileLoader(ErrorStatus WifiFlag)
{
	u8 U8Value;
	u16 Bytes;
	u16 PackCount = 0;
	WM_MESSAGE Msg;
	static u8 temp[400];
	u16 CRC16 = 0;
	
	memset((u8*)&HexLoader, 0, sizeof(HEX_LOADER));
	
	if(ERROR != GetSpecifiedFileExistState(NULL, "0:CPU.hex"))
	{//�Ѿ�������ɲ��Ƶ���Ŀ¼��
		return 1;
	}
	
	Msg.MsgId = WM_SOFTWARE_UPDATE;	
	Msg.Data.v = 1;
	
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
	
	//��ȡ�������еĹ̼���Ϣ
	U8Value = GetServerDeviceVersionInfo(&HexLoader.NewFileSize, &HexLoader.NewVersion[0], sizeof(HexLoader.NewVersion), WifiFlag);
	if(U8Value == 1)
	{//����Ҫ����
		return 2;
	}
	else if(U8Value != 2)
	{
		return 0;//�쳣
	}
	if ((HexLoader.NewFileSize < 1024) || (HexLoader.NewFileSize > 5242880UL))
	{//�������ļ�������С��1KҲ�����ܴ���5M
		return 0;
	}
	//֪ͨ������׼����ʼ�����̼�
	if(ERROR == SetServerReadyForUpdate(&HexLoader.NewVersion[0], WifiFlag))
	{
		return 0;//�쳣
	}
	
	//����ǰ�ļ���С
	sprintf((char*)temp,"%s.hex",&HexLoader.NewVersion[0]);
	DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, (char*)temp);
	
	//����ǰ�ļ���С
	if(ERROR == ReadSpecifiedHexFileSize(SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0],&HexLoader.CurrentFileSize))
	{//û�ж�Ӧ���ļ�
		HexLoader.CurrentFileSize = 0;
	}
	if ((HexLoader.CurrentFileSize > 5242880UL) || (HexLoader.CurrentFileSize > HexLoader.NewFileSize))
	{//�������ļ������ܴ���5M
		DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, NULL);
		HexLoader.CurrentFileSize = 0;
	}

	Msg.Data.v = 2;//��ʾ��ǰ����
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
		
	//��ȡ�������е�HEX�ļ�����
	while(HexLoader.CurrentFileSize < HexLoader.NewFileSize)
	{
		//ÿ������500�ֽ�
		if ((HexLoader.CurrentFileSize + sizeof(temp)) <= HexLoader.NewFileSize)
		{
			Bytes = sizeof(temp);
		}
		else
		{
			Bytes = HexLoader.NewFileSize - HexLoader.CurrentFileSize;
		}
		
		if(ERROR == GetServerHexFileData(PackCount++, HexLoader.CurrentFileSize, Bytes, temp, WifiFlag))
		{
			return 0;//�쳣
		}
				
		HexLoader.CurrentFileSize += Bytes;
		Msg.Data.v = 2;//��ʾ��ǰ����
		if(WM_IsWindow(DlghWin.Dlg))
		{
			WM_SendMessage(DlghWin.Dlg, &Msg);
		}
		
		
		//�������ص�������
		if(ERROR == WriteHexFileData(SOFTWARE_UPDATE_FILE_PATH,&HexLoader.NewVersion[0],&temp[0], Bytes))
		{
			return 4;
		}		
	}
	
	Msg.Data.v = 3;//����У��ֵ
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}	
	if(ERROR == GetSpecifiedHexFileCRCValue(SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0], &CRC16))
	{
		return 4;
	}

	//��ȡ�������е�HEX�ļ�У��������
	U8Value = GetServerHexFileVerifyResult(CRC16, &HexLoader.NewVersion[0], WifiFlag);
	if('1' == U8Value)
	{//	
		//�ļ����سɹ�
		sprintf((char*)temp,"%s/%s.hex", SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0]);
		OSSchedLock();
		f_rename((const char*)temp, "0:CPU.hex");
		OSSchedUnlock();
		return 1;//���³ɹ�
	}
	else if('2' == U8Value)
	{
		DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, NULL);
		return 3;//У��ʧ��
	}
	return 0;
}
