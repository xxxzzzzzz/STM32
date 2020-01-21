#include "wavfmt.h"
#include "ff.h"	

//��ʼ��wavͷ
void recoder_wav_init(__WaveHeader* wavhead) //��ʼ��WAVͷ			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ����Ҫ�����㣬����0
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16�ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾ����PCM
 	wavhead->fmt.NumOfChannels=1;		//1��ʾ������
 	wavhead->fmt.SampleRate=8000;		//8Khz ������
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//16λ����2�ֽ�
 	wavhead->fmt.BlockAlign=2;			//���С:2�ֽ�Ϊ1��
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С������㣬��д0 
} 


