#ifndef _BJ_DETCMD_H_
#define _BJ_DETCMD_H_

/* 定义各个状态位的掩码 */
#define BJ_DET_MASK_TEST_ERR        0x04    // bit异常位
#define BJ_DET_MASK_CHARGE          0x08	// 充电异常位
#define BJ_DET_MASK_DISCHARGE       0x10	// 放电异常位
#define BJ_DET_MASK_WAIT_EXPEN      0x20	// 准备起爆异常位
#define BJ_DET_MASK_SETTIME_OK      0x40	// 延期时间下载异常位

/* 雷管操作指令代码(先发送低位再发送高位)*/
#define BJ_CHECK_IN           0x09              // 点名
#define BJ_CLRALL			  0x19				// 全局复位
#define BJ_SERLRD			  0x29				// 序列码回读
#define BJ_DISCHARGE		  0x39				// 放电指令，对老芯片适用，新芯片没有
#define BJ_WREEPROM           0x49              // EE编程1
#define BJ_CHECK_OSC          0x59              // 测频
#define BJ_MASK               0x69              // 屏蔽
#define BJ_CLR                0x89              // 单个复位
#define BJ_TIMEWR			  0x99				// 设置延期时间
#define BJ_TIMERD			  0xA9				// 回读延期时间
#define BJ_BIT			      0xB9				// BIT检测指令
#define	BJ_ERASE_EEPROM		  0xC9				// 全片擦除
#define BJ_CHARGE			  0xD9				// 充电指令
#define BJ_TEST_CNT			  0xE9				// 延期计数器检测
#define BJ_EXPEN              0xf9              // 准备起爆


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
