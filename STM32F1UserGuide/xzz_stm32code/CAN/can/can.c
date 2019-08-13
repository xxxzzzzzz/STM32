#include "can.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"


/*CAN���ų�ʼ��*/
void CAN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);//��ʼ��CAN1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	//��PA11,PA12ʱ�Ӻ͸���ʱ��AFIO��CAN1�����õ�PA11,PA12����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PA12 CAN TX 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//CAN TX��������Ҫ���ø���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA11 CAN RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//CAN RX�������ſ������ó���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

/*CAN�����ʺ͹�������*/
void CAN_Config(void)
{
	CAN_InitTypeDef CAN_InitTypeStruct;
	
	CAN_InitTypeStruct.CAN_ABOM = ENABLE;
	CAN_InitTypeStruct.CAN_AWUM = ENABLE;//�Զ����ѹ��ܴ�
	
	/*������1M����BS1,BS2,prescaler,SJW*/
	CAN_InitTypeStruct.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitTypeStruct.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitTypeStruct.CAN_Prescaler = 6;
	CAN_InitTypeStruct.CAN_SJW = CAN_SJW_1tq;
	
	CAN_InitTypeStruct.CAN_Mode = CAN_Mode_Normal;//����CANΪ��׼�շ�ģʽ
	CAN_InitTypeStruct.CAN_NART = ENABLE;//֧�ֱ����ش�
	CAN_InitTypeStruct.CAN_TTCM = DISABLE;//�����������CAN2.0�ò���
	CAN_InitTypeStruct.CAN_TXFP = DISABLE;//���ձ���ID���ȼ���ѡ���ȷ��
	CAN_InitTypeStruct.CAN_RFLM = ENABLE;//����fifo����
	
	CAN_Init(CAN1,&CAN_InitTypeStruct);//������CAN1,��STM32F105������CAN2
}

/*CAN����������*/
void CAN_Filter(uint32_t ID)
{
	CAN_FilterInitTypeDef CAN_FilterTypeStruct;
	
	CAN_FilterTypeStruct.CAN_FilterActivation = ENABLE;//�Ƿ�����������,����������
	CAN_FilterTypeStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//���˳���ҪID�����ݾͷ���fifo0
	CAN_FilterTypeStruct.CAN_FilterNumber = 0;
	CAN_FilterTypeStruct.CAN_FilterScale = CAN_FilterScale_32bit;//��32λ������
	CAN_FilterTypeStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	
	CAN_FilterTypeStruct.CAN_FilterIdHigh = (((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF0000)>>16;
	CAN_FilterTypeStruct.CAN_FilterIdLow =  ((uint32_t)(ID<<3|CAN_Id_Extended|CAN_RTR_Data))&0xFFFF;
	
	CAN_FilterTypeStruct.CAN_FilterMaskIdHigh = 0xffff;
	CAN_FilterTypeStruct.CAN_FilterMaskIdLow = 0xffff;
	
	CAN_FilterInit(&CAN_FilterTypeStruct);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//��CAN1�����ж�ʹ��,FMP0���ǽ�������0�����ж�
}

/*CAN�����ж�����*/
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//����CAN�ж����ȼ��ṹ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//�жϷ��䵽0��
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;//CAN1��������0�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�(�����ȼ�)Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�ж����ȼ�����
	NVIC_Init(&NVIC_InitStructure);//��ʼ���ж����ȼ�
	
	
}

