#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MB10_V1.5_190808"	     //系统版本号

typedef struct
{
	u8 mac[32];           //设备ID号
	u8 hw_version[32];    //硬件版本号
    u8 name;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

#endif
