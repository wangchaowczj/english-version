#ifndef _DETCMD_H_
#define _DETCMD_H_

#define BLAST_DELAY_WHOLE_ADD_VALUE			20	//整体增加一个延期

#define CHECKIN			  0x42				// 查漏指令
#define CLRALL			  0x0A				// 全局复位
#define SERLRD			  0x12				// 序列码回读
#define WREEPROM		  0x22				// 写EEPROM指令,序列码编程
#define PWD_UNLOCK		  0xAA				// 解密指令
#define EE_UNLOCK		  0xDA				// EEPROM解锁指令
#define FIXED		      0xC2				// 固定分段时间编程指令
#define CHECKOSC		  0x2A				// 时钟校准指令
#define	MASK			  0x32				// 屏蔽指令			
#define TIMEWR			  0x4A				// 设置延期时间
#define TIMERD			  0x52				// 延期时间回读指令
#define STATUSRD		  0x7A				// 状态回读指令
#define BIT			      0x5A				// BIT检测指令
#define CHARGE			  0x6A				// 充电指令
#define	EXPEN			  0xFA				// 准备起爆指令

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
	EXPEN_STATUS,//芯片当前处于等待起爆信号状态（正确接收准备起爆指令后
	CAP_STATUS,//起爆电容焊接错误标志（起爆电容脱落或者虚焊）
	BIT_STATUS,//BIT测试错误标志
	CHARGE_STATUS,//芯片处于充电状态标志
	DISCHARGE_STATUS,//芯片处于安全放电状态标志
	RESERVED_STATUS,//固定为1
	TIME_STATUS,//芯片延期时间已经设定标志
	BIT_OUT_STATUS//BIT测试比较器输出状态
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
