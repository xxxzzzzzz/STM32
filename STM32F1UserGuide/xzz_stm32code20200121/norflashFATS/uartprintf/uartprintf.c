#include "stm32f10x.h"
#include "uartprintf.h"
#include "stdio.h"	//һ��Ҫ����stdio.h stm32�����Դ��ı�׼��
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 





void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//�����ж����ȼ��ṹ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж���ѡ��2��
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//���ô���USARTλ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�(�����ȼ�)Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�ж����ȼ�����
	NVIC_Init(&NVIC_InitStructure);//��ʼ���ж����ȼ�
	
}


void USART_config(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����GPIOΪ���ڹ���
	USART_InitTypeDef USART_InitStructure;//���ô��ڲ���
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//��APB2����GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//��APB2����UART1ʱ��
	
	//��UART1 TX�������Ŷ�Ӧ��GPIOA_9�ܽ����ó��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��UART1 RX��������GPIOA_10���óɸ�������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	NVIC_Configuration();//�����ж����ȼ����ã��Լ�����
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//���ô�������λΪ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//���ô���ֹͣλΪ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͽ��ܶ�����
	USART_Init(USART1, &USART_InitStructure);//���ڳ�ʼ������
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1
	
}




void USART1_Send(uint8_t ch)
{
//	USART_SendData(USART1, ch);//USART1���ڷ����ֽ�
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//����ȴ��������
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (uint8_t) ch;
}


void USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//���USART1�ǲ��Ƿ������ж�
	{
		ucTemp = USART_ReceiveData(USART1);//���ܴ���
		USART1_Send(ucTemp);//�ѽ��ܵĵ����ݷ��ظ�����
	}
}






