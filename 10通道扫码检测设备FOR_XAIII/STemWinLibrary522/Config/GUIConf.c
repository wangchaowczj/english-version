/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
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

//#define GUI_USER_EXT_SRAM//是否使用外部RAM作为GUI的内存

#include "GUI.h"

#ifdef GUI_USER_EXT_SRAM
#include "includes.h"
#endif
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES  (1024UL*64)
//extern void GUITASK_SetMaxTask(int MaxTask);
/*
设置由存储器管理系统分配的存储块的平均尺寸。块尺寸会影响可用块的最大数量。例如，如果应用
要使用含有大量条目的列表视图，则可将块平均尺寸设置为较小的值。另一方面，如果应用主要将存
储器管理用于一些存储器设备或图像解压，则应把平均尺寸设为较大的值。建议范围为32 至1024。
具体值取决于应用。
*/
#define GUI_BLOCKSIZE 32 
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {
	
#ifndef GUI_USER_EXT_SRAM //使用芯片内部内存空间
	
	//
	// 32 bit aligned memory area
	//
	//static U32 aMemory[GUI_NUMBYTES / 4];
	static U32* aMemory = (U32 *)0x10000000;//使用STM32F4特有的64K,CCM内存,CCM只能内核调用，外设无法使用
	//
	// Assign memory to emWin
	//
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
	//
	// Set default font
	//
	GUI_SetDefaultFont(GUI_FONT_6X8);
#else //使用外部内存空间
	static U32 *aMemory;
	
	aMemory = (U32*)EXT_SRAM_ADDR;
	
	// Assign memory to emWin
	//
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
	//
	// Set default font
	//
	GUI_SetDefaultFont(GUI_FONT_6X8);		
#endif
//	GUITASK_SetMaxTask(5);//设置在启用多任务时可用于访问emWin 的最大任务数,默认值是4
}

/*************************** End of file ****************************/
