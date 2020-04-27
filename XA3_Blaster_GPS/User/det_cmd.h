#ifndef _EB_DET_CMD_H_
#define _EB_DET_CMD_H_


#define DET_CMD_SUCCESS	   0   //ָ��ִ�гɹ�
#define DET_CMD_FAIL       1   //ָ��ִ��ʧ��
#define DET_TIMERWR_FAIL   2   //��Ƶʱд��ʱ����

/* �������״̬λ������ */
#define EB_DET_MASK_TEST_ERR        0x04    // bit�쳣λ
#define EB_DET_MASK_CHARGE          0x08	// ����쳣λ
#define EB_DET_MASK_DISCHARGE       0x10	// �ŵ��쳣λ
#define EB_DET_MASK_WAIT_EXPEN      0x20	// ׼�����쳣λ
#define EB_DET_MASK_SETTIME_OK      0x40	// ����ʱ�������쳣λ


/* �׹ܲ���ָ�����(�ȷ��͵�λ�ٷ��͸�λ)*/
#define CHECK_IN          0x09              // ����
#define CLRALL			  0x19				// ȫ�ָ�λ
#define SERLRD			  0x29				// ������ض�
#define DISCHARGE		  0x39				// �ŵ�ָ�����оƬ���ã���оƬû��
#define CHECK_OSC         0x59              // ��Ƶ
#define MASK              0x69              // ����
#define CLR               0x89              // ������λ
#define TIMEWR			  0x99				// ��������ʱ��
#define TIMERD			  0xA9				// �ض�����ʱ��
#define BIT1			  0xB9				// BIT���1ָ��
#define	BIT2			  0xC9				// BIT���2ָ��
#define CHARGE			  0xD9				// ���ָ��
#define EXPEN             0xf9              // ׼����


/*
* �����������׹ܷ��͸�λָ��
*/
extern void EB_DetClrAll(void);

/*
* �����׹�
* ����ֵ��
*        DET_CMD_SUCCESS-�׹����γɹ�
*        DET_CMD_FAIL-�׹�����ʧ��
*/
extern u8 EB_DetMask(const u8 *psn);

/*
* �����׹�
* psn���׹����к�ָ��
* retry: ���Դ���
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetMaskTry(const u8 *psn, u8 retry);

/*
* ����׹��Ƿ�����
* ����ֵ��
*        DET_CMD_SUCCESS-���׹�����
*        DET_CMD_FAIL-���׹�����
*/
extern u8 EB_DetCheckIn(void);


/*
* ��ȡ�׹�������
* buf���׹������뻺��
* ����ֵ��
*        DET_CMD_SUCCESS-�������ȡ�ɹ�
*        DET_CMD_FAIL-�������ȡʧ��
*/
extern u8 EB_DetSerlRD(u8 *buf);


/*
* ��ȡ�׹�������
* buf���׹������뻺��
* retry  ��ȡ����
* ����ֵ��
*        DET_CMD_SUCCESS-�������ȡ�ɹ�
*        DET_CMD_FAIL-�������ȡʧ��
*/
extern u8 EB_DetSerlRDTry(u8 *buf, u8 retry);


/*
* �׹���ͷ���
* psn���׹����к�ָ��
* ����ֵ��
*        DET_CMD_SUCCESS-��ͷ���ɹ�
*        DET_CMD_FAIL-��ͷ���ʧ��
*/
extern u8 EB_DetBit1(const u8 *psn);

/*
* �׹���ͷ���+����
* psn���׹����к�ָ��
* retry�����Դ���
* ����ֵ��
*        DET_CMD_SUCCESS-��ͷ���ɹ�
*        DET_CMD_FAIL-��ͷ���ʧ��
*/
extern u8 EB_DetBit1Try(const u8 *psn, u8 retry);

/*
* ��ʱ�ض�ָ��
* psn���׹����к�ָ��
* buf: ��ʱ�ض����ؽ������4���ֽ�
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetTimeRD(const u8 *psn, u8 *buf);

/*
* ��ʱ�ض�ָ��+����
* psn���׹����к�ָ��
* buf: ��ʱ�ض����ؽ������4���ֽ�
* retry�����Դ�������С��3������ò�����ȷ�Ľ��
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetTimeRDTry(const u8 *psn, u8 *buf, u8 retry, u16 Delay);//extern u8 EB_DetTimeRDTry(const u8 *psn, u8 *buf, u8 retry);
extern u8 EB_DetTimeRDTryErrorTry(const u8 *psn, u8 *buf, u8 retry, u8 ErrorRetryFlag);
/*
* д����ָ��
* psn���׹����к�ָ��
* scode: �׹�����
* delay: ��ʱʱ��
* bit4clk:64��CLK��Ӧ�Ķ�ʱ������ֵ
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetTimeWR(const u8 *psn, u8 scode, u16 delay, u32 freq, u32* det_dly_clk);

/*
* д����ָ��+����
* psn���׹����к�ָ��
* scode: �׹�����
* delay: ��ʱʱ��
* freq:  оƬƵ��
* bit4clk:64��CLK��Ӧ�Ķ�ʱ������ֵ
* retry�����Դ���
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetTimeWRTry(const u8 *psn, u8 scode, u16 delay, u32 freq, u32* det_dly_clk, u8 retry);

/*
* ��Ƶָ��
* psn���׹����к�ָ��
* scode: �׹�����
* buf: ��������ָ��Ƶ��(��λHZ)
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
extern u8 EB_DetCheckFreq(const u8 *psn, u8 scode, u32* freq);

/*
* ���ָ��
* psn���׹����к�ָ��
* scode: �׹�����
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
//extern u8 EB_DetCharge(const u8 *psn, u8 scode);
extern u8 EB_DetCharge(const u8 *psn, u8 scode, u8 cmd_charge);
/*
* ���ָ��+����
* psn���׹����к�ָ��
* scode: �׹�����
* retry�����Դ���
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
//extern u8 EB_DetChargeTry(const u8 *psn, u8 scode, u8 retry);
extern u8 EB_DetChargeTry(const u8 *psn, u8 scode, u8 cmd_charge, u8 retry);

/*
* ׼����ָ��
* psn���׹����к�ָ��
* scode: �׹�����
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
//extern u8 EB_DetExpen(const u8 *psn, u8 scode);
extern u8 EB_DetExpen(const u8 *psn, u8 scode, u8 cmd_expen);
/*
* ׼����ָ��
* psn���׹����к�ָ��
* scode: �׹�����
* ����ֵ��
*        DET_CMD_SUCCESS-��ʱ�ض��ɹ�
*        DET_CMD_FAIL-��ʱ�ض�ʧ��
*/
//extern u8 EB_DetExpenTry(const u8 *psn, u8 scode, u8 retry);
extern u8 EB_DetExpenTry(const u8 *psn, u8 scode, u8 cmd_expen, u8 retry);
#endif /* _EB_DET_CMD_H_ */
