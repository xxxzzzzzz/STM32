#include "wavfmt.h"
#include "ff.h"	

//初始化wav头
void recoder_wav_init(__WaveHeader* wavhead) //初始化WAV头			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定需要最后计算，先填0
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示线性PCM
 	wavhead->fmt.NumOfChannels=1;		//1表示单声道
 	wavhead->fmt.SampleRate=8000;		//8Khz 采样率
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//16位，既2字节
 	wavhead->fmt.BlockAlign=2;			//块大小:2字节为1块
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小还需计算，先写0 
} 


