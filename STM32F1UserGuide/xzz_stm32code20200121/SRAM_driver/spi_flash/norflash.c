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


void SPI_Flash_Read_ID(void)
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
	
	printf("flash ID = %x\r\n",flash_ID);
	printf("subID    = %x\r\n",subID);
}

