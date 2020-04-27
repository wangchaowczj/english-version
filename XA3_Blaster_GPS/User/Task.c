#include "includes.h"
#include "usb_bsp.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbd_hid_core.h"
#include "usb_dcd_int.h"
#include "usb_hcd_int.h"
#include "GuiIncludes.h"

extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;
extern __ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;


STR_COMM  HidData = {0,RECIEVING,0};
REG_DET_NET_PARAM RegDetNetParam;
NET_PARAM NetParam[NET_PARAM_MAX];
OP_DATA Op_Data;
MSG_SEM MsgSem;
DEVICE_INFO device_info;


/**
  *************************************************************************************
  * @brief  HID设备通信任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppUSBTask(void* p_arg)
{	
	INT8U Err;
		
	MsgSem.usb_rcv =  OSSemCreate(0);
	
	usb_hid_device_init();
	
	while(1)
	{
		OSSemPend(MsgSem.usb_rcv,0,&Err);
		LcdBrightRefreshProcess(1);
		Op_Data.SystemIdleTime = OSTimeGet();
		if(RECIEVED == HidData.status)
		{//usb
			if(SUCCESS == Unpack(&HidData, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
				if(GusetSendBuffer.bytenum != 0)
				{
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					usb_hid_device_tedata(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			HidData.bytenum = 0;
			HidData.status = RECIEVING;		
		}
		else if(RECIEVED == BlueData.status)
		{//蓝牙
			if(SUCCESS == Unpack(&BlueData, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
				if(GusetSendBuffer.bytenum != 0)
				{
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					BlueSendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
		}		
	}
}

/**
  *************************************************************************************
  * @brief  RS485通信任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppRS485Task(void* p_arg)
{	
	INT8U Err;
	u32 c_time1, c_time2;
    
	MsgSem.RS485 =  OSSemCreate(0);
	while(1)
	{
		OSSemPend(MsgSem.RS485,0,&Err);
		LcdBrightRefreshProcess(1);
		Op_Data.SystemIdleTime = OSTimeGet();
		if(RECIEVED == RS485Data.status)
		{
            c_time1 = OSTimeGet();
			if(SUCCESS == Unpack(&RS485Data, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
				if(GusetSendBuffer.bytenum != 0)
				{
                    //级联模式，与RS485中继一起使用时，从机不能响应太快，中继还没做好接收准备，造成通讯异常。从机在响应前加50ms延时。
                    c_time2 = OSTimeGet();
                    if((c_time2-c_time1) < 50)
                    {
                        OSTimeDly(51-(c_time2-c_time1));
                    }
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					RS485SendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			RS485Data.bytenum = 0;
			RS485Data.status = RECIEVING;		
		}
	}
}
/**
  *************************************************************************************
  * @brief  临时任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   用于运行一些比较耗时的函数,避免阻塞当前进程,允许强行删除本任务
  *************************************************************************************  
  */
void AppTemporaryTask(void* p_arg)
{	
	Op_Data.AppTemporaryTaskResult = 0;
	
	while(1)
	{
		if (((u32)p_arg) == 0)
		{//蓝牙模块从机初始化
			Op_Data.AppTemporaryTaskResult = (u8)BlueSlaveInit();
			if((ErrorStatus)Op_Data.AppTemporaryTaskResult == ERROR)
			{
				WriteSystemLog("蓝牙模块从机初始化失败",strlen("蓝牙模块从机初始化失败"), CURRENT_STATUS);
			}
			else
			{
				WriteSystemLog("蓝牙模块从机初始化成功",strlen("蓝牙模块从机初始化成功"), CURRENT_STATUS);
			}			
		}
		else if (((u32)p_arg) == 1)
		{//蓝牙模块主机初始化
			Op_Data.AppTemporaryTaskResult = (u8)BlueHostInit();
			if((ErrorStatus)Op_Data.AppTemporaryTaskResult == ERROR)
			{
				WriteSystemLog("蓝牙模块主机初始化失败",strlen("蓝牙模块主机初始化失败"), CURRENT_STATUS);
			}
			else
			{
				WriteSystemLog("蓝牙模块主机初始化成功",strlen("蓝牙模块主机初始化成功"), CURRENT_STATUS);
			}			
		}
		else if (((u32)p_arg) == 2)
		{//蓝牙模块主机扫描从机
			BlueScanSlave();			
		}
		else if (((u32)p_arg) == 3)
		{//蓝牙模块主机连接从机
			Op_Data.DKeyVoltage = 0xFFFF;
			Op_Data.AppTemporaryTaskResult = (u8)BlueConnectSlave(Op_Data.ConnectBlueIndex);
			
			//如果是密钥的话，连接成功后读一下电池电压
			if((ErrorStatus)Op_Data.AppTemporaryTaskResult != ERROR)
			{
				if(memcmp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name, "BK", 2) == 0)
				{
					GetBlueDkeyBatVoltage(&Op_Data.DKeyVoltage);			
				}
                if(memcmp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name, "BKG_", 4) == 0)
				{
					SetRtcByBlueDkey();			
				}                
			}
		}
		else if (((u32)p_arg) == 3)
		{//蓝牙模块主机断开已连接的从机
			Op_Data.AppTemporaryTaskResult = (u8)BlueHostDisConnect();
		}
		else if (((u32)p_arg) == 4)
		{//开机自检(部分内容)			
			//检查和蓝牙模块通信是否正常
			if(ERROR == BlueSlaveInit())//检查和蓝牙模块通信是否正常
			{
				Op_Data.AppTemporaryTaskResult = 1;
				WriteSystemLog("开机蓝牙模块异常",strlen("开机蓝牙模块异常"), CURRENT_STATUS);
			}
			else
			{
				if(GetBlueVersion(&device_info.BlueVersion[0]) == ERROR)
				{
					Op_Data.AppTemporaryTaskResult = 1;
					WriteSystemLog("开机蓝牙模块异常",strlen("开机蓝牙模块异常"), CURRENT_STATUS);			
				}			
			}

			CLOSE_BLUE_POWER();
			
			//检查和Wifi模块通信是否正常
			if(Op_Data.AppTemporaryTaskResult == 0)
			{
				u8 WifiPower = GET_WIFI_POWER_STATE();
				
				OPEN_WIFI_POWER();
				if(SUCCESS == WifiInit())
				{
					if(ERROR == GetWifiCurrentAPInfo(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry))
					{
						Op_Data.AppTemporaryTaskResult = 2;//Wifi模块异常
						WriteSystemLog("开机Wifi模块异常",strlen("开机Wifi模块异常"), CURRENT_STATUS);
					}
				}
				else
				{
					Op_Data.AppTemporaryTaskResult = 2;//Wifi模块异常
					WriteSystemLog("开机Wifi模块初始化异常",strlen("开机Wifi模块初始化异常"), CURRENT_STATUS);
				}			
				if(WifiPower != 0)
				{//之前电源没有打开
					CLOSE_WIFI_POWER();
				}				
			}
			//检查扫码模块是否正常
			if(Op_Data.AppTemporaryTaskResult == 0)
			{
				if(device_info.HasScan)
				{
                    u8 Result;
                    
                    Result = ScanHardwareState();
					if(0 == Result)
					{
                        Op_Data.ScanIsZEBRA = 0;
						Op_Data.AppTemporaryTaskResult = 3;
						WriteSystemLog("开机扫码模块异常",strlen("开机扫码模块异常"), CURRENT_STATUS);
					}
                    else if(3 == Result)
                    {
                        Op_Data.ScanIsZEBRA = 1;
                    }
                    else
                    {
                        Op_Data.ScanIsZEBRA = 0;
                    }
				}
			}
		}
		else if (((u32)p_arg) == 5)
		{//检测高压芯片相关电路是否正常
			 Op_Data.AppTemporaryTaskResult = (u8)CheckHVProtectPoint();		 
		}		
		OSTaskDel(OS_PRIO_SELF);
	}	
}
/**
  *************************************************************************************
  * @brief  低优先级任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   
  *************************************************************************************  
  */
void AppLowPriTask(void* p_arg)
{
    Op_Data.AppLowTaskResult = 0;
	while(1)
	{		
		if (((u32)p_arg) == 0)
        {
            Op_Data.AppLowTaskResult = (u8)FileSystemFormatSdCard();
        }
        OSTaskDel(OS_PRIO_SELF);
    }
}
/**
  *************************************************************************************
  * @brief  后台任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   用于运行一些比较耗时的函数,避免阻塞当前进程,不允许强行删除本任务
  *************************************************************************************  
  */
void AppBackgroundTask(void* p_arg)
{
	u16 i;
	WM_MESSAGE Msg;
	u32 c_time;
	
	OSTimeDly(10);
	
	while(1)
	{		
		if (((u32)p_arg) == 0)
		{//检测高压芯片相关电路是否正常
			 Msg.Data.v = (int)CheckHVProtectPoint();		 
		}
		else if (((u32)p_arg) == 1)
		{//设备自检,自检低压部分
			Msg.Data.v = (int)(ErrorStatus)DeviceSelfTest(0x01);
		}
		else if(((u32)p_arg) == 2)
		{//等待总线电流稳定
            u8 Buffer[20];
            
			WriteSystemLog("开始等待总线电流稳定",strlen("开始等待总线电流稳定"), CURRENT_STATUS);
			
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+10000/(1000/OS_TICKS_PER_SEC)))
			{
				OSTimeDlyHMSM(0,0,0,200);
				if((Op_Data.LinABCurrent < BusCurrentRangeMax()) && 
					(abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) < 100))
				{
					if(ERROR == WaiLinCurrentSteady(1000))
					{
						WaiLinCurrentSteady(3000);
					}
					break;
				}
			}
            
            AllDisCharge();//发送全局复位指令,芯片上电后的电流比接收到指令后的电流要小一些
            
			Msg.Data.v = (int)WaiLinCurrentSteady(3000+Det.RegCount*5);//注册雷管数越多，超时时间越长
			OSTimeDlyHMSM(0,0,0,100);
			WriteSystemLog("总线电流已经稳定",strlen("总线电流已经稳定"), CURRENT_STATUS);
			BlastInfo.V_BeforeTest = Op_Data.LinABVoltage;
			BlastInfo.I_BeforeTest = Op_Data.LinABCurrent;
			BlastInfo.I_Leak = GetBusLeakCurrent(NULL);

            sprintf((char*)Buffer, "总线漏电:%duA", BlastInfo.I_Leak);
            WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);			
		}
		else if (((u32)p_arg) == 3)
		{//把数据从密钥中导入
			Msg.Data.v = (int)BlueReadDetInfoBackUp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4]);
		}
		else if (((u32)p_arg) == 4)
		{//把数据导出到密钥
			Msg.Data.v = (int)BlueDetInfoBackUp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4]);
		}
//		else if (((u32)p_arg) == 5) 
//		{//安管功能，起爆信息验证
//			Op_Data.DetVerifyErrIndex = 0;
//			Msg.Data.v = (int)DkeyBlastInfoVerify(&Op_Data.DetVerifyErrIndex);		
//		}
		else if (((u32)p_arg) == 6)
		{//设备自检,自检高压部分
			Msg.Data.v = (int)(ErrorStatus)DeviceSelfTest(0x02);
			CLOSE_HV_SW_INSIDE();
			Op_Data.LinNormalHighVoltageGoal = LIN_NORMAL_HIGH_VOLTAGE_GOAL;
			setDacHvValueMinFromVoltage(LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX);
			DacOutHVVoltage(LIN_NORMAL_HIGH_VOLTAGE_GOAL);
			
			i = 10;
			while(i--)
			{
				if(GetChannelxVoltage(ADC_CH_11_3V, 10) < 2500)
				{
					break;
				}
				OSTimeDlyHMSM(0,0,1,0);
			}
		}
		else if (((u32)p_arg) == 7)
		{//设备自检,自检起爆信号
			Msg.Data.v = (int)(ErrorStatus)DeviceSelfTest(0x04);
		}
		else if (((u32)p_arg) == 11) 
		{//扫码
			if(GET_SCAN_POWER_STATE() != 0)
			{//扫码模块电源还没有打开
				OPEN_SCAN();
				OSTimeDlyHMSM(0,0,0,200);				
			}
			ScanWork(20000UL, Op_Data.ScanIsZEBRA);
            if(OSTaskDelReq(OS_PRIO_SELF))
            {
                Msg.Data.v = 2;
            }
            else
            {
                if(0 == ScanData.Bytes)
                {
                    Msg.Data.v = 0;
                }
                else
                {
                    Msg.Data.v = 1;
                }            
            }
		}
		else if (((u32)p_arg) == 12)
		{//合并数据(主机<-从机)
			Msg.Data.v = (int)BlueReadDetInfoInsert(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.InputValue1,Op_Data.InputValue2);
		}
		else if (((u32)p_arg) == 13)
		{//合并数据(主机->从机)
			Msg.Data.v = (int)BlueDetInfoInsert(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.InputValue1,Op_Data.InputValue2);
		}
		else if (((u32)p_arg) == 14) 
		{//通过485导入数据
			Msg.Data.v = BackupImportDetDataFromSlave();
		}
		else if (((u32)p_arg) == 15) 
		{//通过485导出数据
			Msg.Data.v = BackupExportDetDataToSlave();
		}
		else if (((u32)p_arg) == 16) 
		{//雷管授权验证，丹灵
            OSTimeDly(50);
			Msg.Data.v = AllDetAccreditForDL();
		}		
		else
		{
			OSTaskDel(OS_PRIO_SELF);
		}
		
		Msg.MsgId = WM_BACKGROUND_TASK_END;
		CustomSendMessage(DlghWin.Dlg, &Msg);
		OSTaskDel(OS_PRIO_SELF);	
	}
}
/**
  *************************************************************************************
  * @brief  Wifi任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2018年5月8日
  * @note   
  *************************************************************************************  
  */
void AppWifiTask(void* p_arg)
{
	WM_MESSAGE Msg;
	u32 c_time;
	
	Msg.Data.v = (int)ERROR;
	
	if (((u32)p_arg) == 0) 
	{//打开Wifi 并初始化
		OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
		FileSystemLock();
		WifiLock();				
		OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
		WifiUnlock();
		FileSystemUnlock();
		
		ExitSafeMonitor();
				
		if(ERROR != WifiInit())
		{
			if(ERROR != GetWifiCurrentAPInfo(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry))
			{
				WifiScanAP(&MyWifi.ScanResult);//扫描网络
				OSSchedLock();
				ClearSameWifiScanResult(&MyWifi.ScanResult);
				OSSchedUnlock();
				
				//如果WIFI模块没有自动连接上，那么如果扫描到的WIFI中有已经连接过的，就选择连接
				c_time = OSTimeGet();
				while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
				{
					if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
					{
						break;
					}					
				}
				AutoConnect();
				
				Msg.Data.v = (int)SUCCESS;//Wifi模块正常
			}
		}			
	}
	else if (((u32)p_arg) == 1) 
	{//连接指定WIFI网络
		if(ERROR != WifiConnectAP(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry))//连接网络
		{						
			//如果已经连到网络，则保存WIFI密码
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
			{
				OSTimeDly(10);
				if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
				{
					Msg.Data.v = (int)SUCCESS;
					
					SaveWifiInfo(MyWifi.Ssid,MyWifi.Passwords);
					break;
				}				
			}
		}		
	}
	else if (((u32)p_arg) == 2) 
	{//扫描WIFI网络
		if(GET_WIFI_POWER_STATE() == 0)
		{//电源已经打开
            AP_SCAN_RESULT ScanResult;
            
			OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
			FileSystemLock();
			WifiLock();				
			OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
			WifiUnlock();
			FileSystemUnlock();
			
			ExitSafeMonitor();
			
			WifiScanAP(&ScanResult);//扫描网络
            ClearSameWifiScanResult(&ScanResult);			
			OSSchedLock();
            if(OSTaskDelReq(OS_PRIO_SELF) != OS_ERR_TASK_DEL_REQ)
            {
                MyWifi.ScanResult = ScanResult;
            }
			OSSchedUnlock();
		}		
	}
	else if (((u32)p_arg) == 3) 
	{//定时扫描WIFI网络并自动连接
		while(1)
		{
			OSTimeDlyHMSM(0,0,10,0);
			if (GET_WIFI_POWER_STATE() == 0)
			{
				if (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP)
				{//连接服务器更新时钟以及判断是否需要更新程序,开机后只查询一次
					if(Op_Data.AutoUpdateStatus == 0)
					{
						TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)4);
					}									
				}
				else
				{//还没连接
					WifiScanAP(&MyWifi.ScanResult);//扫描网络			
					
					ClearSameWifiScanResult(&MyWifi.ScanResult);
					
					AutoConnect();//如果扫描到的WIFI中有已经连接过的，就选择连接				
				}
			}
		}
	}
	
	Msg.MsgId = WM_WIFI_TASK_END;
	CustomSendMessage(DlghWin.Dlg, &Msg);	
	OSTaskDel(OS_PRIO_SELF);	
}

/**
  *************************************************************************************
  * @brief  雷管操作任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   任务中不要申请公共资源，因为其他任务有可能会删除本任务
  *************************************************************************************  
  */
void AppDetOperateTask(void* p_arg)
{
	u16 i,Value;
	u32 c_time;
	
    AppTaskResult.DetOperate = 0;
    if((u32)p_arg != 100)
    {
        OSTimeDly(5);
    }
		
	while(1)
	{
		if((u32)p_arg == 0)
		{//快速测试
			WriteSystemLog("开始组网测试",strlen("开始组网测试"), CURRENT_STATUS);
			
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{				
				if((Op_Data.LinABCurrent < BusCurrentRangeMax()) && 
					(abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT))
				{
					break;
				}
				OSTimeDlyHMSM(0,0,0,200);
			}			

			AppTaskResult.DetOperate = (int)TestDetQuick();
			OSTimeDlyHMSM(0,0,0,100);			
			GetDetAckCurrentWave();
			OSTimeDlyHMSM(0,0,2,500);//400发雷管时，大概要1.8秒
			
			WriteSystemLog("组网测试结束",strlen("组网测试结束"), CURRENT_STATUS);				
		}
		else if((u32)p_arg == 1)
		{//完全测试
			WriteSystemLog("开始网路测试",strlen("开始网路测试"), CURRENT_STATUS);
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{				
				if((Op_Data.LinABCurrent < BusCurrentRangeMax()) &&
					(abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT))
				{
					break;
				}
				OSTimeDlyHMSM(0,0,0,200);
			}
			
			TestDetComplete();
			OSTimeDlyHMSM(0,0,0,100);			
			GetDetAckCurrentWave();			
			OSTimeDlyHMSM(0,0,2,500);//400发雷管时，大概要1.8秒
			
			WriteSystemLog("网路测试结束",strlen("网路测试结束"), CURRENT_STATUS);						
		}
		else  if((u32)p_arg == 2)
		{//起爆流程测试
			WriteSystemLog("开始起爆流程测试",strlen("开始起爆流程测试"), CURRENT_STATUS);
			
			Op_Data.ForcePowerOffSwitch = 0;//起爆流程中禁止自动关机
            Op_Data.OfflineMonitorBlastProcess = 0;
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{				
				if((Op_Data.LinABCurrent < BusCurrentRangeMax()) && 
					(abs(Op_Data.LinABVoltage -  Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT))
				{
					break;
				}
				OSTimeDlyHMSM(0,0,0,200);
			}			
			Op_Data.BlastStatus = DET_TESTING;
			BlasteTestComplete();
			OSTimeDlyHMSM(0,0,0,100);			
			GetDetAckCurrentWave();	
			OSTimeDlyHMSM(0,0,2,500);//400发雷管时，大概要1.8秒
            if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
            {//用于监测总线掉线
                if (GetDetOnlineCount() > 0)
                {
                    Op_Data.OfflineMonitorBlastProcess = 1;
                }
            }			
			WriteSystemLog("起爆流程测试结束",strlen("起爆流程测试结束"), CURRENT_STATUS);
			
			Op_Data.BlastStatus = DET_TEST_END;		
		}
		else  if((u32)p_arg == 3)
		{//网路充电
			WriteSystemLog("开始网路充电",strlen("开始网路充电"), CURRENT_STATUS);
			
			OSTimeDlyHMSM(0,0,0,50);
			BlastInfo.V_BeforeCharge = Op_Data.LinABVoltage;
			BlastInfo.I_BeforeCharge = Op_Data.LinABCurrent;
			
			Op_Data.LinAdjustTime = OSTimeGet() + 5000;//刚输出高压,电流太大，如果马上调整有可能会出现电压太高，硬件保护
			OPEN_HV_SW();//高压芯片产生的高压正常，输出到总线
			
			ClearProgbarData();
			Op_Data.Progbar.Bar[0].Max = 100;			
			Op_Data.BlastStatus = DET_CHARGING;
			Value = 0;
			
			for(i=0; i < 100; i++)
			{
				Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//增加进度条
				if (abs(Op_Data.LinABVoltage - Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
				{
					OSTimeDlyHMSM(0,0,0,500);
					Value = 0;
					continue;
				}				
				if(Value < ((Det.RegCount>100)?20:10))
				{//电流没有稳定
					if(ERROR != WaiLinCurrentSteady(500))
					{//电流已经稳定
						Value++;
					}
					else
					{
						Value = 0;
					}
				}
				else
				{
					OSTimeDlyHMSM(0,0,0,50);				
				}
				if((i&0x0F) == 0)
				{
					WriteSystemLog("高压调压电路DA最小值:",strlen("高压调压电路DA最小值:"), NEW_STR_INFO);
					WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
				}
			}
			while(OSTimeGet() < Op_Data.LinAdjustTime)
			{
				OSTimeDly(1);
			}
			while (abs(Op_Data.LinABVoltage - Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
			{
				OSTimeDly(1);
			}			
			FillCurrentProgbar();//更新进度条到100%
			
			WriteSystemLog("网络充电结束",strlen("网络充电结束"), CURRENT_STATUS);
			
			WriteSystemLog("高压调压电路DA最小值:",strlen("高压调压电路DA最小值:"), NEW_STR_INFO);
			WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
			
			
			BlastInfo.V_AfterCharge = Op_Data.LinABVoltage;
			BlastInfo.I_AfterCharge = Op_Data.LinABCurrent;
			BlastInfo.I_AfterChargeMin = Op_Data.LinABCurrent;
			BlastInfo.I_AfterChargeMax = Op_Data.LinABCurrent;
			
			Op_Data.BlastStatus = DET_CHARGE_END;
			Op_Data.ChargeCurrentVaryFlag = 1;//开始记录总线峰值变化
		}
		else  if((u32)p_arg == 4)
		{//主机读从机对雷管快速测试
			WriteSystemLog("主机开始控制从机快速测试",strlen("主机开始控制从机快速测试"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = DET_TESTING;
			SET_RS485_SEND();//485设置为发送状态
			Value = AllSlaveDetTest(0, GetMasterTimeShareTest());
			SET_RS485_SEND();//485设置为发送状态
			
			WriteSystemLog("主机控制从机快速测试结束",strlen("主机控制从机快速测试结束"), NEW_STR_INFO);
			
			if(ERROR == (ErrorStatus)Value)
			{//由其他任务主动删除
				while(1) OSTimeDly(50);
			}			
			Op_Data.BlastStatus = DET_TEST_END;
		}
		else  if((u32)p_arg == 5)
		{//主机控制从机进行起爆流程雷管测试
			WriteSystemLog("主机控制从机进行起爆流程测试",strlen("主机控制从机进行起爆流程测试"), NEW_STR_INFO);
			
			Op_Data.ForcePowerOffSwitch = 0;//起爆流程中禁止自动关机
			Op_Data.BlastStatus = DET_TESTING;
			SET_RS485_SEND();//485设置为发送状态
			Value = AllSlaveDetTest(1, GetMasterTimeShareTest());
			SET_RS485_SEND();//485设置为发送状态
			
			WriteSystemLog("主机控制从机进行起爆流程测试结束",strlen("主机控制从机进行起爆流程测试结束"), NEW_STR_INFO);
			if(ERROR == (ErrorStatus)Value)
			{//由其他任务主动删除
				while(1) OSTimeDly(50);
			}
			Op_Data.BlastStatus = DET_TEST_END;
		}
		else  if((u32)p_arg == 6)
		{//主机控制从机网络充电
			WriteSystemLog("主机控制从机网络充电",strlen("主机控制从机网络充电"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = DET_CHARGING;
			SET_RS485_SEND();//485设置为发送状态
			Value = AllSlaveCharge();
			SET_RS485_SEND();//485设置为发送状态
			
			WriteSystemLog("主机控制从机网络充电结束",strlen("主机控制从机网络充电结束"), NEW_STR_INFO);
			if(ERROR == (ErrorStatus)Value)
			{//由其他任务主动删除
				while(1) OSTimeDly(50);
			}
			Op_Data.BlastStatus = DET_CHARGE_END;
		}
		else  if((u32)p_arg == 7)
		{//主机控制从机发送起爆信号
			Op_Data.BlastStatus = DET_BLASTING;
			SaveBlasteFireTime();//保存起爆时间
			AllSlaveBlastDet();
			
			WriteSystemLog("主机控制从机发送起爆信号",strlen("主机控制从机发送起爆信号"), NEW_STR_INFO);
			if(SlaveInfo.RegCount != 0)
			{
				WriteBlasterRecord();//保存起爆记录
                WriteSystemLog(NULL,0,BLAST_RECORDER_COPY);//把起爆记录保存到日志中
			}
			Op_Data.BlastStatus = DET_BLAST_END;
		}
		else  if((u32)p_arg == 8)
		{//主机控制从机退出测试流程	
			WriteSystemLog("主机控制从机退出测试流程",strlen("主机控制从机退出测试流程"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = NOT_START;
			AllSlaveStatusReset();
		}		
		else  if((u32)p_arg == 21)
		{//雷管注册
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}		
            AppTaskResult.DetOperate = (int)CheckDet(&Op_Data.RegDetInfo, 1);
            if(NEED_REPEAT == AppTaskResult.DetOperate)
            {
                AppTaskResult.DetOperate = (int)CheckDet(&Op_Data.RegDetInfo, 1);
            }
			AllDisCharge();//注册完成后发复位指令，减小芯片出现异常的可能(芯片已经解密)。
			OSTimeDlyHMSM(0,0,0,100);	
		}
		else  if((u32)p_arg == 22)
		{//雷管分析
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}		
			AppTaskResult.DetOperate = (int)DetAnalyst(&Op_Data.RegDetInfo);
//			OSTimeDlyHMSM(0,0,0,500);	
		}
		else  if((u32)p_arg == 24)
		{//屏蔽雷管
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//如果滴答定时器为1KHZ,设备连续运行49.7天才有可能溢出
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{				
				if((Op_Data.LinABCurrent < BusCurrentRangeMax()) && (abs(Op_Data.LinABVoltage - Op_Data.LinNormalLowVoltageGoal) < LIN_NORMAL_LOW_VOLTAGE_LIMIT))
				{
					break;
				}
				OSTimeDlyHMSM(0,0,0,200);
			}			
			WaiLinCurrentSteady(2000);
			
			AppTaskResult.DetOperate = (int)DetMaskAll();
			OSTimeDlyHMSM(0,0,0,100);			
			GetDetAckCurrentWave();
			OSTimeDlyHMSM(0,0,0,500);					
		}
		else if((u32)p_arg == 25)
		{//内码重写
			AppTaskResult.DetOperate = (int)XA3_DetCodeWriteNew(&Op_Data.RegDetInfo.Serl, &Op_Data.RegDetInfo.Pwd);		
		}
		else if((u32)p_arg == 26)
		{//内码重写
			AppTaskResult.DetOperate = XA3_DetCodeWriteFromBackup();		
		}        
		else  if((u32)p_arg == 27)
		{//单发检测
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}		
            AppTaskResult.DetOperate = (int)CheckDet(&Op_Data.RegDetInfo, 0);
            if(NEED_REPEAT == AppTaskResult.DetOperate)
            {
                AppTaskResult.DetOperate = (int)CheckDet(&Op_Data.RegDetInfo, 0);
            }
			AllDisCharge();//注册完成后发复位指令，减小芯片出现异常的可能(芯片已经解密)。
			OSTimeDlyHMSM(0,0,0,100);	
		}
		else  if((u32)p_arg == 100)
		{//发送起爆信号
			AppTaskResult.DetOperate = AllDetStartBlast();           
		}		
		else
		{
			OSTaskDel(OS_PRIO_SELF);		
		}
		OSTaskDel(OS_PRIO_SELF);	
	}
}


/**
  *************************************************************************************
  * @brief  安管任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppSafeMonitorTask(void* p_arg)
{	
	u8 Result;
	WM_MESSAGE Msg;
	u8 GPRSIPString[50];
	u8 Buffer1[20];
	u8 Buffer2[20];
	u16 i;
//	device_info.Server.Ip = IpAddrStringToU32("119.90.4.247");
//	device_info.Server.Port = 20601;
	
	OSTimeDly(10);
	
	Op_Data.WifiResult = ERROR;
	i = IPAddrU32ToString(device_info.Server.Ip, &GPRSIPString[0]);
	sprintf((char*)&GPRSIPString[i],":%d",device_info.Server.Port);

	while(1)
	{		
		i = 3;
		while(i--)
		{
			if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
			{
				if(OpenWifiSocket(device_info.Server.Ip, device_info.Server.Port) == ERROR)
				{
					Op_Data.WifiResult = ERROR;//wifi不可用						
				}
				else
				{
					Op_Data.WifiResult = SUCCESS;//wifi可用
					break;
				}				
			}
			OSTimeDlyHMSM(0,0,0,100);
		}
		//服务器中的时间与设备时间相差超过1分钟则更新设备时间
		if(ERROR != GetServerCurrentTime(Op_Data.WifiResult, Buffer1))
		{
			RTC_Read_Time();
			RtcStructToTimeString(&RTC_Type,Buffer2);
			if(GetTimeDifferSec(Buffer1, Buffer2) >= 60)
			{
				UpdateRtcTime(Buffer1);
			}
		}
		if((u32)p_arg == 0)
		{//上传当前起爆信息
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				Result = SendBlastFireInfoData(0xFFFF, Op_Data.WifiResult);
				if(Result == '1')
				{
					UpdateBlasterRecordUploadState(1,0xFFFF);//更新上传标志	
					Msg.Data.v = (int)(ErrorStatus)SUCCESS;					
				}
				else
				{
					Msg.Data.v = (int)(ErrorStatus)ERROR;
				}			
			}
			else
			{
				Msg.Data.v = (int)(ErrorStatus)ERROR;
			}			

		}		
		else if((u32)p_arg == 1)
		{//上传指定的起爆记录
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))				
			{
				Result = SendBlastFireInfoData(BlastRecorder.Select, Op_Data.WifiResult);
				if(Result == '1')
				{
					UpdateBlasterRecordUploadState(1,BlastRecorder.Select);//更新上传标志
					Msg.Data.v = (int)(ErrorStatus)SUCCESS;
				}
				else
				{
					Msg.Data.v = (int)(ErrorStatus)ERROR;
				}			
			}
			else
			{
				Msg.Data.v = (int)(ErrorStatus)ERROR;
			}			
		}
		else if((u32)p_arg == 2)
		{//固件更新
			while(1)
			{
				if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
				{
					Msg.Data.v = DeviceHexFileLoader(Op_Data.WifiResult);//下载固件
					if ((Msg.Data.v == 1) || (Msg.Data.v == 2) || (Msg.Data.v == 4))
					{
						break;
					}

					//重试
					Msg.MsgId = WM_SOFTWARE_UPDATE;	
					Msg.Data.v = 0;
					
					CustomSendMessage(DlghWin.Dlg, &Msg);
					
					if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
					{
						if(OpenWifiSocket(device_info.Server.Ip, device_info.Server.Port) == ERROR)
						{
							Op_Data.WifiResult = ERROR;//wifi不可用						
						}
						else
						{
							Op_Data.WifiResult = SUCCESS;//wifi可用
						}				
					}													
				}
				else
				{
					Msg.Data.v = 0xFF;
					break;
				}			
			}			
		}
		else if((u32)p_arg == 3)
		{//上传所有未上传的起爆记录
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				for(i=0;i<BlastRecorder.Number;i++)
				{
					OSTimeDly(50);//时间太短，LCD得不到刷屏
					//利用设备充电的时间上传起爆记录
					if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
					{//正在充电						
						if(GetBlastRecoderUploadState(i) == ERROR)
						{//该记录没有上传
							if(GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP)
							{//WIFI已经断开
								Op_Data.WifiResult = ERROR;
							}
							Result = SendBlastFireInfoData(i, Op_Data.WifiResult);
							if(Result == '1')
							{
								UpdateBlasterRecordUploadState(1,i);//更新上传标志
							}					
						}					
					}
				}								
			}
			Msg.Data.v = (int)(ErrorStatus)SUCCESS;		
		}
		else if((u32)p_arg == 4)
		{//检测是否有新固件
			if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				Msg.Data.v = GetServerDeviceVersionInfo(&HexLoader.NewFileSize, &HexLoader.NewVersion[0], sizeof(HexLoader.NewVersion), Op_Data.WifiResult);
				if (Msg.Data.v != 0)
				{
					Op_Data.AutoUpdateStatus = 1;//表示已经检测过
					if (Msg.Data.v == 2)
					{
						Op_Data.AutoUpdateStatus = 2;//表示有新固件
					}
				}													
			}			
		}
		else if((u32)p_arg == 5)
		{//上传指定的日志文件
			Msg.Data.v = (int)(ErrorStatus)ERROR;
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))				
			{
				Result = SendLogFileToServer(LogRecorder.Select, Op_Data.WifiResult);
				if(Result == '1')
				{
					Msg.Data.v = (int)(ErrorStatus)SUCCESS;
				}		
			}
		
		}		
		ExitSafeMonitor();
		
		Msg.MsgId = WM_SAFE_MONITOR_TASK_END;
		
		if(((u32)p_arg != 3) && ((u32)p_arg != 4))
		{
			CustomSendMessage(DlghWin.Dlg, &Msg);
		}
		
		OSTaskDel(OS_PRIO_SELF);
	}
}


/**
  *************************************************************************************
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppKeyScanTask(void* p_arg)
{	
	u8 KeyCode; 
	
	KeyConfig();
	while(1)
	{	
		OSTimeDlyHMSM(0,0,0,1);	
		KeyCode = KeyboardScan();
		if(KeyCode != 0)
		{
			
			Op_Data.SystemIdleTime = OSTimeGet();
			if(GUI_KEY_POWER == KeyCode)
			{
				if(WM_IsWindow(DlghWin.Status))
				{
					WM_SetFocus(DlghWin.Status);
				}
				
				GUI_StoreKeyMsg(KeyCode, 0);//GUI_SendKeyMsg(KeyCode, 0);	
			}
			else
			{
				//if(LcdBrightTimeOutValue != 0xFFFFFFFF)
				//{//当前没有关背光
				if(WM_IsWindow(DlghWin.Dlg))
				{
					WM_SetFocus(DlghWin.Dlg);
				}
				
				GUI_StoreKeyMsg(KeyCode, 0);//GUI_SendKeyMsg(KeyCode, 0);				
				//}							
			}					
			LcdBrightRefreshProcess(1);
		}
		else
		{
			LcdBrightRefreshProcess(0);
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  声音任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2020年3月18日
  * @note   不要删除本任务
  *************************************************************************************  
  */
void AppSoundTask(void* p_arg)
{	
	OSTimeDly(5);	
	SoundOutput((u32)p_arg);
    OSTaskDel(OS_PRIO_SELF);
}
/**
  *************************************************************************************
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppUserGUITask(void* p_arg)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟,使用STemWin必需开启
   
	//系统初始化函数放在这个任务是因为初始时要操作文件，需要大量的缓冲区，其他任务分配的堆栈不够
	//SystemHardwareInit();	
	GUI_Init(); //GUI 初始化
	/* 启用多缓冲 */
	WM_MULTIBUF_Enable(1);
//	WM_EnableMemdev(WM_HBKWIN);
	GUI_UC_SetEncodeUTF8();
	WM_SetCreateFlags(WM_CF_MEMDEV);
    
    if(device_info.TextColor == device_info.BackgroundColor)
    {
        device_info.TextColor = GUI_BLUE;
        device_info.BackgroundColor = COLOR_DLG_BK;    
    }

	SetGuiDefaultColor(device_info.BackgroundColor, device_info.TextColor);
    
	/* 桌面窗口的回调函数 */
	WM_SetCallback(WM_HBKWIN, DeskCallback);	
	
    ShowTopTitleDlg();//开始为隐藏状态    
	if(ERROR == CheckDeviceChargePowerOnState())
	{
		if(LTC2942Init() != ERROR)
		{
			if((LTC2942BatVoltage() <= 3610) && (LTC2942BatAccumulated() <= 3))		
			{//剩余容量3%，禁止开机
				OSTimeDlyHMSM(0,0,0,200);
				if((LTC2942BatVoltage() <= 3610) && (LTC2942BatAccumulated() <= 3))	
				{
					POWER_OFF();
					OSTimeDlyHMSM(0,0,0,100);
					__disable_irq();
					__set_FAULTMASK(1);//关闭所有中断
					NVIC_SystemReset();//软件复位
					while(1);
				}							
			}		
		}
	
		ShowDeviceStartDlg();
	}
	else
	{
		ShowDeviceChargeStartDlg();
	}
		
	//键盘扫描任务
	TaskCreate(APP_KEY_SCAN_TASK_PRIO, (void*)0);
			
	while(1)
	{
		GUI_Exec();
		GUI_Delay(1);
	}	
}

/**
  *************************************************************************************
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{
//	u8 err;
    
	Op_Data.DacHvValueMin = 0xFFFF;
	Op_Data.LinNormalLowVoltageGoal = LIN_NORMAL_LOW_VOLTAGE_L_GOAL;
	Op_Data.LinNormalHighVoltageGoal = LIN_NORMAL_HIGH_VOLTAGE_GOAL;
	setDacHvValueMinFromVoltage(LIN_NORMAL_HIGH_VOLTAGE_ADJUST_MAX);
	FileSystemInitOS();
	
	SystemGpioConfig();	
	OSTimeDlyHMSM(0,0,0,50);


	RTC_Config();
	RTC_Read_Time();
	
	//GUI任务
	TaskCreate(APP_USER_GUI_TASK_PRIO, (void*)0);

	while(1)
	{
		OSTimeDly(50);
		if((ERROR != BusStatusIsIdle()) || (0 == GET_LINAB_SHORT_SW_CTRL_STATE()))
		{//当前没有切换到了5V状态或者当前总线电压已经关闭
			if(GET_DET_RECEIVE_RES_STATE() == DET_RECEIVE_RES_ON)
			{
				Op_Data.LinABCurrent = GetBusCurrent(10,NULL);
			}
			
			Op_Data.LinABVoltage = GetBusVoltage(10, NULL);	
		}
		VoltageOutputAdjust();//调整端口输出电压
        
        if(ERROR != RMC_FormatAnalyze())
        {
            if(GetGPSPassCount() == 1)
            {
                GPS_INFO gps;
                
                if(GetLastGPSInfo(&gps))//校准RTC
                {
                    RTC_Type_Temp.year = gps.Year;
                    RTC_Type_Temp.month = gps.Month;
                    RTC_Type_Temp.day = gps.Day;
                    RTC_Type_Temp.hour = gps.Hour;
                    RTC_Type_Temp.minute = gps.Minute;
                    RTC_Type_Temp.second = gps.Second;
                    RTC_Write_Time(); 
                    
                    SaveGPSInfo(&gps);
                    WriteSystemLog("定位成功!", strlen("定位成功!"), NEW_STR_INFO);
                    WriteSystemLog(gps.Longitude, sizeof(gps.Longitude), STR_INFO);
                    WriteSystemLog(gps.Latitude, sizeof(gps.Latitude), STR_INFO);
                }
            }
            else if(GetGPSPassCount() > 10)
            {
                if(GPSIsWorking())
                {
                    GPS_INFO gps;
                    
                    if(GetLastGPSInfo(&gps))
                    {
                        SaveGPSInfo(&gps);
                    }
                    
                    ReceiveGPSInfoStop();
                }
            }
        }       
	}
}
