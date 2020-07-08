#include "includes.h"


TRANSCEIVER UartTransceiver1;
TRANSCEIVER UartTransceiver2;


/**
  *************************************************************************************
  * @brief  ����һ���ֽ����ݵ��շ�����
  * @param  Transceiver �շ���
  * @param  Byte ���� 
  * @retval �ɹ���ʧ��
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SaveTransceiverByte(TRANSCEIVER* Transceiver, u8 Byte)
{
	if (((Transceiver->EndIndex+1)%TRANSCEIVER_BUFFER_MAX) == Transceiver->StartIndex)
	{//�Ѿ�����
		return ERROR;
	}
	else
	{	
		Transceiver->Buffer[Transceiver->EndIndex++] = Byte;
		Transceiver->EndIndex %= TRANSCEIVER_BUFFER_MAX;
		return SUCCESS;		
	}
}

/**
  *************************************************************************************
  * @brief  ��ȡ�շ�����һ���ֽ�����
  * @param  Transceiver �շ���
  * @param  Byte ���ص����� 
  * @retval �ɹ���ʧ��
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus GetTransceiverByte(TRANSCEIVER* Transceiver, u8* Byte)
{
	if (Transceiver->StartIndex == Transceiver->EndIndex)
	{//û������
		return ERROR;
	}
	else
	{	
		*Byte = Transceiver->Buffer[Transceiver->StartIndex++];
		Transceiver->StartIndex %= TRANSCEIVER_BUFFER_MAX;
		return SUCCESS;		
	}
}

ErrorStatus GetTransceiverByteY(TRANSCEIVER* Transceiver, u8* Byte)
{
	u8 temp2;
	if (Transceiver->StartIndex == Transceiver->EndIndex)
	{//û������
		return ERROR;
	}
	else
	{	
		*Byte=temp2;
		Transceiver->StartIndex %= TRANSCEIVER_BUFFER_MAX;
		return SUCCESS;		
	}
}

/**
  *************************************************************************************
  * @brief  ��ȡ�շ������ֽ�����
  * @param  Transceiver �շ���
  * @retval �ֽ���
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
  *************************************************************************************  
  */
u16 GetTransceiverBytesCount(TRANSCEIVER* Transceiver)
{
	return (Transceiver->EndIndex + TRANSCEIVER_BUFFER_MAX - Transceiver->StartIndex)%TRANSCEIVER_BUFFER_MAX;
}

/**
  *************************************************************************************
  * @brief  ����շ���
  * @param  Transceiver �շ���
  * @retval ��
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
  *************************************************************************************  
  */
void ClearTransceiver(TRANSCEIVER* Transceiver)
{
	Transceiver->StartIndex = Transceiver->EndIndex ;
}
