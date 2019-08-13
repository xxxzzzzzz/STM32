#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"
#include "wm8978.h"
#include "i2s.h"
#include "wmxtest.h"


extern SD_CardInfo SDCardInfo;


uint16_t I2S3_Buffer_Rx[32];
FRESULT ret; //mount返回值，返回(0)FR_OK表示挂载成功

uint32_t bytenum = 0;

__WaveHeader wavhead;
u32 wavsize = 0;//WAV文件数据大小，但是不包含wav头数据大小

int main(void)
{
	FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量
	FIL fp;//读写文件要创建文件句柄
		
	
	//RCC_configuration();//初始化时钟
	RCC_Configuration_I2S();//I2S主时钟配置测试
	
	USART_config(115200);//初始化串口
	//recoder_mode();//打开录音
	I2S3_NVIC_Configuration();//I2S2中断优先级设置
	delay_ms(5000);
	
	I2S3_Init();//打开数字话筒录音
	printf("xxxzzzzzz\r\n");
	
	
	
	ret = f_mount(&fsp,"0:",1);//挂载文件系统,设置节点2将SD卡挂载进文件系统
	printf("res = %d\r\n",ret);
	if(ret == FR_NO_FILESYSTEM)//如果SD卡没有格式化，那就格式化
	{
		f_mkfs("0:",0,0);
		ret = f_mount(NULL,"0:",1);//格式化完成后要取消挂载
		ret = f_mount(&fsp,"0:",1);//然后再重新挂载才能使用
		printf("res2 = %d\r\n",ret);
	}
	
		
	//wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader));//创建wav头数据
	recoder_wav_init(&wavhead);//给wav头写头数据
	
	
	
	
	ret = f_open(&fp,"0:recorder.wav",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//打开创建wav文件
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//文件打开成功
		printf("wav open file success\r\n");//wav文件创建成功
	else
		printf("wav open file failed\r\n");
		
	ret = f_write(&fp,(const void*)&wavhead,sizeof(__WaveHeader),&bytenum);//将wav头写入wav文件
	if(ret == FR_OK)
		printf("wav write RIFF success\r\n");
	else
		printf("wav write RIFF failed\r\n");
	
	
	
//	while(1)
//	{
		
		delay_ms(5000);
		wavhead.data.ChunkSize=wavsize;//将计算的音频数据长度写在wav头的数据长度变量里
		wavhead.riff.ChunkSize=wavsize+36;	//将WAV头和后面的音频数据加起来的长度
		//f_lseek(&fp,0);//偏移到wav文件头,把计算的wav文件数据长度覆盖进前面写入文件的wav头数据
		//ret = f_write(&fp,(const void*)&wavhead,sizeof(__WaveHeader),&bytenum);//将数据计算结果写入wav文件
		if(ret == FR_OK)
		{
			//printf("wav write data size success\r\n");
		}
		else
			printf("wav write data size failed\r\n");
		
		printf("wave write end\r\n");
		
		f_close(&fp);		
//	}
	
	
	
	return 0;	
}





__IO uint32_t RxIdx = 0;

void SPI2_IRQHandler(void)
{
	
	//printf("enter i2s3 irq\r\n");
  if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET)
  {
	 SPI_I2S_ClearITPendingBit(SPI2,SPI_I2S_IT_RXNE );//清除中断标志位
    /* Store the I2S2 received data in the relative data table */
    I2S3_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
	if(RxIdx == 31)
	{
		//ret = f_write(&fp,(const void*)I2S3_Buffer_Rx,sizeof(I2S3_Buffer_Rx),&bytenum);//将数据头写入wav文件
		if(ret == FR_OK)
		{
			//printf("wav write data success\r\n");
		}
		else
			printf("wav write data failed\r\n");
		wavsize+=sizeof(I2S3_Buffer_Rx);//计算音频数据长度
		RxIdx = 0;
	}		
			
  }
  
}











