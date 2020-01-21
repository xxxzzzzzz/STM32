#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"

/*内存管理初始化*/
void mem_init(void);
//获取内存使用率
u8 mem_perused(void);


/*动态分配内存*/
// size 分配内存大小(字节)
//返回值:分配到内存的首地址
void *mymalloc(u32 size);

//释放内存函数
//ptr，要释放内存的首地址
void myfree(void *ptr); 

//获取内存使用率
u8 mem_perused(void);
#endif

