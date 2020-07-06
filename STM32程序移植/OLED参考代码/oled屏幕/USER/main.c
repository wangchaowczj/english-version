#include "stm32f10x.h"
#include "OLED_I2C.h"

#include "delay.h"
#include "LED.H"
#include "timer.h"

int main(void)
{
u16 led0pwmval=0;
	u8 dir=1;
unsigned char i;
unsigned char vel[3]={49,50,56};
extern const unsigned char BMP1[];


  led_init();
	DelayInit();
	I2C_Configuration();
	OLED_Init();
	OLED_CLS();//����
	TIM3_PWM_Init(899,0);	 //����Ƶ��PWMƵ��=72000000/900=80Khz
	while(1)
	{
//	  delay_ms(10);
	if(dir)led0pwmval++;
		else led0pwmval--;

 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;										 
		TIM_SetCompare2(TIM3,led0pwmval);
	vel[0]=led0pwmval/100+48;
	  vel[1]=led0pwmval/10%10+48;
		vel[2]=led0pwmval%10+48;    
		
	  OLED_ShowStr(0,3,vel,1);//����6*8�ַ�
		OLED_ShowStr(0,4,"HAUT-LH",2);				//����8*16�ַ�
		DelayS(1);
	
	}
	/*
while(1)
{
//  GPIO_SetBits(GPIOB,GPIO_Pin_5);
//	GPIO_SetBits(GPIOE,GPIO_Pin_5);
//    delay_ms(500);
 
//  GPIO_ResetBits(GPIOB,GPIO_Pin_5);
 // GPIO_ResetBits(GPIOE,GPIO_Pin_5);
 //  delay_ms(500);
// OLED_Fill(0xFF);//ȫ������
//		DelayS(2);
//		OLED_Fill(0x00);//ȫ����
		DelayS(1);
		for(i=0;i<6;i++)
		{
			OLED_ShowCN(22+i*16,0,i);//������ʾ����
		}                                            
		  DelayS(2);
		  OLED_ShowStr(0,3,"HelTec Automation",1);//����6*8�ַ�
	  	OLED_ShowStr(0,4,"Hello Tech",2);				//����8*16�ַ�
	   	DelayS(1);
 	    OLED_CLS();//����
      OLED_OFF();//����OLED����
  	  DelayS(2);
	    OLED_ON();//����OLED���ߺ���
	  	OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//����BMPλͼ��ʾ
	  	DelayS(2);
} 
*/
}

