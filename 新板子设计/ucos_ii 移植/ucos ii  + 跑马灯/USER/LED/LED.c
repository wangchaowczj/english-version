#include "led.h"
//初始化LED1,LED3,LED4对应引脚PB5,PA15,PB7
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//设立一个结构体变量，结构体名称是GPIO_InitTypeDef
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//使能PA PB端口时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	CPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //LED1->PB5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHZ;   //设置传输速度，传输速度一共有三种 2 10 50
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //设置传输方式，传输方式是推挽输出
	/*
	传输方式有八种 为什么要定义成这样的数字呢，和寄存器中表现出来的不一样
						输入：GPIO_Mode_AIN = 0x0,					//模拟输入
  								GPIO_Mode_IN_FLOATING = 0x04, //浮空输入
  								GPIO_Mode_IPD = 0x28,					//上拉输入
  								GPIO_Mode_IPU = 0x48,					//下拉输入
  					输出：GPIO_Mode_Out_OD = 0x14,			//开漏输出
 									GPIO_Mode_Out_PP = 0x10,			//推挽输出
  								GPIO_Mode_AF_OD = 0x1C,				//复用开漏输出
  								GPIO_Mode_AF_PP = 0x18				//复用推挽输出
	*/
	GPIO_Init(GPIOB,&GPIO_InitStructure);               //初始化端口GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_5);	                    //输出高
	
	CPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;       //LED4->PB7
	GPIO_Init(GPIOB,&GPIO_InitStructure);						//初始化端口GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_7);									//输出高
	
	CPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;			//LED3->PA15
	GPIO_Init(GPIOA,&GPIO_InitStructure);						//初始化端口GPIOA
	GPIO_SetBits(GPIOA,GPIO_Pin_15);								//输出高	
}