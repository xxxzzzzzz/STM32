#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "pm2_5_driver.h"

uint32_t ADC_ConvertedValue;

int main(void)
{
	
	uint16_t ADC_1,ADC_2;
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	
	ADC_init_Cfg();//初始化启动ADC DMA
	
	printf("xxxzzzzzz\r\n");
	while(1)
	{
		ADC_2 = ((ADC_ConvertedValue & 0xffff0000)>>16);//这是ADC2的数据
		ADC_1 = (ADC_ConvertedValue & 0xffff);//这是ADC1的数据
		printf("ADC1 value = %d ADC_2 value = %d\r\n",ADC_1,ADC_2);
		delay_ms(500);
			
	}
	return 0;	
}











