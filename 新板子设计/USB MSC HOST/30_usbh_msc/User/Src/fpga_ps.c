 /*
 * FILE								: fpga_ps.c
 * DESCRIPTION				: This file is iCore4 fpga_ps driver.
 * Author							: zh.
 * Copyright					:
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 06/15/2017
 *
 * create.
 * -------------------
 */
 
//----------------------include----------------------------//

#include "fpga_ps.h"
#include "fatfs.h"
#include "usart2.h"
#include "stm32h7xx_hal.h"

//------------------- Function Prototype ------------------//

static int initialize(void);
static int from_tf(void);
static int ps(FIL *);

//-----------------------variable--------------------------//

FPGA_PS_T fpga_ps = {
	.initialize = initialize,
	.from_tf = from_tf,
};

extern FATFS fatfs0,fatfs1;
//--------------------Function----------------------------//
/*
 * Name								: initialize
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 06/15/2017
 * 
 * create.
 * -------------------
 */
static int initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//≥ı ºªØPS≈‰÷√“˝Ω≈ 
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	
	//PS_DATA0<--->PI8 
	//PS_DCLK<--->PI4
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	
	//PS_NCONFIG<--->PC6
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//nSTATUS<--->PD4
	//CONFDONE<--->PC13
	GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	

	return 0;
}
/*
 * Name								: from_tf
 * Description				: ---
 * Author							: zh.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 06/15/2017
 * 
 * create.
 * -------------------
 */
static int from_tf(void)
{
	FIL fil;
	FRESULT res;

		
	res = f_open(&fil,"0:/led.rbf",FA_READ);
		
	if(res != RES_OK){
		usart2.printf(" *f_open error!\r\n");
		return -1;
	}
	res = f_lseek(&fil,0);
	if(res != RES_OK){
		usart2.printf(" *f_lseek error!\r\n");
		return -1;
	}
	
	return ps(&fil);
	
	
}
/*
 * Name								: ps
 * Description				: ---
 * Author							: ysloveivy.
 *
 * History
 * -------------------
 * Rev								: 0.00
 * Date								: 12/03/2014
 * 
 * create.
 * -------------------
 */
static int ps(FIL *f)
{	
	int i;
	int k;
	unsigned int counter;
	unsigned long int ncounter = 0;
	unsigned char buffer[1024];
	FRESULT res;
	FILINFO finfo;
	
	NCONFIG_OFF;
	DCLK_OFF;
	for(i = 0; i < 5000; i++);
	if(NSTATUS == 1){
		usart2.printf("\r\n *fpga error!\r\n");
		return -1;
	}
	for(i = 0;i < 40;i++);
	NCONFIG_ON;
	for(i = 0; i < 40; i++);
	f_stat ("0:/led.rbf",&finfo);	
	
	while(ncounter < finfo.fsize)	{
    res = f_read(f,buffer,1024,&counter);
		if(res != RES_OK){
			usart2.printf("\r\n *f_read error!\r\n");
			return -1;
		}
		for(k = 0; k < counter; k++) {
		  for(i = 0; i < 8; i++) {
	      if(buffer[k]&0x01)DATA0_ON;
				else DATA0_OFF;
				DCLK_ON;
				buffer[k] >>= 1;
			  DCLK_OFF;
	    }
		  ncounter++;
	  }
  }	
	
	if(CONFIG_DONE == 0){
		usart2.printf("\r\n *config error!\r\n");
		return -1;
	}
	
	for(i = 0; i < 40; i++) {
     DCLK_ON;
		 for(i = 0; i < 800; i++);//delay 100us
		 DCLK_OFF;
		 for(i = 0; i < 800; i++);//delay 100us
	}
	return 0;
}
