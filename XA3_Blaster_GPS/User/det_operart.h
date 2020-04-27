#ifndef _DET_OPERART_H_
#define _DET_OPERART_H_

#define BLAST_DELAY_WHOLE_ADD_VALUE			72	//��������һ������

#define DET_SERL_GROUP_INFO_MAX   32
typedef struct
{
	u16 Count;
	u8 Serl;
}DET_SERL_GROUP;
typedef struct
{
	u8 Count;
	DET_SERL_GROUP Group[DET_SERL_GROUP_INFO_MAX];
}DET_SERL_GROUP_INFO;
extern DET_SERL_GROUP_INFO DetSerlGroupInfo;

typedef enum 
{
	DET_PASS,			  //��⵽�׹�
	COMM_ERR, 	          //ͨ�ų���
	TEST_ERR,			  //BIT���� 
	SERL_NULL_ERR,        //�׹ܻ�û��д����
	SECU_CODE_ERR,		  //����У�����
	FREQ_MIN,			  //Ƶ�ʹ�С
	FREQ_MAX,			  //Ƶ�ʹ���
	OVERLAPED,			  //���߹�����������к��Ѿ�ע�� 
	TYPE_ERR,			  //�׹����ʹ���
	CURRENT_MIN,		  //����С
	CURRENT_MAX,		  //������
	CURRENT_UNSTABLE,      //�������ȶ�
    DET_CODE_ERR,          //���벻���Ϲ���2018��3��25��
    DET_FAC_CODE_ERR,     //�׹ܹ����������豸��һ��
    NEED_REPEAT           //��Ҫ����ע��
}CHECK_RESULT;

typedef enum
{
	TEST_END,             //�������
	UNREG_DET             //������ɣ���δע���׹�
}TEST_RESULT;


#define DET_MAX_SIZE         400       //�������׹�����
typedef struct
{
	u16 Name;//��Ӧ�Ŀ׺�,��1��ʼ
	u8  Inside;//��Ӧ�Ŀ��ڲ����,��1��ʼ
}HOLE;
typedef union
{
    u32 U32Value;
    u8  U8Buffer[4];
}U32_UNION;

typedef struct
{
	U32_UNION Serl;	 //������
    U32_UNION Pwd;	 //����	
	U32_UNION Freq;	 //Ƶ��	
	U32_UNION Delayclk;//����д��оƬ����������
    u16 Delay;   //����
	u8  Err;	 //�������
    u8 RetryErr; //���Դ������
	HOLE Hole;   //��Ӧ�Ŀ���Ϣ	
}DET_INFO;

typedef struct
{
	u16 RegCount;    /**<ע���׹���*/
	u16 ErrorCount;  /**<�����׹���*/	
	u16 DlyMax;      /**<�������ֵ*/
	u8 Type;         /**<�׹�����*/
	u8 HoleNumber[DET_MAX_SIZE];/**<ÿ���׵���ƿ����׹���*/
	DET_INFO Info[DET_MAX_SIZE];/**<�׹���Ϣ*/
}DET;

extern DET Det;


#define DET_ANALYST_READ_SN_MASK_BIT      	0x0001
#define DET_ANALYST_BIT_RESULT_MASK_BIT   	0x0002
#define DET_ANALYST_WRITE_DELAY_MASK_BIT  	0x0004
#define DET_ANALYST_FREQ_MASK_BIT         	0x0008
#define DET_ANALYST_UNLOCK_MASK_BIT       	0x0010      
#define DET_ANALYST_CHARGE_MASK_BIT       	0x0020


#define DET_TEST_FREQ_MIN_ERR      			0x01//Ƶ��С
#define DET_TEST_FREQ_MAX_ERR      			0x02//Ƶ�ʴ�
#define DET_TEST_CONNECT_ERR       			0x03//δ����
#define DET_TEST_BIT_ERR           			0x04//BIT����
#define DET_TEST_DELAY_CLOCK_ERR   			0x05//����ֵ��д��ֵ��һ��
#define DET_TEST_CHARGE_ERR        			0x06//������
#define DET_TEST_EXPEN_ERR         			0x07//׼���𱬴���
#define DET_TEST_PWD_ERR         			0x08//�������
#define DET_TEST_COMM_ERR                   0x09//ͨѶ����(�з����ź�,���޷�����)
#define DET_TEST_TYPE_ERR                   0x0A//���ʹ���

extern ErrorStatus XA3_DetInside2Outside(u32 Serl, u32 Pwd, u8* Ptr);
extern ErrorStatus XA3_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd);
extern u8 DetCodeRulePass(u32 Serl, u32 Pwd);
extern ErrorStatus CheckOverlap(const u8* Serl);
extern void AllReset(void);
extern void AllDisCharge(void);
extern ErrorStatus GetDetRegStatus(u16 Index);
extern void GetDetNum(void);
extern void TestInfoInit(void);
extern u8 GetDetType(u32 Serl, u32 Pwd);
extern u8 GetDetFactoryCode(u32 Serl, u32 Pwd);
extern u8 DetIsSingleCap(u8 Type);
extern CHECK_RESULT CheckDetFreq(u32 freq, u8 check_state);
extern ErrorStatus CheckDetType(u32 Serl, u32 Pwd);
extern u16 DetAnalyst(DET_INFO* det_buffer);
extern CHECK_RESULT CheckDet(DET_INFO* det_buffer, u8 Reg);
extern TEST_RESULT TestDetQuick(void);
extern void TestDetComplete(void);
extern void BlasteTestComplete(void);
extern u8 XA3_DetCodeWriteNew(U32_UNION* NewSerl, U32_UNION* NewPwd);
extern u8 XA3_DetCodeWriteFromBackup(void);

extern TEST_RESULT DetMaskAll(void);
extern void GetDetAckCurrentWave(void);
extern u16 GetRetryErrCount(void);
extern u8 HaveUnregisteredDetBySimpleSignal(void);
extern ErrorStatus AllDetStartBlast(void);
extern ErrorStatus CreateStartBlastTask(void);

#endif
