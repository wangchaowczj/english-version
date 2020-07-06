/**
  *************************************************************************************
  * @file    SPI.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SPI模块头文件
  *************************************************************************************
  */
#ifndef _SPI_H_
#define _SPI_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SPI
  *@{
  */
  
/**@defgroup SPI_PinDefines
  *@{
  */  
#define RF_SPI_CS_PORT       		GPIOA
#define RF_SPI_CS_PORT_RCC    		RCC_APB2Periph_GPIOA
#define RF_SPI_CS_PIN         		GPIO_Pin_4
#define RF_SPI_CS_H()               GPIO_SetBits(RF_SPI_CS_PORT, RF_SPI_CS_PIN)
#define RF_SPI_CS_L()               GPIO_ResetBits(RF_SPI_CS_PORT, RF_SPI_CS_PIN)

#define RF_SPI_PORT       		    GPIOA
#define RF_SPI_PORT_RCC    		    RCC_APB2Periph_GPIOA
#define RF_SPI_CLK_PIN         		GPIO_Pin_5
#define RF_SPI_MOSI_PIN         	GPIO_Pin_6
#define RF_SPI_MISO_PIN         	GPIO_Pin_7
#define RF_SPI         				SPI1
#define RF_SPI_RCC         			RCC_APB2Periph_SPI1

extern void RFSpiConfig(void);

#endif
