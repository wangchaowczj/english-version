#ifndef _MASTERMODE_H_
#define _MASTERMODE_H_

//警示状态位
#define SLAVE_ERROR_BIT_BAT_LOW    			0x01  //电池电压过低
#define SLAVE_WARN_BIT_DET_ERROR   			0x02  //有错误雷管
#define SLAVE_WARN_BIT_BUS_LEAK    			0x04  //总线漏电大

//错误状态位

#define SLAVE_ERROR_BIT_DET_TYPE    		0x01  //雷管类型错误
#define SLAVE_ERROR_BIT_BUS_VOLTAGE     	0x02  //总线异常
#define SLAVE_ERROR_BIT_DET_ACCREDIT    	0x04  //有未授权雷管
#define SLAVE_ERROR_BIT_SYSTEM    			0x08  //系统错误，如无法保存数据等
#define SLAVE_ERROR_BIT_BLAST_CMD 			0x10  //没有收到起爆命令

typedef enum
{
	SLAVE_VOLATILE,
	SLAVE_FALSE,
	SLAVE_TRUE
}SLAVE_STATE;

typedef struct
{
	u16 RegCount;        /**<注册雷管数*/
	u16 ErrorCount;      /**<错误雷管数*/
	u8  Type;            /**<雷管类型*/	
	u8  ID[32];          /**<设备ID*/
	u8  Version[32];     /**<软件版本*/
	u16 Battery;         /**<电池*/
	u16 I_Leak;          /**<总线漏电流*/
	u16 V_BeforeTest;     /**<测试前电压*/
	u16 I_BeforeTest;     /**<测试前电流*/
	u16 V_BeforeCharge;  /**<充电前电压*/
	u16 I_BeforeCharge;  /**<充电前电流*/	
	u16 V_AfterCharge;   /**<充电后电压*/
	u16 I_AfterCharge;   /**<充电后电流*/
	u16 V_BeforeFire;    /**<起爆前电压*/
	u16 I_BeforeFire;    /**<起爆前电流*/
	u8 WarnBit;          /**<警示状态位*/
	u8 ErrorBit;         /**<错误状态位*/
	u8 Flags;            /**<进度状态位*/
	SLAVE_STATE State;  /**<状态*/	
}DEVICE_SLAVE;

#define SLAVE_NUMBER_MAX   25
typedef struct
{
	u8 Number;
	u16 RegCount;        /**<注册雷管数*/
	u16 ErrorCount;      /**<错误雷管数*/
    u16 OnlineCount;     /**<在线雷管数*/
	u8  Type;            /**<雷管类型*/	
	DEVICE_SLAVE Info[SLAVE_NUMBER_MAX];
}SLAVE_INFO;
extern SLAVE_INFO SlaveInfo;

extern u8 GetAddrNameFromIndex(u8 SlaveIndex);
extern u8 GetIndexFromAddrName(u8 AddrName);
extern SLAVE_STATE GetSlaveDetInfo(u8 SlaveIndex);
extern void SlaveInitState(void);
extern u8 GetSlaveStateCount(SLAVE_STATE State);
extern u8 GetSlaveOnlineErrorIndex(void);
extern u8 GetSlaveErrorCount(void);
extern u8 GetSlaveIndexFromErrorBit(u8 ErrorBit);
extern u8 GetSlaveWarnCount(void);

extern void GetSlaveDetNumber(void);
extern void GetAllSlaveInfo(u8 Arg);
extern ErrorStatus AllSlaveDetTest(u8 TestType, u8 TimeShare);
extern ErrorStatus AllSlaveCharge(void);
extern void AllSlaveBlastDet(void);
extern void AllSlaveStatusReset(void);
extern void AllSlaveSetRtc(void);
extern u8 BackupImportDetDataFromSlave(void);
extern u8 BackupExportDetDataToSlave(void);
#endif
