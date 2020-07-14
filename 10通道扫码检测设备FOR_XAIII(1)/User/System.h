#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MH10_V1.4_190527"	     //ϵͳ�汾��


//�׹����Ͷ���(��3bit)
#define DET_TYPE_XA_02      0x00	   //2��,˫����,�������߿ɱ��
#define DET_TYPE_XA_06      0x01	   //6��,˫����,�������߿ɱ��
#define DET_TYPE_XA_21      0x02	   //21��,˫����,�������߿ɱ��

//�����������
#define DET_DLY_MAX_XA_02       2000UL
#define DET_DLY_MAX_XA_06       6000UL
#define DET_DLY_MAX_XA_21       21000UL



typedef struct
{
	u8 name;
	u8 ID[32];    //�豸ID��
	u8 HW[32];    //Ӳ���汾��
//    u8 DetClass;  //0��ʾXA��1��ʾBJ
}DEVICE_INFO;

typedef enum
{
	FINISHED,//��Ʒ
	SEMIFINISHED,//���Ʒ
	DET_CODE_VERIFY,//������֤
	ENTER_FACTORY,//�볧��
}USE_STAGE_CFG;

typedef enum
{
	BEFORE,//ǰ����
	AFTER,//�����
}WRITE_BAR_MODE_CFG;

typedef enum
{
	FINISHED_RING,//�����
	NOT_RING,//����
	RIGHT_RING,//�ϸ���
	ERROR_RING//���ϸ���
}SOUND_CFG;
typedef enum
{
	DET_TEST_LCD_STATUS_ON,//���ʱ��Ļ��
	DET_TEST_LCD_STATUS_OFF//���ʱ��Ļ��
}DET_TEST_LCD_STATUS_CFG;

typedef enum
{
	ROUND_GENERAL,//��ͨԲ����
	DURABLE,//��ĥԲ����
	TUNNEL//���ר�ý���
}DET_LINE_TYPE_CFG;


typedef struct
{
	u8 DetType;
	USE_STAGE_CFG Stage;//��⻷��
	SOUND_CFG Sound;
	u8 LcdBright;
	DET_TEST_LCD_STATUS_CFG DetTestLcdStatus;
	u8 DetLineLength;//���߳���
	DET_LINE_TYPE_CFG DetLineType;
	u8 DetCodeWrite;//�����Ƿ�д�룬0��д��1д
	u8 FactoryCode;//��������
	u32 ProductTime;//����ʱ��,"20160627"
	u16 DetDelay;//�׹�����ֵ
	u8 CheckDelay;
	u8 DelayWrite;
	u8 Feature;//������
	u8 CheckDetCode;//�������
    u8 NetMode;//������������
	u16 NetModeFactoryCode;//����ģʽʱ����Ӧ�Ĺܳ�����(��ʾ�ĸ�������Э��)  
    u8 DetOutsideStartHead[8];//13λ�����ǰ8λ
    u32 DetOutsideStartBoxIndex;//13λ����ĺ�5λ
    u8 InBoxNumber;//��������
    u8 AutoBack;//������ʱ���Ƿ���Ҫ�ֶ�����(�����ڵ�ͨ��)
	u32 DetDetectorPassCount;
}USER_PARAM;

typedef struct
{
    const char* Name;
    const u8 Code;
}FACTORY_NAME_CFG;

#endif
