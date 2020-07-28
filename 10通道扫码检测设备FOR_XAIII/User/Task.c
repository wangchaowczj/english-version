#include "GuiIncludes.h"
#include "includes.h"

volatile u32 RS485RecTimeLatest = 0; 
volatile u32 RS232RecTimeLatest  = 0; 
volatile u8 MCU_IO_IN1_Flag = 0; 

STR_COMM  HidData ;//数据帧格式
STR_COMM  RS485Data ;
STR_COMM  RS232Data ;
MSG_SEM MsgSem;
OP_DATA OpData;
DEVICE_INFO DeviceInfo;
DLG_HWIN DlghWin;
USER_PARAM UserParam;

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
void AppUsbTask(void* p_arg)
{	
	INT8U Err;
		
	MsgSem.usb_rcv =  OSSemCreate(0);
	
	while(1)
	{
		OSSemPend(MsgSem.usb_rcv,0,&Err);
		SetSystemSleepCount(OSTimeGet());
		if(RECIEVED == HidData.status)
		{//usb
			if(SUCCESS == Unpack(&HidData, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
				if(GusetSendBuffer.bytenum != 0)
				{
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					DeviceHidSendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			HidData.bytenum = 0;
			HidData.status = RECIEVING;		
		}	
	}
}

/**
  *************************************************************************************
  * @brief  485/232通信任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   用于管厂打码接口
  *************************************************************************************  
  */
void AppFactoryApiTask(void* p_arg)
{
    u32 baudRate = 115200; 
    
//    if((UserParam.NetModeFactoryCode == 34) && (UserParam.Stage == SEMIFINISHED))
//    {//江西国泰，半成品检
//        baudRate = 9600;
//    }
    
    SetUartBaudRate(RS232_COMM, baudRate);
    SetUartBaudRate(RS485_COMM, baudRate);
    
    RS232Data.bytenum = 0;
	RS232Data.status = RECIEVING;
    
    RS485Data.bytenum = 0;
	RS485Data.status = RECIEVING;
    
    ExtiConfig(MCU_IO_IN1_EXTI_LINE, EXTI_Trigger_Falling, ENABLE);
	MCU_IO_IN1_Flag = 0;
    
	while(1)
	{
		OSTimeDly(1);
		if((RS232Data.bytenum > 0) && (RS232Data.status == RECIEVING) && (OSTimeGet() - RS232RecTimeLatest > 5))
		{            
			RS232Data.status = RECIEVED;
            GusetSendBuffer.bytenum = 0;
            
			if(UserParam.NetModeFactoryCode == 34)
            {
                RespondForFactory34(&RS232Data, &GusetSendBuffer);//解析数据
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS232SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			if(UserParam.NetModeFactoryCode == 52)
            {
                RespondForFactory52(&RS232Data, &GusetSendBuffer);//解析数据
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS232SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			else if(UserParam.NetModeFactoryCode == 10)
            {
                if(SUCCESS == Unpack(&RS232Data, &GusetTempBuffer))
                {
                    RespondForFactory10(&GusetTempBuffer, &GusetSendBuffer);//解析数据
                    Pack(&GusetSendBuffer, &GusetTempBuffer);
                    if(GusetTempBuffer.bytenum > 0)
                    {
                        RS232SendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
                    }
                } 
            }			
			
			RS232Data.bytenum = 0;
			RS232Data.status = RECIEVING;//重新开始接收
            
            SetSystemSleepCount(OSTimeGet());
		}
		if((RS485Data.bytenum > 0) && (RS485Data.status == RECIEVING) && (OSTimeGet() - RS485RecTimeLatest > 5))
		{            
			RS485Data.status = RECIEVED;
            GusetSendBuffer.bytenum = 0;
            
			if(UserParam.NetModeFactoryCode == 34)
            {
                RespondForFactory34(&RS485Data, &GusetSendBuffer);//解析数据
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS485SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			if(UserParam.NetModeFactoryCode == 52)
            {
                RespondForFactory52(&RS485Data, &GusetSendBuffer);//解析数据
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS485SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			else if(UserParam.NetModeFactoryCode == 10)
            {
                if(SUCCESS == Unpack(&RS485Data, &GusetTempBuffer))
                {
                    RespondForFactory10(&GusetTempBuffer, &GusetSendBuffer);//解析数据
                    Pack(&GusetSendBuffer, &GusetTempBuffer);
                    if(GusetTempBuffer.bytenum > 0)
                    {
                        RS485SendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
                    }
                }                
            }						
			
			RS485Data.bytenum = 0;
			RS485Data.status = RECIEVING;//重新开始接收

            SetSystemSleepCount(OSTimeGet());
		}
        if(MCU_IO_IN1_Flag)
        {
            MCU_IO_IN1_Flag = 0;
            
            SetSystemSleepCount(OSTimeGet());

            if((OpData.TestStatus == TEST_WAIT_START) && (NeedScan() == 0))
            {
                DetTestStart();//开始检测雷管
            }            
        }
	}
}

/**
  *************************************************************************************
  * @brief  CH374主机任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppCH374Task(void* p_arg)
{
	while(1)
	{//主机
		if (((u32)p_arg) == 0)
		{//主机
			Ch374HostInit(SUPPORT_DEVICE_MSC|SUPPORT_DEVICE_KEYBOARD);		
		}
		else
		{
			Ch374HostInit(SUPPORT_DEVICE_MOUSE|SUPPORT_DEVICE_MSC|SUPPORT_DEVICE_KEYBOARD);
		}
		HostProcess();
	}
}
/**
  *************************************************************************************
  * @brief  键盘扫描任务
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
			SetSystemSleepCount(OSTimeGet());
			if(GUI_CUSTOM_KEY_POWER == KeyCode)
			{
				if(WM_IsWindow(DlghWin.Status))
				{
					WM_SetFocus(DlghWin.Status);
					GUI_StoreKeyMsg(KeyCode, 0);
				}				
			}
			else
			{
				if(WM_IsWindow(DlghWin.Dlg))
				{
					WM_SetFocus(DlghWin.Dlg);
					GUI_StoreKeyMsg(KeyCode, 0);//GUI_SendKeyMsg(KeyCode, 0);					
				}										
			}			
		}
	}	
}
/**
  *************************************************************************************
  * @brief  声音任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppSoundTask(void* p_arg)
{
	while(1)
	{
		OSTimeDly(5);

		Sound((u32)p_arg);

		OSTaskDel(OS_PRIO_SELF);//删除任务
	}	
}
/**
  *************************************************************************************
  * @brief  雷管测试任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void DetDetectorTask(void* p_arg)
{
	OpData.DetDetectorTaskResult = 0;

	while(1)
	{
		OSTimeDly(5);
		if (((u32)p_arg) == 0)
		{//通道雷管上线检测
			QueryDetConnectStatus();
		}
		else if (((u32)p_arg) == 1)
		{//雷管检测
			DetTest();		
		}
		else if (((u32)p_arg) == 2)
		{//等待所有雷管下线
			WaitAllChannelDetOff();	
//            if(UserParam.NetMode)
//            {//联网模式，还需要等待数据被读走
//                if(UserParam.NetModeFactoryCode == 10)//内蒙
//                {
//                    while(OpData.NetModeNext == 0)
//                    {
//                        OSTimeDly(1);
//                    }
//                    while(abs(OSTimeGet() - OpData.NetModeNext) < 1000)//主要是为了留时间给主机重试
//                    {
//                        OSTimeDly(1);
//                    }                
//                }
//            }            
		}		
		else if (((u32)p_arg) == 3)
		{//雷管分析
			OpData.DetDetectorTaskResult = DetAnalyst();		
		}
		WM_SendMessageNoPara(DlghWin.Dlg, WM_DET_TASK);
		OSTaskDel(OS_PRIO_SELF);//删除任务
	}
}
/**
  *************************************************************************************
  * @brief  后台任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2015年6月8日
  * @note   用于运行一些比较耗时的函数,避免阻塞当前进程,不允许强行删除本任务
  *************************************************************************************  
  */
void AppBackgroundTask(void* p_arg)
{
	OpData.AppBackgroundTaskResult = 0;
	
	OSTimeDly(10);	
	
	while(1)
	{		
		if (((u32)p_arg) == 0)
		{//拷入HEX文件
			OSTimeDly(200);	
			if(ERROR != CopyHexFileToSdCard())
			{
				OpData.AppBackgroundTaskResult = 1;
			}		
		}
		else if (((u32)p_arg) == 1)
		{//自检
			OpData.AppBackgroundTaskResult = DeviceSelfTest();
		}
		else if (((u32)p_arg) == 2)
		{//导出检测数据
			OpData.AppBackgroundTaskResult = (u8)CopyDetTestFileToMsc(DetTestRecorder.Select);
		}
		else if (((u32)p_arg) == 3)
		{//导出三码绑定数据
			OpData.AppBackgroundTaskResult = (u8)CopyDetDataBindingFileToMsc(DetDataBindingRecorder.Select);
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
	//系统初始化函数放在这个任务是因为初始时要操作文件，需要大量的缓冲区，其他任务分配的堆栈不够
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟,使用STemWin必需开启
	GUI_Init();  										//emwin初始化
	DeskMenu();	
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
	
	SystemGpioConfig();	
	
	//Hid通信任务
	TaskCreate(APP_USB_TASK_PRIO, (void*)0);		
	//GUI任务
	TaskCreate(APP_USER_GUI_TASK_PRIO, (void*)0);
	DlghWin.Dlg = WM_HBKWIN;
	
	TaskCreate(APP_KEYSCAN_TASK_PRIO, (void*)0);

		
	OpData.UsbState = 0x00;
	while(1)
	{
		OSTimeDly(1);
		UsbAutoIdentify();
	}
}
