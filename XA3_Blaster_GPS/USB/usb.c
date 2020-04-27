//=================================================================================
// 版权: 
//---------------------------------------------------------------------------------
// 作者：    时间：2013-04-19     版本：V1.0.0
//---------------------------------------------------------------------------------
// 修改历史：
// 修改人：        修改时间：           修改内容：	     检查人：
//---------------------------------------------------------------------------------
// 文件：cam_ptc06.c
//---------------------------------------------------------------------------------
// 介绍：摄像头驱动函数
//---------------------------------------------------------------------------------
// 备注：
//=================================================================================
/*----包含文件----*/
#include "includes.h"
#include "usb.h"

#include "usbd_hid_core.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_hid_desc.h"
#include "usbd_msc_desc.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usb_bsp.h"
#include <string.h>
#include <stdio.h>

USB_TRANS Usb_Trans;

//__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;
__ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;

//USB主机HID模式包含如下几个函数：
//usb_into_host：进入主机模式
//usb_host_init：主机模式初始化
//usb_host_transfer：主机模式下数据传输
//usb_host_exit：退出主机模式


void USB_AutoCheck_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(/*USB_PW_SW_RCC | USB_ID_PORT_RCC | */USB_GET_IN_PORT_RCC, ENABLE);
		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	
//	GPIO_InitStructure.GPIO_Pin = USB_ID_PIN;
//	GPIO_Init(USB_ID_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = USB_GET_IN_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(USB_GET_IN_PORT, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = USB_PW_SW_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//	GPIO_Init(USB_PW_SW, &GPIO_InitStructure);
//	
//	CLOSE_USB_BUS_POWER();
}

//=================================================================================
//| 函数名称 | usb_into_host
//|----------|---------------------------------------------------------------------
//| 函数功能 | 进入主机模式
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
USB_STATUS usb_into_host(void)
{
//	USBH_Process(&USB_OTG_Core_dev , &USB_Host);	//该函数在使用USB主机模式时，必须循环调用。
													//该函数实现设备的枚举和数据的发送接收	
	if (!HCD_IsDeviceConnected(&USB_OTG_Core_dev))	//设备未连接
	{
		return USB_UNCONNECTED;
	}
	if (USB_Host.gState == HOST_ERROR_STATE)		//出现错误
	{
		return USB_FAILURE;
	}
	if ((USB_Host.gState == HOST_CLASS) && (Usb_Trans.USB_State == 3))			//已经枚举成功
	{
		return USB_SUCESS;
	}
	if (Usb_Trans.USB_State == 2)						//从机设备插入
	{
		return USB_INSERT;
	}
	if (Usb_Trans.USB_State == 0xFF)					//设备类型不支持
	{
		return USB_NOTSP;
	}
	return USB_PROCEED;									//进行中
}

void usb_host_set_receive(void)
{
	memset(Usb_Trans.Rxbuf,0,USB_MAXLEN);
	Usb_Trans.Rxlen = 0;
	Usb_Trans.USB_restate = 0;
}

USB_STATUS usb_host_waitReceive(uint8_t *rxbuf)
{
	if (Usb_Trans.USB_restate == 1)
	{
		memcpy(rxbuf,Usb_Trans.Rxbuf,Usb_Trans.Rxlen);
		Usb_Trans.Rxlen = 0;
		Usb_Trans.USB_restate = 0;
		return USB_SUCESS;
	}
	else
	{
		return USB_FAILURE;
	}
}

//=================================================================================
//| 函数名称 | usb_host_init
//|----------|---------------------------------------------------------------------
//| 函数功能 | 主机模式初始化
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_host_init(void)
{
	OSSchedLock();

	memset(&USB_OTG_Core_dev.host.channel[0],0,sizeof(USB_OTG_Core_dev.host.channel));//默认为主机时不需要
	//OPEN_USB_BUS_POWER();
	usb_hid_device_exit();
	
	Usb_Trans.USB_msc_flag = 0;
	USBH_Init(&USB_OTG_Core_dev, 
				USB_OTG_FS_CORE_ID,
				&USB_Host,
				&HID_cb, 
				&USR_Callbacks);
	Usb_Trans.USB_State = 1;
	OSSchedUnlock();
}


USB_STATUS usb_host_send(uint8_t *txbuf, uint16_t txlen)
{
	uint8_t i;
	uint32_t count;
	uint8_t sendcount;	//数据发送次数
	uint8_t leavecount;	//剩余数据长度
	
	OSSchedLock();
	if (Usb_Trans.USB_State == 3)		//可以发送数据
	{
		usb_host_set_receive();
		sendcount = txlen/Usb_Trans.OneTransLen;
		leavecount = txlen%Usb_Trans.OneTransLen;
		for (i=0;i<sendcount;i++)
		{
			memset(Usb_Trans.Txbuf,0,64);
			memcpy(Usb_Trans.Txbuf,txbuf+(i*Usb_Trans.OneTransLen),Usb_Trans.OneTransLen);
			count = 0;
			USBH_InterruptSendData(&USB_OTG_Core_dev,Usb_Trans.Txbuf,Usb_Trans.OneTransLen,Usb_Trans.TxChannel);	//数据发送
			while(HCD_GetURB_State(&USB_OTG_Core_dev,Usb_Trans.TxChannel) != URB_DONE)								//等待发送完成
			{
				count++;
				System168MDelay1ms(1);
				if (count >=100)
				{
					OSSchedUnlock();
					return USB_FAILURE;		//失败
				}
			}
		}
		//****剩余的数据发送****//
		memset(Usb_Trans.Txbuf,0,64);
		memcpy(Usb_Trans.Txbuf,txbuf+(sendcount*Usb_Trans.OneTransLen),leavecount);
		count = 0;
		USBH_InterruptSendData(&USB_OTG_Core_dev,Usb_Trans.Txbuf,Usb_Trans.OneTransLen,Usb_Trans.TxChannel);	//数据发送
		while(HCD_GetURB_State(&USB_OTG_Core_dev,Usb_Trans.TxChannel) != URB_DONE)								//等待发送完成
		{
			count++;
			System168MDelay1ms(10);
			if (count >=10)
			{
				OSSchedUnlock();
				return USB_FAILURE;		//失败
			}
		}
		OSSchedUnlock();
		return USB_SUCESS;
	}
	OSSchedUnlock();
	return USB_FAILURE;
}

USB_STATUS usb_host_receive(uint8_t *rxbuf)
{
//	memset(USB_Rx_buf,0,64);		//清空数组
	if (Usb_Trans.USB_State == 3)		//可以接收数据
	{
		Usb_Trans.USB_State = 7;
	}
	USBH_Process(&USB_OTG_Core_dev , &USB_Host);						//数据接收
	if (Usb_Trans.USB_State == 8)		//数据接收成功
	{
//		memcpy(rxbuf,USB_Tx_buf,USB_TransLen);
		Usb_Trans.USB_State = 3;
		return USB_SUCESS;
	}
	else if (Usb_Trans.USB_State == 0)
	{
		return USB_UNCONNECTED;
	}
	return USB_FAILURE;
}



//=================================================================================
//| 函数名称 | usb_host_transfer
//|----------|---------------------------------------------------------------------
//| 函数功能 | 主机模式下数据传输
//|----------|---------------------------------------------------------------------
//| 输入参数 | txbuf：发送数据		rxbuf接收数据
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
//USB_STATUS usb_host_transfer(uint8_t *txbuf, uint8_t *rxbuf)
//{
//	uint8_t i = 3;		//重试次数
//	while (1)
//	{
//		USBH_Process(&USB_OTG_Core_dev , &USB_Host);	//该函数在使用USB主机模式时，必须循环调用。
//														//该函数实现设备的枚举和数据的发送接收
//		//数据发送接收在usbh_hid_core.c文件中的557到589行
//		//主机模式下，从机设备插入时，程序的处理可以在usbh_usr.c文件中的548行USR_HID_CUSTOM_Init函数中编写
//		//数据正确接收到，对数据的处理可以在usbh_hid_custom.c文件中的131行HID_Custom_Decode函数中进行编写
//		//HID_Custom_Decode函数是在usbh_hid_core.c文件中的585行被调用
//		//usbh_usr.c文件中的175到418行的函数，可以用来描述插入主机的从设备信息。详情参考函数注释
//		if(USB_Host_TRX_data == 0)			//已经成功收到信息
//		{
//			memcpy(rxbuf,USB_Rx_buf,64);
//			return USB_SUCESS;
//		}
//		else if (USB_Host_TRX_data == 2)	//没有收到信息
//		{
//			i--;
//			if (i > 0)
//			{
//				USB_Host_TRX_data = 1;			//重新发送
//			}
//			else
//			{
//				return USB_FAILURE;
//			}
//		}
//		SysTickDelay(20);					//延时20毫秒
//		if (!HCD_IsDeviceConnected(&USB_OTG_Core_dev))		//设备未连接
//		{
//			USBH_DeInit(&USB_OTG_Core_dev, &USB_Host);		//主机模式清空为初始值，即退出USB主机模式
//			USB_OTG_StopHost(&USB_OTG_Core_dev);
//			return USB_UNCONNECTED;
//		}
//	}
//}

//=================================================================================
//| 函数名称 | usb_host_exit
//|----------|---------------------------------------------------------------------
//| 函数功能 | 退出主机模式
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_host_exit(void)
{
//	CLOSE_USB_BUS_POWER();
	
	Usb_Trans.USB_State = 0;
	Usb_Trans.OneTransLen = 0;
	USBH_DeInit(&USB_OTG_Core_dev, &USB_Host);		//主机模式清空为初始值，即退出USB主机模式
	USB_OTG_StopHost(&USB_OTG_Core_dev);
}

//USB从机自定义HID模式包含如下几个函数：
//usb_hid_device_init：从机HID模式初始化
//usb_hid_device_redata：从机HID模式接收数据。需要不停的调用
//usb_hid_device_tedata：从机HID模式发送函数
//usb_hid_device_exit：退出从机HID模式

//=================================================================================
//| 函数名称 | usb_hid_device_init
//|----------|---------------------------------------------------------------------
//| 函数功能 | 从机HID模式初始化
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_hid_device_init(void)
{
	OSSchedLock();
	usb_host_exit();
	
	Usb_Trans.USB_msc_flag = 0;
	USBD_Init(&USB_OTG_Core_dev,		//HID初始化
				USB_OTG_FS_CORE_ID,
				&USR_HID_desc,
				&USBD_HID_cb,
				&USR_cb);
	OSSchedUnlock();			
}

//=================================================================================
//| 函数名称 | usb_hid_device_redata
//|----------|---------------------------------------------------------------------
//| 函数功能 | 从机HID模式接收数据，需要不停的调用，等待数据到达
//|----------|---------------------------------------------------------------------
//| 输入参数 | rxbuf：接收数据
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
USB_STATUS usb_hid_device_redata(uint8_t *rxbuf)
{
	if(UsbHidReceiveComplete)									//接收到数据
	{
		memcpy(rxbuf,Usb_Trans.Rxbuf,Usb_Trans.Rxlen);
		Usb_Trans.Rxlen = 0;
		DCD_EP_PrepareRx(&USB_OTG_Core_dev,HID_OUT_EP,Usb_Trans.Rxbuf,64);	//准备接收数据
		UsbHidReceiveComplete = 0;
		return USB_SUCESS;
	}
	return USB_FAILURE;
}



//=================================================================================
//| 函数名称 | usb_hid_device_tedata
//|----------|---------------------------------------------------------------------
//| 函数功能 | 从机HID模式发送数据
//|----------|---------------------------------------------------------------------
//| 输入参数 | txbuf：发送数据
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_hid_device_tedata(uint8_t *txbuf, uint16_t txlen)
{
	uint16_t i;
	uint16_t sendcount;	//数据发送次数
	uint8_t leavecount;	//剩余数据长度

	sendcount = txlen/64;
	leavecount = txlen%64;
	for (i=0;i<sendcount;i++)
	{
		memset(Usb_Trans.Txbuf,0,64);
		memcpy(Usb_Trans.Txbuf,txbuf+(i*64),64);
		USBD_HID_SendReport (&USB_OTG_Core_dev, Usb_Trans.Txbuf, 64);		//数据发送
	}
	//****剩余的数据发送****//
	memset(Usb_Trans.Txbuf,0,64);
	memcpy(Usb_Trans.Txbuf,txbuf+(sendcount*64),leavecount);
	USBD_HID_SendReport (&USB_OTG_Core_dev, Usb_Trans.Txbuf, 64);	//数据发送
}

//=================================================================================
//| 函数名称 | usb_hid_device_exit
//|----------|---------------------------------------------------------------------
//| 函数功能 | 退出从机HID模式
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_hid_device_exit(void)
{
	Usb_Trans.USB_msc_flag = 0;
	DCD_DevDisconnect(&USB_OTG_Core_dev);
	USB_OTG_StopDevice(&USB_OTG_Core_dev);
}

//USB从机MSC模式包含如下几个函数：
//usb_msc_device_init：从机MSC模式初始化
//usb_msc_device_exit：退出从机MSC模式

//=================================================================================
//| 函数名称 | usb_msc_device_init
//|----------|---------------------------------------------------------------------
//| 函数功能 | 从机MSC模式初始化
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_msc_device_init(void)
{
	OSSchedLock();
	usb_host_exit();
	
	Usb_Trans.USB_msc_flag = 1;
	USBD_Init(&USB_OTG_Core_dev,		//MSC初始化
				USB_OTG_FS_CORE_ID,
				&USR_MSC_desc,
				&USBD_MSC_cb,
				&USR_cb);
	OSSchedUnlock();			
}

//=================================================================================
//| 函数名称 | usb_msc_device_exit
//|----------|---------------------------------------------------------------------
//| 函数功能 | 退出从机MSC模式
//|----------|---------------------------------------------------------------------
//| 输入参数 | 
//|----------|---------------------------------------------------------------------
//| 返回参数 | 无
//|----------|---------------------------------------------------------------------
//| 函数设计 | 编写人：    时间：2013-06-05  
//|----------|---------------------------------------------------------------------
//|   备注   | 
//|----------|---------------------------------------------------------------------
//| 修改记录 | 修改人：          时间：         修改内容： 
//=================================================================================
void usb_msc_device_exit(void)
{
	Usb_Trans.USB_msc_flag = 0;
	DCD_DevDisconnect(&USB_OTG_Core_dev);
	USB_OTG_StopDevice(&USB_OTG_Core_dev);
}

/*----Enf Of File----*/
