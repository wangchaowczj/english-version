#include "GuiIncludes.h"
#include "includes.h"

volatile u32 RS485RecTimeLatest = 0; 
volatile u32 RS232RecTimeLatest  = 0; 
volatile u8 MCU_IO_IN1_Flag = 0; 

STR_COMM  HidData ;//����֡��ʽ
STR_COMM  RS485Data ;
STR_COMM  RS232Data ;
MSG_SEM MsgSem;
OP_DATA OpData;
DEVICE_INFO DeviceInfo;
DLG_HWIN DlghWin;
USER_PARAM UserParam;

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
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
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
  * @brief  485/232ͨ������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ���ڹܳ�����ӿ�
  *************************************************************************************  
  */
void AppFactoryApiTask(void* p_arg)
{
    u32 baudRate = 115200; 
    
//    if((UserParam.NetModeFactoryCode == 34) && (UserParam.Stage == SEMIFINISHED))
//    {//������̩�����Ʒ��
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
                RespondForFactory34(&RS232Data, &GusetSendBuffer);//��������
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS232SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			if(UserParam.NetModeFactoryCode == 52)
            {
                RespondForFactory52(&RS232Data, &GusetSendBuffer);//��������
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS232SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			else if(UserParam.NetModeFactoryCode == 10)
            {
                if(SUCCESS == Unpack(&RS232Data, &GusetTempBuffer))
                {
                    RespondForFactory10(&GusetTempBuffer, &GusetSendBuffer);//��������
                    Pack(&GusetSendBuffer, &GusetTempBuffer);
                    if(GusetTempBuffer.bytenum > 0)
                    {
                        RS232SendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
                    }
                } 
            }			
			
			RS232Data.bytenum = 0;
			RS232Data.status = RECIEVING;//���¿�ʼ����
            
            SetSystemSleepCount(OSTimeGet());
		}
		if((RS485Data.bytenum > 0) && (RS485Data.status == RECIEVING) && (OSTimeGet() - RS485RecTimeLatest > 5))
		{            
			RS485Data.status = RECIEVED;
            GusetSendBuffer.bytenum = 0;
            
			if(UserParam.NetModeFactoryCode == 34)
            {
                RespondForFactory34(&RS485Data, &GusetSendBuffer);//��������
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS485SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			if(UserParam.NetModeFactoryCode == 52)
            {
                RespondForFactory52(&RS485Data, &GusetSendBuffer);//��������
                if(GusetSendBuffer.bytenum > 0)
                {
                    RS485SendData(&GusetSendBuffer.uch_buf[0], GusetSendBuffer.bytenum);
                }
            }
			else if(UserParam.NetModeFactoryCode == 10)
            {
                if(SUCCESS == Unpack(&RS485Data, &GusetTempBuffer))
                {
                    RespondForFactory10(&GusetTempBuffer, &GusetSendBuffer);//��������
                    Pack(&GusetSendBuffer, &GusetTempBuffer);
                    if(GusetTempBuffer.bytenum > 0)
                    {
                        RS485SendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
                    }
                }                
            }						
			
			RS485Data.bytenum = 0;
			RS485Data.status = RECIEVING;//���¿�ʼ����

            SetSystemSleepCount(OSTimeGet());
		}
        if(MCU_IO_IN1_Flag)
        {
            MCU_IO_IN1_Flag = 0;
            
            SetSystemSleepCount(OSTimeGet());

            if((OpData.TestStatus == TEST_WAIT_START) && (NeedScan() == 0))
            {
                DetTestStart();//��ʼ����׹�
            }            
        }
	}
}

/**
  *************************************************************************************
  * @brief  CH374��������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void AppCH374Task(void* p_arg)
{
	while(1)
	{//����
		if (((u32)p_arg) == 0)
		{//����
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
  * @brief  ����ɨ������
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
  * @brief  ��������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void AppSoundTask(void* p_arg)
{
	while(1)
	{
		OSTimeDly(5);

		Sound((u32)p_arg);

		OSTaskDel(OS_PRIO_SELF);//ɾ������
	}	
}
/**
  *************************************************************************************
  * @brief  �׹ܲ�������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
  *************************************************************************************  
  */
void DetDetectorTask(void* p_arg)
{
	OpData.DetDetectorTaskResult = 0;

	while(1)
	{
		OSTimeDly(5);
		if (((u32)p_arg) == 0)
		{//ͨ���׹����߼��
			QueryDetConnectStatus();
		}
		else if (((u32)p_arg) == 1)
		{//�׹ܼ��
			DetTest();		
		}
		else if (((u32)p_arg) == 2)
		{//�ȴ������׹�����
			WaitAllChannelDetOff();	
//            if(UserParam.NetMode)
//            {//����ģʽ������Ҫ�ȴ����ݱ�����
//                if(UserParam.NetModeFactoryCode == 10)//����
//                {
//                    while(OpData.NetModeNext == 0)
//                    {
//                        OSTimeDly(1);
//                    }
//                    while(abs(OSTimeGet() - OpData.NetModeNext) < 1000)//��Ҫ��Ϊ����ʱ�����������
//                    {
//                        OSTimeDly(1);
//                    }                
//                }
//            }            
		}		
		else if (((u32)p_arg) == 3)
		{//�׹ܷ���
			OpData.DetDetectorTaskResult = DetAnalyst();		
		}
		WM_SendMessageNoPara(DlghWin.Dlg, WM_DET_TASK);
		OSTaskDel(OS_PRIO_SELF);//ɾ������
	}
}
/**
  *************************************************************************************
  * @brief  ��̨����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2015��6��8��
  * @note   ��������һЩ�ȽϺ�ʱ�ĺ���,����������ǰ����,������ǿ��ɾ��������
  *************************************************************************************  
  */
void AppBackgroundTask(void* p_arg)
{
	OpData.AppBackgroundTaskResult = 0;
	
	OSTimeDly(10);	
	
	while(1)
	{		
		if (((u32)p_arg) == 0)
		{//����HEX�ļ�
			OSTimeDly(200);	
			if(ERROR != CopyHexFileToSdCard())
			{
				OpData.AppBackgroundTaskResult = 1;
			}		
		}
		else if (((u32)p_arg) == 1)
		{//�Լ�
			OpData.AppBackgroundTaskResult = DeviceSelfTest();
		}
		else if (((u32)p_arg) == 2)
		{//�����������
			OpData.AppBackgroundTaskResult = (u8)CopyDetTestFileToMsc(DetTestRecorder.Select);
		}
		else if (((u32)p_arg) == 3)
		{//�������������
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
	//ϵͳ��ʼ���������������������Ϊ��ʼʱҪ�����ļ�����Ҫ�����Ļ������������������Ķ�ջ����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��,ʹ��STemWin���迪��
	GUI_Init();  										//emwin��ʼ��
	DeskMenu();	
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
	
	SystemGpioConfig();	
	
	//Hidͨ������
	TaskCreate(APP_USB_TASK_PRIO, (void*)0);		
	//GUI����
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
