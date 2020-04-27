#ifndef _CHARGE_H_
#define _CHARGE_H_


#define PW_INT_PORT            GPIOF
#define PW_INT_PORT_RCC        RCC_AHB1Periph_GPIOF
#define PW_INT_PIN             GPIO_Pin_9

#define PW_KILL_PORT           GPIOD
#define PW_KILL_PORT_RCC       RCC_AHB1Periph_GPIOD
#define PW_KILL_PIN            GPIO_Pin_6

#define CHARGE_STAT_PORT       GPIOI
#define CHARGE_STAT_PORT_RCC   RCC_AHB1Periph_GPIOI
#define CHARGE_STAT_PIN        GPIO_Pin_10								

#define PW_STATE_PORT      	   GPIOG
#define PW_STATE_PORT_RCC      RCC_AHB1Periph_GPIOG
#define PW_STATE_PIN           GPIO_Pin_10

#define POWER_ON()             GPIO_SetBits(PW_KILL_PORT, PW_KILL_PIN)
#define POWER_OFF()            GPIO_ResetBits(PW_KILL_PORT, PW_KILL_PIN)


#define PW_STATE_ON            0
#define GET_PW_STATE()         GPIO_ReadInputDataBit(PW_STATE_PORT, PW_STATE_PIN)

#define GET_PW_INT_STATE()     GPIO_ReadInputDataBit(PW_INT_PORT, PW_INT_PIN)

#define CHARGING          0
#define GET_CHARGE_SIGNAL_STATE()     GPIO_ReadInputDataBit(CHARGE_STAT_PORT, CHARGE_STAT_PIN)

extern void DeviceChargeConfig(void);
extern ErrorStatus CheckDeviceChargePowerOnState(void);
extern u8 CheckChargeState(void);
#endif
