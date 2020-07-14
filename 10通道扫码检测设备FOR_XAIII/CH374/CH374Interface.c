#include "includes.h"


/**
  *************************************************************************************
  * @brief  CH374 SPI����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   
  *************************************************************************************  
  */
void CH374SpiConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;


	RCC_AHB1PeriphClockCmd(CH374_SPI_SCLK_PORT_RCC | CH374_SPI_MISO_PORT_RCC| 
		CH374_SPI_MOSI_PORT_RCC | CH374_SPI_CS_PORT_RCC | CH374_INT_PORT_RCC, ENABLE);
		
	RCC_APB1PeriphClockCmd(CH374_SPI_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_PinAFConfig(CH374_SPI_SCLK_PORT, GPIO_PinSource10, GPIO_AF_SPI2);
	GPIO_PinAFConfig(CH374_SPI_MISO_PORT, GPIO_PinSource2, GPIO_AF_SPI2);
	GPIO_PinAFConfig(CH374_SPI_MOSI_PORT, GPIO_PinSource3, GPIO_AF_SPI2);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
	GPIO_InitStruct.GPIO_Pin = CH374_SPI_CS_PIN;
	GPIO_Init(CH374_SPI_CS_PORT, &GPIO_InitStruct);
	
	/* ����SPI�������� */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_InitStruct.GPIO_Pin = CH374_SPI_SCLK_PIN;
	GPIO_Init(CH374_SPI_SCLK_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = CH374_SPI_MISO_PIN;
	GPIO_Init(CH374_SPI_MISO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = CH374_SPI_MOSI_PIN;
	GPIO_Init(CH374_SPI_MOSI_PORT, &GPIO_InitStruct);	

	
	/*!< SPI1 configuration */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//USBоƬ�͵�Ƭ������̫����2��Ƶʱ�ڴӻ�MSCģʽ�£�������ļ�ʱ�����в��ԡ�4��Ƶż���� 

	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(CH374_SPI, &SPI_InitStruct);

	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(CH374_SPI, ENABLE);
}

/**
  *************************************************************************************
  * @brief  CH374Ӳ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��8��22��
  * @note   ��
  *************************************************************************************  
  */
void Ch374HardWareConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(CH374_INT_PORT_RCC|USB_GET_IN_PORT_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		
	GPIO_InitStruct.GPIO_Pin = CH374_INT_PIN;
	GPIO_Init(CH374_INT_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
	GPIO_InitStruct.GPIO_Pin = USB_GET_IN_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(USB_GET_IN_PORT, &GPIO_InitStruct);	
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);//PC0
	
	/*�ⲿ�½����ж�*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ⲿ�½����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI9_5_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	CH374SpiConfig();
}
/**
  *************************************************************************************
  * @brief  SPI1���Ͳ�����һ���ֽ�����
  * @param  byte: Ҫ���͵�����
  * @retval ���յ�������
  * @author ZH
  * @date   2015��7��21��
  * @note   ��
  *************************************************************************************  
  */
u8 CH374SpiSendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(CH374_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(CH374_SPI, byte);
	while (SPI_I2S_GetFlagStatus(CH374_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(CH374_SPI);
}

/**
  *************************************************************************************
  * @brief  ��ָ���Ĵ�����ȡ����
  * @param  mAddr: Ҫ���͵�����
  * @retval ���յ�������
  * @author ZH
  * @date   2015��7��21��
  * @note   ��
  *************************************************************************************  
  */
u8 Read374Byte(u8 mAddr)
{
	u8 Value;
	
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374READ);

	Value = CH374SpiSendByte(0xFF);
	
	CH374_SPI_CS_H();
	
	return Value;
}

/**
  *************************************************************************************
  * @brief  ��ָ���Ĵ���д������
  * @param  mAddr: �Ĵ�����ַ
  * @param  mData: д�������  
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   ��
  *************************************************************************************  
  */
void Write374Byte(u8 mAddr, u8 mData)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);
	
	CH374SpiSendByte(mData);
	
	CH374_SPI_CS_H();
}

/**
  *************************************************************************************
  * @brief  ��ָ����ʼ��ַ�������ݿ�
  * @param  mAddr: �Ĵ�����ַ
  * @param  mLen: Ҫ�������ݳ���
  * @param  mBuf: ���ݻ�������ַ  
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   ��
  *************************************************************************************  
  */
void Read374Block(u8 mAddr, u8 mLen, u8* mBuf)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374READ);

	while(mLen--)
	{
		*mBuf++ = CH374SpiSendByte(0xFF);
	}	
	CH374_SPI_CS_H();
}

/**
  *************************************************************************************
  * @brief  ��ָ����ʼ��ַд�����ݿ�
  * @param  mAddr: �Ĵ�����ַ
  * @param  mLen: Ҫд�����ݳ���
  * @param  mBuf: ���ݻ�������ַ  
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   ��
  *************************************************************************************  
  */
void Write374Block(u8 mAddr, u8 mLen, u8* mBuf)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);

	while (mLen--)
	{
		CH374SpiSendByte(*mBuf++);
	}
	CH374_SPI_CS_H();
}

void CH374WriteBlockZero(u8 mAddr, u8 mLen)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);

	while (mLen--)
	{
		CH374SpiSendByte(0x00);
	}
	CH374_SPI_CS_H();
}

ErrorStatus Ch374HardwareState(void)
{
	u8 Value;
	
	Value = Read374Byte(0x04);//Ӳ��ʶ��λ��������֤Ӳ����������Ͷ������ɹ�
	if((Value&0x03) == 0x01)
	{
		return SUCCESS;
	}
	return ERROR;
}
