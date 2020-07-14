/**
  ******************************************************************************
  * @file    usbh_hid_core.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file is the HID Layer Handlers for USB Host HID class.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                HID Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse and Keyboard protocols
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"
#include "usbh_hid_custom.h"
#include "usb.h"
//#include "usbh_hid_keybd.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_CORE 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN HID_Machine_TypeDef        HID_Machine __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN HID_Report_TypeDef         HID_Report __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_Setup_TypeDef          HID_Setup __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HIDDesc_TypeDef       HID_Desc __ALIGN_END ; 

__IO uint8_t start_toggle = 0;


//extern u8 USB_Tx_buf[65];				//发送数据数组
//extern u8 USB_Rx_buf[65];				//接收数据数组
//extern u8 USB_Host_State;				//USB状态位
//extern u8 USB_TransLen;					//数据发送长度
/**
* @}
*/ 

USB_OTG_USBH_Clear_n_FIFO(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTMSK_TypeDef      intmsk;
  USB_OTG_HNPTXSTS_TypeDef     hnptxsts; 
  uint16_t                     len_words , len; 
  
  hnptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->HNPTXSTS);
  
  len_words = (pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len + 3) / 4;
  
  while ((hnptxsts.b.nptxfspcavail > len_words)&&
         (pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len != 0))
  {
    
    len = hnptxsts.b.nptxfspcavail * 4;
    
    if (len > pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len)
    {
      /* Last packet */
      len = pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len;
      
      intmsk.d32 = 0;
      intmsk.b.nptxfempty = 1;
      USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, intmsk.d32, 0);       
    }
    
    len_words = (pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len + 3) / 4;
    
    USB_OTG_WritePacket (pdev , pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_buff, hnptxsts.b.nptxqtop.chnum, len);
    
    pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_buff  += len;
    pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_len   -= len;
    pdev->host.hc[hnptxsts.b.nptxqtop.chnum].xfer_count  += len; 
    
    hnptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->HNPTXSTS);
  }  
}

USB_OTG_USBH_Clear_FIFO(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTMSK_TypeDef      intmsk;
  USB_OTG_HPTXSTS_TypeDef      hptxsts; 
  uint16_t                     len_words , len; 
  
  hptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HPTXSTS);
  
  len_words = (pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len + 3) / 4;
  
  while ((hptxsts.b.ptxfspcavail > len_words)&&
         (pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len != 0))    
  {
    
    len = hptxsts.b.ptxfspcavail * 4;
    
    if (len > pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len)
    {
      len = pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len;
      /* Last packet */
      intmsk.d32 = 0;
      intmsk.b.ptxfempty = 1;
      USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, intmsk.d32, 0); 
    }
    
    len_words = (pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len + 3) / 4;
    
    USB_OTG_WritePacket (pdev , pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_buff, hptxsts.b.ptxqtop.chnum, len);
    
    pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_buff  += len;
    pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len   -= len;
    pdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_count  += len; 
    
    hptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HPTXSTS);
  }  
}

/** @defgroup USBH_HID_CORE_Private_FunctionPrototypes
* @{
*/ 

static USBH_Status USBH_HID_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev , 
                                            void *phost);

static void  USBH_ParseHIDDesc (USBH_HIDDesc_TypeDef *desc, uint8_t *buf);

static void USBH_HID_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev , 
                                       void *phost);

static USBH_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pdev , 
                                   void *phost);

static USBH_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                         void *phost);

static USBH_Status USBH_Get_HID_ReportDescriptor (USB_OTG_CORE_HANDLE *pdev, 
                                                  USBH_HOST *phost,
                                                  uint16_t length);

static USBH_Status USBH_Get_HID_Descriptor (USB_OTG_CORE_HANDLE *pdev,\
                                            USBH_HOST *phost,  
                                            uint16_t length);

static USBH_Status USBH_Set_Idle (USB_OTG_CORE_HANDLE *pdev, 
                                  USBH_HOST *phost,
                                  uint8_t duration,
                                  uint8_t reportId);

static USBH_Status USBH_Set_Protocol (USB_OTG_CORE_HANDLE *pdev, 
                                      USBH_HOST *phost,
                                      uint8_t protocol);


USBH_Class_cb_TypeDef  HID_cb = 
{
  USBH_HID_InterfaceInit,
  USBH_HID_InterfaceDeInit,
  USBH_HID_ClassRequest,
  USBH_HID_Handle
};
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Functions
* @{
*/ 

/**
* @brief  USBH_HID_InterfaceInit 
*         The function init the HID class.
* @param  pdev: Selected device
* @param  hdev: Selected device property
* @retval  USBH_Status :Response for USB HID driver intialization
*/
static USBH_Status USBH_HID_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev, 
                                           void *phost)
{	
  uint8_t maxEP;
  USBH_HOST *pphost = phost;
    
  uint8_t num =0;
  USBH_Status status = USBH_BUSY ;
  HID_Machine.state = HID_ERROR;
  
  if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_HID_CLASS)		//是HID格式
  {
    if(pphost->device_prop.Itf_Desc[0].bInterfaceSubClass  == HID_BOOT_CODE)			//如果是键盘或鼠标则退出
    {
      /*Decode Bootclass Protocl: Mouse or Keyboard*/
      if(pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == HID_KEYBRD_BOOT_CODE)
      {
		Usb_Trans.USB_State = 0xFF;
		return USBH_NOT_SUPPORTED;
//      HID_Machine.cb = &HID_KEYBRD_cb;
      }
      else if(pphost->device_prop.Itf_Desc[0].bInterfaceProtocol  == HID_MOUSE_BOOT_CODE)		  
      {
		Usb_Trans.USB_State = 0xFF;
		return USBH_NOT_SUPPORTED;
//      HID_Machine.cb = &HID_MOUSE_cb;
      }
	}
	HID_Machine.cb = &HID_CUSTOM_cb;
    
    HID_Machine.state     = HID_IDLE;
    HID_Machine.ctl_state = HID_REQ_IDLE; 
    HID_Machine.ep_addr   = pphost->device_prop.Ep_Desc[0][0].bEndpointAddress;
    HID_Machine.length    = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
    HID_Machine.poll      = pphost->device_prop.Ep_Desc[0][0].bInterval ;

	Usb_Trans.OneTransLen = HID_Machine.length;
    if (HID_Machine.poll  < HID_MIN_POLL) 
    {
       HID_Machine.poll = HID_MIN_POLL;
    }

    
    /* Check fo available number of endpoints */
    /* Find the number of EPs in the Interface Descriptor */      
    /* Choose the lower number in order not to overrun the buffer allocated */
    maxEP = ( (pphost->device_prop.Itf_Desc[0].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ? 
             pphost->device_prop.Itf_Desc[0].bNumEndpoints :
                 USBH_MAX_NUM_ENDPOINTS); 
    
    
    /* Decode endpoint IN and OUT address from interface descriptor */
    for (num=0; num < maxEP; num++)
    {
      if(pphost->device_prop.Ep_Desc[0][num].bEndpointAddress & 0x80)
      {
        HID_Machine.HIDIntInEp = (pphost->device_prop.Ep_Desc[0][num].bEndpointAddress);
        HID_Machine.hc_num_in  =\
               USBH_Alloc_Channel(pdev, 
                                  pphost->device_prop.Ep_Desc[0][num].bEndpointAddress);
        Usb_Trans.RxChannel = HID_Machine.hc_num_in;
        /* Open channel for IN endpoint */
        USBH_Open_Channel  (pdev,
                            HID_Machine.hc_num_in,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EP_TYPE_INTR,
                            HID_Machine.length); 
      }
      else
      {
        HID_Machine.HIDIntOutEp = (pphost->device_prop.Ep_Desc[0][num].bEndpointAddress);
        HID_Machine.hc_num_out  =\
                USBH_Alloc_Channel(pdev, 
                                   pphost->device_prop.Ep_Desc[0][num].bEndpointAddress);
        Usb_Trans.TxChannel = HID_Machine.hc_num_out;
        /* Open channel for OUT endpoint */
        USBH_Open_Channel  (pdev,
                            HID_Machine.hc_num_out,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EP_TYPE_INTR,
                            HID_Machine.length); 
      }
      
    }   
    
     start_toggle =0;
     status = USBH_OK; 
  }
  else
  {
	Usb_Trans.USB_State = 0xFF;
	return USBH_NOT_SUPPORTED;
  }
  
  return status;
  
}



/**
* @brief  USBH_HID_InterfaceDeInit 
*         The function DeInit the Host Channels used for the HID class.
* @param  pdev: Selected device
* @param  hdev: Selected device property
* @retval None
*/
void USBH_HID_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
                               void *phost)
{	
   //USBH_HOST *pphost = phost;
    
  if(HID_Machine.hc_num_in != 0x00)
  {   
    USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_in);
    USBH_Free_Channel  (pdev, HID_Machine.hc_num_in);
    HID_Machine.hc_num_in = 0;     /* Reset the Channel as Free */  
  }
  
  if(HID_Machine.hc_num_out != 0x00)
  {   
    USB_OTG_HC_Halt(pdev, HID_Machine.hc_num_out);
    USBH_Free_Channel  (pdev, HID_Machine.hc_num_out);
    HID_Machine.hc_num_out = 0;     /* Reset the Channel as Free */  
  }
 
  start_toggle = 0;
}

/**
* @brief  USBH_HID_ClassRequest 
*         The function is responsible for handling HID Class requests
*         for HID class.
* @param  pdev: Selected device
* @param  hdev: Selected device property
* @retval  USBH_Status :Response for USB Set Protocol request
*/
static USBH_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                         void *phost)
{   
    USBH_HOST *pphost = phost;
    
  USBH_Status status         = USBH_BUSY;
  USBH_Status classReqStatus = USBH_BUSY;
  
  
  /* Switch HID state machine */
  switch (HID_Machine.ctl_state)
  {
  case HID_IDLE:  
  case HID_REQ_GET_HID_DESC:
    
    /* Get HID Desc */ 
    if (USBH_Get_HID_Descriptor (pdev, pphost, USB_HID_DESC_SIZE)== USBH_OK)
    {
      
      USBH_ParseHIDDesc(&HID_Desc, pdev->host.Rx_Buffer);
      HID_Machine.ctl_state = HID_REQ_GET_REPORT_DESC;
    }
    
    break;     
  case HID_REQ_GET_REPORT_DESC:
    
    
    /* Get Report Desc */ 
    if (USBH_Get_HID_ReportDescriptor(pdev , pphost, HID_Desc.wItemLength) == USBH_OK)
    {
      HID_Machine.ctl_state = HID_REQ_HID_SET_IDLE;
    }
    
    break;
    
  case HID_REQ_HID_SET_IDLE:
    
    classReqStatus = USBH_Set_Idle (pdev, pphost, 0, 0);
    
    /* set Idle */
    if (classReqStatus == USBH_OK)
    {
      HID_Machine.ctl_state = HID_REQ_HID_SET_PROTOCOL;  
    }
    else if(classReqStatus == USBH_NOT_SUPPORTED) 
    {
      HID_Machine.ctl_state = HID_REQ_HID_SET_PROTOCOL;        
    } 
    break; 
    
  case HID_REQ_HID_SET_PROTOCOL:
    /* set protocol */
    if (USBH_Set_Protocol (pdev ,pphost, 0) == USBH_OK)
    {
      HID_Machine.ctl_state = HID_REQ_IDLE;
      
      /* all requests performed*/
      status = USBH_OK; 
    } 
    break;
    
  default:
    break;
  }
  
  return status; 
}


/**
* @brief  USBH_HID_Handle 
*         The function is for managing state machine for HID data transfers 
* @param  pdev: Selected device
* @param  hdev: Selected device property
* @retval USBH_Status
*/
static USBH_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pdev , 
                                   void   *phost)
{
//  u32 usbh_sst;
  USBH_HOST *pphost = phost;
  USBH_Status status = USBH_OK;
  
  switch (HID_Machine.state)
  {
    
  case HID_IDLE:
    HID_Machine.cb->Init();
    HID_Machine.state = HID_SYNC;
    
  case HID_SYNC:

    /* Sync with start of Even Frame */
    if(USB_OTG_IsEvenFrame(pdev) == TRUE)
    {
		Usb_Trans.USB_State = 3;
      HID_Machine.state = HID_GET_DATA;  
    }
    break;
    
  case HID_GET_DATA:

	USBH_InterruptReceiveData(pdev, 
								HID_Machine.buff,
								HID_Machine.length,
								HID_Machine.hc_num_in);
	start_toggle = 1;
	
	HID_Machine.state = HID_POLL;
	HID_Machine.timer = HCD_GetCurrentFrame(pdev);
	break;
/*
	if (Usb_Trans.USB_State == 4)
	{
		usbh_sst = 0;
//		USBH_InterruptSendData(pdev,USB_Tx_buf,HID_Machine.length,HID_Machine.hc_num_out);			//数据发送
		while(HCD_GetURB_State(pdev , HID_Machine.hc_num_out) != URB_DONE)								//等待发送完成
		{
			usbh_sst++;
			if (usbh_sst >=1000)
			{
				Usb_Trans.USB_State = 6;		//失败
				break;
			}
		}
		if (usbh_sst < 1000)
		{
			Usb_Trans.USB_State = 5;			//成功
		}
	}
	else if (Usb_Trans.USB_State == 7)
	{
//		USBH_InterruptReceiveData(pdev,USB_Rx_buf,HID_Machine.length,HID_Machine.hc_num_in);		//数据接收
		if(HCD_GetURB_State(pdev , HID_Machine.hc_num_in) != URB_DONE)								//等待接收完成
		{
			Usb_Trans.USB_State = 9;		//失败
		}
		else
		{
			Usb_Trans.USB_State = 8;			//成功
		}
//		HID_Machine.cb->Decode(USB_Rx_buf);															//数据解码
//		HID_Machine.state = HID_IDLE;
//		HID_Machine.timer = HCD_GetCurrentFrame(pdev);
	}
//    USBH_InterruptReceiveData(pdev, 
//                              HID_Machine.buff,
//                              HID_Machine.length,
//                              HID_Machine.hc_num_in);
//    HID_Machine.timer = HCD_GetCurrentFrame(pdev);
    break;
    */
  case HID_POLL:
    if(( HCD_GetCurrentFrame(pdev) - HID_Machine.timer) >= HID_Machine.poll)
    {
      HID_Machine.state = HID_GET_DATA;
    }
    else if(HCD_GetURB_State(pdev , HID_Machine.hc_num_in) == URB_DONE)
    {
      if(start_toggle == 1) /* handle data once */
      {
        start_toggle = 0;
        HID_Machine.cb->Decode(HID_Machine.buff);
      }
    }
    else if(HCD_GetURB_State(pdev, HID_Machine.hc_num_in) == URB_STALL) /* IN Endpoint Stalled */
    {
      
      /* Issue Clear Feature on interrupt IN endpoint */ 
      if( (USBH_ClrFeature(pdev, 
                           pphost,
                           HID_Machine.ep_addr,
                           HID_Machine.hc_num_in)) == USBH_OK)
      {
        /* Change state to issue next IN token */
        HID_Machine.state = HID_GET_DATA;
        
      }
      
    }      
    break;
    
  default:
    break;
  }
  return status;
}


/**
* @brief  USBH_Get_HID_ReportDescriptor
*         Issue report Descriptor command to the device. Once the response 
*         received, parse the report descriptor and update the status.
* @param  pdev   : Selected device
* @param  Length : HID Report Descriptor Length
* @retval USBH_Status : Response for USB HID Get Report Descriptor Request
*/
static USBH_Status USBH_Get_HID_ReportDescriptor (USB_OTG_CORE_HANDLE *pdev,
                                                  USBH_HOST *phost,
                                                  uint16_t length)
{
  
  USBH_Status status;
  
  status = USBH_GetDescriptor(pdev,
                              phost,
                              USB_REQ_RECIPIENT_INTERFACE
                                | USB_REQ_TYPE_STANDARD,                                  
                                USB_DESC_HID_REPORT, 
                                pdev->host.Rx_Buffer,
                                length);
  
  /* HID report descriptor is available in pdev->host.Rx_Buffer.
  In case of USB Boot Mode devices for In report handling ,
  HID report descriptor parsing is not required.
  In case, for supporting Non-Boot Protocol devices and output reports,
  user may parse the report descriptor*/
  
  
  return status;
}

/**
* @brief  USBH_Get_HID_Descriptor
*         Issue HID Descriptor command to the device. Once the response 
*         received, parse the report descriptor and update the status.
* @param  pdev   : Selected device
* @param  Length : HID Descriptor Length
* @retval USBH_Status : Response for USB HID Get Report Descriptor Request
*/
static USBH_Status USBH_Get_HID_Descriptor (USB_OTG_CORE_HANDLE *pdev,
                                            USBH_HOST *phost,
                                            uint16_t length)
{
  
  USBH_Status status;
  
  status = USBH_GetDescriptor(pdev, 
                              phost,
                              USB_REQ_RECIPIENT_INTERFACE
                                | USB_REQ_TYPE_STANDARD,                                  
                                USB_DESC_HID,
                                pdev->host.Rx_Buffer,
                                length);
 
  return status;
}

/**
* @brief  USBH_Set_Idle
*         Set Idle State. 
* @param  pdev: Selected device
* @param  duration: Duration for HID Idle request
* @param  reportID : Targetted report ID for Set Idle request
* @retval USBH_Status : Response for USB Set Idle request
*/
static USBH_Status USBH_Set_Idle (USB_OTG_CORE_HANDLE *pdev,
                                  USBH_HOST *phost,
                                  uint8_t duration,
                                  uint8_t reportId)
{
  
  USBH_Status status;

  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |\
    USB_REQ_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
  phost->Control.setup.b.wValue.w = (duration << 8 ) | reportId;
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  status = USBH_OK;
  
//  return USBH_CtlReq(pdev, phost, 0 , 0 );
	return status;
}


/**
* @brief  USBH_Set_Report
*         Issues Set Report 
* @param  pdev: Selected device
* @param  reportType  : Report type to be sent
* @param  reportID    : Targetted report ID for Set Report request
* @param  reportLen   : Length of data report to be send
* @param  reportBuff  : Report Buffer
* @retval USBH_Status : Response for USB Set Idle request
*/
USBH_Status USBH_Set_Report (USB_OTG_CORE_HANDLE *pdev, 
                                 USBH_HOST *phost,
                                    uint8_t reportType,
                                    uint8_t reportId,
                                    uint8_t reportLen,
                                    uint8_t* reportBuff)
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |\
    USB_REQ_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
  phost->Control.setup.b.wValue.w = (reportType << 8 ) | reportId;
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = reportLen;
  
  return USBH_CtlReq(pdev, phost, reportBuff , reportLen );
}


/**
* @brief  USBH_Set_Protocol
*         Set protocol State.
* @param  pdev: Selected device
* @param  protocol : Set Protocol for HID : boot/report protocol
* @retval USBH_Status : Response for USB Set Protocol request
*/
static USBH_Status USBH_Set_Protocol(USB_OTG_CORE_HANDLE *pdev,
                                     USBH_HOST *phost,
                                     uint8_t protocol)
{
  
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |\
    USB_REQ_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_PROTOCOL;
  
  if(protocol != 0)
  {
    /* Boot Protocol */
    phost->Control.setup.b.wValue.w = 0;
  }
  else
  {
    /*Report Protocol*/
    phost->Control.setup.b.wValue.w = 1;
  }
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );
  
}

/**
* @brief  USBH_ParseHIDDesc 
*         This function Parse the HID descriptor
* @param  buf: Buffer where the source descriptor is available
* @retval None
*/
static void  USBH_ParseHIDDesc (USBH_HIDDesc_TypeDef *desc, uint8_t *buf)
{
  
  desc->bLength                  = *(uint8_t  *) (buf + 0);
  desc->bDescriptorType          = *(uint8_t  *) (buf + 1);
  desc->bcdHID                   =  LE16  (buf + 2);
  desc->bCountryCode             = *(uint8_t  *) (buf + 4);
  desc->bNumDescriptors          = *(uint8_t  *) (buf + 5);
  desc->bReportDescriptorType    = *(uint8_t  *) (buf + 6);
  desc->wItemLength              =  LE16  (buf + 7);
  
} 
/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
