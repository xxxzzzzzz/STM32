#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "sram.h"

#define SRAM_BASE_ADDR (0x68000000) //这样就是操作NE3接口的基地址了
#define SRAM_SIZE (1*1024*1024) //我外部的SRAM是1M字节大小

int main(void)
{
	uint8_t *pSRAM;
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	
	init_FSMC_GPIO();//初始化FSMC连接SRAM的引脚
	FSMC_ModeConfig();//配置FSMC工作模式
	
	pSRAM = (uint8_t *)SRAM_BASE_ADDR;//强制转换就是把SRAM_BASE_ADDR这个数变成地址认识的值
	
	*pSRAM = 0xab;
		
	printf("读出的数据内容为 0x%x\r\n",*pSRAM);
	while(1)
	{
		printf("xxxzzzzzz\r\n");
		
		delay_ms(1000);
	}
	return 0;
	
}





