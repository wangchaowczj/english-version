#include "includes.h"
//#include "LCD_SSD1963.h"





/**
  *************************************************************************************
  * @brief  LCD��������
  * @param  Clk ��ֵԽ����ĻԽ����ԽСԽ��������Ϊ1
  * @retval ��
  * @author ZH
  * @date   2014��11��21��
  * @note   ��
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
  * @brief  �ر�LCD����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��11��21��
  * @note   ��
  *************************************************************************************  
  */
void LcdCloseBright(void)
{
	LCD_BRIGHT_L();
}
//==================================================================================================
//| �������� | LCD_FSMC_Config
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD��FSMCģʽ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void LCD_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);		//D0��D15��IO��ʱ�Ӵ�

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);								//FSMC��ʱ�Ӵ�

	//****GPIO����****//
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

	RCC_AHB1PeriphClockCmd(LCD_BRIGHT_PORT_RCC | LCD_SCREEN_PORT_RCC | LCD_CTRL_RST_PORT_RCC, ENABLE);	//���ƿڵ�IOʱ�Ӵ�


	//****PC9��LCD_DIMM			LCD����������ţ�����������ķ�������������LCD�ı������������С****//
	GPIO_InitStructure.GPIO_Pin = LCD_BRIGHT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_BRIGHT_PORT, &GPIO_InitStructure);

	//****PD11��IO_LCDC_RST		LCD����ģ�鸴λ����****//
	GPIO_InitStructure.GPIO_Pin = LCD_CTRL_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_CTRL_RST_PORT, &GPIO_InitStructure);

	//****PE2��LCDC_TE		LCDˢ������ж�����****//
	GPIO_InitStructure.GPIO_Pin = LCD_SCREEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_SCREEN_PORT, &GPIO_InitStructure);

	//****FSMC����****//
	p.FSMC_AddressSetupTime = 10;			//��ַ����ʱ��
	p.FSMC_AddressHoldTime = 0;				//��ַ����ʱ��
	p.FSMC_DataSetupTime = 60;				//���ݽ���ʱ��
	p.FSMC_BusTurnAroundDuration = 10;		//���߻ָ�ʱ��
	p.FSMC_CLKDivision = 0;					//ʱ�ӷ�Ƶ
	p.FSMC_DataLatency = 0;					//���ݱ���ʱ��
	p.FSMC_AccessMode = FSMC_AccessMode_A;	//ģʽA

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;							//NOR FLASH��BANK1
	//MUXENλ	�������˸�λ�󣬵�ַ�ĵ�16λ�����ݽ������������ߣ���λ����NOR��PSRM�洢����Ч��
	//0����ַ/���ݲ����á�		1����ַ/���ݸ����������ߣ����Ǹ�λ���Ĭ��״̬��
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;		//���������ַ�߲�����
	//MTYPλ	�����ⲿ�洢��������
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;					//�洢�����ͣ�ԭ��ΪFSMC_MemoryType_SRAM
	//MWIDλ	�����ⲿ�洢�����ߵĿ�ȣ��������������͵Ĵ洢����
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;			//���ݿ��Ϊ16λ
	//BURSTENλ	���������洢�����г���ģʽ���ʣ���λ��������洢����ͬ������ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;		//ʹ���첽дģʽ����ֹͻ��ģʽ
	//ASYNCWAITλ
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	//WAITPOL	���ô洢�������ĵȴ��źŵļ��ԣ���λ���ڴ洢���ĳ���ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	//����Ա������ֻ��ͻ��ģʽ����Ч���ȴ��źż���Ϊ��
	//WRAPMOD	��λ�����������Ƿ�֧�ְѷǶ����AHB��������ָ��2�����Բ�������λ���ڴ洢���ĳ���ģʽ����Ч��
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;					//��ֹ�Ƕ���ͻ��ģʽ
	//WAITCFG	������洢�����ڳ��鴫��ģʽʱ��NWAIT�ź�ָʾ������洢�������������Ƿ���Ч���Ƿ���Ҫ����ȴ����ڡ�
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;	//����Ա���ý���ͻ��ģʽ����Ч��NWAIT�ź���ʲôʱ�ڲ���
	//WREN		��λָʾFSMC�Ƿ�����/��ֹ�Դ洢����д������
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			//дʹ��
	//WAITEN	������洢�����ڳ��鴫��ģʽʱ����һλ����/��ֹͨ��NWAIT�źŲ���ȴ�״̬��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;				//����Ա������ֻ��ͻ��ģʽ����Ч������NWAIT�ź�
	//EXTMOD	��λ����FSMCʹ��FSMC_BWTR�Ĵ��������������дʹ�ò�ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;				//������չģʽ
	//CBURSTRW	����дʹ��λ
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;				//��ֹͻ��д����
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;							//���ö�дʱ��
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;								//����дʱ��

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);		//ʹ��FSMC Bank1_SRAM Bank

	LCD_CTRL_RST_H();
	LCD_BRIGHT_L();
}

//==================================================================================================
//| �������� | Lcdc_Reset
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD�������ĸ�λ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void Lcdc_Reset(void)
{
	LCD_CTRL_RST_L();
	System168MDelay1ms(10);
	LCD_CTRL_RST_H();
	System168MDelay1ms(10);
}

//==================================================================================================
//| �������� | Lcd_SpiSend
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD��������GPIO�ڷ���SPI��ʽ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | SendData��Ҫ���͵����ݣ�16�ֽ�
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
static void Lcdc_SpiSend(u16 SendData)
{
	u8 i;

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//CS������
	LCD_WRITE_PARAMETER((LCDSPI_CLK | LCDSPI_DATA | LCDSPI_RST) & (~LCDSPI_CS));

	for (i=0;i<16;i++)
	{
		if (SendData & (1<<(15-i)))			//����1
		{
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_DATA | LCDSPI_RST) & ((~LCDSPI_CLK) & (~LCDSPI_CS)));
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_DATA | LCDSPI_RST | LCDSPI_CLK) & (~LCDSPI_CS));
		}
		else								//����0
		{
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER((LCDSPI_RST) & ((~LCDSPI_CLK) & (~LCDSPI_CS) & (~LCDSPI_DATA)));
			LCD_WRITE_CMD(LCDBA_SET_IO_VAL);
			LCD_WRITE_PARAMETER(LCDSPI_RST | LCDSPI_CLK & (~LCDSPI_CS) & (~LCDSPI_DATA));
		}
	}

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//CS������
	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_RST | LCDSPI_CS);

}

//==================================================================================================
//| �������� | Lcd_Reset
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD��Ļ�ĸ�λ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void Lcd_Reset(void)
{
	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//д����ֵ
	LCD_WRITE_PARAMETER((LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS) & (~LCDSPI_RST));
	System168MDelay1ms(1);						//��ʱ1����
//	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//д����ֵ
//	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS | LCDSPI_RST);
}

//==================================================================================================
//| �������� | Lcdc_Spi_Init
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD��������SPI�ڷ������ݣ���LCD��Ļ��ʼ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
static void Lcdc_Spi_Init(void)
{
	//LCDģ�鸴λ
	Lcd_Reset();
	OSTimeDlyHMSM(0,0,0,50);		//LCDģ�鸴λ����Ҫ�ȴ�40��������ɸ�λ�����ܷ�������

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

	LCD_WRITE_CMD(LCDBA_SET_IO_VAL);		//ȫ����������
	LCD_WRITE_PARAMETER(LCDSPI_CLK | LCDSPI_DATA | LCDSPI_CS | LCDSPI_RST);
}

void Lcd_SetCursor(u16 xAddr, u16 yAddr)
{
	u16 x,y;
	
	#ifdef LCDP_REVERSE		//X��Y��������
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif	
	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
}
//==================================================================================================
//| �������� | Lcd_WriteOneDot
//|----------|--------------------------------------------------------------------------------------
//| �������� | д����Ļһ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | xAddr��X������		yAddr��Y������		Color����ɫ
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void Lcd_WriteOneDot(u16 xAddr, u16 yAddr, u16 Color)
{
	u16 x,y;

	#ifdef LCDP_REVERSE		//X��Y��������
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	LCD_WRITE_PARAMETER(Color);
}

//==================================================================================================
//| �������� | Lcd_ReadOneDot
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ��Ļһ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | xAddr��X������		yAddr��Y������
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | �õ����ɫ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u16 Lcd_ReadOneDot(u16 xAddr, u16 yAddr)
{
	u16 reData;
	u16 x,y;

	#ifdef LCDP_REVERSE		//X��Y��������
		x = yAddr;
		y = xAddr;
	#else
		x = xAddr;
		y = yAddr;
	#endif

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);
	LCD_WRITE_PARAMETER(((x) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(x & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);
	LCD_WRITE_PARAMETER(((y) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(y & 0xFF);

	LCD_WRITE_CMD(LCD2E_READ_MEMORY);		//��������
	reData = LCD_READ_PARAMETER();
	return reData;
}

//==================================================================================================
//| �������� | Lcd_CheckArea
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������Ƿ�Ϸ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | xSAddr��X����ʼ����		xEAddr��X���������
//|          | ySAddr��Y����ʼ����		yEAddr��Y���������
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���ɹ�		1����ʼ������ڽ�������		2������ֵ������Χ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
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
//| �������� | Lcd_WriteData
//|----------|--------------------------------------------------------------------------------------
//| �������� | ˢ�²�����Ļ
//|----------|--------------------------------------------------------------------------------------
//| ������� | xSAddr��X����ʼ����		xEAddr��X���������
//|          | ySAddr��Y����ʼ����		yEAddr��Y���������
//|          | DataBuf���������
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���ɹ�		1����ʼ������ڽ�������		2������ֵ������Χ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u8 Lcd_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 *DataBuf)
{
	u32 i;
	u32 wlen;		//д�����ݳ���
	u8 state;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X��Y��������
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

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(*(DataBuf++));
	}

	return 0;
}

//==================================================================================================
//| �������� | Lcd_8Bit_WriteData
//|----------|--------------------------------------------------------------------------------------
//| �������� | ˢ�²�����Ļ�����������Ϊ8λ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | xSAddr��X����ʼ����		xEAddr��X���������
//|          | ySAddr��Y����ʼ����		yEAddr��Y���������
//|          | DataBuf���������
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���ɹ�		1����ʼ������ڽ�������		2������ֵ������Χ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u8 Lcd_8Bit_WriteData(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u8 *DataBuf)
{
	u32 i;
	u32 wlen;		//д�����ݳ���
	u8 state;
	u16 data;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X��Y��������
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

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	for (i=0;i<wlen;i++)
	{
		data = (u16)(*(DataBuf++) << 8) & 0xFF00;
		data |= (u16)(*(DataBuf++)) & 0x00FF;
		LCD_WRITE_PARAMETER(data);
	}

	return 0;
}

//==================================================================================================
//| �������� | Lcd_WriteAll
//|----------|--------------------------------------------------------------------------------------
//| �������� | ˢ��ȫ����Ļ
//|----------|--------------------------------------------------------------------------------------
//| ������� | DataBuf���������
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void Lcd_WriteAll(u16 *DataBuf)
{
	u32 i;
	u32 wlen;

	wlen = LCDP_HDISP*LCDP_VDISP;

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(*(DataBuf++));
	}
}

//==================================================================================================
//| �������� | Lcd_ClearPart
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���������Ļ
//|----------|--------------------------------------------------------------------------------------
//| ������� | xSAddr��X����ʼ����		xEAddr��X���������
//|          | ySAddr��Y����ʼ����		yEAddr��Y���������
//|          | color���Ը���ɫ���
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | 0���ɹ�		1����ʼ������ڽ�������		2������ֵ������Χ
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
u8 Lcd_ClearPart(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 color)
{
	u32 i;
	u32 wlen;		//д�����ݳ���
	u8 state;
	u16 xs,xe,ys,ye;

	#ifdef LCDP_REVERSE		//X��Y��������
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

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(((xs) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xs & 0xFF);
	LCD_WRITE_PARAMETER(((xe) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(xe & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(((ys) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ys & 0xFF);
	LCD_WRITE_PARAMETER(((ye) >> 8) & 0xFF);
	LCD_WRITE_PARAMETER(ye & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(color);
	}

	return 0;
}

//==================================================================================================
//| �������� | Lcd_ClearAll
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���ȫ����Ļ
//|----------|--------------------------------------------------------------------------------------
//| ������� | color���Ը���ɫ���
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void Lcd_ClearAll(u16 color)
{
	u32 i;
	u32 wlen;

	wlen = LCDP_HDISP*LCDP_VDISP;

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);		//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);			//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2C_WRITE_MEMORY);		//д������
	for (i=0;i<wlen;i++)
	{
		LCD_WRITE_PARAMETER(color);
	}
}




//==================================================================================================
//| �������� | Lcd_PllLockState
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡPLL����״̬
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�zh    ʱ�䣺2015-05-13
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | �������ж�LCD�Ƿ��Ѿ����⸴λ
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
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
//| �������� | Lcd_Init
//|----------|--------------------------------------------------------------------------------------
//| �������� | LCD��������LCD��Ļ�ĳ�ʼ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17
//|----------|--------------------------------------------------------------------------------------
//|   ��ע   | 
//|----------|--------------------------------------------------------------------------------------
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ�
//==================================================================================================
void LcdSSD1963_Init(void)
{
	u8 state;
	u8 i = 10;
	
	LCD_BRIGHT_L();
	
	Lcdc_Reset();
		
	LCD_WRITE_CMD(LCDE2_W_PLL_MN);							//����PLL��M��Nֵ
	LCD_WRITE_PARAMETER(LCDP_PLLM);							//����M
	LCD_WRITE_PARAMETER(LCDP_PLLN);							//����N
	LCD_WRITE_PARAMETER(0x54);								//ʹ��M��N

	do
	{
		LCD_WRITE_CMD(LCDE0_W_PLL);							//����PLL
		LCD_WRITE_PARAMETER(LCDP_UsePLL | LCDP_EnPLL);		//��PLLʱ�ӣ�������PLLʱ����Ϊϵͳʱ��

		System168MDelay1ms(1);									//��ʱ1����

		LCD_WRITE_CMD(LCDE4_R_PLL_STAT);					//��PLL
		state = LCD_READ_PARAMETER();						//��PLLʱ�ӣ�������PLLʱ����Ϊϵͳʱ��

		if (state == LCDP_PLL_LOCKED)
		{
			break;
		}
	}while(i--);
	System168MDelay1ms(10);
	LCD_WRITE_CMD(LCD01_SOFT_RST);							//Һ������λ
	System168MDelay1ms(10);										//�ȴ�10���룬Һ����Ҫ��ȴ�5���룬��ȴ�һ��ʱ��

	LCD_WRITE_CMD(LCDE6_SET_FPR);							//����LCDC_FPR���������ڵõ�����Ƶ��
	LCD_WRITE_PARAMETER(LCDP_FPR >> 16);					//FPR��ֵ
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x0000FF00) >> 8);
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x000000FF));

	LCD_WRITE_CMD(LCDB0_W_LCDMODE);							//����LCD��Ļ�Ĳ���
	LCD_WRITE_PARAMETER(LCDP_24Bit | 
						LCDP_Data_Rising | 
						LCDP_HSYNC_Low | 
						LCDP_VSYNC_Low);
	LCD_WRITE_PARAMETER(0x00);								//TFTģʽ
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//Һ����������320
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//Һ����������240
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER(0x00);								//��TFTģʽ�¸�λ����Ҫ����

	LCD_WRITE_CMD(LCDB4_W_HENG_TIME);						//���ú���ɨ���ʱ�䣬����LCD���ϵõ�����ֵ
	LCD_WRITE_PARAMETER((LCDP_HT-1) >> 8);					//��ʱ��-1
	LCD_WRITE_PARAMETER((LCDP_HT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) >> 8);			//��ͬ���źſ�ʼ�����ݵ����ʱ��
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_HS-1);							//ͬ���ź�ռ�õ�ʱ��-1
	LCD_WRITE_PARAMETER(0x00);								//��ʱ����ɺ���һ�����ݿ�ʼ��ʱ��
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);								//����ʹ�ã����пڲ�ʹ��

	LCD_WRITE_CMD(LCDB6_W_SHU_TIME);						//��������ɨ���ʱ�䣬����LCD���ϵõ�����ֵ
	LCD_WRITE_PARAMETER((LCDP_VT-1) >> 8);					//��ʱ��-1
	LCD_WRITE_PARAMETER((LCDP_VT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) >> 8);			//��ͬ���źſ�ʼ�����ݵ����ʱ��
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_VS-1);							//ͬ���ź�ռ�õ�ʱ��-1
	LCD_WRITE_PARAMETER(0x00);								//��ʱ����ɺ���һ�����ݿ�ʼ��ʱ��
	LCD_WRITE_PARAMETER(0x00);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);						//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);								//����ʼ��ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//�н�����ַ
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);							//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);								//����ʼ��ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//�н�����ַ
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD36_ADDR_MODE);							//���õ�ַģʽ
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

	LCD_WRITE_CMD(LCDB8_SET_GPIO);							//����GPIO����
	LCD_WRITE_PARAMETER(LCDP_GPIOControlHost | 
						LCDP_GPIO_Output);					//GPIO�����ó����
	LCD_WRITE_PARAMETER(LCDP_GPIO0_Normal);					//GPIO0��Ϊ��ͨIO��

	Lcdc_Spi_Init();										//ͨ��LCD��������GPIO�ڷ���SPI��ʽ���ݣ���ʼ��LCD��Ļ

	LCD_WRITE_CMD(LCDF0_SET_DATALEN);						//���õ�Ƭ����LCD������������λ������ʽ
	LCD_WRITE_PARAMETER(LCDP_Pixel_16Bit565);				//����Ϊ16λ565��ʽ����

	Lcd_ClearAll(BLACK);//��������
	
	LCD_BRIGHT_L();//�ȹر��⣬��Ȼ���е������������ط��ٿ�
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//����ʾ	
	//LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength+1);//���ñ���ǿ��	
}

//LCD��ʹ�ù����о�Ȼ��λ����ñ��������ú�������������
void LcdSSD1963_ResetInit(void)
{
	u8 state;
	u8 i = 10;
	
	Lcdc_Reset();
		
	LCD_WRITE_CMD(LCDE2_W_PLL_MN);							//����PLL��M��Nֵ
	LCD_WRITE_PARAMETER(LCDP_PLLM);							//����M
	LCD_WRITE_PARAMETER(LCDP_PLLN);							//����N
	LCD_WRITE_PARAMETER(0x54);								//ʹ��M��N

	do
	{
		LCD_WRITE_CMD(LCDE0_W_PLL);							//����PLL
		LCD_WRITE_PARAMETER(LCDP_UsePLL | LCDP_EnPLL);		//��PLLʱ�ӣ�������PLLʱ����Ϊϵͳʱ��

		System168MDelay1ms(1);									//��ʱ1����

		LCD_WRITE_CMD(LCDE4_R_PLL_STAT);					//��PLL
		state = LCD_READ_PARAMETER();						//��PLLʱ�ӣ�������PLLʱ����Ϊϵͳʱ��

		if (state == LCDP_PLL_LOCKED)
		{
			break;
		}
	}while(i--);

	LCD_WRITE_CMD(LCD01_SOFT_RST);							//Һ������λ
	System168MDelay1ms(10);										//�ȴ�10���룬Һ����Ҫ��ȴ�5���룬��ȴ�һ��ʱ��

	LCD_WRITE_CMD(LCDE6_SET_FPR);							//����LCDC_FPR���������ڵõ�����Ƶ��
	LCD_WRITE_PARAMETER(LCDP_FPR >> 16);					//FPR��ֵ
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x0000FF00) >> 8);
	LCD_WRITE_PARAMETER((LCDP_FPR & 0x000000FF));

	LCD_WRITE_CMD(LCDB0_W_LCDMODE);							//����LCD��Ļ�Ĳ���
	LCD_WRITE_PARAMETER(LCDP_24Bit | 
						LCDP_Data_Rising | 
						LCDP_HSYNC_Low | 
						LCDP_VSYNC_Low);
	LCD_WRITE_PARAMETER(0x00);								//TFTģʽ
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//Һ����������320
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//Һ����������240
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);
	LCD_WRITE_PARAMETER(0x00);								//��TFTģʽ�¸�λ����Ҫ����

	LCD_WRITE_CMD(LCDB4_W_HENG_TIME);						//���ú���ɨ���ʱ�䣬����LCD���ϵõ�����ֵ
	LCD_WRITE_PARAMETER((LCDP_HT-1) >> 8);					//��ʱ��-1
	LCD_WRITE_PARAMETER((LCDP_HT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) >> 8);			//��ͬ���źſ�ʼ�����ݵ����ʱ��
	LCD_WRITE_PARAMETER((LCDP_HBP+LCDP_HS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_HS-1);							//ͬ���ź�ռ�õ�ʱ��-1
	LCD_WRITE_PARAMETER(0x00);								//��ʱ����ɺ���һ�����ݿ�ʼ��ʱ��
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER(0x00);								//����ʹ�ã����пڲ�ʹ��

	LCD_WRITE_CMD(LCDB6_W_SHU_TIME);						//��������ɨ���ʱ�䣬����LCD���ϵõ�����ֵ
	LCD_WRITE_PARAMETER((LCDP_VT-1) >> 8);					//��ʱ��-1
	LCD_WRITE_PARAMETER((LCDP_VT-1) & 0xFF);
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) >> 8);			//��ͬ���źſ�ʼ�����ݵ����ʱ��
	LCD_WRITE_PARAMETER((LCDP_VBP+LCDP_VS) & 0xFF);
	LCD_WRITE_PARAMETER(LCDP_VS-1);							//ͬ���ź�ռ�õ�ʱ��-1
	LCD_WRITE_PARAMETER(0x00);								//��ʱ����ɺ���һ�����ݿ�ʼ��ʱ��
	LCD_WRITE_PARAMETER(0x00);

	LCD_WRITE_CMD(LCD2A_COLUMN_ADDR);						//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);								//����ʼ��ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) >> 8);				//�н�����ַ
	LCD_WRITE_PARAMETER((LCDP_HDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD2B_ROW_ADDR);							//�����е�ַ
	LCD_WRITE_PARAMETER(0x00);								//����ʼ��ַ
	LCD_WRITE_PARAMETER(0x00);
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) >> 8);				//�н�����ַ
	LCD_WRITE_PARAMETER((LCDP_VDISP-1) & 0xFF);

	LCD_WRITE_CMD(LCD36_ADDR_MODE);							//���õ�ַģʽ
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

	LCD_WRITE_CMD(LCDB8_SET_GPIO);							//����GPIO����
	LCD_WRITE_PARAMETER(LCDP_GPIOControlHost | 
						LCDP_GPIO_Output);					//GPIO�����ó����
	LCD_WRITE_PARAMETER(LCDP_GPIO0_Normal);					//GPIO0��Ϊ��ͨIO��

	Lcdc_Spi_Init();										//ͨ��LCD��������GPIO�ڷ���SPI��ʽ���ݣ���ʼ��LCD��Ļ

	LCD_WRITE_CMD(LCDF0_SET_DATALEN);						//���õ�Ƭ����LCD������������λ������ʽ
	LCD_WRITE_PARAMETER(LCDP_Pixel_16Bit565);				//����Ϊ16λ565��ʽ����

	Lcd_ClearAll(BLACK);//��������
//	
//	LCD_BRIGHT_L();//�ȹر��⣬��Ȼ���е������������ط��ٿ�
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//����ʾ	
	//LcdSetBright(LCD_BRIGHT_RANK-device_info.LcdBrightStrength+1);//���ñ���ǿ��	
}

void Lcd_OpenShow(void)
{
	LCD_WRITE_CMD(LCD29_DISPLAY_ON);						//����ʾ
}

void Lcd_CloseShow(void)
{
	LCD_WRITE_CMD(LCD28_DISPLAY_OFF);						//�ر���ʾ
}








