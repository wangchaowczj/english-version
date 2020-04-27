#ifndef _DKEY_H_
#define _DKEY_H_

extern STR_COMM BlueTempBuffer;

extern ErrorStatus GetBlueDkeySoftwareVersion(u8* StrPtr, u8 MaxBytes);
extern ErrorStatus GetBlueDkeyId(u8* StrPtr, u8 MaxBytes);
extern ErrorStatus GetBlueDkeyInfo(void);
extern u8 BlueDkeyUserInfoVerify(u8* UserName, u8* Password, u8* State);
extern u8 BlueDkeyModifyUserInfo(u8* OldUserName, u8* OldPassword, u8* NewUserName, u8* NewPassword, u8* State);
extern ErrorStatus GetBlueDkeyBatVoltage(u16* BatVoltage);
extern u8 GetBlueDkeyRtc(RTC_Typedef* rtc);
extern u8 GetBlueDkeyGPS(GPS_INFO* gps);
extern ErrorStatus SetRtcByBlueDkey(void);
extern u8 BlueDkeyGpsIsPass(GPS_INFO* gps);

#endif
