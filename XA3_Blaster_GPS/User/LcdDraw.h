#ifndef _LCDDRAW_H_
#define _LCDDRAW_H_


typedef enum
{
	FONT_16,
	FONT_24
}LCDFONT;

typedef enum
{
	NORMAL_MODE,
	REVERSE_MODE
}LCDREVERSE;

typedef struct 
{
	u8 CWidth;				//字符的宽度
	u8 CHigh;				//字符的高度
	u8 Clenth;				//字符占的字节数
	u8 HWidth;				//汉字的宽度
	u8 HHigh;				//汉字的高度
	u8 Hlenth;				//汉字占的字节数
}LCD_FONT;

typedef struct 
{
	u16 FrontColor;				//前景色
	u16 BackColor;				//背景色
	LCDFONT Font;				//字体
	LCDREVERSE BackCoState;		//是否更新背景色
}LCD_STATE;
extern LCD_FONT LcdFont;
extern LCD_STATE LcdState;

typedef enum
{
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER
}ALIGN_TYPE;

#define LCDCBRIGHT_16		1
#define LCDCBRIGHT_15		3
#define LCDCBRIGHT_14		5
#define LCDCBRIGHT_13		7
#define LCDCBRIGHT_12		9
#define LCDCBRIGHT_11		11
#define LCDCBRIGHT_10		13
#define LCDCBRIGHT_9		15

#define LCDCBRIGHT_8		17
#define LCDCBRIGHT_7		19
#define LCDCBRIGHT_6		21
#define LCDCBRIGHT_5		23
#define LCDCBRIGHT_4		25
#define LCDCBRIGHT_3		27
#define LCDCBRIGHT_2		29
#define LCDCBRIGHT_1		31
#define LCDCBRIGHT_0		32

#define COLOR_BLACK			0x0000			//黑色		0x000000
#define COLOR_WHITE			0xFFFF			//白色		0xFFFFFF
#define COLOR_YALLOW		0xFFE0			//黄色		0xFFFF00
#define COLOR_BLUE			0x001F			//蓝色		0x0000FF
#define COLOR_DARKGREEN		0X06E0			//深绿色	0x00DC00	可用于电池显示
#define COLOR_CYAN			0x07FF			//青色		0x00FFFF
#define COLOR_DARKCYAN		0x0FEA
#define COLOR_GREEN			0x07E0			//绿色		0x00FF00
#define COLOR_RED			0xF800			//红色		0xFF0000
#define COLOR_LIRED			0xF808

#define COLOR_BLUEXUAN		0x001F
#define COLOR_BLUEWU		0x711F
#define COLOR_BLUEAN		0x000F
#define COLOR_BROWN			0x8145			//棕色		0x802A2A

#define COLOR_GRAY_1		0xD699//0x8FF9			//浅灰色	0xDCDCDC	对应RGB：142	252		202
#define COLOR_GRAY_2		0x52AA			//深灰色	0x545555	对应RGB：84		85		85
#define COLOR_BLUE_2		0x003F			//蓝色		
#define COLOR_BLUE_1		0x27FE			//蓝色		0x2B9FDB	对应RGB：35		255		240
#define COLOR_GREEN_1		0x8FF9
#define COLOR_GREEN_2		0x07E7			//绿色		0x63B72F	对应RGB：0		253		60
#define COLOR_YALLOW_1		0xFF82			//黄色		0xF7AB00	对应RGB：251	240		20
#define COLOR_RED_1			0xF800			//红色		0xE60012	对应RGB：255	0		0

#define DEFAULT_BK_COLOR    COLOR_WHITE
#define DEFAULT_FONT_COLOR  COLOR_BLACK



#define FONT16_CWIDTH		8				//8*16字符宽度
#define FONT16_CHIGH		16				//8*16字符高度
#define FONT16_CLEN			16				//8*16字符占的字节数

#define FONT16_HWIDTH		16				//16*16汉字宽度
#define FONT16_HHIGH		16				//16*16汉字高度
#define FONT16_HLEN			32				//16*16汉字占的字节数

#define FONT24_CWIDTH		12				//12*24字符宽度
#define FONT24_CHIGH		24				//12*24字符高度
#define FONT24_CLEN			48				//12*24字符占的字节数

#define FONT24_HWIDTH		24				//24*24汉字宽度
#define FONT24_HHIGH		24				//24*24汉字高度
#define FONT24_HLEN			72				//24*24汉字占的字节数




#define BOOTH_FUHAO			0
#define USB_FUHAO			1









extern void LcdSetFontState(LCDREVERSE state);
extern LCDREVERSE LcdGetFontState(void);
extern void LcdSetFrontBackColor(u16 Front, u16 Back);
extern u16 LcdGetFrontColor(void);
extern u16 LcdGetBackColor(void);
extern void LcdSetFont(LCDFONT state);
extern LCDFONT LcdGetFont(void);
extern u8 LcdDrawLine_H(u16 xSAddr, u16 xEAddr, u16 yAddr, u16 Color);
extern u8 LcdDrawLine_S(u16 xAddr, u16 ySAddr, u16 yEAddr, u16 Color);
extern u8 LcdDrawRectangle(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 Color);
extern u8 LcdDrawFillRect(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u16 Color);
extern u8 * LcdCheckCharHz(char *pdata, u8 CharHz);
extern void LcdDrawOne(u16 xAddr, u16 yAddr, char *CharName, u8 CharHz);
extern void LcdDrawString(u16 xAddr, u16 yAddr, char* StrBuf);
extern void ShowLcdHorizontaAlignlString(u16 y, ALIGN_TYPE align, char* Str);
extern u8 LcdDrawPicture(u16 xSAddr, u16 ySAddr, u16 xEAddr, u16 yEAddr, u8 *ShowBuf);
extern void LcdShowImage1(void);
extern void LcdShowCloseImage(void);
extern void LcdShowIronImage(u16 x, u16 y, u16 xlen, u16 ylen, u8 state);



















#endif
