#include "i2s.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//I2S 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20140606
//新增I2S2ext_Init、I2S2ext_RX_DMA_Init、I2S_Rec_Start和I2S_Rec_Stop等函数
////////////////////////////////////////////////////////////////////////////////// 	
 

//I2S2初始化
//参数I2S_Standard:  @ref SPI_I2S_Standard  I2S标准,
//I2S_Standard_Phillips,飞利浦标准;
//I2S_Standard_MSB,MSB对齐标准(右对齐);
//I2S_Standard_LSB,LSB对齐标准(左对齐);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM标准
//参数I2S_Mode: @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:从机发送;I2S_Mode_SlaveRx:从机接收;I2S_Mode_MasterTx:主机发送;I2S_Mode_MasterRx:主机接收;
//参数I2S_Clock_Polarity  @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,时钟低电平有效;I2S_CPOL_High,时钟高电平有效
//参数I2S_DataFormat： @ref SPI_I2S_Data_Format :数据长度,I2S_DataFormat_16b,16位标准;I2S_DataFormat_16bextended,16位扩展(frame=32bit);I2S_DataFormat_24b,24位;I2S_DataFormat_32b,32位.
void I2S3_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2S_InitTypeDef I2S_InitStructure;
	
	/* Enable GPIOB, GPIOC and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|  RCC_APB2Periph_GPIOC | 
							RCC_APB2Periph_AFIO , ENABLE);
	
	/* I2S2 SD, CK pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* I2S2 WS pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟,I2S2时钟在SPI2
	
	SPI_I2S_DeInit(SPI2);
	
	I2S_InitStructure.I2S_Mode=I2S_Mode_MasterRx;//IIS从机发送主机接收
	I2S_InitStructure.I2S_Standard=I2S_Standard_Phillips;//IIS标准
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat_16b;//IIS数据长度
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//主时钟输出关闭
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_48k;//IIS频率设置(采样率48K)
	I2S_InitStructure.I2S_CPOL=I2S_CPOL_Low;//空闲状态时钟电平
	I2S_Init(SPI2,&I2S_InitStructure);//初始化IIS

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);//首先关闭I2S接受数据中断
	//SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA请求使能.
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2S2 EN使能.	
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);//I2S2接受中断使能
	
	
} 
 


/*I2S3中断优先级配置*/
void I2S3_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* SPI3 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}












