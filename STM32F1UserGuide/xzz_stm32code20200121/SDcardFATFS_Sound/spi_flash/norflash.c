#include "stm32f10x.h"
#include "norflash.h"
#include "spi_flash.h"
#include "uartprintf.h"
#include "stdio.h"

//void SPI_Flash_Read_ID(void)
//{
//	uint32_t flash_ID = 0,subID = 0;
//	
//	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
//	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
//	SPI_send_Byte(JEDEC_ID);
//	flash_ID = SPI_read_Byte();
//	subID = SPI_read_Byte();
//	subID = subID<<8;
//	subID |= SPI_read_Byte();
//	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1	
//	
//	printf("flash ID = %x\r\n",flash_ID);
//	printf("subID    = %x\r\n",subID);
//}


uint32_t SPI_Flash_Read_ID(void)
{
	uint32_t flash_ID = 0,subID = 0;
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	SPI_Send_Read_Byte(JEDEC_ID);
	flash_ID = SPI_Send_Read_Byte(0x00);
	
	subID = SPI_Send_Read_Byte(0x00);
	subID = subID<<8;
	subID |= SPI_Send_Read_Byte(0x00);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1	
	
	flash_ID = ((flash_ID <<16)|subID);
	printf("FLASH ID = %x\r\n",flash_ID);
	return flash_ID;
}



/*ÒòÎª²Á³ıflash·Ç³£ÏûºÄÊ±¼ä,ËùÒÔÒªĞ´µÈ´ıflashÄÚ²¿²Á³ıÍê³É*/
void wait_flass_Erase(void)
{
	uint8_t status_reg = 0;
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_STATUS);//·¢ËÍ¶ÁÈ¡×´Ì¬¼Ä´æÆ÷ÃüÁî
	do
	{
		status_reg = SPI_Send_Read_Byte(0x00);//·¢ËÍÎŞÓÃµÄÊı¾İ²úÉúÊ±ÖÓ£¬°Ñflash·µ»ØµÄÊı¾İ¶Á³öÀ´
	
	}while((status_reg & 0x01) == 1);//Èç¹û×´Ì¬¼Ä´æÆ÷ÊÇ1¾Í±íÊ¾flash»¹Ã»ÓĞ²Á³ıºÃ£¬¼ÌĞøµÈ´ı
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flash²Á³ıÍê³É£¬¾ÍÀ­¸ßCSÒı½Å	
}

/*²Á³ıflashÖ¸¶¨ÉÈÇø*/
void spi_Erase_sector(uint32_t addr)
{
	norflash_Write_ENABLE();//Ğ´Ê¹ÄÜ
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(ERASE_SECTOR);//·¢ËÍ0x20ÃüÁî
	SPI_Send_Read_Byte((addr>>16)&0xff);//·¢ËÍµØÖ·µÄ¸ß8Î»
	SPI_Send_Read_Byte((addr>>8)&0xff);//·¢ËÍµØÖ·ÖĞ8Î»
	SPI_Send_Read_Byte(addr&0xff);//·¢ËÍµØÖ·µÍ8Î»
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flashµØÖ·Ğ´ÈëÍê±Ï
}



/*¶ÁÈ¡flashÉÈÇøÀïÃæµÄÄÚÈİ*/
/*addr:Òª¶ÁÈ¡flashÉÈÇøµÄÊ×µØÖ·
* readbuff:½«¶ÁÈ¡µÄÒ»¸öÉÈÇø4096×Ö½ÚĞ´Èëbuff£¬
* byte_number:¶ÁÈ¡¶àÉÙ¸ö×Ö½Ú
*/
void READ_norflash_sector(uint32_t addr,uint8_t *readbuff,uint32_t byte_number)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_DATA);//·¢ËÍ¶ÁÈ¡ÉÈÇøÊı¾İÃüÁ
	SPI_Send_Read_Byte((addr>>16)&0xff);//·¢ËÍµØÖ·µÄ¸ß8Î»
	SPI_Send_Read_Byte((addr>>8)&0xff);//·¢ËÍµØÖ·ÖĞ8Î»
	SPI_Send_Read_Byte(addr&0xff);//·¢ËÍµØÖ·µÍ8Î»
	
	while(byte_number--)
	{
		*readbuff = SPI_Send_Read_Byte(0x00);//·¢ËÍÎŞ¹ØÊı¾İ²úÉúÊ±ÖÓ£¬²ÅÄÜ¶ÁÈ¡
		readbuff++;//buffÒÆ¶¯µ½ÏÂÒ»¸öµØÖ·
	}
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}


/*norflashĞ´Ê¹ÄÜ*/
void norflash_Write_ENABLE(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(WRITE_ENABLE);//Ğ´Ê¹ÄÜÃüÁî
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}



/*Ğ´Êı¾İ½øflashÉÈÇø*/
/*addr:ÒªĞ´ÈëflashÉÈÇøµÄÆğÊ¼µØÖ·
* writebuff:½«256¸ö×Ö½ÚĞ´ÈëflashÉÈÇøµÄ»º´æ
* byte_number:Ğ´¶àÉÙ¸ö×Ö½Ú
*/
void WRITE_norflash_sector(uint32_t addr,uint8_t *writebuff,uint32_t byte_number)
{
	if(byte_number > 256)//ÅĞ¶ÏÈç¹ûĞ´Èë´óÓÚ256¸ö×Ö½Ú¾Í±¨´í£¬ÍË³ö³ÌĞò
	{
		printf("write flash data error\r\n");
		return;
	}
	else
	{
		spi_Erase_sector(addr);//Ğ´ÉÈÇøÖ®Ç°Òª²Á³ıÉÈÇø
		wait_flass_Erase();//µÈ´ıflashÄÚ²¿²Á³ıÍê³É
	
		norflash_Write_ENABLE();//²Á³ıÉÈÇøÍêÁËÖ®ºó£¬Ğ´ÉÈÇøÒ²ÒªÊ¹ÄÜ
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
		SPI_Send_Read_Byte(WRITE_DATA);//·¢ËÍĞ´Êı¾İÃüÁî
		SPI_Send_Read_Byte((addr>>16)&0xff);//·¢ËÍµØÖ·µÄ¸ß8Î»
		SPI_Send_Read_Byte((addr>>8)&0xff);//·¢ËÍµØÖ·ÖĞ8Î»
		SPI_Send_Read_Byte(addr&0xff);//·¢ËÍµØÖ·µÍ8Î»
	
		while(byte_number--)
		{
			SPI_Send_Read_Byte(*writebuff);//·¢ËÍÃ¿Ò»¸ö×Ö½ÚĞ´ÈëflashÉÈÇø
			writebuff++;//buffÒÆ¶¯µ½ÏÂÒ»¸öµØÖ·
		}
	
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	}
	
}



/*Ğ´Êı¾İ½øflash¶à¸öÉÈÇø*/
/*WriteAddr:ÒªĞ´ÈëflashÉÈÇøµÄÆğÊ¼µØÖ·
* writebuff:½«ÈÎÒâÊıÁ¿×Ö½ÚĞ´ÈëflashÉÈÇøµÄ»º´æ
* byte_number:Ğ´¶àÉÙ¸ö×Ö½Ú
*/
void WRITE_norflash_mulitsector(uint32_t WriteAddr,uint8_t *writebuff,uint32_t byte_number)
{
	uint8_t page = 0,addr = 0,count = 0,nofullpage = 0;
	
	addr = WriteAddr%256;
	count = 256 - addr;
	page = byte_number / 256;//¼ÆËãĞ´¶àÉÙÒ³
	nofullpage = byte_number % 256;//¼ÆËãÊ£ÏÂ²»ÂúÒ»Ò³µÄ×Ö½ÚÊı
	
	if(addr == 0)//µØÖ·¶ÔÆë
	{
		if(page == 0)//Èç¹ûbyte_numberµÄÖµ²»ÊÇ4096¶øÊÇĞ¡ÓÚ256£¬¾Í°´ÕÕ256Ò³Ğ´¾ÍÊÇ
		{
			WRITE_norflash_sector(WriteAddr,writebuff,byte_number);//Õâ¸öº¯ÊıÒ»´ÎÖ»ÄÜ256×Ö½Ú
		}
		else
		{
			while(page--)
			{
				WRITE_norflash_sector(WriteAddr,writebuff,256);
				WriteAddr+=256;
				writebuff+=256;
			}
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//°Ñ×îºóÊ£ÏÂµÄ¶àÓà²»Âú256×Ö½ÚµÄÊı¾İĞ´Íê
		}
	}
	else
	{
		byte_number= byte_number - count;
		page = byte_number / 256;//¼ÆËãĞ´¶àÉÙÒ³
		nofullpage = byte_number % 256;//¼ÆËãÊ£ÏÂ²»ÂúÒ»Ò³µÄ×Ö½ÚÊı
		
		WRITE_norflash_sector(WriteAddr,writebuff,count);
		WriteAddr = WriteAddr + count;
		writebuff = writebuff + count;
		
		while(page--)
		{
			WRITE_norflash_sector(WriteAddr,writebuff,256);
			WriteAddr = WriteAddr + 256;
			writebuff = writebuff + 256;
		}
		if(nofullpage !=0 )
		{
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//Èç¹ûÓĞ¶àÓà²»ÂúÒ»Ò³µÄÊı¾İ¾Í°ÑËüĞ´Íê
		}
	}
}


