#ifndef _MSGQEUE_H_
#define _MSGQEUE_H_

typedef struct
{
	u8 Count;
	u8 Data[10];
}DET_CMD_STRUCT;
typedef struct
{
	u8 Count;
	u32 Data[4];
}DET_BLAST_CMD_STRUCT;
typedef union
{
    DET_CMD_STRUCT Cmd;
    DET_BLAST_CMD_STRUCT BlastCmd;
}DATA_UNION;
typedef struct
{
    enum
	{
		DET_CMD_DATA,
		DET_BLAST_CMD_DATA
	}Type;
    DATA_UNION Data;
}DET_DATA_STRUCT;

extern void MsgQeueInit(void);
extern DET_DATA_STRUCT* GetFreeDetDataStruct(void);
extern void PutDetDataStruct(DET_DATA_STRUCT *DetDataStruct);

#endif
