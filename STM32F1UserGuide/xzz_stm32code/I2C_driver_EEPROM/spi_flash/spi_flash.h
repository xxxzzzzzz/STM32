#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#include "stm32f10x.h"

void spi_FUN_init(void);//SPI�����ʼ��
void SPI_send_Byte(uint8_t data);/*STM32����һ���ֽڸ�����*/
uint8_t SPI_read_Byte(void);/*STM32����һ�����������*/

uint8_t SPI_Send_Read_Byte(uint8_t data);/*STM32����һ���ֽڸ�����ͬʱ����һ���ֽ�*/

#endif

