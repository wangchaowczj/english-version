#ifndef _DETOUTSIDECODEQUEUE_H_
#define _DETOUTSIDECODEQUEUE_H_

#define DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX   10
typedef struct
{
	u8 Count;
	u8 StartWith[12];//Õ‚¬Î«∞12Œª
	u8 Buffer[DET_OUTSIDE_CODE_QUEUE_BUFFER_MAX];
}DET_OUTSIDE_CODE_BUFFER; 

extern DET_OUTSIDE_CODE_BUFFER DetOutsideCodeBuffer;

extern void SaveDetOutsideCodeQueueItem(u8* Item);
extern u8 DetOutsideCodeQueueIsSameStartWith(u8* Item);
extern u8 DetOutsideCodeQueueIsExist(u8* Item);

#endif

