#ifndef _POWER_H_
#define _POWER_H_

#define CH_SW1_PORT					GPIOC
#define CH_SW1_PORT_RCC				RCC_AHB1Periph_GPIOC
#define CH_SW1_PIN					GPIO_Pin_8
#define CONNECT_CH1()			   	GPIO_ResetBits(CH_SW1_PORT, CH_SW1_PIN)
#define DISCONNECT_CH1()		   	GPIO_SetBits(CH_SW1_PORT, CH_SW1_PIN)

#define CH_SW2_PORT					GPIOC
#define CH_SW2_PORT_RCC				RCC_AHB1Periph_GPIOC
#define CH_SW2_PIN					GPIO_Pin_7
#define CONNECT_CH2()			   	GPIO_ResetBits(CH_SW2_PORT, CH_SW2_PIN)
#define DISCONNECT_CH2()		   	GPIO_SetBits(CH_SW2_PORT, CH_SW2_PIN)

#define CH_SW3_PORT					GPIOC
#define CH_SW3_PORT_RCC				RCC_AHB1Periph_GPIOC
#define CH_SW3_PIN					GPIO_Pin_6
#define CONNECT_CH3()			   	GPIO_ResetBits(CH_SW3_PORT, CH_SW3_PIN)
#define DISCONNECT_CH3()		   	GPIO_SetBits(CH_SW3_PORT, CH_SW3_PIN)

#define CH_SW4_PORT					GPIOG
#define CH_SW4_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW4_PIN					GPIO_Pin_8
#define CONNECT_CH4()			   	GPIO_ResetBits(CH_SW4_PORT, CH_SW4_PIN)
#define DISCONNECT_CH4()		   	GPIO_SetBits(CH_SW4_PORT, CH_SW4_PIN)

#define CH_SW5_PORT					GPIOG
#define CH_SW5_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW5_PIN					GPIO_Pin_7
#define CONNECT_CH5()			   	GPIO_ResetBits(CH_SW5_PORT, CH_SW5_PIN)
#define DISCONNECT_CH5()		   	GPIO_SetBits(CH_SW5_PORT, CH_SW5_PIN)

#define CH_SW6_PORT					GPIOG
#define CH_SW6_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW6_PIN					GPIO_Pin_6
#define CONNECT_CH6()			   	GPIO_ResetBits(CH_SW6_PORT, CH_SW6_PIN)
#define DISCONNECT_CH6()		   	GPIO_SetBits(CH_SW6_PORT, CH_SW6_PIN)

#define CH_SW7_PORT					GPIOG
#define CH_SW7_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW7_PIN					GPIO_Pin_5
#define CONNECT_CH7()			   	GPIO_ResetBits(CH_SW7_PORT, CH_SW7_PIN)
#define DISCONNECT_CH7()		   	GPIO_SetBits(CH_SW7_PORT, CH_SW7_PIN)

#define CH_SW8_PORT					GPIOG
#define CH_SW8_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW8_PIN					GPIO_Pin_4
#define CONNECT_CH8()			   	GPIO_ResetBits(CH_SW8_PORT, CH_SW8_PIN)
#define DISCONNECT_CH8()		   	GPIO_SetBits(CH_SW8_PORT, CH_SW8_PIN)

#define CH_SW9_PORT					GPIOG
#define CH_SW9_PORT_RCC				RCC_AHB1Periph_GPIOG
#define CH_SW9_PIN					GPIO_Pin_3
#define CONNECT_CH9()			   	GPIO_ResetBits(CH_SW9_PORT, CH_SW9_PIN)
#define DISCONNECT_CH9()		   	GPIO_SetBits(CH_SW9_PORT, CH_SW9_PIN)

#define CH_SW10_PORT				GPIOG
#define CH_SW10_PORT_RCC			RCC_AHB1Periph_GPIOG
#define CH_SW10_PIN					GPIO_Pin_2
#define CONNECT_CH10()			   	GPIO_ResetBits(CH_SW10_PORT, CH_SW10_PIN)
#define DISCONNECT_CH10()		   	GPIO_SetBits(CH_SW10_PORT, CH_SW10_PIN)

#define PW_USB_PORT					GPIOE
#define PW_USB_PORT_RCC				RCC_AHB1Periph_GPIOE
#define PW_USB_PIN					GPIO_Pin_5
#define CLOSE_PW_USB()			   	GPIO_ResetBits(PW_USB_PORT, PW_USB_PIN)
#define OPEN_PW_USB()		   		GPIO_SetBits(PW_USB_PORT, PW_USB_PIN)

#define PW_HV_EN_PORT				GPIOF
#define PW_HV_EN_PORT_RCC			RCC_AHB1Periph_GPIOF
#define PW_HV_EN_PIN				GPIO_Pin_12

#define PW_LV_EN_PORT				GPIOF
#define PW_LV_EN_PORT_RCC			RCC_AHB1Periph_GPIOF
#define PW_LV_EN_PIN				GPIO_Pin_13

#define PW_5V_EN_PORT				GPIOF
#define PW_5V_EN_PORT_RCC			RCC_AHB1Periph_GPIOF
#define PW_5V_EN_PIN				GPIO_Pin_11

#define PW_INT_PORT            		GPIOB
#define PW_INT_PORT_RCC        		RCC_AHB1Periph_GPIOB
#define PW_INT_PIN             		GPIO_Pin_0

#define PW_KILL_PORT           		GPIOB
#define PW_KILL_PORT_RCC       		RCC_AHB1Periph_GPIOB
#define PW_KILL_PIN            		GPIO_Pin_1

#define PW_STATE_PORT      	   		GPIOB
#define PW_STATE_PORT_RCC      		RCC_AHB1Periph_GPIOB
#define PW_STATE_PIN           		GPIO_Pin_2

#define AC_IN_PORT      	   		GPIOB
#define AC_IN_PORT_RCC      		RCC_AHB1Periph_GPIOB
#define AC_IN_PIN           		GPIO_Pin_12

#define NOT_AC                      0
#define GET_AC_IN_STATE()         	GPIO_ReadInputDataBit(AC_IN_PORT, AC_IN_PIN)

#define OPEN_PW_HV()			   	GPIO_SetBits(PW_HV_EN_PORT, PW_HV_EN_PIN)
#define CLOSE_PW_HV()		   		GPIO_ResetBits(PW_HV_EN_PORT, PW_HV_EN_PIN)

#define OPEN_PW_LV()			   	GPIO_SetBits(PW_LV_EN_PORT, PW_LV_EN_PIN)
#define CLOSE_PW_LV()		   		GPIO_ResetBits(PW_LV_EN_PORT, PW_LV_EN_PIN)

#define OPEN_PW_5V()			   	GPIO_SetBits(PW_5V_EN_PORT, PW_5V_EN_PIN)
#define CLOSE_PW_5V()		   		GPIO_ResetBits(PW_5V_EN_PORT, PW_5V_EN_PIN)

#define POWER_ON()             		GPIO_SetBits(PW_KILL_PORT, PW_KILL_PIN)
#define POWER_OFF()            		GPIO_ResetBits(PW_KILL_PORT, PW_KILL_PIN)


#define PW_STATE_ON            		0
#define GET_PW_STATE()         		GPIO_ReadInputDataBit(PW_STATE_PORT, PW_STATE_PIN)

#define GET_PW_INT_STATE()     		GPIO_ReadInputDataBit(PW_INT_PORT, PW_INT_PIN)

extern void PowerConfig(void);
extern void ConnectChannel(u8 Channel);

#endif
