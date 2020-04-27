#include "includes.h"
#include "GuiIncludes.h"


/**
  *************************************************************************************
  * @brief  ��ȡ�ϱ���־�ļ��ĵ�һ������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2018��05��10��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetLogDataFirstPack(u8* Ptr, u8 LogIndex)
{
	u16 index = 0;
	
	/*
		����������������ͣ��ļ�����+0x0D+�ļ���С+0x0D
	����������������أ�"1"
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x19;//��������ֽ�

	*(Ptr+index++) = 0x00;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0x00;//�ְ����ŵ��ֽ�

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
	
	/***�ļ�����***/
	memcpy((char*)(Ptr+index), &LogRecorder.FileName[LogIndex][0], sizeof(LogRecorder.FileName[0]));
	index += sizeof(LogRecorder.FileName[0]);
	
	strcpy((char*)(Ptr+index), ".txt");
	index += strlen(".txt");
	*(Ptr+index++) = 0x0D;//�ָ���

	/***�ļ���С***/
	index += sprintf((char*)(Ptr+index),"%d\r", GetLogFileSize(LogIndex));
					
	return index;
}
/**
  *************************************************************************************
  * @brief  ��ȡ�ϱ���־�ļ��Ľ���������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2018��05��10��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetLogDataPack(u8* Ptr, u16 PackIndex)
{
	u16 index = 0;
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x19;//��������ֽ�

	*(Ptr+index++) = (PackIndex>>8)&0xFF;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = PackIndex&0xFF;//�ְ����ŵ��ֽ�
					
	return index;
}

/**
  *************************************************************************************
  * @brief  ��ȡ�ϱ���־�ļ��Ľ���������
  * @param  ���ݻ������׵�ַ
  * @retval ���ݳ���
  * @author ZH
  * @date   2018��05��10��
  * @note   ��
  *************************************************************************************  
  */ 
u16 GetLogDataEndPack(u8* Ptr, u8 LogIndex)
{
	u16 index = 0;
	
	/*
		����������������ͣ��ļ�����+0x0D+�ļ���С+0x0D
	����������������أ�"1"
	*/
	
	*(Ptr+index++) = 0x41;//Э���ַ
	*(Ptr+index++) = 0x00;//��������ֽ�
	*(Ptr+index++) = 0x19;//��������ֽ�

	*(Ptr+index++) = 0xFF;//�ְ����Ÿ��ֽ�,��1��
	*(Ptr+index++) = 0xFF;//�ְ����ŵ��ֽ�

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
	
	/***�ļ�����***/
	memcpy((char*)(Ptr+index), &LogRecorder.FileName[LogIndex][0], sizeof(LogRecorder.FileName[0]));
	index += sizeof(LogRecorder.FileName[0]);
	
	strcpy((char*)(Ptr+index), ".txt");
	index += strlen(".txt");
	*(Ptr+index++) = 0x0D;//�ָ���

	/***�ļ���С***/
	index += sprintf((char*)(Ptr+index),"%d\r", GetLogFileSize(LogIndex));
					
	return index;
}
/**
  *************************************************************************************
  * @brief  ������־�ļ����ݵ���������
  * @param  index ��־�ļ�����
  * @retval ���ͽ��
  *   @arg @b 0x31 �ɹ�
  *   @arg @b ����   ʧ��  
  * @author ZH
  * @date   2018��05��10��
  * @note   ���ϴ������н�ֹд��־
  *************************************************************************************  
  */
u8 SendLogFileToServer(u16 index, ErrorStatus WifiFlag)
{
	const u16 ItemsMax = 400;
	u8 Result;
	WM_MESSAGE Msg;
	u16 SendBytes;
	u16 SendCount = 0;
	u32 fileSize;
	u32 sumBytes = 0;

	Msg.MsgId = WM_UPDATE_PROGBAR;	
	Msg.Data.v = 0;
	
	fileSize = GetLogFileSize(index);
	if(fileSize == 0)
	{
		return 0xFF;
	}
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}	
	//���͵�1������
	SendBuffer.bytenum = GetLogDataFirstPack(SendBuffer.uch_buf, index);//��ȡ�𱬼�¼���ݵ�1��������
	Result = WireleGeneralTransferData(WifiFlag);	
	if(Result != '1')
	{
		return Result;
	}
	SendBytes = ItemsMax;//Ĭ��ÿ�η�400�ֽ�
	while(1)
	{
		SendBuffer.bytenum = GetLogDataPack(SendBuffer.uch_buf, SendCount+1);//�ӵ�2����ʼ�����׹���Ϣ����
				
		if ((SendCount*ItemsMax + SendBytes) > fileSize)
		{
			SendBytes = fileSize%ItemsMax;
		}
		
		if(ERROR == GetLogRecorderData(index, SendCount*ItemsMax, SendBytes, &SendBuffer.uch_buf[SendBuffer.bytenum]))
		{
			return 0xFF;
		}
		SendBuffer.bytenum += SendBytes;
				
		Result = WireleGeneralTransferData(WifiFlag);	
		if(Result != '1')
		{
			return Result;
		}
		sumBytes = SendCount*ItemsMax + SendBytes;
		if (sumBytes >= fileSize)
		{//�Ѿ�ȫ���������
			break;
		}
		SendCount++;//������һ��
		
		if(WM_IsWindow(DlghWin.Dlg))
		{
			Msg.Data.v = (double)sumBytes/fileSize*10000;
			if(Msg.Data.v > 10000)
			{
				Msg.Data.v = 10000;
			}
			WM_SendMessage(DlghWin.Dlg, &Msg);
		}
	}

	//���ͽ�������־
	SendBuffer.bytenum = GetLogDataEndPack(SendBuffer.uch_buf, index);
	Result = WireleGeneralTransferData(WifiFlag);
	
	return Result;
}

