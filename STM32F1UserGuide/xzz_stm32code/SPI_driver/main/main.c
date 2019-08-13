#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"

int main(void)
{
	   	  	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	spi_FUN_init();//SPI��ʼ��
	
	while(1)
	{
		printf("xxxzzzzzz\r\n");
		SPI_Flash_Read_ID();
		delay_ms(1000);
	}
	return 0;
	
}





