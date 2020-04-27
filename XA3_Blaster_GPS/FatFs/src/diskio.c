/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

//#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "includes.h"

/* Definitions of physical drive number for each media */
#define SD_CARD		0
#define AT45DB		1

//static DSTATUS SdState = STA_NOINIT;
#define BLOCK_SIZE            512 /* Block Size in Bytes */

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardPowerOn() == ERROR)
			{
				return STA_NOINIT;
			}
			else
			{
				return RES_OK;
			}
		case AT45DB :
			if(FlashHardwareInit() == ERROR)
			{
				return STA_NOINIT;
			}
			else
			{
				return RES_OK;	
			}		
		default:
			break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardReadBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
            break;
		case AT45DB :
			do
			{
				FlashReadData(BLOCK_SIZE*sector, buff, BLOCK_SIZE);
				buff += BLOCK_SIZE;
				sector++;
			} while(--count);
			return RES_OK;
		default:
			break;
	}
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardWriteBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
			break;
		case AT45DB :
			do
			{
				if(ERROR == FlashWriteData(BLOCK_SIZE*sector, (u8*)buff, BLOCK_SIZE))
				{
					return RES_ERROR;
				}
				buff += BLOCK_SIZE;
				sector++;
			} while(--count);
			return RES_OK;		
		default:
			break;
	}
	return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
//FATFS目前版本仅需处理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三个命令
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardIoCtrl(cmd, buff) != ERROR)
			{
				return RES_OK;
			}
			break;
		case AT45DB :
			switch(cmd)
			{
				case CTRL_SYNC:
					res = RES_OK;
					break;
				case GET_SECTOR_COUNT:
					*((DWORD*)buff) = FLASH_PAGE_NUM*FLASH_PAGE_BYTES/BLOCK_SIZE;//32768页，每页264字节
					res = RES_OK;
					break;
				case GET_SECTOR_SIZE:
					*((DWORD*)buff) = BLOCK_SIZE;
					res = RES_OK;
					break;		
				case GET_BLOCK_SIZE:
					*((DWORD*)buff) = BLOCK_SIZE;
					res = RES_OK;
					break;			
				default:
					res = RES_PARERR;
					break;
			}
			return res;	
		default:
			break;
	}
	return RES_PARERR;	
}
#endif

/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */ 
DWORD get_fattime (void)
{
    DWORD date;
	RTC_Typedef Time;
	
	Time = RTC_Type;
	if(Time.year>1980)//年份改为1980年起,0-11月改为1-12月,将秒数改为0-29
	{
		Time.year -= 1980;
		Time.second /=2;
		date = (Time.year << 25)|(Time.month<<21)|(Time.day<<16)|\
				(Time.hour<<11)|(Time.minute<<5)|(Time.second);

		return date;		
	}
	else
	{
		return 0;
	}


}
