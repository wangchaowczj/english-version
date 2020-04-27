#ifndef _SYSTEMP_H_
#define _SYSTEMP_H_


#define VERSION_STRING   "MQ01_V2.3_200330"	     //ϵͳ�汾��

#define BLAST_DELAY_MAX      61 //����ʱʱ�����ֵΪ(61-1)����


//�׹����Ͷ���(��5bit)
#define DET_TYPE_XA02      0x00	   //2��
#define DET_TYPE_XA06      0x01	   //6��
#define DET_TYPE_XA21      0x02	   //21��

//�����������
#define DET_DLY_MAX_XA02       2000UL
#define DET_DLY_MAX_XA06       6000UL
#define DET_DLY_MAX_XA21       21000UL

//�豸���Ͷ���	
#define DEVICE_TYPE_SAFE_MONITOR               0x00//�����豸
#define DEVICE_TYPE_CHINESE_GENERAL            0x01//��ͨ�ǰ���
#define DEVICE_TYPE_SAFE_MONITOR_OFFLINE       0x02//�����豸��������

//�豸�÷�
#define DEVICE_USAGES_ALONE               0x00//�����豸
#define DEVICE_USAGES_SLAVE               0x01//�ӻ�
#define DEVICE_USAGES_MASTER              0x02//����

typedef struct
{
	u32 Ip;
	u16 Port;
	u8 Date[14];//����ʱ�䣬"20160409122033"
}INTERNET_ADDR;
typedef struct
{
	u8  mac[32];           //�豸ID��
	u8  hw_version[32];    //Ӳ���汾��
	u8  name;	           //�豸��
	u8  DeviceType;        //�豸����
	uint64_t config;       //���ò���
	u16 single_current_min;//����������Сֵ(��λΪuA)
	u16 single_current_max;//�����������ֵ(��λΪuA)
	u32 det_freq_min;      //оƬƵ����Сֵ(��λΪHZ)
	u32 det_freq_max;      //оƬƵ�����ֵ(��λΪHZ)
	u8 LcdBrightStrength;  //Һ����������
	u8 LcdBrightTime;      //Һ������ʱ��,0��ʾ������100��ʾ����
	u8 BlastDelay;         //����ʱʱ��
	INTERNET_ADDR Server;  //���������
	u8 DeviceUsages;       //�豸�÷�(�����豸���������ӻ�)
	u8 HasScan;		       //��ɨ��ͷ
	u8 BlueVersion[4];//����ģ������汾
    u8 NetInfoSort;   //����ʽ��0 ���׺Ŵ�С��������1�����ڴ�С��������
    u8 RegNetParamDefault;
    u32 BackgroundColor;
    u32 TextColor;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

#endif
