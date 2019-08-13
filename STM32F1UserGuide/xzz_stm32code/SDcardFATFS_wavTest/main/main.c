#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"

#include <malloc.h>

FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量
extern SD_CardInfo SDCardInfo;

int main(void)
{
	FIL fp;//读写文件要创建文件句柄
	
	FRESULT ret; //mount返回值，返回(0)FR_OK表示挂载成功
	uint32_t bytenum = 0,i = 0;
	
	__WaveHeader *wavhead=0;
	u32 wavsize = 0;//WAV文件数据大小，但是不包含wav头数据大小
	
	uint8_t buff[12] = {0xff,1,2,3,4,5,6,7,8,9,10,11};//产生的音频数据
	
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
	
		
	wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader));//创建wav头数据
	recoder_wav_init(wavhead);//给wav头写头数据
	
	ret = f_open(&fp,"0:recorder.wav",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//打开创建wav文件
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//文件打开成功
		printf("wav open file success\r\n");//wav文件创建成功
	else
		printf("wav open file failed\r\n");
		
	ret = f_write(&fp,(const void*)wavhead,sizeof(__WaveHeader),&bytenum);//将wav头写入wav文件
	if(ret == FR_OK)
		printf("wav write RIFF success\r\n");
	else
		printf("wav write RIFF failed\r\n");
	
	
	
	ret = f_write(&fp,(const void*)buff,sizeof(buff),&bytenum);//将数据头写入wav文件
	if(ret == FR_OK)
		printf("wav write data success\r\n");
	else
		printf("wav write data failed\r\n");
		
	wavsize=sizeof(buff);//计算音频数据长度
	
	wavhead->riff.ChunkSize=wavsize+36;	//将WAV头和后面的音频数据加起来的长度
	wavhead->data.ChunkSize=wavsize;//将计算的音频数据长度写在wav头的数据长度变量里
	
	f_lseek(&fp,0);//偏移到wav文件头,把计算的wav文件数据长度覆盖进前面写入文件的wav头数据
	ret = f_write(&fp,(const void*)wavhead,sizeof(__WaveHeader),&bytenum);//将数据计算结果写入wav文件
	if(ret == FR_OK)
		printf("wav write data size success\r\n");
	else
		printf("wav write data size failed\r\n");
	f_close(&fp);
	
	//while(1);	
	
	return 0;	
}







