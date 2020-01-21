#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "dma.h"

int main(void)
{
	   	  	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	delay_ms(5000);
	printf("DMA before\r\n");
	
	DMA1_UART_init();//开启usart_buffer发送数组数据到串口初始化
	
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//这里就是串口请求DMA发送数据到串口，真正的开始发送
	
	while(1)
	{
		
	}
	return 0;
	
}





