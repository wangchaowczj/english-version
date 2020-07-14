#ifndef _DETDETECT_H_
#define _DETDETECT_H_


#define TIME2_FREQ         42000000UL
#define SLAVE_TIME_FREQ    42000000UL

#define DET_MAX_DELAY_MS   30000

typedef enum
{
	ALONE_MODE,//��ʾ������ʽ
	SLAVE_MODE,//���ڵ�ģʽ
	SAMPLE_MODE    //�����׹����������ģʽ
}DETECT_TYPE;

typedef enum
{
	FIRST_CURRENT,//�ŵ��������
	FIRST_VOLTAGE,//�����ƽ����
	FIRST_CMD,    //��ָ��
	FIRST_LIGHT   //����ź�
}FIRST_TYPE;

typedef enum
{
	SECOND_LIGHT,    //�⴫����
	SECOND_PUSH,     //�񶯴�����
	SECOND_OFF_TO_ON, //��ͨ��
	SECOND_TEST//���������Ķ��в����ź�
}SECOND_TYPE;
typedef struct
{
	DETECT_TYPE Mode;//���ģʽ
	u8 DetType;//�׹�����,0��ʾ���׹ܣ�1��ʾ�������׹ܣ�2��ʾ�����׹ܣ�3��ʾ������
	u8 DetBatch[16];//��������
	u32 SampleCount;//��������
	u8 FileName[13];//
	FIRST_TYPE FirstTypeSel[4];	
	u8 BusCtrlState;//���߿���״̬
}DET_DETECT;
extern volatile DET_DETECT DetDetect;


typedef enum
{
	TR_IDLE,   //δ����
	TR_WAITING,//���ڵȴ��ź�
	TR_TIMEOUT,//�Ѿ���ʱ
	TR_SUCCEED	//�Ѿ��ɹ����յ��ź�
}TRIGGER_STATE;

typedef struct
{
	u32 DelayExpecte;//��������
	TRIGGER_STATE FirstState;//һ���ź�״̬
	u32 FirstTime;//һ���źŵ���ʱ�ļ���ֵ
	TRIGGER_STATE SecondState;//�����ź�״̬
	u32 SecondTime;//�����źŵ���ʱ�ļ���ֵ
	FIRST_TYPE FirstType;//һ���ź�����
	SECOND_TYPE SecondType;//�����ź�����
	u32 TimeOutCount;//��ʱʱ��
	u8 DetectState;//0��ʾδ��ɼ��
	u8 Version[32];
	u8 HW[32];
	u16 Power5V;
	u16 Power12V;
}ALONE;
extern volatile ALONE Alone;

#define RECEIVE_TIMER_MAX  51
typedef struct
{
	u8 Count;
	u32 Timer[RECEIVE_TIMER_MAX];
}RECEIVE_TIMER;
typedef struct
{
	RECEIVE_TIMER First;
	RECEIVE_TIMER Second;
}DELAY_ANALYST;

extern volatile DELAY_ANALYST DelayAnalyst;

//�����׹����źŵ�ƽ����
typedef struct
{
	u8 Count;
	u32 Width[RECEIVE_TIMER_MAX-1];//us
	u32 DelayExtra;//������ʱ us��0ʱ�̲��Ǳ���ʱ����
}BLAST_CMD;
extern BLAST_CMD BlastCmd;

//�����׹����źŵ�ƽ�����޶�Ӧ�Ķ�ʱ����ֵ
typedef struct
{
	RECEIVE_TIMER WidthLower;
	RECEIVE_TIMER WidthUpper ;
	u32 WidthMinTimer;
	u32 WidthMaxTimer;
	u32 DelayExtraTimer;//������ʱ��Ӧ�Ķ�ʱ��ֵ
}BLAST_CMD_LIMIT;
extern BLAST_CMD_LIMIT BlastCmdLimit;;

#define WAVE_ADC_MAX  250
typedef struct
{
	u16 Buffer[WAVE_ADC_MAX];	
	u16 Index;
}ADC_BUF;
typedef enum
{
	CHANNEL_A,
	CHANNEL_B
}WAVE_SRC;


typedef struct
{
	enum
	{
		DIS_TRIGGER,//��ֹ����
		EN_TRIGGER,//������
	}Status;
	u32 Timer;//����ʱ��
	WAVE_SRC Src;
}WAVE_TRIGGER;

typedef struct
{
	ADC_BUF Before;
	ADC_BUF Later;
	WAVE_TRIGGER Trigger;
}WAVE_ADC;
extern volatile WAVE_ADC WaveAdc;

extern u32 GetTimer2CountFromAloneSpecifiedDelayus(u32 Delay);
extern uint64_t GetDelaynsFromAloneSpecifiedTimer2(u32 Count);
extern uint64_t GetDelaynsFromAloneFirstSecondSignal(u32 FirstTime, u32 SecondTime);
extern u32 GetTimer2CountFromSlaveSpecifiedDelayus(u32 Delay);
extern uint64_t GetDelaynsFromSlaveSpecifiedTimer2(u32 Count);
extern uint64_t GetDelaynsFromSlaveFirstSecondSignal(u32 FirstTime, u32 SecondTime);
extern void SlaveTriggerInfoInit(void);
extern void TriggerSignalAloneStart(void);
extern u8 GetDetDetectAloneState(void);
extern void TriggerSignalStop(void);
extern void CmdSampleStart(void);

extern void TriggerSignalSlaveStart(void);
extern void GetDetDetectSlaveFirstState(void);

extern ErrorStatus GetFirstCmdTriggerTime(u32* Time);
extern void WaveAdcRecorder(WAVE_ADC* WaveStrPrt, u16 AdcValue);

#endif
