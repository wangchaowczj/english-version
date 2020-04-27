#ifndef _XA3_DETCMD_H_
#define _XA3_DETCMD_H_

/* �׹ܲ���ָ�����(�ȷ��͵�λ�ٷ��͸�λ)*/
#define XA3_CHECK_IN                  0x42              // ����
#define XA3_CLEAR_ALL                 0x0A              // ȫ����λ
#define XA3_RESET_ALL                 0x1A              // ȫ��Ӳ����λ
#define XA3_EN_CHARGE_ALL             0xE2              // ȫ�ֳ������
#define XA3_EXPEN_ALL                 0xFA              // ȫ��׼����
#define XA3_SERLRD                    0x12              // ���ݻض�
#define XA3_WR_EEPROM                 0x22              // ���ݱ��
#define XA3_RD_PWD                    0xB2              // �����ȡ
#define XA3_MASK                      0x32              // ����ָ��
#define XA3_PWD_UNLOCK                0xAA              // ����ָ��
#define XA3_WR_FIX_DELAY_UNLOCK       0xDA              // д�̶����ڽ���
#define XA3_WR_EE_DATA_UNLOCK         0xCA              // д���ݽ�������
#define XA3_WR_FIX_DELAY              0xC2              // д�̶�����ʱ����
#define XA3_RESET                     0x3A              // ����Ӳ����λ
#define XA3_CHECKOSC_ALL              0x2A              // ȫ��ʱ��У׼
#define XA3_WR_TIME                   0x4A              // ��������ʱ��
#define XA3_RD_TIME                   0x52              // ����ʱ��ض�
#define XA3_RD_STATUS                 0x7A              // ״̬�ض�
#define XA3_BIT                       0x5A              // BIT����
#define XA3_CHARGE                    0x6A              // ���
#define XA3_CHARGE_ALL                0xEA              // ȫ�ֳ��


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
    XA3_DELAY_ZERO_STATUS,//����ȫ0��־,ͬ��ͷ��bit6λ
    XA3_PWD_UNLOCK_STATUS,//�ѽ��ܱ�־,ͬ��ͷ��bit7λ
	XA3_EXPEN_STATUS,//оƬ��ǰ���ڵȴ����ź�״̬����ȷ����׼����ָ���
	XA3_CAP_STATUS,//�𱬵��ݺ��Ӵ����־���𱬵�����������麸��
	XA3_BIT_STATUS,//BIT���Դ����־
	XA3_CHARGE_STATUS,//оƬ���ڳ��״̬��־
	XA3_FIX_DELAY_UNLOCK_STATUS,//��ǰ���ڹ̶����ڱ�̽���״̬
	XA3_EEPROM_UNLOCK_STATUS,//��ǰ����д���ݱ�̽���״̬
	XA3_TIME_STATUS,//оƬ����ʱ���Ѿ��趨��־
	XA3_BIT_OUT_STATUS//BIT���ԱȽ������״̬
}XA3_DET_STATUS_BIT;




extern ErrorStatus XA3_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes);
extern ErrorStatus XA3_DetCheckIn(void);
extern ErrorStatus XA3_DetCheckInBySimpleSignal(void);
extern void XA3_DetCheckInNotAck(void);
extern void XA3_ClearAll(void);
extern void XA3_ResetAll(void);
extern void XA3_ChargeAllEnable(void);
extern void XA3_ExpenAll(void);
extern void XA3_ExpenAllByBlastSignalPin(void);
extern ErrorStatus XA3_ReadEEPROM(u8 Addr, u8* Buffer);
extern ErrorStatus XA3_ReadSerl(u32* Serl);
extern ErrorStatus XA3_WriteEEPROM(u8 Addr, u8* Buffer);
extern ErrorStatus XA3_ReadPwd(u32 Serl, u32* Pwd);
extern ErrorStatus XA3_Mask(u32 Serl);
extern u8 XA3_PwdUnlock(u32 Serl, u32 Pwd);
extern ErrorStatus XA3_WriteFixDelayUnlock(u32 Serl);
extern ErrorStatus XA3_WriteDataUnlock(u32 Serl);
extern ErrorStatus XA3_WriteFixDelay(u32 Serl, u16 FixDelay);
extern u8 XA3_WriteFixDelayTest(u32 Serl, u16 FixDelay);
extern ErrorStatus XA3_Reset(u32 Serl);
extern void XA3_CheckOscAll(u8 Max, u8 Min);
extern ErrorStatus XA3_WriteTime(u32 Serl, u32 DelayClock);
extern ErrorStatus XA3_ReadTimeClock(u32 Serl, u32* TimeClk, u16* FixDelay, u8* TimeClkFlag);
extern ErrorStatus XA3_ReadStatus(u32 Serl, u16* Status);
extern void XA3_ReadStatusNotAck(u32 Serl);
extern void XA3_Bit(u8 Group);
extern void XA3_Charge(u8 Group);
extern void XA3_ChargeAll(u8 Group);
extern ErrorStatus XA3_GetDetStatus(u16 DetStatus, XA3_DET_STATUS_BIT StatusBit);
extern void XA3_DetBlastOne(void);
extern void XA3_DetBlast(void);
extern ErrorStatus XA3_CustomDetBlast(u16 *Ptr, u8 Count);
extern void XA3_DetFixedPulse(u16 Count, u32 HihtHalf, u32 LowHalf);



#endif

