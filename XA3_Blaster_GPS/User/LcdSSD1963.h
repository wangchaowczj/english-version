#ifndef _LCD_SSD1963_H_
#define _LCD_SSD1963_H_

//----------------------------和LCD屏幕有关的参数------------------------------//
//LCD的设置参数
#define LCDP_HT			408
#define LCDP_HDISP		320
#define LCDP_HFP		18
#define LCDP_HBP		69
#define LCDP_HS			1

#define LCDP_VT			263
#define LCDP_VDISP		240
#define LCDP_VFP		10
#define LCDP_VBP		12
#define LCDP_VS			1

#define LCDP_PLLM		49		//得到VCO为400MHz
#define LCDP_PLLN		3		//得到PLL为100MHz

#define LCDP_FPR		67108	//得到像素时钟频率为6.4MHz

#define LCDP_REVERSE

#ifdef LCDP_REVERSE				//如果是反向
	#define LCDP_MAX_X		240		//x轴的最大像素		因为屏幕竖着放，故横竖相反
	#define LCDP_MAX_Y		320		//y轴的最大像素
#else
	#define LCDP_MAX_X		320		//x轴的最大像素
	#define LCDP_MAX_Y		240		//y轴的最大像素
#endif


//----------------------------和LCD控制器有关的参数------------------------------//
//伽马曲线
#define LCDP_GAMMA0		0
#define LCDP_GAMMA1		1
#define LCDP_GAMMA2		2
#define LCDP_GAMMA3		3

//0x36设置项-和LCD控制器有关
#define LCDP_TopToBottom			0x00	//上下反转
#define LCDP_BottomToTop			0x80

#define LCDP_LeftToRight			0x00	//左右反转
#define LCDP_RightToLeft			0x40

#define LCDP_Normal					0x00	//行和列调换
#define LCDP_Reverse				0x20

#define LCDP_Refresh_TopBottom		0x00	//从上到下刷新(从frame buffer到display的变化)
#define LCDP_Refresh_BottomTop		0x10

#define LCDP_SetRGB					0x00	//设置颜色模式为RGB还是BGR(从frame buffer到display的变化)
#define LCDP_SetBGR					0x08

#define LCDP_Refresh_LeftRight		0x00	//从左到右刷新(从frame buffer到display的变化)
#define LCDP_Refresh_RightLeft		0x04

#define LCDP_NormalLeftRight		0x00	//左右反转(从frame buffer到display的变化)
#define LCDP_FlipLeftRight			0x02

#define LCDP_NormalTopBottom		0x00	//上下反转(从frame buffer到display的变化)
#define LCDP_FlipTopBottom			0x01

//0xB0设置项-和LCD屏幕有关
#define LCDP_24Bit					0x20	//液晶屏数据宽度为24位
#define LCDP_18Bit_DisAll			0x00	//液晶屏数据宽度为18位，不使用dither和FRC
#define LCDP_18Bit_EnDither			0x10	//液晶屏数据宽度为18位，使用dither
#define LCDP_18Bit_EnFRC			0x18	//液晶屏数据宽度为18位，使用FRC

#define LCDP_Data_Falling			0x00	//下降沿获取数据
#define LCDP_Data_Rising			0x04	//上升沿获取数据

#define LCDP_HSYNC_Low				0x00	//HSYNC低有效
#define LCDP_HSYNC_High				0x02	//HSYNC高有效

#define LCDP_VSYNC_Low				0x00	//VSYNC低有效
#define LCDP_VSYNC_High				0x01	//VSYNC高有效

//0xB8设置项-和GPIO口有关
#define LCDP_GPIOControlHost		0x00	//单片机控制GPIO口
#define LCDP_GPIOControlLCDC		0xF0	//LCD控制器控制GPIO口

#define LCDP_GPIO_Output			0x0F	//GPIO输出
#define LCDP_GPIO_Input				0x00	//GPIO输入

#define LCDP_GPIO0_Normal			0x01	//GPIO0作为普通IO
#define LCDP_GPIO0_Sleep			0x00	//GPIO0作为睡眠模式的控制

//0xBA控制项-控制GPIO口输出
#define LCDP_GPIO0					0x01
#define LCDP_GPIO1					0x02
#define LCDP_GPIO2					0x04
#define LCDP_GPIO3					0x08

//0xBC设置项-设置对比度、亮度、饱和度
#define LCDP_EnPostprocess			0x01	//允许对比度、亮度、饱和度处理
#define LCDP_DisPostprocess			0x00	//禁止对比度、亮度、饱和度处理

//0xE0设置项-设置PLL参数
#define LCDP_UsePLL					0x02	//使用PLL作为系统时钟
#define LCDP_UseCrystal				0x00	//使用参考时钟作为系统时钟

#define LCDP_EnPLL					0x01	//打开PLL
#define LCDP_DisPLL					0x00	//关闭PLL

#define LCDP_PLL_LOCKED             0x04    //PLL LOCK

//0xF0设置项-设置单片机和液晶屏的通讯总线
#define LCDP_Pixel_8Bit				0x00	//8位数据长度
#define LCDP_Pixel_12Bit			0x01	//12位数据长度
#define LCDP_Pixel_16Bit			0x02	//16位数据长度
#define LCDP_Pixel_16Bit565			0x03	//16位数据长度，使用565格式
#define LCDP_Pixel_18Bit			0x04	//18位数据长度
#define LCDP_Pixel_24Bit			0x05	//24位数据长度
#define LCDP_Pixel_9Bit				0x06	//9位数据长度



//LCD命令
#define LCD00_NOP					0x00	//空闲指令					无数值
#define LCD01_SOFT_RST				0x01	//软件复位					无数值,除了E0和E5的命令以外全部复位，需要等待5毫秒后再发命令
#define LCD0A_GET_POWER				0x0A	//模块工作模式				读取一个数值
#define LCD0B_GET_ADDR				0x0B	//模块对LCD操作的地址及方式	读取一个数值
#define LCD0D_GET_DISPLAY			0x0D	//LCD显示模式				读取一个数值
#define LCD0E_GET_TEAR				0x0E	//TE引脚功能				读取一个数值
#define LCD10_INTO_SLEEP			0x10	//进入睡眠模式				无数值，可能改变GPIO0，需要看B8设置，需要等待5毫秒再发命令
#define LCD11_EXIT_SLEEP			0x11	//退出睡眠模式				无数值，可能改变GPIO0，需要看B8设置，需要等待5毫秒再发命令，自动调用29
#define LCD12_PART_DISPLAY			0x12	//进入部分显示模式			无数值，显示内容时，需要先调用30设置显示区域
#define LCD13_NORMAL_DISPLAY		0x13	//进入正常显示模式			无数值，调用该命令则自动退出部分显示模式
#define LCD20_EXIT_INVERT			0x20	//退出反向显示模式			无数值，反向显示即写入1为0，写入0为1。
#define LCD21_INTO_INVERT			0x21	//进入反向显示模式			无数值
#define LCD26_GAMMA					0x26	//设置伽马曲线				写入一个数值
#define LCD28_DISPLAY_OFF			0x28	//关闭液晶显示				无数值
#define LCD29_DISPLAY_ON			0x29	//打开液晶显示				无数值
#define LCD2A_COLUMN_ADDR			0x2A	//设置行地址				写入四个数值
#define LCD2B_ROW_ADDR				0x2B	//设置列地址				写入四个数值
#define LCD2C_WRITE_MEMORY			0x2C	//开始写入数据				写入液晶屏的数据
#define LCD2E_READ_MEMORY			0x2E	//开始读出数据				读取液晶屏的数据
#define LCD30_SET_PARTIAL			0x30	//设置部分显示模式的区域	写入四个数值，数据只能一行一行的改变
#define LCD33_SCROLL_AREA			0x33	//设置屏幕滚动区域			写入六个数值，包括上方固定的区域，滚动区域行数，下方固定的区域
#define LCD34_TEAR_OFF				0x34	//关闭TE引脚功能			无数值
#define LCD35_TEAR_ON				0x35	//打开TE引脚功能			写入一个数值，TE引脚的功能
#define LCD36_ADDR_MODE				0x36	//设置地址模式				写入一个数值
												//A7：0-从上到下		1-从下到上
												//A6：0-从左到右		1-从右到左
												//A5：0-正常			1-行和列反转，即行是列，列是行
												//A4：0-从上到下刷屏	1-从下到上刷屏
												//A3：0-RGB				1-BGR
												//A2：0-从左到右刷屏	1-从右到左刷屏
												//A1：0-正常显示		1-buffer中数据左右反向显示
												//A0：0-正常显示		1-buffer中数据上下反向显示
#define LCD37_SCROLL_START			0x37	//设置滚屏滚到第几行		写入两个数值
#define LCD38_EXIT_IDLE				0x38	//退出IDLE模式				无数值，IDLE模式看起来好像可以简化颜色，只可以使用几种颜色
#define LCD39_INTO_IDLE				0x39	//进入IDLE模式				无数值
#define LCD3C_W_MEM_CONTIN			0x3C	//继续写入数据				写入液晶屏的数据
#define LCD3E_R_MEM_CONTIN			0x3E	//继续读出数据				读取液晶屏的数据
#define LCD44_TEAR_LINE				0x44	//扫描到第几行TE引脚有信号	写入两个数值
#define LCD45_GET_SCANLINE			0x45	//读取当前扫描的行数		读取两个数值
#define LCDA1_READ_INFO				0xA1	//读取设备描述				读取五个数值
#define LCDB0_W_LCDMODE				0xB0	//设置LCD的模式参数			写入七个数值
#define LCDB1_R_LCDMODE				0xB1	//读取LCD的模式参数			读取七个数值
#define LCDB4_W_HENG_TIME			0xB4	//设置LCD横行扫描的时间		写入八个数值
#define LCDB5_R_HENG_TIME			0xB5	//读取LCD横行扫描的时间		读取八个数值
#define LCDB6_W_SHU_TIME			0xB6	//设置LCD数行扫描的时间		写入七个数值
#define LCDB7_R_SHU_TIME			0xB7	//读取LCD数行扫描的时间		读取七个数值
#define LCDB8_SET_GPIO				0xB8	//设置GPIO口的模式			写入两个数值
#define LCDB9_GET_GPIO				0xB9	//读取GPIO口的模式			读取两个数值
#define LCDBA_SET_IO_VAL			0xBA	//设置GPIO口的数值			写入一个数值
#define LCDBB_GET_IO_VAL			0xBB	//读取GPIO口的数值			读取一个数值
#define LCDBC_W_POSTPRO				0xBC	//设置后期处理				写入四个数值，包括对比度、亮度、饱和度设置
#define LCDBD_R_POSTPRO				0xBD	//读取后期处理				读取四个数值
#define LCDBE_W_PWM					0xBE	//设置PWM时钟				写入六个数值
#define LCDBF_R_PWM					0xBF	//读取PWM时钟				读取六个数值
#define LCDC0_W_GEN0				0xC0	//GPIO0输出模式设置			写入七个数值
#define LCDC1_R_GEN0				0xC1	//GPIO0输出模式设置			读取七个数值
#define LCDC2_W_GEN1				0xC2	//GPIO1输出模式设置			写入七个数值
#define LCDC3_R_GEN1				0xC3	//GPIO1输出模式设置			读取七个数值
#define LCDC4_W_GEN2				0xC4	//GPIO2输出模式设置			写入七个数值
#define LCDC5_R_GEN2				0xC5	//GPIO2输出模式设置			读取七个数值
#define LCDC6_W_GEN3				0xC6	//GPIO3输出模式设置			写入七个数值
#define LCDC7_R_GEN3				0xC7	//GPIO3输出模式设置			读取七个数值
#define LCDC8_W_GPIO0_ROP			0xC8	//GPIO0ROP模式设置			写入两个数值
#define LCDC9_R_GPIO0_ROP			0xC9	//GPIO0ROP模式读取			读取两个数值
#define LCDCA_W_GPIO1_ROP			0xCA	//GPIO1ROP模式设置			写入两个数值
#define LCDCB_R_GPIO1_ROP			0xCB	//GPIO1ROP模式读取			读取两个数值
#define LCDCC_W_GPIO2_ROP			0xCC	//GPIO2ROP模式设置			写入两个数值
#define LCDCD_R_GPIO2_ROP			0xCD	//GPIO2ROP模式读取			读取两个数值
#define LCDCE_W_GPIO3_ROP			0xCE	//GPIO3ROP模式设置			写入两个数值
#define LCDCF_R_GPIO3_ROP			0xCF	//GPIO3ROP模式读取			读取两个数值
#define LCDD0_W_DBC					0xD0	//设置DBC参数				写入一个数值
#define LCDD1_R_DBC					0xD1	//读取DBC参数				读取一个数值
#define LCDD4_W_DBC_TH				0xD4	//设置DBC在不同电压下的值	写入九个数值
#define LCDD5_R_DBC_TH				0xD5	//读取DBC在不同电压下的值	读取九个数值
#define LCDE0_W_PLL					0xE0	//设置PLL					写入一个数值
#define LCDE2_W_PLL_MN				0xE2	//设置PLL的MN值				写入三个数值
#define LCDE3_R_PLL_MN				0xE3	//读取PLL的MN值				读取三个数值
#define LCDE4_R_PLL_STAT			0xE4	//读取PLL的状态				读取一个数值
#define LCDE5_SET_D_SLEEP			0xE5	//进入深度睡眠				无数值，需要两次读来退出深度睡眠
#define LCDE6_SET_FPR				0xE6	//设置像素时钟频率			写入三个数值
#define LCDE7_GET_FPR				0xE7	//读取像素时钟频率			读取三个数值
#define LCDF0_SET_DATALEN			0xF0	//设置数据长度位数			写入一个数值
#define LCDF1_GET_DATALEN			0xF1	//读取数据长度位数			读取一个数值


#define LCD_BRIGHT_RANK   32
//----------------------------和GPIO口有关的参数------------------------------//
	//PC9：LCD背光控制-输出
#define LCD_BRIGHT_PORT			GPIOI
#define LCD_BRIGHT_PORT_RCC		RCC_AHB1Periph_GPIOI
#define LCD_BRIGHT_PIN			GPIO_Pin_11

	//PD11：LCD控制模块复位-输出
#define LCD_CTRL_RST_PORT		GPIOD
#define LCD_CTRL_RST_PORT_RCC	RCC_AHB1Periph_GPIOD
#define LCD_CTRL_RST_PIN		GPIO_Pin_11

	//PE2：LCD刷屏完成-输入
#define LCD_SCREEN_PORT			GPIOE
#define LCD_SCREEN_PORT_RCC		RCC_AHB1Periph_GPIOE
#define LCD_SCREEN_PIN			GPIO_Pin_2


//IO口控制
#define LCD_BRIGHT_H()		GPIO_WriteBit(LCD_BRIGHT_PORT, LCD_BRIGHT_PIN, Bit_SET)
#define LCD_BRIGHT_L()		GPIO_WriteBit(LCD_BRIGHT_PORT, LCD_BRIGHT_PIN, Bit_RESET)

#define LCD_CTRL_RST_H()	GPIO_WriteBit(LCD_CTRL_RST_PORT, LCD_CTRL_RST_PIN, Bit_SET)
#define LCD_CTRL_RST_L()	GPIO_WriteBit(LCD_CTRL_RST_PORT, LCD_CTRL_RST_PIN, Bit_RESET)

#define IS_LCD_SCREEN_OK()	GPIO_ReadInputDataBit(LCD_SCREEN_PORT, LCD_SCREEN_PIN)


//----------------------------LCD控制器的GPIO口对应的功能------------------------------//
#define LCDSPI_RST		LCDP_GPIO0
#define LCDSPI_CLK		LCDP_GPIO1
#define LCDSPI_DATA		LCDP_GPIO2
#define LCDSPI_CS		LCDP_GPIO3


//----------------------------读写LCD控制器的地址及命令格式------------------------------//

//选择BANK1-BORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
//FSMC_A17 接LCD的DC(寄存器/数据选择)脚
//16 bit => FSMC[24:0]对应HADDR[25:1]
//寄存器基地址 = 0X60000000
//RAM基地址 = 0X60020000 = 0X60000000+2^17*2 = 0X60000000 + 0X40000 = 0X60040000
//当选择不同的地址线时，地址要重新计算。

#define Bank1_LCD_D		((unsigned int)0x60040000)		//数据地址
#define Bank1_LCD_C		((unsigned int)0x60000000)		//命令地址

//向总线写命令和数据
#define LCD_WRITE_CMD(index)		((*(__IO u16 *) (Bank1_LCD_C)) = ((u16)index))
#define LCD_WRITE_PARAMETER(val)	((*(__IO u16 *) (Bank1_LCD_D)) = ((u16)(val)))
//读取数据
#define LCD_READ_PARAMETER()		*(__IO u16 *) (Bank1_LCD_D)





//----------------------------外部可调用函数------------------------------//
extern void LcdSetBright(u8 Clk);
extern void LcdCloseBright(void);
extern void LCD_FSMC_Config(void);		//单片机连接LCD的引脚功能初始化
extern void Lcdc_Reset(void);			//LCD控制器复位
extern void Lcd_Reset(void);			//LCD屏幕复位
extern void LcdSSD1963_Init(void);				//LCD控制器及屏幕初始化
extern void Lcd_SetCursor(u16 xAddr, u16 yAddr);
extern void Lcd_OpenShow(void);			//打开液晶显示
extern void Lcd_CloseShow(void);		//关闭液晶显示
extern void LcdSSD1963_ResetInit(void);

extern ErrorStatus Lcd_PllLockState(void);
extern void Lcd_WriteOneDot(u16 xAddr, u16 yAddr, u16 Color);								//写入一个点
extern u16 Lcd_ReadOneDot(u16 xAddr, u16 yAddr);											//读取一个点
extern u8 Lcd_CheckArea(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr);					//检查区域是否合法
extern u8 Lcd_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 *DataBuf);		//刷新部分屏幕
extern u8 Lcd_8Bit_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u8 *DataBuf);	//刷新部分屏幕-数据是8位数据
extern void Lcd_WriteAll(u16 *DataBuf);														//刷新全部屏幕
extern u8 Lcd_ClearPart(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 color);			//清除部分屏幕
extern void Lcd_ClearAll(u16 color);														//清除全部屏幕




#endif
