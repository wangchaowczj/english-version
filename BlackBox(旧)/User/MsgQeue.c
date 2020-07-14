#include "includes.h"

#define MSG_QEUE_SIZE   3

void * MsgQeueTb[MSG_QEUE_SIZE];//������Ϣָ�����飬���г���Ϊ10
OS_MEM * PartitionPt;//�����������ָ��
u8 Partition[MSG_QEUE_SIZE][sizeof(DET_DATA_STRUCT)];//������������ߴ磬ÿ��100�ֽڣ���10��


void MsgQeueInit(void)
{
    INT8U Err;

    MsgSem.MsgQeue = OSQCreate(&MsgQeueTb[0], MSG_QEUE_SIZE);//������Ϣ���У�����Ϊ10����Ϣ
    PartitionPt = OSMemCreate(Partition, MSG_QEUE_SIZE, sizeof(DET_DATA_STRUCT), &Err);//����������������ڱ�����Ϣ
}

DET_DATA_STRUCT* GetFreeDetDataStruct(void)
{
    INT8U Err;
    
    return (DET_DATA_STRUCT*)OSMemGet(PartitionPt, &Err);//����һ���ڴ��
}


void PutDetDataStruct(DET_DATA_STRUCT *DetDataStruct)
{
    OSMemPut(PartitionPt, DetDataStruct);//�ͷ���Ϣ��ռ�õ��ڴ��
}



