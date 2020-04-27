#ifndef _EB_DET_CMD_H_
#define _EB_DET_CMD_H_


#define DET_CMD_SUCCESS	   0   //指令执行成功
#define DET_CMD_FAIL       1   //指令执行失败
#define DET_TIMERWR_FAIL   2   //测频时写延时错误

/* 定义各个状态位的掩码 */
#define EB_DET_MASK_TEST_ERR        0x04    // bit异常位
#define EB_DET_MASK_CHARGE          0x08	// 充电异常位
#define EB_DET_MASK_DISCHARGE       0x10	// 放电异常位
#define EB_DET_MASK_WAIT_EXPEN      0x20	// 准备起爆异常位
#define EB_DET_MASK_SETTIME_OK      0x40	// 延期时间下载异常位


/* 雷管操作指令代码(先发送低位再发送高位)*/
#define CHECK_IN          0x09              // 点名
#define CLRALL			  0x19				// 全局复位
#define SERLRD			  0x29				// 序列码回读
#define DISCHARGE		  0x39				// 放电指令，对老芯片适用，新芯片没有
#define CHECK_OSC         0x59              // 测频
#define MASK              0x69              // 屏蔽
#define CLR               0x89              // 单个复位
#define TIMEWR			  0x99				// 设置延期时间
#define TIMERD			  0xA9				// 回读延期时间
#define BIT1			  0xB9				// BIT检测1指令
#define	BIT2			  0xC9				// BIT检测2指令
#define CHARGE			  0xD9				// 充电指令
#define EXPEN             0xf9              // 准备起爆


/*
* 对所有在线雷管发送复位指令
*/
extern void EB_DetClrAll(void);

/*
* 屏蔽雷管
* 返回值：
*        DET_CMD_SUCCESS-雷管屏蔽成功
*        DET_CMD_FAIL-雷管屏蔽失败
*/
extern u8 EB_DetMask(const u8 *psn);

/*
* 屏蔽雷管
* psn：雷管序列号指针
* retry: 重试次数
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetMaskTry(const u8 *psn, u8 retry);

/*
* 检查雷管是否在线
* 返回值：
*        DET_CMD_SUCCESS-有雷管在线
*        DET_CMD_FAIL-无雷管在线
*/
extern u8 EB_DetCheckIn(void);


/*
* 读取雷管序列码
* buf：雷管序列码缓存
* 返回值：
*        DET_CMD_SUCCESS-序列码读取成功
*        DET_CMD_FAIL-序列码读取失败
*/
extern u8 EB_DetSerlRD(u8 *buf);


/*
* 读取雷管序列码
* buf：雷管序列码缓存
* retry  读取次数
* 返回值：
*        DET_CMD_SUCCESS-序列码读取成功
*        DET_CMD_FAIL-序列码读取失败
*/
extern u8 EB_DetSerlRDTry(u8 *buf, u8 retry);


/*
* 雷管桥头检测
* psn：雷管序列号指针
* 返回值：
*        DET_CMD_SUCCESS-桥头检测成功
*        DET_CMD_FAIL-桥头检测失败
*/
extern u8 EB_DetBit1(const u8 *psn);

/*
* 雷管桥头检测+重试
* psn：雷管序列号指针
* retry：重试次数
* 返回值：
*        DET_CMD_SUCCESS-桥头检测成功
*        DET_CMD_FAIL-桥头检测失败
*/
extern u8 EB_DetBit1Try(const u8 *psn, u8 retry);

/*
* 延时回读指令
* psn：雷管序列号指针
* buf: 延时回读返回结果，共4个字节
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetTimeRD(const u8 *psn, u8 *buf);

/*
* 延时回读指令+重试
* psn：雷管序列号指针
* buf: 延时回读返回结果，共4个字节
* retry：重试次数，不小于3，否则得不到正确的结果
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetTimeRDTry(const u8 *psn, u8 *buf, u8 retry, u16 Delay);//extern u8 EB_DetTimeRDTry(const u8 *psn, u8 *buf, u8 retry);
extern u8 EB_DetTimeRDTryErrorTry(const u8 *psn, u8 *buf, u8 retry, u8 ErrorRetryFlag);
/*
* 写延期指令
* psn：雷管序列号指针
* scode: 雷管密码
* delay: 延时时间
* bit4clk:64个CLK对应的定时器计数值
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetTimeWR(const u8 *psn, u8 scode, u16 delay, u32 freq, u32* det_dly_clk);

/*
* 写延期指令+重试
* psn：雷管序列号指针
* scode: 雷管密码
* delay: 延时时间
* freq:  芯片频率
* bit4clk:64个CLK对应的定时器计数值
* retry：重试次数
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetTimeWRTry(const u8 *psn, u8 scode, u16 delay, u32 freq, u32* det_dly_clk, u8 retry);

/*
* 测频指令
* psn：雷管序列号指针
* scode: 雷管密码
* buf: 返回数据指针频率(单位HZ)
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
extern u8 EB_DetCheckFreq(const u8 *psn, u8 scode, u32* freq);

/*
* 充电指令
* psn：雷管序列号指针
* scode: 雷管密码
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
//extern u8 EB_DetCharge(const u8 *psn, u8 scode);
extern u8 EB_DetCharge(const u8 *psn, u8 scode, u8 cmd_charge);
/*
* 充电指令+重试
* psn：雷管序列号指针
* scode: 雷管密码
* retry：重试次数
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
//extern u8 EB_DetChargeTry(const u8 *psn, u8 scode, u8 retry);
extern u8 EB_DetChargeTry(const u8 *psn, u8 scode, u8 cmd_charge, u8 retry);

/*
* 准备起爆指令
* psn：雷管序列号指针
* scode: 雷管密码
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
//extern u8 EB_DetExpen(const u8 *psn, u8 scode);
extern u8 EB_DetExpen(const u8 *psn, u8 scode, u8 cmd_expen);
/*
* 准备起爆指令
* psn：雷管序列号指针
* scode: 雷管密码
* 返回值：
*        DET_CMD_SUCCESS-延时回读成功
*        DET_CMD_FAIL-延时回读失败
*/
//extern u8 EB_DetExpenTry(const u8 *psn, u8 scode, u8 retry);
extern u8 EB_DetExpenTry(const u8 *psn, u8 scode, u8 cmd_expen, u8 retry);
#endif /* _EB_DET_CMD_H_ */
