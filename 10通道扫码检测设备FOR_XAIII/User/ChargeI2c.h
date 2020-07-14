#ifndef _CHARGE_I2C_H_
#define _CHARGE_I2C_H_

#define CHARGE_I2C_SCL_PORT             GPIOF
#define CHARGE_I2C_SCL_PORT_RCC         RCC_AHB1Periph_GPIOF
#define CHARGE_I2C_SCL_PIN              GPIO_Pin_11

#define CHARGE_I2C_SDA_PORT             GPIOF
#define CHARGE_I2C_SDA_PORT_RCC         RCC_AHB1Periph_GPIOF
#define CHARGE_I2C_SDA_PIN              GPIO_Pin_12

#define CHARGE_I2C_SCL_H        		GPIO_SetBits(CHARGE_I2C_SCL_PORT, CHARGE_I2C_SCL_PIN)  //时钟线拉高
#define CHARGE_I2C_SCL_L        		GPIO_ResetBits(CHARGE_I2C_SCL_PORT, CHARGE_I2C_SCL_PIN)//时钟线拉低
   
#define CHARGE_I2C_SDA_H        		GPIO_SetBits(CHARGE_I2C_SDA_PORT, CHARGE_I2C_SDA_PIN) //数据线拉高
#define CHARGE_I2C_SDA_L        		GPIO_ResetBits(CHARGE_I2C_SDA_PORT, CHARGE_I2C_SDA_PIN) //数据线拉低

#define CHARGE_I2C_SDA_READ()     		GPIO_ReadInputDataBit(CHARGE_I2C_SDA_PORT, CHARGE_I2C_SDA_PIN)    //读数据线
								
extern void Charge_I2C_GPIO_Config(void);								
extern void Charge_I2C_Delay(unsigned int num);
extern unsigned char Charge_I2C_ReadByte(unsigned int ReadAddress,  unsigned char DeviceAddress, unsigned char* pDat);
extern u8 Charge_I2C_WriteData(unsigned char DeviceAddress, unsigned int WriteAddress, unsigned char* pDat, unsigned int nLen);
extern u8 Charge_I2C_ReadData(unsigned char DeviceAddress, unsigned int ReadAddress,  unsigned char* pDat, unsigned int nLen);
#endif
