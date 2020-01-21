#include "stm32f10x.h"
#include "spi_flash.h"


void spi_FUN_init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//����SPI1��APB2�����ϵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//��ΪCS MISO MOSI SCK����GPIO_A���ţ�
	//���Գ�ʼ��GPIOAʱ�Ӿ����� 
	
	//��ʼ��SPI CSƬѡ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//��ͨGPIO��Ƭѡ�������ó��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��ʼ��SPI SCKʱ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//���츴�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��ʼ��SPI MISO��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��ʼ��SPI MOSI�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//���츴�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//Ϊ�˰�ȫ������Ƚ�CS�������óɸߵ�ƽ
	
	/*��ʼ��SPI1�Ĺ���ģʽ*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//��׼��SPI���䷽ʽ
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//STM32������������ѡ��SPI����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//SPIÿ�η���һ���ֽ�
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//CSӲ�����ó�������ƣ����������ϱߵ�GPIO����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//����flash�ֲ�ѡ���λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);//�ǵ�Ҫ��SPI1
}

/*STM32����һ���ֽڸ�����*/
void SPI_send_Byte(uint8_t data)
{
	//��������ǰ��ȷ��STM32�ڲ���TXE�Ĵ���Ϊ�գ����ܷ�������
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);//��������
	/*���ǲ�֪������ֽ��Ƿ�����ɣ�������ΪSPI�����ݵ�ͬʱҲ�ڽ�������
	* ��������RXNE��־���ж��Ƿ������
	*/
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//û�з�����ѭ���ȴ�
	
	SPI_I2S_ReceiveData(SPI1);
	//����û����Ҫ���ܵ����ݣ�Ҳ����д���ܺ����������������ݲ��Ե�����
}	


/*STM32����һ�����������*/
uint8_t SPI_read_Byte(void)
{
	uint8_t data;
	/*��ΪSPI��ѭ��ͬ���ģ�����SPI�������ݱ��������������ݵĺ�������ʱ��
	����SPI���ܽ������ݣ���˼����SPI����һ���ֽڵ�ʱ�ӣ�ͬʱҲ�ڽ���һ���ֽ�*/
	
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0x00);//�����һ���ģ�ⷢ�����ݲ���ʱ��ʲô��������ν
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//û�з�����ѭ���ȴ�
	data = SPI_I2S_ReceiveData(SPI1);
	return data;
}




/*STM32����һ���ֽڸ�����ͬʱ����һ���ֽ�*/
uint8_t SPI_Send_Read_Byte(uint8_t data)
{
	//��������ǰ��ȷ��STM32�ڲ���TXE�Ĵ���Ϊ�գ����ܷ�������
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);//��������
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//û�з�����ѭ���ȴ�
	return SPI_I2S_ReceiveData(SPI1);
}	

