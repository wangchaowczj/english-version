/*****************************************************************
*版权所有 2012，泰坦科技股份有限公司
*
*文件名： main.c
*内容摘要：主要功能是利用定时器和rs485实现每500ms中位机向上位机发送数据;
*其他说明：本程序除主功能外还有其他功能--上位机可以向中位机发送特定命令，
*           让中位机执行对应操作
*当前版本：v1.0
*作    者：梁尹宣
*完成日期：2012年8月3日
******************************************************************/

#include "Include.h"
#include  "core_cm3.h"

/*****************************************************************
*				            全局变量
******************************************************************/

static __IO uint32_t TimingDelay=0;

			
/*****************************************************************
*函数名称：	 Delay
*功能描述：	利用系统时钟计数器递减达到延时功能
* 
*输入参数：nTime ：需要延的时毫秒数
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void Delay(__IO uint32_t nTime)//delay被调用时，nTime=500
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


/*****************************************************************
*函数名称：	 SetSysClock
*功能描述：	设置系统时钟频率,同时配置相应时钟频率下 HCLK, PCLK2 
*            and PCLK1 预分频
* 
*输入参数：无
*返 回 值：无
*其他说明：	本函数static静态函数， 作用域仅限本文件
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
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
*函数名称：SystemInit
*功能描述：对RCC寄存器各参数进行配置，设定系统时钟	
*            
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
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
*函数名称：TimingDelay_Decrement
*功能描述：中断里调用此函数，即没发生一次中断，此函数被调用，此函数里	
*          的变量TimingDelay 相当于减法计数器
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
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
*函数名称：RCC_Configuration
*功能描述：对RCC寄存器各参数进行配置，设定系统时钟
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void RCC_Configuration(void)
{
    SystemInit(); 
}


/*****************************************************************
*函数名称：NVIC_Configuration
*功能描述：主要对哪些中断要发生，优先级进行设置
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
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
*函数名称：SystemInit1
*功能描述：对系统时钟、中断、相应硬件进行设置和初始化
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void SystemInit1(void)
{
        RCC_Configuration();//设置系统时钟
        NVIC_Configuration();//设置中断

       	LedInit();	  //led初始化
        UART2Init(); //串口2 初始化
	    
}

/*****************************************************************
*函数名称：RS485_Test
*功能描述：利用RS485实现上位机对中位机通过发送命令执行相应操作
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void RS485_Test(void)
{
     unsigned char i = 0;

      while(1)
       {
			   while(UART2GetByte(&i))   //收到数据UART3GetByte(&i)返回值为1；
                { 
                   UART2SendByte(i);
                   if(i==0x01) //点亮led2
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_6);
				   }
        		   if(i==0x02)	//点亮led3
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_7);
				   }
        		   if(i==0x03)  //点亮led4
				   {	 
				       GPIO_ResetBits(GPIOC,GPIO_Pin_9);
				   }
        		   if(i==0x04)  //熄灭led2、led3、led4
        		   {	  
        		             GPIO_SetBits(GPIOC,GPIO_Pin_6);   
        		             GPIO_SetBits(GPIOC,GPIO_Pin_7);
        		             GPIO_SetBits(GPIOC,GPIO_Pin_9);
        		   }
        		   
                }                    
        }
 
}



/*****************************************************************
*函数名称：	 main函数
*功能描述：	对各主要功能的实现
* 
*输入参数：无
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/
int main(void)
{		unsigned char i=0;
        unsigned char a[] = "abncdee";
        
        SystemInit1();//系统初始化

	if (SysTick_Config(72000))  //1ms响应一次中断
        { 
            /* Capture error */ 
      		while (1);
        } 
        /*解析：因为要求是每500ms往中位机发数据一件事，所以放在while语句中，发送据+延时 可以完成相当于中断的效果;
		 *      若是多任务中，其中一个任务需要中断，这把这个任务放在中断函数中调用;
		 */
        while (1)
        {
             //测试代码:测试定时器功能，通过延时来测试

             GPIO_SetBits(GPIOC, GPIO_Pin_6);	     //V6 
             Delay(50);
             GPIO_ResetBits(GPIOC, GPIO_Pin_6);	     //V6 
             Delay(50);
			 
            //功能1代码:每500ms发送数据
		 /*
			 UART2_TX485_Puts("123450");
			 Delay(500);
           */
            //功能2代码:上位发特定指令，中位机执行相应操作
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
