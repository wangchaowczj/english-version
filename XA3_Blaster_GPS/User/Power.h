#ifndef _POWER_H_
#define _POWER_H_

#define PW_1_2V_EN_PORT				GPIOA
#define PW_1_2V_EN_PORT_RCC			RCC_AHB1Periph_GPIOA
#define PW_1_2V_EN_PIN				GPIO_Pin_6

#define PW_5V_SW_PORT                 GPIOG
#define PW_5V_SW_PORT_RCC             RCC_AHB1Periph_GPIOG
#define PW_5V_SW_PIN                  GPIO_Pin_12


#define PW_11_3V_SW_PORT              GPIOF
#define PW_11_3V_SW_RCC               RCC_AHB1Periph_GPIOF
#define PW_11_3V_SW_PIN               GPIO_Pin_5

#define PW_6_7V_SW_PORT               GPIOF
#define PW_6_7V_SW_RCC                RCC_AHB1Periph_GPIOF
#define PW_6_7V_SW_PIN                GPIO_Pin_12

#define LINAB_SHORT_SW_PORT           GPIOB
#define LINAB_SHORT_SW_RCC            RCC_AHB1Periph_GPIOB
#define LINAB_SHORT_SW_PIN            GPIO_Pin_14

#define LINAB_5V_CTRL_PORT            GPIOA
#define LINAB_5V_CTRL_RCC             RCC_AHB1Periph_GPIOA
#define LINAB_5V_CTRL_PIN             GPIO_Pin_10

#define LINAB_6_7V_CTRL_PORT       	  GPIOF
#define LINAB_6_7V_CTRL_RCC           RCC_AHB1Periph_GPIOF
#define LINAB_6_7V_CTRL_PIN           GPIO_Pin_3

#define PW_6_7V_5V_PORT               GPIOG
#define PW_6_7V_5V_RCC                RCC_AHB1Periph_GPIOG
#define PW_6_7V_5V_PIN                GPIO_Pin_6

#define HV_ENABLE_PORT                GPIOE
#define HV_ENABLE_RCC                 RCC_AHB1Periph_GPIOE
#define HV_ENABLE_PIN                 GPIO_Pin_1

#define PW_HV_SW_PORT                 GPIOD
#define PW_HV_SW_RCC                  RCC_AHB1Periph_GPIOD
#define PW_HV_SW_PIN                  GPIO_Pin_3

//设置总线短路
#define LINAB_SHORT_ENABLE()		  GPIO_ResetBits(LINAB_SHORT_SW_PORT, LINAB_SHORT_SW_PIN)
#define LINAB_SHORT_DISABLE()		  GPIO_SetBits(LINAB_SHORT_SW_PORT, LINAB_SHORT_SW_PIN)
#define GET_LINAB_SHORT_SW_CTRL_STATE()      GPIO_ReadOutputDataBit(LINAB_SHORT_SW_PORT, LINAB_SHORT_SW_PIN)

//1.2V电压控制LCD控制芯片供电
#define OPEN_1_2V()			   GPIO_ResetBits(PW_1_2V_EN_PORT, PW_1_2V_EN_PIN)
#define CLOSE_1_2V()		   GPIO_SetBits(PW_1_2V_EN_PORT, PW_1_2V_EN_PIN)

/***高压芯片控制***/
#define OPEN_11_3V_SW()        GPIO_SetBits(PW_11_3V_SW_PORT, PW_11_3V_SW_PIN)
#define CLOSE_11_3V_SW()       GPIO_ResetBits(PW_11_3V_SW_PORT, PW_11_3V_SW_PIN)
#define GET_11_3V_CTRL_STATE()      GPIO_ReadOutputDataBit(PW_11_3V_SW_PORT, PW_11_3V_SW_PIN)

/***高压输出到总线***/
#define OPEN_HV_SW()           GPIO_SetBits(PW_HV_SW_PORT, PW_HV_SW_PIN)
#define CLOSE_HV_SW()          GPIO_ResetBits(PW_HV_SW_PORT, PW_HV_SW_PIN)
#define GET_HV_SW_CTRL_STATE()      GPIO_ReadOutputDataBit(PW_HV_SW_PORT, PW_HV_SW_PIN)

/***6.7V电路控制***/
#define OPEN_6_7V_SW()         GPIO_SetBits(PW_6_7V_SW_PORT, PW_6_7V_SW_PIN)
#define CLOSE_6_7V_SW()        GPIO_ResetBits(PW_6_7V_SW_PORT, PW_6_7V_SW_PIN)//总线短路
#define GET_6_7V_SW_STATE()    GPIO_ReadOutputDataBit(PW_6_7V_SW_PORT, PW_6_7V_SW_PIN)


/***5V电路控制***/
#define OPEN_5V_SW()           GPIO_SetBits(PW_5V_SW_PORT, PW_5V_SW_PIN)
#define CLOSE_5V_SW()          GPIO_ResetBits(PW_5V_SW_PORT, PW_5V_SW_PIN)

/*端口电压6.7V与5V切换*/
#define SET_LV_6_7V()          GPIO_SetBits(LINAB_6_7V_CTRL_PORT, LINAB_6_7V_CTRL_PIN);\
				               GPIO_ResetBits(LINAB_5V_CTRL_PORT, LINAB_5V_CTRL_PIN)
							   
#define SET_LV_5_0V()          GPIO_ResetBits(LINAB_6_7V_CTRL_PORT, LINAB_6_7V_CTRL_PIN);\
				               GPIO_SetBits(LINAB_5V_CTRL_PORT, LINAB_5V_CTRL_PIN)
							   
#define SET_LV_0V()            GPIO_ResetBits(LINAB_6_7V_CTRL_PORT, LINAB_6_7V_CTRL_PIN);\
				               GPIO_ResetBits(LINAB_5V_CTRL_PORT, LINAB_5V_CTRL_PIN)

#define GET_LV_6_7V_SW_STATE()      GPIO_ReadOutputDataBit(LINAB_6_7V_CTRL_PORT, LINAB_6_7V_CTRL_PIN)							   
#define GET_LV_5V_SW_STATE()        GPIO_ReadOutputDataBit(LINAB_5V_CTRL_PORT, LINAB_5V_CTRL_PIN)


/*安全锁使能开关*/
#define OPEN_HV_ENABLE()       GPIO_SetBits(HV_ENABLE_PORT, HV_ENABLE_PIN)
#define CLOSE_HV_ENABLE()      GPIO_ResetBits(HV_ENABLE_PORT, HV_ENABLE_PIN)
#define GET_HV_ENABLE_SW_STATE()    GPIO_ReadOutputDataBit(HV_ENABLE_PORT, HV_ENABLE_PIN)

/***打开高压(不输出)***/
#define OPEN_HV_SW_INSIDE()      OPEN_HV_ENABLE();\
								 OPEN_11_3V_SW();
								 
#define CLOSE_HV_SW_INSIDE()     CLOSE_HV_ENABLE();\
								 CLOSE_11_3V_SW();							 
								 
/*端口输出6.7V*/
#define OPEN_BUS_6_7V()        LINAB_SHORT_DISABLE();\
                               OPEN_6_7V_SW();\
                               SET_LV_6_7V();\
                               CLOSE_HV_SW()
							   
/*关闭端口输出*/
#define CLOSE_BUS_6_7V()       SET_LV_0V()
                               //CLOSE_6_7V_SW();\//6.7关掉，USB口就没电了
                               //CLOSE_HV_SW()
							   
/*关闭总线高压*/
#define CLOSE_BUS_HV()         CLOSE_HV_ENABLE();\
							   CLOSE_11_3V_SW();\
                               CLOSE_HV_SW()
/*关闭总线电压*/							   
#define CLOSE_BUS_VOLTAGE()    CLOSE_BUS_6_7V();\
							   CLOSE_BUS_HV();\
							   LINAB_SHORT_ENABLE()
extern void PowerConfig(void);

#endif
