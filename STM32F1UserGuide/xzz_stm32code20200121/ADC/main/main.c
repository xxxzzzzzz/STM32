#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "pm2_5_driver.h"

uint16_t ADC_ConvertedValue;

int main(void)
{
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	
	ADC_init_Cfg();//��ʼ������ADC DMA
	
	printf("xxxzzzzzz\r\n");
	while(1)
	{
		printf("ADC value = %d\r\n",ADC_ConvertedValue);
		delay_ms(500);
			
	}
	return 0;	
}











