#include "includes.h"

DET_OUTSIDE_CODE_BUFFER DetOutsideCodeBuffer;


/**
  *************************************************************************************
  * @brief  保存外码到队列中
  * @param  
  * @param  Byte 数据 
  * @retval 成功或失败
  * @author ZH
  * @date   2017年07月20日
  * @note   无
  *************************************************************************************  
  */
void SaveDetOutsideCodeQueueItem(u8* Item)
{
	if(!DetOutsideCodeQueueIsExist(Item))
	{
		if(DetOutsideCodeBuffer.Count == 0)
		{
			memcpy((u8*)&DetOutsideCodeBuffer.StartWith[0], Item, 12);
		}
		
		if(DetOutsideCodeBuffer.Count < DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX)
		{			
			DetOutsideCodeBuffer.Buffer[DetOutsideCodeBuffer.Count++] = *(Item+12);
		}	
	}	
}
/**
  *************************************************************************************
  * @brief  判断队列中是否已经存在相同的数据项
  * @param  Item 数据
  * @retval 无
  * @author ZH
  * @date   2017年07月20日
  * @note   无
  *************************************************************************************  
  */
u8 DetOutsideCodeQueueIsSameStartWith(u8* Item)
{
	if((DetOutsideCodeBuffer.Count == 0) ||
		(memcmp((u8*)&DetOutsideCodeBuffer.StartWith[0], Item, 12) == 0))
	{
		return 1;	
	}

	return 0;
}
/**
  *************************************************************************************
  * @brief  判断队列中是否已经存在相同的数据项
  * @param  Item 数据
  * @retval 无
  * @author ZH
  * @date   2017年07月20日
  * @note   无
  *************************************************************************************  
  */
u8 DetOutsideCodeQueueIsExist(u8* Item)
{
	u8 i = 0;
	
	if(DetOutsideCodeBuffer.Count != 0)
	{
		if(memcmp((u8*)&DetOutsideCodeBuffer.StartWith[0], Item, 12) == 0)
		{
			for(i=0; i<DetOutsideCodeBuffer.Count; i++)
			{
				if((*(Item+12)) == DetOutsideCodeBuffer.Buffer[i])
				{
					return 1;
				}
			}	
		}
	}

	return 0;
}

