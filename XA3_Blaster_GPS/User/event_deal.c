#include "includes.h"
#include "display.h"
#include "lcd_show.h"
//#include "DeInfo.h"
#include "event_deal.h"
//#include "scanner.h"
#include "color.h"
#include <math.h>




u16 TempDataIn[20];				//�û����������(��ʱ����)
u16 TempStep = 0;				//�û���ʱ����
u16 TempOverTime = 0;			//���ڵȴ���ʱʱ�����ֵ
u8 USBDkeyCheck = 0;			//���ڼ����Կ������״̬��1-��飬0-����顣�����鵽��Կ�γ������п��ܻ�ֱ�ӳ�����ʾ��
u8 DelaySetState = 0;			//������������ʱ�����ڱ�ʾ�׼����ںͿ��������Ƿ���Ҫ����(BIT0��ʾ�׼����ڣ�BIT1��ʾ��������)
u8 DetVolTempState = 0;			//���׹�ע����׹�ͨ�Ž���ʱ����¼��ǰ���׹�ע�ỹ���׹�ͨ�š��������жϵ�ѹ�쳣ʱ�����ص��ĸ�����




//=================================================================================
//| �������� | Event_SetLcdKeyBrightTime
//|----------|---------------------------------------------------------------------
//| �������� | ���ü��̱������ʾʱ��
//|----------|---------------------------------------------------------------------
//| ������� | NowState���Ƿ��޸���Ļ����	1-�޸ģ�0-���޸�
//|----------|---------------------------------------------------------------------
//| ���ز��� | ��
//|----------|---------------------------------------------------------------------
//| ������� | ��д�ˣ�Ҧ����    ʱ�䣺2013-06-05  
//|----------|---------------------------------------------------------------------
//|   ��ע   | 
//|----------|---------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//=================================================================================
void Event_SetLcdKeyBrightTime(u8 NowState)
{
	if (AT45SetInfo.KeySwitch)		//���̱����
	{
		switch (AT45SetInfo.KeyTime)
		{
			case 1:
				NowKeyCount = (2*2);		//����ʱ��Ϊ2��
				break;
			case 2:
				NowKeyCount = (5*2);		//����ʱ��Ϊ5��
				break;
			case 3:
				NowKeyCount = (10*2);		//����ʱ��Ϊ10��
				break;
			case 4:
				NowKeyCount = (15*2);		//����ʱ��Ϊ15��
				break;
			case 5:
				NowKeyCount = (20*2);		//����ʱ��Ϊ20��
				break;
			case 6:
				NowKeyCount = (30*2);		//����ʱ��Ϊ30��
				break;
			case 7:
				NowKeyCount = (60*2);		//����ʱ��Ϊ1��
				break;
			case 8:
				NowKeyCount = 0xFFFF;		//����ʱ��Ϊ����
				break;
			default:
				NowKeyCount = 0xFFFF;
				break;
		}
		if (Charge_Light == 0)
		{
			KEY_LED_OPEN();					//�ڽ���ú���ʱ���ʹ��˼��̵�
		}
	}
	else
	{
		NowKeyCount = 0;			//�رձ���(����ر�ǰ����λΪ1���رպ󣬸�λΪ0)
	}

	if (NowState)
	{
		if (Charge_Light == 0)
		{
			CLcd_BrightCtrl(32-(AT45SetInfo.LcdBright*2)+1);
		}
	}

	switch (AT45SetInfo.LcdTime)
	{
		case 1:
			NowLcdCount = (5*2);		//����ʱ��Ϊ5��
			break;
		case 2:
			NowLcdCount = (10*2);		//����ʱ��Ϊ10��
			break;
		case 3:
			NowLcdCount = (15*2);		//����ʱ��Ϊ15��
			break;
		case 4:
			NowLcdCount = (20*2);		//����ʱ��Ϊ20��
			break;
		case 5:
			NowLcdCount = (30*2);		//����ʱ��Ϊ30��
			break;
		case 6:
			NowLcdCount = (45*2);		//����ʱ��Ϊ45��
			break;
		case 7:
			NowLcdCount = (60*2);		//����ʱ��Ϊ1��
			break;
		case 8:
			NowLcdCount = 0xFFFF;		//����ʱ��Ϊ����
			break;
		default:
			NowLcdCount = 0xFFFF;
			break;
	}




}







