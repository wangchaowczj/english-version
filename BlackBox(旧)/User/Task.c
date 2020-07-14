#include "includes.h"
#include "usb_bsp.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbd_hid_core.h"
#include "usb_dcd_int.h"
#include "usb_hcd_int.h"


extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;
extern __ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;


STR_COMM  HidData = {0,RECIEVING,0};
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
}
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
void AppCmdSaveTask(void* p_arg)
{	
	INT8U Err;
	DET_DATA_STRUCT *DetDataStruct;	

    MsgQeueInit();
	while(1)
	{
		DetDataStruct = (DET_DATA_STRUCT*)OSQPend(MsgSem.MsgQeue, 0, &Err);//��ȡ��Ϣָ��
        if(DetDataStruct != NULL)
        {
            WriteSystemLog(DetDataStruct, sizeof(DET_DATA_STRUCT), DET_DATA_TYPE);//��������
            PutDetDataStruct(DetDataStruct);//�ͷ���Ϣ��ռ�õ��ڴ��            
        }
	}
}
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
void AppLedTask(void* p_arg)
{		
	while(1)
	{
		SET_LED2_RED_PIN_TOGGLE();
        OSTimeDly(500);
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
    
//	FileSystemInitOS();
	
	SystemGpioConfig();	
    if(0 != SystemHardwareInit())
    {
        while(1)
        {
            OPEN_LED1_RED();
            OPEN_LED2_RED() ;
        }
    }
    usb_msc_device_init();
    
	OSTimeDlyHMSM(0,0,0,50);
    
	TaskCreate(APP_LED_TASK_PRIO, (void*)0);
    TaskCreate(APP_CMD_SAVE_TASK_PRIO, (void*)0);
    
	while(1)
	{
		OSTimeDly(50);
	}
}
