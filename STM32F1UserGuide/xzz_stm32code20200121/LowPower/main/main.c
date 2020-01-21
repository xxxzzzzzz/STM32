#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"


void key_gpio_interrupt_init(void); //PB8 IO���ⲿ�����жϣ�����__WFI

int main(void)
{
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);/* ʹ�ܵ�Դ����Ԫ��ʱ��,����Ҫʹ��ʱ�Ӳ��ܽ������ģʽ */
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	key_gpio_interrupt_init(); //PB8 IO���ⲿ�����жϣ�����__WFI
	delay_ms(5000);
	
	while(1)
	{
		printf("process 1111.... \r\n");
		delay_ms(3000);
		
		/*��� WU ״̬λ*/
		PWR_ClearFlag (PWR_FLAG_WU);
		/* ʹ�� WKUP ���ŵĻ��ѹ��� ��ʹ�� PA0*/
		PWR_WakeUpPinCmd(ENABLE);
		/* �������ģʽ */
		PWR_EnterSTANDBYMode();
		
		
		printf("process 2222.... \r\n");
		delay_ms(3000);
	}
	return 0;
	
}



void key_gpio_interrupt_init(void) //PB8 IO���ⲿ�����жϣ�����__WFI
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





