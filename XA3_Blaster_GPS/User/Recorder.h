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

#define LOG_RECORD_MAX_NUM 30 //��־�ļ��������
typedef struct
{
	u8 FileName[LOG_RECORD_MAX_NUM][6];
	u8 Number;
	u8 Select;
}LOG_RECORDER;
extern LOG_RECORDER LogRecorder;

#define BLASTER_RECORD_MAX_NUM 100 //�𱬼�¼�������
typedef struct
{
	u8 FileName[BLASTER_RECORD_MAX_NUM][12];
	u16 Number;
	u16 Select;
}BLAST_RECORDER;
extern BLAST_RECORDER BlastRecorder;

#define DKEY_DET_INFO_VERIFY_MASK_BIT     	0x0001  //��֤�׹����뻹��ֻ��֤����
#define DKEY_ARGUMENT_MASK_BIT            	0x0002  //����������ʱ�Ƿ����Զ����֤
#define DKEY_ARGUMENT_DEFAULT_MASK_BIT    	0x0004  //Զ����֤ͨ��ʧ��ʱĬ���Ƿ�ͨ��

typedef struct
{
	u8 Operator[9];		//����Ա��ţ�ASCII�룩
	u8 License[32];	    //��Ȩ��
	u8 Mac[32];		    //��ԿID�ţ�ASCII�룩
	u8 Version[32];		//��Կ����汾
	u8 HwVersion[32];	//��ԿӲ���汾
	u8 UploadType;      //�ϴ��������ͣ��ڹ���ϵͳ����Ȩʱд����Կ
	                    //��1��-�������� Ҫ�������ݡ�2��-�������ݣ���������3��-�����������ݣ�������
	u8 ArgumentType;    //Զ����֤��ʶ����1��Ϊ��ҪԶ����֤����2��Ϊ����ҪԶ����֤
	u8 DetVerifyType;   //�׹�Ϊ��֤��ʽ����1��Ϊ�׹�������Ȩ����2��Ϊ�׹�������Ȩ
	uint64_t CtrlBit;   //����λ
	INTERNET_ADDR Server;        //������
}DKEY_INFO;

typedef struct
{
	DKEY_INFO DkeyInfo;  	/**<��Կ��Ϣ*/
	RTC_Typedef Time;    	/**<��ʱ��*/
	u16 I_Leak;          	/**<����©����*/
	u16 V_BeforeTest;     	/**<����ǰ��ѹ*/
	u16 I_BeforeTest;     	/**<����ǰ����*/
	u16 V_BeforeCharge;  	/**<���ǰ��ѹ*/
	u16 I_BeforeCharge;   	/**<���ǰ����*/	
	u16 V_AfterCharge;  	/**<�����ѹ*/
	u16 I_AfterCharge;   	/**<�������*/
	u16 I_AfterChargeMin;   /**<���������Сֵ�����ڼ�¼��������*/
	u16 I_AfterChargeMax;   /**<����������ֵ�����ڼ�¼��������*/
	u16 V_BeforeFire;    	/**<��ǰ��ѹ*/
	u16 I_BeforeFire;    	/**<��ǰ����*/	
}BLAST_INFO;
extern BLAST_INFO BlastInfo;

#define BLAST_SINGLE_MONITOR_BUFFER_SIZE 120
typedef struct
{
	u16 Count;
	s16 LinABAdc[BLAST_SINGLE_MONITOR_BUFFER_SIZE];
}BLAST_SINGLE_MONITOR;
extern BLAST_SINGLE_MONITOR BlastSingleMonitor;


#define BLASTER_RECORD_FILE_PATH_NOT_SAFE_MONITOR     "0:�ǰ����𱬼�¼(MQ01)"
#define BLASTER_RECORD_FILE_PATH_SAFE_MONITOR         "0:�����𱬼�¼(MQ01)"
#define BLASTER_RECORD_FILE_PATH_OFFLINE              "0:�����𱬼�¼(MQ01)"
#define SLAVE_BLASTER_RECORD_FILE_PATH   "0:������������(MQ01)"

#define SYSTEM_OPERATE_INFO_PATH    "0:ϵͳ��־"
//#define SYSTEM_OPERATE_INFO_CURRENT_FILE  "0:ϵͳ��־/��ǰ.txt"
//#define SYSTEM_OPERATE_INFO_BACKUP_FILE   "0:ϵͳ��־/����.txt"
//#define SYSTEM_OPERATE_INFO_BACKUP        "ϵͳ��־/����.txt"
//#define SYSTEM_OPERATE_INFO_FILE_MAX_SIZE  104857600UL  //100M

#define DET_INFO_FILE_PATH  "0:�׹���Ϣ"
#define DET_INFO_FILE_NAME  "0:�׹���Ϣ/�׹���Ϣ.txt"
#define DET_INFO_BACKUP_FILE_NAME  "0:�׹���Ϣ/�׹���Ϣ(����).txt"

#define SLAVE_DET_INFO_FILE_PATH  "0:�������׹���Ϣ�ݴ�"

#define OTHER_FILE_PATH  "0:��������"
#define DL_ACCREDIT_INFO_FILE1_NAME  "0:��������/��Ȩ�ļ�1.bin"
#define DL_ACCREDIT_INFO_FILE2_NAME  "0:��������/��Ȩ�ļ�2.txt"
#define DL_ACCREDIT_INFO_FILE2_ONLY_NAME  "��Ȩ�ļ�2.txt"

#define DEV_GPS_INFO_FILENAME  "0:��������/GPS��λ��Ϣ.bin"

#define WIFI_CONFIG_PARAM_FILE_PATH   "0:WIFI"   /**<�����WIFI�ļ�·��*/
#define WIFI_CONFIG_PARAM_FILE_NAME   "wifi.txt"   /**<�����WIFI�ļ�·��*/

/*AT45�ļ�ϵͳ*/
#define USER_CONFIG_PARAM_FILE_PATH   "1:�û����ò���"   /**<��������ò����ļ�·��*/
#define USER_CONFIG_PARAM_FILE_NAME   "�û����ò���.txt"   /**<��������ò����ļ�·��*/

#define NET_PARAM_FILE_COMPLETE_PATH   "1:�û����ò���/��������.txt"   /**<����Ŀ��������ļ�·��*/
#define DET_FEATURE_FILE_COMPLETE_PATH   "1:�û����ò���/������.txt"   /**<����Ŀ��������ļ�·��*/
#define MASTER_TIME_SHARE_FILE_COMPLETE_PATH   "1:�û����ò���/������ʱ���.txt"   /**<����Ŀ��������ļ�·��*/

#define FACTORY_CFG_FILE_PATH   "1:�������ò���"
#define DEVICE_ID_FILE_NAME     "�豸ID.txt"   /**<*/
#define HW_VERSION_FILE_NAME    "Ӳ���汾.txt"   /**<*/
#define CTRL_BYTES_FILE_NAME    "���ò���.txt"   /**<*/
#define DET_CURRENT_FILE_NAME   "�����׹ܵ�����Χ.txt"   /**<*/
#define DET_FREQ_FILE_NAME      "�׹�Ƶ�ʷ�Χ.txt"   /**<*/
#define DEVICE_TYPE_FILE_NAME      "�豸����.txt"   /**<*/
#define DELAY_SAMPLE_FILE_NAME     "�������.txt"   /**<*/
#define SCAN_FILE_NAME          "ɨ��ģ������.txt"   /**<*/
#define FACTORY_CODE_SUPPORT_FILE_NAME          "������������.txt"   /**<*/

#define INTERNET_PARAM_FILE_PATH   "1:�������"
#define SAFE_MONITOR_INTERNET_PARAM_FILE_PATH     "����ϵͳ������.txt"

#define ADJUST_FILE_PATH       "1:ADУ׼����"
#define BAT_ADJUST_FILE_NAME   "��ص�ѹУ׼����.txt"   /**<*/
#define LINAB_ADJUST_FILE_NAME   "���ߵ�ѹУ׼����.txt"   /**<*/
#define CURRENT_LOW1_ADJUST_FILE_NAME   "��ѹ�µ���С����У׼����.txt"   /**<*/
#define CURRENT_LOW2_ADJUST_FILE_NAME   "��ѹ�µ���������У׼����.txt"   /**<*/
#define CURRENT_HIGH1_ADJUST_FILE_NAME   "��ѹ�µ���С����У׼����.txt"   /**<*/
#define CURRENT_HIGH2_ADJUST_FILE_NAME   "��ѹ�µ���������У׼����.txt"   /**<*/
#define ADJUST_TIME_FILE_NAME   "У׼ʱ��.txt"   /**<*/


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
