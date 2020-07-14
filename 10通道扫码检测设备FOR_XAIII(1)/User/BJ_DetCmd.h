#ifndef _BJ_DETCMD_H_
#define _BJ_DETCMD_H_

/* �������״̬λ������ */
#define BJ_DET_MASK_TEST_ERR        0x04    // bit�쳣λ
#define BJ_DET_MASK_CHARGE          0x08	// ����쳣λ
#define BJ_DET_MASK_DISCHARGE       0x10	// �ŵ��쳣λ
#define BJ_DET_MASK_WAIT_EXPEN      0x20	// ׼�����쳣λ
#define BJ_DET_MASK_SETTIME_OK      0x40	// ����ʱ�������쳣λ

/* �׹ܲ���ָ�����(�ȷ��͵�λ�ٷ��͸�λ)*/
#define BJ_CHECK_IN           0x09              // ����
#define BJ_CLRALL			  0x19				// ȫ�ָ�λ
#define BJ_SERLRD			  0x29				// ������ض�
#define BJ_DISCHARGE		  0x39				// �ŵ�ָ�����оƬ���ã���оƬû��
#define BJ_WREEPROM           0x49              // EE���1
#define BJ_CHECK_OSC          0x59              // ��Ƶ
#define BJ_MASK               0x69              // ����
#define BJ_CLR                0x89              // ������λ
#define BJ_TIMEWR			  0x99				// ��������ʱ��
#define BJ_TIMERD			  0xA9				// �ض�����ʱ��
#define BJ_BIT			      0xB9				// BIT���ָ��
#define	BJ_ERASE_EEPROM		  0xC9				// ȫƬ����
#define BJ_CHARGE			  0xD9				// ���ָ��
#define BJ_TEST_CNT			  0xE9				// ���ڼ��������
#define BJ_EXPEN              0xf9              // ׼����


extern ErrorStatus BJ_CustomCmd(u8 *Cmd, u8 Bytes, u8 ErrorRetry, u8 RecBytes);
extern ErrorStatus BJ_DetCheckIn(void);
extern void BJ_DetClrAll(void);
extern ErrorStatus BJ_DetSerlRd(void);
extern ErrorStatus BJ_DetWrEEPROM(u8* Ptr);
extern ErrorStatus BJ_DetBit(u8 Ser1, u8 Ser2);
extern ErrorStatus BJ_DetMask(u8 Ser1, u8 Ser2);
extern ErrorStatus BJ_DetTimeWr(u8 Ser1, u8 Ser2, u8 Pwd, u32 DelayClk);
extern ErrorStatus BJ_DetTimeRd(u8 Ser1, u8 Ser2);
extern ErrorStatus BJ_DetEraseEEPROM(u8 Ser1, u8 Ser2);
extern ErrorStatus BJ_DetCharge(u8 Ser1, u8 Ser2, u8 Pwd);
extern ErrorStatus BJ_DetTestDelayCount(u8 Ser1, u8 Ser2);
extern ErrorStatus BJ_DetExpen(u8 Ser1, u8 Ser2, u8 Pwd);
extern void BJ_DetBlast(void);
extern ErrorStatus BJ_CustomDetBlast(u16 *Ptr, u8 Count);

#endif
