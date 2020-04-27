#include "includes.h"
#include "GuiIncludes.h"


/**
  *************************************************************************************
  * @brief  获取上报日志文件的第一包数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2018年05月10日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetLogDataFirstPack(u8* Ptr, u8 LogIndex)
{
	u16 index = 0;
	
	/*
		控制器向服务器发送：文件名称+0x0D+文件大小+0x0D
	服务器向控制器返回："1"
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x19;//命令码低字节

	*(Ptr+index++) = 0x00;//分包包号高字节,第1包
	*(Ptr+index++) = 0x00;//分包包号低字节

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
	
	/***文件名称***/
	memcpy((char*)(Ptr+index), &LogRecorder.FileName[LogIndex][0], sizeof(LogRecorder.FileName[0]));
	index += sizeof(LogRecorder.FileName[0]);
	
	strcpy((char*)(Ptr+index), ".txt");
	index += strlen(".txt");
	*(Ptr+index++) = 0x0D;//分隔符

	/***文件大小***/
	index += sprintf((char*)(Ptr+index),"%d\r", GetLogFileSize(LogIndex));
					
	return index;
}
/**
  *************************************************************************************
  * @brief  获取上报日志文件的结束包数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2018年05月10日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetLogDataPack(u8* Ptr, u16 PackIndex)
{
	u16 index = 0;
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x19;//命令码低字节

	*(Ptr+index++) = (PackIndex>>8)&0xFF;//分包包号高字节,第1包
	*(Ptr+index++) = PackIndex&0xFF;//分包包号低字节
					
	return index;
}

/**
  *************************************************************************************
  * @brief  获取上报日志文件的结束包数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2018年05月10日
  * @note   无
  *************************************************************************************  
  */ 
u16 GetLogDataEndPack(u8* Ptr, u8 LogIndex)
{
	u16 index = 0;
	
	/*
		控制器向服务器发送：文件名称+0x0D+文件大小+0x0D
	服务器向控制器返回："1"
	*/
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x19;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节,第1包
	*(Ptr+index++) = 0xFF;//分包包号低字节

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
	
	/***文件名称***/
	memcpy((char*)(Ptr+index), &LogRecorder.FileName[LogIndex][0], sizeof(LogRecorder.FileName[0]));
	index += sizeof(LogRecorder.FileName[0]);
	
	strcpy((char*)(Ptr+index), ".txt");
	index += strlen(".txt");
	*(Ptr+index++) = 0x0D;//分隔符

	/***文件大小***/
	index += sprintf((char*)(Ptr+index),"%d\r", GetLogFileSize(LogIndex));
					
	return index;
}
/**
  *************************************************************************************
  * @brief  发送日志文件数据到服务器中
  * @param  index 日志文件索引
  * @retval 发送结果
  *   @arg @b 0x31 成功
  *   @arg @b 其他   失败  
  * @author ZH
  * @date   2018年05月10日
  * @note   在上传过程中禁止写日志
  *************************************************************************************  
  */
u8 SendLogFileToServer(u16 index, ErrorStatus WifiFlag)
{
	const u16 ItemsMax = 400;
	u8 Result;
	WM_MESSAGE Msg;
	u16 SendBytes;
	u16 SendCount = 0;
	u32 fileSize;
	u32 sumBytes = 0;

	Msg.MsgId = WM_UPDATE_PROGBAR;	
	Msg.Data.v = 0;
	
	fileSize = GetLogFileSize(index);
	if(fileSize == 0)
	{
		return 0xFF;
	}
	if(WM_IsWindow(DlghWin.Dlg))
	{
		WM_SendMessage(DlghWin.Dlg, &Msg);
	}	
	//发送第1包数据
	SendBuffer.bytenum = GetLogDataFirstPack(SendBuffer.uch_buf, index);//获取起爆记录数据第1包的数据
	Result = WireleGeneralTransferData(WifiFlag);	
	if(Result != '1')
	{
		return Result;
	}
	SendBytes = ItemsMax;//默认每次发400字节
	while(1)
	{
		SendBuffer.bytenum = GetLogDataPack(SendBuffer.uch_buf, SendCount+1);//从第2包开始发送雷管信息数据
				
		if ((SendCount*ItemsMax + SendBytes) > fileSize)
		{
			SendBytes = fileSize%ItemsMax;
		}
		
		if(ERROR == GetLogRecorderData(index, SendCount*ItemsMax, SendBytes, &SendBuffer.uch_buf[SendBuffer.bytenum]))
		{
			return 0xFF;
		}
		SendBuffer.bytenum += SendBytes;
				
		Result = WireleGeneralTransferData(WifiFlag);	
		if(Result != '1')
		{
			return Result;
		}
		sumBytes = SendCount*ItemsMax + SendBytes;
		if (sumBytes >= fileSize)
		{//已经全部发送完成
			break;
		}
		SendCount++;//发送下一包
		
		if(WM_IsWindow(DlghWin.Dlg))
		{
			Msg.Data.v = (double)sumBytes/fileSize*10000;
			if(Msg.Data.v > 10000)
			{
				Msg.Data.v = 10000;
			}
			WM_SendMessage(DlghWin.Dlg, &Msg);
		}
	}

	//发送结束包标志
	SendBuffer.bytenum = GetLogDataEndPack(SendBuffer.uch_buf, index);
	Result = WireleGeneralTransferData(WifiFlag);
	
	return Result;
}

