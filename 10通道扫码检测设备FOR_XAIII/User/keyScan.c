#include "includes.h"
#include "GuiCustomUtils.h"

//static ErrorStatus PowerOffKeyState = ERROR;

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
	RCC_AHB1PeriphClockCmd(KEY_OUT0_PORT_RCC | KEY_OUT1_PORT_RCC |KEY_OUT2_PORT_RCC | KEY_OUT3_PORT_RCC | 
		KEY_IN0_PORT_RCC | KEY_IN1_PORT_RCC | KEY_IN2_PORT_RCC | KEY_IN3_PORT_RCC  |
		KEY_PW_PORT_RCC | KEY_FOOT_PORT_RCC , ENABLE);
	
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
	
	/***电源键状态***/
	GPIO_InitStructure.GPIO_Pin = KEY_PW_PIN;	
	GPIO_Init(KEY_PW_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_FOOT_PIN;	
	GPIO_Init(KEY_FOOT_PORT, &GPIO_InitStructure);		
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

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);
	
	SET_KEY_ROW0(pin_state);
	SET_KEY_ROW1(pin_state);
	SET_KEY_ROW2(pin_state);
	SET_KEY_ROW3(pin_state);
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

	GPIO_InitStructure.GPIO_Pin = KEY_OUT3_PIN; 
	GPIO_Init(KEY_OUT3_PORT, &GPIO_InitStructure);
	
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
//| 函数名称 | MatrixKeyScan 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 4X4键盘矩阵扫描
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
static u8 MatrixKeyScan(void)
{
	u8 row_code = 0, vertical_code = 0;
	
	KeyRowOutput(Bit_RESET);          //行输出低电平
	row_code = ReadKeyVertical();     //读列电平状态
	KeyVerticalOutput(Bit_RESET);     //列输出低电平
	vertical_code = ReadKeyRow();     //读行电平状态
	return (row_code | vertical_code);
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
//新版硬件中，原脚踏开关脚线在装配时已经不连，直接使用为江西国泰对接时新增的接口。
//也就是GPIOF1做复用，联网模式时，为中断方式，不连网时当作普通脚踏开关进行扫描使用。
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
	static u8 LateKeyCode = 0 ; //保存上一次的代码代码
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
	CurrentKeyCode = MatrixKeyScan(); //矩阵扫描
	if((CurrentKeyCode != 0xFF) && (LateKeyCode == CurrentKeyCode))//有键按下且与上次检测到的相同
	{
		KeyMsg = KeyCodeToKeyMsg(CurrentKeyCode);
		if(0 != KeyMsg)//把按键代码转换为按键消息
		{//有效按键					
			if(DlyCount < 50)
			{
				DlyCount++;
			}
			else if(DlyCount == 50)//按键按下后50ms产生按键消息
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

