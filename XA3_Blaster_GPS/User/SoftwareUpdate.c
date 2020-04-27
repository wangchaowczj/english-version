#include "includes.h"
#include "GuiIncludes.h"

HEX_LOADER HexLoader;


/**
  *************************************************************************************
  * @brief  获取请求服务器中的固件版本的数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2015年01月27日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetServerDeviceVersionInfoData(u8* Ptr)
{
	u16 index = 0;
	
	/*
		控制器向服务器发送：1+0x0d控制器ID号+0x0D+设备当前软件版本+0x0D+设备硬件版本+0x0D
	服务器向控制器返回：
	(1)	不需要升级： “1”
	(2)	需要升级：“2”+“0x0D”+文件大小+“0x0D”+版本号+“0x0D”
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x14;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节,第1包
	*(Ptr+index++) = 0xFF;//分包包号低字节

	*(Ptr+index++) = '1';
	*(Ptr+index++) = 0x0D;

	/***控制器ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//分隔符

	/***软件版本***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;	
	
	/***硬件版本***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.hw_version);
	index += strlen((const char*)device_info.hw_version);
	*(Ptr+index++) = 0x0D;//分隔符	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  获取设置服务器准备更新固件的数据
  * @param  Ptr 数据缓冲区首地址
  * @param  NewVersion 新固件版本  
  * @retval 数据长度
  * @author ZH
  * @date   2015年01月27日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetSetServerReadyForUpdateData(u8* Ptr, u8* NewVersion)
{
	u16 index = 0;
	
	/*
			控制器向服务器发送：2+0x0D+控制器ID号+0x0D+设备当前软件版本+0x0D+设备硬件版本+0x0D+即将升级的软件版本+0x0D
			服务器向控制器返回：
			(1)	成功： “1”
			(2)	失败：“2”
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x14;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节,第1包
	*(Ptr+index++) = 0xFF;//分包包号低字节

	*(Ptr+index++) = '2';
	*(Ptr+index++) = 0x0D;

	/***控制器ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//分隔符

	/***软件版本***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;	
	
	/***硬件版本***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.hw_version);
	index += strlen((const char*)device_info.hw_version);
	*(Ptr+index++) = 0x0D;//分隔符	

	/***即将升级的软件版本***/	
	strcpy((char*)(Ptr+index), (const char*)NewVersion);
	index += strlen((const char*)NewVersion);
	*(Ptr+index++) = 0x0D;//分隔符	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  获取服务器固件文件数据
  * @param  Ptr 数据缓冲区首地址
  * @param  Pack 数据包号
  * @param  CurrentSize 已接收数据长度
  * @param  NextSize 本次要获取的数据长度  
  * @retval 数据长度
  * @author ZH
  * @date   2015年01月27日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetServerHexFileDataData(u8* Ptr, u16 PackCount, u32 CurrentSize, u16 NextSize)
{
	u16 index = 0;
	
	/*
	控制器向服务器发送：3+0x0D+包号+0x0D+已接收数据长度+0x0D+需要获取的升级数据字节长度（可以根据gprs模块的信号强度来动态控制升级数据的包长度）+0x0D
	服务器向控制器返回：
	包号（两个字节无符号数）+升级数据		
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x14;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节
	*(Ptr+index++) = 0xFF;//分包包号低字节

	*(Ptr+index++) = '3';
	*(Ptr+index++) = 0x0D;

	index += sprintf((char*)(Ptr+index), "%u\r", PackCount);//包号

	index += sprintf((char*)(Ptr+index), "%ld\r", CurrentSize);
	index += sprintf((char*)(Ptr+index), "%ld\r", NextSize);
	
	return index;
}

/**
  *************************************************************************************
  * @brief  获取服务器固件文件数据校验结果数据
  * @param  Ptr 数据缓冲区首地址
  * @param  VerifyValue 固件文件数据校验结果
  * @param  NewVersion 固件版本  
  * @retval 数据长度
  * @author ZH
  * @date   2015年01月27日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetServerHexFileVerifyResultData(u8* Ptr, u16 VerifyValue, u8* NewVersion)
{
	u16 index = 0;
	
	/*
	校验升级文件控制器向服务器发送：4+0x0D+校验码（暂定crc16）+0x0D+要校验的升级文件版本+0x0D
	服务器向控制器返回：
	(1)	成功： “1”
	(2)	失败：“2”		
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x14;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节,第1包
	*(Ptr+index++) = 0xFF;//分包包号低字节

	*(Ptr+index++) = '4';
	*(Ptr+index++) = 0x0D;

	index += sprintf((char*)(Ptr+index), "%u\r", VerifyValue);
	
	/***即将升级的软件版本***/	
	strcpy((char*)(Ptr+index), (const char*)NewVersion);
	index += strlen((const char*)NewVersion);
	*(Ptr+index++) = 0x0D;//分隔符	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  获取服务器中的固件信息
  * @param  FileSize 文件大小
  * @param  VersionPtr 固件版本
  * @param  BufferSize VersionPtr缓冲区大小
  * @param  WifiFlag WIFI是否可用  
  * @retval 发送结果
  *   @arg @b 0x00 通信异常
  *   @arg @b 0x01 不需要更新
  *   @arg @b 0x02 需要更新
  * @author ZH
  * @date   2015年01月27日
  * @note   
  *************************************************************************************  
  */
u8 GetServerDeviceVersionInfo(u32* FileSize, u8* VersionPtr, u8 BufferSize, ErrorStatus WifiFlag)
{	
	u16 StartIndex = 0;
	u16 Bytes = 0;
	
	SendBuffer.bytenum = GetServerDeviceVersionInfoData(SendBuffer.uch_buf);
	Pack(&SendBuffer, &TempBuffer);//数据打包
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if((TempBuffer.bytenum == 7))
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x14) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '1')  && (TempBuffer.uch_buf[6] == 0x0D))
				{
					return 0x01;//不需要升级			
				}			
			}
			else if(TempBuffer.bytenum > 7) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x14) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '2')  && (TempBuffer.uch_buf[6] == 0x0D) &&
					(TempBuffer.uch_buf[TempBuffer.bytenum-1] == 0x0D))
				{
					if(ERROR != GetDecValueFromSeparator('\r', 2, &TempBuffer.uch_buf[5],TempBuffer.bytenum-5,FileSize))
					{
						if(ERROR != GetDataFromSeparator('\r', 3, &TempBuffer.uch_buf[5],TempBuffer.bytenum-5, &StartIndex, &Bytes))
						{
							if(BufferSize > Bytes)
							{
								if(VersionPtr != NULL)
								{
									memcpy(VersionPtr, &TempBuffer.uch_buf[5+StartIndex], Bytes);
									*(VersionPtr+Bytes) = 0x00;								
								}
								return 0x02;//需要升级
							}								
						}									
					}
				}				
			}
		}
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  通知服务器准备开始升级固件
  * @param  NewVersion 新固件版本号
  * @param  WifiFlag WIFI是否可用  
  * @retval 发送结果
  *   @arg @b SUCCESS 成功
  *   @arg @b ERROR 失败
  * @author ZH
  * @date   2015年01月27日
  * @note   
  *************************************************************************************  
  */
ErrorStatus SetServerReadyForUpdate(u8* NewVersion, ErrorStatus WifiFlag)
{
	u8 Result = 0;
	
	SendBuffer.bytenum = GetSetServerReadyForUpdateData(SendBuffer.uch_buf, NewVersion);

	Result = WireleGeneralTransferData(WifiFlag);
	if('1' == Result)
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;//发送失败
	}	
}

/**
  *************************************************************************************
  * @brief  获取服务器中的HEX文件数据
  * @param  CurrentSize 已接收数据长度
  * @param  NextSize 本次要获取的数据长度 
  * @param  WifiFlag WIFI是否可用  
  * @retval 发送结果
  *   @arg @b SUCCESS 成功
  *   @arg @b ERROR 失败
  * @author ZH
  * @date   2015年01月27日
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetServerHexFileData(u16 PackCount, u32 CurrentSize, u16 NextSize, u8* FileData, ErrorStatus WifiFlag)
{
	SendBuffer.bytenum = GetServerHexFileDataData(SendBuffer.uch_buf, PackCount, CurrentSize, NextSize);
	
	Pack(&SendBuffer,&TempBuffer);//数据打包
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == (NextSize+7))
			{
				if((TempBuffer.uch_buf[1] == SendBuffer.uch_buf[1])  && (TempBuffer.uch_buf[2] == SendBuffer.uch_buf[2]) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == (PackCount>>8))  && (TempBuffer.uch_buf[6] == (PackCount&0xFF)))
				{
					memcpy(FileData, &TempBuffer.uch_buf[7], NextSize);
					return SUCCESS;
				}			
			}
		}
	}		
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  获取服务器中的HEX文件校验结果数据
  * @param  VerifyValue 校验值
  * @param  NewVersion 要校验的文件版本号
  * @param  WifiFlag WIFI是否可用  
  * @retval 发送结果
  *   @arg @b SUCCESS 成功
  *   @arg @b ERROR 失败
  * @author ZH
  * @date   2015年01月27日
  * @note   
  *************************************************************************************  
  */
u8 GetServerHexFileVerifyResult(u16 VerifyValue, u8* NewVersion, ErrorStatus WifiFlag)
{
	u8 Result = 0;
	
	SendBuffer.bytenum = GetServerHexFileVerifyResultData(SendBuffer.uch_buf, VerifyValue, NewVersion);
	Result = WireleGeneralTransferData(WifiFlag);

	return Result;
}

/**
  *************************************************************************************
  * @brief  从服务器中下载固件
  * @param  无
  * @retval 结果
  *   @arg @b 0 通信异常
  *   @arg @b 1 下载成功
  *   @arg @b 2 不需要升级 
  *   @arg @b 3 文件校验不通过
  *   @arg @b 4 文件写入失败  
  * @author ZH
  * @date   2015年01月27日
  * @note   
  *************************************************************************************  
  */
u8 DeviceHexFileLoader(ErrorStatus WifiFlag)
{
	u8 U8Value;
	u16 Bytes;
	u16 PackCount = 0;
	WM_MESSAGE Msg;
	static u8 temp[400];
	u16 CRC16 = 0;
	
	memset((u8*)&HexLoader, 0, sizeof(HEX_LOADER));
	
	if(ERROR != GetSpecifiedFileExistState(NULL, "0:CPU.hex"))
	{//已经下载完成并移到根目录下
		return 1;
	}
	
	Msg.MsgId = WM_SOFTWARE_UPDATE;	
	Msg.Data.v = 1;
	
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
	
	//获取服务器中的固件信息
	U8Value = GetServerDeviceVersionInfo(&HexLoader.NewFileSize, &HexLoader.NewVersion[0], sizeof(HexLoader.NewVersion), WifiFlag);
	if(U8Value == 1)
	{//不需要升级
		return 2;
	}
	else if(U8Value != 2)
	{
		return 0;//异常
	}
	if ((HexLoader.NewFileSize < 1024) || (HexLoader.NewFileSize > 5242880UL))
	{//正常的文件不可能小于1K也不可能大于5M
		return 0;
	}
	//通知服务器准备开始升级固件
	if(ERROR == SetServerReadyForUpdate(&HexLoader.NewVersion[0], WifiFlag))
	{
		return 0;//异常
	}
	
	//读当前文件大小
	sprintf((char*)temp,"%s.hex",&HexLoader.NewVersion[0]);
	DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, (char*)temp);
	
	//读当前文件大小
	if(ERROR == ReadSpecifiedHexFileSize(SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0],&HexLoader.CurrentFileSize))
	{//没有对应的文件
		HexLoader.CurrentFileSize = 0;
	}
	if ((HexLoader.CurrentFileSize > 5242880UL) || (HexLoader.CurrentFileSize > HexLoader.NewFileSize))
	{//正常的文件不可能大于5M
		DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, NULL);
		HexLoader.CurrentFileSize = 0;
	}

	Msg.Data.v = 2;//显示当前进度
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}
		
	//获取服务器中的HEX文件数据
	while(HexLoader.CurrentFileSize < HexLoader.NewFileSize)
	{
		//每次最多读500字节
		if ((HexLoader.CurrentFileSize + sizeof(temp)) <= HexLoader.NewFileSize)
		{
			Bytes = sizeof(temp);
		}
		else
		{
			Bytes = HexLoader.NewFileSize - HexLoader.CurrentFileSize;
		}
		
		if(ERROR == GetServerHexFileData(PackCount++, HexLoader.CurrentFileSize, Bytes, temp, WifiFlag))
		{
			return 0;//异常
		}
				
		HexLoader.CurrentFileSize += Bytes;
		Msg.Data.v = 2;//显示当前进度
		if(WM_IsWindow(DlghWin.Dlg))
		{
			WM_SendMessage(DlghWin.Dlg, &Msg);
		}
		
		
		//保存下载到的数据
		if(ERROR == WriteHexFileData(SOFTWARE_UPDATE_FILE_PATH,&HexLoader.NewVersion[0],&temp[0], Bytes))
		{
			return 4;
		}		
	}
	
	Msg.Data.v = 3;//计算校验值
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}	
	if(ERROR == GetSpecifiedHexFileCRCValue(SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0], &CRC16))
	{
		return 4;
	}

	//获取服务器中的HEX文件校验结果数据
	U8Value = GetServerHexFileVerifyResult(CRC16, &HexLoader.NewVersion[0], WifiFlag);
	if('1' == U8Value)
	{//	
		//文件下载成功
		sprintf((char*)temp,"%s/%s.hex", SOFTWARE_UPDATE_FILE_PATH, &HexLoader.NewVersion[0]);
		OSSchedLock();
		f_rename((const char*)temp, "0:CPU.hex");
		OSSchedUnlock();
		return 1;//更新成功
	}
	else if('2' == U8Value)
	{
		DeleteAllFileSpecifiedPath(SOFTWARE_UPDATE_FILE_PATH, NULL);
		return 3;//校验失败
	}
	return 0;
}
