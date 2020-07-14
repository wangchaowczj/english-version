#ifndef _BARCODE_H_
#define _BARCODE_H_

#include "System.h"

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
	BARCODE_FACTORY_ERR,
	BARCODE_PRODUCT_TIME_ERR,
	BARCODE_FEATURE_ERR,
	BARCODE_DELAY_ERR,
	BARCODE_REPEAT_ERR,//�������ظ����
	BARCODE_PACK_ERR //����ͬһ��,ÿ��10��������ǰ12λ��ͬ
}BARCODE_COMPARE_STATUS;
typedef struct
{
	BARCODE_STATUS Status;//����״̬
	BARCODE_COMPARE_STATUS Result;//
	u8 DetCode[8];//����
	u16 Delay;//����
	u8 CodeExternal[14];//����
	u8 DetLineLength;//���߳���
	DET_LINE_TYPE_CFG DetLineType;
	u8 DetType;//�׹�����
	u8 FactoryCode;//��������
	u32 ProductTime;//����ʱ��,8λ��������60627
	u8 Feature;//������
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

