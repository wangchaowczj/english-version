//=================================================================================
// ��Ȩ: 
//---------------------------------------------------------------------------------
// ���ߣ�    ʱ�䣺2013-04-19     �汾��V1.0.0
//---------------------------------------------------------------------------------
// �޸���ʷ��
// �޸��ˣ�        �޸�ʱ�䣺           �޸����ݣ�	     ����ˣ�
//---------------------------------------------------------------------------------
// �ļ���cam_ptc06.h
//---------------------------------------------------------------------------------
// ���ܣ�����ͷ����
//---------------------------------------------------------------------------------
// ��ע��
//=================================================================================
#ifndef _USB_HOST_DEVICE_
#define _USB_HOST_DEVICE_
/*----�����ļ�----*/
#include <stdint.h>

typedef enum {
    USB_SUCESS  = 0,    // ִ�гɹ�
    USB_FAILURE,        // ִ��ʧ��
	USB_UNCONNECTED,	//�豸δ����
	USB_PROCEED,		//������
	USB_INSERT,			//�ӻ��豸����
	USB_NOTSP			//�豸���Ͳ�֧��
} USB_STATUS;

#define USB_MAXLEN			800			//����/����������󳤶�

typedef struct {
	u16 Txlen;					//�������ݳ���
	u8 Txbuf[USB_MAXLEN];		//������������
	u16 Rxlen;					//�������ݳ���
	u8 Rxbuf[USB_MAXLEN];		//������������
	u8 OneTransLen;				//���ݴ���һ�εĳ���
	u8 TxChannel;				//��������ͨ��
	u8 RxChannel;				//��������ͨ��
	u8 USB_State;				//USB��״̬λ
	u8 USB_restate;				//USB�Ƿ���յ�����
	u8 USB_msc_flag;            //1��ʾ��ǰ����MSC״̬
} USB_TRANS;

extern USB_TRANS Usb_Trans;

//extern u8 USB_Tx_buf[256];				//������������
//extern u8 USB_Rx_buf[256];				//������������
//extern u8 USB_Tx_Len;					//�������ݳ���
//extern u8 USB_Rx_Len;					//�������ݳ���
//extern u16 USB_TransLen;				//����һ�η����ܳ���
//extern u8 USB_reLen;					//���ݽ���ʱ�����յ������ݳ���

//#define USB_PW_SW              GPIOF
//#define USB_PW_SW_RCC          RCC_AHB1Periph_GPIOF
//#define USB_PW_SW_PIN          GPIO_Pin_10

//#define USB_ID_PORT            GPIOB  //��
//#define USB_ID_PORT_RCC        RCC_AHB1Periph_GPIOB
//#define USB_ID_PIN             GPIO_Pin_12

//#define USB_ID_PORT            GPIOA  //��
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
