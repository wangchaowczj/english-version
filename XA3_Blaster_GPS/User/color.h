#ifndef _COLOR_H_
#define _COLOR_H_

//24位颜色码
#define RED             0xFF0000
#define GREEN           0x00FF00
#define BLUE			0x0000FF
#define BLACK			0x000000
#define WHITE			0xFFFFFF
#define YALLOW			0xFFFF00

//对话框背景色
#define COLOR_DLG_BK           0x70FFCA

//****界面中最顶上显示时间、电池等的栏****//
#define COLOR_TOP_BACK			BLUE		//界面中最顶上显示时间的栏底色
#define COLOR_TOP_FONT			WHITE		//界面中最顶上显示时间的字符颜色
#define COLOR_TOP_HENG		   BLACK			//界面中顶栏和中间部分的分割线颜色

//****电池相关****//
#define COLOR_BAT_FONT			COLOR_WHITE			//电池的每个格颜色和框的颜色
#define COLOR_BAT_BACK			COLOR_BLACK			//电池的底色
#define COLOR_BAT_QIAN			COLOR_RED_1			//电池欠电时的每个格颜色

//****蓝牙和USB符号相关****//
#define COLOR_BLU_FONT			COLOR_WHITE			//蓝牙符号颜色
#define COLOR_USB_FONT			COLOR_DARKGREEN		//usb符号颜色
#define COLOR_USB_CHAA			COLOR_RED_1			//usb符号上面加的叉的颜色

//****界面的主色调****//
#define COLOR_ZHU				0xD699//0x8FF9COLOR_WHITE			//主界面和信息列表界面的背景色

//****F1到F4相关****//
//#define SHOWCOLOR_F14_BACK			COLOR_YALLOW_1		//F14的主色调
#define COLOR_F14_BACK			COLOR_BLUE_2		//F14的主色调
#define COLOR_F14_FONT			COLOR_WHITE			//F14的字体颜色
#define COLOR_F14_HENG			COLOR_BLACK		//F14和中间部分的分割线颜色

//****主界面相关****//
#define COLOR_IRO_BACK			COLOR_BLUE_2		//主界面中每个框的正常的颜色
#define COLOR_IRO_XUAN			COLOR_GREEN			//主界面中每个框的选中的颜色
#define COLOR_IRO_PRES			COLOR_BLUEAN		//主界面中每个框的按下的颜色
#define COLOR_IRO_MEIY			COLOR_BLUEWU		//主界面中每个框的不可能出现的颜色
#define COLOR_IRO_FONT1			COLOR_BLACK			//主界面中每个框中数字的颜色1
#define COLOR_IRO_FONT2			COLOR_WHITE			//主界面中每个框中数字的颜色2
#define COLOR_IRO_LITI			COLOR_BLACK			//主界面中每个框体现立体感的边的颜色
#define COLOR_NAM_FONT			COLOR_BLACK			//主界面中每个框下面对应的字符的颜色

//****提示界面相关****//
#define COLOR_TIS_BACK			COLOR_YALLOW		//提示界面的主色调
#define COLOR_TIS_FONT			COLOR_BLACK			//提示界面的字体颜色
#define COLOR_TIS_HENG			COLOR_BLACK			//提示界面的横线颜色
#define COLOR_TIS_LITI			COLOR_BLACK			//提示界面体现立体感的边的颜色
#define COLOR_TIS_GOUU			COLOR_DARKGREEN		//提示界面勾的颜色
#define COLOR_TIS_CHAA			COLOR_RED			//提示界面叉的颜色
#define COLOR_INT_BACK			COLOR_WHITE			//提示界面数据框的底色
#define COLOR_INT_FONT			COLOR_BLACK			//提示界面数据框数字的颜色
#define COLOR_INT_LITI			COLOR_BLACK			//提示界面数据框体现立体感的边的颜色
#define COLOR_PRT_BACK			COLOR_WHITE			//提示界面进度条的底色
#define COLOR_PRT_FONT			COLOR_BLUE			//提示界面进度条每个进度的颜色
#define COLOR_PRT_LITI			COLOR_BLACK			//提示界面进度条体现立体感的边的颜色

//****信息列表界面相关****//
#define COLOR_INF_BACK			COLOR_WHITE			//信息列表界面的主色调
#define COLOR_INF_FONT			COLOR_BLACK			//信息列表界面的字体颜色
#define COLOR_INF_HENG			COLOR_BLUE			//信息列表界面的横线颜色
#define COLOR_INF_XUAN			COLOR_BLUE			//信息列表界面的选中位置的颜色(反白显示)
#define COLOR_INF_YIDO			COLOR_CYAN			//信息列表界面的单发雷管移动的颜色(选中显示)
#define COLOR_INF_XFON			COLOR_LIRED			//信息列表界面的单发雷管移动时字体的颜色(选中显示)
#define COLOR_PRI_BACK			COLOR_WHITE			//信息列表界面的进度条的底色
#define COLOR_PRI_FONT			COLOR_BLUE			//信息列表界面的进度条每个进度的颜色
#define COLOR_PRI_LITI			COLOR_BLACK			//信息列表界面的进度条体现立体感的边的颜色
#define COLOR_INI_BACK			COLOR_WHITE			//信息列表界面的数据框的底色
#define COLOR_INI_FONT			COLOR_BLACK			//信息列表界面的数据框数字的颜色
#define COLOR_INI_LITI			COLOR_BLACK			//信息列表界面的数据框体现立体感的边的颜色
#define COLOR_SEI_LEFT			COLOR_BLUE			//信息列表界面的设置档位左边的颜色
#define COLOR_SEI_RIGT			COLOR_BLACK			//信息列表界面的设置档位右边的颜色
#define COLOR_SEI_KUAN			COLOR_BLUE			//信息列表界面的当前位置的框的颜色
#define COLOR_SEI_TIAN			COLOR_BLUE			//信息列表界面的当前位置的框的填充的颜色
#define COLOR_TIM_XUAN			COLOR_WHITE			//信息列表界面设置时间时的选中时字体的颜色
#define COLOR_TIM_RECT			COLOR_BLUE			//信息列表界面设置时间时的选中框的颜色

//****关机充电界面相关****//
#define COLOR_CHO_BACK			COLOR_BLACK			//充电界面的电池符号的底色
#define COLOR_CHO_FONT			COLOR_WHITE			//充电界面的电池符号的进度颜色

#endif



























