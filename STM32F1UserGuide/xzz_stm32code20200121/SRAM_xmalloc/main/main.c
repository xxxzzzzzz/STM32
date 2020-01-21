#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "malloc.h" //动态分配函数malloc  free 包含该头文件

#define NULL 0

int main(void)
{
	char *p;
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	mem_init();//初始化动态内存
	delay_ms(5000);
	
	p=mymalloc(2048);//动态分配2K内存空间给指针p
	if(p != NULL)//证明内存分配成功可以写数据
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





