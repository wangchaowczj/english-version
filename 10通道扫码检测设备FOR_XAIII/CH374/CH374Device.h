#ifndef _CH374DEVICE_H_
#define _CH374DEVICE_H_

typedef struct OneDescriptor
{
  const u8 *Des;
  u16 Size;
}ONE_DESCRIPTOR, *PONE_DESCRIPTOR;

typedef enum
{
	DEVICE_HID,//从机，HID格式
	DEVICE_MSC
}CH374_DEVICE_TYPE;

extern void CH374DeviceInit(CH374_DEVICE_TYPE DeviceType);
extern void CH374DeviceInterrupt(void);

#endif
