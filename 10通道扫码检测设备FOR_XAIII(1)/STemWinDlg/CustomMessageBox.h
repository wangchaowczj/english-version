
#ifndef _CUSSOMMESSAGEBOX_H_
#define _CUSSOMMESSAGEBOX_H_

typedef enum
{
	MB_OK,
	MB_YESNO,
	MB_NULL
}MSG_BOX_BUTTON_TYPE;

typedef enum
{
	RESULT_OK,
	RESULT_YES,
	RESULT_NO,
	RESULT_NULL
}MSG_BOX_RESULT;
extern MSG_BOX_RESULT ShowCustomMessageBox(const char * sMessage, const char * sCaption, MSG_BOX_BUTTON_TYPE uType) ;
extern u8 GetCustomMessageBoxStatus(void);
extern void ColseCustomMessageBoxForce(void);
extern void ColseCustomMessageBoxAndParentForce(void);

#endif
