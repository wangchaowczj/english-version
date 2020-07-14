#ifndef _TRANSCEIVER_H_
#define _TRANSCEIVER_H_


#define QUEUE_BUFFER_MAX			11//实际能保存的会少一个
#define QUEUE_ITEM_LENGTH           8
typedef struct
{
	u16 StartIndex;
	u16 EndIndex;
	u8 Buffer[QUEUE_BUFFER_MAX][QUEUE_ITEM_LENGTH];
}DET_INFO_QUEUE;


extern DET_INFO_QUEUE DetInfoQueue;

extern void SaveDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item);
extern ErrorStatus GetDetInfoQueueItem(DET_INFO_QUEUE* Queue, u8* Item);
extern u16 GetDetInfoQueueItemCount(DET_INFO_QUEUE* Queue);
extern void ClearDetInfoQueue(DET_INFO_QUEUE* Queue);
extern u8 DetInfoQueueItemIsExist(DET_INFO_QUEUE* Queue, u8* Item);

#endif
