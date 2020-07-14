#ifndef _DETHARDWARE_H_
#define _DETHARDWARE_H_



//雷管通信信号发送引脚
#define DET_TX_PORT                    GPIOB
#define DET_TX_PORT_RCC                RCC_AHB1Periph_GPIOB
#define DET_TX_PIN                     GPIO_Pin_13
#define SET_LINA_H()                   GPIO_SetBits(DET_TX_PORT, DET_TX_PIN)
#define SET_LINA_L()                   GPIO_ResetBits(DET_TX_PORT, DET_TX_PIN)


//雷管通信信号发送引脚
#define DET_TX1_PORT                   GPIOB
#define DET_TX1_PORT_RCC               RCC_AHB1Periph_GPIOB
#define DET_TX1_PIN                    GPIO_Pin_14
#define SET_LINB_H()                   GPIO_ResetBits(DET_TX1_PORT, DET_TX1_PIN)
#define SET_LINB_L()                   GPIO_SetBits(DET_TX1_PORT, DET_TX1_PIN)

#define SET_LIN_AHBL()                 SET_LINA_H();SET_LINB_L()
#define SET_LIN_ALBH()                 SET_LINA_L();SET_LINB_H()
#define SET_LIN_SHORT()                SET_LINA_L();SET_LINB_L()
#define SET_LIN_REVERSE()              DET_TX_PORT->ODR ^=DET_TX_PIN;\
                                       DET_TX1_PORT->ODR ^=DET_TX1_PIN     
#define SET_LIN_OPEN()                 SET_LIN_AHBL()

//雷管通信信号接收引脚
#define DET_RX_PORT                    GPIOB
#define DET_RX_PORT_RCC                RCC_AHB1Periph_GPIOB
#define DET_RX_PIN                     GPIO_Pin_15
#define GET_RX_PIN()                   GPIO_ReadInputDataBit(DET_RX_PORT, DET_RX_PIN)
#define DET_RX_EXTI_LINE			   EXTI_Line15

//高压切换引脚
#define HV_SW_PORT                     GPIOF
#define HV_SW_PORT_RCC                 RCC_AHB1Periph_GPIOF
#define HV_SW_PIN                      GPIO_Pin_14
#define OPEN_HV_SW()                   GPIO_SetBits(HV_SW_PORT, HV_SW_PIN)
#define CLOSE_HV_SW()                  GPIO_ResetBits(HV_SW_PORT, HV_SW_PIN)
#define GET_HV_SW_STATUS()             GPIO_ReadOutputDataBit(HV_SW_PORT, HV_SW_PIN)

#define DET_CMD_TIMER_FREQ    84000000

#define	T7000US			  587999UL			 	// 起爆信号低电平
#define	T3700US			  310799UL				// 起爆信号高电平
#define	T1000US	          83999UL
#define	T800US	          67199UL
#define	T500US	          41999UL			
#define T400US            33599	
#define	T100US			  8399			 	//
#define	T50US			  4199				//
#define	T40US			  3359				//
#define	T10US			  839				//

#define TNUS(COUNT)       (84UL*(COUNT)-1)

//接收雷管返回数据时的频率限制范围
#define DET_RECEIVE_FREQ_MIN   50000UL   //50K
#define DET_RECEIVE_FREQ_MAX   120000UL  //120K

#define DET_RECEIVE_HEAD_BEFORE_TIME    0x55
#define DET_RECEIVE_HEAD_AFTER_TIME     0x15

#define DET_FREQ_MEASURE_WRITE_VALUE    0x0000FF

typedef enum
{
    XA2_DET
}DET_CLASS;

typedef struct
{
	enum
	{
		DET_SEND,
		DET_DELAY_REC,
		DET_WAITING_REC,
		DET_TIMEOUT,
		DET_END
	}State;
	u8 Count;
	u8 Index;
	u32 Timer[164];//发送数据中每字节需要用18个，最长的命令有9字节，起始位1字节，等待接收1字节 164=1+ (8+1)*2*9+1
}DET_SEND_BUFFER;

#define DET_REC_DATA_MAX     7
#define DET_REC_BUFFER_MAX   70   //DET_REC_DATA_MAX*10
typedef struct
{
	__IO u8 Count;
	u32 Timer[DET_REC_BUFFER_MAX];
}DET_REC_BUFFER;

typedef struct
{
	u8 ExpentCount;
	u8 Data[DET_REC_DATA_MAX];
}DET_REC_DATA;

typedef struct
{
	enum
	{
		DET_CMD,
		DET_FIXED_PULSE,
		DET_BLAST
	}Mode;
	DET_SEND_BUFFER SendBuffer;
	DET_REC_BUFFER  RecBuffer;
	DET_REC_DATA RecData;
}DET_PROTOCOL;	
extern DET_PROTOCOL DetProtocol;


extern void DetHardwareInit(void);
extern void ExtiConfig(uint32_t EXTI_Line, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd);
extern u32 GetDetFreqFromHead(void);
extern u32 GetDetFreqFromTimeRd(void);
extern u16 XA_GetDetFixedDelay(void);
extern ErrorStatus BJ_DetSendAndRec(u8 ErrorRetry, u8 RecBytes);
extern ErrorStatus XA_DetSendAndRec(u8 ErrorRetry, u8 RecBytes);
extern ErrorStatus XA2_DetSendAndRec(u8 ErrorRetry, u8 RecBytes);
#endif

