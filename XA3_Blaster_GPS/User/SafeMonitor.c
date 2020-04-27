#include "includes.h"

STR_COMM SendBuffer, ReceiceBuffer, TempBuffer;
extern void UpdateProgbar(u16 Value);
/**
  *************************************************************************************
  * @brief  获取设备开机时上传安管系统的数据(协议中的心跳数据)
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   无
  *************************************************************************************  
  */  
u16 GetDeviceStartData(u8* Ptr)
{
//	u16 index = 0;
//	u16 Value;

//	/*
//	控制器向服务器发送：
//	（1）当经纬度坐标没有定位成功时：控制器ID号 +0x0D+0x02+ 0x0D+设备版本+0x0D+设备电量+0x0D
//	（2）当经纬度坐标定位成功时：控制器ID号 +0x0D+经纬度+ 0x0D+设备版本+0x0D+设备电量+0x0D
//	服务器返回：
//	（1）	需加密的数据包：XX+0x0D
//	XX-成功0x01 失败:错误码
//	*/	
//	
//	*(Ptr+index++) = 0x41;//协议地址
//	*(Ptr+index++) = 0x00;//命令码高字节
//	*(Ptr+index++) = 0x17;//命令码低字节
//	
//	*(Ptr+index++) = 0xFF;//分包包号高字节，不需要分包
//	*(Ptr+index++) = 0xFF;//分包包号低字节，不需要分包
//	
//	/***控制器ID***/
//	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
//	index += strlen((const char*)device_info.mac);
//	*(Ptr+index++) = 0x0D;//分隔符
//	
//	/***经纬度,格式为"11620.1092E,3957.7800N"***/
//	if(GPS_SUCESS != Op_Data.Gps.GPS_state)
//	{
//		strcpy((char*)(Ptr+index), "0x02");
//		index += strlen("0x02");
//	}
//	else
//	{			
//		memcpy(Ptr+index, Op_Data.Gps.GPS_Longitude, 10);//经度，数据格式不一样
//		index += 10;
//		*(Ptr+index++) = Op_Data.Gps.GPS_Longitude[11];
//		*(Ptr+index++) = ',';
//		memcpy(Ptr+index, Op_Data.Gps.GPS_Latitude, 9);//纬度，数据格式不一样
//		index += 9;	
//		*(Ptr+index++) = Op_Data.Gps.GPS_Latitude[10];	
//	}
//	*(Ptr+index++) = 0x0D;
//	
//	/***软件版本***/
//	strcpy((char*)(Ptr+index), VERSION_STRING);
//	index += strlen(VERSION_STRING);
//	*(Ptr+index++) = 0x0D;
//	
//	/***设备电量***/
//	Value = Op_Data.BatteryVoltage;
//	index += sprintf((char*)(Ptr+index), "%d", Value);
//	
//	*(Ptr+index++) = 0x0D;
//	
//	return index;
return 0;
}

/**
  *************************************************************************************
  * @brief  获取设备远程论证数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月25日
  * @note   无
  *************************************************************************************  
  */  
u16 GetDeviceArgumentData(u8* Ptr)
{
	u16 index = 0;

	/*
	控制器向服务器发送：
	授权ID号（对应详细地址）+0x0D+控制器ID号+ 0x0D + 密钥ID号+ 0x0D
	服务器返回：
	XX+0x0D+YY+0x0D+ZZ+0x0D+KK+0x0D
	X 成功的标示，成功：0x01，失败：错误码。
	YY是授权ID有效性 ZZ是控制器有效性 KK密钥有效性。
	有效：”1”，无效：”2”。
	*/	
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x16;//命令码低字节
	
	*(Ptr+index++) = 0xFF;//分包包号高字节，不需要分包
	*(Ptr+index++) = 0xFF;//分包包号低字节，不需要分包

	/***授权ID号***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.License);
	index += strlen((const char*)BlastInfo.DkeyInfo.License);
	*(Ptr+index++) = 0x0D;//分隔符
	
	/***控制器ID***/
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//分隔符	
	
	/***密钥ID***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Mac);
	index += strlen((const char*)BlastInfo.DkeyInfo.Mac);
	*(Ptr+index++) = 0x0D;//分隔符	
	
	return index;
}

/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */  
u16 GetBlastDataFirstPackFromCurrentFire(u8* Ptr)
{
	u16 index = 0;
	u16 Value;

	/*
		包号：0x0000+授权ID号+ 0x0D +密钥的ID号 + 0x0D +控制器ID号 +0x0D+
		起爆时间 + 0x0D + 经纬度坐标 + 0x0D + 注册雷管数量 +0x0D+错误雷管数量 +0x0D + 
		注册器数+ 0x0D+ 雷管信息长度+0x0D+软件版本号+0x0D+密钥登录的用户名+0x0D+上传数据类型+0x0D
	*/

	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	*(Ptr+index++) = 0x00;//分包包号高字节,第1包
	*(Ptr+index++) = 0x00;//分包包号低字节

	/***授权ID号***/	
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.License);
	index += strlen((const char*)BlastInfo.DkeyInfo.License);
	*(Ptr+index++) = 0x0D;
	
	/***密钥ID***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Mac);
	index += strlen((const char*)BlastInfo.DkeyInfo.Mac);
	*(Ptr+index++) = 0x0D;//分隔符	

	/***控制器ID***/	
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//分隔符
	
	/***起爆时间(当前时间)***/
	index +=sprintf((char*)(Ptr+index), "%04d%02d%02d%02d%02d%02d", BlastInfo.Time.year, 
				BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);			
	*(Ptr+index++) = 0x0D;	
	
	/***经纬度,格式为"11620.1092E,3957.7800N"***/
	if((strlen((char*)Op_Data.Gps.Longitude) >1) && (strlen((char*)Op_Data.Gps.Latitude) > 1))
	{
		memcpy(Ptr+index, Op_Data.Gps.Longitude, strlen((char*)Op_Data.Gps.Longitude));//经度，数据格式不一样
		index += strlen((char*)Op_Data.Gps.Longitude);
		*(Ptr+index++) = ',';
		memcpy(Ptr+index, Op_Data.Gps.Latitude, strlen((char*)Op_Data.Gps.Latitude));//纬度，数据格式不一样
		index += strlen((char*)Op_Data.Gps.Latitude);		
	}	

	*(Ptr+index++) = 0x0D;	
	
	/***注册雷管数量***/
	Value = Det.RegCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***错误雷管数量***/
	Value = Det.ErrorCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;		

	/***注册器数量***/
	*(Ptr+index++) = '1';	
	*(Ptr+index++) = 0x0D;	

	/***每发雷管信息长度14字节***/
	*(Ptr+index++) = '1';	
	*(Ptr+index++) = '4';
	*(Ptr+index++) = 0x0D;	

	/***软件版本***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;
	
	/***密钥用户名***/
	strcpy((char*)(Ptr+index), (const char*)BlastInfo.DkeyInfo.Operator);
	index += strlen((const char*)BlastInfo.DkeyInfo.Operator);
	*(Ptr+index++) = 0x0D;//分隔符
	
	/***上传数据类型***/
	*(Ptr+index++) = '2';//*(Ptr+index++) = BlastInfo.DkeyInfo.UploadType;//上传数据类型
	*(Ptr+index++) = 0x0D;//分隔符
	
	return index;
} 

/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */  
u16 GetBlastDataFirstPackFromRecorder(u8* Ptr, u16 RecorderIndex, u8* SlaveNumber)
{
	u16 index = 0;
	u8 Buffer[100], Number = 0;
	u8 FileName[17];
	u32 Value;
    
	/*
		包号：0x0000+授权ID号+ 0x0D +密钥的ID号 + 0x0D +控制器ID号 +0x0D+
		起爆时间 + 0x0D + 经纬度坐标 + 0x0D + 注册雷管数量 +0x0D+错误雷管数量 +0x0D + 
		注册器数+ 0x0D+ 雷管信息长度+0x0D+软件版本号+0x0D
	*/
	if(RecorderIndex == 0xFFFF)
	{//当前起爆记录
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}

	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	*(Ptr+index++) = 0x00;//分包包号高字节,第1包
	*(Ptr+index++) = 0x00;//分包包号低字节
	
	while(1)
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//更新界面
		}
		
		Buffer[0] = 0x00;
		switch(Number)
		{
			case 0:
				/***授权ID号***/
				GetConfigFileString("密钥授权码", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "授权ID_123");	
				break;
			case 1:
				/***密钥ID***/
				GetConfigFileString("密钥ID", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "ID_123");
				break;
			case 2:
				/***控制器ID***/
				GetConfigFileString("起爆器ID", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "起爆器ID");				
				break;	
			case 3:
				/***起爆时间***/
				GetConfigFileString("起爆时间", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 4:
				/***经纬度,格式为"11620.1092E,3957.7800N"***/
				GetConfigFileString("经纬度坐标", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 5:
				/***注册雷管数量"***/
				GetConfigFileString("注册雷管总数", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;	
			case 6:
				/***错误雷管数量"***/
				GetConfigFileString("错误雷管总数", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 7:
				/***注册器数量***/
				*SlaveNumber = (u8)GetConfigFileU32("译码器数量", 1, (u8*)GetBlasterPath(),FileName);
				sprintf((char*)&Buffer[0], "%d", *SlaveNumber);
				//GetConfigFileString("译码器数量", &Buffer[0],100,BLASTER_RECORD_FILE_PATH,FileName);
				break;	
			case 8:
				/***每发雷管信息长度14字节***/
				Buffer[0] = '1';
				Buffer[1] = '4';
				Buffer[2] = 0x00;
				break;
			case 9:
				/***软件版本"***/
				GetConfigFileString("软件版本", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 10:
				/***用户名"***/
				GetConfigFileString("用户名", &Buffer[0],100, (u8*)GetBlasterPath(),FileName);				
				break;
			case 11:
				/***上传数据类型"***/
				Value = GetConfigFileU32("上传数据类型",'2', (u8*)GetBlasterPath(),FileName);	
				Buffer[0] = (u8)((Value&0xFF) + '0');
				Buffer[1] = 0x00;
				break;				
			default:
				return index;
//				break;
		}
		strcpy((char*)(Ptr+index), (char*)&Buffer[0]);
		index += strlen((const char*)&Buffer[0]);
		*(Ptr+index++) = 0x0D;//数据间的分隔符	
			
		Number++;
	}
}

/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */  
u16 GetBlastDataSecondPackFromCurrentFire(u8* Ptr)
{
	u16 index = 0;
	u16 Value;
	
	/*
		包号：0x0001+注册器ID+0x0D+软件版本号+0x0D +注册雷管数+0x0D+错误雷管数+0x0D +
		测试后总线电压+ 0x0D +测试后总线电流+ 0x0D +总线漏电流+ 0x0D +网路充电后总线电压+ 
		0x0D +网路充电后总线电流+ 0x0D +起爆前总线电压+ 0x0D +起爆前总线电流+ 0x0D
	*/

	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	*(Ptr+index++) = 0x00;//分包包号高字节,第2包
	*(Ptr+index++) = 0x01;//分包包号低字节

	/***注册器ID***/
	strcpy((char*)(Ptr+index), (const char*)device_info.mac);
	index += strlen((const char*)device_info.mac);
	*(Ptr+index++) = 0x0D;//分隔符
	
	/***软件版本***/
	strcpy((char*)(Ptr+index), VERSION_STRING);
	index += strlen(VERSION_STRING);
	*(Ptr+index++) = 0x0D;
	
	/***注册雷管数量***/
	Value = Det.RegCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***错误雷管数量***/
	Value = Det.ErrorCount;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***测试后总线电压***/
	Value = BlastInfo.V_BeforeTest;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***测试后总线电流***/
	Value = BlastInfo.I_BeforeTest;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	
	
	/***总线漏电流***/
	Value = BlastInfo.I_Leak;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***网路充电后总线电压***/
	Value = BlastInfo.V_AfterCharge;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;	

	/***网路充电后总线电流***/
	Value = BlastInfo.I_AfterCharge;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;

	/***起爆前总线电压***/
	Value = BlastInfo.V_BeforeFire;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;		

	/***起爆前总线电流***/
	Value = BlastInfo.I_BeforeFire;
	index += sprintf((char*)(Ptr+index),"%d",Value);
	*(Ptr+index++) = 0x0D;
	
	return index;
} 
/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  数据缓冲区首地址
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */  
u16 GetBlastDataSecondPackFromRecorder(u8* Ptr, u16 RecorderIndex, u8 AddrName)
{
	u16 index = 0;
	u8 Buffer[100], Number = 0, Len;
	u8 FileName[17];
	u8 KeyName[40];
	
	/*
		包号：0x0001+注册器ID+0x0D+软件版本号+0x0D +注册雷管数+0x0D+错误雷管数+0x0D +
		测试后总线电压+ 0x0D +测试后总线电流+ 0x0D +总线漏电流+ 0x0D +网路充电后总线电压+ 
		0x0D +网路充电后总线电流+ 0x0D +起爆前总线电压+ 0x0D +起爆前总线电流+ 0x0D
	*/
	if(RecorderIndex == 0xFFFF)
	{//当前起爆记录
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	*(Ptr+index++) = 0x00;//分包包号高字节,第1包
	*(Ptr+index++) = 0x01;//分包包号低字节

	while(1)
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//更新界面
		}
		Buffer[0] = 0x00;
		switch(Number)
		{
			case 0:
				/***注册器ID***/
				sprintf((char*)KeyName, "%c译码器ID", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				//strcpy(&Buffer[0], "起爆器ID_1");
				break;
			case 1:
				/***软件版本"***/
				sprintf((char*)KeyName, "%c软件版本", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);					
				break;
			case 2:
				/***注册雷管数量"***/
				sprintf((char*)KeyName, "%c注册雷管数", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);			
				break;	
			case 3:
				/***错误雷管数量"***/
				sprintf((char*)KeyName, "%c错误雷管数", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);			
				break;	
			case 4:
				/***雷管测试后总线电压"***/
				sprintf((char*)KeyName, "%c雷管测试后总线电压", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}
				break;	
			case 5:
				/***雷管测试后总线电流"***/
				sprintf((char*)KeyName, "%c雷管测试后总线电流", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;	
			case 6:
				/***总线漏电流"***/
				sprintf((char*)KeyName, "%c总线漏电流", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;
			case 7:
				/***充电后总线电压"***/
				sprintf((char*)KeyName, "%c网路充电后总线电压", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;	
			case 8:
				/***网路充电后总线电流"***/
				sprintf((char*)KeyName, "%c网路充电后总线电流", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;
			case 9:
				/***起爆前总线电压"***/
				sprintf((char*)KeyName, "%c起爆前总线电压", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;			
			case 10:
				/***起爆前总线电流"***/
				sprintf((char*)KeyName, "%c起爆前总线电流", AddrName);
				GetConfigFileString(KeyName, &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
				Len = strlen((const char*)Buffer);//去掉单位
				if(Len > 2)
				{
					Buffer[Len-2] = 0x00;
				}				
				break;				
			default:
				return index;
//				break;
		}
		strcpy((char*)(Ptr+index), (char*)&Buffer[0]);
		index += strlen((const char*)&Buffer[0]);
		*(Ptr+index++) = 0x0D;	
			
		Number++;
	}
}

/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  Ptr数据缓冲区首地址
  * @param  PackIndex 数据包索引值，不小于2
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   在调用本函数时需要确保孔号已经排序好
  *************************************************************************************  
  */  
u16 GetBlastDataDetInfoFromCurrentFire(u8* Ptr, u16* PackIndex)
{
	u16 i, index = 0,Pack;
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	Pack = *PackIndex;
	if(40*(Pack-2+1) >= Det.RegCount)
	{
		*(Ptr+index++) = 0xFF;//当前为雷管信息的最后一包
		*(Ptr+index++) = 0xFE;//当前为雷管信息的最后一包
		*PackIndex  = 0xFFFE;
	}
	else
	{
		*(Ptr+index++) = (u8)(Pack>>8);//分包包号高字节
		*(Ptr+index++) = (u8)(Pack&0xFF);//分包包号低字节		
	}
	
	for(i = 40*(Pack-2); i < Det.RegCount; i++)//每次传40发雷管信息,每发雷管占14个字节
	{		
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[2];
		*(Ptr+index++) = Det.Info[i].Serl.U8Buffer[3];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[2];
		*(Ptr+index++) = Det.Info[i].Pwd.U8Buffer[3];
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[2];//频率不会超过3字节
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[1];
		*(Ptr+index++) = Det.Info[i].Freq.U8Buffer[0];
		*(Ptr+index++) = Det.Info[i].Err;
		*(Ptr+index++) = (u8)(Det.Info[i].Delay>>8);
		*(Ptr+index++) = (u8)(Det.Info[i].Delay&0xFF);
		if(index >= 565)//每次传40发雷管信息,每发雷管占14个字节
		{
			break;
		}
	}
	return index;
}

/**
  *************************************************************************************
  * @brief  根据当前起爆状态获取起爆数据
  * @param  Ptr数据缓冲区首地址
  * @param  PackIndex 数据包索引值，不小于2
  * @retval 数据长度
  * @author ZH
  * @date   2014年12月11日
  * @note   在调用本函数时需要确保孔号已经排序好
  *************************************************************************************  
  */  
u16 GetBlastDataDetInfoFromRecorder(u8* Ptr, u16* PackIndex, u16 RecorderIndex, u8 AddrName)
{
	u16 i, j, Len, index = 0,Pack;
	u16 RegCount,delay;
	u8 KeyBuffer[30], Buffer[100], FileName[17], Count;
	u32 Value;
	const u8 DetCountEveryTime = 30;
	
	if(RecorderIndex == 0xFFFF)
	{//当前起爆记录
		sprintf((char*)FileName, "%02d%02d%02d%02d%02d%02d.txt", BlastInfo.Time.year-2000, 
			BlastInfo.Time.month, BlastInfo.Time.day, BlastInfo.Time.hour, BlastInfo.Time.minute, BlastInfo.Time.second);	
	}
	else
	{
		if(RecorderIndex >= BlastRecorder.Number)
		{
			return 0;
		}
		memcpy(FileName, &BlastRecorder.FileName[RecorderIndex][0],12);
		strcpy((char*)&FileName[12],".txt");		
	}	
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	Pack = *PackIndex;
	
	sprintf((char*)Buffer, "%c注册雷管数", AddrName);
	RegCount = GetConfigFileU32(Buffer, 0, (u8*)GetBlasterPath(),FileName);
	
	if(DetCountEveryTime*(Pack-2+1) >= RegCount)
	{
		*(Ptr+index++) = 0xFF;//当前为雷管信息的最后一包
		*(Ptr+index++) = 0xFE;//当前为雷管信息的最后一包
		*PackIndex  = 0xFFFE;
	}
	else
	{
		*(Ptr+index++) = (u8)(Pack>>8);//分包包号高字节
		*(Ptr+index++) = (u8)(Pack&0xFF);//分包包号低字节		
	}
	
	for(i = DetCountEveryTime*(Pack-2); i < RegCount; i++)//每次传30发雷管信息,第发雷管占14个字节
	{
		if(Op_Data.UpdateGuiFlag)
		{
			UpdateProgbar(0);//更新界面
		}
		sprintf((char*)KeyBuffer,"%c雷管信息%d",AddrName, i+1);
		memset(&Buffer[0], 0, sizeof(Buffer));
		GetConfigFileString(&KeyBuffer[0], &Buffer[0],100, (u8*)GetBlasterPath(),FileName);
		Len = strlen((const char*)&Buffer[0]);
		Count = 0;
		for(j=0; j<Len; j++)
		{
			if(Buffer[j] == '\t')
			{
				Count++;
				Buffer[j] = 0x00;
			}
		}
		if(Count < 5)
		{
			return 0;
		}
		Count = 0;
		for(j=0; j<Len; j++)
		{
			if(Buffer[j] == 0x00)
			{
				Count++;
				if(Count == 2)//内码
				{
					HexStrToCharStr(&Buffer[j+1],16,Ptr+index);
					index += 8;
				}
				else if(Count == 3)//频率
				{
					sscanf((const char*)&Buffer[j+1],"%ld", &Value);
					*(Ptr+index++) = (u8)(Value>>16);
					*(Ptr+index++) = (u8)(Value>>8);
					*(Ptr+index++) = (u8)(Value&0xFF);
				}
				else if(Count == 4)//延期
				{
					sscanf((const char*)&Buffer[j+1],"%ld", &Value);
					delay = Value;
				}				
				else if(Count == 5)//状态码
				{
					HexStrToCharStr(&Buffer[j+1],2,Ptr+index);
					index += 1;
					*(Ptr+index++) = (u8)(delay>>8);
					*(Ptr+index++) = (u8)(delay&0xFF);					
					break;
				}				
			}
		}
		if(index >= (DetCountEveryTime*14+5))//每次传30发雷管信息,第发雷管占14个字节
		{
			break;
		}
	}
	return index;
}

u16 GetBlastDataEndPack(u8* Ptr)
{
	u16 index = 0;
	
	*(Ptr+index++) = 0x41;//协议地址
	*(Ptr+index++) = 0x00;//命令码高字节
	*(Ptr+index++) = 0x15;//命令码低字节

	*(Ptr+index++) = 0xFF;//分包包号高字节,结束包号
	*(Ptr+index++) = 0xFF;//分包包号低字节,结束包号	
	
	return index;
}
ErrorStatus WireleTransferData(u8* tbuf, u16 tlen, u8* rbuf, u16* rlen, u8 retry, u16 timer_out, ErrorStatus WifiFlag)
{
	if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (WifiFlag != ERROR))
	{//wifi已经连接
		if(SUCCESS == WifiTransferData(tbuf, tlen, rbuf, rlen,retry,timer_out))
		{
			return SUCCESS;
		}			
	}
//	else
//	{
//		if(GPRS_info.Result == GPRS_SUCESS)
//		{
//			if(GPRS_SUCESS == GPRSTransferData(tbuf, tlen, rbuf, rlen,retry,timer_out))
//			{
//				return SUCCESS;
//			}		
//		}	
//	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  常规的无线传输数据
  * @param  无
  * @retval 发送结果
  *   @arg @b 0x00 发送失败
  *   @arg @b 0x01 发送接收成功  
  *   @arg @b 0x02 接收到的数据解包成功但返回错误
  *   @arg @b 0x03 接收到的数据解包失败   
  * @author ZH
  * @date   2014年12月26日
  * @note   
  *************************************************************************************  
  */
u8 WireleGeneralTransferData(ErrorStatus WifiFlag)
{
	Pack(&SendBuffer, &TempBuffer);//数据打包

	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 7)
			{
				if((TempBuffer.uch_buf[1] == SendBuffer.uch_buf[1])  && (TempBuffer.uch_buf[2] == SendBuffer.uch_buf[2]) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) && (TempBuffer.uch_buf[6] == 0x0D))
				{
					if ((TempBuffer.uch_buf[5] >= '0') && (TempBuffer.uch_buf[5] <= '9'))
					{
						return TempBuffer.uch_buf[5];
					}
					return 0x01;
				}			
			}
			return 0x02;
		}
		return 0x03;
	}
	return 0x00;
}
/**
  *************************************************************************************
  * @brief  发送设备开机数据到服务器中
  * @param  无
  * @retval 发送结果
  *   @arg @b SUCCESS 成功
  *   @arg @b ERROR   失败  
  * @author ZH
  * @date   2014年12月11日
  * @note  
  *************************************************************************************  
  */
ErrorStatus SendDeviceStartData(ErrorStatus WifiFlag)
{	
	u8 Result;
	
	SendBuffer.bytenum = GetDeviceStartData(SendBuffer.uch_buf);
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
  * @brief  发送远程论证数据到服务器中
  * @param  无
  * @retval 发送结果
  *   @arg @b 0 通信失败
  *   @arg @b 1 验证通过 
  *   @arg @b 2 授权ID无效
  *   @arg @b 3 控制器ID无效
  *   @arg @b 4 密钥ID无效
  * @author ZH
  * @date   2014年12月11日
  * @note  
  *************************************************************************************  
  */
u8 SendDeviceArgumentData(ErrorStatus WifiFlag)
{	
	SendBuffer.bytenum = GetDeviceArgumentData(SendBuffer.uch_buf);
	
	Pack(&SendBuffer, &TempBuffer);//数据打包
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,10000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 13) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x16) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF) &&
					(TempBuffer.uch_buf[5] == '1')  && (TempBuffer.uch_buf[6] == 0x0D) &&
					(TempBuffer.uch_buf[7] == '1')  && (TempBuffer.uch_buf[8] == 0x0D)  && 
					(TempBuffer.uch_buf[9] == '1')  && (TempBuffer.uch_buf[10] == 0x0D)  && 
					(TempBuffer.uch_buf[11] == '1')  && (TempBuffer.uch_buf[12] == 0x0D))
				{
					return 0x01;//验证通过				
				}
				else if((TempBuffer.uch_buf[7] != '1')  && (TempBuffer.uch_buf[8] == 0x0D))
				{
					return 0x02;//授权ID无效
				}
				else if((TempBuffer.uch_buf[9] != '1')  && (TempBuffer.uch_buf[10] == 0x0D))
				{
					return 0x03;//控制器ID无效
				}
				else if((TempBuffer.uch_buf[11] != '1')  && (TempBuffer.uch_buf[12] == 0x0D))
				{
					return 0x04;//密钥ID无效
				}				
			}
		}
	}
	return 0x00;
}

/**
  *************************************************************************************
  * @brief  发送起爆信息数据到服务器中
  * @param  index 起爆记录索引,0xFFFF表示当前起爆记录
  * @retval 发送结果
  *   @arg @b 0x01 成功
  *   @arg @b 其他   失败  
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */
u8 SendBlastFireInfoData(u16 index, ErrorStatus WifiFlag)
{
	u8 Result;
	u16 PackIndex;
	u8 SlaveNumber = 0;//译码器的数量
	u8 SlaveIndex = 0;//当前刚在发送的译码器索引
	u8 FileName[20];
	
	if(index >= BlastRecorder.Number)
	{
		return 0xFF;
	}
	memcpy(FileName, &BlastRecorder.FileName[index][0],12);
	strcpy((char*)&FileName[12],".txt");	
	if(ConfigFileVerify((u8*)GetBlasterPath(), (u8*)FileName, 0) == ERROR)
	{
		return 0xFF;		
	}
	
	//发送第1包数据
	SendBuffer.bytenum = GetBlastDataFirstPackFromRecorder(SendBuffer.uch_buf, index, &SlaveNumber);//获取起爆记录数据第1包的数据
	if(0x00 == SendBuffer.bytenum)
	{
		return 0xFF;
	}

	Result = WireleGeneralTransferData(WifiFlag);	
	if(Result != '1')
	{
		return Result;
	}
	while(SlaveIndex < SlaveNumber)
	{
		//发送第2包数据
		SendBuffer.bytenum = GetBlastDataSecondPackFromRecorder(SendBuffer.uch_buf, index, GetAddrNameFromIndex(SlaveIndex));//获取起爆记录数据第2包的数据
		if(0x00 == SendBuffer.bytenum)
		{
			return 0xFF;
		}		
		Result = WireleGeneralTransferData(WifiFlag);	
		if(Result != '1')
		{
			return Result;
		}
		
		//从第3包开始发送雷管信息数据
		PackIndex = 2;
		while(1)
		{
			if(Op_Data.UpdateGuiFlag)
			{
				UpdateProgbar(0);//更新界面
			}
			//获取当前起爆数据的雷管数据数据，每次发送40发雷管信息，完成后包索引返回0xFFFE
			SendBuffer.bytenum = GetBlastDataDetInfoFromRecorder(SendBuffer.uch_buf, &PackIndex, index, GetAddrNameFromIndex(SlaveIndex));
			if(0x00 == SendBuffer.bytenum)
			{
				return 0xFF;
			}			

			Result = WireleGeneralTransferData(WifiFlag);	
			if(Result != '1')
			{
				return Result;
			}
			if(PackIndex == 0xFFFE)
			{//所有雷管都已经发送
				break;
			}
			else
			{
				PackIndex++;//继续发送下一包雷管信息
			}
		}
		SlaveIndex++;//发送下一个译码器的信息
	}

	
	//发送结束包标志
	SendBuffer.bytenum = GetBlastDataEndPack(SendBuffer.uch_buf);
	Result = WireleGeneralTransferData(WifiFlag);
	
	return Result;
}
/**
  *************************************************************************************
  * @brief  退出安管功能，关闭GPRS模块电源和WIFI SOCKET
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */
void ExitSafeMonitor(void)
{
	CloseWifiSocket();
//	GPRS_POWER_OFF();
}

/**
  *************************************************************************************
  * @brief  获取起爆记录的上传状态
  * @param  无
  * @retval ERROR 表示未上传 ，SUCCESS不需要上传
  * @author ZH
  * @date   2014年12月11日
  * @note   返回的数据不包含雷管信息
  *************************************************************************************  
  */
ErrorStatus GetBlastRecoderUploadState(u16 Index)
{
	u8 Buffer[100];
	u8 FileName[17];
	
	if(Index < BlastRecorder.Number)
	{
		memcpy(FileName, &BlastRecorder.FileName[Index][0],12);
		strcpy((char*)&FileName[12],".txt");	
		if(ERROR != GetConfigFileString("信息上传标志", &Buffer[0],50, (u8*)GetBlasterPath(), FileName))
		{
			if(memcmp(Buffer,"未上传",6) == 0)
			{
				return ERROR;
			}
		}	
	}

	return SUCCESS;
}


/**
  *************************************************************************************
  * @brief  获取服务器中当前的北京时间
  * @param  无
  * @retval 发送结果
  *   @arg @b ERROR 通信失败
  *   @arg @b SUCCESS 成功
  * @author ZH
  * @date   2016年06月18日
  * @note  
  *************************************************************************************  
  */
ErrorStatus GetServerCurrentTime(ErrorStatus WifiFlag, u8* ServerTimePtr)
{	
	SendBuffer.bytenum = 0;
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x41;//协议地址
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x00;//命令码高字节
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0x18;//命令码低字节

	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0xFF;//分包包号高字节
	SendBuffer.uch_buf[SendBuffer.bytenum++] = 0xFF;//分包包号低字节

	Pack(&SendBuffer, &TempBuffer);//数据打包
	
	if(SUCCESS == WireleTransferData(TempBuffer.uch_buf, TempBuffer.bytenum, ReceiceBuffer.uch_buf, &ReceiceBuffer.bytenum,3,5000, WifiFlag))
	{
		if(Unpack(&ReceiceBuffer, &TempBuffer) != ERROR)
		{
			if(TempBuffer.bytenum == 20) 			
			{
				if((TempBuffer.uch_buf[1] == 0x00)  && (TempBuffer.uch_buf[2] == 0x18) &&
					(TempBuffer.uch_buf[3] == 0xFF)  && (TempBuffer.uch_buf[4] == 0xFF)  && (TempBuffer.uch_buf[TempBuffer.bytenum-1] == 0x0D))
				{
					memset(ServerTimePtr,0,15);
					memcpy(ServerTimePtr, &TempBuffer.uch_buf[5], 14);					
					return SUCCESS;//获取成功			
				}				
			}
		}
	}
	return ERROR;
}

