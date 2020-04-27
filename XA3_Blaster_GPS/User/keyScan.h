#ifndef _KEYSCAN_H_
#define _KEYSCAN_H_

/*��������*/
#define KEY0_CODE       	0x00E5	//��Ӧԭ��ͼ���B8
#define KEY1_CODE       	0x01F7	//��Ӧԭ��ͼ���B12
#define KEY2_CODE       	0x0075	//��Ӧԭ��ͼ���B5
#define KEY3_CODE       	0x02F7	//��Ӧԭ��ͼ���B13
#define KEY4_CODE       	0x00B3	//��Ӧԭ��ͼ���B10
#define KEY5_CODE       	0x00B5	//��Ӧԭ��ͼ���B6
#define KEY6_CODE       	0x00B6	//��Ӧԭ��ͼ���B2
#define KEY7_CODE       	0x00D3	//��Ӧԭ��ͼ���B11
#define KEY8_CODE       	0x00D5	//��Ӧԭ��ͼ���B7
#define KEY9_CODE       	0x00D6  //��Ӧԭ��ͼ���B3
#define KEY_LEFT_CODE       0x0073  //��Ӧԭ��ͼ���B9
#define KEY_RIGHT_CODE      0x0076  //��Ӧԭ��ͼ���B1
#define KEY_ENTER_CODE      0x00E6	//��Ӧԭ��ͼ���B4
#define KEY_DOUBLE_CODE     0x03F7	//��Ӧԭ��ͼ���

/***�ܽŶ���***/
#define KEY_OUT0_PORT             GPIOG
#define KEY_OUT0_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_OUT0_PIN              GPIO_Pin_3

#define KEY_OUT1_PORT             GPIOG
#define KEY_OUT1_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_OUT1_PIN              GPIO_Pin_4

#define KEY_OUT2_PORT             GPIOG
#define KEY_OUT2_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_OUT2_PIN              GPIO_Pin_5

#define KEY_IN0_PORT              GPIOF
#define KEY_IN0_PORT_RCC          RCC_AHB1Periph_GPIOF
#define KEY_IN0_PIN               GPIO_Pin_15

#define KEY_IN1_PORT             GPIOG
#define KEY_IN1_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN1_PIN              GPIO_Pin_0

#define KEY_IN2_PORT             GPIOG
#define KEY_IN2_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN2_PIN              GPIO_Pin_1

#define KEY_IN3_PORT             GPIOG
#define KEY_IN3_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN3_PIN              GPIO_Pin_2

#define KEY_1_PORT               GPIOF
#define KEY_1_PORT_RCC           RCC_AHB1Periph_GPIOF
#define KEY_1_PIN                GPIO_Pin_13

#define KEY_3_PORT               GPIOF
#define KEY_3_PORT_RCC           RCC_AHB1Periph_GPIOF
#define KEY_3_PIN                GPIO_Pin_14

#define KEY_PW_PORT              GPIOG
#define KEY_PW_PORT_RCC          RCC_AHB1Periph_GPIOG
#define KEY_PW_PIN               GPIO_Pin_11

#define KEY_LED_PORT              GPIOA
#define KEY_LED_PORT_RCC          RCC_AHB1Periph_GPIOA
#define KEY_LED_PIN               GPIO_Pin_9
#define OPEN_KEY_LED()      	  GPIO_ResetBits(KEY_LED_PORT, KEY_LED_PIN)
#define CLOSE_KEY_LED()           GPIO_SetBits(KEY_LED_PORT, KEY_LED_PIN)
#define SET_KEY_LED_TOGGLE()      (KEY_LED_PORT->ODR ^=KEY_LED_PIN)

/*����ɨ��*/
#define SET_KEY_ROW0(state)      GPIO_WriteBit(KEY_OUT0_PORT, KEY_OUT0_PIN, state)
#define SET_KEY_ROW1(state)      GPIO_WriteBit(KEY_OUT1_PORT, KEY_OUT1_PIN, state)
#define SET_KEY_ROW2(state)      GPIO_WriteBit(KEY_OUT2_PORT, KEY_OUT2_PIN, state)

#define GET_KEY_ROW0()           GPIO_ReadInputDataBit(KEY_OUT0_PORT, KEY_OUT0_PIN)
#define GET_KEY_ROW1()           GPIO_ReadInputDataBit(KEY_OUT1_PORT, KEY_OUT1_PIN)
#define GET_KEY_ROW2()           GPIO_ReadInputDataBit(KEY_OUT2_PORT, KEY_OUT2_PIN)

#define SET_KEY_VERTICAL0(state) GPIO_WriteBit(KEY_IN0_PORT, KEY_IN0_PIN, state)
#define SET_KEY_VERTICAL1(state) GPIO_WriteBit(KEY_IN1_PORT, KEY_IN1_PIN, state)
#define SET_KEY_VERTICAL2(state) GPIO_WriteBit(KEY_IN2_PORT, KEY_IN2_PIN, state)
#define SET_KEY_VERTICAL3(state) GPIO_WriteBit(KEY_IN3_PORT, KEY_IN3_PIN, state)

#define GET_KEY_VERTICAL0()      GPIO_ReadInputDataBit(KEY_IN0_PORT, KEY_IN0_PIN)
#define GET_KEY_VERTICAL1()      GPIO_ReadInputDataBit(KEY_IN1_PORT, KEY_IN1_PIN)
#define GET_KEY_VERTICAL2()      GPIO_ReadInputDataBit(KEY_IN2_PORT, KEY_IN2_PIN)
#define GET_KEY_VERTICAL3()      GPIO_ReadInputDataBit(KEY_IN3_PORT, KEY_IN3_PIN)



/*����״̬*/
#define KEY_DOWN    0

#define GET_KEY_1_STATE()        GPIO_ReadInputDataBit(KEY_1_PORT, KEY_1_PIN)
#define GET_KEY_3_STATE()        GPIO_ReadInputDataBit(KEY_3_PORT, KEY_3_PIN)
#define GET_KEY_PW_STATE()       GPIO_ReadInputDataBit(KEY_PW_PORT, KEY_PW_PIN)



extern void KeyConfig(void);
extern u8 KeyboardScan(void);
extern void SetPowerOffKeyState(void);
extern void ResetPowerOffKeyState(void);
extern ErrorStatus GetPowerOffKeyState(void);
#endif
