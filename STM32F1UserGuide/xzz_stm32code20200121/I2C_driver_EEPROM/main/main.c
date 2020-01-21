#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "stm32f10x_i2c.h"

/*I2C1引脚初始化函数*/
void I2C_GPIO_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//I2C1在APB1总线打开总线时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//打开GPIOB的时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;/*GPIOB6是I2C1的SCL引脚*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //I2C引脚都是开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;/*GPIOB7是I2C1的SDA引脚*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //I2C引脚都是开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*I2C工作模式初始化*/
void I2C_mode_config()
{
	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//I2C应答ACK使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//I2C主机只发生7位地址
	I2C_InitStructure.I2C_ClockSpeed = 400000;//I2C通信速率400K
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//选择2:1没有问题
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//主机设置为I2C模式
	I2C_InitStructure.I2C_OwnAddress1 = 0;//STM32做从机自己I2C地址
	I2C_Init(I2C1, &I2C_InitStructure);//把上面配置的I2C参数给I2C1控制器
	
	I2C_Cmd(I2C1, ENABLE);//配置完后一定不要忘记打开I2C1功能
}

/*写1个字节到EEPROM芯片*/
/*
* pBuffer发送给EEPROM数据
* WriteAddr要把数据写入EEPROM里面哪一个地址上
*/
void sendByteEEPROM(u8* pBuffer,u8 WriteAddr)
{
	I2C_GenerateSTART(I2C1, ENABLE);//发送起始信号
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//检测EV5事件
	
	I2C_Send7bitAddress(I2C1,0,I2C_Direction_Transmitter);//发送从器件地址，器件地址是0x00
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//检测EV6事件
	
	I2C_SendData(I2C1, WriteAddr);//向EEPROM写地址
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件
	
	I2C_SendData(I2C1, *pBuffer);//发送要写给EEPROM的数据
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8事件
	
	I2C_GenerateSTOP(I2C1, ENABLE);//发送停止信号
	
}

/*读取EEPROM数据*/
//读取的eeprom数据放在pBuffer
//要读取eeprom那个地址上的数据，用ReadAddr指定地址
void I2C_EEPROM_ByteRead(uint8_t *pBuffer, uint8_t ReadAddr)
{  
	//I2C_EE_WaitEepromStandbyState();//操作EEPROM的话这里还要执行等待EEPROM准备好的函数
	//操作其它IIC器件应该不需要等待函数
	
	I2C_GenerateSTART(I2C1, ENABLE);//发送起始信号
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//检测EV5
	
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Transmitter);//器件地址是0x00	
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//检测EV6
	
	I2C_SendData(I2C1, ReadAddr);//要读取那个地址上的数据，发送读取的地址
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//检测EV8
	
	I2C_GenerateSTART(I2C1, ENABLE);//从新发送start信号
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//检测EV5
	
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Receiver);//器件地址是0x00
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//检测EV6
	
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); //检测EV7
	
	*pBuffer = I2C_ReceiveData(I2C1);//读取从机地址上的数据
	
	I2C_AcknowledgeConfig(I2C1, DISABLE);//STM32主机发送非应答(不需要从机应答)信号    
	I2C_GenerateSTOP(I2C1, ENABLE);//产生停止信号
}


int main(void)
{
	   	  	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	
	I2C_GPIO_INIT();//初始化IIC管脚
	I2C_mode_config();//初始IIC模式
	
	//sendByteEEPROM(u8* pBuffer,u8 WriteAddr);//发数据到EEPROM
	// I2C_EEPROM_ByteRead(uint8_t *pBuffer, uint8_t ReadAddr)读取EEPROM数据
	while(1)
	{
		printf("xxxzzzzzz\r\n");
		
	}
	return 0;
	
}





