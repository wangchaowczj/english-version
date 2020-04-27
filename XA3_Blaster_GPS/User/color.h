#ifndef _COLOR_H_
#define _COLOR_H_

//24λ��ɫ��
#define RED             0xFF0000
#define GREEN           0x00FF00
#define BLUE			0x0000FF
#define BLACK			0x000000
#define WHITE			0xFFFFFF
#define YALLOW			0xFFFF00

//�Ի��򱳾�ɫ
#define COLOR_DLG_BK           0x70FFCA

//****�����������ʾʱ�䡢��صȵ���****//
#define COLOR_TOP_BACK			BLUE		//�����������ʾʱ�������ɫ
#define COLOR_TOP_FONT			WHITE		//�����������ʾʱ����ַ���ɫ
#define COLOR_TOP_HENG		   BLACK			//�����ж������м䲿�ֵķָ�����ɫ

//****������****//
#define COLOR_BAT_FONT			COLOR_WHITE			//��ص�ÿ������ɫ�Ϳ����ɫ
#define COLOR_BAT_BACK			COLOR_BLACK			//��صĵ�ɫ
#define COLOR_BAT_QIAN			COLOR_RED_1			//���Ƿ��ʱ��ÿ������ɫ

//****������USB�������****//
#define COLOR_BLU_FONT			COLOR_WHITE			//����������ɫ
#define COLOR_USB_FONT			COLOR_DARKGREEN		//usb������ɫ
#define COLOR_USB_CHAA			COLOR_RED_1			//usb��������ӵĲ����ɫ

//****�������ɫ��****//
#define COLOR_ZHU				0xD699//0x8FF9COLOR_WHITE			//���������Ϣ�б����ı���ɫ

//****F1��F4���****//
//#define SHOWCOLOR_F14_BACK			COLOR_YALLOW_1		//F14����ɫ��
#define COLOR_F14_BACK			COLOR_BLUE_2		//F14����ɫ��
#define COLOR_F14_FONT			COLOR_WHITE			//F14��������ɫ
#define COLOR_F14_HENG			COLOR_BLACK		//F14���м䲿�ֵķָ�����ɫ

//****���������****//
#define COLOR_IRO_BACK			COLOR_BLUE_2		//��������ÿ�������������ɫ
#define COLOR_IRO_XUAN			COLOR_GREEN			//��������ÿ�����ѡ�е���ɫ
#define COLOR_IRO_PRES			COLOR_BLUEAN		//��������ÿ����İ��µ���ɫ
#define COLOR_IRO_MEIY			COLOR_BLUEWU		//��������ÿ����Ĳ����ܳ��ֵ���ɫ
#define COLOR_IRO_FONT1			COLOR_BLACK			//��������ÿ���������ֵ���ɫ1
#define COLOR_IRO_FONT2			COLOR_WHITE			//��������ÿ���������ֵ���ɫ2
#define COLOR_IRO_LITI			COLOR_BLACK			//��������ÿ������������еıߵ���ɫ
#define COLOR_NAM_FONT			COLOR_BLACK			//��������ÿ���������Ӧ���ַ�����ɫ

//****��ʾ�������****//
#define COLOR_TIS_BACK			COLOR_YALLOW		//��ʾ�������ɫ��
#define COLOR_TIS_FONT			COLOR_BLACK			//��ʾ�����������ɫ
#define COLOR_TIS_HENG			COLOR_BLACK			//��ʾ����ĺ�����ɫ
#define COLOR_TIS_LITI			COLOR_BLACK			//��ʾ������������еıߵ���ɫ
#define COLOR_TIS_GOUU			COLOR_DARKGREEN		//��ʾ���湴����ɫ
#define COLOR_TIS_CHAA			COLOR_RED			//��ʾ��������ɫ
#define COLOR_INT_BACK			COLOR_WHITE			//��ʾ�������ݿ�ĵ�ɫ
#define COLOR_INT_FONT			COLOR_BLACK			//��ʾ�������ݿ����ֵ���ɫ
#define COLOR_INT_LITI			COLOR_BLACK			//��ʾ�������ݿ���������еıߵ���ɫ
#define COLOR_PRT_BACK			COLOR_WHITE			//��ʾ����������ĵ�ɫ
#define COLOR_PRT_FONT			COLOR_BLUE			//��ʾ���������ÿ�����ȵ���ɫ
#define COLOR_PRT_LITI			COLOR_BLACK			//��ʾ�����������������еıߵ���ɫ

//****��Ϣ�б�������****//
#define COLOR_INF_BACK			COLOR_WHITE			//��Ϣ�б�������ɫ��
#define COLOR_INF_FONT			COLOR_BLACK			//��Ϣ�б�����������ɫ
#define COLOR_INF_HENG			COLOR_BLUE			//��Ϣ�б����ĺ�����ɫ
#define COLOR_INF_XUAN			COLOR_BLUE			//��Ϣ�б�����ѡ��λ�õ���ɫ(������ʾ)
#define COLOR_INF_YIDO			COLOR_CYAN			//��Ϣ�б����ĵ����׹��ƶ�����ɫ(ѡ����ʾ)
#define COLOR_INF_XFON			COLOR_LIRED			//��Ϣ�б����ĵ����׹��ƶ�ʱ�������ɫ(ѡ����ʾ)
#define COLOR_PRI_BACK			COLOR_WHITE			//��Ϣ�б����Ľ������ĵ�ɫ
#define COLOR_PRI_FONT			COLOR_BLUE			//��Ϣ�б����Ľ�����ÿ�����ȵ���ɫ
#define COLOR_PRI_LITI			COLOR_BLACK			//��Ϣ�б����Ľ�������������еıߵ���ɫ
#define COLOR_INI_BACK			COLOR_WHITE			//��Ϣ�б��������ݿ�ĵ�ɫ
#define COLOR_INI_FONT			COLOR_BLACK			//��Ϣ�б��������ݿ����ֵ���ɫ
#define COLOR_INI_LITI			COLOR_BLACK			//��Ϣ�б��������ݿ���������еıߵ���ɫ
#define COLOR_SEI_LEFT			COLOR_BLUE			//��Ϣ�б��������õ�λ��ߵ���ɫ
#define COLOR_SEI_RIGT			COLOR_BLACK			//��Ϣ�б��������õ�λ�ұߵ���ɫ
#define COLOR_SEI_KUAN			COLOR_BLUE			//��Ϣ�б����ĵ�ǰλ�õĿ����ɫ
#define COLOR_SEI_TIAN			COLOR_BLUE			//��Ϣ�б����ĵ�ǰλ�õĿ��������ɫ
#define COLOR_TIM_XUAN			COLOR_WHITE			//��Ϣ�б��������ʱ��ʱ��ѡ��ʱ�������ɫ
#define COLOR_TIM_RECT			COLOR_BLUE			//��Ϣ�б��������ʱ��ʱ��ѡ�п����ɫ

//****�ػ����������****//
#define COLOR_CHO_BACK			COLOR_BLACK			//������ĵ�ط��ŵĵ�ɫ
#define COLOR_CHO_FONT			COLOR_WHITE			//������ĵ�ط��ŵĽ�����ɫ

#endif



























