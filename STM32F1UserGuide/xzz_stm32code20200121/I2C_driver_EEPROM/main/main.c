#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "stm32f10x_i2c.h"

/*I2C1���ų�ʼ������*/
void I2C_GPIO_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//I2C1��APB1���ߴ�����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//��GPIOB��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;/*GPIOB6��I2C1��SCL����*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //I2C���Ŷ��ǿ�©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;/*GPIOB7��I2C1��SDA����*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //I2C���Ŷ��ǿ�©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*I2C����ģʽ��ʼ��*/
void I2C_mode_config()
{
	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//I2CӦ��ACKʹ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//I2C����ֻ����7λ��ַ
	I2C_InitStructure.I2C_ClockSpeed = 400000;//I2Cͨ������400K
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//ѡ��2:1û������
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//��������ΪI2Cģʽ
	I2C_InitStructure.I2C_OwnAddress1 = 0;//STM32���ӻ��Լ�I2C��ַ
	I2C_Init(I2C1, &I2C_InitStructure);//���������õ�I2C������I2C1������
	
	I2C_Cmd(I2C1, ENABLE);//�������һ����Ҫ���Ǵ�I2C1����
}

/*д1���ֽڵ�EEPROMоƬ*/
/*
* pBuffer���͸�EEPROM����
* WriteAddrҪ������д��EEPROM������һ����ַ��
*/
void sendByteEEPROM(u8* pBuffer,u8 WriteAddr)
{
	I2C_GenerateSTART(I2C1, ENABLE);//������ʼ�ź�
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//���EV5�¼�
	
	I2C_Send7bitAddress(I2C1,0,I2C_Direction_Transmitter);//���ʹ�������ַ��������ַ��0x00
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//���EV6�¼�
	
	I2C_SendData(I2C1, WriteAddr);//��EEPROMд��ַ
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//���EV8�¼�
	
	I2C_SendData(I2C1, *pBuffer);//����Ҫд��EEPROM������
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//���EV8�¼�
	
	I2C_GenerateSTOP(I2C1, ENABLE);//����ֹͣ�ź�
	
}

/*��ȡEEPROM����*/
//��ȡ��eeprom���ݷ���pBuffer
//Ҫ��ȡeeprom�Ǹ���ַ�ϵ����ݣ���ReadAddrָ����ַ
void I2C_EEPROM_ByteRead(uint8_t *pBuffer, uint8_t ReadAddr)
{  
	//I2C_EE_WaitEepromStandbyState();//����EEPROM�Ļ����ﻹҪִ�еȴ�EEPROM׼���õĺ���
	//��������IIC����Ӧ�ò���Ҫ�ȴ�����
	
	I2C_GenerateSTART(I2C1, ENABLE);//������ʼ�ź�
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//���EV5
	
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Transmitter);//������ַ��0x00	
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//���EV6
	
	I2C_SendData(I2C1, ReadAddr);//Ҫ��ȡ�Ǹ���ַ�ϵ����ݣ����Ͷ�ȡ�ĵ�ַ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//���EV8
	
	I2C_GenerateSTART(I2C1, ENABLE);//���·���start�ź�
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//���EV5
	
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Receiver);//������ַ��0x00
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//���EV6
	
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); //���EV7
	
	*pBuffer = I2C_ReceiveData(I2C1);//��ȡ�ӻ���ַ�ϵ�����
	
	I2C_AcknowledgeConfig(I2C1, DISABLE);//STM32�������ͷ�Ӧ��(����Ҫ�ӻ�Ӧ��)�ź�    
	I2C_GenerateSTOP(I2C1, ENABLE);//����ֹͣ�ź�
}


int main(void)
{
	   	  	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	
	I2C_GPIO_INIT();//��ʼ��IIC�ܽ�
	I2C_mode_config();//��ʼIICģʽ
	
	//sendByteEEPROM(u8* pBuffer,u8 WriteAddr);//�����ݵ�EEPROM
	// I2C_EEPROM_ByteRead(uint8_t *pBuffer, uint8_t ReadAddr)��ȡEEPROM����
	while(1)
	{
		printf("xxxzzzzzz\r\n");
		
	}
	return 0;
	
}





