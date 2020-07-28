/*****************************************************************
*��Ȩ���� 2012��̩̹�Ƽ��ɷ����޹�˾
*
*�ļ����� main.c
*����ժҪ����Ҫ���������ö�ʱ����rs485ʵ��ÿ500ms��λ������λ����������;
*����˵������������������⻹����������--��λ����������λ�������ض����
*           ����λ��ִ�ж�Ӧ����
*��ǰ�汾��v1.0
*��    �ߣ�������
*������ڣ�2012��8��3��
******************************************************************/

#include "Include.h"
#include  "core_cm3.h"

/*****************************************************************
*				            ȫ�ֱ���
******************************************************************/

static __IO uint32_t TimingDelay=0;

			
/*****************************************************************
*�������ƣ�	 Delay
*����������	����ϵͳʱ�Ӽ������ݼ��ﵽ��ʱ����
* 
*���������nTime ����Ҫ�ӵ�ʱ������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void Delay(__IO uint32_t nTime)//delay������ʱ��nTime=500
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


/*****************************************************************
*�������ƣ�	 SetSysClock
*����������	����ϵͳʱ��Ƶ��,ͬʱ������Ӧʱ��Ƶ���� HCLK, PCLK2 
*            and PCLK1 Ԥ��Ƶ
* 
*�����������
*�� �� ֵ����
*����˵����	������static��̬������ ��������ޱ��ļ�
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_20MHz
  SetSysClockTo20();
#elif defined SYSCLK_FREQ_36MHz
  SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
  SetSysClockTo56();  
#elif defined SYSCLK_FREQ_72MHz
  SetSysClockTo72();
#endif
} 


/*****************************************************************
*�������ƣ�SystemInit
*������������RCC�Ĵ����������������ã��趨ϵͳʱ��	
*            
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void SystemInit (void)
{
  /*!< RCC system reset(for debug purpose) */
  /*!< Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;
  /*!< Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0] and MCO[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FF0000;  
  /*!< Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;
  /*!< Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;
  /*!< Reset PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;
  /*!< Disable all interrupts */
  RCC->CIR = 0x00000000;
    
  /*!< Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
  /*!< Configure the Flash Latency cycles and enable prefetch buffer */
  SetSysClock();

}


/*****************************************************************
*�������ƣ�TimingDelay_Decrement
*�����������ж�����ô˺�������û����һ���жϣ��˺��������ã��˺�����	
*          �ı���TimingDelay �൱�ڼ���������
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
   
void TimingDelay_Decrement(void)  
{  
  
  if (TimingDelay != 0x00)  
  {   
    TimingDelay--;  
  }
  
}   


/*****************************************************************
*�������ƣ�RCC_Configuration
*������������RCC�Ĵ����������������ã��趨ϵͳʱ��
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void RCC_Configuration(void)
{
    SystemInit(); 
}


/*****************************************************************
*�������ƣ�NVIC_Configuration
*������������Ҫ����Щ�ж�Ҫ���������ȼ���������
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
void NVIC_Configuration(void)
{ 
        //NVIC_InitTypeDef NVIC_InitStructure;
   
#ifdef  VECT_TAB_RAM  
        /* Set the Vector Table base location at 0x20000000 */ 
        NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
        /* Set the Vector Table base location at 0x08000000 */ 
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
}


/*****************************************************************
*�������ƣ�SystemInit1
*������������ϵͳʱ�ӡ��жϡ���ӦӲ���������úͳ�ʼ��
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void SystemInit1(void)
{
        RCC_Configuration();//����ϵͳʱ��
        NVIC_Configuration();//�����ж�

       	LedInit();	  //led��ʼ��
        UART2Init(); //����2 ��ʼ��
	    
}

/*****************************************************************
*�������ƣ�RS485_Test
*��������������RS485ʵ����λ������λ��ͨ����������ִ����Ӧ����
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void RS485_Test(void)
{
     unsigned char i = 0;

      while(1)
       {
			   while(UART2GetByte(&i))   //�յ�����UART3GetByte(&i)����ֵΪ1��
                { 
                   UART2SendByte(i);
                   if(i==0x01) //����led2
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_6);
				   }
        		   if(i==0x02)	//����led3
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_7);
				   }
        		   if(i==0x03)  //����led4
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_9);
				   }
        		   if(i==0x04)  //Ϩ��led2��led3��led4
        		   {	  
        		             GPIO_SetBits(GPIOC,GPIO_Pin_6);   
        		             GPIO_SetBits(GPIOC,GPIO_Pin_7);
        		             GPIO_SetBits(GPIOC,GPIO_Pin_9);
        		   }
        		   
                }                    
        }
 
}



/*****************************************************************
*�������ƣ�	 main����
*����������	�Ը���Ҫ���ܵ�ʵ��
* 
*�����������
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
int main(void)
{		unsigned char i=0;
        unsigned char a[] = "abncdee";
        
        SystemInit1();//ϵͳ��ʼ��

	if (SysTick_Config(72000))  //1ms��Ӧһ���ж�
        { 
            /* Capture error */ 
      		while (1);
        } 
        /*��������ΪҪ����ÿ500ms����λ��������һ���£����Է���while����У����;�+��ʱ ��������൱���жϵ�Ч��;
		 *      ���Ƕ������У�����һ��������Ҫ�жϣ���������������жϺ����е���;
		 */
        while (1)
        {
             //���Դ���:���Զ�ʱ�����ܣ�ͨ����ʱ������

             GPIO_SetBits(GPIOC, GPIO_Pin_6);	     //V6 
             Delay(50);
             GPIO_ResetBits(GPIOC, GPIO_Pin_6);	     //V6 
             Delay(50);
			 
            //����1����:ÿ500ms��������
		 /*
			 UART2_TX485_Puts("123450");
			 Delay(500);
           */
            //����2����:��λ���ض�ָ���λ��ִ����Ӧ����
		//	RS485_Test();
	
		}	 
       
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
