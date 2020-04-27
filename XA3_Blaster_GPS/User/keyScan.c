#include "includes.h"
#include "GuiCustomUtils.h"

static ErrorStatus PowerOffKeyState = ERROR;

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
	RCC_AHB1PeriphClockCmd(DET_RECEIVE_RES_PORT_RCC | KEY_OUT1_PORT_RCC | KEY_OUT2_PORT_RCC | 
		KEY_IN0_PORT_RCC | KEY_IN1_PORT_RCC | KEY_IN2_PORT_RCC | KEY_IN3_PORT_RCC | KEY_1_PORT_RCC |
		KEY_3_PORT_RCC | KEY_PW_PORT_RCC , ENABLE);
	
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
	
	GPIO_InitStructure.GPIO_Pin = KEY_IN0_PIN; 
	GPIO_Init(KEY_IN0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_IN1_PIN; 
	GPIO_Init(KEY_IN1_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_IN2_PIN; 
	GPIO_Init(KEY_IN2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_IN3_PIN; 
	GPIO_Init(KEY_IN3_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_1_PIN; 
	GPIO_Init(KEY_1_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = KEY_3_PIN; 
	GPIO_Init(KEY_3_PORT, &GPIO_InitStructure);
	
	/***��Դ��״̬***/
	GPIO_InitStructure.GPIO_Pin = KEY_PW_PIN;	
	GPIO_Init(KEY_PW_PORT, &GPIO_InitStructure);

    //���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    
    //���̵�
	GPIO_InitStructure.GPIO_Pin = KEY_LED_PIN;	
	GPIO_Init(KEY_LED_PORT, &GPIO_InitStructure);
    
    CLOSE_KEY_LED();
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
	
	SET_KEY_ROW0(pin_state);
	SET_KEY_ROW1(pin_state);
	SET_KEY_ROW2(pin_state);	
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
static u16 ReadKeyRow(void)
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
static u16 ReadKeyVertical(void)
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
//| �������� | 4X3���̾���ɨ��
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
static u16 MatrixKeyScan(void)
{
	u16 row_code = 0, vertical_code = 0;
	
	KeyRowOutput(Bit_RESET);          //������͵�ƽ
	row_code = ReadKeyVertical();     //���е�ƽ״̬
	KeyVerticalOutput(Bit_RESET);     //������͵�ƽ
	vertical_code = ReadKeyRow();     //���е�ƽ״̬
	return (row_code | vertical_code);
}

//==================================================================================================
//| �������� | Key1Key3Scan 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ɨ��1��3��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 0x100 ��ʾ��1�����£�0x200��ʾ��3�����£�0x300��ʾ1��3������
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static u16 Key1Key3Scan(void)
{
	u16 key_code = 0x0000;

	if(GET_KEY_1_STATE()==KEY_DOWN)
	{
		key_code |= 0x100;
	}
	if(GET_KEY_3_STATE()==KEY_DOWN)
	{
		key_code |= 0x200;
	}
	return key_code;
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
			code = GUI_KEY_0;
			break;
		case KEY1_CODE:
			code = GUI_KEY_1;
			break;
		case KEY2_CODE:
			code = GUI_KEY_2;
			break;
		case KEY3_CODE:
			code = GUI_KEY_3;
			break;
		case KEY4_CODE:
			code = GUI_KEY_4;
			break;
		case KEY5_CODE:
			code = GUI_KEY_5;
			break;
		case KEY6_CODE:
			code = GUI_KEY_6;
			break;
		case KEY7_CODE:
			code = GUI_KEY_7;
			break;
		case KEY8_CODE:
			code = GUI_KEY_8;
			break;		
		case KEY9_CODE:
			code = GUI_KEY_9;
			break;	
		case KEY_LEFT_CODE:
			code = GUI_KEY_LEFT;
			break;	
		case KEY_RIGHT_CODE:
			code = GUI_KEY_RIGHT;
			break;	
		case KEY_ENTER_CODE:
			code = GUI_KEY_ENTER;
			break;	
		case KEY_DOUBLE_CODE:
			code = GUI_KEY_DOUBLE;
			break;	
		default:
			break;			
	}
	return code;
}

void SetPowerOffKeyState(void)
{
	PowerOffKeyState = SUCCESS;
}
void ResetPowerOffKeyState(void)
{
	PowerOffKeyState = ERROR;
}
ErrorStatus GetPowerOffKeyState(void)
{
	return PowerOffKeyState;
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
	static u16 LateKeyCode = 0 ; //������һ�εĴ������
	static u8 DlyCount = 0;	
	u16 CurrentKeyCode = 0;
	static u8 DoubleFlag= 0;//˫��Ϊ̧�𴥷����赥������
	u8 KeyMsg;
	
	if(Op_Data.DoubleKeySwitch == 0)
	{
		CLOSE_DOUBLE_KEY_SW();
	}
	
	if(ERROR != GetPowerOffKeyState())
	{
		ResetPowerOffKeyState();
		return GUI_KEY_POWER;	
	}
	CurrentKeyCode = MatrixKeyScan(); //����ɨ��
	CurrentKeyCode |= Key1Key3Scan(); //1��3��ɨ��
	if(0 == DoubleFlag)
	{//˫��δ����
		if((CurrentKeyCode != 0x00F7) && (LateKeyCode == CurrentKeyCode))//�м����������ϴμ�⵽����ͬ
		{
			KeyMsg = KeyCodeToKeyMsg(CurrentKeyCode);
			if(0 != KeyMsg)//�Ѱ�������ת��Ϊ������Ϣ
			{//��Ч����					
				if(DlyCount < 50)
				{
					DlyCount++;
					if(DlyCount > 30)
					{
						if(KEY_DOUBLE_CODE == CurrentKeyCode)
						{//��ǰ���µ���˫��
							if(Op_Data.DoubleKeySwitch != 0)//�Ѿ������˫��
							{
								OPEN_DOUBLE_KEY_SW();
							}
							else
							{
								DlyCount = 0;
							}
						}					
					}
				}
				else if(DlyCount == 50)//�������º�50ms����������Ϣ
				{
					DlyCount = 0xFF;
					if(KEY_DOUBLE_CODE != CurrentKeyCode)
					{//����Ϊ���´���						
						return KeyMsg;
					}
					else
					{//˫��Ϊ̧�𴥷�
						DoubleFlag = 1;//��⵽˫�����Ѿ�����
						DlyCount = 0;//�ȴ��ͷ�							
					}
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
	}
	else
	{//�ȴ�˫���ͷ�
		if((CurrentKeyCode & 0x0300) != 0x300)
		{//˫��������һ���Ѿ��ͷ�
			DlyCount++;
			if(DlyCount >= 50)
			{
				DoubleFlag = 0;					
				return GUI_KEY_DOUBLE;			
			}
		}
		else
		{
			DlyCount = 0;
		}
	}

	return 0;
}

