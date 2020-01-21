#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


void key_gpio_interrupt_init(void); //PB8 IO口外部输入中断，唤醒__WFI

int main(void)
{
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式 */
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	key_gpio_interrupt_init(); //PB8 IO口外部输入中断，唤醒__WFI
	delay_ms(5000);
	
	while(1)
	{
		printf("process 1111.... \r\n");
		delay_ms(3000);
		
		/*清除 WU 状态位*/
		PWR_ClearFlag (PWR_FLAG_WU);
		/* 使能 WKUP 引脚的唤醒功能 ，使能 PA0*/
		PWR_WakeUpPinCmd(ENABLE);
		/* 进入待机模式 */
		PWR_EnterSTANDBYMode();
		
		
		printf("process 2222.... \r\n");
		delay_ms(3000);
	}
	return 0;
	
}



void key_gpio_interrupt_init(void) //PB8 IO口外部输入中断，唤醒__WFI
{
	GPIO_InitTypeDef GPIO_initStructure;
	EXTI_InitTypeDef EXTI_initStructure;
	NVIC_InitTypeDef NVIC_initStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_initStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
		
	EXTI_ClearITPendingBit(EXTI_Line8);
	
	EXTI_initStructure.EXTI_Line = EXTI_Line8;
	EXTI_initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_initStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_initStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_initStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_initStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_initStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_Init(&NVIC_initStructure);
	
}


void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearFlag(EXTI_Line8);
	EXTI_ClearITPendingBit(EXTI_Line8);
}





