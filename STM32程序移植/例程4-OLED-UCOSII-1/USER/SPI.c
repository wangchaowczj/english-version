#include "includes.h"



/**
  *************************************************************************************
  * @brief  SPI1配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void RFSpiConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	RCC_APB2PeriphClockCmd(RF_SPI_CS_PORT_RCC | RF_SPI_PORT_RCC | RF_SPI_RCC | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = RF_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RF_SPI_CS_PORT, &GPIO_InitStruct);
	
	/* 配置SPI1引脚类型 */
	GPIO_InitStruct.GPIO_Pin = RF_SPI_CLK_PIN | RF_SPI_MOSI_PIN | RF_SPI_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_SPI_PORT, &GPIO_InitStruct);

	
	/*!< SPI1 configuration */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(RF_SPI, &SPI_InitStruct);

	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(RF_SPI, ENABLE);	
}	
