#ifndef _MIDFUNCTION_H_
#define _MIDFUNCTION_H_

#include "BarCode.h"

extern u32 U32DataCMP(const u32* ptr, u8 length);
extern u16 U16DataCMP(const u16* ptr, u8 length);
extern u16 GetU16DataMax(const u16* ptr, u8 length);
extern u16 GetU16DataMin(const u16* ptr, u8 length);
extern u32 GetU32DataMax(const u32* ptr, u8 length);
extern u32 GetU32DataMin(const u32* ptr, u8 length);
extern u16 GetU16DataAverage(u16* ptr, u16 lenght);
extern void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght);
extern void RemoveCharFromString(u8*ptr, u8 char_val, u16 len);
extern u16 GetCharIndexFromString(u8*ptr, u8 char_val, u16 len);
extern u8 HexCharToValue(u8 c);
extern void HexStrToCharStr(u8* ptr_in, u16 in_len, u8* ptr_out);
extern ErrorStatus CheckNumberChars(char* Str, u8 FloatFlag, u16 Len);
extern u8 GetDetTypeForXA(const char* DetTypeString);
extern const char* GetDetTypeStringForXA(u8 DetType);
extern u8 VerifyDetCode(void);
extern u8 NeedScan(void);

extern ErrorStatus CheckDetCurrentForDetAutoTest(void);
extern ErrorStatus CheckDetOff(void);

extern void UsbAutoIdentify(void);
extern u16 GetMaxDelay(u8 DetType);
extern u8 AllChannelBarCodeHasData(void);
extern u8 GetChannelBarCodeEmptyIndex(void);
extern u8 GetBarCodeSameIndex(BARCODE* BarCode);
extern u8 GetChannelOldStatusErrorCount(void);
extern u8 GetChannelOldStatusErrorIndex(u8 Order);
extern void SaveChannelOldStatus(void);
extern void ClearChannelOldStatus(void);
extern void UpdateDetDetectorPassCount(void);
extern ErrorStatus GetCurrentDetOutsideStart(u8* DetOutside);
extern void CreateChannelDetOutside(void);
extern ErrorStatus DetOutsideVerify(u8* Ptr);
extern u16 GetBoxNumberFromDetOutside(u8* DetOutside);
extern u8 GetInBoxIndexFromDetOutside(u8* DetOutside);
extern u32 GetBoxIndexFromDetOutside(u8* DetOutside);

#endif
