#ifndef __WAVFMT_H
#define __WAVFMT_H
#include "stm32f10x.h"
//RIFF块
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;这里固定为"RIFF",就是0x46464952
    u32 ChunkSize ;		   	//文件总大小减8
    u32 Format;	   			//格式WAVE,就是0x45564157
}ChunkRIFF ;

//fmt块
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;这里固定为"fmt ",就是0X20746D66
    u32 ChunkSize ;		   	//子集合大小(不包括ID和size),这里为20
    u16 AudioFormat;	  	//音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
	u16 NumOfChannels;		//通道数量;1,表示单声道;2,双声道;
	u32 SampleRate;			//采用率;0X1F40,表示8Khz
	u32 ByteRate;			//字节速率; 
	u16 BlockAlign;			//块对齐(字节); 
	u16 BitsPerSample;		//单个采样数据大小;4位ADPCM,就设置成4

}ChunkFMT;	   

//fact块
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;这里固定为"fact",就是0X74636166;
    u32 ChunkSize ;		   	//子集合大小(不包括ID和size),这里为4
    u32 NumOfSamples;	  	//数据转换为PCM格式后的大小
}ChunkFACT;

//LIST? 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;?????"LIST",?0X74636166;
    u32 ChunkSize ;		   	//?????(???ID?Size);???:4. 
}ChunkLIST;

//data块 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;这里固定为"data",就是0X61746164
    u32 ChunkSize ;		   	//子集合大小(不包括ID和size),这里为文件大小-60 
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

void recoder_wav_init(__WaveHeader* wavhead); //初始化WAV头	


#endif

