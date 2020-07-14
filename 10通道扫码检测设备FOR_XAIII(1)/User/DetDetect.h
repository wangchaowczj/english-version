#ifndef _DETDETECT_H_
#define _DETDETECT_H_


#define TIME2_FREQ         42000000UL
#define SLAVE_TIME_FREQ    42000000UL

#define DET_MAX_DELAY_MS   30000

typedef enum
{
	ALONE_MODE,//表示独立方式
	SLAVE_MODE,//带节点模式
	SAMPLE_MODE    //电子雷管起爆命令采样模式
}DETECT_TYPE;

typedef enum
{
	FIRST_CURRENT,//放电电流脉冲
	FIRST_VOLTAGE,//脉冲电平输入
	FIRST_CMD,    //起爆指令
	FIRST_LIGHT   //光电信号
}FIRST_TYPE;

typedef enum
{
	SECOND_LIGHT,    //光传感器
	SECOND_PUSH,     //振动传感器
	SECOND_OFF_TO_ON, //断通靶
	SECOND_TEST//主机发来的二靶测试信号
}SECOND_TYPE;
typedef struct
{
	DETECT_TYPE Mode;//抽检模式
	u8 DetType;//雷管类型,0表示电雷管，1表示导爆管雷管，2表示电子雷管，3表示导爆管
	u8 DetBatch[16];//生产批号
	u32 SampleCount;//样本数量
	u8 FileName[13];//
	FIRST_TYPE FirstTypeSel[4];	
	u8 BusCtrlState;//总线控制状态
}DET_DETECT;
extern volatile DET_DETECT DetDetect;


typedef enum
{
	TR_IDLE,   //未启用
	TR_WAITING,//正在等待信号
	TR_TIMEOUT,//已经超时
	TR_SUCCEED	//已经成功接收到信号
}TRIGGER_STATE;

typedef struct
{
	u32 DelayExpecte;//名义延期
	TRIGGER_STATE FirstState;//一靶信号状态
	u32 FirstTime;//一靶信号到达时的计数值
	TRIGGER_STATE SecondState;//二靶信号状态
	u32 SecondTime;//二靶信号到达时的计数值
	FIRST_TYPE FirstType;//一靶信号类型
	SECOND_TYPE SecondType;//二靶信号类型
	u32 TimeOutCount;//超时时间
	u8 DetectState;//0表示未完成检测
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

//电子雷管起爆信号电平设置
typedef struct
{
	u8 Count;
	u32 Width[RECEIVE_TIMER_MAX-1];//us
	u32 DelayExtra;//额外延时 us，0时刻不是边沿时有用
}BLAST_CMD;
extern BLAST_CMD BlastCmd;

//电子雷管起爆信号电平上下限对应的定时计数值
typedef struct
{
	RECEIVE_TIMER WidthLower;
	RECEIVE_TIMER WidthUpper ;
	u32 WidthMinTimer;
	u32 WidthMaxTimer;
	u32 DelayExtraTimer;//额外延时对应的定时器值
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
		DIS_TRIGGER,//禁止触发
		EN_TRIGGER,//允许触发
	}Status;
	u32 Timer;//触发时间
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
