#ifndef _RA8875_H_
#define _RA8875_H_

#define SPI_WRITE_DATA	0x00
#define SPI_READ_DATA	0x40
#define SPI_WRITE_CMD	0x80
#define SPI_READ_STATUS	0xC0

#define	cSetD0		     0x01
#define	cSetD1		     0x02
#define	cSetD2		     0x04
#define	cSetD3		     0x08
#define	cSetD4		     0x10
#define	cSetD5		     0x20
#define	cSetD6		     0x40
#define	cSetD7		     0x80

#define	cClrD0		     0xfe
#define	cClrD1		     0xfd
#define	cClrD2		     0xfb
#define	cClrD3		     0xf7
#define	cClrD4		     0xef
#define	cClrD5		     0xdf
#define	cClrD6		     0xbf
#define	cClrD7		     0x7f

#define cClrD1D0         0xfc
#define cClrD1SetD0	     0x01
#define cSetD1ClrD0	     0x02
#define cSetD1D0		 0x03
#define cClrD3D2         0xf3
#define cClrD3SetD2      0x04
#define cSetD3ClrD2      0x08
#define cSetD3D2		 0x0c

//5.6寸TFT显示屏，分辨率640X480,16级灰度
#define LCD_WIDTH_PIXEL      320
#define LCD_HEIGHT_PIXEL     240

typedef enum
{
	F16x16And8x16,
	F24x24And12x24,
	F32x32And16x32
}FONT_SIZE;


#define   RED      0xF800//红色
#define   GREEN    0x07E0//绿色
#define   BLUE0    0xB65D//淡蓝色
#define   BLUE     0x001F//蓝色
#define   GRAY0    0xD699//灰色0
#define   GRAY1    0x8410//灰色1
#define   GRAY2    0x4208//灰色2
#define   WHITE    0xFFFF//白色
#define   BLACK    0x0000//黑色   
#define   YELLOW   0xFFE0//黄色

#define   LCD_BRIGHT_RANK     32 //数值越大，背光越暗，但超过32就已经分辨不出来了

extern void LcdGPIOConfig(void);
extern void LcdRA8875Init(void);
extern void LcdSetBackgroundColor(u16 color);
extern void LcdSetTextHaveBackgroundColor(void);
extern void LcdSetTextNoBackgroundColor(void);
extern void LcdSetBright(u8 Clk);
extern void LcdMemoryClear(void);
extern u16 LcdGetPixel(u16 x, u16 y);
extern void LcdSetPixel(u16 x, u16 y, u16 color);
extern void LcdSetWriteCursor(u16 x, u16 y);
extern void LcdSetReadCursor(u16 x, u16 y);
extern u16 LcdGetPoint(void);
extern void LcdSetPoint(u16 color);
extern void LcdDrawLine(u16 x1 , u16 y1 , u16 x2 , u16 y2);
extern void LcdDrawRect(u16 x1, u16 y1, u16 x2, u16 y2, u8 FillFlag);
extern void LcdDrawCircle(u16 x,u16 y, u8 r, u8 FillFlag);
extern void LcdDrawTriangle(u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u8 FillFlag);
extern void LcdSetTextWriteCursor(u16 x,u16 y);
extern void LcdSetBackgroundColor(u16 color);
extern void LcdSetFrontColor(u16 color);
extern void LcdClear(u16 Color);
extern void LcdSetFontSize(FONT_SIZE font) ;
extern FONT_SIZE LcdGetFontSize(void);
extern u8 LcdGetFontSpace(void);
extern void LcdShowString(char* str);

extern void LcdBTESetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer);
extern void LcdBTESetOperateCode(uint8_t _ucOperate);
extern void LcdBTEStart(void);
extern void LcdBTEWait(void);

extern void LcdTouchReadXY(u16* TouchX, u16* TouchY);
#endif
