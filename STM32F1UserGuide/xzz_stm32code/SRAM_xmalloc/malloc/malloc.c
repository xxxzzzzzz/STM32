#include "stm32f10x.h"
#include "malloc.h"


//定义分配内存的参数
//#define MEM_BLOCK_SIZE			32  	  						//内存块大小32字节
//#define MEM_MAX_SIZE			42*1024  						//最大内存42K
//#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	//内存表大小

#define MEM2_BLOCK_SIZE			32  	  						//内存块大小32字节
#define MEM2_MAX_SIZE			960*1024  						//外部SRAM内存池申请960K，要留一部分给内存表
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//内存表大小


//外部SRAM内存池4字节对齐
//__align(4) u8 membase[MEM_MAX_SIZE];			//RAM内部内存存池
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0x68000000)));	//32字节对齐
//这是用attribute绝对定位，把数组定义到外部内存的首地址0x68000000，为什么这样定义?
//这是因为我们SRAM章节说了，使用0x68000000才能驱动FSMC去读取1M的外部SRAM

u16 memmapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0x68000000+MEM2_MAX_SIZE)));
//定义外部SRAM内存管理表大小



const u32 memtblsize=MEM2_ALLOC_TABLE_SIZE;		//外部内存表大小
const u32 memblksize=MEM2_BLOCK_SIZE;			//外部内存分块大小
const u32 memsize=MEM2_MAX_SIZE;				//外部内存总大小

//定义内存控制器结构
struct _m_mallco_dev
{
	void (*init)(void);				//接受初始化内存函数
	u8 (*perused)(void);		  	//接受内存使用率函数
	u8 	*membase;					//内存池(内部SRAM)
	u16 *memmap; 					//内存管理状态表
	u8  memrdy; 					//内存管理是否就绪
};

struct _m_mallco_dev mallco_dev;	//定义一个内存控制器变量


//给内存控制器赋默认值
struct _m_mallco_dev mallco_dev=
{
	mem_init,			//内存初始化函数mem_init放进来
	mem_perused,		//内存使用率函数放进来
	//membase,			//放SRAM内存池进结构体
	mem2base,			//放外部SRAM内存池进结构体
	memmapbase,			//放SRAM内存管理表进结构体
	0,  				//在没有执行内存初始化函数的时候,默认写0
};

/*设置内存*/
// *s 内存首地址
// c 给每个内存地址写的值
// count 需要清0的内存地址数(字节为单位)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	

/*内存管理初始化*/
void mem_init(void)  
{  
    mymemset(mallco_dev.memmap, 0,memtblsize*2);//内存状态表数据清0  
	mymemset(mallco_dev.membase, 0,memsize);	//内存池(SRAM)所以数据清0
	mallco_dev.memrdy=1;						//内存初始化函数执行,这里写1
}  


//内存分配实现
//size 要分配的内存大小(字节) 
//返回值0xFFFFFFFF表示内存申请出错。内存申请正确返回内存偏移地址值
u32 mem_malloc(u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	  
	u16 cmemb=0;
    u32 i;  
    if(!mallco_dev.memrdy)mallco_dev.init();	
    if(size==0)return 0XFFFFFFFF;				
    nmemb=size/memblksize;  					
    if(size%memblksize)nmemb++;  
    for(offset=memtblsize-1;offset>=0;offset--)	 
    {     
		if(!mallco_dev.memmap[offset])cmemb++;	
		else cmemb=0;							
		if(cmemb==nmemb)						
		{
            for(i=0;i<nmemb;i++)  				 
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*memblksize);			  
		}
    }  
    return 0XFFFFFFFF; 
} 
/*动态分配内存*/
// size 分配内存大小(字节)
//返回值:分配到内存的首地址
void *mymalloc(u32 size)  
{  
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)return 0; //如果mem_malloc返回0xFFFFFFFF表示内存申请失败，返回0 
    else return (void*)((u32)mallco_dev.membase+offset);//内存申请成功返回申请成功内存的首地址  
}  




//获取内存使用率
u8 mem_perused(void)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize;i++)  
    {  
        if(mallco_dev.memmap[i])used++; 
    } 
    return (used*100)/(memtblsize);  
}  

//释放内存
//offset要释放内存池里面某块内存的偏移地址
//返回0表示释放内存成功，返回1表示释放内存失败
u8 mem_free(u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)
	{
		mallco_dev.init();    
        return 1;
    }  
    if(offset<memsize) 
    {  
        int index=offset/memblksize;		  
        int nmemb=mallco_dev.memmap[index];	
        for(i=0;i<nmemb;i++)  				
        {  
            mallco_dev.memmap[index+i]=0;  
        }
        return 0;  
    }else return 2;
} 

//释放内存函数
//ptr，要释放内存的首地址
void myfree(void *ptr)  
{  
	u32 offset;  
    if(ptr==0)return;  
 	offset=(u32)ptr-(u32)mallco_dev.membase;  
    mem_free(offset);	    
}  


/*复制内存*/
// *des 目的地址
// *src 源地址
// n 需要复制的内容长度

void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  


/*重新分配内存*/
// *ptr 旧内存首地址
// size 要分配的内存大小
//返回值:新申请到的内存首地址

void *myrealloc(void *ptr,u32 size)  
{  
    u32 offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return 0;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase+offset),ptr,size);	   
        myfree(ptr);  											  	
        return (void*)((u32)mallco_dev.membase+offset);  			
    }  
}



