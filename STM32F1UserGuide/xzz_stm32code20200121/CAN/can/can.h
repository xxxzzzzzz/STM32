#ifndef __CAN_H
#define __CAN_H
#include "stm32f10x.h"
void CAN_init(void);//CAN引脚初始化
void CAN_Config(void);/*CAN波特率和功能设置*/
void CAN_Filter(uint32_t ID);/*CAN过滤器配置*/


void CAN_NVIC_Config(void);/*CAN接受中断配置*/
#endif

