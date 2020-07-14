#ifndef _DETCMD_H_
#define _DETCMD_H_

#define BLAST_DELAY_WHOLE_ADD_VALUE			20	//��������һ������

#define CHECKIN			  0x42				// ��©ָ��
#define CLRALL			  0x0A				// ȫ�ָ�λ
#define SERLRD			  0x12				// ������ض�
#define WREEPROM		  0x22				// дEEPROMָ��,��������
#define PWD_UNLOCK		  0xAA				// ����ָ��
#define EE_UNLOCK		  0xDA				// EEPROM����ָ��
#define FIXED		      0xC2				// �̶��ֶ�ʱ����ָ��
#define CHECKOSC		  0x2A				// ʱ��У׼ָ��
#define	MASK			  0x32				// ����ָ��			
#define TIMEWR			  0x4A				// ��������ʱ��
#define TIMERD			  0x52				// ����ʱ��ض�ָ��
#define STATUSRD		  0x7A				// ״̬�ض�ָ��
#define BIT			      0x5A				// BIT���ָ��
#define CHARGE			  0x6A				// ���ָ��
#define	EXPEN			  0xFA				// ׼����ָ��

/*
оƬ���յ���ȷ��ȫ�ָ�λָ���
 Discharge(��ȫ�ŵ�״̬Ϊ1)��
 Charge�����״̬Ϊ0����
 Time_ok������ʱ��������ɱ�ʶΪ0����
 Test_err��BIT���Դ����־Ϊ1����
 min_cap_err���𱬵����麸��־Ϊ0����
 WaitforExpen�����ڵȴ��𱬽׶α�־Ϊ0��
*/
typedef enum
{
	EXPEN_STATUS,//оƬ��ǰ���ڵȴ����ź�״̬����ȷ����׼����ָ���
	CAP_STATUS,//�𱬵��ݺ��Ӵ����־���𱬵�����������麸��
	BIT_STATUS,//BIT���Դ����־
	CHARGE_STATUS,//оƬ���ڳ��״̬��־
	DISCHARGE_STATUS,//оƬ���ڰ�ȫ�ŵ�״̬��־
	RESERVED_STATUS,//�̶�Ϊ1
	TIME_STATUS,//оƬ����ʱ���Ѿ��趨��־
	BIT_OUT_STATUS//BIT���ԱȽ������״̬
}DET_STATUS_BIT;

extern ErrorStatus XA_DetCheckIn(void);
extern ErrorStatus XA_DetMask(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus XA_DetWrEEPROM(u8* Ptr);
extern ErrorStatus XA_DetUnLockPwd(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2);
extern ErrorStatus XA_DetUnLockEEPROM(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus XA_DetFixed(u8 Ser1, u8 Ser2, u8 Ser3, u16 Time);
extern void XA_DetCheckOSC(u8 Max, u8 Min);
extern void XA_DetClrAll(void);
extern ErrorStatus XA_DetSerlRd(void);
extern void XA_DetBit(u8 Ser1Group);
extern ErrorStatus XA_DetTimeRd(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus XA_DetTimeWr(u8 Ser1, u8 Ser2, u8 Ser3, u32 Delay);
extern ErrorStatus XA_DetStatusRd(u8 Ser1, u8 Ser2, u8 Ser3);
extern void XA_DetCharge(u8 Ser1Group);
extern void XA_DetExpen(void);
extern void XA_DetBlastCmdEncode(void);
extern void XA_DetBlast(void);
extern ErrorStatus XA_GetDetStatus(DET_STATUS_BIT StatusBit);
extern void XA_DetFixedPulse(u16 Count);
extern u16 XA_BlastDelayConvertToDetDelay(u16 DelayMs);
extern u32 XA_BlastDelayConvertToDetDelayClock(u16 DelayMs, u32 Freq);

#endif
