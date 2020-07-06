#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "movie.h"	
//ALIENTEKս��STM32������ʵ��12
//OLED��ʾʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
	
//  int main(void)
//  {	u8 t;
// 	delay_init();	    	 //��ʱ������ʼ��	  
// 	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//  	LED_Init();			     //LED�˿ڳ�ʼ��
// 	OLED_Init();			//��ʼ��OLED      
//  	OLED_ShowString(0,0, "0.96' OLED TEST");  
//  	OLED_ShowString(0,16,"ATOM@ALIENTEK");  
//  	OLED_ShowString(0,32,"2010/06/3");  
//  	OLED_ShowString(0,48,"ASCII:");  
//  	OLED_ShowString(63,48,"CODE:");  
// 	OLED_Refresh_Gram();	 
// 	t=' ';  
// 	while(1) 
// 	{		
// 		OLED_ShowChar(48,48,t,16,1);//��ʾASCII�ַ�	   
// 		OLED_Refresh_Gram();
// 		t++;
// 		if(t>'~')t=' ';
// 		OLED_ShowNum(103,48,t,3,16);//��ʾASCII�ַ�����ֵ 
// 		delay_ms(300);
// 		LED1=!LED1;
// 	}	  
// 	
// }

int main(void)
{	
	u8 t=0;			  
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	LED_Init();			     //LED�˿ڳ�ʼ��
	OLED_Init();			//��ʼ��OLED   		
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
		Movie_Show_Img(14,12,t);//ͼƬ��ʾ��������
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
