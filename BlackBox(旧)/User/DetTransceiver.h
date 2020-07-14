#ifndef _DETTRANSCEIVER_H_
#define _DETTRANSCEIVER_H_


#define DET_RX_PORT             GPIOD
#define DET_RX_PORT_RCC         RCC_AHB1Periph_GPIOD
#define DET_RX_PIN              GPIO_Pin_9
#define GET_DET_RX_STATUS()     GPIO_ReadInputDataBit(DET_RX_PORT, DET_RX_PIN)

#define DET_TX_PORT             GPIOD
#define DET_TX_PORT_RCC         RCC_AHB1Periph_GPIOD
#define DET_TX_PIN              GPIO_Pin_8
#define DET_ACK_HIGH()          GPIO_SetBits(DET_TX_PORT, DET_TX_PIN)
#define DET_ACK_LOW()           GPIO_ResetBits(DET_TX_PORT, DET_TX_PIN) 
#define DET_ACK_TOGGLE()        GPIO_ToggleBits(DET_TX_PORT, DET_TX_PIN) 
#define DET_ACK_INIT()          DET_ACK_LOW()

#define DET_CMD_TIMER_FREQ    84000000


#define	T7000US			  587999UL			 	// 起爆信号低电平
#define	T3700US			  310799UL				// 起爆信号高电平
#define	T1000US	          83999UL
#define	T800US	          67199UL
#define	T500US	          41999UL			
#define T400US            33599	
#define	T100US			  8399			 	//
#define	T40US			  3359				//
#define	T10US			  839				//

#define TNUS(COUNT)       (84UL*(COUNT)-1)

#define DET_REC_BUFFER_MAX      4

typedef struct
{
	u8 Count;
	u32 Timer[DET_REC_BUFFER_MAX];
}DET_REC_BUFFER;

#define DET_CMD_BUFFER_MAX      9
typedef struct
{
	u8 Count;
    u8 Bit;
	u8 Data[DET_CMD_BUFFER_MAX];
}DET_CMD_BUFFER;

#define DET_ACK_BUFFER_MAX      3
typedef struct
{
	u8 Count;
    u8 Index;
    u8 Bit;
	u8 Data[DET_ACK_BUFFER_MAX];
}DET_ACK_BUFFER;

typedef struct
{
	enum
	{
        DET_UNKNOW,
		DET_CMD,
		DET_BLAST
	}Mode;	
	DET_REC_BUFFER  Rec;//接收到的信号
    DET_CMD_BUFFER Cmd;//接收到的命令
    DET_ACK_BUFFER Ack;//应答数据
}DET_PROTOCOL;	
extern DET_PROTOCOL DetProtocol;


extern void DetPinConfig(void) ;
extern void DetTimerConfig(void);
extern u8 GetEventBit(u8 Cmd);

extern void StartDetAck(void);
extern void DetRecStatusInit(void);

#endif
