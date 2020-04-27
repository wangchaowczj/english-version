/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3240G-EVAL
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                      DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2u)

/**************************************设置任务优先级**************************************/
#define RES_PUBLIC_STK_MUTEX_PRIO                    8
#define RES_ADC_MUTEX_PRIO                           9
#define RES_FILE_MUTEX_PRIO                          11
#define RES_WIFI_MUTEX_PRIO                          13
#define RES_LCD_BRIGHT_MUTEX_PRIO                    15
#define RES_GPS_MUTEX_PRIO                           16

#define APP_DET_OPERATE_TASK_PRIO                    21
#define APP_STARTUP_TASK_PRIO                        22
#define APP_USB_TASK_PRIO                            23
#define APP_RS485_TASK_PRIO                          25
#define APP_KEY_SCAN_TASK_PRIO                       27
#define APP_SAFE_MONITOR_TASK_PRIO                   31
#define APP_BACKGROUND_TASK_PRIO                     33
#define APP_TEMPORARY_TASK_PRIO                  	 37
#define APP_WIFI_TASK_PRIO                           39
#define APP_SOUND_TASK_PRIO                          41
#define APP_USER_GUI_TASK_PRIO                       43
#define APP_LOW_TASK_PRIO                            50

/*******************************设置栈大小（单位为 OS_STK ）*******************************/  
#define APP_STARTUP_TASK_STK_SIZE                    256
#define APP_USB_TASK_STK_SIZE                        512
#define APP_USER_GUI_TASK_STK_SIZE                   1024
#define APP_KEY_SCAN_TASK_STK_SIZE                   512
#define APP_SAFE_MONITOR_TASK_STK_SIZE               1536
#define APP_WIFI_TASK_STK_SIZE                       512

#define APP_PUBLIC_TASK_STK_SIZE                     512

#endif
