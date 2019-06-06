/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sdio.h"

/* Definitions of physical drive number for each drive */
// #define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
// #define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
// #define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	if (pdrv == DEV_SD)
	{
		u8 tmp = SD_Init();
		if (tmp)
		{
			return RES_NOTRDY;
		}
		else
		{
			return RES_OK;
		}
	}
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	/* Physical drive nmuber to identify the drive */
	BYTE *buff,   /* Data buffer to store read data */
	DWORD sector, /* Start sector in LBA */
	UINT count	/* Number of sectors to read */
)
{
	if (count == 0)
	{
		return RES_PARERR;
	}
	if (pdrv == DEV_SD)
	{
		u8 tmp;
		tmp = SD_ReadDisk(buff, sector, count);
		if (tmp)
		{ //Failed
			SD_Init();
			tmp = SD_ReadDisk(buff, sector, count);
		}
		if (tmp)
			return RES_ERROR;
		else
			return RES_OK;
	}
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	DWORD sector,	 /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	if (pdrv == DEV_SD && count > 0)
	{
		u8 tmp;
		tmp = SD_WriteDisk(buff, sector, count);
		if (tmp)
		{ //Failed
			SD_Init();
			tmp = SD_WriteDisk(buff, sector, count);
		}
		if (tmp)
			return RES_ERROR;
		else
			return RES_OK;
	}
	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	switch (cmd)
	{
	case GET_SECTOR_SIZE:
		*(DWORD *)buff = 512;
		return RES_OK;
	case GET_BLOCK_SIZE:
		*(WORD *)buff = SDCardInfo.CardBlockSize;
		return RES_OK;
	case GET_SECTOR_COUNT:
		*(DWORD *)buff = SDCardInfo.CardCapacity / 512;
		return RES_OK;
	}
	return RES_PARERR;
}
