/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "stm32_eval_sdio_sd.h"
#include "sdcode.h"
#include <string.h>

/* Ϊÿ���豸����һ�������� */
#define ATA			    0     // Ԥ��SD��ʹ��
#define SPI_FLASH		1     // �ⲿSPI Flash

extern SD_CardInfo SDCardInfo;	  //���ڴ洢������Ϣ��DSR��һ���֣�


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
		        /* �豸ID��ȡ�����ȷ */
        status &= ~STA_NOINIT;	
		
			break;
    
		case SPI_FLASH:      
      /* SPI Flash״̬��⣺��ȡSPI Flash �豸ID */
//      if(sFLASH_ID == SPI_FLASH_ReadID())
//      {
//        /* �豸ID��ȡ�����ȷ */
//        status &= ~STA_NOINIT;
//      }
//      else
//      {
//        /* �豸ID��ȡ������� */
//        status = STA_NOINIT;;
//      }
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
//	 uint16_t i;
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case ATA:	         /* SD CARD */
				SDIO_NVIC_Configuration(); 
				if(SD_Init() != SD_OK )
					 status = STA_NOINIT;
				else
					 status &= ~STA_NOINIT;	
				
			break;
    
		case SPI_FLASH:    /* SPI Flash */ 
      /* ��ʼ��SPI Flash */
//			SPI_FLASH_Init();
//      /* ��ʱһС��ʱ�� */
//      i=500;
//	    while(--i);	
//      /* ����SPI Flash */
//	    SPI_Flash_WAKEUP();
//      /* ��ȡSPI FlashоƬ״̬ */
//      status=disk_status(SPI_FLASH);
			break;
      
		default:
			status = STA_NOINIT;
	}
	return status;
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
	DRESULT status = RES_PARERR;
	SD_Error sd_state;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			
			if((DWORD)buff&3 ) 
			{//buff ��ַ������
			
				while(count--)
				{
					__align(4) uint8_t tempbuff[512];
					
					status = disk_read(ATA,(BYTE *)tempbuff,sector++,1);
					if(status == RES_OK )
					{					
						memcpy(buff,tempbuff,SDCardInfo.CardBlockSize);
						
						buff += SDCardInfo.CardBlockSize;
					}	
				}
				return status;
			
			}
			else
			{
		
				sd_state = SD_ReadMultiBlocks(buff,sector * SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize,count);
				
				sd_state = SD_WaitReadOperation();
				while(SD_GetStatus() != SD_TRANSFER_OK);
			
				if (sd_state == SD_OK)
					status = RES_OK;
				else
					status = RES_ERROR;
			}
		
		break;
    
		case SPI_FLASH:
      /* ����ƫ��2MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����6MB�ռ� */
//      sector+=512;      
//      SPI_FLASH_BufferRead(buff, sector <<12, count<<12);
//      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
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
//	 uint32_t write_addr; 
	DRESULT status = RES_PARERR;
	
	SD_Error sd_state;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case ATA:	/* SD CARD */   
		if((DWORD)buff&3 ) 
		{//buff ��ַ������
		
			while(count--)
			{
				__align(4) uint8_t tempbuff[512];
				
				memcpy(tempbuff,buff,SDCardInfo.CardBlockSize);
				
				status = disk_write(ATA,(BYTE *)tempbuff,sector++,1);
				if(status == RES_OK )
				{									
					buff += SDCardInfo.CardBlockSize;
				}	
			}
			return status;
		
		}		
		else
		{

			sd_state = SD_WriteMultiBlocks((uint8_t *)buff,sector * SDCardInfo.CardBlockSize, SDCardInfo.CardBlockSize,count);
			
		  sd_state = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		
			if (sd_state == SD_OK)
				status = RES_OK;
			else
				status = RES_ERROR;
			
		}
		break;

		case SPI_FLASH:
      /* ����ƫ��2MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����6MB�ռ� */
//			sector+=512;
//      write_addr = sector<<12;    
//      SPI_FLASH_SectorErase(write_addr);
//      SPI_FLASH_BufferWrite((u8 *)buff,write_addr,count<<12);
//      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
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
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			
			switch (cmd) {
					/* �������� */
					case GET_SECTOR_COUNT:
						*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;		
					break;
					/* ������С  */
					case GET_SECTOR_SIZE :
						*(WORD * )buff = SDCardInfo.CardBlockSize;
					break;
					/* ͬʱ������������ */
					case GET_BLOCK_SIZE :
						*(DWORD * )buff = 1;
					break;        
				}
				status = RES_OK;	
		
		
			break;
    
		case SPI_FLASH:
//			switch (cmd) {
//        /* ����������1536*4096/1024/1024=6(MB) */
//        case GET_SECTOR_COUNT:
//          *(DWORD * )buff = 1536;		
//        break;
//        /* ������С  */
//        case GET_SECTOR_SIZE :
//          *(WORD * )buff = 4096;
//        break;
//        /* ͬʱ������������ */
//        case GET_BLOCK_SIZE :
//          *(DWORD * )buff = 1;
//        break;        
//      }
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


__weak DWORD get_fattime(void) {
	/* ���ص�ǰʱ��� */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

