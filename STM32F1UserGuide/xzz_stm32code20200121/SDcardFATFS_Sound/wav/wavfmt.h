#ifndef __WAVFMT_H
#define __WAVFMT_H
#include "stm32f10x.h"
//RIFF��
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"RIFF",����0x46464952
    u32 ChunkSize ;		   	//�ļ��ܴ�С��8
    u32 Format;	   			//��ʽWAVE,����0x45564157
}ChunkRIFF ;

//fmt��
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"fmt ",����0X20746D66
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��size),����Ϊ20
    u16 AudioFormat;	  	//��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
	u16 NumOfChannels;		//ͨ������;1,��ʾ������;2,˫����;
	u32 SampleRate;			//������;0X1F40,��ʾ8Khz
	u32 ByteRate;			//�ֽ�����; 
	u16 BlockAlign;			//�����(�ֽ�); 
	u16 BitsPerSample;		//�����������ݴ�С;4λADPCM,�����ó�4

}ChunkFMT;	   

//fact��
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"fact",����0X74636166;
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��size),����Ϊ4
    u32 NumOfSamples;	  	//����ת��ΪPCM��ʽ��Ĵ�С
}ChunkFACT;

//LIST? 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;?????"LIST",?0X74636166;
    u32 ChunkSize ;		   	//?????(???ID?Size);???:4. 
}ChunkLIST;

//data�� 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"data",����0X61746164
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��size),����Ϊ�ļ���С-60 
}ChunkDATA;

//wav?
typedef __packed struct
{ 
	ChunkRIFF riff;	//riff?
	ChunkFMT fmt;  	//fmt?
//	ChunkFACT fact;	//fact? ??PCM,???????	 
	ChunkDATA data;	//data?		 
}__WaveHeader; 

//wav ???????
typedef __packed struct
{ 
    u16 audioformat;			//????;0X01,????PCM;0X11??IMA ADPCM
	u16 nchannels;				//????;1,?????;2,?????; 
	u16 blockalign;				//???(??);  
	u32 datasize;				//WAV???? 

    u32 totsec ;				//?????,??:?
    u32 cursec ;				//??????
	
    u32 bitrate;	   			//???(??)
	u32 samplerate;				//??? 
	u16 bps;					//??,??16bit,24bit,32bit
	
	u32 datastart;				//????????(????????)
}__wavctrl; 

void recoder_wav_init(__WaveHeader* wavhead); //��ʼ��WAVͷ	


#endif

