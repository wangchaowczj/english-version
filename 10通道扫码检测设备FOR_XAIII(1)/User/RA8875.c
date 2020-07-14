#include "includes.h"


void LcdReset(void)
{
	OPEN_LCD_POWER();
	SystemDelay1ms(100);
    //ȷ��LCD��λ������һ������������̣���Ȼ��ЩоƬ��λ����
    LCD_RESET_PIN_H();//Ӳ����λ
    SystemDelay10us(1);    
	LCD_RESET_PIN_L();//Ӳ����λ
	SystemDelay1ms(100);//25m����Ҫ������41us	
	LCD_RESET_PIN_H();//Ӳ����λ
	SystemDelay1ms(10);	//����1MS
	LCD_CmdWrite(0x13); //TFT RESET=L	
	LCD_DataWrite(0x04);
	SystemDelay100us(5);	
	LCD_CmdWrite(0x13);	 //TFT RESET=H
	LCD_DataWrite(0x0C);	
	SystemDelay100us(5);
}
void LcdActiveWindow(u16 XL,u16 XR ,u16 YT ,u16 YB)
{
    //setting active window X
  
    LCD_CmdWrite(0x30);//HSAW0
	LCD_DataWrite(XL); 
    LCD_CmdWrite(0x31);//HSAW1	   
	LCD_DataWrite(XL>>8);

    LCD_CmdWrite(0x34);//HEAW0
	LCD_DataWrite(XR);
    LCD_CmdWrite(0x35);//HEAW1	   
	LCD_DataWrite(XR>>8);

    //setting active window Y
  
    LCD_CmdWrite(0x32);//VSAW0
	LCD_DataWrite(YT);  
    LCD_CmdWrite(0x33);//VSAW1	   
	LCD_DataWrite(YT>>8);
  
    LCD_CmdWrite(0x36);//VEAW0
	LCD_DataWrite(YB); 
    LCD_CmdWrite(0x37);//VEAW1	   
	LCD_DataWrite(YB>>8);
}
void Chk_Busy(void)
{
//	u8 temp;
 	u32 dly = 0;
	
	while(GET_LCD_STATE() == 0)
	{
		dly++;
		if(dly > 0xFFFFF)
		{
			break;
		}
	}
	
//	do
//	{
//		dly--;
//		temp=LCD_StatusRead();
//	}while(((temp&0x80)==0x80) && (dly != 0));
}
/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetFrontColor
*	����˵��: �趨ǰ��ɫ
*	��    ��: ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdSetFrontColor(u16 color)
{	
    LCD_CmdWrite(0x63);
	LCD_DataWrite((color&0xf800)>>11);
	
    LCD_CmdWrite(0x64);
	LCD_DataWrite((color&0x07e0)>>5);
	
	LCD_CmdWrite(0x65);
	LCD_DataWrite((color&0x001f));	
}
/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetBackColor
*	����˵��: �趨����ɫ
*	��    ��: ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdSetBackgroundColor(u16 color)
{
    LCD_CmdWrite(0x60);
	LCD_DataWrite((color&0xf800)>>11);
	
    LCD_CmdWrite(0x61);
	LCD_DataWrite((color&0x07e0)>>5);
	
	LCD_CmdWrite(0x62);
	LCD_DataWrite((color&0x001f));
}
void LcdClearActiveWindow(void)
{
	u8 temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= cSetD6 ;
	LCD_DataWrite(temp);  
}

void LcdMemoryClear(void)
{
	u8 temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= cSetD7 ;
	LCD_DataWrite(temp); 
	Chk_Busy(); 
}
void LcdDisplayOn(void)
{	
    LCD_CmdWrite(0x01);//PWRR
	LCD_DataWrite(0x80);
}

/**
  *************************************************************************************
  * @brief  LCD��������
  * @param  Clk ��ֵԽ����ĻԽ����ԽСԽ��������Ϊ1,0��ʾ�ر���
  * @retval ��
  * @author ZH
  * @date   2014��11��21��
  * @note   ��
  *************************************************************************************  
  */
void LcdSetBright(u8 Clk)
{
	u8 i;
	
	SystemDelay100us(5);
	if(Clk > LCD_BRIGHT_RANK)
	{
		Clk = LCD_BRIGHT_RANK;
	}	
	OSSchedLock();
	LCD_DIMM_PIN_L();
	for (i=0;i<Clk;i++)
	{
		LCD_DIMM_PIN_L();
		SystemDelay1us();
		LCD_DIMM_PIN_H();
		SystemDelay1us();
	}
	OSSchedUnlock();
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetCursor
*	����˵��: ����д�Դ�Ĺ��λ�ã�ͼ��ģʽ��
*	��    �Σ�_usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdSetWriteCursor(u16 x, u16 y)
{
	/* �����ڴ�д�������� ��ע��0x80-83 �ǹ��ͼ�ε����꡿ */
	LCD_CmdWrite(0x46);
    LCD_DataWrite(x);
	LCD_CmdWrite(0x47);
    LCD_DataWrite(x >> 8);
	LCD_CmdWrite(0x48);
    LCD_DataWrite(y);
	LCD_CmdWrite(0x49);
    LCD_DataWrite(y >> 8);	
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetCursor
*	����˵��: ���ö��Դ�Ĺ��λ�á� �ܶ������Ŀ�����д���Ͷ��������ͬ�ļĴ���������RA8875�ǲ�ͬ�ġ�
*	��    �Σ�_usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdSetReadCursor(u16 x, u16 y)
{
	/* �����ڴ���������� */
	LCD_CmdWrite(0x4A);
    LCD_DataWrite(x);
	LCD_CmdWrite(0x4B);
    LCD_DataWrite(x >> 8);
	LCD_CmdWrite(0x4C);
    LCD_DataWrite(y);
	LCD_CmdWrite(0x4D);
    LCD_DataWrite(y >> 8);
}
void LcdClear(u16 Color)
{
	LcdActiveWindow(0,LCD_WIDTH_PIXEL-1,0,LCD_HEIGHT_PIXEL-1);
	LcdSetBackgroundColor(Color);
	LcdMemoryClear();
}
/*
*********************************************************************************************************
*	�� �� ��: BTE_SetTarBlock
*	����˵��: ����RA8875 BTEĿ�������Լ�Ŀ��ͼ��
*	��    ��:
*			uint16_t _usX : ˮƽ�������
*			uint16_t _usY : ��ֱ�������
*			uint16_t _usHeight : ����߶�
*			uint16_t _usWidth : ������
*			uint8_t _ucLayer �� 0 ͼ��1�� 1 ͼ��2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdBTESetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer)
{
	/* ����������� */
	LCD_CmdWrite(0x58);
	LCD_DataWrite(_usX);
	LCD_CmdWrite(0x59);
	LCD_DataWrite(_usX >> 8);		

	LCD_CmdWrite(0x5A);
	LCD_DataWrite(_usY);	
	if (_ucLayer == 0)	/* ͼ��2 */
	{
		LCD_CmdWrite(0x5B);
		LCD_DataWrite(_usY >> 8);	
	}
	else
	{
		LCD_CmdWrite(0x5B);
		LCD_DataWrite((1 << 7) | (_usY >> 8));	/* Bit7 ��ʾͼ�㣬 0 ͼ��1�� 1 ͼ��2*/
	}

	/* ���������� */
	LCD_CmdWrite(0x5C);
	LCD_DataWrite(_usWidth);	
	LCD_CmdWrite(0x5D);
	LCD_DataWrite(_usWidth >> 8);	

	/* ��������߶� */
	LCD_CmdWrite(0x5E);
	LCD_DataWrite(_usHeight);	
	LCD_CmdWrite(0x5F);
	LCD_DataWrite(_usHeight >> 8);	
}

/*
*********************************************************************************************************
*	�� �� ��: BTE_SetOperateCode
*	����˵��: �趨BTE ������͹�դ������
*	��    ��: _ucOperate : ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdBTESetOperateCode(uint8_t _ucOperate)
{
	/*  �趨BTE ������͹�դ������  */
	LCD_CmdWrite(0x51);
	LCD_DataWrite(_ucOperate);
}
/*
*********************************************************************************************************
*	�� �� ��: BTE_Start
*	����˵��: ����BTE����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdBTEStart(void)
{
	LCD_CmdWrite(0x50);	/* ���üĴ�����ַ */
	LCD_DataWrite(0x80);	/* д��Ĵ���ֵ */    	
}
/*
*********************************************************************************************************
*	�� �� ��: BTE_Wait
*	����˵��: �ȴ�BTE��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdBTEWait(void)
{
 	u32 dly = 0;
	
	while(GET_LCD_STATE() == 0)
	{
		dly++;
		if(dly > 0xFFFFF)
		{
			break;
		}
	}	
//	do
//	{
//		dly--;
//		temp=LCD_StatusRead();
//	}while(((temp&0x40)==0x40) && (dly != 0));
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawLine
*	����˵��: ����RA8875��Ӳ�����ܣ���2��仭һ��ֱ�ߡ�
*	��    �Σ�
*			_usX1, _usY1 ����ʼ������
*			_usX2, _usY2 ����ֹ��Y����
*			_usColor     ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdDrawLine(u16 x1 , u16 y1 , u16 x2 , u16 y2 )
{
	/* pdf ��131ҳ
		RA8875 ֧��ֱ�߻�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ�ֱ�ߡ�����
		��ֱ�ߵ���ʼ��REG[91h~94h] �������REG[95h~98h]��ֱ�ߵ���ɫREG[63h~65h]��Ȼ����
		����ͼ�趨REG[90h] Bit4 = 0, Bit0=0 ��REG[90h] Bit7 = 1��RA8875 �ͻὫֱ�ߵ�ͼ��д��
		DDRAM����Ե���TFT ģ���ϾͿ�����ʾ������ֱ�ߡ�
	*/
	
	/* ����������� */
	LCD_CmdWrite(0x91);
    LCD_DataWrite(x1);
	LCD_CmdWrite(0x92);
    LCD_DataWrite(x1>>8);
	LCD_CmdWrite(0x93);
    LCD_DataWrite(y1);
	LCD_CmdWrite(0x94);
    LCD_DataWrite(y1>>8);	

	/* �����յ����� */
	LCD_CmdWrite(0x95);
    LCD_DataWrite(x2);
	LCD_CmdWrite(0x96);
    LCD_DataWrite(x2>>8);
	LCD_CmdWrite(0x97);
    LCD_DataWrite(y2);
	LCD_CmdWrite(0x98);
    LCD_DataWrite(y2>>8);	

	LCD_CmdWrite(0x90);/* ��ʼ��ֱ�� */
    LCD_DataWrite((1 << 7) | (0 << 4) | (0 << 0));				
	Chk_Busy();/* �ȴ����� */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_FillRect
*	����˵��: ����RA8875Ӳ���������һ������Ϊ��ɫ
*	��    �Σ�
*			_usX,_usY���������Ͻǵ�����
*			_usHeight �����εĸ߶�
*			_usWidth  �����εĿ��
*			_usColor  ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdDrawRect(u16 x1, u16 y1, u16 x2, u16 y2, u8 FillFlag)
{
	/*
		RA8875 ֧�ַ��λ�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ����Ρ�����
	�����ε���ʼ��REG[91h~94h]�������REG[95h~98h]�����ε���ɫREG[63h~65h]��Ȼ����
	����ͼ�趨REG[90h] Bit4=1, Bit0=0 ��REG[90h] Bit7 = 1��RA8875 �ͻὫ���ε�ͼ��д��
	DDRAM����Ե���TFT ģ���ϾͿ�����ʾ�����ķ��Ρ����趨REG[90h] Bit5 = 1����ɻ���һ
	ʵ�ķ��� (Fill)

	 ---------------->---
	|(x1��y1)        |
	V                    V  _usHeight
	|                    |
	 ---------------->--- (x2��y2)
		  _usWidth
	*/

	/* ����������� */
	LCD_CmdWrite(0x91);
    LCD_DataWrite(x1);
	LCD_CmdWrite(0x92);
    LCD_DataWrite(x1>>8);
	LCD_CmdWrite(0x93);
    LCD_DataWrite(y1);
	LCD_CmdWrite(0x94);
    LCD_DataWrite(y1>>8);	

	/* �����յ����� */
	LCD_CmdWrite(0x95);
    LCD_DataWrite(x2);
	LCD_CmdWrite(0x96);
    LCD_DataWrite(x2>>8);
	LCD_CmdWrite(0x97);
    LCD_DataWrite(y2);
	LCD_CmdWrite(0x98);
    LCD_DataWrite(y2>>8);

	LCD_CmdWrite(0x90); 
	if(0x00 != FillFlag)
	{
		LCD_DataWrite((1 << 7) | (1 << 5) | (1 << 4) | (0 << 0));// ��ʼ����������	
	}
	else
	{
		LCD_DataWrite((1 << 7) | (0 << 5) | (1 << 4) | (0 << 0));// ��ʼ���ƿ��ľ���
	}
	Chk_Busy();/* �ȴ����� */
}
void LcdDrawCircle(u16 x,u16 y, u8 r, u8 FillFlag)
{
    LCD_CmdWrite(0x99);
	LCD_DataWrite(x);  
    LCD_CmdWrite(0x9a);	   
	LCD_DataWrite(x>>8);  
	   
    LCD_CmdWrite(0x9b);
	LCD_DataWrite(y); 
    LCD_CmdWrite(0x9c);	   
	LCD_DataWrite(y>>8);
 
    LCD_CmdWrite(0x9d);
	LCD_DataWrite(r);

	LCD_CmdWrite(0x90);//DCR	
	if(0x00 == FillFlag)
	{
		LCD_DataWrite(0x40);		
	}
	else
	{
		LCD_DataWrite(0x60);	
	}
	Chk_Busy();/* �ȴ����� */
}
void LcdDrawTriangle(u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u8 FillFlag)
{	    
  
    LCD_CmdWrite(0x91);
	LCD_DataWrite(x1);  
    LCD_CmdWrite(0x92);	   
	LCD_DataWrite(x1>>8);
  
    LCD_CmdWrite(0x93);
	LCD_DataWrite(y1);  
    LCD_CmdWrite(0x94);	   
	LCD_DataWrite(y1>>8);
 
    LCD_CmdWrite(0x95);
	LCD_DataWrite(x2);  
    LCD_CmdWrite(0x96);	   
	LCD_DataWrite(x2>>8);
 
    LCD_CmdWrite(0x97);
	LCD_DataWrite(y2);  
    LCD_CmdWrite(0x98);	   
	LCD_DataWrite(y2>>8);

    LCD_CmdWrite(0xA9);
	LCD_DataWrite(x3);  
    LCD_CmdWrite(0xAA);	   
	LCD_DataWrite(x3>>8);
  
    LCD_CmdWrite(0xAB);
	LCD_DataWrite(y3);   
    LCD_CmdWrite(0xAC);	   
	LCD_DataWrite(y3>>8);  
	
	LCD_CmdWrite(0x90);//DCR
	if(0x00 == FillFlag)
	{
		LCD_DataWrite(0x81);	
	}
	else
	{
		LCD_DataWrite(0xA1);	
	}
	Chk_Busy();/* �ȴ����� */	
}

void LcdSetCGROMFont(void)
{
	u8 temp;
	
	LCD_CmdWrite(0x21);//FNCR0
	temp = LCD_DataRead();
	temp &= cClrD7 ;
	LCD_DataWrite(temp);
}

void LcdSetCGRAMFont(void)
{
	u8 temp;
	
	LCD_CmdWrite(0x21);//FNCR0
	temp = LCD_DataRead();
	temp |= cSetD7 ;
	LCD_DataWrite(temp);
}
void LcdSetInternalCGROM(void)
{
	u8 temp;
	LCD_CmdWrite(0x21);//FNCR0
	temp = LCD_DataRead();
	temp &= cClrD5 ;
	LCD_DataWrite(temp);
}
void LcdSetExternalCGROM(void)
{
	u8 temp;
	LCD_CmdWrite(0x21);//FNCR0
	temp = LCD_DataRead();
	temp |= cSetD5 ;
	LCD_DataWrite(temp);
}
void LcdSetFontSize(FONT_SIZE font) 
{
 	u8 temp;
	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x3f;
	if(F24x24And12x24 == font)
	{
		temp |= 0x40; 
	}
	else if (F32x32And16x32 == font)
	{
		temp |= 0x80; 
	}
	else
	{
		;
	}
	LCD_DataWrite(temp);
}

FONT_SIZE LcdGetFontSize(void)
{
 	u8 temp;
	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	if(temp&0x40)
	{
		return F24x24And12x24;
	}
	else if(temp&0x80)
	{
		return F32x32And16x32;
	}
	else
	{
		return F16x16And8x16;
	}
}
void LcdSetFontSpace(u8 setx) //bit[5:0]
{   
	u8 temp,temp1;
	
    temp1=setx&0x3F;
   	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xc0;
	temp |= temp1; 
	LCD_DataWrite(temp);

}

u8 LcdGetFontSpace(void) //bit[5:0]
{   
	u8 temp;
	
   	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	return (temp&0x3F);
}
void LcdSetGTSerialROMSelectGT21L16T1W(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x1f;
	LCD_DataWrite(temp);
}

void LcdSetGTSerialROMSelectGT23L16U2W(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x1f;
	temp |= 0x20;
	LCD_DataWrite(temp);
}

void LcdSetGTSerialROMSelectGT23L24T3Y(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x1f;
	temp |= 0x40;
	LCD_DataWrite(temp);
}

void LcdSetGTSerialROMSelectGT23L24M1Z(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x1f;
	temp |= 0x60;
	LCD_DataWrite(temp);
}

void LcdSetGTSerialROMSelectGT23L32S4W(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x1f;
	temp |= 0x80;
	LCD_DataWrite(temp);
}
void LcdSetFontCodeGB2312(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xE3;
	LCD_DataWrite(temp);
}
void LcdSetFontStandard(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xFC;
	LCD_DataWrite(temp);
}

void LcdSetFontArial(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xFC;
	temp |= 0x01;
	LCD_DataWrite(temp);
}

void LcdSetFontRoman(void)
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xFC;
	temp |= 0x02;
	LCD_DataWrite(temp);
}

void SetFontBold(void) //for ASCII
{ 
	u8 temp;
	LCD_CmdWrite(0x2F);//FNCR1
	temp = LCD_DataRead();
	temp &= 0xFC;
	temp |= 0x03;
	LCD_DataWrite(temp);
}
void LcdSetSerialRomSelect0(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=cClrD7;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomSelect1(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp |=cSetD7;
	LCD_DataWrite(temp);
}
void LcdSetSerialRomAddress24Bit(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=cClrD6;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomAddress32Bit(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp |=cSetD6;
	LCD_DataWrite(temp);
}

void LcdSetSelectSerialWaveformMode0(void)
{	
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=cClrD5;
	LCD_DataWrite(temp);
}

void LcdSetSelectSerialWaveformMode3(void)
{	
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp |=cSetD5;
	LCD_DataWrite(temp);
}
void LcdSetSerialROMReadCycle5bus(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=0xE7;
    temp |=cSetD3;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomReadCycle6bus(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=0xE7;
    temp |=cSetD4;
	LCD_DataWrite(temp);
}
void LcdSetSerialRomFontMode(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=cClrD2;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomDMAMode(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp |=cSetD2;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomSignalMode(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=0xFC;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomDualMode0(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=0xFC;
    temp |=cSetD1;
	LCD_DataWrite(temp);
}

void LcdSetSerialRomDualMode1(void)
{   
	u8 temp;
  	LCD_CmdWrite(0x05);//PWRR
	temp = LCD_DataRead();
	temp &=0xFC;
    temp |=cSetD1;
    temp |=cSetD0;
	LCD_DataWrite(temp);
}
void LcdSetSromClkDiv(u8 CLK_DIV)
{
	LCD_CmdWrite(0x06);
    LCD_DataWrite(CLK_DIV);
}
void LcdSetTextNoAlignment(void)
{
	u8 temp;
	
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD7 ;
	LCD_DataWrite(temp);
}
void LcdSetTextAlignment(void)
{
	u8 temp;
	
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp |= cSetD7 ;
	LCD_DataWrite(temp);
}
void LcdSetTextHaveBackgroundColor(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD6 ;
	LCD_DataWrite(temp);
}
void LcdSetTextNoBackgroundColor(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp |= cSetD6 ;
	LCD_DataWrite(temp);
}


void LcdSetTextNoRotate(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD4 ;
	LCD_DataWrite(temp);
}
void LcdSetTextRotate90(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp |= cSetD4 ;
	LCD_DataWrite(temp);
}


void LcdSetTextHorizontalEnlargeX1(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD3D2 ;
	LCD_DataWrite(temp);
}
void LcdSetTextHorizontalEnlargeX2(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD3D2 ;
	temp |= cClrD3SetD2 ;
	LCD_DataWrite(temp);
}
void LcdSetTextHorizontalEnlargeX3(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD3D2 ;
	temp |= cSetD3ClrD2 ;
	LCD_DataWrite(temp);
}
void LcdSetTextHorizontalEnlargeX4(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD3D2 ;
	temp |= cSetD3D2 ;
	LCD_DataWrite(temp);
}
void LcdSetTextVerticalEnlargeX1(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD1D0 ;
	LCD_DataWrite(temp);
}
void LcdSetTextVerticalEnlargeX2(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD1D0 ;
	temp |= cClrD1SetD0 ;
	LCD_DataWrite(temp);
}
void LcdSetTextVerticalEnlargeX3(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD1D0 ;
	temp |= cSetD1ClrD0 ;
	LCD_DataWrite(temp);
}
void LcdSetTextVerticalEnlargeX4(void)
{
	u8 temp;
	LCD_CmdWrite(0x22);//FNCR1
	temp = LCD_DataRead();
	temp &= cClrD1D0 ;
	temp |= cSetD1D0 ;
	LCD_DataWrite(temp);
}
void LcdSetGraphicMode(void)
{	
	u8 temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp &= cClrD7 ;
	LCD_DataWrite(temp);
}

void LcdSetTextMode(void)
{	
	u8 temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp |= cSetD7 ;
	LCD_DataWrite(temp);	 
}
void LcdSetTextWriteCursor(u16 x,u16 y)
{
 
    LCD_CmdWrite(0x2A);//HOFS0
	LCD_DataWrite(x);

    LCD_CmdWrite(0x2B);//HOFS1	   
	LCD_DataWrite(x>>8);

 
    LCD_CmdWrite(0x2C);//VOFS0
	LCD_DataWrite(y);

    LCD_CmdWrite(0x2D);//VOFS1	   
	LCD_DataWrite(y>>8); 
}
void LcdSetFontRom(void)
{
	LcdSetCGROMFont();
    LcdSetExternalCGROM();
    LcdSetFontSize(F24x24And12x24);
	LcdSetGTSerialROMSelectGT23L32S4W();
    //LcdSetFontCodeGB2312();
    LcdSetFontRoman();
    LcdSetSerialRomSelect0();
	LcdSetSerialRomAddress24Bit();
    LcdSetSelectSerialWaveformMode0();
    LcdSetSerialROMReadCycle5bus();
	LcdSetSerialRomFontMode();
    LcdSetSerialRomSignalMode();
    LcdSetSromClkDiv(3);	//clock
	LcdSetTextHaveBackgroundColor();
	//LcdSetTextNoBackgroundColor();
	LcdSetBackgroundColor(0xFFFF);
	LcdSetFrontColor(BLACK);
	
}
/*
*********************************************************************************************************
*	�� �� ��: LcdTouchInit
*	����˵��: ��ʼ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LcdTouchInit(void)
{
    LCD_CmdWrite(0xF0);//������������ж�λ
	LCD_DataWrite(1 << 2);

	/*************************************************************
		REG[71h] Touch Panel Control Register 1 (TPCR1)

		Bit7 N/A
		Bit6 �������ģʽ�趨
			0 : �Զ�ģʽ��
			1 : �ֶ�ģʽ��
		Bit5 ������������ADC �ο���ѹ(Vref)��Դ�趨
			0 : �ڲ������ο���ѹ��
			1 : �ⲿ����ο���ѹ��ADC �ο���ѹ׼λ= 1/2 VDD��
		Bit4-3 N/A
		Bit2 �����ж�Ѷ�ŵ�����������·ѡ��
			0: �ر�����������·��
			1: ��������������·��
		Bit1-0 ��������ֶ�ģʽ֮ѡ��λ
			00b : ����ģʽ�����ؿ��Ƶ�Ԫ��������ģʽ��
			01b : ��ⴥ���¼��������ڴ�ģʽ����������ⴥ���¼��ķ�
				�����¼��������������жϻ����ɻ�������֪(REG[F1h]
				Bit2)��
			10b : X ������ߢȡģʽ���ڴ�ģʽ����λ�õ�X �����ݻᱻ��
				���� REG[72h] ��REG[74h]��
			11b : Y ������ߢȡģʽ���ڴ�ģʽ����λ�õ�Y �����ݻᱻ��
				����REG[73h] and REG[74h]��
	************************************************************************/
	LCD_CmdWrite(0x71);
	LCD_DataWrite((0 << 6) | (0 << 5) | (1 << 2));//ѡ���Զ�ģʽ

	/*************************************************************
		REG[70h] Touch Panel Control Register 0 (TPCR0)

		Bit7 ������幦���趨
			0 : �رա�
			1 : ������
		Bit6-4 ������������ȡ��ʱ���趨
			000b : ADC ȡ��ʱ��Ϊ512 ��ϵͳƵ�����ڡ�
			001b : ADC ȡ��ʱ��Ϊ 1024 ��ϵͳƵ�����ڡ�
			010b : ADC ȡ��ʱ��Ϊ 2048 ��ϵͳƵ�����ڡ�
			011b : ADC ȡ��ʱ��Ϊ 4096 ��ϵͳƵ�����ڡ�
			100b : ADC ȡ��ʱ��Ϊ 8192 ��ϵͳƵ�����ڡ�
			101b : ADC ȡ��ʱ��Ϊ 16384 ��ϵͳƵ�����ڡ�
			110b : ADC ȡ��ʱ��Ϊ 32768 ��ϵͳƵ�����ڡ�
			111b : ADC ȡ��ʱ��Ϊ65536 ��ϵͳƵ�����ڡ�
		Bit3 ������廽��ģʽ
			0 : �رմ����¼�����ģʽ��
			1 : �����¼��ɻ���˯��ģʽ��
		Bit2-0 ������������ADC Ƶ���趨
			000b : ϵͳƵ�ʡ�
			001b : ϵͳƵ�� / 2��
			010b : ϵͳƵ�� / 4��
			011b : ϵͳƵ�� / 8��
			100b : ϵͳƵ�� / 16��
			101b : ϵͳƵ�� / 32��
			110b : ϵͳƵ�� / 64��
			111b : ϵͳƵ�� / 128��

		ע : ADC ������Ƶ���趨���ܳ���10MHz��
	************************************************************************/
	LCD_CmdWrite(0x70);
	LCD_DataWrite((1 << 7) | (2 << 4) | (0 << 3) | (4 << 0));//������������
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_TouchReadX
*	����˵��: ��ȡXλ��ADC   �������������ʱ�жϷ��������ã���Ҫ����Ĵ������ʳ�ͻ )
*	��    ��: ��
*	�� �� ֵ: ADCֵ��10Bit
*********************************************************************************************************
*/
void LcdTouchReadXY(u16* TouchX, u16* TouchY)
{
	uint16_t usAdc;
	uint8_t ucRegValue;
	uint8_t ucReg74;
	uint16_t s_usTouchX, s_usTouchY;
	
	/* �����ȡ�ж��¼���־ */
	LCD_CmdWrite(0xF1);
	LCD_DataRead();
	LCD_DataRead();
	ucRegValue = LCD_DataRead();
	if (ucRegValue & (1 << 2))
	{
		LCD_CmdWrite(0x74);
		ucReg74 = LCD_DataRead();
		LCD_CmdWrite(0x72);
		usAdc = LCD_DataRead();	/* Bit9-2 */
		usAdc <<= 2;
		usAdc += (ucReg74 & 0x03);

		s_usTouchX = usAdc;
		LCD_CmdWrite(0x73);
		usAdc = LCD_DataRead();	/* Bit9-2 */
		usAdc <<= 2;
		usAdc += ((ucReg74 & 0x0C) >> 2);

		s_usTouchY = usAdc;
	}
	else
	{
		s_usTouchX = 0;
		s_usTouchY = 0;
	}

	/*************************************************************
		bit2 д�빦��?? ��������ж����λ
		0 : δ������
		1 : �����������жϡ�
		��ȡ����?? ��������ж�״̬
		0 : δ������������жϡ�
		1 : ������������жϡ�
	**************************************************************/
	LCD_CmdWrite(0xF1);
	LCD_DataWrite(1 << 2);
	
	
	*TouchX = s_usTouchX;
	*TouchY = s_usTouchY;
}
void LcdRA8875Init(void)
{	
	SystemDelay100us(200);
	
	LcdReset();

    // 640x480	  SYSCLK=25*(5+1)/2^1
    LCD_CmdWrite(0x88);    	
    LCD_DataWrite(0x05);
    SystemDelay100us(2); //Ҫ����100us 

    LCD_CmdWrite(0x89);    
    LCD_DataWrite(0x01);     
    SystemDelay100us(2); //Ҫ����100us
	
    LCD_CmdWrite(0x10);  //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8int MPU interface
    //LCD_DataWrite(0x0c);   // if 8int MCU interface   and 65k color display  						    
    LCD_DataWrite(0x0f); // if 16int MCU interface   and 65k color display  

#if (LCD_WIDTH_PIXEL==240) || (LCD_HEIGHT_PIXEL==240)
//==============	Display Window	320x240 ==================
    LCD_CmdWrite(0x04);     //set PCLK invers
    LCD_DataWrite(0x03);  
    
     //Horizontal set
    LCD_CmdWrite(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0] 
    LCD_DataWrite(0x27);//Horizontal display width(pixels) = (HDWR + 1)*8  
    LCD_CmdWrite(0x15); //Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
    LCD_DataWrite(0x03);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]  
    LCD_CmdWrite(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0] 
    LCD_DataWrite(0x03);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8   
    LCD_CmdWrite(0x17); //HSTR//HSYNC Start Position[4:0] 
    LCD_DataWrite(0x02);//HSYNC Start Position(PCLK) = (HSTR + 1)*8 
    LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC. 
    LCD_DataWrite(0x03);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8 
     //Vertical set 
    LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height Bit [7:0] 
    LCD_DataWrite(0xef);//Vertical pixels = VDHR + 1

    LCD_CmdWrite(0x1A); //VDHR1 //Vertical Display Height Bit [8] 
    LCD_DataWrite(0x00);//Vertical pixels = VDHR + 1 

    LCD_CmdWrite(0x1B); //VNDR0 //Vertical Non-Display Period Bit [7:0]
    LCD_DataWrite(0x0A);//Vertical Non-Display area = (VNDR + 1) 

    LCD_CmdWrite(0x1C); //VNDR1 //Vertical Non-Display Period Bit [8] 
    LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)   
    LCD_CmdWrite(0x1D); //VSTR0 //VSYNC Start Position[7:0]
    LCD_DataWrite(0x0e);//VSYNC Start Position(PCLK) = (VSTR + 1) 
    LCD_CmdWrite(0x1E); //VSTR1 //VSYNC Start Position[8] 
    LCD_DataWrite(0x06);//VSYNC Start Position(PCLK) = (VSTR + 1) 
    LCD_CmdWrite(0x1F); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]
    LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1) 

  //Active window  set 
  //setting active window X
    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)	   
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    LCD_DataWrite(0x3F); //Horizontal End Point of Active Window [7:0]
    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)	   
    LCD_DataWrite(0x01); //Horizontal End Point of Active Window [9:8]

   //setting active window Y
    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)	   
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    LCD_DataWrite(0xef); //Vertical End Point of Active Window [7:0]
    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)	   
    LCD_DataWrite(0x00); //Vertical End Point of Active Window [8]

//    LCD_CmdWrite(0x04);     //set PCLK invers
//    LCD_DataWrite(0x03);  
//    
//     //Horizontal set
//    LCD_CmdWrite(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0] 
//    LCD_DataWrite(0x27);//Horizontal display width(pixels) = (HDWR + 1)*8  
//    LCD_CmdWrite(0x15); //Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
//    LCD_DataWrite(0x00);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]  
//    LCD_CmdWrite(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0] 
//    LCD_DataWrite(0x03);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8   
//    LCD_CmdWrite(0x17); //HSTR//HSYNC Start Position[4:0] 
//    LCD_DataWrite(0x02);//HSYNC Start Position(PCLK) = (HSTR + 1)*8 
//    LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC. 
//    LCD_DataWrite(0x03);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8 
//     //Vertical set 
//    LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height Bit [7:0] 
//    LCD_DataWrite(0xef);//Vertical pixels = VDHR + 1

//    LCD_CmdWrite(0x1A); //VDHR1 //Vertical Display Height Bit [8] 
//    LCD_DataWrite(0x00);//Vertical pixels = VDHR + 1 

//    LCD_CmdWrite(0x1B); //VNDR0 //Vertical Non-Display Period Bit [7:0]
//    LCD_DataWrite(0x0f);//Vertical Non-Display area = (VNDR + 1) 

//    LCD_CmdWrite(0x1C); //VNDR1 //Vertical Non-Display Period Bit [8] 
//    LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)   
//    LCD_CmdWrite(0x1D); //VSTR0 //VSYNC Start Position[7:0]
//    LCD_DataWrite(0x0e);//VSYNC Start Position(PCLK) = (VSTR + 1) 
//    LCD_CmdWrite(0x1E); //VSTR1 //VSYNC Start Position[8] 
//    LCD_DataWrite(0x06);//VSYNC Start Position(PCLK) = (VSTR + 1) 
//    LCD_CmdWrite(0x1F); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]
//    LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1) 

//  //Active window  set 
//  //setting active window X
//    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
//    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
//    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)	   
//    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
//    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
//    LCD_DataWrite(0x3F); //Horizontal End Point of Active Window [7:0]
//    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)	   
//    LCD_DataWrite(0x01); //Horizontal End Point of Active Window [9:8]

//   //setting active window Y
//    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
//    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
//    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)	   
//    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
//    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
//    LCD_DataWrite(0xef); //Vertical End Point of Active Window [7:0]
//    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)	   
//    LCD_DataWrite(0x00); //Vertical End Point of Active Window [8]
//==============	Display Window	640x480 ==================
#else 
    LCD_CmdWrite(0x04);    //PCLK =SYSCLK/2
    LCD_DataWrite(0x01);
    SystemDelay100us(100);
    
    //Horizontal set
    LCD_CmdWrite(0x14);//HDWR//Horizontal Display Width Setting Bit[6:0]                   
    LCD_DataWrite(0x4F);//Horizontal display width(pixels) = (HDWR + 1)*8                   
    LCD_CmdWrite(0x15); //Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
    LCD_DataWrite(0x05);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]            
    LCD_CmdWrite(0x16);//HNDR//Horizontal Non-Display Period Bit[4:0]                      
    LCD_DataWrite(0x0f);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8             
    LCD_CmdWrite(0x17);//HSTR//HSYNC Start Position[4:0]                                   
    LCD_DataWrite(0x01);//HSYNC Start Position(PCLK) = (HSTR + 1)*8                         
    LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC.                  
    LCD_DataWrite(0x00);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8  
    //Vertical set       
    LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height int [7:0]                         
    LCD_DataWrite(0xdf);//Vertical pixels = VDHR + 1                                        
    LCD_CmdWrite(0x1A);//VDHR1 //Vertical Display Height int [8]                           
    LCD_DataWrite(0x01);//Vertical pixels = VDHR + 1                                        
    LCD_CmdWrite(0x1B);//VNDR0 //Vertical Non-Display Period int [7:0]                     
    LCD_DataWrite(0x0A);//Vertical Non-Display area = (VNDR + 1)                            
    LCD_CmdWrite(0x1C);//VNDR1 //Vertical Non-Display Period int [8]                       
    LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)                            
    LCD_CmdWrite(0x1D);//VSTR0 //VSYNC Start Position[7:0]                                 
    LCD_DataWrite(0x0E);//VSYNC Start Position(PCLK) = (VSTR + 1)                           
    LCD_CmdWrite(0x1E);//VSTR1 //VSYNC Start Position[8]                                   
    LCD_DataWrite(0x00);//VSYNC Start Position(PCLK) = (VSTR + 1)                           
    LCD_CmdWrite(0x1F);//VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]                     
    LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1)                              
   
   //Active window  set 
   //setting active window X
    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)	   
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    LCD_DataWrite(0x7f); //Horizontal End Point of Active Window [7:0]
    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)	   
    LCD_DataWrite(0x02); //Horizontal End Point of Active Window [9:8]

   //setting active window Y
    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)	   
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    LCD_DataWrite(0xdf); //Vertical End Point of Active Window [7:0]
    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)	   
    LCD_DataWrite(0x01); //Vertical End Point of Active Window [8]
#endif	
	LcdClear(0x0000);//�Ժ�ɫ����

	LcdDisplayOn();

	//backlight on 
	LcdSetBright(0);
//	LcdSetFrontColor(0xf800);


	LcdSetFontRom();
	
	LcdClear(0x0000);//
		
	
//	LcdTouchInit();//��ʼ���������
}

u16 LcdGetPoint(void)
{
	LCD_CmdWrite(0x02);
	return LCD_DataRead();
}
void LcdSetPoint(u16 color)
{
	LCD_CmdWrite(0x02);
	LCD_DataWrite(color);
}
u16 LcdGetPixel(u16 x, u16 y)
{
	u16 Value;
	
	while(1)
	{
//		Chk_Busy();//���Է���FSMC����ʱ��Ҫ����ʱ
		LCD_CmdWrite(0x40);
		LCD_DataWrite(1);	/* ����Ϊ��ͼģʽ����ȡ��겻�Զ���1 */
		//SystemDelay1us();//���Է���FSMC����ʱ��Ҫ����ʱ
		LcdSetReadCursor(x, y);	/* ���ö�ȡ���λ�� */
		//SystemDelay1us();//���Է���FSMC����ʱ��Ҫ����ʱ
		LCD_CmdWrite(0x02);
		//SystemDelay1us();//���Է���FSMC����ʱ��Ҫ����ʱ
		LCD_DataRead();
		LCD_DataRead();
		Value = LCD_DataRead();
		if(Value == LCD_DataRead())
		{
			return Value;
		}
	}
}
void LcdSetPixel(u16 x, u16 y, u16 color)
{
	LcdSetWriteCursor(x, y);	/* ���ù��λ�� */

	LCD_CmdWrite(0x02); 		/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */
	LCD_DataWrite(color);
}

void LcdShowString(char* str)
{
	LcdSetTextMode();
	
	LCD_CmdWrite(0x02);
	while(*str != '\0')
	{
		LCD_DataWrite(*str);
		++str;	  	
		Chk_Busy();
	}
}
