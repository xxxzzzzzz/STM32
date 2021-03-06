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



/*因为擦除flash非常消耗时间,所以要写等待flash内部擦除完成*/
void wait_flass_Erase(void)
{
	uint8_t status_reg = 0;
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_STATUS);//发送读取状态寄存器命令
	do
	{
		status_reg = SPI_Send_Read_Byte(0x00);//发送无用的数据产生时钟，把flash返回的数据读出来
	
	}while((status_reg & 0x01) == 1);//如果状态寄存器是1就表示flash还没有擦除好，继续等待
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flash擦除完成，就拉高CS引脚	
}

/*擦除flash指定扇区*/
void spi_Erase_sector(uint32_t addr)
{
	norflash_Write_ENABLE();//写使能
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(ERASE_SECTOR);//发送0x20命令
	SPI_Send_Read_Byte((addr>>16)&0xff);//发送地址的高8位
	SPI_Send_Read_Byte((addr>>8)&0xff);//发送地址中8位
	SPI_Send_Read_Byte(addr&0xff);//发送地址低8位
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flash地址写入完毕
}



/*读取flash扇区里面的内容*/
/*addr:要读取flash扇区的首地址
* readbuff:将读取的一个扇区4096字节写入buff，
* byte_number:读取多少个字节
*/
void READ_norflash_sector(uint32_t addr,uint8_t *readbuff,uint32_t byte_number)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_DATA);//发送读取扇区数据命�
	SPI_Send_Read_Byte((addr>>16)&0xff);//发送地址的高8位
	SPI_Send_Read_Byte((addr>>8)&0xff);//发送地址中8位
	SPI_Send_Read_Byte(addr&0xff);//发送地址低8位
	
	while(byte_number--)
	{
		*readbuff = SPI_Send_Read_Byte(0x00);//发送无关数据产生时钟，才能读取
		readbuff++;//buff移动到下一个地址
	}
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}


/*norflash写使能*/
void norflash_Write_ENABLE(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(WRITE_ENABLE);//写使能命令
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}



/*写数据进flash扇区*/
/*addr:要写入flash扇区的起始地址
* writebuff:将256个字节写入flash扇区的缓存
* byte_number:写多少个字节
*/
void WRITE_norflash_sector(uint32_t addr,uint8_t *writebuff,uint32_t byte_number)
{
	if(byte_number > 256)//判断如果写入大于256个字节就报错，退出程序
	{
		printf("write flash data error\r\n");
		return;
	}
	else
	{
		spi_Erase_sector(addr);//写扇区之前要擦除扇区
		wait_flass_Erase();//等待flash内部擦除完成
	
		norflash_Write_ENABLE();//擦除扇区完了之后，写扇区也要使能
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
		SPI_Send_Read_Byte(WRITE_DATA);//发送写数据命令
		SPI_Send_Read_Byte((addr>>16)&0xff);//发送地址的高8位
		SPI_Send_Read_Byte((addr>>8)&0xff);//发送地址中8位
		SPI_Send_Read_Byte(addr&0xff);//发送地址低8位
	
		while(byte_number--)
		{
			SPI_Send_Read_Byte(*writebuff);//发送每一个字节写入flash扇区
			writebuff++;//buff移动到下一个地址
		}
	
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	}
	
}



/*写数据进flash多个扇区*/
/*WriteAddr:要写入flash扇区的起始地址
* writebuff:将任意数量字节写入flash扇区的缓存
* byte_number:写多少个字节
*/
void WRITE_norflash_mulitsector(uint32_t WriteAddr,uint8_t *writebuff,uint32_t byte_number)
{
	uint8_t page = 0,addr = 0,count = 0,temp = 0,nofullpage = 0;
	
	addr = WriteAddr%256;
	count = 256 - addr;
	page = byte_number / 256;//计算写多少页
	nofullpage = byte_number % 256;//计算剩下不满一页的字节数
	
	if(addr == 0)//地址对齐
	{
		if(page == 0)//如果byte_number的值不是4096而是小于256，就按照256页写就是
		{
			WRITE_norflash_sector(WriteAddr,writebuff,byte_number);//这个函数一次只能256字节
		}
		else
		{
			while(page--)
			{
				WRITE_norflash_sector(WriteAddr,writebuff,256);
				WriteAddr+=256;
				writebuff+=256;
			}
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//把最后剩下的多余不满256字节的数据写完
		}
	}
	else
	{
		byte_number= byte_number - count;
		page = byte_number / 256;//计算写多少页
		nofullpage = byte_number % 256;//计算剩下不满一页的字节数
		
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
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//如果有多余不满一页的数据就把它写完
		}
	}
}


