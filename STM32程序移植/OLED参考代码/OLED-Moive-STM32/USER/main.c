#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "movie.h"	
//ALIENTEK战舰STM32开发板实验12
//OLED显示实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
	
//  int main(void)
//  {	u8 t;
// 	delay_init();	    	 //延时函数初始化	  
// 	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//  	LED_Init();			     //LED端口初始化
// 	OLED_Init();			//初始化OLED      
//  	OLED_ShowString(0,0, "0.96' OLED TEST");  
//  	OLED_ShowString(0,16,"ATOM@ALIENTEK");  
//  	OLED_ShowString(0,32,"2010/06/3");  
//  	OLED_ShowString(0,48,"ASCII:");  
//  	OLED_ShowString(63,48,"CODE:");  
// 	OLED_Refresh_Gram();	 
// 	t=' ';  
// 	while(1) 
// 	{		
// 		OLED_ShowChar(48,48,t,16,1);//显示ASCII字符	   
// 		OLED_Refresh_Gram();
// 		t++;
// 		if(t>'~')t=' ';
// 		OLED_ShowNum(103,48,t,3,16);//显示ASCII字符的码值 
// 		delay_ms(300);
// 		LED1=!LED1;
// 	}	  
// 	
// }

int main(void)
{	
	u8 t=0;			  
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	LED_Init();			     //LED端口初始化
	OLED_Init();			//初始化OLED   		
	OLED_ShowString(0,0,"ALIENTEK");
	OLED_ShowString(0,16,"Mini STM32");
	OLED_ShowString(0,32,"MOVIE SHOW");
	OLED_Refresh_Gram();
	delay_ms(800);
	OLED_ShowString(0,48,"Read...");
	OLED_Refresh_Gram();
	delay_ms(1000);
	OLED_Clear();
	while(1) 
	{	
		Movie_Show_Img(14,12,t);//图片显示在最中央
		OLED_Refresh_Gram();
		if(t<12)delay_ms(600);
		else if(t<14)delay_ms(1000);	  
		else if(t<15)delay_ms(1800);
		else if(t<19)delay_ms(800);
		else if(t<20)delay_ms(1800);
		else if(t<130)delay_ms(100);
		else if(t<132)
		{
			delay_ms(1500);
			delay_ms(1500);
		}else if(t<133)delay_ms(1000);
		else if(t<140)delay_ms(100);
		else if(t<141)
		{
			delay_ms(1800);
			delay_ms(1200);
		} 
		t++;			  
		if(t>140)t=0;
	//	LED0=!LED0;
	}	  
}
