#include "includes.h"


DET_INFO_QUEUE DetInfoQueue;


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
void SaveDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item)
{
	if (((Queue->EndIndex+1)%QUEUE_BUFFER_MAX) == Queue->StartIndex)
	{//已经存满
		Queue->StartIndex++;
		Queue->StartIndex %= QUEUE_BUFFER_MAX;
	}

	memcpy(&Queue->Buffer[Queue->EndIndex++][0], Item, QUEUE_ITEM_LENGTH);
	Queue->EndIndex %= QUEUE_BUFFER_MAX;
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
ErrorStatus GetDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item)
{
	if (Queue->StartIndex == Queue->EndIndex)
	{//没有数据
		return ERROR;
	}
	else
	{
		memcpy(Item, &Queue->Buffer[Queue->StartIndex++][0], QUEUE_ITEM_LENGTH);
		Queue->StartIndex %= QUEUE_BUFFER_MAX;
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
u16 GetDetInfoQueueItemCount(DET_INFO_QUEUE* Queue)
{
	return (Queue->EndIndex + QUEUE_BUFFER_MAX - Queue->StartIndex)%QUEUE_BUFFER_MAX;
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
void ClearDetInfoQueue(DET_INFO_QUEUE* Queue)
{
	Queue->StartIndex = Queue->EndIndex ;
}

/**
  *************************************************************************************
  * @brief  判断队列中是否已经存在相同的数据项
  * @param  Queue 队列器
  * @retval 无
  * @author ZH
  * @date   2016年05月10日
  * @note   无
  *************************************************************************************  
  */
u8 DetInfoQueueItemIsExist(DET_INFO_QUEUE* Queue, u8* Item)
{
	u16 Index = 0;
	
	Index = Queue->StartIndex;
	while (Index != Queue->EndIndex)
	{		
		if(memcmp(Item, &Queue->Buffer[Index++][0], QUEUE_ITEM_LENGTH) == 0)
		{
			return 1;
		}
		Index %= QUEUE_BUFFER_MAX;	
	}
	return 0;
}
