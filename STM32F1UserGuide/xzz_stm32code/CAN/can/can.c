#include "can.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"


/*CAN引脚初始化*/
void CAN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);//初始化CAN1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	//打开PA11,PA12时钟和复用时钟AFIO，CAN1我们用的PA11,PA12引脚
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PA12 CAN TX 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//CAN TX发送引脚要设置复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA11 CAN RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//CAN RX接受引脚可以设置成上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

/*CAN波特率和功能设置*/
void CAN_Config(void)
{
	CAN_InitTypeDef CAN_InitTypeStruct;
	
	CAN_InitTypeStruct.CAN_ABOM = ENABLE;
	CAN_InitTypeStruct.CAN_AWUM = ENABLE;//自动唤醒功能打开
	
	/*波特率1M设置BS1,BS2,prescaler,SJW*/
	CAN_InitTypeStruct.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitTypeStruct.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitTypeStruct.CAN_Prescaler = 6;
	CAN_InitTypeStruct.CAN_SJW = CAN_SJW_1tq;
	
	CAN_InitTypeStruct.CAN_Mode = CAN_Mode_Normal;//设置CAN为标准收发模式
	CAN_InitTypeStruct.CAN_NART = ENABLE;//支持报文重传
	CAN_InitTypeStruct.CAN_TTCM = DISABLE;//这个功能我们CAN2.0用不上
	CAN_InitTypeStruct.CAN_TXFP = DISABLE;//按照报文ID优先级来选择先发�
	CAN_InitTypeStruct.CAN_RFLM = ENABLE;//锁定fifo功能
	
	CAN_Init(CAN1,&CAN_InitTypeStruct);//我们用CAN1,在STM32F105里面有CAN2
}

/*CAN过滤器配置*/
void CAN_Filter(uint32_t ID)
{
	CAN_FilterInitTypeDef CAN_FilterTypeStruct;
	
	CAN_FilterTypeStruct.CAN_FilterActivation = ENABLE;//是否启动过滤器,我这里启动
	CAN_FilterTypeStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤出需要ID的数据就放在fifo0
	CAN_FilterTypeStruct.CAN_FilterNumber = 0;
	CAN_FilterTypeStruct.CAN_FilterScale = CAN_FilterScale_32bit;//用32位过滤器
	CAN_FilterTypeStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	
	CAN_FilterTypeStruct.CAN_FilterIdHigh = (((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF0000)>>16;
	CAN_FilterTypeStruct.CAN_FilterIdLow =  ((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF;
	
	CAN_FilterTypeStruct.CAN_FilterMaskIdHigh = 0xffff;
	CAN_FilterTypeStruct.CAN_FilterMaskIdLow = 0xffff;
	
	CAN_FilterInit(&CAN_FilterTypeStruct);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//打开CAN1接受中断使能,FMP0就是接受邮箱0产生中断
}

/*CAN接受中断配置*/
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//定义CAN中断优先级结构体
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//中断分配到0组
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;//CAN1接受邮箱0中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//响应优先级(子优先级)为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断优先级开启
	NVIC_Init(&NVIC_InitStructure);//初始化中断优先级
	
	
}

