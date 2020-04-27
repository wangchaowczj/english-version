#ifndef _BLUEMASTER_H_
#define _BLUEMASTER_H_

extern ErrorStatus GetBlueSlaveVersion(u8 add, u8* Version, u8 MaxLength);
extern ErrorStatus GetBlueSlaveDetNumberAndType(u8 add, u16* RegNumber, u16* ErrNumber, u8* Type);

extern u8 BlueReadDetInfoBackUp(u8 add);
extern ErrorStatus BlueDetInfoBackUp(u8 add);
extern ErrorStatus GetBlueSlaveDetNumberSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole, u16* RegNumber, u16* StartIndex);
extern u8 BlueReadDetInfoInsert(u8 add, u16 StartHole, u16 EndHole);
extern u8 BlueDetInfoInsert(u8 add, u16 StartHole, u16 EndHole);

#endif
