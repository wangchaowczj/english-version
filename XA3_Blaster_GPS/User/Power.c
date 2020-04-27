#include "includes.h"

//==================================================================================================
//| 函数名称 | PowerConfig 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置电源模块
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-28 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void PowerConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//设置串口时钟
	RCC_AHB1PeriphClockCmd(PW_5V_SW_PORT_RCC | LINAB_SHORT_SW_RCC |
		LINAB_5V_CTRL_RCC | LINAB_6_7V_CTRL_RCC | PW_1_2V_EN_PORT_RCC | PW_6_7V_SW_RCC | PW_11_3V_SW_RCC | 
		PW_6_7V_5V_RCC | HV_ENABLE_RCC | PW_HV_SW_RCC, ENABLE);	//GPIO时钟配置

	/***外接有上/下拉电阻***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	/***+5V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_5V_SW_PIN;	
	GPIO_Init(PW_5V_SW_PORT, &GPIO_InitStructure);
	
	/***IO_6.7OVER***/
	GPIO_InitStructure.GPIO_Pin = LINAB_5V_CTRL_PIN;	
	GPIO_Init(LINAB_5V_CTRL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LINAB_6_7V_CTRL_PIN;	
	GPIO_Init(LINAB_6_7V_CTRL_PORT, &GPIO_InitStructure);
	
	/***IO_11.3V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_11_3V_SW_PIN;	
	GPIO_Init(PW_11_3V_SW_PORT, &GPIO_InitStructure);

	/***HV_ENABLE***/
	GPIO_InitStructure.GPIO_Pin = HV_ENABLE_PIN;	
	GPIO_Init(HV_ENABLE_PORT, &GPIO_InitStructure);
	
	/***HV_SW***/
	GPIO_InitStructure.GPIO_Pin = PW_HV_SW_PIN;	
	GPIO_Init(PW_HV_SW_PORT, &GPIO_InitStructure);	
	
	/***IO_6.7V_EN***/
	GPIO_InitStructure.GPIO_Pin = PW_6_7V_SW_PIN;
	GPIO_Init(PW_6_7V_SW_PORT, &GPIO_InitStructure);

	/***IO_6.7V_SW***/
	GPIO_InitStructure.GPIO_Pin = LINAB_SHORT_SW_PIN;
	GPIO_Init(LINAB_SHORT_SW_PORT, &GPIO_InitStructure);
	
	/***内部上拉***/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/***端口电压6.7V与5V切换***/
	GPIO_InitStructure.GPIO_Pin = PW_6_7V_5V_PIN;
	GPIO_Init(PW_6_7V_5V_PORT, &GPIO_InitStructure);	
		
	GPIO_InitStructure.GPIO_Pin = PW_1_2V_EN_PIN;
	GPIO_Init(PW_1_2V_EN_PORT, &GPIO_InitStructure);


	/***打开打开1.2V用于液晶控制***/
	OPEN_1_2V();
	
	/***打开内部5V电压***/
	OPEN_5V_SW();
	
	/***关闭高压并切换到低压***/
	CLOSE_BUS_HV();
	
	/***总线电压***/
	//OPEN_BUS_6_7V();
	CLOSE_BUS_VOLTAGE();
}

