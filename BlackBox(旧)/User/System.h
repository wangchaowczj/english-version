#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MB10_V1.5_190808"	     //ϵͳ�汾��

typedef struct
{
	u8 mac[32];           //�豸ID��
	u8 hw_version[32];    //Ӳ���汾��
    u8 name;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

#endif
