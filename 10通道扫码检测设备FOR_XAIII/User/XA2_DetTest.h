#ifndef _XA2_DETTEST_H_
#define _XA2_DETTEST_H_

extern void XA2_DetTest(void);
extern void XA2_DetCodeVerify(void);
extern void XA2_QueryDetConnectStatus(void);
extern void XA2_WaitAllChannelDetOff(void);
extern u8 XA2_DetIsFixDelay(u8 Type);
extern u8 DetCodeRulePass(u32 Serl, u32 Pwd);
extern DET_ERR_CODE DetWriteTestRecorderCheck(u8 ChannelCount);

#endif
