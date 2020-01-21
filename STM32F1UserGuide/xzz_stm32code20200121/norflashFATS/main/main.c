#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "ff.h" //使用文件系统必须添加这个头文件


FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量
FIL fp;//读写文件要创建文件句柄

int main(void)
{
	FRESULT ret; //mount返回值，返回(0)FR_OK表示挂载成功
	uint32_t bytenum = 0;
	char INbuf[]="123456,abcdefg";//写入文件的数据
	char OUTbuf[30];
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	delay_ms(5000);
	
	ret = f_mount(&fsp,"1:",1);//挂载文件系统,将norflash挂载进文件系统
	printf("res = %d\r\n",ret);
	
	if(ret == FR_NO_FILESYSTEM)//如果norflash没有格式化，那就格式化
	{
		f_mkfs("1:",0,0);
		ret = f_mount(NULL,"1:",1);//格式化完成后要取消挂载
		ret = f_mount(&fsp,"1:",1);//然后再重新挂载才能使用
		printf("res2 = %d\r\n",ret);
	}
	
	
	ret = f_open(&fp,"1:文件系统文件创建.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//打开文件
	
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//文件打开成功
	{
		printf("open file success\r\n");//向文件写入数据
		ret = f_write(&fp,INbuf,sizeof(INbuf),&bytenum);
		if(ret == FR_OK)
		{
			printf("write file success bytenum = %d\r\n",bytenum);
		}
		else
		{
			printf("write file failed\r\n");
		}
		f_lseek(&fp,0);//文件光标从新移动到开头
		ret = f_read(&fp,OUTbuf,f_size(&fp),&bytenum);//读出文件的数据
		if(ret == FR_OK)
		{
			printf("read file success bytenum = %d\r\n",bytenum);
			printf("-----%s----\r\n",OUTbuf);
		}
		else
		{
			printf("read file failed\r\n");
		}
		
	}
	f_close(&fp);//这个一定不要忘了，如果不调用close可能导致文件不被保存
	
	while(1);
		

	return 0;
	
}












