#include "stm32f10x_dma.h"
#include "dma.h"
#include "uartprintf.h"
#include "sysclock.h"

const uint8_t usart_buffer[32] = "abcdefg123456"; //加入const 这段数组存放在flash

uint32_t readbuff[32];//没有const的全局数组是放在RAM里面

//void DMA1_init(void)
//{
//	DMA_InitTypeDef DMA_InitStruct;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//开启AHB总线DMA1时钟
//	
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)flash_buffer;//传入flash数组的地址
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)readbuff;//这是要接受数据的地址
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_BufferSize = 32;//我们一次传输32个数据，因为数组是32�
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//Word表示1个数据32位
//	
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//	
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//flash数组数据只向RAM数组发送一次32个
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //我们只使用了一个DMA通道，我就选最高优先级
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;//因为是flash数组发到RAM数组，所以这里要打开内存到内存模式
//	DMA_Init(DMA1_Channel6, &DMA_InitStruct);//选择DMA1 通道 6
//	
//	DMA_Cmd(DMA1_Channel6, ENABLE);//打开DMA1 通道16
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



/*DMA串口实验*/
void DMA1_UART_init(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//开启AHB总线DMA1时钟
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(USART1_BASE+0x04);//这是串口外设基地址+偏移地址(串口数据寄存器地址)
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)usart_buffer;//这是发送给串口数据的数组
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//内存发送数据到串口外设
	DMA_InitStruct.DMA_BufferSize = 32;//我们一次传输32个数据，因为数组是32�
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//因为串口只有一个数据寄存器，所以不需要内存递增
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//Byte表示1个数据8位
	
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存递增要打开，因为数组是递增的
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//这里的内存数据定义的是8位
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //我们只使用了一个DMA通道，我就选最高优先级
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//因为是flash数组发到串口，所以这里要关闭内存到内存模式
	DMA_Init(DMA1_Channel4, &DMA_InitStruct);//根据数据手册串口1 选择DMA1 通道 4
	
	DMA_ClearFlag(DMA1_FLAG_TC4);//清除发送完成标志位
	
	DMA_Cmd(DMA1_Channel4, ENABLE);//打开DMA1 通道4
	
}

