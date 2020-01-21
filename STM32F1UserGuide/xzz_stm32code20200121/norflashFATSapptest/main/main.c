#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "ff.h" //使用文件系统必须添加这个头文件
#include<string.h>

FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量



FRESULT scan_files(char* path)        /* Start node to be scanned (***also used as work area***) */
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    return res;
}


int main(void)
{
	FRESULT ret; //mount返回值，返回(0)FR_OK表示挂载成功
	char readbuff[100];//如果flash有很多文件，这个数组就要定义大点
	
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	delay_ms(5000);
	
	ret = f_mount(&fsp,"1:",1);//挂载文件系统,将norflash挂载进文件系统
	if(ret == FR_OK)
		printf("fatfs mount success \r\n");
	else
		printf("fatfs mount failed \r\n");
		
	if(ret == FR_NO_FILESYSTEM)//如果norflash没有格式化，那就格式化
	{
		f_mkfs("1:",0,0);
		ret = f_mount(NULL,"1:",1);//格式化完成后要取消挂载
		ret = f_mount(&fsp,"1:",1);//然后再重新挂载才能使用
		printf("formatting fatfs\r\n");
	}
	
	scan_files("1:");//传入磁盘编号，norflash我们设置的是1，搜索文件打印出来 
		  	
	while(1);
		

	return 0;
	
}












