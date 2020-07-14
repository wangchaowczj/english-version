#ifndef _XA2_DETCMD_H_
#define _XA2_DETCMD_H_

/* 雷管操作指令代码(先发送低位再发送高位)*/
#define XA2_CHECK_IN                  0x42              // 点名
#define XA2_CLEAR_ALL                 0x0A              // 全局软复位
#define XA2_RESET_ALL                 0x1A              // 全局硬件复位
#define XA2_EN_CHARGE_ALL             0xE2              // 全局充电允许
#define XA2_EXPEN_ALL                 0xFA              // 全局准备起爆
#define XA2_SERLRD                    0x12              // 数据回读
#define XA2_WR_EEPROM                 0x22              // 数据编程
#define XA2_RD_PWD                    0xB2              // 密码读取
#define XA2_MASK                      0x32              // 屏蔽指令
#define XA2_PWD_UNLOCK                0xAA              // 解密指令
#define XA2_WR_FIX_DELAY_UNLOCK       0xDA              // 写固定延期解锁
#define XA2_WR_EE_DATA_UNLOCK         0xCA              // 写数据解锁解锁
#define XA2_WR_FIX_DELAY              0xC2              // 写固定延期时间编程
#define XA2_RESET                     0x3A              // 单个硬件复位
#define XA2_CHECKOSC_ALL              0x2A              // 全局时钟校准
#define XA2_WR_TIME                   0x4A              // 设置延期时间
#define XA2_RD_TIME                   0x52              // 延期时间回读
#define XA2_RD_STATUS                 0x7A              // 状态回读
#define XA2_BIT                       0x5A              // BIT测试
#define XA2_CHARGE                    0x6A              // 充电
#define XA2_CHARGE_ALL                0xEA              // 全局充电


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
    XA2_DELAY_ZERO_STATUS,//延期全0标志,同步头的bit6位
    XA2_PWD_UNLOCK_STATUS,//已解密标志,同步头的bit7位
	XA2_EXPEN_STATUS,//芯片当前处于等待起爆信号状态（正确接收准备起爆指令后
	XA2_CAP_STATUS,//起爆电容焊接错误标志（起爆电容脱落或者虚焊）
	XA2_BIT_STATUS,//BIT测试错误标志
	XA2_CHARGE_STATUS,//芯片处于充电状态标志
	XA2_FIX_DELAY_UNLOCK_STATUS,//当前处于固定延期编程解锁状态
	XA2_EEPROM_UNLOCK_STATUS,//当前处于写数据编程解锁状态
	XA2_TIME_STATUS,//芯片延期时间已经设定标志
	XA2_BIT_OUT_STATUS//BIT测试比较器输出状态
}XA2_DET_STATUS_BIT;

#define XA2_BLAST_DELAY_WHOLE_ADD_VALUE			72	//整体增加一个延期


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

