#include "includes.h"


TRANSCEIVER UartTransceiver1;
TRANSCEIVER UartTransceiver2;


/**
  *************************************************************************************
  * @brief  保存一个字节数据到收发器中
  * @param  Transceiver 收发器
  * @param  Byte 数据 
  * @retval 成功或失败
  * @author ZH
  * @date   2016年05月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SaveTransceiverByte(TRANSCEIVER* Transceiver, u8 Byte)
{
	if (((Transceiver->EndIndex+1)%TRANSCEIVER_BUFFER_MAX) == Transceiver->StartIndex)
	{//已经存满
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
  * @brief  获取收发器中一个字节数据
  * @param  Transceiver 收发器
  * @param  Byte 返回的数据 
  * @retval 成功或失败
  * @author ZH
  * @date   2016年05月10日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus GetTransceiverByte(TRANSCEIVER* Transceiver, u8* Byte)
{
	if (Transceiver->StartIndex == Transceiver->EndIndex)
	{//没有数据
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
	{//没有数据
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
  * @brief  获取收发器中字节数量
  * @param  Transceiver 收发器
  * @retval 字节数
  * @author ZH
  * @date   2016年05月10日
  * @note   无
  *************************************************************************************  
  */
u16 GetTransceiverBytesCount(TRANSCEIVER* Transceiver)
{
	return (Transceiver->EndIndex + TRANSCEIVER_BUFFER_MAX - Transceiver->StartIndex)%TRANSCEIVER_BUFFER_MAX;
}

/**
  *************************************************************************************
  * @brief  清空收发器
  * @param  Transceiver 收发器
  * @retval 无
  * @author ZH
  * @date   2016年05月10日
  * @note   无
  *************************************************************************************  
  */
void ClearTransceiver(TRANSCEIVER* Transceiver)
{
	Transceiver->StartIndex = Transceiver->EndIndex ;
}
