#include "stm32f10x.h"
#include "spi_flash.h"


void spi_FUN_init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//启动SPI1在APB2总线上的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//因为CS MISO MOSI SCK都在GPIO_A引脚，
	//所以初始化GPIOA时钟就是了 
	
	//初始化SPI CS片选引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通GPIO做片选所以设置成推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化SPI SCK时钟引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化SPI MISO输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化SPI MOSI输出引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//为了安全起见，先将CS引脚设置成高电平
	
	/*初始化SPI1的工作模式*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//标准的SPI传输方式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//STM32是主机，所以选择SPI主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//SPI每次发送一个字节
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//CS硬件设置成软件控制，就是我们上边的GPIO控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//根据flash手册选择高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);//记得要打开SPI1
}

/*STM32发送一个字节给外设*/
void SPI_send_Byte(uint8_t data)
{
	//发送数据前先确保STM32内部的TXE寄存器为空，才能发送数据
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);//发送数据
	/*我们不知道这个字节是否发送完成，但是因为SPI发数据的同时也在接受数据
	* 所以我用RXNE标志来判断是否发送完成
	*/
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//没有发送完循环等待
	
	SPI_I2S_ReceiveData(SPI1);
	//这里没有需要接受的数据，也必须写接受函数，否则会出现数据不对的问题
}	


/*STM32接受一个外设的数据*/
uint8_t SPI_read_Byte(void)
{
	uint8_t data;
	/*因为SPI是循环同步的，所以SPI接受数据必须依靠发送数据的函数产生时钟
	这样SPI才能接受数据，意思就是SPI发送一个字节的时钟，同时也在接受一个字节*/
	
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0x00);//所以我还得模拟发送数据产生时钟什么数据无所谓
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//没有发送完循环等待
	data = SPI_I2S_ReceiveData(SPI1);
	return data;
}




/*STM32发送一个字节给外设同时接受一个字节*/
uint8_t SPI_Send_Read_Byte(uint8_t data)
{
	//发送数据前先确保STM32内部的TXE寄存器为空，才能发送数据
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);//发送数据
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//没有发送完循环等待
	return SPI_I2S_ReceiveData(SPI1);
}	

