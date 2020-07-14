#ifndef _XA2_DETCMD_H_
#define _XA2_DETCMD_H_

/* �׹ܲ���ָ�����(�ȷ��͵�λ�ٷ��͸�λ)*/
#define XA2_CHECK_IN                  0x42              // ����
#define XA2_CLEAR_ALL                 0x0A              // ȫ����λ
#define XA2_RESET_ALL                 0x1A              // ȫ��Ӳ����λ
#define XA2_EN_CHARGE_ALL             0xE2              // ȫ�ֳ������
#define XA2_EXPEN_ALL                 0xFA              // ȫ��׼����
#define XA2_SERLRD                    0x12              // ���ݻض�
#define XA2_WR_EEPROM                 0x22              // ���ݱ��
#define XA2_RD_PWD                    0xB2              // �����ȡ
#define XA2_MASK                      0x32              // ����ָ��
#define XA2_PWD_UNLOCK                0xAA              // ����ָ��
#define XA2_WR_FIX_DELAY_UNLOCK       0xDA              // д�̶����ڽ���
#define XA2_WR_EE_DATA_UNLOCK         0xCA              // д���ݽ�������
#define XA2_WR_FIX_DELAY              0xC2              // д�̶�����ʱ����
#define XA2_RESET                     0x3A              // ����Ӳ����λ
#define XA2_CHECKOSC_ALL              0x2A              // ȫ��ʱ��У׼
#define XA2_WR_TIME                   0x4A              // ��������ʱ��
#define XA2_RD_TIME                   0x52              // ����ʱ��ض�
#define XA2_RD_STATUS                 0x7A              // ״̬�ض�
#define XA2_BIT                       0x5A              // BIT����
#define XA2_CHARGE                    0x6A              // ���
#define XA2_CHARGE_ALL                0xEA              // ȫ�ֳ��


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
    XA2_DELAY_ZERO_STATUS,//����ȫ0��־,ͬ��ͷ��bit6λ
    XA2_PWD_UNLOCK_STATUS,//�ѽ��ܱ�־,ͬ��ͷ��bit7λ
	XA2_EXPEN_STATUS,//оƬ��ǰ���ڵȴ����ź�״̬����ȷ����׼����ָ���
	XA2_CAP_STATUS,//�𱬵��ݺ��Ӵ����־���𱬵�����������麸��
	XA2_BIT_STATUS,//BIT���Դ����־
	XA2_CHARGE_STATUS,//оƬ���ڳ��״̬��־
	XA2_FIX_DELAY_UNLOCK_STATUS,//��ǰ���ڹ̶����ڱ�̽���״̬
	XA2_EEPROM_UNLOCK_STATUS,//��ǰ����д���ݱ�̽���״̬
	XA2_TIME_STATUS,//оƬ����ʱ���Ѿ��趨��־
	XA2_BIT_OUT_STATUS//BIT���ԱȽ������״̬
}XA2_DET_STATUS_BIT;

#define XA2_BLAST_DELAY_WHOLE_ADD_VALUE			72	//��������һ������


extern ErrorStatus XA2_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes);
extern ErrorStatus XA2_DetCheckIn(void);
extern void XA2_DetCheckInNotAck(void);
extern void XA2_ClearAll(void);
extern void XA2_ResetAll(void);
extern void XA2_ChargeAllEnable(void);
extern void XA2_ExpenAll(void);
extern void XA2_ExpenAllByBlastSignalPin(void);
extern ErrorStatus XA2_ReadEEPROM(u8 Addr, u8* Buffer);
extern ErrorStatus XA2_ReadSerl(u32* Serl);
extern ErrorStatus XA2_WriteEEPROM(u8 Addr, u8* Buffer);
extern ErrorStatus XA2_ReadPwd(u32 Serl, u32* Pwd);
extern ErrorStatus XA2_Mask(u32 Serl);
extern u8 XA2_PwdUnlock(u32 Serl, u32 Pwd);
extern ErrorStatus XA2_WriteFixDelayUnlock(u32 Serl);
extern ErrorStatus XA2_WriteDataUnlock(u32 Serl);
extern ErrorStatus XA2_WriteFixDelay(u32 Serl, u16 FixDelay);
extern ErrorStatus XA2_Reset(u32 Serl);
extern void XA2_CheckOscAll(u8 Max, u8 Min);
extern ErrorStatus XA2_WriteTime(u32 Serl, u32 DelayClock);
extern ErrorStatus XA2_ReadTimeClock(u32 Serl, u32* TimeClk, u16* FixDelay, u8* TimeClkFlag);
extern ErrorStatus XA2_ReadStatus(u32 Serl, u16* Status);
extern void XA2_ReadStatusNotAck(u32 Serl);
extern void XA2_Bit(u8 Group);
extern void XA2_Charge(u8 Group);
extern void XA2_ChargeAll(u8 Group);
extern ErrorStatus XA2_GetDetStatus(u16 DetStatus, XA2_DET_STATUS_BIT StatusBit);


extern ErrorStatus XA2_CustomDetBlast(u16 *Ptr, u8 Count);
extern void XA2_DetFixedPulse(u16 Count, u32 HihtHalf, u32 LowHalf);



#endif

