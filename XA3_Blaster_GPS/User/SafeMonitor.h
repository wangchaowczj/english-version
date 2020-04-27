#ifndef _SAFEMONITOR_H_
#define _SAFEMONITOR_H_

extern STR_COMM SendBuffer, ReceiceBuffer, TempBuffer;

extern ErrorStatus WireleTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out, ErrorStatus WifiFlag);
extern u8 WireleGeneralTransferData(ErrorStatus WifiFlag);
extern ErrorStatus SendDeviceStartData(ErrorStatus WifiFlag);
extern u8 SendDeviceArgumentData(ErrorStatus WifiFlag);
extern u8 SendBlastFireInfoData(u16 index, ErrorStatus WifiFlag);
extern void ExitSafeMonitor(void);
extern ErrorStatus GetBlastRecoderUploadState(u16 Index);
extern ErrorStatus GetServerCurrentTime(ErrorStatus WifiFlag, u8* ServerTimePtr);

#endif
