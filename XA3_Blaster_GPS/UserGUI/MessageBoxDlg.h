#ifndef _MESSAGEBOXDLG_H_
#define _MESSAGEBOXDLG_H_

//typedef void (*DlgFun)(void); //定义一个函数指针类型

typedef struct
{
	char TitleBuffer[31];
	char Help1Buffer[46];	
	char Help2Buffer[46];
	char MsgBuffer[200];	
	char BtLeftText[7];
	char BtRightText[7];
	int Color;
	DlgFun FunLeft;
	DlgFun FunRight;
	DlgFun FunEnter;	
	DlgFun FunBackground;
	DlgFun FunNext;		
}MSG_DLG;

extern void InitMessageBoxDlg(void);
extern void SetMessageBoxDlgMsgColor(int color);
extern void SetMessageBoxDlgTitle(char* Title);
extern void SetMessageBoxDlgHelp1(char* Help1);
extern void SetMessageBoxDlgHelp2(char* Help2);
extern void SetMessageBoxDlgMsg(char* Msg);
extern void SetMessageBoxDlgBtLeftText(char* Text);
extern void SetMessageBoxDlgBtRightText(char* Text);
extern void SetMessageBoxDlgBtLeftFun(DlgFun FunLeft);
extern void SetMessageBoxDlgBtRightFun(DlgFun FunRight);
extern void SetMessageBoxDlgBtEnterFun(DlgFun FunRight);
extern void SetMessageBoxDlgEnterNextFun(DlgFun FunNext);
extern void SetMessageBoxDlgBackgroundFun(DlgFun Background);

extern void ShowMessageBoxDlg(void);

#endif
