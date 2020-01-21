#ifndef __SDCODE_H
#define __SDCODE_H
#include "uartprintf.h"
#include "stdio.h"
#include "stm32_eval_sdio_sd.h"

#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      32  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)



void SD_init(void);//SD卡初始化
void SD_EraseTest(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
void SDIO_NVIC_Configuration(void);//在firmware的C文件唯独没有初始化SDIO中断优先级，所以这里先初始化

#endif

