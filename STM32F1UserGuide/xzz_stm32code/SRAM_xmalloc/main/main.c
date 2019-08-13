#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "malloc.h" //��̬���亯��malloc  free ������ͷ�ļ�

#define NULL 0

int main(void)
{
	char *p;
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	mem_init();//��ʼ����̬�ڴ�
	delay_ms(5000);
	
	p=mymalloc(2048);//��̬����2K�ڴ�ռ��ָ��p
	if(p != NULL)//֤���ڴ����ɹ�����д����
	{
		sprintf((char *)p,"malloc request success!!");
	}
	
	printf("xxxzzzzzz\r\n");
	printf("-------%s-----\r\n",p);
	delay_ms(1000);
	printf("-------%s-----\r\n",p);
	delay_ms(1000);
	printf("malloc memuser = %d /100\r\n",mem_perused());
	myfree(p);
	printf("free memuser = %d /100\r\n",mem_perused());
	
	delay_ms(1000);
	
	return 0;	
}





