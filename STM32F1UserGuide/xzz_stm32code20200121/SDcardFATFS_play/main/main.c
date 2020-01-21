#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"
#include "wm8978.h"
#include "i2s.h" 

FATFS fsp;//文件系统使用要先有这个句柄，这个句柄很大所以定义在全局变量
//extern SD_CardInfo SDCardInfo;

#define WAVEFILEBUFSIZE		1024
FIL fp;
uint8_t WaveFileBuf[WAVEFILEBUFSIZE];//2048内存改为1024
extern uint8_t dmaTxBuf[1024];


uint8_t GlobaleInc = 0;
uint8_t Globalefunc = 1;
uint16_t LeftHLdata = 0;
uint16_t RightHLdata = 0;

void TIM3_init(void);


int main(void)
{
	uint8_t TempBuf[1024];
	int result = 0,i = 0,x = 0;
	uint32_t br=0;
	uint8_t res=0;
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;
	uint8_t Hdata = 0;
	uint8_t Ldata = 0;
	uint16_t HLdata = 0;		
	FILINFO info;
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	I2S3_Init();//MCU端I2S3初始化
	
	res = WM8978_Init();//播放前确保WM8978音频芯片I2S初始化成功
	printf("wm8978 reset = %d\r\n",res);
	
	delay_ms(3000);
	
	result = f_mount(&fsp,"0:",1);
	if (result == FR_OK) {
		printf("SD card mount success!!!->%d\r\n",result);
	}
	else
	{
		printf("SD card mount failed!!!->%d\r\n",result);
	}
	
	
	//result=f_open(&fp,"0:DBFS.wav", FA_WRITE|FA_READ);//把wav文件打开
	result=f_open(&fp,"0:baobei.wav", FA_WRITE|FA_READ);//把wav文件打开
	printf("f_open = %d\r\n",result);
	if(result == FR_OK)
	{
		printf("file open success\r\n");
		
		//result = f_stat("0:DBFS.wav",&info);//看看WAV文件大小和电脑WAV文件大小是否一致
		result = f_stat("0:baobei.wav",&info);//看看WAV文件大小和电脑WAV文件大小是否一致
		printf("file size = %ld\r\n",info.fsize);
		
		f_read(&fp,TempBuf,1024,&br);//读取WAV头1024字节
		riff=(ChunkRIFF *)TempBuf;//将头1024字节转成riff，这样可以查看riff信息是否正确
		
		printf("riff Format = %x\r\n",riff->Format);//wav文件0x45564157
		if(riff->Format==0X45564157)	//是WAV文件
		{
			fmt=(ChunkFMT *)(TempBuf+48);//获取FMT块，这里从头开始偏移48字节
			printf("fmt chunkID = %x\r\n",fmt->ChunkID);
			printf("fmt ChunkSize = %x\r\n",fmt->ChunkSize);
			printf("fmt AudioFormat = %x\r\n",fmt->AudioFormat);
			printf("fmt NumOfChannels = %x\r\n",fmt->NumOfChannels);
			printf("fmt SampleRate = %x\r\n",fmt->SampleRate);
			printf("fmt ByteRate = %x\r\n",fmt->ByteRate);
			printf("fmt BlockAlign = %x\r\n",fmt->BlockAlign);
			printf("fmt BitsPerSample = %x\r\n",fmt->BitsPerSample);			
		}
		
		
		data=(ChunkDATA *)(TempBuf+128);//从riff首字节偏移72字节就是data->ChunkID
		printf("data ChunkID = %x\r\n",data->ChunkID);
		printf("sound num = %x\r\n",data->ChunkSize);//data里面的ChunkSize是后面的音频数据大小
		
		//f_lseek(&fp, 80);//从头开始偏移80字节，后面就是音频数据字节了
		f_lseek(&fp, 144);//从头开始偏移136字节，后面就是音频数据字节了
		//TIM3_init();//初始化TIM3
		x = 0;
		while(x<data->ChunkSize)//用音频数据大小来确定能播放多长时间，时间到退出while循环
		{
			i = 0;
			f_read(&fp,WaveFileBuf,1024,&br);//每次循环读取2048字节音频数据进行播放
			
			while(i < sizeof(WaveFileBuf))
			{
				if(Globalefunc == 1)
				{
						Globalefunc = 0;
					
						Ldata = WaveFileBuf[i];//音频低字节
						Hdata = WaveFileBuf[++i];//音频高字节,必须先++i，如果你直接i++，就形成了先去i数据，然后再i++有问题
						LeftHLdata = (uint16_t)(Hdata<<8|Ldata);//高位在前，低位在后
					
						i++;		
						
						Ldata = WaveFileBuf[i];//音频低字节
						Hdata = WaveFileBuf[++i];//音频高字节,必须先++i，如果你直接i++，就形成了先去i数据，然后再i++有问题
						RightHLdata = (uint16_t)(Hdata<<8|Ldata);//高位在前，低位在后
						i++;
				}
						GlobaleInc = 1;		
		
			}
			x=x+1024;
			//printf("x = %d\r\n",x);//可以打印数据看看播放完成后的大小是不是接近wav文件大小
		}
		
		
	}
	else
	{
		printf("file open failed\r\n");
		result = f_close (&fp);
		
	}
		
	f_close(&fp);
	
	
	
	
	printf("exec end ...\r\n");
//	result = f_read(&fp,&xzzwavhead,sizeof(xzzwavhead),&bw);
	
	while(1)
	{
		SPI_I2S_SendData(SPI3, 0x00ff);
	}
	
	return 0;	
}




void TIM3_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //开启TIM3时钟
	
	TIM_DeInit(TIM3);
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseStructure.TIM_Period = 20;//20us计数满产生一次中断
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//计数器1us加一次1，72M/72=1M
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_PrescalerConfig(TIM3,71,TIM_PSCReloadMode_Immediate);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		
}


void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)//检查TIM3中断是否发生
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清楚TIM3中断标志位
		if(GlobaleInc == 1)
		{
			
			SPI_I2S_SendData(SPI3, RightHLdata);//将高16位右声道音频数据放入I2S总线
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);//等待I2S总线音频数据发送完成，才能发生第2次
			SPI_I2S_SendData(SPI3, LeftHLdata);//将高16位左声道音频数据放入I2S总线
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);//等待第2次I2S音频数据发送完成
			
			GlobaleInc = 0;
			Globalefunc = 1;
		}
							
	}
}












