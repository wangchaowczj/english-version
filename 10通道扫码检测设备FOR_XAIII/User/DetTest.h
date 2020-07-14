#ifndef _DETTEST_H_
#define _DETTEST_H_

#define DET_LV_CURRENT_TEST_MASK_BIT        	 		0x00000001UL  //��ѹ�¾�̬��������ѡ��λ
#define DET_BIT_TEST_MASK_BIT        			 		0x00000002UL  //BIT����ѡ��λ
#define DET_CHARGE_TEST_MASK_BIT        		 		0x00000004UL  //������ѡ��λ
#define DET_CHARGE_HV_CURRENT_TEST_MASK_BIT      		0x00000008UL  //�����ѹ��������ѡ��λ

typedef struct
{
	u16 det_lv_current_min;   		/**<��ѹ�µ�����Сֵ����λ0.01uA*/
	u16 det_lv_current_max;   		/**<��ѹ�µ������ֵ����λ0.01uA*/
	u16 det_lv_current_time_out;    /**<��ѹ�µ������Գ�ʱʱ�䣬��λ0.1��*/
	u16 det_hv_current_min;   		/**<��ѹ�µ�����Сֵ����λ0.01uA*/
	u16 det_hv_current_max;	  		/**<��ѹ�µ������ֵ����λ0.01uA*/
	u16 det_hv_current_time_out;	/**<��ѹ�µ������Գ�ʱʱ�䣬��λ0.1��*/
	u32 det_freq_min;         		/**<оƬƵ����Сֵ����λHZ*/
	u32 det_freq_max;         		/**<оƬƵ�����ֵ����λHZ*/	
	u32 test_ctrl;            		/**<���������λ*/    
}TEST_PARAM;

extern TEST_PARAM test_param;
extern TEST_PARAM test_param_cfg[];//0Ϊ���Ʒ��1Ϊ��Ʒ



#define DET_SINGLE_CAP_MASK_BIT   	0x04 //оƬ��������1λ��ʾ������




#define DET_PWD_FIX_VALUE   0xFFFE   //оƬ����̶�ֵ





typedef enum
{
	DET_PASS = 0x00,
	LV_CURRENT_LESS,               //��ѹ���������С
	LV_CURRENT_MORE,               //��ѹ�����������
	LV_REVERSE_CURRENT_LESS,       //��ѹ�·������С
	LV_REVERSE_CURRENT_MORE,	   //��ѹ�·��������
	DET_INFO_READ_ERR,             //�������ȡ���� 
	DET_FREQ_MIN,                  //Ƶ��С
	DET_FREQ_MAX,                  //Ƶ�ʴ�
	DET_TYPE_ERR,                  //�׹����ʹ���
	DET_BRIDGE_SHORT,              //��ͷ��·
	DET_BIT_STATUS_READ_ERR,       //��BITʱ״̬�ض�����
	DET_BIT_ERR,                   //BIT����	
	DET_NO_WORK_CAP,               //û�й�������
	DET_WRITE_DELAY_ERR,		   //д����ָ�����
	DET_PASSWORD_ERR,              //оƬ�������
	DET_CHARGE_STATUS_READ_ERR,    //����ʱ״̬�ض�����
	DET_CHARGE_ERR,                //������
	DET_NO_RESPONSE_EXPEN,         //׼����ָ����Ӧ����   
	DET_EXPEN_TIMERD_ERR,          //��׼����ʱ����ʱ��ض�����
	DET_EXPEN_ERR,                 //׼���𱬴���	
	HV_CURRENT_LESS,               //��ѹ���������С
	HV_CURRENT_MORE,               //��ѹ�����������
	HV_REVERSE_CURRENT_LESS,       //��ѹ�·������С
	HV_REVERSE_CURRENT_MORE,       //��ѹ�·��������
    LV_VOLTAGE_MIN,                //���ߵ�ѹʱ��ѹ����	
    LV_VOLTAGE_MAX,                //���ߵ�ѹʱ��ѹ����
    HV_VOLTAGE_MIN,                //���߸�ѹʱ��ѹ����	
    HV_VOLTAGE_MAX,                //���߸�ѹʱ��ѹ����
	DET_WR_CODE_NULL_ERR,          //д����ʱû������(�����߼�������)
	DET_WR_CODE_UNLOCK_PWD_ERR,    //д����ʱ���ܴ���
	DET_WR_CODE_UNLOCK_EEPROM_ERR, //д����ʱ��EEPROM����
	DET_WR_CODE_WR_FIXED_ERR,      //д����ʱд����ֵ����
	DET_WR_CODE_ERR,			   //дEEPROM����
	DET_WR_CODE_READ_ERR,		   //д�����ض����������
	DET_WR_CODE_VERIFY_ERR,        //д����������������д�벻һ��
	DET_CODE_REPEAT_TEST,          //�������Ѿ�������
	DET_FIX_DELAY_READ_ERR,        //����ʱ��ض�����
	DET_FIX_DELAY_NOT_EQUAL,       //����ʱ�����û����õĲ�һ��,���޵�����оƬ
	DET_WR_TYPE_ERR,               //�޸��׹����ʹ���
	DET_WR_TYPE_READ_ERR,          //�޸��׹����ͺ������������д�벻һ��
	DET_SHORT_ERR,			       //�׹ܶ�·
	DET_OPEN_CIRCUIT_ERR,          //�׹ܶ�·
	DET_CODE_ALL_ZERO,             //����ȫ0
	DET_CODE_NOT_SAME,             //�׹������벻һ��
    DET_BIT_NO_RESPONSE,           //BIT����û����Ӧ
    DET_CHARGE_NO_RESPONSE,        //�������û����Ӧ
    DET_PASSWORD_READ_ERR,         //PWD��ȡ����	
	DET_PASSWORD_NOT_ACK,          //���ͽ���ָ������Ӧ
    DET_CODE_RULE_ERR,             //���벻���Ϲ���(XA2)
    DET_READ_EEPROM_ERR,           //��EEPROM����
    DET_CODE_BACKUP_ERR1,          //DATA1-DATA4�����뱸�����ݴ���
    DET_CODE_BACKUP_ERR2,          //DATA5-DATA8���뱸�����ݴ���
    DET_TEST_RECORDER_DATA_ERR2,   //DATA9-DATA12���ݴ���
	DET_TEST_RECORDER_CRC_ERR,     //����¼У�����(XA2)        
    DET_TEST_RECORDER_ERR1,        //û�о���I�ͼ��(XA2)
    DET_TEST_RECORDER_ERR2,        //û�о���II�ͼ��(XA2)
	DET_TEST_RECORDER_ERR3,        //û�о������Ʒ���(XA2)
	DET_TEST_RECORDER_CRC_DIFFERENT_ERR,   //���ζ���¼��ͬ
	DET_TEST_RECORDER_REPEAT_ERR, //���Ʒ�ظ����(XA2)
//	DET_TEST_RECORDER_REPEAT_ERR2, //��Ʒ�ظ����(XA2)
	
	DET_TESTING = 0xFF             //������
}DET_ERR_CODE;

typedef struct
{
	DET_ERR_CODE Status;      //���Դ������
    u8 ser1;				 // ���к�1
	u8 ser2;				 // ���к�2
	u8 ser3;				 // ���к�2
	u8 fac;					 // ������
	u8 bat;				     // ����1
	u8 year;				 // ���
	u8 Pwd1;                 //����
	u8 Pwd2;                 //����
	u32 freq;                //Ƶ��,��λHz
	u8 type;                 //оƬ����
	u16 lv_current;          //��ѹ���������ֵ
	u16 lv_reverse_current;  //��ѹ�·������ֵ
	u16 hv_current;          //��ѹ���������ֵ
	u16 hv_reverse_current;	 //��ѹ�·������ֵ
}DET_INFO;
typedef union
{
    u32 U32Value;
    u8  U8Buffer[4];
}U32_UNION;
typedef struct
{
	DET_ERR_CODE Status;      //���Դ������
    U32_UNION Serl;	 //������
    U32_UNION Pwd;	 //����
	u32 freq;                //Ƶ��,��λHz
	u16 lv_current;          //��ѹ���������ֵ
	u16 lv_reverse_current;  //��ѹ�·������ֵ
	u16 hv_current;          //��ѹ���������ֵ
	u16 hv_reverse_current;	 //��ѹ�·������ֵ*
}XA2_DET_INFO;
#define DET_CHANNEL_MAX   10
typedef struct
{
	u8 Count;
//	DET_INFO Info[DET_CHANNEL_MAX];
    XA2_DET_INFO XA2_Info[DET_CHANNEL_MAX];	
	BARCODE BarCode[DET_CHANNEL_MAX];
	DET_ERR_CODE OldStatus[DET_CHANNEL_MAX];
    DET_ERR_CODE NewStatus[DET_CHANNEL_MAX];
}CHANNEL;
extern CHANNEL Channel;


#define DET_PWD_FIX_VALUE   0xFFFE   //оƬ����̶�ֵ

extern void ClearDetInfo(void);
extern void ClearNewStatus(void);
extern ErrorStatus WaitCurrentReady(u8 CurrentValue, u16 TimeOut);
extern ErrorStatus WaitRxPinReady(u16 TimeOut);
extern ErrorStatus WaitRxPinHigh(u16 TimeOut);
extern void AllChannelLinShortTest(u8 type);
extern void AllChannelOpenCircuitTest(u8 type);
extern u8 TestParamIsSet(u32 MaskBit);
extern void AllChannelCurrentTest(u8 ReverseFlag);
extern void AllChannelVoltageTest(u8 Flag);
extern void AllChannelDetFreqTest(void);
extern u8 AllChannelTestEnd(void);
extern u8 AllChannelTestEndFromBuffer(DET_ERR_CODE* Status);
extern void AllChannelDetHVCurrentTest(u8 ReverseFlag);
extern u8 GetChannelStatusCount(DET_ERR_CODE Status);
extern u8 GetChannelNewStatusCount(DET_ERR_CODE Status);
extern void AllChannelStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus);
extern void AllChannelNewStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus);
extern void SoundActionByStatus(void);
extern u32 GetDetFreqFromTimerCount(u16 count);
extern u8 AllChannelIsOpenCircuit(void);
extern void SetAllChannelBarCodeIdle(void);
//extern void XA_AllChannelDetCodeAllZeroVerify(void);
extern u8 DetCodeIsAllZero(DET_INFO* Det);

extern void DetTest(void);
extern u16 DetAnalyst(void);
extern u16 XA2_DetAnalyst(void);
extern void QueryDetConnectStatus(void);
extern void WaitAllChannelDetOff(void);

#endif
