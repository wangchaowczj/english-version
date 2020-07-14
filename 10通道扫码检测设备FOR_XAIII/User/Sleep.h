#ifndef _SLEEP_H_
#define _SLEEP_H_

#define SLEEP_WAKE_PORT				GPIOC
#define SLEEP_WAKE_PORT_RCC			RCC_AHB1Periph_GPIOC
#define SLEEP_WAKE_PIN				GPIO_Pin_5

#define SYSTEM_ENTER_SLEEP_MODE_COUNT   600000UL  //10分钟
#define SYSTEM_ENTER_POWEROFF_MODE_COUNT   1200UL  //20分钟，秒为单位

extern u32 GetSystemSleepCount(void);
extern void SetSystemSleepCount(u32 NewCount);
extern u8 SystemSleepCountIsTimeout(u32 CurrentCount);
extern void SetSystemSleepDisable(void);
extern void SetSystemSleepEnable(void);
extern u8 SystemSleepIsEnable(void);

extern void SystemSleepModeConfig(void);
extern void SystemEnterSleepMode(void);

#endif
