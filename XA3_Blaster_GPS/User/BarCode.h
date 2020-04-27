#ifndef _BARCODE_H_
#define _BARCODE_H_

#include "System.h"

typedef struct
{
    u16 Box;//箱号
    u8 InBoxBig;//每箱有多少盒
    u8 InBoxSmall;//每盒有多少发   
    u8 Factory;
	u8 Year;
    u8 Month;
    u8 Day;
}BOX_BIG_INFO;
typedef struct
{
    u16 Box;//盒号
    u8 InBoxSmall;//每盒有多少发   
    u8 Factory;
	u8 Year;
    u8 Month;
    u8 Day;
}BOX_SMALL_INFO;

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
	BARCODE_DET_LINE_TYPE_ERR,
	BARCODE_DET_LINE_LENGTH_ERR,
	BARCODE_FACTORY_ERR,
	BARCODE_PRODUCT_TIME_ERR,
	BARCODE_DELAY_ERR
}BARCODE_COMPARE_STATUS;
typedef struct
{
	BARCODE_STATUS Status;//数据状态
	BARCODE_COMPARE_STATUS Result;//
	U32_UNION DetSerl;//序列码
    U32_UNION DetPwd;//密码
	u16 Delay;//延期
	u8 DetType;//雷管类型
}BARCODE;

extern BARCODE BarCode;

extern u16 GetScanDataBytes(void);
extern void BarCodeDataAnalyst(void);

extern ErrorStatus BarCodeAnalyst17ForBoxBig(BOX_BIG_INFO* BoxBigInfo);
extern ErrorStatus BarCodeAnalyst18ForBoxSmall(BOX_SMALL_INFO* BoxSmallInfo);

#endif

