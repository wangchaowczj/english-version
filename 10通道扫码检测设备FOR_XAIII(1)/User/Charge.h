#ifndef _CHARGE_H_
#define _CHARGE_H_

#define CHARGE_DEVICE_ADDR    (0x6B<<1)

#define PW_INT_PORT            GPIOB
#define PW_INT_PORT_RCC        RCC_AHB1Periph_GPIOB
#define PW_INT_PIN             GPIO_Pin_5

#define PW_KILL_PORT           GPIOB
#define PW_KILL_PORT_RCC       RCC_AHB1Periph_GPIOB
#define PW_KILL_PIN            GPIO_Pin_4


#define PW_STATE_PORT      	   GPIOB
#define PW_STATE_PORT_RCC      RCC_AHB1Periph_GPIOB
#define PW_STATE_PIN           GPIO_Pin_6

//2A充电芯片管脚配置
#define CHARGE_PG_PORT         GPIOG
#define CHARGE_PG_PORT_RCC     RCC_AHB1Periph_GPIOG
#define CHARGE_PG_PIN          GPIO_Pin_2
#define GET_CHARGE_PG_PIN()    GPIO_ReadInputDataBit(CHARGE_PG_PORT, CHARGE_PG_PIN)

//电源选择输入。高表示USB主机源和低表明一个适配器电源
#define CHARGE_PSEL_PORT         GPIOG
#define CHARGE_PSEL_PORT_RCC     RCC_AHB1Periph_GPIOG
#define CHARGE_PSEL_PIN          GPIO_Pin_3
#define CHARGE_PSEL_PIN_H()      GPIO_SetBits(CHARGE_PSEL_PORT, CHARGE_PSEL_PIN)
#define CHARGE_PSEL_PIN_L()      GPIO_ResetBits(CHARGE_PSEL_PORT, CHARGE_PSEL_PIN)

#define CHARGE_STATUS_PORT         GPIOG
#define CHARGE_STATUS_PORT_RCC     RCC_AHB1Periph_GPIOG
#define CHARGE_STATUS_PIN          GPIO_Pin_4
//0表示正在充电，1表示充电完成或者禁止充电，方波方法表示异常，周期大概50ms
#define GET_CHARGE_STATUS_PIN()      GPIO_ReadInputDataBit(CHARGE_STATUS_PORT, CHARGE_STATUS_PIN)

#define USB_SW_PORT         GPIOA
#define USB_SW_PORT_RCC     RCC_AHB1Periph_GPIOA
#define USB_SW_PIN          GPIO_Pin_11
#define USB_OUTPUT_L()      GPIO_SetBits(USB_SW_PORT, USB_SW_PIN)
#define USB_OUTPUT_H()      GPIO_ResetBits(USB_SW_PORT, USB_SW_PIN)

#define CHARGE_INT_PORT         GPIOC
#define CHARGE_INT_PORT_RCC     RCC_AHB1Periph_GPIOC
#define CHARGE_INT_PIN          GPIO_Pin_5
#define GET_CHARGE_INT_PIN()      GPIO_ReadInputDataBit(CHARGE_INT_PORT, CHARGE_INT_PIN)

#define CHARGE_OTG_PORT         GPIOB
#define CHARGE_OTG_PORT_RCC     RCC_AHB1Periph_GPIOB
#define CHARGE_OTG_PIN          GPIO_Pin_0
#define CHARGE_OTG_PIN_H()      GPIO_SetBits(CHARGE_OTG_PORT, CHARGE_OTG_PIN)
#define CHARGE_OTG_PIN_L()      GPIO_ResetBits(CHARGE_OTG_PORT, CHARGE_OTG_PIN)

#define CHARGE_CE_PORT         GPIOC
#define CHARGE_CE_PORT_RCC     RCC_AHB1Periph_GPIOC
#define CHARGE_CE_PIN          GPIO_Pin_4
#define CHARGE_CE_PIN_H()      GPIO_SetBits(CHARGE_CE_PORT, CHARGE_CE_PIN)
#define CHARGE_CE_PIN_L()      GPIO_ResetBits(CHARGE_CE_PORT, CHARGE_CE_PIN)

#define CHARGE_OFF()			USB_OUTPUT_H();\
								CHARGE_CE_PIN_H();\
								CHARGE_PSEL_PIN_H();\
								CHARGE_OTG_PIN_L()
								
#define CHARGE_ON()				USB_OUTPUT_L();\
								CHARGE_CE_PIN_L();\
								CHARGE_PSEL_PIN_L();\
								CHARGE_OTG_PIN_H()								

#define POWER_ON()             GPIO_SetBits(PW_KILL_PORT, PW_KILL_PIN)
#define POWER_OFF()            GPIO_ResetBits(PW_KILL_PORT, PW_KILL_PIN)


#define PW_STATE_ON            0
#define GET_PW_STATE()         GPIO_ReadInputDataBit(PW_STATE_PORT, PW_STATE_PIN)

#define GET_PW_INT_STATE()     GPIO_ReadInputDataBit(PW_INT_PORT, PW_INT_PIN)


#define USB_NOT_CHARGE           0
#define GET_CHARGE_IN_STATE()    GPIO_ReadInputDataBit(USB_GET_IN_PORT, USB_GET_IN_PIN)




extern void DeviceChargeConfig(void);
extern ErrorStatus CheckDeviceChargePowerOnState(void);
extern void ChargerRegInit(void);
extern ErrorStatus GetChargeHardwareState(void);
extern u8 CheckChargeState(void);
extern u8 CheckChargeStatusFromStatusPin(void);
#endif
