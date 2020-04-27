#include "includes.h"
#include "GuiCustomUtils.h"

static ErrorStatus PowerOffKeyState = ERROR;

//==================================================================================================
//| 函数名称 | KeyConfig 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 按键扫描模块配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void KeyConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(DET_RECEIVE_RES_PORT_RCC | KEY_OUT1_PORT_RCC | KEY_OUT2_PORT_RCC | 
		KEY_IN0_PORT_RCC | KEY_IN1_PORT_RCC | KEY_IN2_PORT_RCC | KEY_IN3_PORT_RCC | KEY_1_PORT_RCC |
		KEY_3_PORT_RCC | KEY_PW_PORT_RCC , ENABLE);
	
	/***按键引脚都已外接10K上拉电阻***/	
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
	
	/***电源键状态***/
	GPIO_InitStructure.GPIO_Pin = KEY_PW_PIN;	
	GPIO_Init(KEY_PW_PORT, &GPIO_InitStructure);

    //输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    
    //键盘灯
	GPIO_InitStructure.GPIO_Pin = KEY_LED_PIN;	
	GPIO_Init(KEY_LED_PORT, &GPIO_InitStructure);
    
    CLOSE_KEY_LED();
}

//==================================================================================================
//| 函数名称 | KeyRowOutputLow 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X3键盘矩阵行输出
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 行输出状态
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void KeyRowOutput(BitAction pin_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//先把列配置为输入
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
	
	//把行配置为输出
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
//| 函数名称 | ReadKeyRow 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X3键盘矩阵读行数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 按键代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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
//| 函数名称 | KeyRowOutputLow 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X3键盘矩阵列输出
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 列输出状态
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void KeyVerticalOutput(BitAction pin_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//先把行配置为输入
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
	
	//再把列配置为输出
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
//| 函数名称 | ReadKeyVertical 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X3键盘矩阵读列数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 按键代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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
//| 函数名称 | MatrixKeyScan 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X3键盘矩阵扫描
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 按键代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static u16 MatrixKeyScan(void)
{
	u16 row_code = 0, vertical_code = 0;
	
	KeyRowOutput(Bit_RESET);          //行输出低电平
	row_code = ReadKeyVertical();     //读列电平状态
	KeyVerticalOutput(Bit_RESET);     //列输出低电平
	vertical_code = ReadKeyRow();     //读行电平状态
	return (row_code | vertical_code);
}

//==================================================================================================
//| 函数名称 | Key1Key3Scan 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 扫描1和3键
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 0x100 表示有1键按下，0x200表示有3键按下，0x300表示1和3都按下
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
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
//| 函数名称 | KeyCodeToKeyMsg 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据扫描到的按键代码转换为对应的系统按键消息
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | keycode 按键代码  msg 转换后的消息指针
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 0  未找到对应的按键消息
//|          | 1  转换成功
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static u8 KeyCodeToKeyMsg(u16 keycode)
{
	u8 code = 0;
	
	switch(keycode)//按键代码转换为对应的系统按键消息
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
//| 函数名称 | KeyScan 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 按键扫描函数，根据扫描结果将产生按键消息
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 函数由滴答定时器每隔1ms调用1次，抬起触发,支持单键和1、3双键扫描
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u8 KeyboardScan(void)
{
	static u16 LateKeyCode = 0 ; //保存上一次的代码代码
	static u8 DlyCount = 0;	
	u16 CurrentKeyCode = 0;
	static u8 DoubleFlag= 0;//双键为抬起触发，需单独处理
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
	CurrentKeyCode = MatrixKeyScan(); //矩阵扫描
	CurrentKeyCode |= Key1Key3Scan(); //1和3键扫描
	if(0 == DoubleFlag)
	{//双键未按下
		if((CurrentKeyCode != 0x00F7) && (LateKeyCode == CurrentKeyCode))//有键按下且与上次检测到的相同
		{
			KeyMsg = KeyCodeToKeyMsg(CurrentKeyCode);
			if(0 != KeyMsg)//把按键代码转换为按键消息
			{//有效按键					
				if(DlyCount < 50)
				{
					DlyCount++;
					if(DlyCount > 30)
					{
						if(KEY_DOUBLE_CODE == CurrentKeyCode)
						{//当前按下的是双键
							if(Op_Data.DoubleKeySwitch != 0)//已经允许打开双键
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
				else if(DlyCount == 50)//按键按下后50ms产生按键消息
				{
					DlyCount = 0xFF;
					if(KEY_DOUBLE_CODE != CurrentKeyCode)
					{//单键为按下触发						
						return KeyMsg;
					}
					else
					{//双键为抬起触发
						DoubleFlag = 1;//检测到双键都已经按下
						DlyCount = 0;//等待释放							
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
	{//等待双键释放
		if((CurrentKeyCode & 0x0300) != 0x300)
		{//双键至少有一个已经释放
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

