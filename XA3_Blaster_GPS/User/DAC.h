#ifndef _DAC_H_
#define _DAC_H_

#define DAC_HV_PORT          GPIOA
#define DAC_HV_PORT_RCC      RCC_AHB1Periph_GPIOA
#define DAC_HV_PIN           GPIO_Pin_4

#define DAC_HV_CHANNEL       DAC_Channel_1

#define DAC_LV_PORT          GPIOA
#define DAC_LV_PORT_RCC      RCC_AHB1Periph_GPIOA
#define DAC_LV_PIN           GPIO_Pin_5

#define DAC_LV_CHANNEL       DAC_Channel_2

extern void DacConfig(void);
extern void SetLVVoltageMin(void);
extern void SetLVVoltageMax(void);
extern ErrorStatus SetLVVoltageUp(u8 DacStep);
extern ErrorStatus SetLVVoltageDown(u8 DacStep);
extern u16 DacOutLVVoltage(u16 Voltage);
extern void SetHVVoltageMin(void);
extern void SetHVVoltageMax(void);
extern ErrorStatus SetHVVoltageUp(u8 DacStep);
extern ErrorStatus SetHVVoltageDown(u8 DacStep);
extern u16 DacOutHVVoltage(u16 Voltage);
extern u16 getDacLvValueMin(void);
extern void setDacLvValueMin(u16 DacValue);
extern void setDacLvValueMinFromVoltage(u16 VoltageMax);
extern u16 getDacHvValueMin(void);
extern void setDacHvValueMin(u16 DacValue);
extern void setDacHvValueMinFromVoltage(u16 VoltageMax);
extern u16 getCurrentHvDacValue(void);

extern void VoltageOutputAdjust(void);

#endif

