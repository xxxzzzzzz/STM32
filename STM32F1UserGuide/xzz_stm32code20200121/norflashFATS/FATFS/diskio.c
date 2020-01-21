/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "spi_flash.h"
#include "norflash.h"

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define SPI_NORFLASH 1

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	

	switch (pdrv) {
	case 0 :
		//result = ATA_disk_status();
		// translate the reslut code here
		return stat;

	case SPI_NORFLASH :
		 if(SPI_Flash_Read_ID() == 0xef4017)
		 {
			//norflash通信正常
			 stat = 0;
		 }
		 else
		 {
			 //norflash通信不正常
			 stat = STA_NOINIT;
		 }
		 //stat |= STA_PROETCT //flash通信正常，只是flash硬件设置了写保护

		return stat;

	case 2 :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	

	switch (pdrv) {
	case 0 :
		//result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case SPI_NORFLASH :
		 spi_FUN_init();//SPI初始化
		 return disk_status(SPI_NORFLASH);//传入SPI_NORFLASH节点，表示初始化的是NORFLASH
		 

	case 2 :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	

	switch (pdrv) {
	case 0 :
		// translate the arguments here

		//result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case SPI_NORFLASH :
		 sector = sector+512;//偏移2M
		 READ_norflash_sector(sector*4096,buff,count*4096);//norflash每个扇区数4096字节
		 res = RES_OK;//默认flash读取数据正常
		 return res;

	case 2 :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	

	switch (pdrv) {
	case 0 :
		// translate the arguments here

		//result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case SPI_NORFLASH :
		 sector = sector+512;//偏移2M字节 就是2097152字节/4096(一个扇区4096字节) = 512扇区
		 WRITE_norflash_mulitsector(sector*4096,(uint8_t *)buff,count*4096);
		 //这里和disk_read函数一样，只是buff要强转一下，其实uint8_t和BYTE都是8位的意思
		 res = RES_OK;//默认flash写数据正常
		 return res;

	case 2 :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
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
	

	switch (pdrv) {
	case 0 :

		return res;

	case SPI_NORFLASH :

		 switch (cmd)
		 {
			case GET_SECTOR_COUNT:
				*(DWORD * )buff = 1536; //1536个扇区 6M空间
			break;
			case GET_SECTOR_SIZE :
				*(WORD * )buff = 4096;
			break;
			case GET_BLOCK_SIZE :
				*(DWORD * )buff = 1;
			break;
		 }
		 
		 res = RES_OK;//默认IOCTL操作正常
		 return res;

	case 2 :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
#endif


DWORD get_fattime (void)
{
	/*这个地方写RTC时钟驱动,然后读取时间返回给上层*/
	
	return 0;//这里应该返回时间参数
}
