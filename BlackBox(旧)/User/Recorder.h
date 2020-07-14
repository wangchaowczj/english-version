#ifndef _RECORDER_H_
#define _RECORDER_H_

typedef enum
{
	NEW_STR_INFO = 0,
	STR_INFO,
	HEX_ARRAY,
	U16_DATA,
	DET_DATA_TYPE,
	DEV_START,
	H32_ARRAY,
	CURRENT_STATUS,
	HEX_ARRAY_NO_TITLE
}DATE_WRITE_TYPE;

#define LOG_RECORD_MAX_NUM 30 //日志文件最大条数
typedef struct
{
	u8 FileName[LOG_RECORD_MAX_NUM][6];
	u8 Number;
	u8 Select;
}LOG_RECORDER;
extern LOG_RECORDER LogRecorder;

#define SYSTEM_OPERATE_INFO_PATH    "0:系统日志"

extern void FileSystemInitOS(void);
extern void FileSystemLock(void);
extern void FileSystemUnlock(void);

extern ErrorStatus FileIsExist(char *FilePathName);
extern ErrorStatus GetFileSize(char *FilePathName, u32* size);

extern void LogRecordFileInit(void);
extern ErrorStatus WriteSystemLog(void* data, u16 len, DATE_WRITE_TYPE op_type);

#endif
