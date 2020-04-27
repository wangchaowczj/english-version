#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MQ01_V2.3_200330"	     //系统版本号

#define BLAST_DELAY_MAX      61 //起爆延时时间最大值为(61-1)分钟


//雷管类型定义(共5bit)
#define DET_TYPE_XA02      0x00	   //2秒
#define DET_TYPE_XA06      0x01	   //6秒
#define DET_TYPE_XA21      0x02	   //21秒

//最大延期限制
#define DET_DLY_MAX_XA02       2000UL
#define DET_DLY_MAX_XA06       6000UL
#define DET_DLY_MAX_XA21       21000UL

//设备类型定义	
#define DEVICE_TYPE_SAFE_MONITOR               0x00//安管设备
#define DEVICE_TYPE_CHINESE_GENERAL            0x01//普通非安管
#define DEVICE_TYPE_SAFE_MONITOR_OFFLINE       0x02//安管设备，离线起爆

//设备用法
#define DEVICE_USAGES_ALONE               0x00//独立设备
#define DEVICE_USAGES_SLAVE               0x01//从机
#define DEVICE_USAGES_MASTER              0x02//主机

typedef struct
{
	u32 Ip;
	u16 Port;
	u8 Date[14];//设置时间，"20160409122033"
}INTERNET_ADDR;
typedef struct
{
	u8  mac[32];           //设备ID号
	u8  hw_version[32];    //硬件版本号
	u8  name;	           //设备号
	u8  DeviceType;        //设备类型
	uint64_t config;       //配置参数
	u16 single_current_min;//单个电流最小值(单位为uA)
	u16 single_current_max;//单个电流最大值(单位为uA)
	u32 det_freq_min;      //芯片频率最小值(单位为HZ)
	u32 det_freq_max;      //芯片频率最大值(单位为HZ)
	u8 LcdBrightStrength;  //液晶背光亮度
	u8 LcdBrightTime;      //液晶背光时间,0表示常暗，100表示常亮
	u8 BlastDelay;         //起爆延时时间
	INTERNET_ADDR Server;  //网络服务器
	u8 DeviceUsages;       //设备用法(独立设备、主机、从机)
	u8 HasScan;		       //带扫码头
	u8 BlueVersion[4];//蓝牙模块软件版本
    u8 NetInfoSort;   //排序方式，0 按孔号从小到大排序，1按延期从小到大排序
    u8 RegNetParamDefault;
    u32 BackgroundColor;
    u32 TextColor;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

#endif
