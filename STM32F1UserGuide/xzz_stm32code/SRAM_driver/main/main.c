#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "sram.h"

#define SRAM_BASE_ADDR (0x68000000) //�������ǲ���NE3�ӿڵĻ���ַ��
#define SRAM_SIZE (1*1024*1024) //���ⲿ��SRAM��1M�ֽڴ�С

int main(void)
{
	uint8_t *pSRAM;
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	
	init_FSMC_GPIO();//��ʼ��FSMC����SRAM������
	FSMC_ModeConfig();//����FSMC����ģʽ
	
	pSRAM = (uint8_t *)SRAM_BASE_ADDR;//ǿ��ת�����ǰ�SRAM_BASE_ADDR�������ɵ�ַ��ʶ��ֵ
	
	*pSRAM = 0xab;
		
	printf("��������������Ϊ 0x%x\r\n",*pSRAM);
	while(1)
	{
		printf("xxxzzzzzz\r\n");
		
		delay_ms(1000);
	}
	return 0;
	
}





