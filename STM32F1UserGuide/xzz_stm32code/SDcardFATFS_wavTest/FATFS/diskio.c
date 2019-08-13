/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stm32_eval_sdio_sd.h"
#include "sdcode.h"
#include <string.h>
/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SPI_NORFLASH 1
#define SDcard 0


extern SD_CardInfo SDCardInfo;
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	SD_Error Status = STA_NOINIT;//定义一个变量检查SD卡初始化状态

	switch (pdrv) {
//	case 0 :
//		//result = ATA_disk_status();
//		// translate the reslut code here
//		return stat;
	case SDcard : //2号节点就是SD卡，1号节点是norflash
		
		 stat &= ~STA_NOINIT;//不管SD卡初始化是否成功我们都返回正确
		 break;
	
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

		 break;
		 
	default:
		stat = STA_NOINIT;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	SD_Error Status = SD_OK;

	switch (pdrv) {
//	case 0 :
//		//result = ATA_disk_initialize();

//		// translate the reslut code here

//		return stat;
	case SDcard : //2号节点就是SD卡，1号节点是norflash
		 SDIO_NVIC_Configuration(); //SDIO中断优先级初始化不要忘了
		 if((Status = SD_Init()) != SD_OK)
		 {
			stat = STA_NOINIT;//SD卡初始化失败
		 }
		 else
		 {
			stat &= ~STA_NOINIT;//SD卡初始化成功
		 }
		 break;
		 
	case SPI_NORFLASH :
		 spi_FUN_init();//SPI初始化
		 return disk_status(SPI_NORFLASH);//传入SPI_NORFLASH节点，表示初始化的是NORFLASH
		 break;
	default:
		 stat = STA_NOINIT;
	
	}
	return stat;
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
	SD_Error ret_status = SD_OK;
	
	
	switch (pdrv) {
//	case 0 :
//		// translate the arguments here

//		//result = ATA_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;
	case SDcard :
		 if((DWORD)buff&3)
		 {
			 while(count--)
			 {	
				__align(4) uint8_t tempbuff[512];//让数组按照4字节对齐
				res = disk_read(SDcard,(BYTE *)tempbuff,sector++,1);
				if(res == RES_OK)
				{
					memcpy(buff,tempbuff,SDCardInfo.CardBlockSize);
					buff+=SDCardInfo.CardBlockSize;
				}	 	
			 }
			 return res;
		 }
		else
		{
			ret_status = SD_ReadMultiBlocks(buff, sector*SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize,count);
			ret_status = SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
			if(ret_status == SD_OK)
			{
				res = RES_OK;
			}
			else
			{
				res = RES_ERROR;
			}
		}
		break;
		
	case SPI_NORFLASH :
		 sector = sector+512;//偏移2M
		 READ_norflash_sector(sector*4096,buff,count*4096);//norflash每个扇区数4096字节
		 res = RES_OK;//默认flash读取数据正常
		 break;

	
		 
		 /*----------------------------*/
		
		 default:
			res = RES_PARERR;
	}

	return res;
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
	DRESULT res = RES_PARERR;
	SD_Error ret_status = SD_OK;
	
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}
	
	switch (pdrv) {
//	case 0 :
//		// translate the arguments here

//		//result = ATA_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;
	case SDcard :
		
		if((DWORD)buff&3)
		 {
			 while(count--)
			 {	
				__align(4) uint8_t tempbuff[512];//让数组按照4字节对齐
				memcpy(tempbuff,buff,SDCardInfo.CardBlockSize);
				res = disk_write(SDcard,(BYTE *)tempbuff,sector++,1);
				if(res == RES_OK)
				{
					buff+=SDCardInfo.CardBlockSize;;
				}	 	
			 }
			 return res;
		 }
		 /*-------------------------------*/	
		 else
		 {
			ret_status = SD_WriteMultiBlocks((uint8_t *)buff, sector*SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize, count);
		 	 
			if(ret_status == SD_OK)
			{
				ret_status = SD_WaitWriteOperation();
				while(SD_GetStatus() != SD_TRANSFER_OK);
			}
			if(ret_status == SD_OK)
			{
				res = RES_OK;
			}
			else
			{
				res = RES_ERROR; 
			}
		 }
		 break;
		 
	case SPI_NORFLASH :
		 sector = sector+512;//偏移2M字节 就是2097152字节/4096(一个扇区4096字节) = 512扇区
		 WRITE_norflash_mulitsector(sector*4096,(uint8_t *)buff,count*4096);
		 //这里和disk_read函数一样，只是buff要强转一下，其实uint8_t和BYTE都是8位的意思
		 res = RES_OK;//默认flash写数据正常
		 return res;

	
		 default:
			res = RES_PARERR;
	}

	return res;
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
//	case 0 :

//		return res;
	case SDcard :
		 
	     switch (cmd)
		 {
			case GET_SECTOR_COUNT:
				*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize; 
			     //SD卡总大小除以每个扇区的字节就是扇区个数
			break;
			case GET_SECTOR_SIZE :
				*(WORD * )buff = SDCardInfo.CardBlockSize;//每个扇区大小是512字节
			break;
			case GET_BLOCK_SIZE :
				*(DWORD * )buff = 1;//SD卡也是一次只擦除一个扇区
			break;
		 }
		res = RES_OK;//默认IOCTL操作正常
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

	
	}

	return RES_PARERR;
}
#endif


DWORD get_fattime (void)
{
	/*这个地方写RTC时钟驱动,然后读取时间返回给上层*/
	
	return ((WORD)(2015-1980)<<25) //year 2015
		|((DWORD)1<<21)	//month 1
		|((DWORD)1<<16)	//mday 1
		|((DWORD)0<<11)	//hour 0
		|((DWORD)0<<5)	//min 0
		|((DWORD)0<<1); //sec 0
}
