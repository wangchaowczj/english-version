#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define ICON_STARTX			20			//显示图标的起始X坐标
#define ICON_STARTY			35			//显示图标的起始Y坐标
#define ICON_LENX			40			//显示图标的宽度
#define ICON_LENY			40			//显示图标的高度
#define ICON_BLANKX			42			//横向每个图标之间的宽度
#define ICON_BLANKY			45			//竖向每个图标之间的宽度

#define ICON_ZIFULEN		3			//字符顶端距离图标低端的长度



#define ICON_1X				(ICON_STARTX+(ICON_LENX/2)-6)		//1号图标字符的位置
#define ICON_1Y				(ICON_STARTY+(ICON_LENY/2)-12)
#define ICON_2X				(ICON_1X+ICON_LENX+ICON_BLANKX)		//2号图标字符的位置
#define ICON_2Y				ICON_1Y
#define ICON_3X				(ICON_2X+ICON_LENX+ICON_BLANKX)		//3号图标字符的位置
#define ICON_3Y				ICON_1Y
#define ICON_4X				ICON_1X								//4号图标字符的位置
#define ICON_4Y				(ICON_1Y+ICON_LENY+ICON_BLANKY)
#define ICON_5X				ICON_2X								//5号图标字符的位置
#define ICON_5Y				ICON_4Y
#define ICON_6X				ICON_3X								//6号图标字符的位置
#define ICON_6Y				ICON_4Y
#define ICON_7X				ICON_1X								//7号图标字符的位置
#define ICON_7Y				(ICON_4Y+ICON_LENY+ICON_BLANKY)
#define ICON_8X				ICON_2X								//8号图标字符的位置
#define ICON_8Y				ICON_7Y
#define ICON_9X				ICON_3X								//9号图标字符的位置
#define ICON_9Y				ICON_7Y

#define ICON_TU1X			ICON_STARTX							//1号图标的起始位置
#define ICON_TU1Y			ICON_STARTY
#define ICON_TU2X			(ICON_TU1X+ICON_LENX+ICON_BLANKX)	//2号图标的起始位置
#define ICON_TU2Y			ICON_TU1Y
#define ICON_TU3X			(ICON_TU2X+ICON_LENX+ICON_BLANKX)	//3号图标的起始位置
#define ICON_TU3Y			ICON_TU1Y
#define ICON_TU4X			ICON_TU1X							//4号图标的起始位置
#define ICON_TU4Y			(ICON_TU1Y+ICON_LENY+ICON_BLANKY)
#define ICON_TU5X			ICON_TU2X							//5号图标的起始位置
#define ICON_TU5Y			ICON_TU4Y
#define ICON_TU6X			ICON_TU3X							//6号图标的起始位置
#define ICON_TU6Y			ICON_TU4Y
#define ICON_TU7X			ICON_TU1X							//7号图标的起始位置
#define ICON_TU7Y			(ICON_TU4Y+ICON_LENY+ICON_BLANKY)
#define ICON_TU8X			ICON_TU2X							//8号图标的起始位置
#define ICON_TU8Y			ICON_TU7Y
#define ICON_TU9X			ICON_TU3X							//9号图标的起始位置
#define ICON_TU9Y			ICON_TU7Y

#define ICON_Z123Y			ICON_TU1Y + ICON_LENY + ICON_ZIFULEN+2		//图标对应的字符的纵坐标
#define ICON_Z456Y			ICON_TU4Y + ICON_LENY + ICON_ZIFULEN+2
#define ICON_Z789Y			ICON_TU7Y + ICON_LENY + ICON_ZIFULEN+2


//****左右功能键相关的参数****//
#define BUTTON_SMALL_LEFT_X			5									//LEFT按键的起始位置
#define BUTTON_SMALL_Y			    286
#define BUTTON_SMALL_WIDTH          60   //50
#define BUTTON_SMALL_HEIGHT         32
#define BUTTON_SMALL_RIGHT_X			(LCDP_MAX_X-BUTTON_SMALL_WIDTH-BUTTON_SMALL_LEFT_X)			//RIGHT按键的起始位置

#define BUTTON_BIG_LEFT_X			5									//LEFT按键的起始位置
#define BUTTON_BIG_Y			    286
#define BUTTON_BIG_WIDTH            82
#define BUTTON_BIG_HEIGHT           32
#define BUTTON_BIG_RIGHT_X			(LCDP_MAX_X-BUTTON_BIG_WIDTH - BUTTON_BIG_LEFT_X)			//RIGHT按键的起始位置


#define BOTTOM_STATUS_X         (BUTTON_SMALL_LEFT_X + BUTTON_SMALL_WIDTH + 5)
#define BOTTOM_STATUS_WIDTH     (BUTTON_SMALL_RIGHT_X -5 - BOTTOM_STATUS_X)

//顶部标题栏
#define TOP_TITLE_X         0
#define TOP_TITLE_Y         0
#define TOP_TITLE_WIDTH     240
#define TOP_TITLE_HEIGHT    20

//信息区域
#define INFO_TITLE_LINE_X			15		//信息列表显示区域的横线坐标
#define INFO_TITLE_LINE_Y           35
#define INFO_TITLE_LINE_WIDTH		210		//信息列表显示区域的横线长度
#define INFO_TITLE_COLOR		    GUI_BLUE	//信息列表显示区域的横线颜色
#define INFO_TITLE_Y                5

//帮助区域
#define INFO_HELP_LINE_X            15
#define INFO_HELP_LINE_Y            220
#define INFO_HELP_LINE_WIDTH		210		//信息列表帮助区域的横线长度
#define INFO_HELP_COLOR		        GUI_BLUE	//信息列表帮助区域的横线颜色
#define INFO_HELP_Y                 233
#define INFO_HELP_Y1                226
#define INFO_HELP_Y2                246

//****和提示界面相关的参数****//
#define TISHIX				40									//提示界面的起始位置
#define TISHIY				30
#define TISHILENX			164									//提示界面的长宽
#define TISHILENY			245
#define TISHILINEX			(TISHIX+20)							//提示界面的横线起始位置
#define TISHILINEY			(TISHIY+16+17)
#define TISHILINELEN		(TISHILENX-40)						//提示界面的横线长度
#define TISHIF14X			TISHIX								//提示界面的F1到F4按键显示块的起始位置
#define TISHIF14Y			(TISHIY+TISHILENY-25)

#define TISHIBIAOTI			((TISHILINEY-TISHIY-16)/2+TISHIY)	//提示界面标题的起始纵坐标

#define TISHIANJIAN			TISHIF14Y+((TISHIY+TISHILENY-TISHIF14Y-16)/2)	//提示界面按键的纵坐标

//提示栏显示区域可以显示11行数据，每一行的位置如下
#define TLINE1			(TISHILINEY+5)
#define TLINE2			TLINE1+16
#define TLINE3			TLINE2+16
#define TLINE4			TLINE3+16
#define TLINE5			TLINE4+16
#define TLINE6			TLINE5+16
#define TLINE7			TLINE6+16
#define TLINE8			TLINE7+16
#define TLINE9			TLINE8+16
#define TLINE10			TLINE9+16
#define TLINE11			TLINE10+16

#define TISHIPROLENX	120										//提示栏中的进度条长宽
#define TISHIPROLENY	14
#define TISHIPROX		TISHIX+((TISHILENX-TISHIPROLENX)/2)		//提示栏中的进度条横坐标

//****信息列表界面相关参数****//
#define CDY1			22										//信息列表显示区域的起始y轴到结束y轴坐标
#define CDY2			282

#define CDHENGX			18										//信息列表显示区域的横线坐标
#define CDHENGY1		50
#define CDHENGY2		245
#define CDHENGLEN		209										//信息列表显示区域的横线长度

#define CDBIAOTIY		(CDY1+(CDHENGY1-CDY1-24)/2)				//信息列表界面标题的纵坐标

#define CDTISHIX		8										//信息列表界面提示的横坐标
#define CDTISHIY1		(CDHENGY2+4)							//信息列表界面显示两行时的纵坐标
#define CDTISHIY2		CDTISHIY1+17
#define CDTISHIZY		CDTISHIY1+8								//信息列表界面显示一行时的纵坐标

#define CDZIFUX			(CDHENGX-5)								//信息列表界面显示内容区域横坐标

//信息列表显示区域可以显示11行数据，每一行的位置如下
#define CLINE1			CDHENGY1+6								//信息列表界面显示内容区域纵坐标
#define CLINE2			CLINE1+17
#define CLINE3			CLINE2+17
#define CLINE4			CLINE3+17
#define CLINE5			CLINE4+17
#define CLINE6			CLINE5+17
#define CLINE7			CLINE6+17
#define CLINE8			CLINE7+17
#define CLINE9			CLINE8+17
#define CLINE10			CLINE9+17
#define CLINE11			CLINE10+17

#define CDPROLENX		180										//信息列表中的进度条长宽
#define CDPROLENY		16
#define CDPROX			(5+((LCDP_MAX_X-5-CDPROLENX)/2))		//信息列表中的进度条横坐标

//****设置背光和时间等参数的档位条参数****//
#define CDDALLLEN		230										//档位条的总长度(包括图片)
#define CDDALLX			(5+((LCDP_MAX_X-5-CDDALLLEN)/2))		//档位条的起始坐标(包括图片)

#define CDIRONLENX		40										//档位条左右的图片长宽(空出余地)
#define CDIRONLENY		40

#define CDDANGLENX		(CDDALLLEN-(CDIRONLENX*2))				//档位条的长宽(不包括图片)
#define CDDANGLENY		16

#define CDDANGX			(5+((LCDP_MAX_X-5-CDDALLLEN)/2)+CDIRONLENX)		//档位条的起始坐标(不包括图片)

//****充电界面的充电符号参数****//
#define CHONGX			35										//充电界面充电符号的起始坐标
#define CHONGY			120

#define CHONGLENX		170										//充电界面充电符号的长宽
#define CHONGLENY		80

#define CHONGTXL		10										//充电界面充电符号前面小头的长宽
#define CHONGTYL		20
#define CHONGTY			CHONGY+((CHONGLENY-CHONGTYL)/2)			//充电界面充电符号的纵坐标






typedef void (*DlgFun)(void); //定义一个函数指针类型

extern void ShowClearTitle(void);
extern void ShowToTitleTime(void);
//extern void ShowPhoneSignalStrength(u8 CSQValue);
//extern void ShowGpsState(u16 x, u8 State);

extern void ShowInfoTitle(char* ptr);
extern void ShowInfoHelp(char* Line1, char* Line2);
extern void GetDetTypeName(char* Name, u8 Type);
extern void GetDetInfoString(char* Info, u8 Type);
//extern void GetGpsInfoString(GPS_INFO* Gps,u8 Type, char* Str);


extern int ShowDkeyUserInfoError(u8 Result, u8 State);
extern int ShowSDCardError(void);
extern void SetProgbarParam(void);
extern void UpdateProgbar(u16 Value);
extern void SetGuiDefaultColor(u32 BackgroundColor, u32 TextColor);
extern u32 GetDifferentColor(void);
extern void GetAccreditErrorString1ForDL(char* Info, u8 Code);
extern void GetAccreditErrorString2ForDL(char* Info, u8 Code);

#endif
