#ifndef _MASTERMODE_H_
#define _MASTERMODE_H_

//��ʾ״̬λ
#define SLAVE_ERROR_BIT_BAT_LOW    			0x01  //��ص�ѹ����
#define SLAVE_WARN_BIT_DET_ERROR   			0x02  //�д����׹�
#define SLAVE_WARN_BIT_BUS_LEAK    			0x04  //����©���

//����״̬λ

#define SLAVE_ERROR_BIT_DET_TYPE    		0x01  //�׹����ʹ���
#define SLAVE_ERROR_BIT_BUS_VOLTAGE     	0x02  //�����쳣
#define SLAVE_ERROR_BIT_DET_ACCREDIT    	0x04  //��δ��Ȩ�׹�
#define SLAVE_ERROR_BIT_SYSTEM    			0x08  //ϵͳ�������޷��������ݵ�
#define SLAVE_ERROR_BIT_BLAST_CMD 			0x10  //û���յ�������

typedef enum
{
	SLAVE_VOLATILE,
	SLAVE_FALSE,
	SLAVE_TRUE
}SLAVE_STATE;

typedef struct
{
	u16 RegCount;        /**<ע���׹���*/
	u16 ErrorCount;      /**<�����׹���*/
	u8  Type;            /**<�׹�����*/	
	u8  ID[32];          /**<�豸ID*/
	u8  Version[32];     /**<����汾*/
	u16 Battery;         /**<���*/
	u16 I_Leak;          /**<����©����*/
	u16 V_BeforeTest;     /**<����ǰ��ѹ*/
	u16 I_BeforeTest;     /**<����ǰ����*/
	u16 V_BeforeCharge;  /**<���ǰ��ѹ*/
	u16 I_BeforeCharge;  /**<���ǰ����*/	
	u16 V_AfterCharge;   /**<�����ѹ*/
	u16 I_AfterCharge;   /**<�������*/
	u16 V_BeforeFire;    /**<��ǰ��ѹ*/
	u16 I_BeforeFire;    /**<��ǰ����*/
	u8 WarnBit;          /**<��ʾ״̬λ*/
	u8 ErrorBit;         /**<����״̬λ*/
	u8 Flags;            /**<����״̬λ*/
	SLAVE_STATE State;  /**<״̬*/	
}DEVICE_SLAVE;

#define SLAVE_NUMBER_MAX   25
typedef struct
{
	u8 Number;
	u16 RegCount;        /**<ע���׹���*/
	u16 ErrorCount;      /**<�����׹���*/
    u16 OnlineCount;     /**<�����׹���*/
	u8  Type;            /**<�׹�����*/	
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
