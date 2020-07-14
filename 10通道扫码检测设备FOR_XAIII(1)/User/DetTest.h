#ifndef _DETTEST_H_
#define _DETTEST_H_

#define DET_LV_CURRENT_TEST_MASK_BIT        	 		0x00000001UL  //低压下静态电流测试选择位
#define DET_BIT_TEST_MASK_BIT        			 		0x00000002UL  //BIT测试选择位
#define DET_CHARGE_TEST_MASK_BIT        		 		0x00000004UL  //充电测试选择位
#define DET_CHARGE_HV_CURRENT_TEST_MASK_BIT      		0x00000008UL  //充电后高压电流测试选择位

typedef struct
{
	u16 det_lv_current_min;   		/**<低压下电流最小值，单位0.01uA*/
	u16 det_lv_current_max;   		/**<低压下电流最大值，单位0.01uA*/
	u16 det_lv_current_time_out;    /**<低压下电流测试超时时间，单位0.1秒*/
	u16 det_hv_current_min;   		/**<高压下电流最小值，单位0.01uA*/
	u16 det_hv_current_max;	  		/**<高压下电流最大值，单位0.01uA*/
	u16 det_hv_current_time_out;	/**<高压下电流测试超时时间，单位0.1秒*/
	u32 det_freq_min;         		/**<芯片频率最小值，单位HZ*/
	u32 det_freq_max;         		/**<芯片频率最大值，单位HZ*/	
	u32 test_ctrl;            		/**<测试项控制位*/    
}TEST_PARAM;

extern TEST_PARAM test_param;
extern TEST_PARAM test_param_cfg[];//0为半成品，1为成品



#define DET_SINGLE_CAP_MASK_BIT   	0x04 //芯片类型中用1位表示单电容




#define DET_PWD_FIX_VALUE   0xFFFE   //芯片密码固定值





typedef enum
{
	DET_PASS = 0x00,
	LV_CURRENT_LESS,               //低压下正向电流小
	LV_CURRENT_MORE,               //低压下正向电流大
	LV_REVERSE_CURRENT_LESS,       //低压下反向电流小
	LV_REVERSE_CURRENT_MORE,	   //低压下反向电流大
	DET_INFO_READ_ERR,             //序列码读取错误 
	DET_FREQ_MIN,                  //频率小
	DET_FREQ_MAX,                  //频率大
	DET_TYPE_ERR,                  //雷管类型错误
	DET_BRIDGE_SHORT,              //桥头短路
	DET_BIT_STATUS_READ_ERR,       //测BIT时状态回读错误
	DET_BIT_ERR,                   //BIT错误	
	DET_NO_WORK_CAP,               //没有工作电容
	DET_WRITE_DELAY_ERR,		   //写延期指令错误
	DET_PASSWORD_ERR,              //芯片密码错误
	DET_CHARGE_STATUS_READ_ERR,    //测充电时状态回读错误
	DET_CHARGE_ERR,                //充电错误
	DET_NO_RESPONSE_EXPEN,         //准备起爆指令响应错误   
	DET_EXPEN_TIMERD_ERR,          //测准备起爆时延期时间回读错误
	DET_EXPEN_ERR,                 //准备起爆错误	
	HV_CURRENT_LESS,               //高压下正向电流小
	HV_CURRENT_MORE,               //高压下正向电流大
	HV_REVERSE_CURRENT_LESS,       //高压下反向电流小
	HV_REVERSE_CURRENT_MORE,       //高压下反向电流大
    LV_VOLTAGE_MIN,                //总线低压时电压过低	
    LV_VOLTAGE_MAX,                //总线低压时电压过高
    HV_VOLTAGE_MIN,                //总线高压时电压过低	
    HV_VOLTAGE_MAX,                //总线高压时电压过高
	DET_WR_CODE_NULL_ERR,          //写内码时没有内码(程序逻辑出问题)
	DET_WR_CODE_UNLOCK_PWD_ERR,    //写内码时解密错误
	DET_WR_CODE_UNLOCK_EEPROM_ERR, //写内码时解EEPROM错误
	DET_WR_CODE_WR_FIXED_ERR,      //写内码时写延期值错误
	DET_WR_CODE_ERR,			   //写EEPROM错误
	DET_WR_CODE_READ_ERR,		   //写内码后回读序列码错误
	DET_WR_CODE_VERIFY_ERR,        //写内码后读出的数据与写入不一致
	DET_CODE_REPEAT_TEST,          //该内码已经检测过了
	DET_FIX_DELAY_READ_ERR,        //延期时间回读错误
	DET_FIX_DELAY_NOT_EQUAL,       //延期时间与用户设置的不一致,仅限单电容芯片
	DET_WR_TYPE_ERR,               //修改雷管类型错误
	DET_WR_TYPE_READ_ERR,          //修改雷管类型后读出的数据与写入不一致
	DET_SHORT_ERR,			       //雷管短路
	DET_OPEN_CIRCUIT_ERR,          //雷管断路
	DET_CODE_ALL_ZERO,             //内码全0
	DET_CODE_NOT_SAME,             //雷管与条码不一致
    DET_BIT_NO_RESPONSE,           //BIT命令没有响应
    DET_CHARGE_NO_RESPONSE,        //充电命令没有响应
    DET_PASSWORD_READ_ERR,         //PWD读取错误	
	DET_PASSWORD_NOT_ACK,          //发送解密指令无响应
    DET_CODE_RULE_ERR,             //内码不符合规则(XA2)
    DET_READ_EEPROM_ERR,           //读EEPROM出错
    DET_CODE_BACKUP_ERR1,          //DATA1-DATA4序列码备份数据错误
    DET_CODE_BACKUP_ERR2,          //DATA5-DATA8密码备份数据错误
    DET_TEST_RECORDER_DATA_ERR2,   //DATA9-DATA12数据错误
	DET_TEST_RECORDER_CRC_ERR,     //检测记录校验错误(XA2)        
    DET_TEST_RECORDER_ERR1,        //没有经过I型检测(XA2)
    DET_TEST_RECORDER_ERR2,        //没有经过II型检测(XA2)
	DET_TEST_RECORDER_ERR3,        //没有经过半成品检测(XA2)
	DET_TEST_RECORDER_CRC_DIFFERENT_ERR,   //两次读记录不同
	DET_TEST_RECORDER_REPEAT_ERR, //半成品重复检测(XA2)
//	DET_TEST_RECORDER_REPEAT_ERR2, //成品重复检测(XA2)
	
	DET_TESTING = 0xFF             //测试中
}DET_ERR_CODE;

typedef struct
{
	DET_ERR_CODE Status;      //测试错误代码
    u8 ser1;				 // 序列号1
	u8 ser2;				 // 序列号2
	u8 ser3;				 // 序列号2
	u8 fac;					 // 工厂号
	u8 bat;				     // 批号1
	u8 year;				 // 年号
	u8 Pwd1;                 //密码
	u8 Pwd2;                 //密码
	u32 freq;                //频率,单位Hz
	u8 type;                 //芯片类型
	u16 lv_current;          //低压下正向电流值
	u16 lv_reverse_current;  //低压下反向电流值
	u16 hv_current;          //高压下正向电流值
	u16 hv_reverse_current;	 //高压下反向电流值
}DET_INFO;
typedef union
{
    u32 U32Value;
    u8  U8Buffer[4];
}U32_UNION;
typedef struct
{
	DET_ERR_CODE Status;      //测试错误代码
    U32_UNION Serl;	 //序列码
    U32_UNION Pwd;	 //密码
	u32 freq;                //频率,单位Hz
	u16 lv_current;          //低压下正向电流值
	u16 lv_reverse_current;  //低压下反向电流值
	u16 hv_current;          //高压下正向电流值
	u16 hv_reverse_current;	 //高压下反向电流值*
}XA2_DET_INFO;
#define DET_CHANNEL_MAX   10
typedef struct
{
	u8 Count;
//	DET_INFO Info[DET_CHANNEL_MAX];
    XA2_DET_INFO XA2_Info[DET_CHANNEL_MAX];	
	BARCODE BarCode[DET_CHANNEL_MAX];
	DET_ERR_CODE OldStatus[DET_CHANNEL_MAX];
    DET_ERR_CODE NewStatus[DET_CHANNEL_MAX];
}CHANNEL;
extern CHANNEL Channel;


#define DET_PWD_FIX_VALUE   0xFFFE   //芯片密码固定值

extern void ClearDetInfo(void);
extern void ClearNewStatus(void);
extern ErrorStatus WaitCurrentReady(u8 CurrentValue, u16 TimeOut);
extern ErrorStatus WaitRxPinReady(u16 TimeOut);
extern ErrorStatus WaitRxPinHigh(u16 TimeOut);
extern void AllChannelLinShortTest(u8 type);
extern void AllChannelOpenCircuitTest(u8 type);
extern u8 TestParamIsSet(u32 MaskBit);
extern void AllChannelCurrentTest(u8 ReverseFlag);
extern void AllChannelVoltageTest(u8 Flag);
extern void AllChannelDetFreqTest(void);
extern u8 AllChannelTestEnd(void);
extern u8 AllChannelTestEndFromBuffer(DET_ERR_CODE* Status);
extern void AllChannelDetHVCurrentTest(u8 ReverseFlag);
extern u8 GetChannelStatusCount(DET_ERR_CODE Status);
extern u8 GetChannelNewStatusCount(DET_ERR_CODE Status);
extern void AllChannelStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus);
extern void AllChannelNewStatusReplace(DET_ERR_CODE OldStatus, DET_ERR_CODE NewStatus);
extern void SoundActionByStatus(void);
extern u32 GetDetFreqFromTimerCount(u16 count);
extern u8 AllChannelIsOpenCircuit(void);
extern void SetAllChannelBarCodeIdle(void);
//extern void XA_AllChannelDetCodeAllZeroVerify(void);
extern u8 DetCodeIsAllZero(DET_INFO* Det);

extern void DetTest(void);
extern u16 DetAnalyst(void);
extern u16 XA2_DetAnalyst(void);
extern void QueryDetConnectStatus(void);
extern void WaitAllChannelDetOff(void);

#endif
