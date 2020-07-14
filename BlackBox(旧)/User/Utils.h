#ifndef _UTILS_H_
#define _UTILS_H_

extern u32 U32DataCMP(const u32* ptr, u8 length);
extern u16 U16DataCMP(const u16* ptr, u8 length);
extern u16 GetU16DataMax(const u16* ptr, u8 length);
extern u16 GetU16DataMin(const u16* ptr, u8 length);
extern u16 GetU16DataAverage(u16* ptr, u16 lenght);
extern void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght);

extern ErrorStatus GetDetCmdNameString(u8 Cmd, char* Ptr);

#endif
