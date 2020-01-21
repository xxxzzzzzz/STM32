#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#include "stm32f10x.h"

void spi_FUN_init(void);//SPI外设初始化
void SPI_send_Byte(uint8_t data);/*STM32发送一个字节给外设*/
uint8_t SPI_read_Byte(void);/*STM32接受一个外设的数据*/

uint8_t SPI_Send_Read_Byte(uint8_t data);/*STM32发送一个字节给外设同时接受一个字节*/

#endif

