/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* USER CODE BEGIN Includes */
#include "usart2.h"
#include "fatfs.h"
#include "gpio.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
FATFS fatfs;
FIL file; 
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */
  
  /* USER CODE END USB_HOST_Init_PreTreatment */
  
  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostHS, USBH_UserProcess, HOST_HS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostHS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
  
  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostHS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
	int i,j;
	static FRESULT res;
	unsigned char write_buffer[512];
	unsigned char read_buffer[512];
	unsigned int counter;
	
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
	//f_mount
	res = f_mount(&fatfs,"0:",1);
	if(res != RES_OK){
		USBH_UsrLog("\r\nf_mount error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}
	}else{
		USBH_UsrLog("\r\nf_mount successful!");
	}	
	
	//f_open
	for(i = 0; i < 512 ; i ++)write_buffer[i] = i % 256;
	res = f_open(&file,"0:/test.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件
	if(res != RES_OK){
		USBH_UsrLog("f_open error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}
	}else{
		USBH_UsrLog("f_open successful!");
	}
	
	//f_lseek
	res = f_lseek(&file,0);
	if(res != RES_OK){
		USBH_UsrLog("f_lseek error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}	
	}else{
		USBH_UsrLog("f_lseek successful!");
	}
	
	//f_write
	res = f_write(&file,write_buffer,512,&counter);
	if(res != RES_OK || counter != 512){
		USBH_UsrLog("f_write error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}
	}else{
		USBH_UsrLog("f_write successful!");
	}
	
	//f_lseek
	res = f_lseek(&file,0);
	if(res != RES_OK){
		USBH_UsrLog("f_lseek error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}	
	}else{
		USBH_UsrLog("f_lseek successful!");
	}

	//f_read
	res = f_read(&file,read_buffer,512,&counter); 
	if(res != RES_OK || counter != 512){
		USBH_UsrLog("f_read error!");
		while(1){
			LED_ON;
			HAL_Delay(500);
			LED_OFF;
			HAL_Delay(500);			
		}	
	}else{
		USBH_UsrLog("f_read successful!");
	}
	f_close(&file);
	
	USBH_UsrLog("read data:");
	for(i = 0;i < 32;i++){
		for(j = 0; j < 16; j ++)
			usart2.printf("%02X ",read_buffer[i*16+j]);
		usart2.printf("\r\n");
	}	
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
