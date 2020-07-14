#include "includes.h"

#define MSG_QEUE_SIZE   3

void * MsgQeueTb[MSG_QEUE_SIZE];//定义消息指针数组，队列长度为10
OS_MEM * PartitionPt;//定义内码分区指针
u8 Partition[MSG_QEUE_SIZE][sizeof(DET_DATA_STRUCT)];//定义内码分区尺寸，每块100字节，共10块


void MsgQeueInit(void)
{
    INT8U Err;

    MsgSem.MsgQeue = OSQCreate(&MsgQeueTb[0], MSG_QEUE_SIZE);//创建消息队列，长度为10条消息
    PartitionPt = OSMemCreate(Partition, MSG_QEUE_SIZE, sizeof(DET_DATA_STRUCT), &Err);//创建内码分区，用于保存消息
}

DET_DATA_STRUCT* GetFreeDetDataStruct(void)
{
    INT8U Err;
    
    return (DET_DATA_STRUCT*)OSMemGet(PartitionPt, &Err);//申请一个内存块
}


void PutDetDataStruct(DET_DATA_STRUCT *DetDataStruct)
{
    OSMemPut(PartitionPt, DetDataStruct);//释放消息所占用的内存块
}



