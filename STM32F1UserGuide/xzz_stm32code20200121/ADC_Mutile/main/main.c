#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "pm2_5_driver.h"

uint16_t ADC_ConvertedValue[16];

int main(void)
{
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	
	ADC_init_Cfg();//初始化启动ADC DMA
	
	printf("xxxzzzzzz\r\n");
	while(1)
	{
		printf("ADC channel0 PA0 value = %d ",ADC_ConvertedValue[0]);
		printf(" channel1 PA1 value = %d ",ADC_ConvertedValue[1]);
		printf(" channel2 PA2 value = %d ",ADC_ConvertedValue[2]);
		printf(" channel3 PA3 value = %d\r\n",ADC_ConvertedValue[3]);
		printf("ADC channel4 PA4 value = %d ",ADC_ConvertedValue[4]);
		printf(" channel5 PA5 value = %d ",ADC_ConvertedValue[5]);
		printf(" channel6 PA6 value = %d ",ADC_ConvertedValue[6]);
		printf(" channel7 PA7 value = %d\r\n",ADC_ConvertedValue[7]);
		printf("ADC channel8 PB0 value = %d",ADC_ConvertedValue[8]);
		printf(" channel9 PB1 value = %d ",ADC_ConvertedValue[9]);
		printf(" channel10 PC1 value = %d ",ADC_ConvertedValue[10]);
		printf(" channel11 PC2 value = %d\r\n",ADC_ConvertedValue[11]);
		printf("ADC channel12 PC3 value = %d",ADC_ConvertedValue[12]);
		printf(" channel13 PC4 value = %d ",ADC_ConvertedValue[13]);
		printf(" channel14 PC5 value = %d ",ADC_ConvertedValue[14]);
		printf(" channel15 PC6 value = %d ",ADC_ConvertedValue[15]);
		printf("\r\n");
		printf("========================================\r\n");	
		delay_ms(500);
			
	}
	return 0;	
}











