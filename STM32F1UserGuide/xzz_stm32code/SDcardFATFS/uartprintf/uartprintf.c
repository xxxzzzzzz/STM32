#include "stm32f10x.h"
#include "uartprintf.h"
#include "stdio.h"	//一定要加入stdio.h stm32里面自带的标准库
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 





void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//定义中断优先级结构体
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断组选择2组
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//配置串口USART位中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//响应优先级(子优先级)为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断优先级开启
	NVIC_Init(&NVIC_InitStructure);//初始化中断优先级
	
}


void USART_config(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure; //设置GPIO为串口功能
	USART_InitTypeDef USART_InitStructure;//设置串口参数
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//打开APB2总线GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//打开APB2总线UART1时钟
	
	//将UART1 TX发送引脚对应的GPIOA_9管脚设置成推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//将UART1 RX接受引脚GPIOA_10设置成浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	NVIC_Configuration();//串口中断优先级配置，自己定义
	
	USART_InitStructure.USART_BaudRate = baudrate;//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//配置串口数据位为8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//配置串口停止位为1位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//发送接受都启动
	USART_Init(USART1, &USART_InitStructure);//串口初始化配置
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);//使能串口1
	
}




void USART1_Send(uint8_t ch)
{
//	USART_SendData(USART1, ch);//USART1串口发送字节
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//必须等待发送完成
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (uint8_t) ch;
}


void USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//检查USART1是不是发生了中断
	{
		ucTemp = USART_ReceiveData(USART1);//接受串口
		USART1_Send(ucTemp);//把接受的的数据返回给电脑
	}
}






