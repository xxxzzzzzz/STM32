#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "ff.h"
#include "sdcode.h"

FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量
FIL fp;//读写文件要创建文件句柄

extern SD_CardInfo SDCardInfo;

int main(void)
{
	char readbuf[30];//读取文件数据的缓冲区
	FRESULT ret; //mount返回值，返回(0)FR_OK表示挂载成功
	uint32_t bytenum = 0;
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口

	delay_ms(5000);
	ret = f_mount(&fsp,"0:",1);//挂载文件系统,设置节点2将SD卡挂载进文件系统
	printf("res = %d\r\n",ret);
	
	if(ret == FR_NO_FILESYSTEM)//如果SD卡没有格式化，那就格式化
	{
		f_mkfs("0:",0,0);
		ret = f_mount(NULL,"0:",1);//格式化完成后要取消挂载
		ret = f_mount(&fsp,"0:",1);//然后再重新挂载才能使用
		printf("res2 = %d\r\n",ret);
	}
	printf("SD card sector size = %d\r\n",SDCardInfo.CardBlockSize);
	
	ret = f_open(&fp,"0:SD卡测试文件.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//打开文件
	
	printf("open ret = %d\r\n",ret);
//	if(ret == FR_OK)//文件打开成功
//	{
//		printf("open file success\r\n");//向文件写入数据
//		ret = f_write(&fp,INbuf,sizeof(INbuf),&bytenum);
//		if(ret == FR_OK)
//		{
//			printf("write file success bytenum = %d\r\n",bytenum);
//		}
//		else
//		{
//			printf("write file failed\r\n");
//		}
//	}
//	f_close(&fp);//这个一定不要忘了，如果不调用close可能导致文件不被保存
	
	if(ret == FR_OK)//文件打开成功
	{
		printf("open file success\r\n");
		f_lseek(&fp,0);
		ret = f_read(&fp,readbuf,f_size(&fp),&bytenum);//读取文件数据
		if(ret == FR_OK)
		{
			printf("read file success bytenum = %d\r\n",bytenum);
		}
		else
		{
			printf("read file failed\r\n");
		}
		printf("------------%s----------\r\n",readbuf);
	}
	f_close(&fp);//这个一定不要忘了，如果不调用close可能导致文件不被保存
	printf("file system test end\r\n");
	//while(1);	
	return 0;	
}







