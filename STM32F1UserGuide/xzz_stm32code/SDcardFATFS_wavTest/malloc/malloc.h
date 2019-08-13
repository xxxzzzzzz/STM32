#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"

/*�ڴ�����ʼ��*/
void mem_init(void);
//��ȡ�ڴ�ʹ����
u8 mem_perused(void);


/*��̬�����ڴ�*/
// size �����ڴ��С(�ֽ�)
//����ֵ:���䵽�ڴ���׵�ַ
void *mymalloc(u32 size);

//�ͷ��ڴ溯��
//ptr��Ҫ�ͷ��ڴ���׵�ַ
void myfree(void *ptr); 

//��ȡ�ڴ�ʹ����
u8 mem_perused(void);
#endif

