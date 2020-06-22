/**
  ******************************************************************************
  * @file    usbh_hid_keybd.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file is the application layer for USB Host HID Keyboard handling
  *          QWERTY and AZERTY Keyboard are supported as per the selection in 
  *          usbh_hid_keybd.h              
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
#include "usbh_hid_custom.h"
#include "usb.h"
#include "includes.h"
/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_KEYBD 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_KEYBD_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_HID_KEYBD_Private_FunctionPrototypes
* @{
*/ 
static void  HID_Custom_Init (void);
static void  HID_Custom_Decode(uint8_t *data);

/**
* @}
*/ 

 
/** @defgroup USBH_HID_KEYBD_Private_Variables
* @{
*/
HID_cb_TypeDef HID_CUSTOM_cb= 
{
  HID_Custom_Init,
  HID_Custom_Decode
};

/*
*******************************************************************************
*                                             LOCAL CONSTANTS
*******************************************************************************
*/



/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Functions
* @{
*/ 


/**
* @brief  KEYBRD_Init.
*         Initialize the keyboard function.
* @param  None
* @retval None
*/
static void  HID_Custom_Init (void)
{
  /* Call User Init*/
  USR_HID_CUSTOM_Init();
}

/**
* @brief  KEYBRD_ProcessData.USB作为主机时接收函数
*         The function is to decode the pressed keys.
* @param  pbuf : Pointer to the HID IN report data buffer
* @retval None
*/

static void HID_Custom_Decode(uint8_t *pbuf)
{
	Usb_Trans.USB_restate = 1;
	if ((HidData.bytenum + Usb_Trans.OneTransLen) > BUF_MAX)
	{
		HidData.bytenum = 0;
	}
	memcpy(&HidData.uch_buf[HidData.bytenum], pbuf, Usb_Trans.OneTransLen);
    HidData.bytenum += Usb_Trans.OneTransLen;
	SlipSimpleCheck(&HidData);
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

