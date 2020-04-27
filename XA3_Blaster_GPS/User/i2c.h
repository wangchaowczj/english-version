#ifndef _I2C_H_
#define _I2C_H_

#define I2C_SCL_PORT             GPIOH
#define I2C_SCL_PORT_RCC         RCC_AHB1Periph_GPIOH
#define I2C_SCL_PIN              GPIO_Pin_10

#define I2C_SDA_PORT             GPIOH
#define I2C_SDA_PORT_RCC         RCC_AHB1Periph_GPIOH
#define I2C_SDA_PIN              GPIO_Pin_11

#define I2C_SCL_H        		GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN)  //时钟线拉高
#define I2C_SCL_L        		GPIO_ResetBits(I2C_SCL_PORT, I2C_SCL_PIN)//时钟线拉低
   
#define I2C_SDA_H        		GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN) //数据线拉高
#define I2C_SDA_L        		GPIO_ResetBits(I2C_SDA_PORT, I2C_SDA_PIN) //数据线拉低

#define I2C_SDA_READ()     		GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN)    //读数据线
								
extern void I2C_GPIO_Config(void);								
extern void I2C_Delay(unsigned int num);
extern unsigned char I2C_ReadByte(unsigned int ReadAddress,  unsigned char DeviceAddress, unsigned char* pDat);
extern u8 I2C_WriteData(unsigned char DeviceAddress, unsigned int WriteAddress, unsigned char* pDat, unsigned int nLen);
extern u8 I2C_ReadData(unsigned char DeviceAddress, unsigned int ReadAddress,  unsigned char* pDat, unsigned int nLen);
#endif
