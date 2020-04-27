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

extern ErrorStatus DetCheckIn(void);
extern ErrorStatus DetMask(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus DetWrEEPROM(u8* Ptr);
extern ErrorStatus DetUnLockPwd(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2);
extern ErrorStatus DetUnLockPwdNotRetry(u8 Ser1, u8 Ser2, u8 Ser3, u8 Pwd1, u8 Pwd2);
extern ErrorStatus DetUnLockEEPROM(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus DetFixed(u8 Ser1, u8 Ser2, u8 Ser3, u16 Time);
extern void DetCheckOSC(u8 Max, u8 Min);
extern void DetClrAll(void);
extern ErrorStatus DetSerlRd(void);
extern void DetBit(u8 Ser1Group);
extern ErrorStatus DetTimeRd(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus DetTimeRdForFreqMeasure(u8 Ser1, u8 Ser2, u8 Ser3);
extern ErrorStatus DetTimeWr(u8 Ser1, u8 Ser2, u8 Ser3, u32 Delay);
extern ErrorStatus DetStatusRd(u8 Ser1, u8 Ser2, u8 Ser3);
extern void DetCharge(u8 Ser1Group);
extern void DetExpen(void);
extern void DetExpenByBlastSignalPin(void);
extern void DetBlastCmdEncode(void);
extern void DetBlastOne(void);
extern void DetBlast(void);
extern ErrorStatus GetDetStatus(DET_STATUS_BIT StatusBit);
extern void DetFixedPulse(u16 Count);
extern void DetSingleCapFixedPulse(u16 Count);
extern void DetDoubleCapFixedPulse(u16 Count);
extern u16 BlastDelayConvertToDetDelay(u16 DelayMs);
extern u32 BlastDelayConvertToDetDelayClock(u16 DelayMs, u32 Freq);

extern ErrorStatus DetCheckInBySimpleSignal(void);
extern void DetCheckInNotAck(void);
extern void DetStatusRdNotAck(u8 Ser1, u8 Ser2, u8 Ser3);

#endif
