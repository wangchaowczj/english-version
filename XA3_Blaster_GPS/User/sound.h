#ifndef _SOUND_H_
#define _SOUND_H_


/*以84M进行计算，22.05K*/
#define SOUND_FREQ_22_05K   (1904*2)



#define SOUND_PWM_PORT      GPIOD
#define SOUND_PWM_PORT_RCC  RCC_AHB1Periph_GPIOD
#define SOUND_PWM_PIN       GPIO_Pin_13

#define SOUND_SW_PORT      GPIOG
#define SOUND_SW_PORT_RCC  RCC_AHB1Periph_GPIOG
#define SOUND_SW_PIN       GPIO_Pin_7

/*声音电源控制*/
#define OPEN_SOUND_SW()        GPIO_SetBits(SOUND_SW_PORT, SOUND_SW_PIN)
#define CLOSE_SOUND_SW()       GPIO_ResetBits(SOUND_SW_PORT, SOUND_SW_PIN) 
#define SOUND_SW_IS_CLOSE       0
#define GET_SOUND_SW_STATUS()      		GPIO_ReadOutputDataBit(SOUND_SW_PORT, SOUND_SW_PIN)

typedef enum 
{
    SOUND_NORMAL,
    SOUND_SPECIFIC
}SOUND_DATA_TYPE;
typedef struct
{
    SOUND_DATA_TYPE Type;
	u16 Index;
    u16 Size;
	const u8* Ptr;
}SOUND_DATA_ITEM;

typedef struct
{
	u16 Index;
    u16 Count;
	SOUND_DATA_ITEM Data[2];
}SOUND_DATA;
extern SOUND_DATA SoundData;

extern void SoundConfig(void);
extern void SoundOutput(u8 type);
extern void SoundOutputPice(u8 type);
extern INT8U CreateSoudTask(u32 type);
extern void SoundOutputRight(void);
extern void SoundOutputError(void);

#endif
