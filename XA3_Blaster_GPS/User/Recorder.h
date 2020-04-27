#ifndef _RECORDER_H_
#define _RECORDER_H_



typedef enum
{
	NEW_STR_INFO = 0,
	STR_INFO,
	HEX_ARRAY,
	U16_DATA,
	TEST_INFO,
	TASK_DATA,
	BLAST_SINGLE_DATA,
	DEV_START,
	H32_ARRAY,
	CURRENT_STATUS,
	HEX_ARRAY_NO_TITLE,
    BLAST_RECORDER_COPY
}DATE_WRITE_TYPE;

#define LOG_RECORD_MAX_NUM 30 //日志文件最大条数
typedef struct
{
	u8 FileName[LOG_RECORD_MAX_NUM][6];
	u8 Number;
	u8 Select;
}LOG_RECORDER;
extern LOG_RECORDER LogRecorder;

#define BLASTER_RECORD_MAX_NUM 100 //起爆记录最大条数
typedef struct
{
	u8 FileName[BLASTER_RECORD_MAX_NUM][12];
	u16 Number;
	u16 Select;
}BLAST_RECORDER;
extern BLAST_RECORDER BlastRecorder;

#define DKEY_DET_INFO_VERIFY_MASK_BIT     	0x0001  //验证雷管内码还是只验证数量
#define DKEY_ARGUMENT_MASK_BIT            	0x0002  //进入起爆流程时是否进行远程论证
#define DKEY_ARGUMENT_DEFAULT_MASK_BIT    	0x0004  //远程论证通信失败时默认是否通过

typedef struct
{
	u8 Operator[9];		//操作员编号（ASCII码）
	u8 License[32];	    //授权码
	u8 Mac[32];		    //密钥ID号（ASCII码）
	u8 Version[32];		//密钥软件版本
	u8 HwVersion[32];	//密钥硬件版本
	u8 UploadType;      //上传数据类型，在管理系统中授权时写入秘钥
	                    //“1”-正常数据 要存入数据“2”-开发数据（开发）“3”-爆破试验数据（工厂）
	u8 ArgumentType;    //远程认证标识，“1”为需要远程论证，“2”为不需要远程论证
	u8 DetVerifyType;   //雷管为验证方式，“1”为雷管数量授权，“2”为雷管内码授权
	uint64_t CtrlBit;   //控制位
	INTERNET_ADDR Server;        //服务器
}DKEY_INFO;

typedef struct
{
	DKEY_INFO DkeyInfo;  	/**<密钥信息*/
	RTC_Typedef Time;    	/**<起爆时间*/
	u16 I_Leak;          	/**<总线漏电流*/
	u16 V_BeforeTest;     	/**<测试前电压*/
	u16 I_BeforeTest;     	/**<测试前电流*/
	u16 V_BeforeCharge;  	/**<充电前电压*/
	u16 I_BeforeCharge;   	/**<充电前电流*/	
	u16 V_AfterCharge;  	/**<充电后电压*/
	u16 I_AfterCharge;   	/**<充电后电流*/
	u16 I_AfterChargeMin;   /**<充电后电流最小值，用于记录电流波动*/
	u16 I_AfterChargeMax;   /**<充电后电流最大值，用于记录电流波动*/
	u16 V_BeforeFire;    	/**<起爆前电压*/
	u16 I_BeforeFire;    	/**<起爆前电流*/	
}BLAST_INFO;
extern BLAST_INFO BlastInfo;

#define BLAST_SINGLE_MONITOR_BUFFER_SIZE 120
typedef struct
{
	u16 Count;
	s16 LinABAdc[BLAST_SINGLE_MONITOR_BUFFER_SIZE];
}BLAST_SINGLE_MONITOR;
extern BLAST_SINGLE_MONITOR BlastSingleMonitor;


#define BLASTER_RECORD_FILE_PATH_NOT_SAFE_MONITOR     "0:非安管起爆记录(MQ01)"
#define BLASTER_RECORD_FILE_PATH_SAFE_MONITOR         "0:安管起爆记录(MQ01)"
#define BLASTER_RECORD_FILE_PATH_OFFLINE              "0:离线起爆记录(MQ01)"
#define SLAVE_BLASTER_RECORD_FILE_PATH   "0:译码器起爆数据(MQ01)"

#define SYSTEM_OPERATE_INFO_PATH    "0:系统日志"
//#define SYSTEM_OPERATE_INFO_CURRENT_FILE  "0:系统日志/当前.txt"
//#define SYSTEM_OPERATE_INFO_BACKUP_FILE   "0:系统日志/备份.txt"
//#define SYSTEM_OPERATE_INFO_BACKUP        "系统日志/备份.txt"
//#define SYSTEM_OPERATE_INFO_FILE_MAX_SIZE  104857600UL  //100M

#define DET_INFO_FILE_PATH  "0:雷管信息"
#define DET_INFO_FILE_NAME  "0:雷管信息/雷管信息.txt"
#define DET_INFO_BACKUP_FILE_NAME  "0:雷管信息/雷管信息(备份).txt"

#define SLAVE_DET_INFO_FILE_PATH  "0:译码器雷管信息暂存"

#define OTHER_FILE_PATH  "0:其他数据"
#define DL_ACCREDIT_INFO_FILE1_NAME  "0:其他数据/授权文件1.bin"
#define DL_ACCREDIT_INFO_FILE2_NAME  "0:其他数据/授权文件2.txt"
#define DL_ACCREDIT_INFO_FILE2_ONLY_NAME  "授权文件2.txt"

#define DEV_GPS_INFO_FILENAME  "0:其他数据/GPS定位信息.bin"

#define WIFI_CONFIG_PARAM_FILE_PATH   "0:WIFI"   /**<保存的WIFI文件路径*/
#define WIFI_CONFIG_PARAM_FILE_NAME   "wifi.txt"   /**<保存的WIFI文件路径*/

/*AT45文件系统*/
#define USER_CONFIG_PARAM_FILE_PATH   "1:用户配置参数"   /**<保存的配置参数文件路径*/
#define USER_CONFIG_PARAM_FILE_NAME   "用户配置参数.txt"   /**<保存的配置参数文件路径*/

#define NET_PARAM_FILE_COMPLETE_PATH   "1:用户配置参数/孔网参数.txt"   /**<保存的孔网参数文件路径*/
#define DET_FEATURE_FILE_COMPLETE_PATH   "1:用户配置参数/特征号.txt"   /**<保存的孔网参数文件路径*/
#define MASTER_TIME_SHARE_FILE_COMPLETE_PATH   "1:用户配置参数/主机分时检测.txt"   /**<保存的孔网参数文件路径*/

#define FACTORY_CFG_FILE_PATH   "1:出厂配置参数"
#define DEVICE_ID_FILE_NAME     "设备ID.txt"   /**<*/
#define HW_VERSION_FILE_NAME    "硬件版本.txt"   /**<*/
#define CTRL_BYTES_FILE_NAME    "配置参数.txt"   /**<*/
#define DET_CURRENT_FILE_NAME   "单发雷管电流范围.txt"   /**<*/
#define DET_FREQ_FILE_NAME      "雷管频率范围.txt"   /**<*/
#define DEVICE_TYPE_FILE_NAME      "设备类型.txt"   /**<*/
#define DELAY_SAMPLE_FILE_NAME     "抽检配置.txt"   /**<*/
#define SCAN_FILE_NAME          "扫码模块配置.txt"   /**<*/
#define FACTORY_CODE_SUPPORT_FILE_NAME          "工厂代码配置.txt"   /**<*/

#define INTERNET_PARAM_FILE_PATH   "1:网络参数"
#define SAFE_MONITOR_INTERNET_PARAM_FILE_PATH     "安管系统服务器.txt"

#define ADJUST_FILE_PATH       "1:AD校准参数"
#define BAT_ADJUST_FILE_NAME   "电池电压校准参数.txt"   /**<*/
#define LINAB_ADJUST_FILE_NAME   "总线电压校准参数.txt"   /**<*/
#define CURRENT_LOW1_ADJUST_FILE_NAME   "低压下电流小量程校准参数.txt"   /**<*/
#define CURRENT_LOW2_ADJUST_FILE_NAME   "低压下电流大量程校准参数.txt"   /**<*/
#define CURRENT_HIGH1_ADJUST_FILE_NAME   "高压下电流小量程校准参数.txt"   /**<*/
#define CURRENT_HIGH2_ADJUST_FILE_NAME   "高压下电流大量程校准参数.txt"   /**<*/
#define ADJUST_TIME_FILE_NAME   "校准时间.txt"   /**<*/


extern void FileSystemInitOS(void);
extern void FileSystemLock(void);
extern void FileSystemUnlock(void);

extern ErrorStatus FileIsExist(char *FilePathName);
extern ErrorStatus GetFileSize(char *FilePathName, u32* size);
extern void ReadFactoryConfigParam(void);
extern ErrorStatus ConfigFileVerify(u8* PathName, u8* FileName, u8 DeleteFlag);
extern ErrorStatus GetConfigFileString(u8* KeyName, u8* ReturnedString, u16 nSize, u8* PathName, u8* FileName);
extern u32 GetConfigFileU32(u8* KeyName,u32 nDefault, u8* PathName, u8* FileName);
extern ErrorStatus CreateNewFileData(u8* PathName, u8* FileName, u8* FileData, u16 Bytes);
extern ErrorStatus ReadSpecifiedFileData(u8* PathName, u8* FileName, u8* FileData, u16 Bytes);
extern ErrorStatus GetSpecifiedFileExistState(u8* PathName, u8* FileName);
extern ErrorStatus DeleteAllDetInfo(void);
extern ErrorStatus DeleteAllErrorDetInfo(void);
extern ErrorStatus DeleteDetInfo(HOLE* StartHole, HOLE* EndHole);
extern ErrorStatus DeleteDetInfoNotChangeRank(HOLE* StartHole, HOLE* EndHole);
extern void DeleteDetInfoNotSave(HOLE* StartHole, HOLE* EndHole);
extern ErrorStatus SaveAllDetInfo(u8  Type);
extern ErrorStatus ReadAllDetInfo(u8  Type);
extern ErrorStatus SaveAllDetInfoNotChangeRank(u8  Type);
extern ErrorStatus SaveNetParam(void);
extern void ReadNetParam(void);
extern ErrorStatus SaveUserConfigParam(void);
extern void ReadUserConfigParam(void);
extern ErrorStatus ReadOldGPSInfo(GPS_INFO* ptr);
extern ErrorStatus SaveGPSInfo(GPS_INFO* ptr);
extern void SaveBlasteFireTime(void);
extern ErrorStatus WriteBlasterRecord(void);
extern void DeleteAllFileSpecifiedPath(char* Path, char* ExtraFileName);
extern void BlastRecordFileInit(void);
extern void LogRecordFileInit(void);
extern ErrorStatus ReadSpecifiedHexFileSize(u8* PathName, u8* Version, u32* FileSize);
extern ErrorStatus WriteHexFileData(u8* PathName, u8* Version, u8* FileData, u16 Bytes);
extern ErrorStatus GetSpecifiedHexFileCRCValue(u8* PathName, u8* Version, u16* CRCValue);
extern ErrorStatus UpdateBlasterRecordUploadState(u8 State, u16 Index);
extern ErrorStatus WriteSystemLog(void* data, u16 len, DATE_WRITE_TYPE op_type);
extern u32 GetLogFileSize(u8 index);
extern ErrorStatus SaveSlaveDetInfo(u8 SlaveIndex);
extern ErrorStatus ReadSlaveDetInfo(u8 SlaveIndex);
extern ErrorStatus FileSystemTest(unsigned char disk);
extern u8 GetBlastRecorderDeviceUsages(u16 index);
extern u8 GetBlastRecorderDeviceName(u16 index);
extern u8 InserHole(u16 HoleName, u16 AddDelay);
extern u8 HoleMove(u16 StartHole, u16 EndHole, s16 Pos, s16 Delay);
extern u16 GetBlastRecorderIndexFromFileName(u8* FileName);
extern u32 GetBlastRecorderFileSize(u16 FileIndex);
extern ErrorStatus GetBlastRecorderData(u16 FileIndex, u32 ReadIndex, u16 ReadSize, u8* Ptr);
extern ErrorStatus GetLogRecorderData(u16 FileIndex, u32 ReadIndex, u16 ReadSize, u8* Ptr);
extern u16 GetLogRecorderIndexFromFileName(u8* FileName);
extern u32 GetLogRecorderFileSize(u16 FileIndex);
extern u8 WifiConfigIsOpen(void);
extern ErrorStatus SaveWifiInfo(u8* ssid, u8* passwords);
extern ErrorStatus ReadWifiInfo(u8* ssid, u8* passwords);
extern ErrorStatus BlastSingleSendedResult(void);
extern ErrorStatus SaveDetFeature(u8 Feature);
extern u8 GetDetFeatureDefult(void);
extern ErrorStatus WriteAccreditInfoFileForDL(u8 fileIndex, u32 StartIndex, u16 Bytes, u8* FileData);
extern u8 GetAccreditDetInfoForDL(u8* UIDStr, u8* YXQ);
extern u8 HasAccreditInfoFileForDL(void);
extern void DeleteAllAccreditErrorFile(void);
extern ErrorStatus WriteAccreditErrorFile(u8 FileIndex, u8* FileData, u8 Bytes);
extern ErrorStatus ReadAccreditErrorFile(u8 FileIndex, u16 StartIndex, u8 Count, u8* FileData);
extern u16 GetAccreditErrorCount(u8 FileIndex);
extern u16 GetAllAccreditErrorCount(void);
extern u8 GetAccreditErrorFileCount(void);
extern u8 GetAccreditErrorFileIndex(u8 ErrorIndex);
extern ErrorStatus GetLocationFromAccreditFile(void);
extern u8 AccreditAreaVerifyForDL(void);
extern ErrorStatus SaveMasterTimeShareTest(u8 TimeShare);
extern u8 GetMasterTimeShareTest(void);

#endif
