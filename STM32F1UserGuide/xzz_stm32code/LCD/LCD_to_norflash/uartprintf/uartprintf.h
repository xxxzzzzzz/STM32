#ifndef __UARTPRINTF_H
#define __UARTPRINTF_H
#include "stm32f10x.h"

void NVIC_Configuration(void);//�ж����ȼ�����
void USART_config(uint32_t baudrate);//���ڳ�ʼ��
void USART1_Send(uint8_t ch);//���ڷ����ֽ�

#endif

