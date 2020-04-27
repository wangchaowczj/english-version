#ifndef _BARCODE_H_
#define _BARCODE_H_

#include "System.h"

typedef struct
{
    u16 Box;//���
    u8 InBoxBig;//ÿ���ж��ٺ�
    u8 InBoxSmall;//ÿ���ж��ٷ�   
    u8 Factory;
	u8 Year;
    u8 Month;
    u8 Day;
}BOX_BIG_INFO;
typedef struct
{
    u16 Box;//�к�
    u8 InBoxSmall;//ÿ���ж��ٷ�   
    u8 Factory;
	u8 Year;
    u8 Month;
    u8 Day;
}BOX_SMALL_INFO;

typedef enum
{
	BARCODE_IDLE,//���ݿ���
	BARCODE_ERROR,//ɨ��õ������ݲ��ܰ�Э�����
	BARCODE_NEW,//ɨ��õ������ݿ��԰�Э�����
	BARCODE_WR_END//ɨ��õ��������Ѿ�д���׹�
}BARCODE_STATUS;

typedef enum
{
	BARCODE_PASS,//ɨ��õ���������������ͬ
	BARCODE_DET_TYPE_ERR,
	BARCODE_DET_LINE_TYPE_ERR,
	BARCODE_DET_LINE_LENGTH_ERR,
	BARCODE_FACTORY_ERR,
	BARCODE_PRODUCT_TIME_ERR,
	BARCODE_DELAY_ERR
}BARCODE_COMPARE_STATUS;
typedef struct
{
	BARCODE_STATUS Status;//����״̬
	BARCODE_COMPARE_STATUS Result;//
	U32_UNION DetSerl;//������
    U32_UNION DetPwd;//����
	u16 Delay;//����
	u8 DetType;//�׹�����
}BARCODE;

extern BARCODE BarCode;

extern u16 GetScanDataBytes(void);
extern void BarCodeDataAnalyst(void);

extern ErrorStatus BarCodeAnalyst17ForBoxBig(BOX_BIG_INFO* BoxBigInfo);
extern ErrorStatus BarCodeAnalyst18ForBoxSmall(BOX_SMALL_INFO* BoxSmallInfo);

#endif

