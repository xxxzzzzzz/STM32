#ifndef __CAN_H
#define __CAN_H
#include "stm32f10x.h"
void CAN_init(void);//CAN���ų�ʼ��
void CAN_Config(void);/*CAN�����ʺ͹�������*/
void CAN_Filter(uint32_t ID);/*CAN����������*/


void CAN_NVIC_Config(void);/*CAN�����ж�����*/
#endif

