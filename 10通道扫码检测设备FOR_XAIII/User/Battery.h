#ifndef _BATTERY_H_
#define _BATTERY_H_

#define BATTERY_VOLTAGE_BUFFER_SIZE  5
typedef struct
{
	u16 Voltage[BATTERY_VOLTAGE_BUFFER_SIZE];
	u8 Index;
	u16 DisValue;
	u32 Timer;	
}BATTERY;

extern u16 GetBatteryCapacityByVoltage(u16 Voltage);
extern u16 InitBatteryDisplayValue(u16 Voltage);
extern u16 GetBatteryDisplayValue(u16 Voltage);

#endif
