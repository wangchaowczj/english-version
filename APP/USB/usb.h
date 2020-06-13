//=================================================================================
// 版权: 
//---------------------------------------------------------------------------------
// 作者：    时间：2013-04-19     版本：V1.0.0
//---------------------------------------------------------------------------------
// 修改历史：
// 修改人：        修改时间：           修改内容：	     检查人：
//---------------------------------------------------------------------------------
// 文件：cam_ptc06.h
//---------------------------------------------------------------------------------
// 介绍：摄像头驱动
//---------------------------------------------------------------------------------
// 备注：
//=================================================================================
#ifndef _USB_HOST_DEVICE_
#define _USB_HOST_DEVICE_
/*----包含文件----*/
#include <stdint.h>

typedef enum {
    USB_SUCESS  = 0,    // 执行成功
    USB_FAILURE,        // 执行失败
	USB_UNCONNECTED,	//设备未连接
	USB_PROCEED,		//进行中
	USB_INSERT,			//从机设备插入
	USB_NOTSP			//设备类型不支持
} USB_STATUS;

#define USB_MAXLEN			800			//发送/接收数据最大长度

typedef struct {
	u16 Txlen;					//发送数据长度
	u8 Txbuf[USB_MAXLEN];		//发送数据数组
	u16 Rxlen;					//接收数据长度
	u8 Rxbuf[USB_MAXLEN];		//接收数据数组
	u8 OneTransLen;				//数据传输一次的长度
	u8 TxChannel;				//发送数据通道
	u8 RxChannel;				//接收数据通道
	u8 USB_State;				//USB的状态位
	u8 USB_restate;				//USB是否接收到数据
	u8 USB_msc_flag;            //1表示当前处于MSC状态
} USB_TRANS;

extern USB_TRANS Usb_Trans;

//extern u8 USB_Tx_buf[256];				//发送数据数组
//extern u8 USB_Rx_buf[256];				//接收数据数组
//extern u8 USB_Tx_Len;					//发送数据长度
//extern u8 USB_Rx_Len;					//接收数据长度
//extern u16 USB_TransLen;				//数据一次发送总长度
//extern u8 USB_reLen;					//数据接收时，接收到的数据长度

//#define USB_PW_SW              GPIOF
//#define USB_PW_SW_RCC          RCC_AHB1Periph_GPIOF
//#define USB_PW_SW_PIN          GPIO_Pin_10

//#define USB_ID_PORT            GPIOB  //旧
//#define USB_ID_PORT_RCC        RCC_AHB1Periph_GPIOB
//#define USB_ID_PIN             GPIO_Pin_12

//#define USB_ID_PORT            GPIOA  //新
//#define USB_ID_PORT_RCC        RCC_AHB1Periph_GPIOA
//#define USB_ID_PIN             GPIO_Pin_10
//#define USB_CONNET_GUEST        0
//#define GET_USB_ID()    GPIO_ReadInputDataBit(USB_ID_PORT, USB_ID_PIN)

#define USB_GET_IN_PORT            GPIOA
#define USB_GET_IN_PORT_RCC        RCC_AHB1Periph_GPIOA
#define USB_GET_IN_PIN             GPIO_Pin_10

#define USB_NOT_CHARGE           0
#define GET_CHARGE_IN_STATE()    GPIO_ReadInputDataBit(USB_GET_IN_PORT, USB_GET_IN_PIN)

//#define OPEN_USB_BUS_POWER()    GPIO_ResetBits(USB_PW_SW, USB_PW_SW_PIN)
//#define CLOSE_USB_BUS_POWER()   GPIO_SetBits(USB_PW_SW, USB_PW_SW_PIN)

void USB_AutoCheck_Config(void);

USB_STATUS usb_into_host(void);
void usb_host_set_receive(void);
USB_STATUS usb_host_waitReceive(uint8_t *rxbuf);
void usb_host_init(void);
USB_STATUS usb_host_send(uint8_t *txbuf, uint16_t txlen);
USB_STATUS usb_host_receive(uint8_t *rxbuf);
//USB_STATUS usb_host_transfer(uint8_t *txbuf, uint8_t *rxbuf);
void usb_host_exit(void);

void usb_hid_device_init(void);
USB_STATUS usb_hid_device_redata(uint8_t *rxbuf);
void usb_hid_device_tedata(uint8_t *txbuf, uint16_t txlen);
void usb_hid_device_exit(void);

void usb_msc_device_init(void);
void usb_msc_device_exit(void);



#endif /* _CAM_PTC06_ */
/*----End Of File----*/
