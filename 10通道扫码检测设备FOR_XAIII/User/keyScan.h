#ifndef _KEYSCAN_H_
#define _KEYSCAN_H_

/*按键代码*/
#define KEY0_CODE       	0x7B
#define KEY1_CODE       	0xE7
#define KEY2_CODE       	0xD7
#define KEY3_CODE       	0xB7
#define KEY4_CODE       	0xEB
#define KEY5_CODE       	0xDB
#define KEY6_CODE       	0xBB
#define KEY7_CODE       	0xDD
#define KEY8_CODE       	0xBD
#define KEY9_CODE       	0x7D
#define KEY_BACK_CODE       0x77
#define KEY_OK_CODE         0xBE
#define KEY_F1_CODE         0xDE
#define KEY_F2_CODE         0xED
#define KEY_F3_CODE         0x7E
#define KEY_F4_CODE         0xEE
#define KEY_POWER_CODE      0xEC

/***管脚定义***/
#define KEY_IN0_PORT             GPIOG
#define KEY_IN0_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN0_PIN              GPIO_Pin_9

#define KEY_IN1_PORT             GPIOG
#define KEY_IN1_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN1_PIN              GPIO_Pin_10

#define KEY_IN2_PORT             GPIOG
#define KEY_IN2_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN2_PIN              GPIO_Pin_11

#define KEY_IN3_PORT             GPIOG
#define KEY_IN3_PORT_RCC         RCC_AHB1Periph_GPIOG
#define KEY_IN3_PIN              GPIO_Pin_12

#define KEY_OUT0_PORT            GPIOG
#define KEY_OUT0_PORT_RCC        RCC_AHB1Periph_GPIOG
#define KEY_OUT0_PIN             GPIO_Pin_13

#define KEY_OUT1_PORT            GPIOG
#define KEY_OUT1_PORT_RCC        RCC_AHB1Periph_GPIOG
#define KEY_OUT1_PIN             GPIO_Pin_14

#define KEY_OUT2_PORT            GPIOG
#define KEY_OUT2_PORT_RCC        RCC_AHB1Periph_GPIOG
#define KEY_OUT2_PIN             GPIO_Pin_15

#define KEY_OUT3_PORT            GPIOB
#define KEY_OUT3_PORT_RCC        RCC_AHB1Periph_GPIOB
#define KEY_OUT3_PIN             GPIO_Pin_3

#define KEY_PW_PORT              GPIOB
#define KEY_PW_PORT_RCC          RCC_AHB1Periph_GPIOB
#define KEY_PW_PIN               GPIO_Pin_2

#define KEY_FOOT_PORT            GPIOC
#define KEY_FOOT_PORT_RCC        RCC_AHB1Periph_GPIOC
#define KEY_FOOT_PIN             GPIO_Pin_4

/*按键扫描*/
#define SET_KEY_ROW0(state)      GPIO_WriteBit(KEY_OUT0_PORT, KEY_OUT0_PIN, state)
#define SET_KEY_ROW1(state)      GPIO_WriteBit(KEY_OUT1_PORT, KEY_OUT1_PIN, state)
#define SET_KEY_ROW2(state)      GPIO_WriteBit(KEY_OUT2_PORT, KEY_OUT2_PIN, state)
#define SET_KEY_ROW3(state)      GPIO_WriteBit(KEY_OUT3_PORT, KEY_OUT3_PIN, state)

#define GET_KEY_ROW0()           GPIO_ReadInputDataBit(KEY_OUT0_PORT, KEY_OUT0_PIN)
#define GET_KEY_ROW1()           GPIO_ReadInputDataBit(KEY_OUT1_PORT, KEY_OUT1_PIN)
#define GET_KEY_ROW2()           GPIO_ReadInputDataBit(KEY_OUT2_PORT, KEY_OUT2_PIN)
#define GET_KEY_ROW3()           GPIO_ReadInputDataBit(KEY_OUT3_PORT, KEY_OUT3_PIN)

#define SET_KEY_VERTICAL0(state) GPIO_WriteBit(KEY_IN0_PORT, KEY_IN0_PIN, state)
#define SET_KEY_VERTICAL1(state) GPIO_WriteBit(KEY_IN1_PORT, KEY_IN1_PIN, state)
#define SET_KEY_VERTICAL2(state) GPIO_WriteBit(KEY_IN2_PORT, KEY_IN2_PIN, state)
#define SET_KEY_VERTICAL3(state) GPIO_WriteBit(KEY_IN3_PORT, KEY_IN3_PIN, state)

#define GET_KEY_VERTICAL0()      GPIO_ReadInputDataBit(KEY_IN0_PORT, KEY_IN0_PIN)
#define GET_KEY_VERTICAL1()      GPIO_ReadInputDataBit(KEY_IN1_PORT, KEY_IN1_PIN)
#define GET_KEY_VERTICAL2()      GPIO_ReadInputDataBit(KEY_IN2_PORT, KEY_IN2_PIN)
#define GET_KEY_VERTICAL3()      GPIO_ReadInputDataBit(KEY_IN3_PORT, KEY_IN3_PIN)



/*按键状态*/
#define KEY_DOWN    0

#define GET_KEY_PW_STATE()       GPIO_ReadInputDataBit(KEY_PW_PORT, KEY_PW_PIN)
#define GET_KEY_FOOT_STATE()     GPIO_ReadInputDataBit(KEY_FOOT_PORT, KEY_FOOT_PIN)


extern void KeyConfig(void);
extern u8 KeyboardScan(void);
//extern void SetPowerOffKeyState(void);
//extern void ResetPowerOffKeyState(void);
//extern ErrorStatus GetPowerOffKeyState(void);
#endif
