#ifndef _LOADINGDLG_H_
#define _LOADINGDLG_H_

typedef struct
{
    const char * sCaption;
    const char * sMessage;    
}LOADING_PARAM;

extern void ShowLoadingDlg(const char* sCaption, const char* sMessage);
extern void CloseLoadingDlg(void);
extern u8 LoadingDlgIsShow(void);

#endif
