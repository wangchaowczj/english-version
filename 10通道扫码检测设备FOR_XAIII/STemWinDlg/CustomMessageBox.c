/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : MESSAGEBOX.c
Purpose     : Implementation of Mesagebox
---------------------------END-OF-HEADER------------------------------
*/


#include <string.h>             /* for memset */
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "TEXT.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"

#include "includes.h"
#include "GuiIncludes.h"


/*********************************************************************
*
*       Defaults
*
**********************************************************************
*/


#define CUSTOM_MESSAGEBOX_BORDER 5
#define CUSTOM_MESSAGEBOX_XSIZEOK 100
#define CUSTOM_MESSAGEBOX_YSIZEOK 35
#define CUSTOM_MESSAGEBOX_BKCOLOR GUI_WHITE
#define CUSTOM_MESSAGEBOX_YSIZEOK_SPACE 20

static u8 IsShowMsgFlag = 0;
u8 GetCustomMessageBoxStatus(void)
{
	return IsShowMsgFlag;
}
void ColseCustomMessageBoxForce(void)
{
	if(IsShowMsgFlag)
	{
		IsShowMsgFlag = 2;
	}	
}
void ColseCustomMessageBoxAndParentForce(void)
{
	if(IsShowMsgFlag)
	{
		IsShowMsgFlag = 3;
	}	
}
/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _MESSAGEBOX_cbCallback
*/
static void _MESSAGEBOX_cbCallback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{		
		case WM_INIT_DIALOG:
			FRAMEWIN_SetClientColor(hWin, CUSTOM_MESSAGEBOX_BKCOLOR);
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_1MS_ID, GUI_TIMER_1MS, 0);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_CUSTOM_KEY_OK:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_NO)))
					{
						DlghWin.Dlg = WM_GetParent(hWin);
						GUI_EndDialog(hWin, RESULT_YES);
					}
					else
					{
						DlghWin.Dlg = WM_GetParent(hWin);
						GUI_EndDialog(hWin, RESULT_OK);
					}
					IsShowMsgFlag = 0;	
					break;
				case GUI_CUSTOM_KEY_BACK:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_NO)))
					{
						DlghWin.Dlg = WM_GetParent(hWin);
						GUI_EndDialog(hWin, RESULT_NO);
						IsShowMsgFlag = 0;	
					}
					break;
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_1MS_ID:
					if(IsShowMsgFlag == 2)
					{
						IsShowMsgFlag = 0;	
						GUI_EndDialog(hWin, RESULT_NULL);
					}
					else if(IsShowMsgFlag == 3)
					{
						WM_HWIN Parent ;
						
						IsShowMsgFlag = 0;	
						Parent = WM_GetParent(hWin);
						GUI_EndDialog(hWin, RESULT_NULL);
						CloseWindowAndParent(Parent);
					}				
					else
					{
						WM_RestartTimer(pMsg->Data.v, GUI_TIMER_1MS);
					}
					break;
				default:
					break;
			}
			break;
//    case WM_NOTIFY_PARENT:
//      {
//        int NCode = pMsg->Data.v;             /* Get notification code */
//        int Id    = WM_GetId(pMsg->hWinSrc);  /* Get control ID */
//        switch (NCode) 
//		{
//          case WM_NOTIFICATION_RELEASED:      /* React only if released */
//            if (Id == GUI_ID_OK) 
//			{
//				GUI_EndDialog(hWin, RESULT_OK);         /* End dialog with return value 0 if OK */
//            }
//            else if (Id == GUI_ID_YES) 
//			{
//				GUI_EndDialog(hWin, RESULT_YES);         /* End dialog with return value 0 if OK */
//            }			
//            else if (Id == GUI_ID_NO) 
//			{
//				GUI_EndDialog(hWin, RESULT_NO);         /* End dialog with return value 0 if OK */
//            }			
//            break;
//			
//        }
//      }
//      break;
    default:
      WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       MESSAGEBOX_Create
*/
WM_HWIN MessageBoxDlgCreate(const char * sMessage, const char * sCaption, MSG_BOX_BUTTON_TYPE uType) 
{
	GUI_WIDGET_CREATE_INFO _aDialogCreate[4];                                     /* 0: FrameWin, 1: Text, 2: Button */
	int BorderSize = FRAMEWIN_GetDefaultBorderSize();                             /* Default border size of frame window */
	int xSizeFrame = CUSTOM_MESSAGEBOX_XSIZEOK*2+CUSTOM_MESSAGEBOX_YSIZEOK_SPACE + 2 * BorderSize + CUSTOM_MESSAGEBOX_BORDER * 2; /* XSize of frame window */
	int ySizeFrame;                                                               /* YSize of frame window */
	int x0, y0;                                                                   /* Position of frame window */
	int xSizeMessage;                                                             /* Length in pixels of message */
	int xSizeCaption;                                                             /* Length in pixels of caption */
	int ySizeCaption;                                                             /* YSize of caption */
	int ySizeMessage;                                                             /* YSize of message */
	GUI_RECT Rect;
	const GUI_FONT GUI_UNI_PTR * pOldFont;
	/* Zeroinit variables */
	memset(_aDialogCreate, 0, sizeof(_aDialogCreate));
	/* Get dimension of message */
	pOldFont = GUI_SetFont(TEXT_GetDefaultFont());
	GUI_GetTextExtend(&Rect, sMessage, 255);
	xSizeMessage = Rect.x1 - Rect.x0 + CUSTOM_MESSAGEBOX_BORDER * 2;
	ySizeMessage = Rect.y1 - Rect.y0 + 1;
	if (xSizeFrame < (xSizeMessage + 4 + CUSTOM_MESSAGEBOX_BORDER * 2)) 
	{
		xSizeFrame = xSizeMessage + 4 + CUSTOM_MESSAGEBOX_BORDER * 2;
	}
	ySizeCaption = GUI_GetYSizeOfFont(FRAMEWIN_GetDefaultFont());
	ySizeFrame = ySizeMessage +            /* size of message */
			   CUSTOM_MESSAGEBOX_YSIZEOK +      /* size of button */
			   ySizeCaption +            /* caption size */
			   CUSTOM_MESSAGEBOX_BORDER * 3 +   /* inner border - text, text - button, button - bottom */
			   BorderSize * 2 +          /* top & bottom border */
			   1+                       /* inner border */
			   CUSTOM_MESSAGEBOX_BORDER;                        
	/* Get xsize of caption */
	xSizeCaption = GUI_GetStringDistX(sCaption);
	if (xSizeFrame < xSizeCaption + BorderSize * 2) 
	{
		xSizeFrame = xSizeCaption + BorderSize * 2;
	}
	/* Check maximum */
	if (xSizeFrame > LCD_GetXSize()) 
	{
		xSizeFrame = LCD_GetXSize();
	}
	if (ySizeFrame > LCD_GetYSize()) 
	{
		ySizeFrame = LCD_GetYSize();
	}
	/* Calculate position of framewin */
	x0 = (LCD_GetXSize() - xSizeFrame) / 2;
	y0 = (LCD_GetYSize() - STATUSBAR_HEIGHT - ySizeFrame) / 2;
	/* restore modified Context */
	GUI_SetFont(pOldFont);
	/* Fill frame win resource */
	_aDialogCreate[0].pfCreateIndirect = FRAMEWIN_CreateIndirect;
	_aDialogCreate[0].pName            = sCaption;
	_aDialogCreate[0].x0               = x0;
	_aDialogCreate[0].y0               = y0;
	_aDialogCreate[0].xSize            = xSizeFrame;
	_aDialogCreate[0].ySize            = ySizeFrame;
	_aDialogCreate[0].Flags            = FRAMEWIN_CF_MOVEABLE;

	/* Fill text resource */
	_aDialogCreate[1].pfCreateIndirect = TEXT_CreateIndirect;
	_aDialogCreate[1].pName            = sMessage;
	_aDialogCreate[1].x0               = (xSizeFrame - xSizeMessage - BorderSize * 2) / 2;
	_aDialogCreate[1].y0               = CUSTOM_MESSAGEBOX_BORDER;
	_aDialogCreate[1].xSize            = xSizeMessage;
	_aDialogCreate[1].ySize            = ySizeMessage;
	_aDialogCreate[1].Para             = GUI_TA_TOP | GUI_TA_HCENTER;

	/* Fill button resource */
	_aDialogCreate[2].pfCreateIndirect = BUTTON_CreateIndirect;
	_aDialogCreate[2].pName            = "确定";
	_aDialogCreate[2].Id               = GUI_ID_YES;
	_aDialogCreate[2].x0               = (xSizeFrame - (CUSTOM_MESSAGEBOX_XSIZEOK*2+CUSTOM_MESSAGEBOX_YSIZEOK_SPACE) - BorderSize * 2) / 2;
	_aDialogCreate[2].y0               = CUSTOM_MESSAGEBOX_BORDER * 2 + ySizeMessage;
	_aDialogCreate[2].xSize            = CUSTOM_MESSAGEBOX_XSIZEOK;
	_aDialogCreate[2].ySize            = CUSTOM_MESSAGEBOX_YSIZEOK;

	_aDialogCreate[3].pfCreateIndirect = BUTTON_CreateIndirect;
	_aDialogCreate[3].pName            = "返回";
	_aDialogCreate[3].Id               = GUI_ID_NO;
	_aDialogCreate[3].x0               = (xSizeFrame - (CUSTOM_MESSAGEBOX_XSIZEOK*2+CUSTOM_MESSAGEBOX_YSIZEOK_SPACE) - BorderSize * 2) / 2 + 
										CUSTOM_MESSAGEBOX_XSIZEOK+CUSTOM_MESSAGEBOX_YSIZEOK_SPACE;
	_aDialogCreate[3].y0               = CUSTOM_MESSAGEBOX_BORDER * 2 + ySizeMessage;
	_aDialogCreate[3].xSize            = CUSTOM_MESSAGEBOX_XSIZEOK;
	_aDialogCreate[3].ySize            = CUSTOM_MESSAGEBOX_YSIZEOK;  
	if(uType == MB_OK)
	{//只有一个按钮
		_aDialogCreate[2].Id         = GUI_ID_OK;
		_aDialogCreate[2].x0         = (xSizeFrame - CUSTOM_MESSAGEBOX_XSIZEOK - BorderSize * 2) / 2;
	}
	IsShowMsgFlag = 1;
	/* Create dialog */
	if(WM_IsWindow(DlghWin.Dlg))
	{
		return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _MESSAGEBOX_cbCallback, DlghWin.Dlg, 0, 0);
	}
	else
	{
		return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _MESSAGEBOX_cbCallback,WM_HBKWIN, 0, 0);
	}
}

/*********************************************************************
*
*       GUI_MessageBox
*/
MSG_BOX_RESULT ShowCustomMessageBox(const char * sMessage, const char * sCaption, MSG_BOX_BUTTON_TYPE uType) 
{
	WM_HWIN hWin;

	hWin = MessageBoxDlgCreate(sMessage, sCaption, uType);
	if(uType == MB_OK)
	{
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_NO));  
	}
	else if(uType == MB_NULL)
	{
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_YES));
		WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_NO));
	}
	WM_MakeModal(hWin);
	DlghWin.Dlg = hWin;

	/* Exec dialog */
	return (MSG_BOX_RESULT)GUI_ExecCreatedDialog(hWin);
}
