#ifndef _DET_OPERART_H_
#define _DET_OPERART_H_

#define BLAST_DELAY_WHOLE_ADD_VALUE			72	//整体增加一个延期

#define DET_SERL_GROUP_INFO_MAX   32
typedef struct
{
	u16 Count;
	u8 Serl;
}DET_SERL_GROUP;
typedef struct
{
	u8 Count;
	DET_SERL_GROUP Group[DET_SERL_GROUP_INFO_MAX];
}DET_SERL_GROUP_INFO;
extern DET_SERL_GROUP_INFO DetSerlGroupInfo;

typedef enum 
{
	DET_PASS,			  //检测到雷管
	COMM_ERR, 	          //通信出错
	TEST_ERR,			  //BIT错误 
	SERL_NULL_ERR,        //雷管还没有写内码
	SECU_CODE_ERR,		  //密码校验错误
	FREQ_MIN,			  //频率过小
	FREQ_MAX,			  //频率过大
	OVERLAPED,			  //上线过程中这个序列号已经注册 
	TYPE_ERR,			  //雷管类型错误
	CURRENT_MIN,		  //电流小
	CURRENT_MAX,		  //电流大
	CURRENT_UNSTABLE,      //电流不稳定
    DET_CODE_ERR,          //内码不符合规则2018年3月25日
    DET_FAC_CODE_ERR,     //雷管工厂代码与设备不一致
    NEED_REPEAT           //需要重新注册
}CHECK_RESULT;

typedef enum
{
	TEST_END,             //测试完成
	UNREG_DET             //测试完成，有未注册雷管
}TEST_RESULT;


#define DET_MAX_SIZE         400       //最大带载雷管数量
typedef struct
{
	u16 Name;//对应的孔号,从1开始
	u8  Inside;//对应的孔内部序号,从1开始
}HOLE;
typedef union
{
    u32 U32Value;
    u8  U8Buffer[4];
}U32_UNION;

typedef struct
{
	U32_UNION Serl;	 //序列码
    U32_UNION Pwd;	 //密码	
	U32_UNION Freq;	 //频率	
	U32_UNION Delayclk;//真正写入芯片的延期周期
    u16 Delay;   //延期
	u8  Err;	 //错误代码
    u8 RetryErr; //重试错误代码
	HOLE Hole;   //对应的孔信息	
}DET_INFO;

typedef struct
{
	u16 RegCount;    /**<注册雷管数*/
	u16 ErrorCount;  /**<错误雷管数*/	
	u16 DlyMax;      /**<最大延期值*/
	u8 Type;         /**<雷管类型*/
	u8 HoleNumber[DET_MAX_SIZE];/**<每个孔的设计孔内雷管数*/
	DET_INFO Info[DET_MAX_SIZE];/**<雷管信息*/
}DET;

extern DET Det;


#define DET_ANALYST_READ_SN_MASK_BIT      	0x0001
#define DET_ANALYST_BIT_RESULT_MASK_BIT   	0x0002
#define DET_ANALYST_WRITE_DELAY_MASK_BIT  	0x0004
#define DET_ANALYST_FREQ_MASK_BIT         	0x0008
#define DET_ANALYST_UNLOCK_MASK_BIT       	0x0010      
#define DET_ANALYST_CHARGE_MASK_BIT       	0x0020


#define DET_TEST_FREQ_MIN_ERR      			0x01//频率小
#define DET_TEST_FREQ_MAX_ERR      			0x02//频率大
#define DET_TEST_CONNECT_ERR       			0x03//未连接
#define DET_TEST_BIT_ERR           			0x04//BIT错误
#define DET_TEST_DELAY_CLOCK_ERR   			0x05//延期值与写入值不一致
#define DET_TEST_CHARGE_ERR        			0x06//充电错误
#define DET_TEST_EXPEN_ERR         			0x07//准备起爆错误
#define DET_TEST_PWD_ERR         			0x08//密码错误
#define DET_TEST_COMM_ERR                   0x09//通讯错误(有返回信号,但无法解析)
#define DET_TEST_TYPE_ERR                   0x0A//类型错误

extern ErrorStatus XA3_DetInside2Outside(u32 Serl, u32 Pwd, u8* Ptr);
extern ErrorStatus XA3_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd);
extern u8 DetCodeRulePass(u32 Serl, u32 Pwd);
extern ErrorStatus CheckOverlap(const u8* Serl);
extern void AllReset(void);
extern void AllDisCharge(void);
extern ErrorStatus GetDetRegStatus(u16 Index);
extern void GetDetNum(void);
extern void TestInfoInit(void);
extern u8 GetDetType(u32 Serl, u32 Pwd);
extern u8 GetDetFactoryCode(u32 Serl, u32 Pwd);
extern u8 DetIsSingleCap(u8 Type);
extern CHECK_RESULT CheckDetFreq(u32 freq, u8 check_state);
extern ErrorStatus CheckDetType(u32 Serl, u32 Pwd);
extern u16 DetAnalyst(DET_INFO* det_buffer);
extern CHECK_RESULT CheckDet(DET_INFO* det_buffer, u8 Reg);
extern TEST_RESULT TestDetQuick(void);
extern void TestDetComplete(void);
extern void BlasteTestComplete(void);
extern u8 XA3_DetCodeWriteNew(U32_UNION* NewSerl, U32_UNION* NewPwd);
extern u8 XA3_DetCodeWriteFromBackup(void);

extern TEST_RESULT DetMaskAll(void);
extern void GetDetAckCurrentWave(void);
extern u16 GetRetryErrCount(void);
extern u8 HaveUnregisteredDetBySimpleSignal(void);
extern ErrorStatus AllDetStartBlast(void);
extern ErrorStatus CreateStartBlastTask(void);

#endif
