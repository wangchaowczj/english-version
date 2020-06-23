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
#define FS_USB      1
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
//	BYTE status = USBH_MSC_OK;
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardReadBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
			break;
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
//	BYTE status = USBH_MSC_OK;
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardWriteBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
			break;	
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
//	DRESULT res;
//FATFSĿǰ�汾���账��CTRL_SYNC��GET_SECTOR_COUNT��GET_BLOCK_SIZ��������
	switch (pdrv) 
	{
		case SD_CARD :
			if(SdCardIoCtrl(cmd, buff) != ERROR)
			{
				return RES_OK;
			}
			break;
//	case FS_USB :
//		{
//			//if (drv) return RES_PARERR;
//			res = RES_ERROR;

//			//if (Stat & STA_NOINIT) return RES_NOTRDY;
//			switch (cmd)
//			{
//				case CTRL_SYNC :		/* Make sure that no pending write process */
//					res = RES_OK;
//					break;

//				case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
//					*(DWORD*)buff = (DWORD) HostInfo.Msc.BlackCount*HostInfo.Msc.BlackSize;
//					res = RES_OK;
//					break;

//				case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
//					*(WORD*)buff = 512;
//					res = RES_OK;
//					break;

//				case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */\
//					*(DWORD*)buff = HostInfo.Msc.BlackSize;
//					res = RES_OK;
//					break;

//				default:
//					res = RES_PARERR;
//					break;
//			}
//			return res;
//		}			
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
//    DWORD date;
//	RTC_Typedef Time;
//	
//	Time = RTC_Type;
//	if(Time.year>1980)//��ݸ�Ϊ1980����,0-11�¸�Ϊ1-12��,��������Ϊ0-29
//	{
//		Time.year -= 1980;
//		Time.second /=2;
//		date = (Time.year << 25)|(Time.month<<21)|(Time.day<<16)|\
//				(Time.hour<<11)|(Time.minute<<5)|(Time.second);

//		return date;		
//	}
//	else
	{
		return 0;
	}
}