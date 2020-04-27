#include "includes.h"
//#include "LCD_SSD1963.h"





/**
  *************************************************************************************
  * @brief  LCD背光设置
  * @param  Clk 该值越大，屏幕越暗，越小越亮，至少为1
  * @retval 无
  * @author ZH
  * @date   2014年11月21日
  * @note   无
  *************************************************************************************  
  */
void LcdSetBright(u8 Clk)
{
	u8 i;
	
	System168MDelay100us(5);
	for (i=0;i<Clk;i++)
	{
		LCD_BRIGHT_L();
		System168MDelay1us();
		LCD_BRIGHT_H();
		System168MDelay1us();
	}
}
/**
  *************************************************************************************
  * @brief  关闭LCD背景
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年11月21日
  * @note   无
  *************************************************************************************  
  */
void LcdCloseBright(void)
{
	LCD_BRIGHT_L();
}
//==================================================================================================
//| 函数名称 | LCD_FSMC_Config
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD的FSMC模式配置
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void LCD_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);		//D0到D15的IO口时钟打开

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);								//FSMC的时钟打开

	//****GPIO配置****//
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);	//D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);	//D1
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		//D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		//D3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		//D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		//D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		//D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);	//D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);	//D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);	//D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);	//D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);	//D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);	//D12
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		//D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		//D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);	//D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);	//A0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		//NOE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		//NWE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);		//NE1(CS)


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
								GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(LCD_BRIGHT_PORT_RCC | LCD_SCREEN_PORT_RCC | LCD_CTRL_RST_PORT_RCC, ENABLE);	//控制口的IO时钟打开


	//****PC9：LCD_DIMM			LCD背光控制引脚，该引脚输出的方波个数决定了LCD的背光输出电流大小****//
	GPIO_InitStructure.GPIO_Pin = LCD_BRIGHT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_BRIGHT_PORT, &GPIO_InitStructure);

	//****PD11：IO_LCDC_RST		LCD控制模块复位引脚****//
	GPIO_InitStructure.GPIO_Pin = LCD_CTRL_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_CTRL_RST_PORT, &GPIO_InitStructure);

	//****PE2：LCDC_TE		LCD刷屏完成判断引脚****//
	GPIO_InitStructure.GPIO_Pin = LCD_SCREEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_SCREEN_PORT, &GPIO_InitStructure);

	//****FSMC配置****//
	p.FSMC_AddressSetupTime = 10;			//地址建立时间
	p.FSMC_AddressHoldTime = 0;				//地址保持时间
	p.FSMC_DataSetupTime = 60;				//数据建立时间
	p.FSMC_BusTurnAroundDuration = 10;		//总线恢复时间
	p.FSMC_CLKDivision = 0;					//时钟分频
	p.FSMC_DataLatency = 0;					//数据保持时间
	p.FSMC_AccessMode = FSMC_AccessMode_A;	//模式A

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;							//NOR FLASH的BANK1
	//MUXEN位	当设置了该位后，地址的低16位和数据将共用数据总线，该位仅对NOR和PSRM存储器有效。
	//0：地址/数据不复用。		1：地址/数据复用数据总线；这是复位后的默认状态。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;		//数据线与地址线不复用
	//MTYP位	定义外部存储器的类型
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;					//存储器类型，原来为FSMC_MemoryType_SRAM
	//MWID位	定义外部存储器总线的宽度，适用于所有类型的存储器。
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;			//数据宽度为16位
	//BURSTEN位	允许对闪存存储器进行成组模式访问；该位仅在闪存存储器的同步成组模式下有效。
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;		//使用异步写模式，禁止突发模式
	//ASYNCWAIT位
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	//WAITPOL	设置存储器产生的等待信号的极性；该位仅在存储器的成组模式下有效。
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	//本成员的配置只在突发模式下有效，等待信号极性为低
	//WRAPMOD	该位决定控制器是否支持把非对齐的AHB成组操作分割成2次线性操作；该位仅在存储器的成组模式下有效。
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;					//禁止非对齐突发模式
	//WAITCFG	当闪存存储器处于成组传输模式时，NWAIT信号指示从闪存存储器出来的数据是否有效或是否需要插入等待周期。
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;	//本成员配置仅在突发模式下有效。NWAIT信号在什么时期产生
	//WREN		该位指示FSMC是否允许/禁止对存储器的写操作。
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			//写使能
	//WAITEN	当闪存存储器处于成组传输模式时，这一位允许/禁止通过NWAIT信号插入等待状态。
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;				//本成员的配置只在突发模式下有效，禁用NWAIT信号
	//EXTMOD	该位允许FSMC使用FSMC_BWTR寄存器，即允许读和写使用不同的时序。
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;				//允许扩展模式
	//CBURSTRW	成组写使能位
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;				//禁止突发写操作
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;							//配置读写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;								//配置写时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);		//使能FSMC Bank1_SRAM Bank

	LCD_CTRL_RST_H();
	LCD_BRIGHT_L();
}

//==================================================================================================
//| 函数名称 | Lcdc_Reset
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD控制器的复位
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void Lcdc_Reset(void)
{
	LCD_CTRL_RST_L();
	System168MDelay1ms(10);
	LCD_CTRL_RST_H();
	System168MDelay1ms(10);
}

//==================================================================================================
//| 函数名称 | Lcd_SpiSend
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD控制器的GPIO口发送SPI格式数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | SendData：要发送的数据，16字节
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
static void Lcdc_SpiSend(u16 SendData)
{
	u8 i;

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//CS脚拉低
	LCD_WRITE_PARAMETER((LCDSPI_CLK | LCDSPI_DATA | LCDSPI_RST) & (~LCDSPI_CS));

	for (i=0;i<16;i++)
	{
		if (SendData & (1<<(15-i)))			//发送1
		{
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_DATA | LCDSPI_RST) & ((~LCDSPI_CLK) & (~LCDSPI_CS)));
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_DATA | LCDSPI_RST | LCDSPI_CLK) & (~LCDSPI_CS));
		}
		else								//发送0
		{
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_RST) & ((~LCDSPI_CLK) & (~LCDSPI_CS) & (~LCDSPI_DATA)));
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER(LCDSPI_RST | LCDSPI_CLK & (~LCDSPI_CS) & (~LCDSPI_DATA));
		}
	}

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//CS脚拉高
	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_RST | LCDSPI_CS);

}

//==================================================================================================
//| 函数名称 | Lcd_Reset
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD屏幕的复位
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void Lcd_Reset(void)
{
	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//写入数值
	LCD_WRITE_PARAMETER((LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS) & (~LCDSPI_RST));
	System168MDelay1ms(1);						//延时1毫秒
//	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//写入数值
//	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS | LCDSPI_RST);
}

//==================================================================================================
//| 函数名称 | Lcdc_Spi_Init
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD控制器的SPI口发送数据，让LCD屏幕初始化
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
static void Lcdc_Spi_Init(void)
{
	//LCD模块复位
	Lcd_Reset();
	OSTimeDlyHMSM(0,0,0,50);		//LCD模块复位后需要等待40毫秒来完成复位，才能发送命令

	Lcdc_SpiSend((((0x00 << 2) + 2) << 8) + 0x07);
	Lcdc_SpiSend((((0x01 << 2) + 2) << 8) + 0x00);
	Lcdc_SpiSend((((0x02 << 2) + 2) << 8) + 0x03);
	Lcdc_SpiSend((((0x03 << 2) + 2) << 8) + 0xCC);
	Lcdc_SpiSend((((0x04 << 2) + 2) << 8) + 0x46);
	Lcdc_SpiSend((((0x05 << 2) + 2) << 8) + 0x0D);
	Lcdc_SpiSend((((0x06 << 2) + 2) << 8) + 0x00);
	Lcdc_SpiSend((((0x07 << 2) + 2) << 8) + 0x00);
	Lcdc_SpiSend((((0x08 << 2) + 2) << 8) + 0x08);
	Lcdc_SpiSend((((0x09 << 2) + 2) << 8) + 0x40);
	Lcdc_SpiSend((((0x0A << 2) + 2) << 8) + 0x88);
	Lcdc_SpiSend((((0x0B << 2) + 2) << 8) + 0x88);
	Lcdc_SpiSend((((0x0C << 2) + 2) << 8) + 0x20);
	Lcdc_SpiSend((((0x0D << 2) + 2) << 8) + 0x20);
	Lcdc_SpiSend((((0x0E << 2) + 2) << 8) + 0x10);
	Lcdc_SpiSend((((0x0F << 2) + 2) << 8) + 0x24);
	Lcdc_SpiSend((((0x10 << 2) + 2) << 8) + 0x04);
	Lcdc_SpiSend((((0x11 << 2) + 2) << 8) + 0x24);
	Lcdc_SpiSend((((0x12 << 2) + 2) << 8) + 0x24);
	Lcdc_SpiSend((((0x1E << 2) + 2) << 8) + 0x00);
	Lcdc_SpiSend((((0x20 << 2) + 2) << 8) + 0x00);

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//全部引脚拉高
	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS | LCDSPI_RST);
}

void Lcd_SetCursor(u16 xAddr, u16 yAddr)
{
	u16 x,y;
	
	#ifdef LCDP_REVERSE		//X与Y交换方向
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif	
	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
}
//==================================================================================================
//| 函数名称 | Lcd_WriteOneDot
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 写入屏幕一个点
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xAddr：X轴坐标		yAddr：Y轴坐标		Color：颜色
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void Lcd_WriteOneDot(u16 xAddr, u16 yAddr, u16 Color)
{
	u16 x,y;

	#ifdef LCDP_REVERSE		//X与Y交换方向
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	LCD_WRITE_PARAMETER(Color);
}

//==================================================================================================
//| 函数名称 | Lcd_ReadOneDot
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 读取屏幕一个点
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xAddr：X轴坐标		yAddr：Y轴坐标
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 该点的颜色
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u16 Lcd_ReadOneDot(u16 xAddr, u16 yAddr)
{
	u16 reData;
	u16 x,y;

	#ifdef LCDP_REVERSE		//X与Y交换方向
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);

	LCD_WRITE_CMD(LCD2E_READ_MEMORY);		//读出数据
	reData = LCD_READ_PARAMETER();
	return reData;
}

//==================================================================================================
//| 函数名称 | Lcd_CheckArea
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 检查区域是否合法
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xSAddr：X轴起始坐标		xEAddr：X轴结束坐标
//|          | ySAddr：Y轴起始坐标		yEAddr：Y轴结束坐标
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：成功		1：起始坐标大于结束坐标		2：坐标值超出范围
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 Lcd_CheckArea(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr)
{
	if ((xSAddr > xEAddr) | (ySAddr > yEAddr))
	{
		return 1;
	}
	if ((xEAddr >= LCDP_HDISP) | (yEAddr >= LCDP_VDISP))
	{
		return 2;
	}
	return 0;
}

//==================================================================================================
//| 函数名称 | Lcd_WriteData
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 刷新部分屏幕
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xSAddr：X轴起始坐标		xEAddr：X轴结束坐标
//|          | ySAddr：Y轴起始坐标		yEAddr：Y轴结束坐标
//|          | DataBuf：填充数组
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：成功		1：起始坐标大于结束坐标		2：坐标值超出范围
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 Lcd_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 *DataBuf)
{
	u32 i;
	u32 wlen;		//写入数据长度
	u8 state;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X与Y交换方向
		xs = ySAddr;
		xe = yEAddr;
		ys = xSAddr;
		ye = xEAddr;
	#else
		xs = xSAddr;
		xe = xEAddr;
		ys = ySAddr;
		ye = yEAddr;
	#endif

	state = Lcd_CheckArea(xs, ys, xe, ye);
	if (state)
	{
		return state;
	}

	wlen = (xe-xs+1)*(ye-ys+1);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(*(DataBuf++));
	}

	return 0;
}

//==================================================================================================
//| 函数名称 | Lcd_8Bit_WriteData
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 刷新部分屏幕，输入的数据为8位数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xSAddr：X轴起始坐标		xEAddr：X轴结束坐标
//|          | ySAddr：Y轴起始坐标		yEAddr：Y轴结束坐标
//|          | DataBuf：填充数组
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：成功		1：起始坐标大于结束坐标		2：坐标值超出范围
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 Lcd_8Bit_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u8 *DataBuf)
{
	u32 i;
	u32 wlen;		//写入数据长度
	u8 state;
	u16 data;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X与Y交换方向
		xs = ySAddr;
		xe = yEAddr;
		ys = xSAddr;
		ye = xEAddr;
	#else
		xs = xSAddr;
		xe = xEAddr;
		ys = ySAddr;
		ye = yEAddr;
	#endif

	state = Lcd_CheckArea(xs, ys, xe, ye);
	if (state)
	{
		return state;
	}

	wlen = (xe-xs+1)*(ye-ys+1);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	for (i=0;i<wlen;i++)
	{
		data = (u16)(*(DataBuf++) << 8) & 0xFF00;
		data |= (u16)(*(DataBuf++)) & 0x00FF;
		LCD_WRITE_PARAMETER(data);
	}

	return 0;
}

//==================================================================================================
//| 函数名称 | Lcd_WriteAll
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 刷新全部屏幕
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | DataBuf：填充数组
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void Lcd_WriteAll(u16 *DataBuf)
{
	u32 i;
	u32 wlen;

	wlen = LCDP_HDISP*LCDP_VDISP;

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(*(DataBuf++));
	}
}

//==================================================================================================
//| 函数名称 | Lcd_ClearPart
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 清除部分屏幕
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | xSAddr：X轴起始坐标		xEAddr：X轴结束坐标
//|          | ySAddr：Y轴起始坐标		yEAddr：Y轴结束坐标
//|          | color：以该颜色填充
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 0：成功		1：起始坐标大于结束坐标		2：坐标值超出范围
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
u8 Lcd_ClearPart(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 color)
{
	u32 i;
	u32 wlen;		//写入数据长度
	u8 state;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X与Y交换方向
		xs = ySAddr;
		xe = yEAddr;
		ys = xSAddr;
		ye = xEAddr;
	#else
		xs = xSAddr;
		xe = xEAddr;
		ys = ySAddr;
		ye = yEAddr;
	#endif
	state = Lcd_CheckArea(xs, ys, xe, ye);
	if (state)
	{
		return state;
	}

	wlen = (xe-xs+1)*(ye-ys+1);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(color);
	}

	return 0;
}

//==================================================================================================
//| 函数名称 | Lcd_ClearAll
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 清除全部屏幕
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | color：以该颜色填充
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void Lcd_ClearAll(u16 color)
{
	u32 i;
	u32 wlen;

	wlen = LCDP_HDISP*LCDP_VDISP;

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//设置行地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//设置列地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//写入数据
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(color);
	}
}




//==================================================================================================
//| 函数名称 | Lcd_PllLockState
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取PLL锁定状态
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：zh    时间：2015-05-13
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 可用于判断LCD是否已经意外复位
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
ErrorStatus Lcd_PllLockState(void)
{
	LCD_WRITE_CMD(LCDE4_R_PLL_STAT);
	if(LCDP_PLL_LOCKED == LCD_READ_PARAMETER())
	{
		return SUCCESS;
	}
	return ERROR;
}

//==================================================================================================
//| 函数名称 | Lcd_Init
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | LCD控制器及LCD屏幕的初始化
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   备注   | 
//|----------|--------------------------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容：
//==================================================================================================
void LcdSSD1963_Init(void)
{
	u8 state;
	u8 i = 10;
	
	LCD_BRIGHT_L();
	
	Lcdc_Reset();
		
	LCD_WRITE_CMD(LCDE2_W_PLL_MN);							//设置PLL的M和N值
	LCD_WRITE_PARAMETER(LCDP_PLLM);							//设置M
	LCD_WRITE_PARAMETER(LCDP_PLLN);							//设置N
	LCD_WRITE_PARAMETER(0x54);								//使能M和N

	do
	{
		LCD_WRITE_CMD(LCDE0_W_PLL);							//设置PLL
		LCD_WRITE_PARAMETER(LCDP_UsePLL | LCDP_EnPLL);		//打开PLL时钟，并且用PLL时钟作为系统时钟

		System168MDelay1ms(1);									//延时1毫秒

		LCD_WRITE_CMD(LCDE4_R_PLL_STAT);					//读PLL
		state = LCD_READ_PARAMETER();						//打开PLL时钟，并且用PLL时钟作为系统时钟

		if (state == LCDP_PLL_LOCKED)
		{
			break;
		}
	}while(i--);
	System168MDelay1ms(10);
	LCD_WRITE_CMD(LCD01_SOFT_RST);							//液晶屏软复位
	System168MDelay1ms(10);										//等待10毫秒，液晶屏要求等待5毫秒，多等待一段时间

	LCD_WRITE_CMD(LCDE6_SET_FPR);							//设置LCDC_FPR参数，用于得到像素频率
	LCD_WRITE_PARAMETER(LCDP_FPR >> 16);					//FPR数值
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x0000FF00) >> 8);
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x000000FF));

	LCD_WRITE_CMD(LCDB0_W_LCDMODE);							//设置LCD屏幕的参数
	LCD_WRITE_PARAMETER(LCDP_24Bit | 
						LCDP_Data_Rising | 
						LCDP_HSYNC_Low | 
						LCDP_VSYNC_Low);
	LCD_WRITE_PARAMETER(0x00);								//TFT模式
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//液晶屏的行数320
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//液晶屏的列数240
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER(0x00);								//在TFT模式下该位不需要设置

	LCD_WRITE_CMD(LCDB4_W_HENG_TIME);						//设置横行扫描的时间，根据LCD资料得到的数值
	LCD_WRITE_PARAMETER((LCDP_HT-1) >> 8);					//总时间-1
	LCD_WRITE_PARAMETER((LCDP_HT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) >> 8);			//从同步信号开始到数据到达的时间
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_HS-1);							//同步信号占用的时间-1
	LCD_WRITE_PARAMETER(0x00);								//总时间完成后到下一个数据开始的时间
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);								//串行使用，并行口不使用

	LCD_WRITE_CMD(LCDB6_W_SHU_TIME);						//设置数行扫描的时间，根据LCD资料得到的数值
	LCD_WRITE_PARAMETER((LCDP_VT-1) >> 8);					//总时间-1
	LCD_WRITE_PARAMETER((LCDP_VT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) >> 8);			//从同步信号开始到数据到达的时间
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_VS-1);							//同步信号占用的时间-1
	LCD_WRITE_PARAMETER(0x00);								//总时间完成后到下一个数据开始的时间
	LCD_WRITE_PARAMETER(0x00);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);						//设置行地址
	LCD_WRITE_PARAMETER(0x00);								//行起始地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//行结束地址
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);							//设置列地址
	LCD_WRITE_PARAMETER(0x00);								//列起始地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//列结束地址
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD36_ADDR_MODE);							//设置地址模式
	LCD_WRITE_PARAMETER(LCDP_TopToBottom | 
						LCDP_LeftToRight | 
					#ifdef LCDP_REVERSE
						LCDP_Reverse | 
					#else
						LCDP_Normal | 
					#endif
						LCDP_Refresh_TopBottom | 
						LCDP_SetRGB | 
						LCDP_Refresh_LeftRight | 
					#ifdef LCDP_REVERSE
						LCDP_NormalLeftRight | 
					#else
						LCDP_FlipLeftRight | 
					#endif
						LCDP_FlipTopBottom);

	LCD_WRITE_CMD(LCDB8_SET_GPIO);							//设置GPIO控制
	LCD_WRITE_PARAMETER(LCDP_GPIOControlHost | 
						LCDP_GPIO_Output);					//GPIO均配置成输出
	LCD_WRITE_PARAMETER(LCDP_GPIO0_Normal);					//GPIO0作为普通IO口

	Lcdc_Spi_Init();										//通过LCD控制器的GPIO口发送SPI格式数据，初始化LCD屏幕

	LCD_WRITE_CMD(LCDF0_SET_DATALEN);						//设置单片机到LCD控制器的数据位数及格式
	LCD_WRITE_PARAMETER(LCDP_Pixel_16Bit565);				//设置为16位565格式数据

	Lcd_ClearAll(BLACK);//避免闪屏
	
	LCD_BRIGHT_L();//先关背光，不然会有点闪，在其他地方再开
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//打开显示	
	//LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength+1);//设置背光强度	
}

//LCD在使用过程中竟然复位后调用本函数，该函数不操作背光
void LcdSSD1963_ResetInit(void)
{
	u8 state;
	u8 i = 10;
	
	Lcdc_Reset();
		
	LCD_WRITE_CMD(LCDE2_W_PLL_MN);							//设置PLL的M和N值
	LCD_WRITE_PARAMETER(LCDP_PLLM);							//设置M
	LCD_WRITE_PARAMETER(LCDP_PLLN);							//设置N
	LCD_WRITE_PARAMETER(0x54);								//使能M和N

	do
	{
		LCD_WRITE_CMD(LCDE0_W_PLL);							//设置PLL
		LCD_WRITE_PARAMETER(LCDP_UsePLL | LCDP_EnPLL);		//打开PLL时钟，并且用PLL时钟作为系统时钟

		System168MDelay1ms(1);									//延时1毫秒

		LCD_WRITE_CMD(LCDE4_R_PLL_STAT);					//读PLL
		state = LCD_READ_PARAMETER();						//打开PLL时钟，并且用PLL时钟作为系统时钟

		if (state == LCDP_PLL_LOCKED)
		{
			break;
		}
	}while(i--);

	LCD_WRITE_CMD(LCD01_SOFT_RST);							//液晶屏软复位
	System168MDelay1ms(10);										//等待10毫秒，液晶屏要求等待5毫秒，多等待一段时间

	LCD_WRITE_CMD(LCDE6_SET_FPR);							//设置LCDC_FPR参数，用于得到像素频率
	LCD_WRITE_PARAMETER(LCDP_FPR >> 16);					//FPR数值
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x0000FF00) >> 8);
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x000000FF));

	LCD_WRITE_CMD(LCDB0_W_LCDMODE);							//设置LCD屏幕的参数
	LCD_WRITE_PARAMETER(LCDP_24Bit | 
						LCDP_Data_Rising | 
						LCDP_HSYNC_Low | 
						LCDP_VSYNC_Low);
	LCD_WRITE_PARAMETER(0x00);								//TFT模式
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//液晶屏的行数320
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//液晶屏的列数240
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER(0x00);								//在TFT模式下该位不需要设置

	LCD_WRITE_CMD(LCDB4_W_HENG_TIME);						//设置横行扫描的时间，根据LCD资料得到的数值
	LCD_WRITE_PARAMETER((LCDP_HT-1) >> 8);					//总时间-1
	LCD_WRITE_PARAMETER((LCDP_HT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) >> 8);			//从同步信号开始到数据到达的时间
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_HS-1);							//同步信号占用的时间-1
	LCD_WRITE_PARAMETER(0x00);								//总时间完成后到下一个数据开始的时间
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);								//串行使用，并行口不使用

	LCD_WRITE_CMD(LCDB6_W_SHU_TIME);						//设置数行扫描的时间，根据LCD资料得到的数值
	LCD_WRITE_PARAMETER((LCDP_VT-1) >> 8);					//总时间-1
	LCD_WRITE_PARAMETER((LCDP_VT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) >> 8);			//从同步信号开始到数据到达的时间
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_VS-1);							//同步信号占用的时间-1
	LCD_WRITE_PARAMETER(0x00);								//总时间完成后到下一个数据开始的时间
	LCD_WRITE_PARAMETER(0x00);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);						//设置行地址
	LCD_WRITE_PARAMETER(0x00);								//行起始地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//行结束地址
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);							//设置列地址
	LCD_WRITE_PARAMETER(0x00);								//列起始地址
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//列结束地址
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD36_ADDR_MODE);							//设置地址模式
	LCD_WRITE_PARAMETER(LCDP_TopToBottom | 
						LCDP_LeftToRight | 
					#ifdef LCDP_REVERSE
						LCDP_Reverse | 
					#else
						LCDP_Normal | 
					#endif
						LCDP_Refresh_TopBottom | 
						LCDP_SetRGB | 
						LCDP_Refresh_LeftRight | 
					#ifdef LCDP_REVERSE
						LCDP_NormalLeftRight | 
					#else
						LCDP_FlipLeftRight | 
					#endif
						LCDP_FlipTopBottom);

	LCD_WRITE_CMD(LCDB8_SET_GPIO);							//设置GPIO控制
	LCD_WRITE_PARAMETER(LCDP_GPIOControlHost | 
						LCDP_GPIO_Output);					//GPIO均配置成输出
	LCD_WRITE_PARAMETER(LCDP_GPIO0_Normal);					//GPIO0作为普通IO口

	Lcdc_Spi_Init();										//通过LCD控制器的GPIO口发送SPI格式数据，初始化LCD屏幕

	LCD_WRITE_CMD(LCDF0_SET_DATALEN);						//设置单片机到LCD控制器的数据位数及格式
	LCD_WRITE_PARAMETER(LCDP_Pixel_16Bit565);				//设置为16位565格式数据

	Lcd_ClearAll(BLACK);//避免闪屏
//	
//	LCD_BRIGHT_L();//先关背光，不然会有点闪，在其他地方再开
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//打开显示	
	//LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength+1);//设置背光强度	
}

void Lcd_OpenShow(void)
{
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//打开显示
}

void Lcd_CloseShow(void)
{
	LCD_WRITE_CMD(LCD28_DISPLAY_OFF);						//关闭显示
}








