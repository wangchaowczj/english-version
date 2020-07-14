#ifndef _XA_DETTEST_H_
#define _XA_DETTEST_H_
//#include "BarCode.h"



extern u8 XA_DetIsSingleCap(u8 Type);
extern u8 XA_DetIsFixDelay(u8 Type);

extern void XA_DetTest(void);
extern u16 XA_DetAnalyst(void);
extern void XA_QueryDetConnectStatus(void);
extern void XA_WaitAllChannelDetOff(void);
extern void XA_DetCodeVerify(void);

#endif
