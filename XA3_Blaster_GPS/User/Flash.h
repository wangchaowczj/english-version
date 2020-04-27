/**
  *************************************************************************************
  * @file    Flash.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   Flash模块源文件
  *************************************************************************************
  */
#ifndef _FLASH_H_
#define _FLASH_H_

/*----模块宏定义----*/
/* 命令列表 */
#define  DFCMD_CONT_READ_LF         0x03  /* Continuous array read(low frequency). */
#define  DFCMD_CONT_READ_HF         0x0B  /* Continuous array read(high frequency) */
#define  DFCMD_BLOCK_ERASE          0x50  /* Block erase */
#define  DFCMD_SECTOR_ERASE         0x7C  /* Sector erase */
#define  DFCMD_PAGE_ERASE           0x81  /* Page erase. */ 
#define  DFCMD_BUF1_PROG            0x82  /* Main memory page program through buffer 1 */
#define  DFCMD_BUF1_FLASH           0x83  /* Buffer 1 flash with page erase. */
#define  DFCMD_BUF1_WRITE           0x84  /* Buffer 1 write. */
#define  DFCMD_BUF2_PROG            0x85  /* Main memory page program through buffer 2 */
#define  DFCMD_BUF2_FLASH           0x86  /* Buffer 2 flash with page erase. */
#define  DFCMD_BUF2_WRITE           0x87  /* Buffer 2 write. */
#define  DFCMD_BUF1_FLASH_NE        0x88  /* Buffer 1 to main memory page program without built-in erase. */
#define  DFCMD_BUF2_FLASH_NE        0x89  /* Buffer 1 to main memory page program without built-in erase. */
#define  DFCMD_CHIP_ERASE           0xC7  /* Chip erase. */
#define  DFCMD_BUF1_READ_LF         0xD1  /* Buffer 1 read(low frequency). */
#define  DFCMD_READ_PAGE            0xD2  /* Read main memory page. */
#define  DFCMD_BUF2_READ_LF         0xD3  /* Buffer 2 read(low frequency). */
#define  DFCMD_BUF1_READ            0xD4  /* Buffer 1 read(higy frequency). */
#define  DFCMD_BUF2_READ            0xD6  /* Buffer 2 read(higy frequency). */
#define  DFCMD_READ_STATUS          0xD7  /* Read status register. */
#define  DFCMD_CONT_READ            0xE8  /* Continuos read. */
#define  DFCMD_MM_PAGE_TO_B1_XFER   0x53  /* Main memory page to buffer 1 transfer.*/
#define  DFCMD_MM_PAGE_TO_B2_XFER   0x55  /* Main memory page to buffer 2 transfer.*/
#define  DFCMD_AUTO_REW_B1          0x58  /* Auto page rewrite through buffer 1*/
#define  DFCMD_AUTO_REW_B2          0x59  /* Auto page rewrite through buffer 2*/
#define  DFCMD_READ_ID              0x9F  /* READ ID DEVICE*/

/* FLASH状态Mask */
#define AT45DBX_MASK_BUSY           0x80  // 指示AT45DBX是忙还是准备好状态
#define AT45DBX_MASK_COMP           0x40  // 指示AT45DBX中Buffer与Main Flash的比较结果
#define AT45DBX_MASK_DINFO          0x3C  // 指示AT45DBX的芯片信息

/***管脚定义***/
#define FLASH_WP_PORT                GPIOI
#define FLASH_WP_PORT_RCC            RCC_AHB1Periph_GPIOI
#define FLASH_WP_PIN                 GPIO_Pin_5

#define FLASH_SCLK_PORT              GPIOI 
#define FLASH_SCLK_PORT_RCC          RCC_AHB1Periph_GPIOI
#define FLASH_SCLK_PIN               GPIO_Pin_1

#define FLASH_MISO_PORT              GPIOI
#define FLASH_MISO_PORT_RCC          RCC_AHB1Periph_GPIOI
#define FLASH_MISO_PIN               GPIO_Pin_2

#define FLASH_MOSI_PORT              GPIOI
#define FLASH_MOSI_PORT_RCC          RCC_AHB1Periph_GPIOI
#define FLASH_MOSI_PIN               GPIO_Pin_3

#define FLASH_CS_PORT                GPIOI
#define FLASH_CS_PORT_RCC            RCC_AHB1Periph_GPIOI
#define FLASH_CS_PIN                 GPIO_Pin_0

#define FLASH_RST_PORT               GPIOI 
#define FLASH_RST_PORT_RCC           RCC_AHB1Periph_GPIOI
#define FLASH_RST_PIN                GPIO_Pin_4

#define FlashSPI_RCC   RCC_APB1Periph_SPI2
#define FlashSPI  SPI2

//AT45DB
#define FLASH_RESET_L()   GPIO_ResetBits(FLASH_RST_PORT, FLASH_RST_PIN);
#define FLASH_RESET_H()   GPIO_SetBits(FLASH_RST_PORT, FLASH_RST_PIN);

#define FLASH_CS_L()   GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN);
#define FLASH_CS_H()   GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN);

#define FLASH_WP_L()   GPIO_ResetBits(FLASH_WP_PORT, FLASH_WP_PIN);
#define FLASH_WP_H()   GPIO_SetBits(FLASH_WP_PORT, FLASH_WP_PIN);

typedef enum {
    AT45DBX_READ = 0,
    AT45DBX_WRITE
} AT45DBXDIR;         // AT45DBX数据访问方向

#ifndef NULL
#define NULL 0 
#endif

#define FLASH_PAGE_BYTES     264   //
#define FLASH_PAGE_NUM       32768

extern void FlashGpioConfig(void);
extern ErrorStatus FlashHardwareInit(void);
extern ErrorStatus FlashErasePage(u16 Page);
extern ErrorStatus FlashEraseBlock(u16 Block);
extern ErrorStatus FlashEraseChip(void);
extern void FlashReadData(u32 Addr, u8 *Data, u16 Bytes);
extern ErrorStatus FlashWriteData(u32 Addr, const u8 *Data, u16 Bytes);



#endif /* _FLASH_AT45DBX_H_ */

/*----End Of File----*/
