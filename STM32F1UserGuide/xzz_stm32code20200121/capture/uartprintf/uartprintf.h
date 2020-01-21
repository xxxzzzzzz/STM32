#ifndef __UARTPRINTF_H
#define __UARTPRINTF_H
#include "stm32f10x.h"

void NVIC_Configuration(void);//中断优先级配置
void USART_config(uint32_t baudrate);//串口初始化
void USART1_Send(uint8_t ch);//串口发送字节

#endif

