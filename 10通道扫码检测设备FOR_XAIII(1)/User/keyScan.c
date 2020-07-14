#include "includes.h"
#include "GuiCustomUtils.h"

//static ErrorStatus PowerOffKeyState = ERROR;

//==================================================================================================
//| �������� | KeyConfig 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����ɨ��ģ������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void KeyConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***ʱ������***/
	RCC_AHB1PeriphClockCmd(KEY_OUT0_PORT_RCC | KEY_OUT1_PORT_RCC |KEY_OUT2_PORT_RCC | KEY_OUT3_PORT_RCC | 
		KEY_IN0_PORT_RCC | KEY_IN1_PORT_RCC | KEY_IN2_PORT_RCC | KEY_IN3_PORT_RCC  |
		KEY_PW_PORT_RCC | KEY_FOOT_PORT_RCC , ENABLE);
	
	/***�������Ŷ������10K��������***/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 


	GPIO_InitStructure.GPIO_Pin = KEY_OUT0_PIN; 
	GPIO_Init(KEY_OUT0_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT1_PIN; 
	GPIO_Init(KEY_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_OUT2_PIN; 
	GPIO_Init(KEY_OUT2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN0_PIN; 
	GPIO_Init(KEY_IN0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_IN1_PIN; 
	GPIO_Init(KEY_IN1_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_IN2_PIN; 
	GPIO_Init(KEY_IN2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_IN3_PIN; 
	GPIO_Init(KEY_IN3_PORT, &GPIO_InitStructure);
	
	/***��Դ��״̬***/
	GPIO_InitStructure.GPIO_Pin = KEY_PW_PIN;	
	GPIO_Init(KEY_PW_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_FOOT_PIN;	
	GPIO_Init(KEY_FOOT_PORT, &GPIO_InitStructure);		
}

//==================================================================================================
//| �������� | KeyRowOutputLow 
//|----------|--------------------------------------------------------------------------------------
//| �������� | 4X3���̾��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | �����״̬
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void KeyRowOutput(BitAction pin_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//�Ȱ�������Ϊ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_IN0_PIN; 
	GPIO_Init(KEY_IN0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_IN1_PIN; 
	GPIO_Init(KEY_IN1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN2_PIN; 
	GPIO_Init(KEY_IN2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN3_PIN; 
	GPIO_Init(KEY_IN3_PORT, &GPIO_InitStructure);	
	
	//��������Ϊ���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_OUT0_PIN; 
	GPIO_Init(KEY_OUT0_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT1_PIN; 
	GPIO_Init(KEY_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_OUT2_PIN; 
	GPIO_Init(KEY_OUT2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);
	
	SET_KEY_ROW0(pin_state);
	SET_KEY_ROW1(pin_state);
	SET_KEY_ROW2(pin_state);
	SET_KEY_ROW3(pin_state);
}

//==================================================================================================
//| �������� | ReadKeyRow 
//|----------|--------------------------------------------------------------------------------------
//| �������� | 4X3���̾����������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u8 ReadKeyRow(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u8 temp = 0;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_OUT0_PIN; 
	GPIO_Init(KEY_OUT0_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT1_PIN; 
	GPIO_Init(KEY_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_OUT2_PIN; 
	GPIO_Init(KEY_OUT2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);	
	if(GET_KEY_ROW0() != KEY_DOWN)
	{
		temp |= 0x01;
	}
	if(GET_KEY_ROW1() != KEY_DOWN)
	{
		temp |= 0x02;
	}
	if(GET_KEY_ROW2() != KEY_DOWN)
	{
		temp |= 0x04;
	}
	if(GET_KEY_ROW3() != KEY_DOWN)
	{
		temp |= 0x08;
	}	
	return temp;
}

//==================================================================================================
//| �������� | KeyRowOutputLow 
//|----------|--------------------------------------------------------------------------------------
//| �������� | 4X3���̾��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | �����״̬
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void KeyVerticalOutput(BitAction pin_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//�Ȱ�������Ϊ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_OUT0_PIN; 
	GPIO_Init(KEY_OUT0_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT1_PIN; 
	GPIO_Init(KEY_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_OUT2_PIN; 
	GPIO_Init(KEY_OUT2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);
	
	//�ٰ�������Ϊ���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_IN0_PIN; 
	GPIO_Init(KEY_IN0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_IN1_PIN; 
	GPIO_Init(KEY_IN1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN2_PIN; 
	GPIO_Init(KEY_IN2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN3_PIN; 
	GPIO_Init(KEY_IN3_PORT, &GPIO_InitStructure);	
	
	SET_KEY_VERTICAL0(pin_state);
	SET_KEY_VERTICAL1(pin_state);
	SET_KEY_VERTICAL2(pin_state);	
	SET_KEY_VERTICAL3(pin_state);		
}

//==================================================================================================
//| �������� | ReadKeyVertical 
//|----------|--------------------------------------------------------------------------------------
//| �������� | 4X3���̾����������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u8 ReadKeyVertical(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u8 temp = 0;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_InitStructure.GPIO_Pin = KEY_IN0_PIN; 
	GPIO_Init(KEY_IN0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_IN1_PIN; 
	GPIO_Init(KEY_IN1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN2_PIN; 
	GPIO_Init(KEY_IN2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN3_PIN; 
	GPIO_Init(KEY_IN3_PORT, &GPIO_InitStructure);	

	if(GET_KEY_VERTICAL0() != KEY_DOWN)
	{
		temp |= 0x10;
	}
	if(GET_KEY_VERTICAL1() != KEY_DOWN)
	{
		temp |= 0x20;
	}
	if(GET_KEY_VERTICAL2() != KEY_DOWN)
	{
		temp |= 0x40;
	}
	if(GET_KEY_VERTICAL3() != KEY_DOWN)
	{
		temp |= 0x80;
	}	
	return temp;
}

//==================================================================================================
//| �������� | MatrixKeyScan 
//|----------|--------------------------------------------------------------------------------------
//| �������� | 4X4���̾���ɨ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u8 MatrixKeyScan(void)
{
	u8 row_code = 0, vertical_code = 0;
	
	KeyRowOutput(Bit_RESET);          //������͵�ƽ
	row_code = ReadKeyVertical();     //���е�ƽ״̬
	KeyVerticalOutput(Bit_RESET);     //������͵�ƽ
	vertical_code = ReadKeyRow();     //���е�ƽ״̬
	return (row_code | vertical_code);
}


//==================================================================================================
//| �������� | KeyCodeToKeyMsg 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����ɨ�赽�İ�������ת��Ϊ��Ӧ��ϵͳ������Ϣ
//|----------|--------------------------------------------------------------------------------------
//| ������� | keycode ��������  msg ת�������Ϣָ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 0  δ�ҵ���Ӧ�İ�����Ϣ
//|          | 1  ת���ɹ�
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u8 KeyCodeToKeyMsg(u16 keycode)
{
	u8 code = 0;
	
	switch(keycode)//��������ת��Ϊ��Ӧ��ϵͳ������Ϣ
	{
		case KEY0_CODE:
			code = GUI_CUSTOM_KEY_0;
			break;
		case KEY1_CODE:
			code = GUI_CUSTOM_KEY_1;
			break;
		case KEY2_CODE:
			code = GUI_CUSTOM_KEY_2;
			break;
		case KEY3_CODE:
			code = GUI_CUSTOM_KEY_3;
			break;
		case KEY4_CODE:
			code = GUI_CUSTOM_KEY_4;
			break;
		case KEY5_CODE:
			code = GUI_CUSTOM_KEY_5;
			break;
		case KEY6_CODE:
			code = GUI_CUSTOM_KEY_6;
			break;
		case KEY7_CODE:
			code = GUI_CUSTOM_KEY_7;
			break;
		case KEY8_CODE:
			code = GUI_CUSTOM_KEY_8;
			break;		
		case KEY9_CODE:
			code = GUI_CUSTOM_KEY_9;
			break;	
		case KEY_BACK_CODE:
			code = GUI_CUSTOM_KEY_BACK;
			break;	
		case KEY_OK_CODE:
			code = GUI_CUSTOM_KEY_OK;
			break;
		case KEY_F1_CODE:
			code = GUI_CUSTOM_KEY_F1;
			break;
		case KEY_F2_CODE:
			code = GUI_CUSTOM_KEY_F2;
			break;
		case KEY_F3_CODE:
			code = GUI_CUSTOM_KEY_F3;
			break;
		case KEY_F4_CODE:
			code = GUI_CUSTOM_KEY_F4;
			break;			
		default:
			break;			
	}
	return code;
}

//void SetPowerOffKeyState(void)
//{
//	PowerOffKeyState = SUCCESS;
//}
//void ResetPowerOffKeyState(void)
//{
//	PowerOffKeyState = ERROR;
//}
//ErrorStatus GetPowerOffKeyState(void)
//{
//	return PowerOffKeyState;
//}

u8 KeyFootScan(void)
{
	static u8 KeyDownCount = 0;
	
	if (GET_KEY_FOOT_STATE() == KEY_DOWN)
	{
		if(KeyDownCount != 0xFF)
		{
			KeyDownCount++;
			if(KeyDownCount >= 50)
			{
				KeyDownCount = 0xFF;
				return 1;
			}		
		}
	}
	else
	{
		KeyDownCount = 0;
	}
	return 0;
}
//�°�Ӳ���У�ԭ��̤���ؽ�����װ��ʱ�Ѿ�������ֱ��ʹ��Ϊ������̩�Խ�ʱ�����Ľӿڡ�
//Ҳ����GPIOF1�����ã�����ģʽʱ��Ϊ�жϷ�ʽ��������ʱ������ͨ��̤���ؽ���ɨ��ʹ�á�
u8 KeyFoot2Scan(void)
{
	static u8 KeyDownCount = 0;
	
	if (GET_MCU_IO_IN1_PIN() == KEY_DOWN)
	{
		if(KeyDownCount != 0xFF)
		{
			KeyDownCount++;
			if(KeyDownCount >= 50)
			{
				KeyDownCount = 0xFF;
				return 1;
			}		
		}
	}
	else
	{
		KeyDownCount = 0;
	}
	return 0;
}
u8 KeyPowerScan(void)
{
	static u8 KeyDownCount = 0;
	
	if (GET_PW_INT_STATE() == 0)
	{
		if(KeyDownCount != 0xFF)
		{
			KeyDownCount++;
			if(KeyDownCount >= 50)
			{
				KeyDownCount = 0xFF;
				return 1;
			}		
		}
	}
	else
	{
		KeyDownCount = 0;
	}
	return 0;
}
//==================================================================================================
//| �������� | KeyScan 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ����ɨ�躯��������ɨ����������������Ϣ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | �����ɵδ�ʱ��ÿ��1ms����1�Σ�̧�𴥷�,֧�ֵ�����1��3˫��ɨ��
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
u8 KeyboardScan(void)
{
	static u8 LateKeyCode = 0 ; //������һ�εĴ������
	static u8 DlyCount = 0;	
	u8 CurrentKeyCode = 0;
	u8 KeyMsg;
	
	if(KeyFootScan())
	{
		return GUI_CUSTOM_KEY_FOOT;
	}
    if(UserParam.NetMode == 0)
    {
        if(KeyFoot2Scan())
        {
            return GUI_CUSTOM_KEY_FOOT;
        }     
    }
   
	if(KeyPowerScan())
	{
		return GUI_CUSTOM_KEY_POWER;	
	}
	CurrentKeyCode = MatrixKeyScan(); //����ɨ��
	if((CurrentKeyCode != 0xFF) && (LateKeyCode == CurrentKeyCode))//�м����������ϴμ�⵽����ͬ
	{
		KeyMsg = KeyCodeToKeyMsg(CurrentKeyCode);
		if(0 != KeyMsg)//�Ѱ�������ת��Ϊ������Ϣ
		{//��Ч����					
			if(DlyCount < 50)
			{
				DlyCount++;
			}
			else if(DlyCount == 50)//�������º�50ms����������Ϣ
			{
				DlyCount = 0xFF;					
				return KeyMsg;
			}
			else
			{
				;
			}
			return 0;
		}
	}
	LateKeyCode = CurrentKeyCode;
	DlyCount = 0;

	return 0;
}

