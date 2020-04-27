#ifndef _DET_CODE_REPAIR_H_
#define _DET_CODE_REPAIR_H_

extern u8 DetCodeNeedPepairByNotComplete(u8 Serl_2, u8 Serl_3);
extern u8 DetCodeNeedPepair(u8 Serl_2);
extern u8 DetPwdNeedPepair(DET_INFO *DetInfo);
extern u32 DetDelayClkPepair(u32 DelayClk);
extern u8 BlasterNeedPepair(void);

#endif
