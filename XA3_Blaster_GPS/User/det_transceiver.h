#ifndef _DET_TRANSCEIVER_H_
#define _DET_TRANSCEIVER_H_

/*TIM3ʵ��Ƶ��*/
#define TIME3_FREQ   84000000

/**tim2ʵ��Ƶ��Ϊ84M����ʱ4.125ms��ӦоƬƵ��40kʱ165��ʱ��**/
#define TIME2_DET_165CLK    346500UL


//��168Mʱ�ӽ��м���
#define DET_SEND_TIMER                TIM8
#define DET_SEND_TIMER_RCC            RCC_APB2Periph_TIM8
#define DET_SEND_TIMER_100US_VALUE    (16800-1)
#define DET_SEND_TIMER_50US_VALUE     (8400-1)  

#define WAVE_WIDTH_100US  DET_SEND_TIMER_100US_VALUE
#define WAVE_WIDTH_50US	  DET_SEND_TIMER_50US_VALUE

#define EB_DET_TRANSCEIVER_SUCCESS 0     //�շ��ɹ� 
#define EB_DET_TRANSCEIVER_FAIL    1	 //�շ�ʧ��

//��168Mʱ�ӽ��м���
#define BLAST_SIGNAL_SEND_TIMER                TIM2
#define BLAST_SIGNAL_SEND_TIMER_RCC            RCC_APB1Periph_TIM2
#define BLAST_SIGNAL_SEND_TIMER_25MS_VALUE     (2100000-1)
#define BLAST_SIGNAL_SEND_TIMER_10_2MS_VALUE   (856800-1) 



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
#define DET_COM_SEND_PIN_INIT()    DET_COM_SEND_LOW() //���ų�ʼ��ƽ

#define ADJUST_G_LOW()             GPIO_ResetBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_HIGH()            GPIO_SetBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_TOGGLE()          GPIO_ToggleBits(ADJUST_G_PORT, ADJUST_G_PIN)
#define ADJUST_G_RESET()           ADJUST_G_TOGGLE();\
                                   ADJUST_G_HIGH()

#define BLAST_SIGNAL_LOW()         GPIO_ResetBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_HIGH()        GPIO_SetBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_TOGGLE()      GPIO_ToggleBits(BLAST_SIGNAL_SEND_PORT, BLAST_SIGNAL_SEND_PIN)
#define BLAST_SIGNAL_PIN_INIT()    BLAST_SIGNAL_LOW() //���ų�ʼ��ƽ

#define SET_CMD_SEND_STATE()       SET_LV_6_7V();\
								   CLOSE_DOUBLE_KEY_SW();\
                                   CLOSE_DET_RECEIVE_RES()
								   
#define SET_CMD_RECEIVE_STATE()    SET_LV_5_0V();\
								   CLOSE_DOUBLE_KEY_SW();\
							       OPEN_DET_RECEIVE_RES()
								   


//�����׹ܷ�������ʱ��Ƶ�����Ʒ�Χ
#define DET_RECEIVE_FREQ_MIN   40000UL   //40K
#define DET_RECEIVE_FREQ_MAX   120000UL  //120K

typedef struct
{
	u16 send_index;          //������������
	u16 send_length;         //������ͳ���
	u16 send_data[170];      //�������������
	u32 rise_receive[40];    //���������ص���ʱ�ļ���ֵ
	u32 fall_receive[40];    //�����½��ص���ʱ�ļ���ֵ
	u8  receive_rise_length; //������ʵ�ʲ���ĸ���
	u8  receive_fall_length ;//�½���ʵ�ʲ���ĸ���
	u8  receive_edg_max_num ;//���������ո���
	u8  receive_bytes ;      //Ҫ���յ��ֽ���
	u8  receive_state;       //������ɱ�־
	u8  enable_receive;      //������ձ�־
	u8 blast_signal_flag;    //�������źű�־
	u8 blast_signal_toggle_count;	 //���ź��������
}DET_COMM;
extern DET_COMM det_comm;

extern void DetSendConfig(void) ;
extern void DetReceiverConfig(void) ;
extern void DetTransmit(const u8* cmd_ptr, const u8 length);
extern void SendDetBlastSignal(void);
extern u8 DetReceiver(u8* buf, const u8 mode);

#endif
