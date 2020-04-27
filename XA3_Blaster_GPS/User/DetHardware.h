#ifndef _DETHARDWARE_H_
#define _DETHARDWARE_H_


#define BLAST_SIGNAL_SEND_TIMER                TIM2
#define BLAST_SIGNAL_SEND_TIMER_RCC            RCC_APB1Periph_TIM2



/***�ܽŶ���***/
#define DET_RECEIVE_RES_PORT          GPIOI
#define DET_RECEIVE_RES_PORT_RCC      RCC_AHB1Periph_GPIOI
#define DET_RECEIVE_RES_PIN           GPIO_Pin_9

#define BLAST_SIGNAL_SEND_PORT        GPIOI
#define BLAST_SIGNAL_SEND_PORT_RCC    RCC_AHB1Periph_GPIOI
#define BLAST_SIGNAL_SEND_PIN         GPIO_Pin_6

#define DET_CMD_SEND_PORT             GPIOI
#define DET_CMD_SEND_PORT_RCC         RCC_AHB1Periph_GPIOI
#define DET_CMD_SEND_PIN              GPIO_Pin_7

#define ADJUST_G_PORT                 GPIOE
#define ADJUST_G_PORT_RCC             RCC_AHB1Periph_GPIOE
#define ADJUST_G_PIN                  GPIO_Pin_0

#define DOUBLE_KEY1_SIM_SW_PORT            GPIOE
#define DOUBLE_KEY1_SIM_SW_PORT_RCC        RCC_AHB1Periph_GPIOE
#define DOUBLE_KEY1_SIM_SW_PIN             GPIO_Pin_3

#define DOUBLE_KEY2_SIM_SW_PORT            GPIOE
#define DOUBLE_KEY2_SIM_SW_PORT_RCC        RCC_AHB1Periph_GPIOE
#define DOUBLE_KEY2_SIM_SW_PIN             GPIO_Pin_4

#define DOUBLE_KEY_SW_PORT            GPIOE
#define DOUBLE_KEY_SW_PORT_RCC        RCC_AHB1Periph_GPIOE
#define DOUBLE_KEY_SW_PIN             GPIO_Pin_6


/*��Ϊ�ӻ�ʱģ��˫������ʹ�ܿ���*/
#define OPEN_DOUBLE_KEY1_SIM_SW()       GPIO_ResetBits(DOUBLE_KEY1_SIM_SW_PORT, DOUBLE_KEY1_SIM_SW_PIN)
#define CLOSE_DOUBLE_KEY1_SIM_SW()      GPIO_SetBits(DOUBLE_KEY1_SIM_SW_PORT, DOUBLE_KEY1_SIM_SW_PIN) 

#define OPEN_DOUBLE_KEY2_SIM_SW()       GPIO_ResetBits(DOUBLE_KEY2_SIM_SW_PORT, DOUBLE_KEY2_SIM_SW_PIN)
#define CLOSE_DOUBLE_KEY2_SIM_SW()      GPIO_SetBits(DOUBLE_KEY2_SIM_SW_PORT, DOUBLE_KEY2_SIM_SW_PIN) 

/*��Ϊ����ʱ˫������ʹ�ܿ���*/
#define OPEN_DOUBLE_KEY_SW()       GPIO_ResetBits(DOUBLE_KEY_SW_PORT, DOUBLE_KEY_SW_PIN)
#define CLOSE_DOUBLE_KEY_SW()      GPIO_SetBits(DOUBLE_KEY_SW_PORT, DOUBLE_KEY_SW_PIN)  


#define OPEN_DET_RECEIVE_RES()     GPIO_ResetBits(DET_RECEIVE_RES_PORT, DET_RECEIVE_RES_PIN)
#define CLOSE_DET_RECEIVE_RES()    GPIO_SetBits(DET_RECEIVE_RES_PORT, DET_RECEIVE_RES_PIN)
#define DET_RECEIVE_RES_ON         0
#define GET_DET_RECEIVE_RES_STATE()  GPIO_ReadOutputDataBit(DET_RECEIVE_RES_PORT, DET_RECEIVE_RES_PIN)

#define DET_COM_SEND_LOW()         GPIO_ResetBits(DET_CMD_SEND_PORT, DET_CMD_SEND_PIN)
#define DET_COM_SEND_HIGH()        GPIO_SetBits(DET_CMD_SEND_PORT, DET_CMD_SEND_PIN)
#define DET_COM_SEND_TOGGLE()      GPIO_ToggleBits(DET_CMD_SEND_PORT, DET_CMD_SEND_PIN)
#define DET_COM_SEND_PIN_INIT()    DET_COM_SEND_LOW()

#define ADJUST_G_LOW()             GPIO_ResetBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_HIGH()            GPIO_SetBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_TOGGLE()          GPIO_ToggleBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_RESET()           ADJUST_G_TOGGLE();\
                                   ADJUST_G_HIGH()

#define BLAST_SIGNAL_LOW()         GPIO_ResetBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_HIGH()        GPIO_SetBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_TOGGLE()      GPIO_ToggleBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_PIN_INIT()    BLAST_SIGNAL_LOW()

#define SET_CMD_SEND_STATE()       SET_LV_6_7V();\
								   CLOSE_DOUBLE_KEY_SW();\
                                   CLOSE_DET_RECEIVE_RES()
								   
#define SET_CMD_RECEIVE_STATE()    SET_LV_5_0V();\
								   CLOSE_DOUBLE_KEY_SW();\
							       OPEN_DET_RECEIVE_RES()


#define DET_CMD_TIMER_FREQ    84000000


#define	T7000US			  587999UL			 	// ���źŵ͵�ƽ
#define	T3700US			  310799UL				// ���źŸߵ�ƽ
#define	T1000US	          83999UL
#define	T800US	          67199UL
#define	T500US	          41999UL			
#define T400US            33599	
#define	T100US			  8399			 	//
#define	T40US			  3359				//
#define	T10US			  839				//

#define TNUS(COUNT)       (84UL*(COUNT)-1)

//�����׹ܷ�������ʱ��Ƶ�����Ʒ�Χ
#define DET_RECEIVE_FREQ_MIN   50000UL   //50K
#define DET_RECEIVE_FREQ_MAX   120000UL  //120K

#define DET_RECEIVE_HEAD_BEFORE_TIME    0x55
#define DET_RECEIVE_HEAD_AFTER_TIME     0x15

#define DET_FREQ_MEASURE_WRITE_VALUE    0x0000FE

/*оƬ�ڲ�ʱ�ӶԱ�׼������(1ms)����ֵ��������*/
#define DET_FIXED_PULSE_MIN_FOR_MS		 50
#define DET_FIXED_PULSE_MAX_FOR_MS		 120

typedef struct
{
	enum
	{
		DET_END,//Ĭ��Ϊ����״̬
		DET_SEND,//�ڷ���״̬�²������˿ڵ�ѹ����
		DET_DELAY_REC,
		DET_WAITING_REC,
		DET_TIMEOUT		
	}State;
	u16 Count;
	u16 Index;
	u32 Timer[164];//����������ÿ�ֽ���Ҫ��18�������������9�ֽڣ���ʼλ1�ֽڣ��ȴ�����1�ֽ� 164=1+ (8+1)*2*9+1
}DET_SEND_BUFFER;

#define DET_REC_DATA_MAX     10
#define DET_REC_BUFFER_MAX   100   //DET_REC_DATA_MAX*10
#define DET_SEND_CMD_MAX      10
typedef struct
{
	u8 Count;
	u32 Timer[DET_REC_BUFFER_MAX];
}DET_REC_BUFFER;

typedef struct
{
	u8 ExpentCount;
	u8 Data[DET_REC_DATA_MAX];
}DET_REC_DATA;

typedef struct
{
	u8 Count;
	u8 Buffer[DET_SEND_CMD_MAX];
}DET_SEND_CMD;

typedef struct
{
	enum
	{
		DET_CMD,
		DET_FIXED_PULSE,
		DET_BLAST,
		DET_CMD_BLAST
	}Mode;
	enum
	{
		HOOK_IDLE,
		HOOK_CHECK_IN
	}Hook;	
	DET_SEND_BUFFER SendBuffer;//Ҫ���͵�ԭʼ���ݾ�������������
	DET_REC_BUFFER  Rise;//RISE��������˲����ź�	
	DET_REC_BUFFER  Fall;//FALL��������˲����ź�
	DET_REC_BUFFER  RecBuffer;//��RISE��FALL����·�źźϳ�һ·�ź�
	DET_REC_DATA RecData;//���յ�������
	DET_SEND_CMD SendCmd;//Ҫ���͵�ԭʼ����
	DET_REC_BUFFER  RealRise;//RISEʵ�ʽ��յ����źţ����ڼ�¼��־����ͨѶ����	
	DET_REC_BUFFER  RealFall;//FALLʵ�ʽ��յ����źţ����ڼ�¼��־����ͨѶ����    
}DET_PROTOCOL;	
extern DET_PROTOCOL DetProtocol;

extern void OpenDetCmdDelayInterval(void);
extern void CloseDetCmdDelayInterval(void);
extern void DetHardwareInit(void);
extern void ExtiConfig(uint32_t EXTI_Line, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd);
extern u32 GetDetFreqFromHead(void);
extern u8 GetDetWriteDelayFLag(void);
extern u32 GetDetDelayClock(void);
extern u16 GetDetFixedDelay(void);
extern u32 GetDetFreqFromTimeRd(void);
extern ErrorStatus XA3_DetSendAndRec(u8 ErrorRetry, u8 RecBytes);
#endif

