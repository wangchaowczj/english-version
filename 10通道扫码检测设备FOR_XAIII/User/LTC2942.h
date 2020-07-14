#ifndef _LTC2942_H_
#define _LTC2942_H_



#define LTC2942_ADDR   0xC8

#define LI_BAT_CHARGE_VOLTAGE_MAX     4080 //充电时电压会比实际要高,充满时基本一样

#define LI_BAT_CHARGE_COMPLETE_VALUE  58981//90%

extern ErrorStatus LTC2942Init(void);
extern u16 LTC2942BatVoltage(void);
extern u16 LTC2942BatAccumulated(void);
extern u16 LTC2942BatAccumulatedValue(void);
extern ErrorStatus LTC2942UpdateBatAccumulated(u16 Value);
extern ErrorStatus LTC2942ChargeCompleteUpdate(void);
extern ErrorStatus LTC2942BatAccumulatedAdjust(void);
#endif
