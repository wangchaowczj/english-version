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

//#define GUI_USER_EXT_SRAM//�Ƿ�ʹ���ⲿRAM��ΪGUI���ڴ�

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
�����ɴ洢������ϵͳ����Ĵ洢���ƽ���ߴ硣��ߴ��Ӱ����ÿ��������������磬���Ӧ��
Ҫʹ�ú��д�����Ŀ���б���ͼ����ɽ���ƽ���ߴ�����Ϊ��С��ֵ����һ���棬���Ӧ����Ҫ����
������������һЩ�洢���豸��ͼ���ѹ����Ӧ��ƽ���ߴ���Ϊ�ϴ��ֵ�����鷶ΧΪ32 ��1024��
����ֵȡ����Ӧ�á�
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
	
#ifndef GUI_USER_EXT_SRAM //ʹ��оƬ�ڲ��ڴ�ռ�
	
	//
	// 32 bit aligned memory area
	//
	//static U32 aMemory[GUI_NUMBYTES / 4];
	static U32* aMemory = (U32 *)0x10000000;//ʹ��STM32F4���е�64K,CCM�ڴ�,CCMֻ���ں˵��ã������޷�ʹ��
	//
	// Assign memory to emWin
	//
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
	//
	// Set default font
	//
	GUI_SetDefaultFont(GUI_FONT_6X8);
#else //ʹ���ⲿ�ڴ�ռ�
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
//	GUITASK_SetMaxTask(5);//���������ö�����ʱ�����ڷ���emWin �����������,Ĭ��ֵ��4
}

/*************************** End of file ****************************/
