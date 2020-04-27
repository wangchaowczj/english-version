#ifndef _SOFTWAREUPDATE_H_
#define _SOFTWAREUPDATE_H_

#define SOFTWARE_UPDATE_FILE_PATH   "0:固件远程更新暂存"


typedef struct
{	
	u32 NewFileSize ;
	u8 NewVersion[40];
	u32 CurrentFileSize ;
}HEX_LOADER;
extern HEX_LOADER HexLoader;

extern u8 DeviceHexFileLoader(ErrorStatus WifiFlag);
extern u8 GetServerDeviceVersionInfo(u32* FileSize, u8* VersionPtr, u8 BufferSize, ErrorStatus WifiFlag);

#endif
