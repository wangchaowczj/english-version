#ifndef _FSMC_H_
#define _FSMC_H_

#define LCD_TOUCH_STATE_PORT         GPIOE
#define LCD_TOUCH_STATE_PORT_RCC     RCC_AHB1Periph_GPIOE
#define LCD_TOUCH_STATE_PIN          GPIO_Pin_0
#define GET_LCD_TOUCH_STATE()        GPIO_ReadInputDataBit(LCD_TOUCH_STATE_PORT, LCD_TOUCH_STATE_PIN)

#define LCD_STATE_PORT               GPIOD
#define LCD_STATE_PORT_RCC           RCC_AHB1Periph_GPIOD
#define LCD_STATE_PIN                GPIO_Pin_6
#define GET_LCD_STATE()              GPIO_ReadInputDataBit(LCD_STATE_PORT, LCD_STATE_PIN)

#define LCD_DIMM_PORT                GPIOD
#define LCD_DIMM_PORT_RCC            RCC_AHB1Periph_GPIOD
#define LCD_DIMM_PIN                 GPIO_Pin_13
#define LCD_DIMM_PIN_L()             GPIO_ResetBits(LCD_DIMM_PORT, LCD_DIMM_PIN) 
#define LCD_DIMM_PIN_H()             GPIO_SetBits(LCD_DIMM_PORT, LCD_DIMM_PIN)
#define GET_DIMM_PIN_STATE()         GPIO_ReadOutputDataBit(LCD_DIMM_PORT, LCD_DIMM_PIN)

#define LCD_RESET_PORT               GPIOD
#define LCD_RESET_PORT_RCC           RCC_AHB1Periph_GPIOD
#define LCD_RESET_PIN                GPIO_Pin_11
#define LCD_RESET_PIN_L()            GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN) 
#define LCD_RESET_PIN_H()            GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN)

#define LCD_POWER_PORT               GPIOC
#define LCD_POWER_PORT_RCC           RCC_AHB1Periph_GPIOC
#define LCD_POWER_PIN                GPIO_Pin_9
#define OPEN_LCD_POWER()             GPIO_ResetBits(LCD_POWER_PORT, LCD_POWER_PIN) 
#define CLOSE_LCD_POWER()            GPIO_SetBits(LCD_POWER_PORT, LCD_POWER_PIN)

//----------------------------��дLCD�������ĵ�ַ�������ʽ------------------------------//

//ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
//FSMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
//16 bit => FSMC[24:0]��ӦHADDR[25:1]
//�Ĵ�������ַ = 0X60000000
//RAM����ַ = 0X60000000+2^17*2 = 0X60000000 + 0X40000 = 0X60040000
//��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼��㡣
#define Bank1_LCD_D		((unsigned int)0x60000000)		//���ݵ�ַ
//#define Bank1_LCD_D		((unsigned int)0x60040000)		//���ݵ�ַ
#define Bank1_LCD_C		((unsigned int)0x60040000)		//�����ַ

//������д���������
#define LCD_CmdWrite(index)		((*(__IO u16 *) (Bank1_LCD_C)) = ((u16)index))
								
#define LCD_DataWrite(val)	((*(__IO u16 *) (Bank1_LCD_D)) = ((u16)(val)))

//��ȡ״̬
#define LCD_StatusRead()		    *(__IO u16 *) (Bank1_LCD_C)

//��ȡ����
#define LCD_DataRead()		*(__IO u16 *) (Bank1_LCD_D)

extern void LCD_FSMC_Config(void);

#endif
