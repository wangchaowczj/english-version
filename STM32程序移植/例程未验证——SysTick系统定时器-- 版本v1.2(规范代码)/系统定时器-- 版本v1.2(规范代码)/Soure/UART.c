/*****************************************************************
*��Ȩ���� 2012��̩̹�Ƽ��ɷ����޹�˾
*
*�ļ����� UART.C
*����ժҪ����RS485�˿�,���ڽ��г�ʼ�������ļ��������� ���ͽӡ��պ���
*����˵����
*          
*��ǰ�汾��v1.0
*��    �ߣ�������
*������ڣ�2012��8��3��
******************************************************************/

#include "Include.h"

/*****************************************************************
*				            �궨��
******************************************************************/

#define RX_485  GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define TX_485  GPIO_ResetBits(GPIOA,GPIO_Pin_1);


/*****************************************************************
*�������ƣ�	UART2Init
*����������	�Դ���2�����������á�485���ƶ˿ڳ�ʼ��
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
void UART2Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ������ʱ��	  
         /*--------------485���ƶ˳�ʼ��------PA1----------*/
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
*�������ƣ�	UART2_TX485_Putc
*����������	���͵����ַ�����
* 
*���������c:Ҫ���͵��ַ�
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void UART2_TX485_Putc(unsigned char c)
{
    
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );

}

/*****************************************************************
*�������ƣ�	UART2_TX485_Puts
*����������	�����ַ�������
* 
*���������str:Ҫ���͵��ַ���
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/

void UART2_TX485_Puts(char * str)
{   
   
    while(*str)
    {  
       TX_485;     //��485���Ͷ˿ڣ��رս��ն˿�
       DelayNmS(1);
        USART_SendData(USART2, *str++);
        /* Loop until the end of transmission */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //��ⷢ�����ݼĴ����ձ�־λ
       DelayNmS(1);
        RX_485;	    //�رշ��Ͷ˿ڣ��򿪽���485�˿ڣ�
    }

}

/*****************************************************************
*�������ƣ�	UART2SendByte
*����������	��������
* 
*���������SendData:Ҫ���͵�����
*�� �� ֵ����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
void UART2SendByte(unsigned char SendData)
{	    
        TX_485; 
	    DelayNmS(1);
        USART_SendData(USART2,SendData);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);// USART_FLAG_TXE���ͼĴ����ձ�־λ
		DelayNmS(1);
	    RX_485;
	   
	    
}


/*****************************************************************
*�������ƣ�	UART2GetByte
*����������	��������
* 
*���������GetData:Ҫ���յ�����
*�� �� ֵ��0��û�н��յ����� ;  1:���յ�����
*����˵������
*��ǰ�汾��v1.0
*��    ��: ������
*������ڣ�2012��8��3��
*�޸�����      �汾��      �޸���      �޸�����
*-----------------------------------------------------------------
*
******************************************************************/
unsigned char UART2GetByte(unsigned char* GetData)
{
  	    RX_485;
	    DelayNmS(1); 
        if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) //USART_FLAG_RXNE:�������ݼĴ����ǿձ�־λ
        {
                return 0;//û���յ�����
        }
        *GetData = USART_ReceiveData(USART2);
	    DelayNmS(1);
	    TX_485; 
        return 1;//�յ�����
}


