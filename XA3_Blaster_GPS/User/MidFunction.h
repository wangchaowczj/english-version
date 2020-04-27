#ifndef _MIDFUNCTION_H_
#define _MIDFUNCTION_H_

extern void LcdBrightInitOS(void);
extern u16 GetUtf8StrCharsCount(const char* Str);
extern u32 U32DataCMP(const u32* ptr, u8 length);
extern u16 U16DataCMP(const u16* ptr, u8 length);
extern u16 GetU16DataMax(const u16* ptr, u8 length);
extern u16 GetU16DataMin(const u16* ptr, u8 length);
extern u16 GetU16DataAverage(u16* ptr, u16 lenght);
extern void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght);
extern void RemoveCharFromString(u8*ptr, u8 char_val, u16 len);
extern u8 IsHexChar(u8 c);
extern u8 HexCharToValue(u8 c);
extern void HexStrToCharStr(u8* ptr_in, u16 in_len, u8* ptr_out);
extern ErrorStatus CheckNumberChars(char* Str, u8 FloatFlag);
extern void U32ToAscii(u32 value, u8* str);
extern u32 AsciiToU32(u8* str);
extern u16 FindUnregHole(u16 current, u8 dir);
extern u16 GetErrorDetIndex(u16 err_index);
extern u16 GetDetMaxDelay(u8 dettype);
extern u16 CheckDetDelayIsSame(void);
extern void AllRegDetRankBaseOnHoleInfo(void);
extern void AllRegDetRankBaseOnDelay(void);
//extern void AllRegDetRankBaseOnDetInfo(void);
extern u16 GetDetIndexFromHoleInfo(HOLE* Hole);
extern u16 GetNearbyDetIndexFromHoleInfo(HOLE* Hole);
extern u16 GetEmptyDetIndex(void);
extern u16 GetRegDetHoleMax(void);
extern ErrorStatus ValidateDetType(void);
extern void ValidateDetTypeAndDelay(void);
extern u16 CalcRegHoleDelayFromNetParam(u16 Index);
extern ErrorStatus DelayParamVerify(HOLE* StartHole, HOLE* EndHole, u16 StartDly, u16 SpacesDly, u16 InsideSpacesDly);
extern ErrorStatus DelayModifyBaseOnParam(HOLE* StartHole, HOLE* EndHole, u16 StartDly, u16 SpacesDly, u16 InsideSpacesDly);
extern u16 GetRegHoleMinIndexFromHoleRange(HOLE* StartHole, HOLE* EndHole);
extern u8 BusStateMonitor(u16 Voltage, u16 Current);
extern ErrorStatus WaiLinCurrentSteady(u32 TimeOut);

extern ErrorStatus GetCurrentHoleMaxMinRegInside(u16 HoleName, u8* MaxInside, u8* MinInside);

extern u16 GetRegDetIndexFirst(u16 Start);
extern u16 RegHoleInit(void);
extern ErrorStatus GetSmallRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Small);
extern ErrorStatus GetBigRegHoleInfoBaseOnCurrent(HOLE* Current, HOLE* Big);
extern u8 GetCurrentHoleNameDetCount(u16 HoleName);
extern u16 GetHoleNameFristInsideDelay(u16 HoleName);
extern u16 GetCountSpecifiedHoleRange(HOLE* StartHole, HOLE* EndHole);
extern u16 GetHoleIndexSpecifiedStartHole(HOLE* StartHole, u16 Pos);
extern ErrorStatus DeviceSelfTest(u8 Type);
extern ErrorStatus BusStatusIsIdle(void);
extern void LcdBrightRefreshProcess(u8 RefreshFlag);
extern void ClearProgbarData(void);
extern void FillCurrentProgbar(void);
extern void SetRegDetDelay(u16 Delay);
extern u8 DetInfoHasSame(void);
extern ErrorStatus BlastSingleSelfTest(u8 Slave);
extern ErrorStatus BlastSingleSelfTest(u8 Slave);
extern void SetNormalHighVoltageGoal(void);
extern u8 BlasterIs36(void);
extern u8 BlasterIs04(void);
extern u8 GetBlasterFactoryCode(void);
extern u8 DetFactoryCodeIsRulePass(u32 Serl, u32 Pwd);
extern u8 AllDetFactoryCodeIsRulePass(void);
extern u16 GetDetOnlineCount(void);
extern u8 DetOfflinePossible(u16 I_BeforeFire);
extern u8 HasDivideRegHole(void);
extern void DetCode2UIDStr(U32_UNION* Serl, U32_UNION* Pwd, u8* UIDStr);
extern u16 AllDetAccreditForDL(void);
extern u8 DetIsAccreditPassForDL(U32_UNION* Serl, U32_UNION* Pwd, u8* ErrorCode);
extern char* GetBlasterPath(void);
extern u8 GPSInfoTimeIsPass(GPS_INFO* Gps, RTC_Typedef* rtc);
extern u8 FormatForDetInfo(u8* Version);
extern u8 FormatIsSameForDetInfo(u8* Version);
extern u8 CheckHardwareVersion(void);
extern void GetSTM32ChipID(u8* Id);
extern void GetSTM32ChipIDStr(u8* IdStr);

#endif
