#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "dma.h"

int main(void)
{
	   	  	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	delay_ms(5000);
	printf("DMA before\r\n");
	
	DMA1_UART_init();//����usart_buffer�����������ݵ����ڳ�ʼ��
	
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//������Ǵ�������DMA�������ݵ����ڣ������Ŀ�ʼ����
	
	while(1)
	{
		
	}
	return 0;
	
}





