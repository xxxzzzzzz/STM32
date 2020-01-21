#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "lcd.h"

#include <string.h>
#include "lvgl.h"



int main(void)
{
	
		
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//串口初始化
	printf("xxxxzzzzz\r\n");
	//LCD_Init();
	lv_init();
	
	delay_ms(1000);

lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL); /*Add a button the current?
,→screen*/
lv_obj_set_pos(btn, 10, 10); /*Set its position*/
lv_obj_set_size(btn, 100, 50); /*Set its size*/


lv_obj_t * label = lv_label_create(btn, NULL); /*Add a label to the button*/
lv_label_set_text(label, "Button"); /*Set the labels text*/
	
	while(1)
	{
		
	}
	
	return 0;
	
}










