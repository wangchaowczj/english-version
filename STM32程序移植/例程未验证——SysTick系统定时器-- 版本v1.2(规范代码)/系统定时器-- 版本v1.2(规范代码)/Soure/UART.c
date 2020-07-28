/*****************************************************************
*版权所有 2012，泰坦科技股份有限公司
*
*文件名： UART.C
*内容摘要：对RS485端口,串口进行初始化，本文件还包括了 发送接、收函数
*其他说明：
*          
*当前版本：v1.0
*作    者：梁尹宣
*完成日期：2012年8月3日
******************************************************************/

#include "Include.h"

/*****************************************************************
*				            宏定义
******************************************************************/

#define RX_485  GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define TX_485  GPIO_ResetBits(GPIOA,GPIO_Pin_1);


/*****************************************************************
*函数名称：	UART2Init
*功能描述：	对串口2参数进行设置、485控制端口初始化
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
void UART2Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能外设时钟	  
         /*--------------485控制端初始化------PA1----------*/
         GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;          //485_TX
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           //485_RX
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = 9600;         
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
        USART_Init(USART2, &USART_InitStructure);

        USART_Cmd(USART2, ENABLE);
}

/*****************************************************************
*函数名称：	UART2_TX485_Putc
*功能描述：	发送单个字符数据
* 
*输入参数：c:要发送的字符
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void UART2_TX485_Putc(unsigned char c)
{
    
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );

}

/*****************************************************************
*函数名称：	UART2_TX485_Puts
*功能描述：	发送字符串数据
* 
*输入参数：str:要发送的字符串
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/

void UART2_TX485_Puts(char * str)
{   
   
    while(*str)
    {  
       TX_485;     //打开485发送端口，关闭接收端口
       DelayNmS(1);
        USART_SendData(USART2, *str++);
        /* Loop until the end of transmission */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //检测发送数据寄存器空标志位
       DelayNmS(1);
        RX_485;	    //关闭发送端口，打开接收485端口，
    }

}

/*****************************************************************
*函数名称：	UART2SendByte
*功能描述：	发送数据
* 
*输入参数：SendData:要发送的数据
*返 回 值：无
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/
void UART2SendByte(unsigned char SendData)
{	    
        TX_485; 
	    DelayNmS(1);
        USART_SendData(USART2,SendData);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);// USART_FLAG_TXE发送寄存器空标志位
		DelayNmS(1);
	    RX_485;
	   
	    
}


/*****************************************************************
*函数名称：	UART2GetByte
*功能描述：	发送数据
* 
*输入参数：GetData:要接收的数据
*返 回 值：0：没有接收到数据 ;  1:接收到数据
*其他说明：无
*当前版本：v1.0
*作    者: 梁尹宣
*完成日期：2012年8月3日
*修改日期      版本号      修改人      修改内容
*-----------------------------------------------------------------
*
******************************************************************/
unsigned char UART2GetByte(unsigned char* GetData)
{
  	    RX_485;
	    DelayNmS(1); 
        if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) //USART_FLAG_RXNE:接收数据寄存器非空标志位
        {
                return 0;//没有收到数据
        }
        *GetData = USART_ReceiveData(USART2);
	    DelayNmS(1);
	    TX_485; 
        return 1;//收到数据
}


