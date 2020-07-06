#include "sys.h"
#include "oled.h"
#include "movie.h" 
#include "m_data.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板-扩展实验5
//OLED动画显示 代码			 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/8/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 

//这两个参数定义动画每帧的长宽。
#define MOVIE_XSIZE	100 
#define MOVIE_YSIZE 40  
#define FRAME_SIZE MOVIE_XSIZE*MOVIE_YSIZE/8   
							  
//x:0~127
//y:0~63
//picx:图片编号		 
void Movie_Show_Img(u16 x,u16 y,u32 picx)
{      			    
	u16 temp,t,t1;
	u16 y0=y;	
	picx*=FRAME_SIZE;				    				   
    for(t=0;t<FRAME_SIZE;t++)
    {   
		temp=movie_data[picx+t];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1); 
			else OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			y++;
			if((y-y0)==MOVIE_YSIZE)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
 

























