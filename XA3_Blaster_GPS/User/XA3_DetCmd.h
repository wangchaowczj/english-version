#ifndef _XA3_DETCMD_H_
#define _XA3_DETCMD_H_

/* 雷管操作指令代码(先发送低位再发送高位)*/
#define XA3_CHECK_IN                  0x42              // 点名
#define XA3_CLEAR_ALL                 0x0A              // 全局软复位
#define XA3_RESET_ALL                 0x1A              // 全局硬件复位
#define XA3_EN_CHARGE_ALL             0xE2              // 全局充电允许
#define XA3_EXPEN_ALL                 0xFA              // 全局准备起爆
#define XA3_SERLRD                    0x12              // 数据回读
#define XA3_WR_EEPROM                 0x22              // 数据编程
#define XA3_RD_PWD                    0xB2              // 密码读取
#define XA3_MASK                      0x32              // 屏蔽指令
#define XA3_PWD_UNLOCK                0xAA              // 解密指令
#define XA3_WR_FIX_DELAY_UNLOCK       0xDA              // 写固定延期解锁
#define XA3_WR_EE_DATA_UNLOCK         0xCA              // 写数据解锁解锁
#define XA3_WR_FIX_DELAY              0xC2              // 写固定延期时间编程
#define XA3_RESET                     0x3A              // 单个硬件复位
#define XA3_CHECKOSC_ALL              0x2A              // 全局时钟校准
#define XA3_WR_TIME                   0x4A              // 设置延期时间
#define XA3_RD_TIME                   0x52              // 延期时间回读
#define XA3_RD_STATUS                 0x7A              // 状态回读
#define XA3_BIT                       0x5A              // BIT测试
#define XA3_CHARGE                    0x6A              // 充电
#define XA3_CHARGE_ALL                0xEA              // 全局充电


/*
芯片接收到正确的全局复位指令后：
 Discharge(安全放电状态为1)、
 Charge（充电状态为0）、
 Time_ok（延期时间设置完成标识为0）、
 Test_err（BIT测试错误标志为1）、
 min_cap_err（起爆电容虚焊标志为0）、
 WaitforExpen（处于等待起爆阶段标志为0）
*/
typedef enum
{
    XA3_DELAY_ZERO_STATUS,//延期全0标志,同步头的bit6位
    XA3_PWD_UNLOCK_STATUS,//已解密标志,同步头的bit7位
	XA3_EXPEN_STATUS,//芯片当前处于等待起爆信号状态（正确接收准备起爆指令后
	XA3_CAP_STATUS,//起爆电容焊接错误标志（起爆电容脱落或者虚焊）
	XA3_BIT_STATUS,//BIT测试错误标志
	XA3_CHARGE_STATUS,//芯片处于充电状态标志
	XA3_FIX_DELAY_UNLOCK_STATUS,//当前处于固定延期编程解锁状态
	XA3_EEPROM_UNLOCK_STATUS,//当前处于写数据编程解锁状态
	XA3_TIME_STATUS,//芯片延期时间已经设定标志
	XA3_BIT_OUT_STATUS//BIT测试比较器输出状态
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

