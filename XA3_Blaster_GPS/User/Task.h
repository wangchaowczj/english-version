#ifndef _TASK_H_
#define _TASK_H_

#include "GUI.h"
#include "WM.h"

typedef enum
{
	NOT_START = 0,//��û��ʼ������
	BLAST_ERROR,//���ִ���
	DET_TESTING,//���ڽ����׹ܲ���
	DET_TEST_END,//�׹ܲ������
	DET_CHARGING,//������·��� 
	DET_CHARGE_END,//��·������
	DET_BLASTING,//������
	DET_BLAST_END,//�����
    BLAST_DISABLE=0xFF//������ִ��������
}BLAST_STATUS;
//
typedef enum
{
	BLAST_INFO_VERIFY,
	DET_INFO_IN_OR_OUT,
	DET_INFO_INSERT
}BLUE_SLAVE_STATUS;

typedef struct
{
	HOLE StartHole;
	u16  StartDly;
}REG_DET_NET_PARAM;
extern REG_DET_NET_PARAM RegDetNetParam;

#define NET_PARAM_MAX 99
typedef struct
{
	u8  Count;
	u16 SpacesDly;
	u16 InsideSpacesDly;
}NET_PARAM;
extern NET_PARAM NetParam[NET_PARAM_MAX];

typedef struct
{
	u16 Value;
	u16 Max;
}PROGBAR_DATA;
typedef struct
{
	u8 Index;
	PROGBAR_DATA Bar[3];//Ŀǰһ���������ֻ��3��������
}PROGBAR;
typedef struct
{		
	GPS_INFO Gps;
	ErrorStatus SafeMonResult;
	ErrorStatus WifiResult;
	u16 overlaped;	
	u16 LinABVoltage;
	u16 LinABCurrent;
	U16 BatVoltage;
	DET_INFO RegDetInfo;
	u8 DetTestResult;//bit0 Ϊ1��ʾ��δע���׹�
	u8 DoubleKeySwitch;//˫������
	u16 DetVerifyErrIndex;//�����̶��׹�������֤ʱ���ֵķ���Ȩ�׹�����
	u8 AdjustStatus;
	u8 UploadDeviceInfoFlag;//0��ʾ�ȴ��ϴ��������ݣ�1��ʾ��Ҫ�ϴ���2��ʾ��ʼ�ϴ���3��ʾ�ϴ�����
	u8 IdleUploadBlastRecordFlag;//���ÿ����ϴ��𱬼�¼��־,0��ʾ��û�ϴ���1��ʾ�Ѿ��ϴ�
	u8 ChargeCurrentVaryFlag;//��¼��������仯��־
	u8 BlastVerifyResult;
	U8 Temp[100];
	BLAST_STATUS BlastStatus;
	u8 BlastInfoUpdateResult;
	u8 AppTemporaryTaskResult;
	u8 ConnectBlueIndex;
	BLUE_SLAVE_STATUS BlueSlaveStatus;
	u8 CurrentSlaveIndex;
	u8 DislaySlaveIndex;
	u8 SetSlaveIndex;
	u8 BlastRecorderDeviceUsages;
	u8 BlastRecorderDeviceName;
	u16 BlastRecorderDeviceRegNumber;
	u8 BlastRecorderSlaveNumber;
	u32 SystemIdleTime;
	u8 UpdateGuiFlag;
	u8 ForcePowerOffSwitch;
	u32 LinAdjustTime;//ϵͳʱ�䳬����ֵʱ����������˿ڵ�ѹ
	PROGBAR Progbar;
	u8 DelaySampleCfg;
	u8 DelaySampleFlag;
	u16 DelaySampleDelay;//�����׹���������ֵ
	u8 DlgParam;
	u32 InputValue1;
	u32 InputValue2;
	u8 BusMonitorDisable;
	u8 BusMonitorCfg;//1 ��ʾ�쳣������·ʱ���Զ��ָ�
    u8 BusMonitorDisableShow;
	u8 AutoUpdateStatus;//�Զ����¹̼�����״̬
    u8 BusCurrentOffAutoSave;//�Զ�������ߵ����Ƿ�û��
	u16 LinNormalLowVoltageGoal;
	u16 LinNormalHighVoltageGoal;
	u16 DacHvValueMin;//��¼�����ѹ������DA��Сֵ
	U16 DKeyVoltage;
    u8 AppLowTaskResult;
    u8 FindUnRegDetInfo;
    u8 WaitDetVerify;//����ģʽʱ������ǰ�������󶨣������APP��֤�����׹ܺ���ܽ���������
    DET_INFO UnRegDetInfo;
    u8 ScanIsZEBRA;//����ɨ��ͷ
    u8 BlastDeleteDet;//������ǰ������������ɾ�����׹�
    u8 MainMenuNow;//��ǰ��������
    u8 OfflineMonitorBlastProcess;//�������׹ܵ��߼��
}OP_DATA;

typedef struct
{
	OS_EVENT* usb_rcv;
	OS_EVENT* RS485;
}MSG_SEM;



extern STR_COMM  HidData ;
extern OP_DATA Op_Data;
extern MSG_SEM MsgSem;

extern void AppStartUpTask(void* p_arg);
extern void AppUserGUITask(void* p_arg);
extern void AppKeyScanTask(void* p_arg);
extern void AppUSBTask(void* p_arg);
extern void AppRS485Task(void* p_arg);
extern void AppGPSTask(void* p_arg);
extern void AppSafeMonitorTask(void* p_arg);
extern void DetAutoIdentifyTask(void* p_arg);
extern void AppDetOperateTask(void* p_arg);
extern void AppTemporaryTask(void* p_arg);
extern void AppBackgroundTask(void* p_arg);
extern void AppWifiTask(void* p_arg);
extern void AppSoundTask(void* p_arg);
extern void AppLowPriTask(void* p_arg);

#endif
