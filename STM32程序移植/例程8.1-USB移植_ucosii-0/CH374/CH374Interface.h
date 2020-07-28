#ifndef _CH374INTERFACE_H_
#define _CH374INTERFACE_H_

#define CH374_SPI_SCLK_PORT              GPIOB 
#define CH374_SPI_SCLK_PORT_RCC          RCC_AHB1Periph_GPIOB
#define CH374_SPI_SCLK_PIN               GPIO_Pin_10

#define CH374_SPI_MISO_PORT              GPIOC
#define CH374_SPI_MISO_PORT_RCC          RCC_AHB1Periph_GPIOC
#define CH374_SPI_MISO_PIN               GPIO_Pin_2

#define CH374_SPI_MOSI_PORT              GPIOC
#define CH374_SPI_MOSI_PORT_RCC          RCC_AHB1Periph_GPIOC
#define CH374_SPI_MOSI_PIN               GPIO_Pin_3

#define CH374_SPI_CS_PORT       		 GPIOC
#define CH374_SPI_CS_PORT_RCC    		 RCC_AHB1Periph_GPIOD
#define CH374_SPI_CS_PIN         		 GPIO_Pin_1

#define CH374_SPI_CS_H()                 GPIO_SetBits(CH374_SPI_CS_PORT, CH374_SPI_CS_PIN)
#define CH374_SPI_CS_L()                 GPIO_ResetBits(CH374_SPI_CS_PORT, CH374_SPI_CS_PIN)


#define CH374_INT_PORT       		     GPIOC
#define CH374_INT_PORT_RCC    		     RCC_AHB1Periph_GPIOC
#define CH374_INT_PIN         		     GPIO_Pin_0

#define GET_CH374_INT_PIN()              GPIO_ReadInputDataBit(CH374_INT_PORT, CH374_INT_PIN)

#define CH374_SPI_RCC   RCC_APB1Periph_SPI2
#define CH374_SPI  SPI2

#define USB_GET_IN_PORT       		 GPIOE
#define USB_GET_IN_PORT_RCC    		 RCC_AHB1Periph_GPIOE
#define USB_GET_IN_PIN         		 GPIO_Pin_4

#define USB_CONNET_DEVICE   0
#define READ_USB_GET_IN_PIN()        GPIO_ReadInputDataBit(USB_GET_IN_PORT, USB_GET_IN_PIN)


extern void Ch374HardWareConfig(void);
extern u8 Read374Byte(u8 mAddr);
extern void Write374Byte(u8 mAddr, u8 mData);
extern void CH374WriteBlockZero(u8 mAddr, u8 mLen);
extern void Read374Block(u8 mAddr, u8 mLen, u8* mBuf);
extern void Write374Block(u8 mAddr, u8 mLen, u8* mBuf);
extern ErrorStatus Ch374HardwareState(void);
#endif

