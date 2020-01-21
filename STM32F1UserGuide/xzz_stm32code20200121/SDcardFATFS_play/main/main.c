#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"
#include "wm8978.h"
#include "i2s.h" 

FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���
//extern SD_CardInfo SDCardInfo;

#define WAVEFILEBUFSIZE		1024
FIL fp;
uint8_t WaveFileBuf[WAVEFILEBUFSIZE];//2048�ڴ��Ϊ1024
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
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	I2S3_Init();//MCU��I2S3��ʼ��
	
	res = WM8978_Init();//����ǰȷ��WM8978��ƵоƬI2S��ʼ���ɹ�
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
	
	
	//result=f_open(&fp,"0:DBFS.wav", FA_WRITE|FA_READ);//��wav�ļ���
	result=f_open(&fp,"0:baobei.wav", FA_WRITE|FA_READ);//��wav�ļ���
	printf("f_open = %d\r\n",result);
	if(result == FR_OK)
	{
		printf("file open success\r\n");
		
		//result = f_stat("0:DBFS.wav",&info);//����WAV�ļ���С�͵���WAV�ļ���С�Ƿ�һ��
		result = f_stat("0:baobei.wav",&info);//����WAV�ļ���С�͵���WAV�ļ���С�Ƿ�һ��
		printf("file size = %ld\r\n",info.fsize);
		
		f_read(&fp,TempBuf,1024,&br);//��ȡWAVͷ1024�ֽ�
		riff=(ChunkRIFF *)TempBuf;//��ͷ1024�ֽ�ת��riff���������Բ鿴riff��Ϣ�Ƿ���ȷ
		
		printf("riff Format = %x\r\n",riff->Format);//wav�ļ�0x45564157
		if(riff->Format==0X45564157)	//��WAV�ļ�
		{
			fmt=(ChunkFMT *)(TempBuf+48);//��ȡFMT�飬�����ͷ��ʼƫ��48�ֽ�
			printf("fmt chunkID = %x\r\n",fmt->ChunkID);
			printf("fmt ChunkSize = %x\r\n",fmt->ChunkSize);
			printf("fmt AudioFormat = %x\r\n",fmt->AudioFormat);
			printf("fmt NumOfChannels = %x\r\n",fmt->NumOfChannels);
			printf("fmt SampleRate = %x\r\n",fmt->SampleRate);
			printf("fmt ByteRate = %x\r\n",fmt->ByteRate);
			printf("fmt BlockAlign = %x\r\n",fmt->BlockAlign);
			printf("fmt BitsPerSample = %x\r\n",fmt->BitsPerSample);			
		}
		
		
		data=(ChunkDATA *)(TempBuf+128);//��riff���ֽ�ƫ��72�ֽھ���data->ChunkID
		printf("data ChunkID = %x\r\n",data->ChunkID);
		printf("sound num = %x\r\n",data->ChunkSize);//data�����ChunkSize�Ǻ������Ƶ���ݴ�С
		
		//f_lseek(&fp, 80);//��ͷ��ʼƫ��80�ֽڣ����������Ƶ�����ֽ���
		f_lseek(&fp, 144);//��ͷ��ʼƫ��136�ֽڣ����������Ƶ�����ֽ���
		//TIM3_init();//��ʼ��TIM3
		x = 0;
		while(x<data->ChunkSize)//����Ƶ���ݴ�С��ȷ���ܲ��Ŷ೤ʱ�䣬ʱ�䵽�˳�whileѭ��
		{
			i = 0;
			f_read(&fp,WaveFileBuf,1024,&br);//ÿ��ѭ����ȡ2048�ֽ���Ƶ���ݽ��в���
			
			while(i < sizeof(WaveFileBuf))
			{
				if(Globalefunc == 1)
				{
						Globalefunc = 0;
					
						Ldata = WaveFileBuf[i];//��Ƶ���ֽ�
						Hdata = WaveFileBuf[++i];//��Ƶ���ֽ�,������++i�������ֱ��i++�����γ�����ȥi���ݣ�Ȼ����i++������
						LeftHLdata = (uint16_t)(Hdata<<8|Ldata);//��λ��ǰ����λ�ں�
					
						i++;		
						
						Ldata = WaveFileBuf[i];//��Ƶ���ֽ�
						Hdata = WaveFileBuf[++i];//��Ƶ���ֽ�,������++i�������ֱ��i++�����γ�����ȥi���ݣ�Ȼ����i++������
						RightHLdata = (uint16_t)(Hdata<<8|Ldata);//��λ��ǰ����λ�ں�
						i++;
				}
						GlobaleInc = 1;		
		
			}
			x=x+1024;
			//printf("x = %d\r\n",x);//���Դ�ӡ���ݿ���������ɺ�Ĵ�С�ǲ��ǽӽ�wav�ļ���С
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //����TIM3ʱ��
	
	TIM_DeInit(TIM3);
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseStructure.TIM_Period = 20;//20us����������һ���ж�
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//������1us��һ��1��72M/72=1M
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
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)//���TIM3�ж��Ƿ���
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//���TIM3�жϱ�־λ
		if(GlobaleInc == 1)
		{
			
			SPI_I2S_SendData(SPI3, RightHLdata);//����16λ��������Ƶ���ݷ���I2S����
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);//�ȴ�I2S������Ƶ���ݷ�����ɣ����ܷ�����2��
			SPI_I2S_SendData(SPI3, LeftHLdata);//����16λ��������Ƶ���ݷ���I2S����
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);//�ȴ���2��I2S��Ƶ���ݷ������
			
			GlobaleInc = 0;
			Globalefunc = 1;
		}
							
	}
}












