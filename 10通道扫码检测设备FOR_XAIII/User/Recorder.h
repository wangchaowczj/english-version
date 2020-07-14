#ifndef _RECORDER_H_
#define _RECORDER_H_




/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define FACTORY_CFG_START_ADDR             ADDR_FLASH_SECTOR_3
#define FACTORY_CFG_END_ADDR               (ADDR_FLASH_SECTOR_4-1)

//设备ID
#define FACTORY_CFG_DEV_ID_ADDR            (FACTORY_CFG_START_ADDR + 0)
#define FACTORY_CFG_DEV_ID_SIZE            32//大小必需是2的倍数,不含CRC16

//硬件版本
#define FACTORY_CFG_HW_VERSION_ADDR        (FACTORY_CFG_DEV_ID_ADDR + FACTORY_CFG_DEV_ID_SIZE + 2)
#define FACTORY_CFG_HW_VERSION_SIZE        32//大小必需是2的倍数,不含CRC16

//电流校准参数
#define FACTORY_CFG_ADJUST_CURRENT_ADDR   (FACTORY_CFG_HW_VERSION_ADDR+FACTORY_CFG_HW_VERSION_SIZE + 2)
#define FACTORY_CFG_ADJUST_CURRENT_SIZE   4//大小必需是2的倍数,不含CRC16,参数K占2字节

//低压校准参数
#define FACTORY_CFG_ADJUST_1TO8_ADDR   (FACTORY_CFG_ADJUST_CURRENT_ADDR+FACTORY_CFG_ADJUST_CURRENT_SIZE + 2)
#define FACTORY_CFG_ADJUST_1TO8_SIZE   4//大小必需是2的倍数,不含CRC16,参数K占2字节

//低压校准参数
#define FACTORY_CFG_ADJUST_9_ADDR   (FACTORY_CFG_ADJUST_1TO8_ADDR+FACTORY_CFG_ADJUST_1TO8_SIZE + 2)
#define FACTORY_CFG_ADJUST_9_SIZE   4//大小必需是2的倍数,不含CRC16,参数K占2字节

//低压校准参数
#define FACTORY_CFG_ADJUST_10_ADDR   (FACTORY_CFG_ADJUST_9_ADDR+FACTORY_CFG_ADJUST_9_SIZE + 2)
#define FACTORY_CFG_ADJUST_10_SIZE   4//大小必需是2的倍数,不含CRC16,参数K占2字节

//低压校准参数
#define FACTORY_CFG_ADJUST_BAT_ADDR   (FACTORY_CFG_ADJUST_10_ADDR+FACTORY_CFG_ADJUST_10_SIZE + 2)
#define FACTORY_CFG_ADJUST_BAT_SIZE   4//大小必需是2的倍数,不含CRC16,参数K占2字节

//设备校准时间
#define DEVICE_ADJUST_TIME_ADD            (FACTORY_CFG_ADJUST_BAT_ADDR + FACTORY_CFG_ADJUST_BAT_SIZE + 2)
#define DEVICE_ADJUST_TIME_ADD_LEN        14//大小必需是2的倍数,不含CRC16

//工厂代码转换表
#define FACTORY_CFG_FACTORY_CODE_ADDR        (DEVICE_ADJUST_TIME_ADD + DEVICE_ADJUST_TIME_ADD_LEN + 2)
#define FACTORY_CFG_FACTORY_CODE_SIZE        16//大小必需是2的倍数,不含CRC16

#define FACTORY_CFG_DET_CLASS_ADDR        (FACTORY_CFG_FACTORY_CODE_ADDR + FACTORY_CFG_FACTORY_CODE_SIZE + 2)
#define FACTORY_CFG_DET_CLASS_SIZE        2//大小必需是2的倍数,不含CRC16

#define CONFIG_PARAM_FILE_PATH   "0:配置参数(MH)"   /**<保存的配置参数文件路径*/
#define CONFIG_PARAM_FILE_COMPLETE_PATH   "0:配置参数(MH)/配置参数.txt"   /**<保存的配置参数文件路径*/

#define DET_INFO_QUEUE_FILE_PATH   "0:测试状态(MH)"   /**<保存的配置参数文件路径*/
#define DET_INFO_QUEUE_FILE_COMPLETE_PATH   "0:测试状态(MH)/测试状态.txt"   /**<保存的配置参数文件路径*/

#define TEST_PARAM_FILE_COMPLETE_PATH   "0:配置参数(MH)/测试参数.txt"   /**<保存的配置参数文件路径*/

#define DET_TEST_RECORDER_MAX_NUM       100 //起爆记录最大条数
#define DET_TEST_RECORD_FILE_PATH   "0:检测记录(MH)"   /**<保存的雷管测试记录路径*/

typedef struct
{
	u8 FileName[DET_TEST_RECORDER_MAX_NUM][6];
	u16 Number;
	u16 Select;//当前选择的记录文件索引
	u16 Index;//当前选择的记录文件内索引
}DET_TEST_RECORDER;
extern DET_TEST_RECORDER DetTestRecorder;

#define DET_DATA_BINDING_RECORDER_MAX_NUM       30 //起爆记录最大条数
#define DET_DATA_BINDING_RECORD_FILE_PATH      "0:三码绑定(MH)"   /**<保存的雷管测试记录路径*/
typedef struct
{
	u8 FileName[DET_DATA_BINDING_RECORDER_MAX_NUM][6];
	u16 Number;
	u16 Select;//当前选择的记录文件索引
	u16 Index;//当前选择的记录文件内索引
}DET_DATA_BINDING_RECORDER;
extern DET_DATA_BINDING_RECORDER DetDataBindingRecorder;

extern void U32ToAscii(u32 value, u8* str);
extern u32 AsciiToU32(u8* str);

extern void FileSystemInitOS(void);
extern void ReadFactoryParam(void);
extern ErrorStatus WriteFactoryParam(void);
extern void DetTestRecordFileInit(void);
extern void DetDataBindingRecordFileInit(void);
extern void DeleteAllFileSpecifiedPath(char* Path);
extern ErrorStatus ReadDetTestRecordTestCount(u8* Name, u32* TotilNum, u32* ErrNum);
extern ErrorStatus ReadDetDataBindingRecordTestCount(u8* Name, u32* TotilNum);
extern ErrorStatus DeleteDetTestRecordFile(u16 Index);
extern ErrorStatus DeleteDetDataBindingRecordFile(u16 Index);

extern ErrorStatus WriteKeyString(u8* path,u8* key, u8* buf);
extern ErrorStatus WriteKeyU32Value(u8* path,u8* key, u32 value);
extern void ReadUserConfigParam(void);
extern void DetTestParamInit(void);
extern ErrorStatus SaveDetTestParamCfg(void);
extern ErrorStatus ReadDetTestParamCfg(void);
extern ErrorStatus SaveDetTestQueueInfo(void);
extern ErrorStatus ReadDetTestQueueInfo(void);

extern ErrorStatus CopyDetTestFileToMsc(u16 Index);
extern ErrorStatus CopyDetDataBindingFileToMsc(u16 Index);
extern ErrorStatus CopyHexFileToSdCard(void);

extern ErrorStatus DeleteOverdueFile(char* path, RTC_Typedef* time, u8 months);
extern ErrorStatus WriteDetTestRecord(void);


#endif
