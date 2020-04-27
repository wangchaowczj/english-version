#ifndef _TASK_H_
#define _TASK_H_

#include "GUI.h"
#include "WM.h"

typedef enum
{
	NOT_START = 0,//还没开始起爆流程
	BLAST_ERROR,//出现错误
	DET_TESTING,//正在进行雷管测试
	DET_TEST_END,//雷管测试完成
	DET_CHARGING,//正在网路充电 
	DET_CHARGE_END,//网路充电完成
	DET_BLASTING,//正在起爆
	DET_BLAST_END,//起爆完成
    BLAST_DISABLE=0xFF//不允许执行起爆流程
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
	PROGBAR_DATA Bar[3];//目前一个界面最多只有3个进度条
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
	u8 DetTestResult;//bit0 为1表示有未注册雷管
	u8 DoubleKeySwitch;//双键开关
	u16 DetVerifyErrIndex;//起爆流程对雷管内码验证时出现的非授权雷管索引
	u8 AdjustStatus;
	u8 UploadDeviceInfoFlag;//0表示等待上传心跳数据，1表示需要上传，2表示开始上传，3表示上传结束
	u8 IdleUploadBlastRecordFlag;//利用空闲上传起爆记录标志,0表示还没上传，1表示已经上传
	u8 ChargeCurrentVaryFlag;//记录充电后电流变化标志
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
	u32 LinAdjustTime;//系统时间超过该值时才允许调整端口电压
	PROGBAR Progbar;
	u8 DelaySampleCfg;
	u8 DelaySampleFlag;
	u16 DelaySampleDelay;//抽检的雷管延期设置值
	u8 DlgParam;
	u32 InputValue1;
	u32 InputValue2;
	u8 BusMonitorDisable;
	u8 BusMonitorCfg;//1 提示异常，但短路时可自动恢复
    u8 BusMonitorDisableShow;
	u8 AutoUpdateStatus;//自动更新固件功能状态
    u8 BusCurrentOffAutoSave;//自动检测总线电流是否没有
	u16 LinNormalLowVoltageGoal;
	u16 LinNormalHighVoltageGoal;
	u16 DacHvValueMin;//记录充电后调压过程中DA最小值
	U16 DKeyVoltage;
    u8 AppLowTaskResult;
    u8 FindUnRegDetInfo;
    u8 WaitDetVerify;//级联模式时，如果是安管三码绑定，必需等APP验证所有雷管后才能进入充电流程
    DET_INFO UnRegDetInfo;
    u8 ScanIsZEBRA;//斑马扫码头
    u8 BlastDeleteDet;//起爆流程前的组网测试中删除了雷管
    u8 MainMenuNow;//当前是主界面
    u8 OfflineMonitorBlastProcess;//起爆流程雷管掉线监测
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
