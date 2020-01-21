#include "stm32f10x_dma.h"
#include "dma.h"
#include "uartprintf.h"
#include "sysclock.h"

const uint8_t usart_buffer[32] = "abcdefg123456"; //����const �����������flash

uint32_t readbuff[32];//û��const��ȫ�������Ƿ���RAM����

//void DMA1_init(void)
//{
//	DMA_InitTypeDef DMA_InitStruct;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//����AHB����DMA1ʱ��
//	
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)flash_buffer;//����flash����ĵ�ַ
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)readbuff;//����Ҫ�������ݵĵ�ַ
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_BufferSize = 32;//����һ�δ���32�����ݣ���Ϊ������32�
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//Word��ʾ1������32λ
//	
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//	
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//flash��������ֻ��RAM���鷢��һ��32��
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //����ֻʹ����һ��DMAͨ�����Ҿ�ѡ������ȼ�
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;//��Ϊ��flash���鷢��RAM���飬��������Ҫ���ڴ浽�ڴ�ģʽ
//	DMA_Init(DMA1_Channel6, &DMA_InitStruct);//ѡ��DMA1 ͨ�� 6
//	
//	DMA_Cmd(DMA1_Channel6, ENABLE);//��DMA1 ͨ��16
//}

void dma_print(void)
{
	int i = 0;
	for(i=0;i<32;i++)
	{
		printf(" %d ",readbuff[i]);
		delay_ms(100);
	}
}



/*DMA����ʵ��*/
void DMA1_UART_init(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//����AHB����DMA1ʱ��
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(USART1_BASE+0x04);//���Ǵ����������ַ+ƫ�Ƶ�ַ(�������ݼĴ�����ַ)
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)usart_buffer;//���Ƿ��͸��������ݵ�����
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//�ڴ淢�����ݵ���������
	DMA_InitStruct.DMA_BufferSize = 32;//����һ�δ���32�����ݣ���Ϊ������32�
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��Ϊ����ֻ��һ�����ݼĴ��������Բ���Ҫ�ڴ����
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//Byte��ʾ1������8λ
	
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ����Ҫ�򿪣���Ϊ�����ǵ�����
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//������ڴ����ݶ������8λ
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //����ֻʹ����һ��DMAͨ�����Ҿ�ѡ������ȼ�
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//��Ϊ��flash���鷢�����ڣ���������Ҫ�ر��ڴ浽�ڴ�ģʽ
	DMA_Init(DMA1_Channel4, &DMA_InitStruct);//���������ֲᴮ��1 ѡ��DMA1 ͨ�� 4
	
	DMA_ClearFlag(DMA1_FLAG_TC4);//���������ɱ�־λ
	
	DMA_Cmd(DMA1_Channel4, ENABLE);//��DMA1 ͨ��4
	
}

