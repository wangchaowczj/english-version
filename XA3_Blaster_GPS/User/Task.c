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
  * @brief  HID�豸ͨ������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
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
		{//����
			if(SUCCESS == Unpack(&BlueData, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
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
  * @brief  RS485ͨ������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
				if(GusetSendBuffer.bytenum != 0)
				{
                    //����ģʽ����RS485�м�һ��ʹ��ʱ���ӻ�������Ӧ̫�죬�м̻�û���ý���׼�������ͨѶ�쳣���ӻ�����Ӧǰ��50ms��ʱ��
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
  * @brief  ��ʱ����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��������һЩ�ȽϺ�ʱ�ĺ���,����������ǰ����,����ǿ��ɾ��������
  *************************************************************************************  
  */
void AppTemporaryTask(void* p_arg)
{	
	Op_Data.AppTemporaryTaskResult = 0;
	
	while(1)
	{
		if (((u32)p_arg) == 0)
		{//����ģ��ӻ���ʼ��
			Op_Data.AppTemporaryTaskResult = (u8)BlueSlaveInit();
			if((ErrorStatus)Op_Data.AppTemporaryTaskResult == ERROR)
			{
				WriteSystemLog("����ģ��ӻ���ʼ��ʧ��",strlen("����ģ��ӻ���ʼ��ʧ��"), CURRENT_STATUS);
			}
			else
			{
				WriteSystemLog("����ģ��ӻ���ʼ���ɹ�",strlen("����ģ��ӻ���ʼ���ɹ�"), CURRENT_STATUS);
			}			
		}
		else if (((u32)p_arg) == 1)
		{//����ģ��������ʼ��
			Op_Data.AppTemporaryTaskResult = (u8)BlueHostInit();
			if((ErrorStatus)Op_Data.AppTemporaryTaskResult == ERROR)
			{
				WriteSystemLog("����ģ��������ʼ��ʧ��",strlen("����ģ��������ʼ��ʧ��"), CURRENT_STATUS);
			}
			else
			{
				WriteSystemLog("����ģ��������ʼ���ɹ�",strlen("����ģ��������ʼ���ɹ�"), CURRENT_STATUS);
			}			
		}
		else if (((u32)p_arg) == 2)
		{//����ģ������ɨ��ӻ�
			BlueScanSlave();			
		}
		else if (((u32)p_arg) == 3)
		{//����ģ���������Ӵӻ�
			Op_Data.DKeyVoltage = 0xFFFF;
			Op_Data.AppTemporaryTaskResult = (u8)BlueConnectSlave(Op_Data.ConnectBlueIndex);
			
			//�������Կ�Ļ������ӳɹ����һ�µ�ص�ѹ
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
		{//����ģ�������Ͽ������ӵĴӻ�
			Op_Data.AppTemporaryTaskResult = (u8)BlueHostDisConnect();
		}
		else if (((u32)p_arg) == 4)
		{//�����Լ�(��������)			
			//��������ģ��ͨ���Ƿ�����
			if(ERROR == BlueSlaveInit())//��������ģ��ͨ���Ƿ�����
			{
				Op_Data.AppTemporaryTaskResult = 1;
				WriteSystemLog("��������ģ���쳣",strlen("��������ģ���쳣"), CURRENT_STATUS);
			}
			else
			{
				if(GetBlueVersion(&device_info.BlueVersion[0]) == ERROR)
				{
					Op_Data.AppTemporaryTaskResult = 1;
					WriteSystemLog("��������ģ���쳣",strlen("��������ģ���쳣"), CURRENT_STATUS);			
				}			
			}

			CLOSE_BLUE_POWER();
			
			//����Wifiģ��ͨ���Ƿ�����
			if(Op_Data.AppTemporaryTaskResult == 0)
			{
				u8 WifiPower = GET_WIFI_POWER_STATE();
				
				OPEN_WIFI_POWER();
				if(SUCCESS == WifiInit())
				{
					if(ERROR == GetWifiCurrentAPInfo(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry))
					{
						Op_Data.AppTemporaryTaskResult = 2;//Wifiģ���쳣
						WriteSystemLog("����Wifiģ���쳣",strlen("����Wifiģ���쳣"), CURRENT_STATUS);
					}
				}
				else
				{
					Op_Data.AppTemporaryTaskResult = 2;//Wifiģ���쳣
					WriteSystemLog("����Wifiģ���ʼ���쳣",strlen("����Wifiģ���ʼ���쳣"), CURRENT_STATUS);
				}			
				if(WifiPower != 0)
				{//֮ǰ��Դû�д�
					CLOSE_WIFI_POWER();
				}				
			}
			//���ɨ��ģ���Ƿ�����
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
						WriteSystemLog("����ɨ��ģ���쳣",strlen("����ɨ��ģ���쳣"), CURRENT_STATUS);
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
		{//����ѹоƬ��ص�·�Ƿ�����
			 Op_Data.AppTemporaryTaskResult = (u8)CheckHVProtectPoint();		 
		}		
		OSTaskDel(OS_PRIO_SELF);
	}	
}
/**
  *************************************************************************************
  * @brief  �����ȼ�����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
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
  * @brief  ��̨����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��������һЩ�ȽϺ�ʱ�ĺ���,����������ǰ����,������ǿ��ɾ��������
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
		{//����ѹоƬ��ص�·�Ƿ�����
			 Msg.Data.v = (int)CheckHVProtectPoint();		 
		}
		else if (((u32)p_arg) == 1)
		{//�豸�Լ�,�Լ��ѹ����
			Msg.Data.v = (int)(ErrorStatus)DeviceSelfTest(0x01);
		}
		else if(((u32)p_arg) == 2)
		{//�ȴ����ߵ����ȶ�
            u8 Buffer[20];
            
			WriteSystemLog("��ʼ�ȴ����ߵ����ȶ�",strlen("��ʼ�ȴ����ߵ����ȶ�"), CURRENT_STATUS);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
            
            AllDisCharge();//����ȫ�ָ�λָ��,оƬ�ϵ��ĵ����Ƚ��յ�ָ���ĵ���ҪСһЩ
            
			Msg.Data.v = (int)WaiLinCurrentSteady(3000+Det.RegCount*5);//ע���׹���Խ�࣬��ʱʱ��Խ��
			OSTimeDlyHMSM(0,0,0,100);
			WriteSystemLog("���ߵ����Ѿ��ȶ�",strlen("���ߵ����Ѿ��ȶ�"), CURRENT_STATUS);
			BlastInfo.V_BeforeTest = Op_Data.LinABVoltage;
			BlastInfo.I_BeforeTest = Op_Data.LinABCurrent;
			BlastInfo.I_Leak = GetBusLeakCurrent(NULL);

            sprintf((char*)Buffer, "����©��:%duA", BlastInfo.I_Leak);
            WriteSystemLog(Buffer,strlen((char*)Buffer), STR_INFO);			
		}
		else if (((u32)p_arg) == 3)
		{//�����ݴ���Կ�е���
			Msg.Data.v = (int)BlueReadDetInfoBackUp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4]);
		}
		else if (((u32)p_arg) == 4)
		{//�����ݵ�������Կ
			Msg.Data.v = (int)BlueDetInfoBackUp(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4]);
		}
//		else if (((u32)p_arg) == 5) 
//		{//���ܹ��ܣ�����Ϣ��֤
//			Op_Data.DetVerifyErrIndex = 0;
//			Msg.Data.v = (int)DkeyBlastInfoVerify(&Op_Data.DetVerifyErrIndex);		
//		}
		else if (((u32)p_arg) == 6)
		{//�豸�Լ�,�Լ��ѹ����
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
		{//�豸�Լ�,�Լ����ź�
			Msg.Data.v = (int)(ErrorStatus)DeviceSelfTest(0x04);
		}
		else if (((u32)p_arg) == 11) 
		{//ɨ��
			if(GET_SCAN_POWER_STATE() != 0)
			{//ɨ��ģ���Դ��û�д�
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
		{//�ϲ�����(����<-�ӻ�)
			Msg.Data.v = (int)BlueReadDetInfoInsert(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.InputValue1,Op_Data.InputValue2);
		}
		else if (((u32)p_arg) == 13)
		{//�ϲ�����(����->�ӻ�)
			Msg.Data.v = (int)BlueDetInfoInsert(BlueSlaveList.Info[Op_Data.ConnectBlueIndex].Name[4],Op_Data.InputValue1,Op_Data.InputValue2);
		}
		else if (((u32)p_arg) == 14) 
		{//ͨ��485��������
			Msg.Data.v = BackupImportDetDataFromSlave();
		}
		else if (((u32)p_arg) == 15) 
		{//ͨ��485��������
			Msg.Data.v = BackupExportDetDataToSlave();
		}
		else if (((u32)p_arg) == 16) 
		{//�׹���Ȩ��֤������
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
  * @brief  Wifi����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2018��5��8��
  * @note   
  *************************************************************************************  
  */
void AppWifiTask(void* p_arg)
{
	WM_MESSAGE Msg;
	u32 c_time;
	
	Msg.Data.v = (int)ERROR;
	
	if (((u32)p_arg) == 0) 
	{//��Wifi ����ʼ��
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
				WifiScanAP(&MyWifi.ScanResult);//ɨ������
				OSSchedLock();
				ClearSameWifiScanResult(&MyWifi.ScanResult);
				OSSchedUnlock();
				
				//���WIFIģ��û���Զ������ϣ���ô���ɨ�赽��WIFI�����Ѿ����ӹ��ģ���ѡ������
				c_time = OSTimeGet();
				while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))	
				{
					if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
					{
						break;
					}					
				}
				AutoConnect();
				
				Msg.Data.v = (int)SUCCESS;//Wifiģ������
			}
		}			
	}
	else if (((u32)p_arg) == 1) 
	{//����ָ��WIFI����
		if(ERROR != WifiConnectAP(MyWifi.Ssid,MyWifi.Passwords,MyWifi.Auth,MyWifi.Encry))//��������
		{						
			//����Ѿ��������磬�򱣴�WIFI����
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
	{//ɨ��WIFI����
		if(GET_WIFI_POWER_STATE() == 0)
		{//��Դ�Ѿ���
            AP_SCAN_RESULT ScanResult;
            
			OSTaskDelReq(APP_SAFE_MONITOR_TASK_PRIO);
			FileSystemLock();
			WifiLock();				
			OSTaskDel(APP_SAFE_MONITOR_TASK_PRIO);
			WifiUnlock();
			FileSystemUnlock();
			
			ExitSafeMonitor();
			
			WifiScanAP(&ScanResult);//ɨ������
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
	{//��ʱɨ��WIFI���粢�Զ�����
		while(1)
		{
			OSTimeDlyHMSM(0,0,10,0);
			if (GET_WIFI_POWER_STATE() == 0)
			{
				if (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP)
				{//���ӷ���������ʱ���Լ��ж��Ƿ���Ҫ���³���,������ֻ��ѯһ��
					if(Op_Data.AutoUpdateStatus == 0)
					{
						TaskCreate(APP_SAFE_MONITOR_TASK_PRIO, (void*)4);
					}									
				}
				else
				{//��û����
					WifiScanAP(&MyWifi.ScanResult);//ɨ������			
					
					ClearSameWifiScanResult(&MyWifi.ScanResult);
					
					AutoConnect();//���ɨ�赽��WIFI�����Ѿ����ӹ��ģ���ѡ������				
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
  * @brief  �׹ܲ�������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   �����в�Ҫ���빫����Դ����Ϊ���������п��ܻ�ɾ��������
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
		{//���ٲ���
			WriteSystemLog("��ʼ��������",strlen("��ʼ��������"), CURRENT_STATUS);
			
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
			OSTimeDlyHMSM(0,0,2,500);//400���׹�ʱ�����Ҫ1.8��
			
			WriteSystemLog("�������Խ���",strlen("�������Խ���"), CURRENT_STATUS);				
		}
		else if((u32)p_arg == 1)
		{//��ȫ����
			WriteSystemLog("��ʼ��·����",strlen("��ʼ��·����"), CURRENT_STATUS);
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
			OSTimeDlyHMSM(0,0,2,500);//400���׹�ʱ�����Ҫ1.8��
			
			WriteSystemLog("��·���Խ���",strlen("��·���Խ���"), CURRENT_STATUS);						
		}
		else  if((u32)p_arg == 2)
		{//�����̲���
			WriteSystemLog("��ʼ�����̲���",strlen("��ʼ�����̲���"), CURRENT_STATUS);
			
			Op_Data.ForcePowerOffSwitch = 0;//�������н�ֹ�Զ��ػ�
            Op_Data.OfflineMonitorBlastProcess = 0;
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
			OSTimeDlyHMSM(0,0,2,500);//400���׹�ʱ�����Ҫ1.8��
            if(DEVICE_USAGES_ALONE == device_info.DeviceUsages)
            {//���ڼ�����ߵ���
                if (GetDetOnlineCount() > 0)
                {
                    Op_Data.OfflineMonitorBlastProcess = 1;
                }
            }			
			WriteSystemLog("�����̲��Խ���",strlen("�����̲��Խ���"), CURRENT_STATUS);
			
			Op_Data.BlastStatus = DET_TEST_END;		
		}
		else  if((u32)p_arg == 3)
		{//��·���
			WriteSystemLog("��ʼ��·���",strlen("��ʼ��·���"), CURRENT_STATUS);
			
			OSTimeDlyHMSM(0,0,0,50);
			BlastInfo.V_BeforeCharge = Op_Data.LinABVoltage;
			BlastInfo.I_BeforeCharge = Op_Data.LinABCurrent;
			
			Op_Data.LinAdjustTime = OSTimeGet() + 5000;//�������ѹ,����̫��������ϵ����п��ܻ���ֵ�ѹ̫�ߣ�Ӳ������
			OPEN_HV_SW();//��ѹоƬ�����ĸ�ѹ���������������
			
			ClearProgbarData();
			Op_Data.Progbar.Bar[0].Max = 100;			
			Op_Data.BlastStatus = DET_CHARGING;
			Value = 0;
			
			for(i=0; i < 100; i++)
			{
				Op_Data.Progbar.Bar[Op_Data.Progbar.Index].Value++;//���ӽ�����
				if (abs(Op_Data.LinABVoltage - Op_Data.LinNormalHighVoltageGoal) > LIN_NORMAL_HIGH_VOLTAGE_LIMIT)
				{
					OSTimeDlyHMSM(0,0,0,500);
					Value = 0;
					continue;
				}				
				if(Value < ((Det.RegCount>100)?20:10))
				{//����û���ȶ�
					if(ERROR != WaiLinCurrentSteady(500))
					{//�����Ѿ��ȶ�
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
					WriteSystemLog("��ѹ��ѹ��·DA��Сֵ:",strlen("��ѹ��ѹ��·DA��Сֵ:"), NEW_STR_INFO);
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
			FillCurrentProgbar();//���½�������100%
			
			WriteSystemLog("���������",strlen("���������"), CURRENT_STATUS);
			
			WriteSystemLog("��ѹ��ѹ��·DA��Сֵ:",strlen("��ѹ��ѹ��·DA��Сֵ:"), NEW_STR_INFO);
			WriteSystemLog(&Op_Data.DacHvValueMin,sizeof(Op_Data.DacHvValueMin), U16_DATA);	
			
			
			BlastInfo.V_AfterCharge = Op_Data.LinABVoltage;
			BlastInfo.I_AfterCharge = Op_Data.LinABCurrent;
			BlastInfo.I_AfterChargeMin = Op_Data.LinABCurrent;
			BlastInfo.I_AfterChargeMax = Op_Data.LinABCurrent;
			
			Op_Data.BlastStatus = DET_CHARGE_END;
			Op_Data.ChargeCurrentVaryFlag = 1;//��ʼ��¼���߷�ֵ�仯
		}
		else  if((u32)p_arg == 4)
		{//�������ӻ����׹ܿ��ٲ���
			WriteSystemLog("������ʼ���ƴӻ����ٲ���",strlen("������ʼ���ƴӻ����ٲ���"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = DET_TESTING;
			SET_RS485_SEND();//485����Ϊ����״̬
			Value = AllSlaveDetTest(0, GetMasterTimeShareTest());
			SET_RS485_SEND();//485����Ϊ����״̬
			
			WriteSystemLog("�������ƴӻ����ٲ��Խ���",strlen("�������ƴӻ����ٲ��Խ���"), NEW_STR_INFO);
			
			if(ERROR == (ErrorStatus)Value)
			{//��������������ɾ��
				while(1) OSTimeDly(50);
			}			
			Op_Data.BlastStatus = DET_TEST_END;
		}
		else  if((u32)p_arg == 5)
		{//�������ƴӻ������������׹ܲ���
			WriteSystemLog("�������ƴӻ����������̲���",strlen("�������ƴӻ����������̲���"), NEW_STR_INFO);
			
			Op_Data.ForcePowerOffSwitch = 0;//�������н�ֹ�Զ��ػ�
			Op_Data.BlastStatus = DET_TESTING;
			SET_RS485_SEND();//485����Ϊ����״̬
			Value = AllSlaveDetTest(1, GetMasterTimeShareTest());
			SET_RS485_SEND();//485����Ϊ����״̬
			
			WriteSystemLog("�������ƴӻ����������̲��Խ���",strlen("�������ƴӻ����������̲��Խ���"), NEW_STR_INFO);
			if(ERROR == (ErrorStatus)Value)
			{//��������������ɾ��
				while(1) OSTimeDly(50);
			}
			Op_Data.BlastStatus = DET_TEST_END;
		}
		else  if((u32)p_arg == 6)
		{//�������ƴӻ�������
			WriteSystemLog("�������ƴӻ�������",strlen("�������ƴӻ�������"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = DET_CHARGING;
			SET_RS485_SEND();//485����Ϊ����״̬
			Value = AllSlaveCharge();
			SET_RS485_SEND();//485����Ϊ����״̬
			
			WriteSystemLog("�������ƴӻ����������",strlen("�������ƴӻ����������"), NEW_STR_INFO);
			if(ERROR == (ErrorStatus)Value)
			{//��������������ɾ��
				while(1) OSTimeDly(50);
			}
			Op_Data.BlastStatus = DET_CHARGE_END;
		}
		else  if((u32)p_arg == 7)
		{//�������ƴӻ��������ź�
			Op_Data.BlastStatus = DET_BLASTING;
			SaveBlasteFireTime();//������ʱ��
			AllSlaveBlastDet();
			
			WriteSystemLog("�������ƴӻ��������ź�",strlen("�������ƴӻ��������ź�"), NEW_STR_INFO);
			if(SlaveInfo.RegCount != 0)
			{
				WriteBlasterRecord();//�����𱬼�¼
                WriteSystemLog(NULL,0,BLAST_RECORDER_COPY);//���𱬼�¼���浽��־��
			}
			Op_Data.BlastStatus = DET_BLAST_END;
		}
		else  if((u32)p_arg == 8)
		{//�������ƴӻ��˳���������	
			WriteSystemLog("�������ƴӻ��˳���������",strlen("�������ƴӻ��˳���������"), NEW_STR_INFO);
			
			Op_Data.BlastStatus = NOT_START;
			AllSlaveStatusReset();
		}		
		else  if((u32)p_arg == 21)
		{//�׹�ע��
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
			AllDisCharge();//ע����ɺ󷢸�λָ���СоƬ�����쳣�Ŀ���(оƬ�Ѿ�����)��
			OSTimeDlyHMSM(0,0,0,100);	
		}
		else  if((u32)p_arg == 22)
		{//�׹ܷ���
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}		
			AppTaskResult.DetOperate = (int)DetAnalyst(&Op_Data.RegDetInfo);
//			OSTimeDlyHMSM(0,0,0,500);	
		}
		else  if((u32)p_arg == 24)
		{//�����׹�
			if(0 != GET_HV_SW_CTRL_STATE())
			{
				CLOSE_HV_SW();
				OSTimeDlyHMSM(0,0,0,500);
			}
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
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
		{//������д
			AppTaskResult.DetOperate = (int)XA3_DetCodeWriteNew(&Op_Data.RegDetInfo.Serl, &Op_Data.RegDetInfo.Pwd);		
		}
		else if((u32)p_arg == 26)
		{//������д
			AppTaskResult.DetOperate = XA3_DetCodeWriteFromBackup();		
		}        
		else  if((u32)p_arg == 27)
		{//�������
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
			AllDisCharge();//ע����ɺ󷢸�λָ���СоƬ�����쳣�Ŀ���(оƬ�Ѿ�����)��
			OSTimeDlyHMSM(0,0,0,100);	
		}
		else  if((u32)p_arg == 100)
		{//�������ź�
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
  * @brief  ��������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
					Op_Data.WifiResult = ERROR;//wifi������						
				}
				else
				{
					Op_Data.WifiResult = SUCCESS;//wifi����
					break;
				}				
			}
			OSTimeDlyHMSM(0,0,0,100);
		}
		//�������е�ʱ�����豸ʱ������1����������豸ʱ��
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
		{//�ϴ���ǰ����Ϣ
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				Result = SendBlastFireInfoData(0xFFFF, Op_Data.WifiResult);
				if(Result == '1')
				{
					UpdateBlasterRecordUploadState(1,0xFFFF);//�����ϴ���־	
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
		{//�ϴ�ָ�����𱬼�¼
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))				
			{
				Result = SendBlastFireInfoData(BlastRecorder.Select, Op_Data.WifiResult);
				if(Result == '1')
				{
					UpdateBlasterRecordUploadState(1,BlastRecorder.Select);//�����ϴ���־
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
		{//�̼�����
			while(1)
			{
				if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
				{
					Msg.Data.v = DeviceHexFileLoader(Op_Data.WifiResult);//���ع̼�
					if ((Msg.Data.v == 1) || (Msg.Data.v == 2) || (Msg.Data.v == 4))
					{
						break;
					}

					//����
					Msg.MsgId = WM_SOFTWARE_UPDATE;	
					Msg.Data.v = 0;
					
					CustomSendMessage(DlghWin.Dlg, &Msg);
					
					if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP))
					{
						if(OpenWifiSocket(device_info.Server.Ip, device_info.Server.Port) == ERROR)
						{
							Op_Data.WifiResult = ERROR;//wifi������						
						}
						else
						{
							Op_Data.WifiResult = SUCCESS;//wifi����
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
		{//�ϴ�����δ�ϴ����𱬼�¼
			if ((GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				for(i=0;i<BlastRecorder.Number;i++)
				{
					OSTimeDly(50);//ʱ��̫�̣�LCD�ò���ˢ��
					//�����豸����ʱ���ϴ��𱬼�¼
					if(GET_CHARGE_SIGNAL_STATE() == CHARGING)
					{//���ڳ��						
						if(GetBlastRecoderUploadState(i) == ERROR)
						{//�ü�¼û���ϴ�
							if(GET_WIFI_NLINK_STATE() != WIFI_CONNECT_AP)
							{//WIFI�Ѿ��Ͽ�
								Op_Data.WifiResult = ERROR;
							}
							Result = SendBlastFireInfoData(i, Op_Data.WifiResult);
							if(Result == '1')
							{
								UpdateBlasterRecordUploadState(1,i);//�����ϴ���־
							}					
						}					
					}
				}								
			}
			Msg.Data.v = (int)(ErrorStatus)SUCCESS;		
		}
		else if((u32)p_arg == 4)
		{//����Ƿ����¹̼�
			if ((GET_WIFI_POWER_STATE() == 0) && (GET_WIFI_NLINK_STATE() == WIFI_CONNECT_AP) && (Op_Data.WifiResult != ERROR))
			{
				Msg.Data.v = GetServerDeviceVersionInfo(&HexLoader.NewFileSize, &HexLoader.NewVersion[0], sizeof(HexLoader.NewVersion), Op_Data.WifiResult);
				if (Msg.Data.v != 0)
				{
					Op_Data.AutoUpdateStatus = 1;//��ʾ�Ѿ�����
					if (Msg.Data.v == 2)
					{
						Op_Data.AutoUpdateStatus = 2;//��ʾ���¹̼�
					}
				}													
			}			
		}
		else if((u32)p_arg == 5)
		{//�ϴ�ָ������־�ļ�
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
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
				//{//��ǰû�йر���
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
  * @brief  ��������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2020��3��18��
  * @note   ��Ҫɾ��������
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
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void AppUserGUITask(void* p_arg)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��,ʹ��STemWin���迪��
   
	//ϵͳ��ʼ���������������������Ϊ��ʼʱҪ�����ļ�����Ҫ�����Ļ������������������Ķ�ջ����
	//SystemHardwareInit();	
	GUI_Init(); //GUI ��ʼ��
	/* ���ö໺�� */
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
    
	/* ���洰�ڵĻص����� */
	WM_SetCallback(WM_HBKWIN, DeskCallback);	
	
    ShowTopTitleDlg();//��ʼΪ����״̬    
	if(ERROR == CheckDeviceChargePowerOnState())
	{
		if(LTC2942Init() != ERROR)
		{
			if((LTC2942BatVoltage() <= 3610) && (LTC2942BatAccumulated() <= 3))		
			{//ʣ������3%����ֹ����
				OSTimeDlyHMSM(0,0,0,200);
				if((LTC2942BatVoltage() <= 3610) && (LTC2942BatAccumulated() <= 3))	
				{
					POWER_OFF();
					OSTimeDlyHMSM(0,0,0,100);
					__disable_irq();
					__set_FAULTMASK(1);//�ر������ж�
					NVIC_SystemReset();//�����λ
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
		
	//����ɨ������
	TaskCreate(APP_KEY_SCAN_TASK_PRIO, (void*)0);
			
	while(1)
	{
		GUI_Exec();
		GUI_Delay(1);
	}	
}

/**
  *************************************************************************************
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
	
	//GUI����
	TaskCreate(APP_USER_GUI_TASK_PRIO, (void*)0);

	while(1)
	{
		OSTimeDly(50);
		if((ERROR != BusStatusIsIdle()) || (0 == GET_LINAB_SHORT_SW_CTRL_STATE()))
		{//��ǰû���л�����5V״̬���ߵ�ǰ���ߵ�ѹ�Ѿ��ر�
			if(GET_DET_RECEIVE_RES_STATE() == DET_RECEIVE_RES_ON)
			{
				Op_Data.LinABCurrent = GetBusCurrent(10,NULL);
			}
			
			Op_Data.LinABVoltage = GetBusVoltage(10, NULL);	
		}
		VoltageOutputAdjust();//�����˿������ѹ
        
        if(ERROR != RMC_FormatAnalyze())
        {
            if(GetGPSPassCount() == 1)
            {
                GPS_INFO gps;
                
                if(GetLastGPSInfo(&gps))//У׼RTC
                {
                    RTC_Type_Temp.year = gps.Year;
                    RTC_Type_Temp.month = gps.Month;
                    RTC_Type_Temp.day = gps.Day;
                    RTC_Type_Temp.hour = gps.Hour;
                    RTC_Type_Temp.minute = gps.Minute;
                    RTC_Type_Temp.second = gps.Second;
                    RTC_Write_Time(); 
                    
                    SaveGPSInfo(&gps);
                    WriteSystemLog("��λ�ɹ�!", strlen("��λ�ɹ�!"), NEW_STR_INFO);
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
