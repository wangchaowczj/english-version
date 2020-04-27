#ifndef _EVENTDEAL_H_
#define _EVENTDEAL_H_




extern u16 TempDataIn[20];			//用户输入的数据(临时保存)

extern u16 TempStep;				//用户临时步骤
extern u16 TempOverTime;			//用于等待超时时间的数值
extern u8 USBDkeyCheck;				//用于检查密钥的连接状态。1-检查，0-不检查。如果检查到密钥拔出，则有可能会直接出现提示。
extern u8 DelaySetState;			//批量设置延期时，用于表示孔间延期和孔内延期是否还需要输入(BIT0表示孔间延期，BIT1表示孔内延期)
extern u8 DetVolTempState;			//在雷管注册和雷管通信界面时，记录当前是雷管注册还是雷管通信。用于在判断电压异常时，返回到哪个界面






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
