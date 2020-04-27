#ifndef _DKEY_H_
#define _DKEY_H_

extern ErrorStatus GetDkeyInfo(void);
extern u8 DkeyUserInfoVerify(u8* UserName, u8* Password, u8* State);
extern u8 DkeyModifyUserInfo(u8* UserName, u8* OldPassword, u8* NewPassword, u8* State);
extern u8 DkeyBlastInfoVerify(u16* DetErrIndex);
extern u8 DkeyReadDetCount(u16* DetCount);
extern u8 DkeyUpdateDetCount(u16 DetNumber);
extern u8 DkeyReadDetInfoBackUpDataBytes(u32* Bytes);
extern u8 DkeyReadDetInfoBackUp(void);
extern ErrorStatus DkeyDeleteDetInfoBackUpData(void);
extern ErrorStatus DkeyDetInfoBackUp(void);
extern ErrorStatus DkeyBlastRecorderBackUp(void);
#endif
