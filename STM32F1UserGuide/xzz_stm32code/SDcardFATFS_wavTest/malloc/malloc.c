#include "stm32f10x.h"
#include "malloc.h"


//��������ڴ�Ĳ���
#define MEM_BLOCK_SIZE			32  	  						//�ڴ���С32�ֽ�
#define MEM_MAX_SIZE			512  						//����ڴ�512
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	//�ڴ���С



__align(4) u8 membase[MEM_MAX_SIZE];			//RAM�ڲ��ڴ���


u16 memmapbase[MEM_ALLOC_TABLE_SIZE];//SRAM�ڴ������С

const u32 memtblsize=MEM_ALLOC_TABLE_SIZE;		//�ⲿ�ڴ���С
const u32 memblksize=MEM_BLOCK_SIZE;			//�ⲿ�ڴ�ֿ��С
const u32 memsize=MEM_MAX_SIZE;				//�ⲿ�ڴ��ܴ�С

//�����ڴ�������ṹ
struct _m_mallco_dev
{
	void (*init)(void);				//���ܳ�ʼ���ڴ溯��
	u8 (*perused)(void);		  	//�����ڴ�ʹ���ʺ���
	u8 	*membase;					//�ڴ��(�ڲ�SRAM)
	u16 *memmap; 					//�ڴ����״̬��
	u8  memrdy; 					//�ڴ�����Ƿ����
};

struct _m_mallco_dev mallco_dev;	//����һ���ڴ����������


//���ڴ��������Ĭ��ֵ
struct _m_mallco_dev mallco_dev=
{
	mem_init,			//�ڴ��ʼ������mem_init�Ž���
	mem_perused,		//�ڴ�ʹ���ʺ����Ž���
	membase,			//��SRAM�ڴ�ؽ��ṹ��
	memmapbase,			//��SRAM�ڴ�������ṹ��
	0,  				//��û��ִ���ڴ��ʼ��������ʱ��,Ĭ��д0
};

/*�����ڴ�*/
// *s �ڴ��׵�ַ
// c ��ÿ���ڴ��ַд��ֵ
// count ��Ҫ��0���ڴ��ַ��(�ֽ�Ϊ��λ)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	

/*�ڴ�����ʼ��*/
void mem_init(void)  
{  
    mymemset(mallco_dev.memmap, 0,memtblsize*2);//�ڴ�״̬��������0  
	mymemset(mallco_dev.membase, 0,memsize);	//�ڴ��(SRAM)����������0
	mallco_dev.memrdy=1;						//�ڴ��ʼ������ִ��,����д1
}  


//�ڴ����ʵ��
//size Ҫ������ڴ��С(�ֽ�) 
//����ֵ0xFFFFFFFF��ʾ�ڴ���������ڴ�������ȷ�����ڴ�ƫ�Ƶ�ֵַ
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
/*��̬�����ڴ�*/
// size �����ڴ��С(�ֽ�)
//����ֵ:���䵽�ڴ���׵�ַ
void *mymalloc(u32 size)  
{  
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)return 0; //���mem_malloc����0xFFFFFFFF��ʾ�ڴ�����ʧ�ܣ�����0 
    else return (void*)((u32)mallco_dev.membase+offset);//�ڴ�����ɹ���������ɹ��ڴ���׵�ַ  
}  




//��ȡ�ڴ�ʹ����
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

//�ͷ��ڴ�
//offsetҪ�ͷ��ڴ������ĳ���ڴ��ƫ�Ƶ�ַ
//����0��ʾ�ͷ��ڴ�ɹ�������1��ʾ�ͷ��ڴ�ʧ��
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

//�ͷ��ڴ溯��
//ptr��Ҫ�ͷ��ڴ���׵�ַ
void myfree(void *ptr)  
{  
	u32 offset;  
    if(ptr==0)return;  
 	offset=(u32)ptr-(u32)mallco_dev.membase;  
    mem_free(offset);	    
}  


/*�����ڴ�*/
// *des Ŀ�ĵ�ַ
// *src Դ��ַ
// n ��Ҫ���Ƶ����ݳ���

void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  


/*���·����ڴ�*/
// *ptr ���ڴ��׵�ַ
// size Ҫ������ڴ��С
//����ֵ:�����뵽���ڴ��׵�ַ

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



