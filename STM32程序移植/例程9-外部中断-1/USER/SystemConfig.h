/**
  *************************************************************************************
  * @file    SystemConfig.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为SystemConfig.c的头文件
  *************************************************************************************
  */
#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SystemConfig
  *@{
  */
/**@defgroup SystemConfig_PinDefines
  *@brief 引脚定义
  *@{
  */



/**
 *@} SystemConfig_PinDefines
 */
/**
 *@} SystemConfig
 */
/**
 *@} PowerBoard
 */ 
extern void SystemGpioConfig(void);
extern ErrorStatus SystemHardwareInit(void);
extern void SystemPoint2Config(void);


#endif
