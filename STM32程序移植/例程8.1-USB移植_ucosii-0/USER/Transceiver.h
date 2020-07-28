#ifndef _TRANSCEIVER_H_
#define _TRANSCEIVER_H_


#define TRANSCEIVER_BUFFER_MAX			1024
typedef struct
{
	u16 StartIndex;
	u16 EndIndex;
	u8 Buffer[TRANSCEIVER_BUFFER_MAX];
}TRANSCEIVER;

extern TRANSCEIVER UartTransceiver;
extern TRANSCEIVER RfTransceiver;



extern ErrorStatus SaveTransceiverByte(TRANSCEIVER* Transceiver, u8 Byte);
extern ErrorStatus GetTransceiverByte(TRANSCEIVER* Transceiver, u8* Byte);
extern u16 GetTransceiverBytesCount(TRANSCEIVER* Transceiver);
extern void ClearTransceiver(TRANSCEIVER* Transceiver);

#endif
