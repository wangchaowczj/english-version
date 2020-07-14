#include "includes.h"


DET_INFO_QUEUE DetInfoQueue;


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
void SaveDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item)
{
	if (((Queue->EndIndex+1)%QUEUE_BUFFER_MAX) == Queue->StartIndex)
	{//�Ѿ�����
		Queue->StartIndex++;
		Queue->StartIndex %= QUEUE_BUFFER_MAX;
	}

	memcpy(&Queue->Buffer[Queue->EndIndex++][0], Item, QUEUE_ITEM_LENGTH);
	Queue->EndIndex %= QUEUE_BUFFER_MAX;
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
ErrorStatus GetDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item)
{
	if (Queue->StartIndex == Queue->EndIndex)
	{//û������
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
  * @brief  ��ȡ�շ������ֽ�����
  * @param  Transceiver �շ���
  * @retval �ֽ���
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
  *************************************************************************************  
  */
u16 GetDetInfoQueueItemCount(DET_INFO_QUEUE* Queue)
{
	return (Queue->EndIndex + QUEUE_BUFFER_MAX - Queue->StartIndex)%QUEUE_BUFFER_MAX;
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
void ClearDetInfoQueue(DET_INFO_QUEUE* Queue)
{
	Queue->StartIndex = Queue->EndIndex ;
}

/**
  *************************************************************************************
  * @brief  �ж϶������Ƿ��Ѿ�������ͬ��������
  * @param  Queue ������
  * @retval ��
  * @author ZH
  * @date   2016��05��10��
  * @note   ��
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
