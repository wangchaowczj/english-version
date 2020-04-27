#ifndef _EVENTDEAL_H_
#define _EVENTDEAL_H_




extern u16 TempDataIn[20];			//�û����������(��ʱ����)

extern u16 TempStep;				//�û���ʱ����
extern u16 TempOverTime;			//���ڵȴ���ʱʱ�����ֵ
extern u8 USBDkeyCheck;				//���ڼ����Կ������״̬��1-��飬0-����顣�����鵽��Կ�γ������п��ܻ�ֱ�ӳ�����ʾ��
extern u8 DelaySetState;			//������������ʱ�����ڱ�ʾ�׼����ںͿ��������Ƿ���Ҫ����(BIT0��ʾ�׼����ڣ�BIT1��ʾ��������)
extern u8 DetVolTempState;			//���׹�ע����׹�ͨ�Ž���ʱ����¼��ǰ���׹�ע�ỹ���׹�ͨ�š��������жϵ�ѹ�쳣ʱ�����ص��ĸ�����






extern u8 Event_Judge_Delay(u32 NowDelay);
extern void Event_SetStringCursor(u16 NowX, u16 NowY, u32 ShowS, u8 ShowLen);
extern u8 Event_PiliangDelay(u8 NowState, u32 NowCount);
extern void Event_DelayState(u16 NowX, u16 NowY, u8 NowState);
extern void Event_SetLcdKeyBrightTime(u8 NowState);
extern void Event_GetLcdBTime(u8 LTime, u8 *buf);
extern void Event_GetKeyBTime(u8 KTime, u8 *buf);
extern void Event_SaveInputData(u8 NowSelect, u8 NowInit, u8 NowInput);
extern u8 Event_CheckRTCData(void);
extern u8 Event_WaitLineSteaby(u8 State);
extern u8 Event_CheckDetType(const u8 det_bat2, u8 NowType);
extern u8 Event_ModBusCheckDetType(const u8 det_bat2, u8 NowType);
extern u16 Event_ModBusGetDelayMax(u8 NowType);
extern void Event_BTNameCheck(u8 NowCheck);
extern void Event_BTOpenPower(void);
extern void Event_BTClosePower(void);
extern void Event_BTAccountErrDetNum(void);
extern u8 Event_GetDetTypeStr(const u8 det_bat2);
extern void Event_USBOpen(void);
extern void Event_USBColse(void);
extern u8 Event_CheckDetReg(u16 NowNum);























#endif
