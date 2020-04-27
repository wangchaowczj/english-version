
#ifndef _CUSSOMMESSAGEBOX_H_
#define _CUSSOMMESSAGEBOX_H_

typedef struct
{
    char Caption[16];//最多5个汉字
    char Message[100];    
    char Help1[46];
    char Help2[46];
    char Left[7];//最多2个汉字
    char Right[7];//最多2个汉字
    u32 MessageColor;
}MSG_BOX_DATA;
extern MSG_BOX_DATA MsgBoxData;

extern int ShowMsgBox(void);

extern void MsgBoxInit(void);
extern void SetMsgBoxCaption(char* str);
extern void SetMsgBoxMessage(char* str);
extern void SetMsgBoxHelp1(char* str);
extern void SetMsgBoxHelp2(char* str);
extern void SetMsgBoxLeft(char* str);
extern void SetMsgBoxRight(char* str);
extern void SetMsgBoxMessageColor(u32 Color);

extern u8 MsgBoxIsShow(void);
extern void CloseMsgBoxForce(void);
extern void CloseMsgBoxAndParentForce(void);

#endif
