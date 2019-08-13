#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "lcd.h"


int main(void)
{
	
	uint32_t i=0;
	char temp = '我';
	
	
	RCC_configuration();//初始化时钟
	LCD_Init();

	delay_ms(5000);
		
	
	LCD_Clear(BLUE);

	
	while(1)
	{

	}
	
	return 0;
	
}









