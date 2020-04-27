#ifndef _LCD_SSD1963_H_
#define _LCD_SSD1963_H_

//----------------------------��LCD��Ļ�йصĲ���------------------------------//
//LCD�����ò���
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

#define LCDP_PLLM		49		//�õ�VCOΪ400MHz
#define LCDP_PLLN		3		//�õ�PLLΪ100MHz

#define LCDP_FPR		67108	//�õ�����ʱ��Ƶ��Ϊ6.4MHz

#define LCDP_REVERSE

#ifdef LCDP_REVERSE				//����Ƿ���
	#define LCDP_MAX_X		240		//x����������		��Ϊ��Ļ���ŷţ��ʺ����෴
	#define LCDP_MAX_Y		320		//y����������
#else
	#define LCDP_MAX_X		320		//x����������
	#define LCDP_MAX_Y		240		//y����������
#endif


//----------------------------��LCD�������йصĲ���------------------------------//
//٤������
#define LCDP_GAMMA0		0
#define LCDP_GAMMA1		1
#define LCDP_GAMMA2		2
#define LCDP_GAMMA3		3

//0x36������-��LCD�������й�
#define LCDP_TopToBottom			0x00	//���·�ת
#define LCDP_BottomToTop			0x80

#define LCDP_LeftToRight			0x00	//���ҷ�ת
#define LCDP_RightToLeft			0x40

#define LCDP_Normal					0x00	//�к��е���
#define LCDP_Reverse				0x20

#define LCDP_Refresh_TopBottom		0x00	//���ϵ���ˢ��(��frame buffer��display�ı仯)
#define LCDP_Refresh_BottomTop		0x10

#define LCDP_SetRGB					0x00	//������ɫģʽΪRGB����BGR(��frame buffer��display�ı仯)
#define LCDP_SetBGR					0x08

#define LCDP_Refresh_LeftRight		0x00	//������ˢ��(��frame buffer��display�ı仯)
#define LCDP_Refresh_RightLeft		0x04

#define LCDP_NormalLeftRight		0x00	//���ҷ�ת(��frame buffer��display�ı仯)
#define LCDP_FlipLeftRight			0x02

#define LCDP_NormalTopBottom		0x00	//���·�ת(��frame buffer��display�ı仯)
#define LCDP_FlipTopBottom			0x01

//0xB0������-��LCD��Ļ�й�
#define LCDP_24Bit					0x20	//Һ�������ݿ��Ϊ24λ
#define LCDP_18Bit_DisAll			0x00	//Һ�������ݿ��Ϊ18λ����ʹ��dither��FRC
#define LCDP_18Bit_EnDither			0x10	//Һ�������ݿ��Ϊ18λ��ʹ��dither
#define LCDP_18Bit_EnFRC			0x18	//Һ�������ݿ��Ϊ18λ��ʹ��FRC

#define LCDP_Data_Falling			0x00	//�½��ػ�ȡ����
#define LCDP_Data_Rising			0x04	//�����ػ�ȡ����

#define LCDP_HSYNC_Low				0x00	//HSYNC����Ч
#define LCDP_HSYNC_High				0x02	//HSYNC����Ч

#define LCDP_VSYNC_Low				0x00	//VSYNC����Ч
#define LCDP_VSYNC_High				0x01	//VSYNC����Ч

//0xB8������-��GPIO���й�
#define LCDP_GPIOControlHost		0x00	//��Ƭ������GPIO��
#define LCDP_GPIOControlLCDC		0xF0	//LCD����������GPIO��

#define LCDP_GPIO_Output			0x0F	//GPIO���
#define LCDP_GPIO_Input				0x00	//GPIO����

#define LCDP_GPIO0_Normal			0x01	//GPIO0��Ϊ��ͨIO
#define LCDP_GPIO0_Sleep			0x00	//GPIO0��Ϊ˯��ģʽ�Ŀ���

//0xBA������-����GPIO�����
#define LCDP_GPIO0					0x01
#define LCDP_GPIO1					0x02
#define LCDP_GPIO2					0x04
#define LCDP_GPIO3					0x08

//0xBC������-���öԱȶȡ����ȡ����Ͷ�
#define LCDP_EnPostprocess			0x01	//����Աȶȡ����ȡ����Ͷȴ���
#define LCDP_DisPostprocess			0x00	//��ֹ�Աȶȡ����ȡ����Ͷȴ���

//0xE0������-����PLL����
#define LCDP_UsePLL					0x02	//ʹ��PLL��Ϊϵͳʱ��
#define LCDP_UseCrystal				0x00	//ʹ�òο�ʱ����Ϊϵͳʱ��

#define LCDP_EnPLL					0x01	//��PLL
#define LCDP_DisPLL					0x00	//�ر�PLL

#define LCDP_PLL_LOCKED             0x04    //PLL LOCK

//0xF0������-���õ�Ƭ����Һ������ͨѶ����
#define LCDP_Pixel_8Bit				0x00	//8λ���ݳ���
#define LCDP_Pixel_12Bit			0x01	//12λ���ݳ���
#define LCDP_Pixel_16Bit			0x02	//16λ���ݳ���
#define LCDP_Pixel_16Bit565			0x03	//16λ���ݳ��ȣ�ʹ��565��ʽ
#define LCDP_Pixel_18Bit			0x04	//18λ���ݳ���
#define LCDP_Pixel_24Bit			0x05	//24λ���ݳ���
#define LCDP_Pixel_9Bit				0x06	//9λ���ݳ���



//LCD����
#define LCD00_NOP					0x00	//����ָ��					����ֵ
#define LCD01_SOFT_RST				0x01	//�����λ					����ֵ,����E0��E5����������ȫ����λ����Ҫ�ȴ�5������ٷ�����
#define LCD0A_GET_POWER				0x0A	//ģ�鹤��ģʽ				��ȡһ����ֵ
#define LCD0B_GET_ADDR				0x0B	//ģ���LCD�����ĵ�ַ����ʽ	��ȡһ����ֵ
#define LCD0D_GET_DISPLAY			0x0D	//LCD��ʾģʽ				��ȡһ����ֵ
#define LCD0E_GET_TEAR				0x0E	//TE���Ź���				��ȡһ����ֵ
#define LCD10_INTO_SLEEP			0x10	//����˯��ģʽ				����ֵ�����ܸı�GPIO0����Ҫ��B8���ã���Ҫ�ȴ�5�����ٷ�����
#define LCD11_EXIT_SLEEP			0x11	//�˳�˯��ģʽ				����ֵ�����ܸı�GPIO0����Ҫ��B8���ã���Ҫ�ȴ�5�����ٷ�����Զ�����29
#define LCD12_PART_DISPLAY			0x12	//���벿����ʾģʽ			����ֵ����ʾ����ʱ����Ҫ�ȵ���30������ʾ����
#define LCD13_NORMAL_DISPLAY		0x13	//����������ʾģʽ			����ֵ�����ø��������Զ��˳�������ʾģʽ
#define LCD20_EXIT_INVERT			0x20	//�˳�������ʾģʽ			����ֵ��������ʾ��д��1Ϊ0��д��0Ϊ1��
#define LCD21_INTO_INVERT			0x21	//���뷴����ʾģʽ			����ֵ
#define LCD26_GAMMA					0x26	//����٤������				д��һ����ֵ
#define LCD28_DISPLAY_OFF			0x28	//�ر�Һ����ʾ				����ֵ
#define LCD29_DISPLAY_ON			0x29	//��Һ����ʾ				����ֵ
#define LCD2A_COLUMN_ADDR			0x2A	//�����е�ַ				д���ĸ���ֵ
#define LCD2B_ROW_ADDR				0x2B	//�����е�ַ				д���ĸ���ֵ
#define LCD2C_WRITE_MEMORY			0x2C	//��ʼд������				д��Һ����������
#define LCD2E_READ_MEMORY			0x2E	//��ʼ��������				��ȡҺ����������
#define LCD30_SET_PARTIAL			0x30	//���ò�����ʾģʽ������	д���ĸ���ֵ������ֻ��һ��һ�еĸı�
#define LCD33_SCROLL_AREA			0x33	//������Ļ��������			д��������ֵ�������Ϸ��̶������򣬹��������������·��̶�������
#define LCD34_TEAR_OFF				0x34	//�ر�TE���Ź���			����ֵ
#define LCD35_TEAR_ON				0x35	//��TE���Ź���			д��һ����ֵ��TE���ŵĹ���
#define LCD36_ADDR_MODE				0x36	//���õ�ַģʽ				д��һ����ֵ
												//A7��0-���ϵ���		1-���µ���
												//A6��0-������		1-���ҵ���
												//A5��0-����			1-�к��з�ת���������У�������
												//A4��0-���ϵ���ˢ��	1-���µ���ˢ��
												//A3��0-RGB				1-BGR
												//A2��0-������ˢ��	1-���ҵ���ˢ��
												//A1��0-������ʾ		1-buffer���������ҷ�����ʾ
												//A0��0-������ʾ		1-buffer���������·�����ʾ
#define LCD37_SCROLL_START			0x37	//���ù��������ڼ���		д��������ֵ
#define LCD38_EXIT_IDLE				0x38	//�˳�IDLEģʽ				����ֵ��IDLEģʽ������������Լ���ɫ��ֻ����ʹ�ü�����ɫ
#define LCD39_INTO_IDLE				0x39	//����IDLEģʽ				����ֵ
#define LCD3C_W_MEM_CONTIN			0x3C	//����д������				д��Һ����������
#define LCD3E_R_MEM_CONTIN			0x3E	//������������				��ȡҺ����������
#define LCD44_TEAR_LINE				0x44	//ɨ�赽�ڼ���TE�������ź�	д��������ֵ
#define LCD45_GET_SCANLINE			0x45	//��ȡ��ǰɨ�������		��ȡ������ֵ
#define LCDA1_READ_INFO				0xA1	//��ȡ�豸����				��ȡ�����ֵ
#define LCDB0_W_LCDMODE				0xB0	//����LCD��ģʽ����			д���߸���ֵ
#define LCDB1_R_LCDMODE				0xB1	//��ȡLCD��ģʽ����			��ȡ�߸���ֵ
#define LCDB4_W_HENG_TIME			0xB4	//����LCD����ɨ���ʱ��		д��˸���ֵ
#define LCDB5_R_HENG_TIME			0xB5	//��ȡLCD����ɨ���ʱ��		��ȡ�˸���ֵ
#define LCDB6_W_SHU_TIME			0xB6	//����LCD����ɨ���ʱ��		д���߸���ֵ
#define LCDB7_R_SHU_TIME			0xB7	//��ȡLCD����ɨ���ʱ��		��ȡ�߸���ֵ
#define LCDB8_SET_GPIO				0xB8	//����GPIO�ڵ�ģʽ			д��������ֵ
#define LCDB9_GET_GPIO				0xB9	//��ȡGPIO�ڵ�ģʽ			��ȡ������ֵ
#define LCDBA_SET_IO_VAL			0xBA	//����GPIO�ڵ���ֵ			д��һ����ֵ
#define LCDBB_GET_IO_VAL			0xBB	//��ȡGPIO�ڵ���ֵ			��ȡһ����ֵ
#define LCDBC_W_POSTPRO				0xBC	//���ú��ڴ���				д���ĸ���ֵ�������Աȶȡ����ȡ����Ͷ�����
#define LCDBD_R_POSTPRO				0xBD	//��ȡ���ڴ���				��ȡ�ĸ���ֵ
#define LCDBE_W_PWM					0xBE	//����PWMʱ��				д��������ֵ
#define LCDBF_R_PWM					0xBF	//��ȡPWMʱ��				��ȡ������ֵ
#define LCDC0_W_GEN0				0xC0	//GPIO0���ģʽ����			д���߸���ֵ
#define LCDC1_R_GEN0				0xC1	//GPIO0���ģʽ����			��ȡ�߸���ֵ
#define LCDC2_W_GEN1				0xC2	//GPIO1���ģʽ����			д���߸���ֵ
#define LCDC3_R_GEN1				0xC3	//GPIO1���ģʽ����			��ȡ�߸���ֵ
#define LCDC4_W_GEN2				0xC4	//GPIO2���ģʽ����			д���߸���ֵ
#define LCDC5_R_GEN2				0xC5	//GPIO2���ģʽ����			��ȡ�߸���ֵ
#define LCDC6_W_GEN3				0xC6	//GPIO3���ģʽ����			д���߸���ֵ
#define LCDC7_R_GEN3				0xC7	//GPIO3���ģʽ����			��ȡ�߸���ֵ
#define LCDC8_W_GPIO0_ROP			0xC8	//GPIO0ROPģʽ����			д��������ֵ
#define LCDC9_R_GPIO0_ROP			0xC9	//GPIO0ROPģʽ��ȡ			��ȡ������ֵ
#define LCDCA_W_GPIO1_ROP			0xCA	//GPIO1ROPģʽ����			д��������ֵ
#define LCDCB_R_GPIO1_ROP			0xCB	//GPIO1ROPģʽ��ȡ			��ȡ������ֵ
#define LCDCC_W_GPIO2_ROP			0xCC	//GPIO2ROPģʽ����			д��������ֵ
#define LCDCD_R_GPIO2_ROP			0xCD	//GPIO2ROPģʽ��ȡ			��ȡ������ֵ
#define LCDCE_W_GPIO3_ROP			0xCE	//GPIO3ROPģʽ����			д��������ֵ
#define LCDCF_R_GPIO3_ROP			0xCF	//GPIO3ROPģʽ��ȡ			��ȡ������ֵ
#define LCDD0_W_DBC					0xD0	//����DBC����				д��һ����ֵ
#define LCDD1_R_DBC					0xD1	//��ȡDBC����				��ȡһ����ֵ
#define LCDD4_W_DBC_TH				0xD4	//����DBC�ڲ�ͬ��ѹ�µ�ֵ	д��Ÿ���ֵ
#define LCDD5_R_DBC_TH				0xD5	//��ȡDBC�ڲ�ͬ��ѹ�µ�ֵ	��ȡ�Ÿ���ֵ
#define LCDE0_W_PLL					0xE0	//����PLL					д��һ����ֵ
#define LCDE2_W_PLL_MN				0xE2	//����PLL��MNֵ				д��������ֵ
#define LCDE3_R_PLL_MN				0xE3	//��ȡPLL��MNֵ				��ȡ������ֵ
#define LCDE4_R_PLL_STAT			0xE4	//��ȡPLL��״̬				��ȡһ����ֵ
#define LCDE5_SET_D_SLEEP			0xE5	//�������˯��				����ֵ����Ҫ���ζ����˳����˯��
#define LCDE6_SET_FPR				0xE6	//��������ʱ��Ƶ��			д��������ֵ
#define LCDE7_GET_FPR				0xE7	//��ȡ����ʱ��Ƶ��			��ȡ������ֵ
#define LCDF0_SET_DATALEN			0xF0	//�������ݳ���λ��			д��һ����ֵ
#define LCDF1_GET_DATALEN			0xF1	//��ȡ���ݳ���λ��			��ȡһ����ֵ


#define LCD_BRIGHT_RANK   32
//----------------------------��GPIO���йصĲ���------------------------------//
	//PC9��LCD�������-���
#define LCD_BRIGHT_PORT			GPIOI
#define LCD_BRIGHT_PORT_RCC		RCC_AHB1Periph_GPIOI
#define LCD_BRIGHT_PIN			GPIO_Pin_11

	//PD11��LCD����ģ�鸴λ-���
#define LCD_CTRL_RST_PORT		GPIOD
#define LCD_CTRL_RST_PORT_RCC	RCC_AHB1Periph_GPIOD
#define LCD_CTRL_RST_PIN		GPIO_Pin_11

	//PE2��LCDˢ�����-����
#define LCD_SCREEN_PORT			GPIOE
#define LCD_SCREEN_PORT_RCC		RCC_AHB1Periph_GPIOE
#define LCD_SCREEN_PIN			GPIO_Pin_2


//IO�ڿ���
#define LCD_BRIGHT_H()		GPIO_WriteBit(LCD_BRIGHT_PORT, LCD_BRIGHT_PIN, Bit_SET)
#define LCD_BRIGHT_L()		GPIO_WriteBit(LCD_BRIGHT_PORT, LCD_BRIGHT_PIN, Bit_RESET)

#define LCD_CTRL_RST_H()	GPIO_WriteBit(LCD_CTRL_RST_PORT, LCD_CTRL_RST_PIN, Bit_SET)
#define LCD_CTRL_RST_L()	GPIO_WriteBit(LCD_CTRL_RST_PORT, LCD_CTRL_RST_PIN, Bit_RESET)

#define IS_LCD_SCREEN_OK()	GPIO_ReadInputDataBit(LCD_SCREEN_PORT, LCD_SCREEN_PIN)


//----------------------------LCD��������GPIO�ڶ�Ӧ�Ĺ���------------------------------//
#define LCDSPI_RST		LCDP_GPIO0
#define LCDSPI_CLK		LCDP_GPIO1
#define LCDSPI_DATA		LCDP_GPIO2
#define LCDSPI_CS		LCDP_GPIO3


//----------------------------��дLCD�������ĵ�ַ�������ʽ------------------------------//

//ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
//FSMC_A17 ��LCD��DC(�Ĵ���/����ѡ��)��
//16 bit => FSMC[24:0]��ӦHADDR[25:1]
//�Ĵ�������ַ = 0X60000000
//RAM����ַ = 0X60020000 = 0X60000000+2^17*2 = 0X60000000 + 0X40000 = 0X60040000
//��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼��㡣

#define Bank1_LCD_D		((unsigned int)0x60040000)		//���ݵ�ַ
#define Bank1_LCD_C		((unsigned int)0x60000000)		//�����ַ

//������д���������
#define LCD_WRITE_CMD(index)		((*(__IO u16 *) (Bank1_LCD_C)) = ((u16)index))
#define LCD_WRITE_PARAMETER(val)	((*(__IO u16 *) (Bank1_LCD_D)) = ((u16)(val)))
//��ȡ����
#define LCD_READ_PARAMETER()		*(__IO u16 *) (Bank1_LCD_D)





//----------------------------�ⲿ�ɵ��ú���------------------------------//
extern void LcdSetBright(u8 Clk);
extern void LcdCloseBright(void);
extern void LCD_FSMC_Config(void);		//��Ƭ������LCD�����Ź��ܳ�ʼ��
extern void Lcdc_Reset(void);			//LCD��������λ
extern void Lcd_Reset(void);			//LCD��Ļ��λ
extern void LcdSSD1963_Init(void);				//LCD����������Ļ��ʼ��
extern void Lcd_SetCursor(u16 xAddr, u16 yAddr);
extern void Lcd_OpenShow(void);			//��Һ����ʾ
extern void Lcd_CloseShow(void);		//�ر�Һ����ʾ
extern void LcdSSD1963_ResetInit(void);

extern ErrorStatus Lcd_PllLockState(void);
extern void Lcd_WriteOneDot(u16 xAddr, u16 yAddr, u16 Color);								//д��һ����
extern u16 Lcd_ReadOneDot(u16 xAddr, u16 yAddr);											//��ȡһ����
extern u8 Lcd_CheckArea(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr);					//��������Ƿ�Ϸ�
extern u8 Lcd_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 *DataBuf);		//ˢ�²�����Ļ
extern u8 Lcd_8Bit_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u8 *DataBuf);	//ˢ�²�����Ļ-������8λ����
extern void Lcd_WriteAll(u16 *DataBuf);														//ˢ��ȫ����Ļ
extern u8 Lcd_ClearPart(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 color);			//���������Ļ
extern void Lcd_ClearAll(u16 color);														//���ȫ����Ļ




#endif
