#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MH10_V1.4_190527"	     //系统版本号


//雷管类型定义(共3bit)
#define DET_TYPE_XA_02      0x00	   //2秒,双电容,延期在线可编程
#define DET_TYPE_XA_06      0x01	   //6秒,双电容,延期在线可编程
#define DET_TYPE_XA_21      0x02	   //21秒,双电容,延期在线可编程

//最大延期限制
#define DET_DLY_MAX_XA_02       2000UL
#define DET_DLY_MAX_XA_06       6000UL
#define DET_DLY_MAX_XA_21       21000UL



typedef struct
{
	u8 name;
	u8 ID[32];    //设备ID号
	u8 HW[32];    //硬件版本号
//    u8 DetClass;  //0表示XA，1表示BJ
}DEVICE_INFO;

typedef enum
{
	FINISHED,//成品
	SEMIFINISHED,//半成品
	DET_CODE_VERIFY,//内码验证
	ENTER_FACTORY,//入厂检
}USE_STAGE_CFG;

typedef enum
{
	BEFORE,//前打码
	AFTER,//后打码
}WRITE_BAR_MODE_CFG;

typedef enum
{
	FINISHED_RING,//完成响
	NOT_RING,//不响
	RIGHT_RING,//合格响
	ERROR_RING//不合格响
}SOUND_CFG;
typedef enum
{
	DET_TEST_LCD_STATUS_ON,//检测时屏幕开
	DET_TEST_LCD_STATUS_OFF//检测时屏幕关
}DET_TEST_LCD_STATUS_CFG;

typedef enum
{
	ROUND_GENERAL,//普通圆脚线
	DURABLE,//耐磨圆脚线
	TUNNEL//隧道专用脚线
}DET_LINE_TYPE_CFG;


typedef struct
{
	u8 DetType;
	USE_STAGE_CFG Stage;//检测环节
	SOUND_CFG Sound;
	u8 LcdBright;
	DET_TEST_LCD_STATUS_CFG DetTestLcdStatus;
	u8 DetLineLength;//脚线长度
	DET_LINE_TYPE_CFG DetLineType;
	u8 DetCodeWrite;//内码是否写入，0不写，1写
	u8 FactoryCode;//工厂代码
	u32 ProductTime;//生产时间,"20160627"
	u16 DetDelay;//雷管延期值
	u8 CheckDelay;
	u8 DelayWrite;
	u8 Feature;//特征号
	u8 CheckDetCode;//内码检验
    u8 NetMode;//不联网、联网
	u16 NetModeFactoryCode;//联网模式时，对应的管厂代码(表示哪个工厂的协议)  
    u8 DetOutsideStartHead[8];//13位管码的前8位
    u32 DetOutsideStartBoxIndex;//13位管码的后5位
    u8 InBoxNumber;//盒内数量
    u8 AutoBack;//检测错误时，是否需要手动返回(仅用于单通道)
	u32 DetDetectorPassCount;
}USER_PARAM;

typedef struct
{
    const char* Name;
    const u8 Code;
}FACTORY_NAME_CFG;

#endif
