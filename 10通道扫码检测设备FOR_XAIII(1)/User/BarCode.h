#ifndef _BARCODE_H_
#define _BARCODE_H_

#include "System.h"

typedef enum
{
	BARCODE_IDLE,//数据空闲
	BARCODE_ERROR,//扫码得到的数据不能按协议解析
	BARCODE_NEW,//扫码得到的数据可以按协议解析
	BARCODE_WR_END//扫码得到的数据已经写入雷管
}BARCODE_STATUS;

typedef enum
{
	BARCODE_PASS,//扫码得到的数据与设置相同
	BARCODE_DET_TYPE_ERR,
	BARCODE_FACTORY_ERR,
	BARCODE_PRODUCT_TIME_ERR,
	BARCODE_FEATURE_ERR,
	BARCODE_DELAY_ERR,
	BARCODE_REPEAT_ERR,//该内码重复检测
	BARCODE_PACK_ERR //不是同一捆,每捆10发，外码前12位相同
}BARCODE_COMPARE_STATUS;
typedef struct
{
	BARCODE_STATUS Status;//数据状态
	BARCODE_COMPARE_STATUS Result;//
	u8 DetCode[8];//内码
	u16 Delay;//延期
	u8 CodeExternal[14];//外码
	u8 DetLineLength;//脚线长度
	DET_LINE_TYPE_CFG DetLineType;
	u8 DetType;//雷管类型
	u8 FactoryCode;//工厂代码
	u32 ProductTime;//生产时间,8位整数，如60627
	u8 Feature;//特征号
}BARCODE;

extern BARCODE BarCode;

#define FACTORY_CODE_SUPPORT_MAX  15
extern u8 FactoryCodeSupport[];

extern ErrorStatus FactoryCodeInsideToOutside(u8 Inside, u8 *Outside);
extern ErrorStatus FactoryCodeOutsideToInside(u8 Outside, u8* Inside);

extern void BarCodeDataAnalyst(void);
extern ErrorStatus XA2_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd);
extern ErrorStatus XA2_DetInside2Outside(u32 Serl, u32 Pwd, u8* Ptr);
//extern u8 DetCodeRulePass(u32 Serl, u32 Pwd);

#endif

